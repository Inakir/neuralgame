//Inaki Rosa
//CSCE 420-500
//Final Project
//Due 4/25/2017
//Neuron.h
//Header file for neauron class and contains connection struct. This was mostly taken from the Neural network youtube video
//For more info about the Neural Network check the readme

#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>

using namespace std;

class Neuron;
typedef vector<Neuron> Layer;

struct Connection
{
	double weight;
	double deltaWeight;
};

class Neuron
{
public:
	Neuron(unsigned numOutputs, unsigned myIndex);			//Neuron Constructor
	void setOutputVal(double val) { m_outputVal = val; }	//sets output 
	double getOutputVal(void) const { return m_outputVal; }	//gets output
	void feedForward(const Layer &prevLayer);				//used to give it inputs
	void calcOutputGradients(double targetVal);				//gets the gradients of the outputs
	void calcHiddenGradients(const Layer &nextLayer);		//gets the gradients of the hidden layers
	void updateInputWeights(Layer &prevLayer);				//changes the weights

private:
	static double eta;   // [0.0..1.0] overall net training rate
	static double alpha; // [0.0..n] multiplier of last weight change (momentum)
	static double transferFunction(double x);
	static double transferFunctionDerivative(double x);
	static double randomWeight(void) { return rand() / double(RAND_MAX); }
	double sumDOW(const Layer &nextLayer) const;
	double m_outputVal;
	vector<Connection> m_outputWeights;
	unsigned m_myIndex;
	double m_gradient;
};
