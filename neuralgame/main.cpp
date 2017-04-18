#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>

#include "Net.h"

using namespace std;

void fillValues(vector<double> &inputs, vector<double> &targets, vector<double> &user_inputs, int index)
{
	int count = 0;
	while (count < 32)
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

int main()
{
	cout << "Enter 64 inputs of either 1 or 0. " << endl;

	vector<unsigned> topology;

	topology = { 32, 4, 4, 4, 4, 4, 4, 4, 4, 2, 1 };
	Net nnet(topology);

	vector<double> inputs, targets, results;
	int trainingPass = 0;
	int trainSamples = 2000;
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
	cin >> temp;
}
