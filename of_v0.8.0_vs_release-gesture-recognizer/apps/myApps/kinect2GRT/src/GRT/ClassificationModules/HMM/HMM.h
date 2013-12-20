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

 NOTE: This is still under development and needs to be properly integrated into the GRT framework!
 */

#ifndef GRT_HMM_HEADER
#define GRT_HMM_HEADER

#include "HiddenMarkovModel.h"
#include "../../GestureRecognitionPipeline/Classifier.h"

namespace GRT{

class HMM : public Classifier
{
public:
	HMM(UINT numStates=5,UINT numSymbols=10,UINT modelType=HiddenMarkovModel::LEFTRIGHT,UINT delta=1,UINT maxNumIter=100,double minImprovement=1.0e-2);
    
    HMM(const HMM &rhs);
    
	virtual ~HMM(void);
    
    HMM& operator=(const HMM &rhs);
    
    /**
     This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.  
     It clones the data from the Base Class Classifier pointer (which should be pointing to an ANBC instance) into this instance
     
     @param Classifier *classifier: a pointer to the Classifier Base Class, this should be pointing to another ANBC instance
     @return returns true if the clone was successfull, false otherwise
     */
    virtual bool clone(const Classifier *classifier);
    
    /**
     This trains the ANBC model, using the labelled classification data.
     This overrides the train function in the Classifier base class.
     
     @param LabelledClassificationData &trainingData: a reference to the training data
     @return returns true if the ANBC model was trained, false otherwise
     */
    virtual bool train(LabelledClassificationData &trainingData);
    
    virtual bool train(LabelledTimeSeriesClassificationData &trainingData);
    
    /**
     This predicts the class of the inputVector.
     This overrides the predict function in the Classifier base class.
     
     @param VectorDouble inputVector: the input vector to classify
     @return returns true if the prediction was performed, false otherwise
     */
    virtual bool predict(VectorDouble inputVector);
    
    /**
     This saves the trained ANBC model to a file.
     This overrides the saveModelToFile function in the Classifier base class.
     
     @param string filename: the name of the file to save the ANBC model to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(string filename){ return false; }
    
    /**
     This saves the trained ANBC model to a file.
     This overrides the saveModelToFile function in the Classifier base class.
     
     @param fstream &file: a reference to the file the ANBC model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file){ return false; }
    
    /**
     This loads a trained ANBC model from a file.
     This overrides the loadModelFromFile function in the Classifier base class.
     
     @param string filename: the name of the file to load the ANBC model from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(string filename){ return false; }
    
    /**
     This loads a trained ANBC model from a file.
     This overrides the loadModelFromFile function in the Classifier base class.
     
     @param fstream &file: a reference to the file the ANBC model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file){ return false; }
    
    bool convertDataToObservationSequence( LabelledTimeSeriesClassificationData &classData, vector< vector< UINT > > &observationSequences );

	vector< HiddenMarkovModel > models;

	//Variables for all the HMMs
	UINT numStates;			//The number of states for each model
	UINT numSymbols;		//The number of symbols for each model
	UINT modelType;         //Set if the model is ERGODIC or LEFTRIGHT
	UINT delta;				//The number of states a model can move to in a LeftRight model
	UINT maxNumIter;		//The maximum number of iter allowed during the full training
	double minImprovement;  //The minimum improvement value for each model during training
    
    static RegisterClassifierModule< HMM > registerModule;
};
    
}//End of namespace GRT

#endif //GRT_HMM_HEADER
