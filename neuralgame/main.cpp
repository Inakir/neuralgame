//Inaki Rosa
//CSCE 420-500
//Final Project
//Due 4/25/2017
//main.cpp
//This is the main page and handles the entirety of the game except the neural network stuff. 

#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>

//All the FLTK stuff
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>

#include "Net.h"

using namespace std;

Fl_Window window(800, 400, "Guessing game");	//fltk window for the game

vector<unsigned> topology = { 32,16,8,4,2,1 };	//Each value stands for how many neurons make up a layer. 
														//Each layer is connected in the order that they are stored
														//So the first and last values stand for the input and outputs 
														//layers respectively, with all the values in between
														//standing for hidden layers
Net nnet(topology);								//Initializes the net based on the given topology

vector<double> inputs, targets, results;		//These are the vectors that are used to feed the net
vector<double> user_inputs, AI_guesses;			//These hold all the values inputted by the user and all the guesses done by the AI

int trainSamples = 500;							//Sets how many samples are taken at teach points

string instr;									//All the following strings make up the message displayed by the window
string user_training = "\nTraining Choices: ";
string user_choices = "\nYour Choices: ";
string AI_choices = "\nAI Guesses:   ";

double avg_blue = 0;				//These values are used by the AI to determine each guess that is made
double avg_red = 0;					//They hold the averages of red and blue choices as well as the total amount of red
double bluesz = .000001;			// and blue choices are made, respectively, They are a decimial because they are used to divide
double redsz = .000001;				// and they can't be zero since there might not always be a red or blue choice

double percent;						//These values are used at the end to determine how many correct guesses were made
int correct_guesses;
int score = 0; 

//0-31 32-63 64-95
//void that ends the game and sets the information for the last window
void endGame()
{
	correct_guesses = 0;
	user_training = "\nTraining Choices: ";
	user_choices = "\nYour Choices: ";
	AI_choices = "\nAI Guesses:   ";
	//Fills in the training string for indexes 0-63
	for (int i = 0; i <= 63; i++) {

		if (user_inputs[i] == 1.0)
		{
			user_training += "r";
		}
		else
		{
			user_training += "b";
		}
	}

	//Fills in the user choice string string for indexes 64-95
	for (int i = 64; i <= 95; i++) {

		if (user_inputs[i] == 1.0)
		{
			user_choices += "r";
		}
		else
		{
			user_choices += "b";
		}
	}
	
	//Fills in the AI string string for indexes 64-95 and calculates percentage correct
	for (int i = 0; i < 32; i++)
	{
		if (AI_guesses[i] == user_inputs[i + 64])
		{
			correct_guesses++;
		}
		if (AI_guesses[i]==1.0) 
		{
			AI_choices += "r";
		}
		else 
		{
			AI_choices += "b";
		}
	}
	percent = correct_guesses / 32.0;
}


//used for training the net, given an index uses that as the start of a string of inputs 
//and one output
void fillValues(int index)
{
	//keeps track of how many elements we've gone through
	int count = 0;

	//loops until 32 inputs are found
	while (count < 32)
	{
		//cout << index << " ";
		inputs.push_back(user_inputs[index]);
		count++;
		index++;
	}

	//after 32 inputs have been filled, the 33rd is the output
	targets.push_back(user_inputs[index]);
}

//prints out vectors with a predefined message
void showVectorVals(string label, vector<double> &v)
{
	cout << label << " ";
	for (unsigned i = 0; i < v.size(); ++i) {
		cout << v[i] << " ";
	}

	cout << endl;
}

//Case 1: First text draw
//Case 2: User inputs < 64
//Case 3: User inputs > 64, AI now guessing 
//Case 4: Game is done
void setText()
{
	//0-31 32-63 64-95
	if (user_inputs.size() == 0)
	{
		instr = "Instructions: ";
		instr += "\nThis is a guessing game. You are gonna press the buttons below 65 times trying to be unpredicatable. ";
		instr += "\nAfter that our AI will analyze your inputs. You are then going to press the buttons 32 times again, ";
		instr += "\n however this time the game will try and predict your choice. ";
		instr += "\nTheoretically we should only be able to predict 50% of your choices, we'll see how that goes ;). \n\n\n\n";
	}
	else if (user_inputs.size() < 64)
	{
		instr = "Choices left to go before the AI starts guessing: ";
		instr += to_string((64 - user_inputs.size()));
		instr += "\n";
		instr += user_training;
	}
	else if (user_inputs.size() <= 95)
	{
		instr = "The AI is now guessing what your inputs are \n\n";
		instr += "Choices left: ";
		instr += to_string((96 - user_inputs.size()));
		instr += "\n";
		instr += user_training;
		instr += user_choices;
		instr += AI_choices;
		if (user_inputs.size() == 64)
		{
			instr += "\n";
		}
		else if (user_inputs.back() == AI_guesses[AI_guesses.size()-2])
		{
			instr += "\nAI correct!\n";
			score++;
		}
		else //size == 96
		{
			instr += "\nAI wrong!\n";
		}
		instr += to_string(score);
		instr += "/32";
	}
	else
	{
		instr = "Game is over\n";
		instr += "Choices left: 0\n";
		instr += user_training;
		instr += user_choices;
		instr += AI_choices;
		instr += "\nPercent Guessed Correctly: ";
		instr += to_string(correct_guesses);
		instr += "/32 = ";
		instr += to_string(percent);
	}
	window.redraw();
}

