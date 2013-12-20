/*
 GRT MIT License
 Copyright (c) <2012> <Nicholas Gillian, Media Lab, MIT>
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 and associated documentation files (the "Software"), to deal in the Software without restriction, 
 including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial 
 portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
 LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef GRT_HIDDEN_MARKOV_MODEL_HEADER
#define GRT_HIDDEN_MARKOV_MODEL_HEADER

#include "../../Util/GRTCommon.h"

namespace GRT {

//This class is used for the HMM batch training
class HMMTrainingObject{
public:
	HMMTrainingObject(){
		pk = 0.0;
	}
	~HMMTrainingObject(){}
	MatrixDouble alpha;     //The forward estimate matrix
	MatrixDouble beta;      //The backward estimate matrix
	VectorDouble c;         //The scaling coefficient vector
	double pk;				//P( O | Model )
};

class HiddenMarkovModel{

public:
	HiddenMarkovModel();
	~HiddenMarkovModel();

	HiddenMarkovModel(UINT numStates,UINT numSymbols,UINT modelType,UINT delta);
	HiddenMarkovModel(const MatrixDouble &a,const MatrixDouble &b,const VectorDouble &pi,UINT modelType,UINT delta);
    
    bool resetModel(UINT numStates,UINT numSymbols,UINT modelType,UINT delta);
    bool train(const vector< vector<UINT> > &trainingData);
    double predict(UINT newSample);
    double predict(const vector<UINT> &obs);
    VectorDouble getTrainingLog();

public:
    bool randomizeMatrices(UINT numStates,UINT numSymbols);
	double predictLogLikelihood(vector<UINT> &obs);
	bool forwardBackward(HMMTrainingObject &trainingObject,const vector<UINT> &obs);
    bool train_(const vector< vector<UINT> > &obs,UINT maxIter, UINT &currentIter,double &newLoglikelihood);
    void printAB();
    
	UINT numStates;             //The number of states for this model
	UINT numSymbols;            //The number of symbols for this model
	MatrixDouble a;             //The transitions probability matrix
	MatrixDouble b;             //The emissions probability matrix
	VectorDouble pi;            //The state start probability vector
    VectorDouble trainingLog;   //Stores the loglikelihood at each iteration the BaumWelch algorithm

	UINT modelType;
	UINT delta;				//The number of states a model can move to in a LeftRight model
	UINT numTestSpawns;		//The number of test spawns
	UINT maxNumTestIter;	//The maximum number of iter allowed during a test spawn
	UINT maxNumIter;		//The maximum number of iter allowed during the full training
	bool modelTrained;
	double logLikelihood;	//The log likelihood of an observation sequence given the modal, calculated by the forward method
	double cThreshold;		//The classification threshold for this model
	double minImprovement;	//The minimum improvement value for the training loop
    CircularBuffer<UINT> observationSequence;
    vector< UINT > estimatedStates;

	enum HMMModelTypes{ERGODIC=0,LEFTRIGHT=1};
    
    DebugLog debugLog;
    ErrorLog errorLog;
    WarningLog warningLog;
    
};

}//end of namespace GRT

#endif //GRT_HIDDEN_MARKOV_MODEL_HEADER
