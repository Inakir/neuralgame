#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>

#include "Net.h"

using namespace std;

void fillValues(vector<double> &inputs, vector<double> &targets, vector<double> &user_inputs, int index)
{
	int count = 0;
	while (count < 4)
	{
		if (index >= user_inputs.size())
		{
			index = 0;
		}
		inputs.push_back(user_inputs[index]);
		count++;
		index++;
	}

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

int drawWindow()
{
	return 0;
}

void redPress()
{

}

static void redPress(Fl_Widget *w, void* data) {
	Fl_Button *b = (Fl_Button*)w;
	fprintf(stderr, "Button '%s' was %s\n", b->label(), b->value() ? "Pushed" : "Released");
}

static void bluePress(Fl_Widget *w, void* data) {
	Fl_Button *b = (Fl_Button*)w;
	fprintf(stderr, "Button '%s' was %s\n", b->label(), b->value() ? "Pushed" : "Released");
}

int main()
{
	Fl_Window window(800, 400, "Guessing game");
	string instr = "Instructions: ";
	instr +=       "\nThis is a guessing game. You are gonna press the buttons below 32 times trying to be unpredicatable. ";
	instr +=       "\nAfter that our AI will analyze your inputs. You are then going to press the buttons 32 times again, ";
	instr +=       "\n however this time the game will try and predict your choice. ";
	instr +=       "\nTheoretically we should only be able to predict 50% of your choices, we'll see how that goes ;). ";
	Fl_Box box(50, 50, 700, 20, instr.c_str());
	Fl_Button redB(0, 200, 400, 100, "Red");
	redB.color(fl_rgb_color(255, 0, 0));
	redB.callback(redPress);
	Fl_Button blueB(400, 200, 400, 100, "Blue");
	blueB.color(fl_rgb_color(0, 0, 255));
	blueB.callback(bluePress);
	window.show();

	vector<unsigned> topology;

	topology = { 4,4,1 };
	Net nnet(topology);

	vector<double> inputs, targets, results;
	int trainingPass = 0;
	int trainSamples = 20000;
	//these are the actual values that need to be filled in
	vector<double> user_inputs = { 1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0 };

	int index = 0;
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

		fillValues(inputs, targets, user_inputs, index);		//this fills in the user inputs

		if (trainingPass > trainSamples - 32)
			showVectorVals(": Inputs: ", inputs);

		nnet.feedForward(inputs);

		//collect the net's actual results
		nnet.getResults(results);
		if (trainingPass > trainSamples - 32)
			showVectorVals(": Outputs: ", results);

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

	cout << endl << "Done" << endl;

	int temp;
	return Fl::run();
	cin >> temp;
}