//trains the neural net
//has outputs commented out for final product efficiency 
void train()
{
	int index = 0;		//filled in during analysis 
	avg_blue = 0;
	avg_red  = 0;
	bluesz = .000001;
	redsz  = .000001;

	for (int trainingPass = 0; trainingPass < trainSamples; ++trainingPass)
	{
		if (index >= user_inputs.size()-32)
		{
			index = 0;
		}

		if (trainingPass > trainSamples - 3) {
			//cout << endl << "Pass " << trainingPass;
		}

		inputs.clear();
		targets.clear();

		//this fills in the user inputs as well as the output based on what inedex we are on
		fillValues(index);		

		if (trainingPass > trainSamples - 3) {
			//showVectorVals(": Inputs: ", inputs);
		}

		//passes the inputs through
		nnet.feedForward(inputs);

		//collect the outputs from the net
		nnet.getResults(results);

		if (trainingPass > trainSamples - 3) {
			//showVectorVals(": Outputs: ", results);
		}

		if (targets[0] == 0.0 && trainingPass>trainSamples - 64) {
			avg_blue += results[0];
			bluesz++;
		}
		else if (targets[0] == 1.0 && trainingPass>trainSamples - 64) {
			avg_red += results[0];
			redsz++;
		}

		//Shows what the target output was this iteration (only does it for last three)
		if (trainingPass > trainSamples - 3) {
			//showVectorVals(": Targets: ", targets);
		}

		//train the net on what outputs should have been
		nnet.backProp(targets);

		//report how well the training is working (last three iterations)
		if (trainingPass > trainSamples - 3) {
			//cout << "Net recent average error: " << nnet.getRecentAverageError() << endl;
		}
	
		++index;
	}
}

//AI method. Trains the neural net and then makes a guess
void AI()
{
	//0-31 32-63 64-95
	if (user_inputs.size() == 64)
	{
		train(); train(); train(); train(); train(); train(); train();
	}
		
	//trains the net
	train();
		
	//uses previous statistics to determine how to guess red or blue
	avg_red /= redsz;
	avg_blue /= bluesz;
	double mid = (avg_red + avg_blue) / 2;
	bool correct = avg_red>avg_blue;
		
	//after training we're gonna go through and guess
	inputs.clear();
	targets.clear();
		
	//this fills in the user inputs
	fillValues(user_inputs.size() - 33);		
		
	//feeds the inputs to the net
	nnet.feedForward(inputs);
		
	//collect the net's actual results
	nnet.getResults(results);
		
	//This next line is for debugging purposes
	//cout << correct << " " << avg_red << " " << avg_blue << endl;
		
	// Folowing ifs decide the guess
	if (results[0] < mid) 
	{
		if (correct) 
		{
			AI_guesses.push_back(0.0);
			//AI_choices += "b";
			AI_choices += "x";
		}
		else 
		{
			AI_guesses.push_back(1.0);
			//AI_choices += "r";
			AI_choices += "x";
		}
	}
	else {
		if (correct) {
			AI_guesses.push_back(1.0);
			//AI_choices += "r";
			AI_choices += "x";
		}
		else {
			AI_guesses.push_back(0.0);
			//AI_choices += "b";
			AI_choices += "x";
		}
	}
}


//Activated when the red button is pressed
static void redPress(Fl_Widget *w, void* data) 
{
	//fills in the user inputs. After 64 values have been inputted the AI starts guessing. The AI retrains itself before guessing. This is why the call to the fuction AI comes
	//after the previous input, because it'd be cheating if it trained itself with the results already in the list so it doesn't add that value to the list until after the guess.
	if (user_inputs.size() < 64) 
	{ 
		user_inputs.push_back(1.0);
		//user_training += "r";
		user_training += "x";
		if (user_inputs.size() == 63)
		{
			AI();
		}
	}
	else 
	{ 
		//user_choices += "r";
		user_choices  += "x";
		user_inputs.push_back(1.0);
		if (user_inputs.size() < 96)
		{
			AI();
		}
		else //After 96 inputs the game ends (33 inputs for training phase, then 32 more for actual testing
		{
			endGame();
		}
	}
	setText();
}

//Activated when the red button is pressed
static void bluePress(Fl_Widget *w, void* data) 
{
	//fills in the user inputs. After 64 values have been inputted the AI starts guessing. The AI retrains itself before guessing. This is why the call to the fuction AI comes
	//after the previous input, because it'd be cheating if it trained itself with the results already in the list so it doesn't add that value to the list until after the guess.
	if (user_inputs.size() < 64)
	{ 
		user_inputs.push_back(0.0);
		//user_training += "b";
		user_training += "x";
		if (user_inputs.size() == 63)
		{
			AI();
		}
	}
	else
	{
		//user_choices += "b";
		user_choices  += "x";
		user_inputs.push_back(0.0);
		if(user_inputs.size() < 96)
		{
			AI();
		}
		else //After 96 inputs the game ends (33 inputs for training phase, then 32 more for actual testing
		{
			endGame();
		}
	}
	setText();
}

int main()
{
	//FLTK Window init
	
	//Box Init
	setText();
	Fl_Box box(50, 100, 700, 20, instr.c_str());
	
	//Buttons
	Fl_Button redB(0, 200, 400, 100, "Red");
	redB.color(fl_rgb_color(255, 0, 0));
	redB.callback(redPress, &user_inputs);
	
	Fl_Button blueB(400, 200, 400, 100, "Blue");
	blueB.color(fl_rgb_color(0, 0, 255));
	blueB.callback(bluePress, &user_inputs);
	
	window.show();
	window.redraw();
	
	return Fl::run();
}