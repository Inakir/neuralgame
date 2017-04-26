//Inaki Rosa
//CSCE 420-500
//Final Project
//Due 4/25/2017
//Net.h
//This file defines the functions of the Neuron class. This was mostly taken from the Neural network youtube video
//For more info about the Neural Network check the readme

#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>

#include "Neuron.h"

using namespace std; 

class Net
{
public:
	Net(const vector<unsigned> &topology);			//constructor 
	void feedForward(const vector<double> &inputVals);		//feeds inputs to the Net
	void backProp(const vector<double> &targetVals);		//trains the net using the target values
	void getResults(vector<double> &resultVals) const;		//gets the results of the neural network after it has been fed forward
	double getRecentAverageError(void) const { return m_recentAverageError; }	//gets the recent error for the net, useful for debugging

private:
	vector<Layer> m_layers; // m_layers[layerNum][neuronNum]
	double m_error;				
	double m_recentAverageError;		
	static double m_recentAverageSmoothingFactor;
};
