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
vector<unsigned> topology = { 32,8,8,8,8,4,2,1 };
Net nnet(topology);
vector<double> inputs, targets, results;
vector<double> user_inputs, AI_guesses;
int trainSamples = 2000;
string instr;
string user_training = "\nTraining Choices: ";
string user_choices = "\nYour Choices: ";
string AI_choices = "\nAI Guesses:   ";
double percent;

void endGame()
{
	double correct_guesses = 0.0;
	for (int i = 0; i < 32; i++)
	{
		if (AI_guesses[i] == user_inputs[i + 33])
			correct_guesses++;
	}
	percent = correct_guesses / 32.0;
}

void fillValues(int index)
{
	int count = 0;
	while (count < 32)
	{
		// only useful if the game loops around, which it does not but who knows, maybe future iterations
		if (index >= user_inputs.size())
		{
			index = 0;
		}

		inputs.push_back(user_inputs[index]);
		count++;
		index++;
	}

	// only useful if the game loops around, which it does not but who knows, maybe future iterations
	if (index >= user_inputs.size())
	{
		index = 0;
	}
	targets.push_back(user_inputs[index]);
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
//t = 1: User inputs < 32
//t = 2: User inputs > 32, AI now guessing analyzing 
//t > 2: Game is done
void setText(int i)
{
	if (i == 0)
	{
		instr = "Instructions: ";
		instr += "\nThis is a guessing game. You are gonna press the buttons below 32 times trying to be unpredicatable. ";
		instr += "\nAfter that our AI will analyze your inputs. You are then going to press the buttons 32 times again, ";
		instr += "\n however this time the game will try and predict your choice. ";
		instr += "\nTheoretically we should only be able to predict 50% of your choices, we'll see how that goes ;). \n\n\n\n";
	}
	else if (i == 1)
	{
		instr = "Instructions: ";
		instr += "\nThis is a guessing game. You are gonna press the buttons below 32 times trying to be unpredicatable. ";
		instr += "\nAfter that our AI will analyze your inputs. You are then going to press the buttons 32 times again, ";
		instr += "\n however this time the game will try and predict your choice. ";
		instr += "\nTheoretically we should only be able to predict 50% of your choices, we'll see how that goes ;). \n\n";
		instr += "Choices left to go before the AI starts guessing: ";
		instr += to_string((33 - user_inputs.size()));
		instr += "\n";
		instr += user_training;
	}
	else if (i == 2)
	{
		instr = "Instructions: ";
		instr += "\nSorry running out of space will fix later \n\n";
		instr += "Choices left: ";
		instr += to_string((65 - user_inputs.size()));
		instr += "\n";
		instr += user_training;
		instr += user_choices;
		instr += AI_choices;
	}
	else
	{
		instr = "Game is over";
		instr += user_training;
		instr += user_choices;
		instr += AI_choices;
		instr += "\nPercent Guessed Correctly: ";
		instr += to_string(percent);
	}
	window.redraw();
}

void AI()
{
	if (user_inputs.size() >= 65)	//After 65 inputs the game ends (33 inputs for training phase, then 32 more for actual testing
	{
		endGame();
		setText(3);
	}
	else if (user_inputs.size() > 32)	//Goes on here after 33 inputs have been recorded
	{
		setText(2);

		int index = 0;
		int trainingPass = 0;
		double avg_blue = 0;
		double avg_red = 0;
		double bluesz = .000001;
		double redsz = .000001;
		while (trainingPass<trainSamples)
		{
			if (index >= user_inputs.size())
			{
				index = 0;
			}
			++trainingPass;

			if (trainingPass > trainSamples - 32)
				cout << endl << "Pass " << trainingPass;

			inputs.clear();
			targets.clear();

			fillValues(index);		//this fills in the user inputs

			if (trainingPass > trainSamples - 32)
				showVectorVals(": Inputs: ", inputs);

			nnet.feedForward(inputs);

			//collect the net's actual results
			nnet.getResults(results);
			if (trainingPass > trainSamples - 32)
				showVectorVals(": Outputs: ", results);
			
			if (targets[0] == 0.0 && trainingPass>100) {
				avg_blue += results[0];
				bluesz++;
			}
			else if (targets[0] == 1.0 && trainingPass>100) {
				avg_red += results[0];
				redsz++;
			}

			//train the net on what outputs should have been
			if (trainingPass > trainSamples - 32)
				showVectorVals(": Targets: ", targets);
			//assert(targets.size() == topology.back());

			nnet.backProp(targets);
			//report how well the training is working
			if (trainingPass > trainSamples - 32)
				cout << "Net recent average error: " << nnet.getRecentAverageError() << endl;
			++index;
		}
		//
		avg_red /= redsz;
		avg_blue /= bluesz;
		double mid = (avg_red + avg_blue) / 2;
		bool correct = avg_red>avg_blue;	//sometimes the outputs get inverted 

		//
		inputs.clear();
		targets.clear();
		fillValues(user_inputs.size()-1);		//this fills in the user inputs
		nnet.feedForward(inputs);
		//collect the net's actual results
		nnet.getResults(results);
		nnet.backProp(targets);
		if (results[0] < .5) {
			if (correct) {
				AI_guesses.push_back(0.0);
				AI_choices += "b";
			}
			else {
				AI_guesses.push_back(1.0);
				AI_choices += "r";
			}
		}
		else {
			if (correct) {
				AI_guesses.push_back(1.0);
				AI_choices += "r";
			}
			else {
				AI_guesses.push_back(0.0);
				AI_choices += "b";
			}
		}
		////
	}
	else
	{
		setText(1);
	}
}

static void redPress(Fl_Widget *w, void* data) 
{
	user_inputs.push_back(1.0);
	if (user_inputs.size() <= 33) { user_training += "r"; }
	else						  { user_choices  += "r"; }
	AI();
}

static void bluePress(Fl_Widget *w, void* data) 
{
	user_inputs.push_back(0.0);
	if (user_inputs.size() <= 33) { user_training += "b"; }
	else						  { user_choices  += "b"; }
	AI();
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
	//delete input_disp;
	//delete text;
}