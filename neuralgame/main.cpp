#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>

#include "Net.h"

using namespace std;

Fl_Window window(800, 400, "Guessing game");
vector<unsigned> topology = { 32,16,16,8,4,4,4,2,1 };
Net nnet(topology);
vector<double> inputs, targets, results;
vector<double> user_inputs, AI_guesses;
int trainSamples = 500;
string instr;
string user_training = "\nTraining Choices: ";
string user_choices = "\nYour Choices: ";
string AI_choices = "\nAI Guesses:   ";
double percent;
int correct_guesses;
double avg_blue = 0;
double avg_red = 0;
double bluesz = .000001;
double redsz = .000001;

void endGame()
{
	correct_guesses = 0;
	user_training = "\nTraining Choices: ";
	user_choices = "\nYour Choices: ";
	AI_choices = "\nAI Guesses:   ";

	for (int i = 0; i < 32; i++) {

		if (user_inputs[i] == 1.0)
		{
			user_training += "r";
		}
		else
		{
			user_training += "b";
		}
	}

	for (int i = 1; i < 65; i++) {

		if (user_inputs[i] == 1.0)
		{
			user_choices += "r";
		}
		else
		{
			user_choices += "b";
		}
	}
	
	for (int i = 0; i < 32; i++)
	{
		if (AI_guesses[i] == user_inputs[i + 65])
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

void fillValues(int index)
{
	int count = 0;

	//loops until 32 inputs are found
	while (count < 32)
	{
		//cout << index << " ";
		inputs.push_back(user_inputs[index]);
		count++;
		index++;
	}

	//cout << index << " ";
	//after 32 inputs have been filled, the 33rd is the output
	targets.push_back(user_inputs[index]);
	//cout << endl;
}

void showVectorVals(string label, vector<double> &v)
{
	cout << label << " ";
	for (unsigned i = 0; i < v.size(); ++i) {
		cout << v[i] << " ";
	}

	cout << endl;
}

//t = 0: First text draw
//t = 1: User inputs < 64
//t = 2: User inputs > 64, AI now guessing 
//t > 2: Game is done
void setText(int i)
{
	if (i == 0)
	{
		instr = "Instructions: ";
		instr += "\nThis is a guessing game. You are gonna press the buttons below 65 times trying to be unpredicatable. ";
		instr += "\nAfter that our AI will analyze your inputs. You are then going to press the buttons 32 times again, ";
		instr += "\n however this time the game will try and predict your choice. ";
		instr += "\nTheoretically we should only be able to predict 50% of your choices, we'll see how that goes ;). \n\n\n\n";
	}
	else if (i == 1)
	{
		instr = "Choices left to go before the AI starts guessing: ";
		instr += to_string((65 - user_inputs.size()));
		instr += "\n";
		instr += user_training;
	}
	else if (i == 2)
	{
		instr = "The AI is now guessing what your inputs are \n\n";
		instr += "Choices left: ";
		instr += to_string((97 - user_inputs.size()));
		instr += "\n";
		instr += user_training;
		instr += user_choices;
		instr += AI_choices;
	}
	else
	{
		instr = "Game is over";
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

void train()
{
	int index = 0;
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

void AI()
{
	if (user_inputs.size() >= 97)	//After 65 inputs the game ends (33 inputs for training phase, then 32 more for actual testing
	{
		endGame();
		setText(3);
	}
	else if (user_inputs.size() > 64)	//Goes on here after 64 inputs and starts guessing
	{
		if (user_inputs.size() == 65)
		{
			train(); train(); train(); train(); train(); train(); train();
		}
		setText(2);
		
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
	else if (user_inputs.size() > 32)	//Goes on here after 33 inputs have been recorded
	{
		setText(1);
		//train();
	}
	else
	{
		setText(1);
	}
}

static void redPress(Fl_Widget *w, void* data) 
{
	if (user_inputs.size() <= 64) 
	{ 
		user_inputs.push_back(1.0);
		//user_training += "r";
		user_training += "x";
		AI();
	}
	else						  
	{ 
		//user_choices += "r";
		user_choices  += "x";
		AI();
		user_inputs.push_back(1.0);
	}
}

static void bluePress(Fl_Widget *w, void* data) 
{
	if (user_inputs.size() <= 64) 
	{ 
		user_inputs.push_back(0.0);
		//user_training += "b";
		user_training += "x";
		AI();
	}
	else						  
	{
		//user_choices += "b";
		user_choices  += "x";
		AI();
		user_inputs.push_back(0.0);
	}
}

int main()
{
	//FLTK Window init
	
	//Box Init
	setText(0);
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