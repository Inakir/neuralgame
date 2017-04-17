// neural-net-tutorial.cpp
// David Miller, http://millermattson.com/dave
// See the associated video for instructions: http://vimeo.com/19569529


#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>

#include "Net.h"

using namespace std;

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

	topology = { 2, 4, 1 };
	Net nnet(topology);

	vector<double> inputs, targets, results;
	int trainingPass = 0;

	vector<double> input_vals, target_vals;
	input_vals =
	{ 1.0, 0.0,
		1.0, 1.0,
		1.0, 0.0,
		0.0, 1.0,
		0.0, 1.0,
		0.0, 1.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0,
		0.0, 1.0,
		0.0, 0.0,
		0.0, 0.0,
		1.0, 0.0 };
	target_vals =
	{ 1.0,
		0.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		0.0,
		0.0,
		1.0,
		0.0,
		1.0,
		0.0,
		0.0,
		1.0 };
	int i = 0;
	while (trainingPass<2000)
	{
		if (i >= target_vals.size())
		{
			i = 0;
		}
		++trainingPass;
		cout << endl << "Pass " << trainingPass;

		inputs.clear();
		targets.clear();

		inputs.push_back(input_vals[i * 2]);
		inputs.push_back(input_vals[i * 2 + 1]);
		targets.push_back(target_vals[i]);

		showVectorVals(": Inputs: ", inputs);

		nnet.feedForward(inputs);

		//collect the net's actual results
		nnet.getResults(results);
		showVectorVals(": Outputs: ", results);

		//train the net on what outputs should have been
		showVectorVals(": Targets: ", targets);
		//assert(targets.size() == topology.back());

		nnet.backProp(targets);

		//report how well the training is working
		cout << "Net recent average error: " << nnet.getRecentAverageError() << endl;
		++i;
	}

	cout << endl << "Done" << endl;

	//inputs = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};

	int temp;
	cin >> temp;
}
