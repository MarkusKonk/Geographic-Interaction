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

#include "HMM.h"

namespace GRT {

//Register the HMM with the classifier base type
RegisterClassifierModule< HMM > HMM::registerModule("HMM");
    
HMM::HMM(UINT numStates,UINT numSymbols,UINT modelType,UINT delta,UINT maxNumIter,double minImprovement)
{
    trained = false;
	numClasses = 0;
    numFeatures = 0;
	this->numStates = numStates;
	this->numSymbols = numSymbols;
	this->modelType = modelType; 
	this->delta = delta;
	this->maxNumIter = maxNumIter;
	this->minImprovement = minImprovement;
    useNullRejection = true;
    
    classifierMode = TIMESERIES_CLASSIFIER_MODE;
    classifierType = "HMM";
    debugLog.setProceedingText("[DEBUG HMM]");
    errorLog.setProceedingText("[ERROR HMM]");
    warningLog.setProceedingText("[WARNING HMM]");
}
    
HMM::HMM(const HMM &rhs){
    *this = rhs;
}

HMM::~HMM(void)
{
}

HMM& HMM::operator=(const HMM &rhs){
    if( this != &rhs ){
        this->numStates = rhs.numStates;
        this->numSymbols = rhs.numSymbols;
        this->modelType = rhs.modelType;
        this->delta = rhs.delta;
        this->maxNumIter = rhs.maxNumIter;
        this->minImprovement = rhs.minImprovement;
        this->models = rhs.models;
        
        copyBaseVariables( (Classifier*)&rhs );
    }
    return *this;
    
}
    
bool HMM::clone(const Classifier *classifier){
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        HMM *ptr = (HMM*)classifier;
        
        this->numStates = ptr->numStates;
        this->numSymbols = ptr->numSymbols;
        this->modelType = ptr->modelType;
        this->delta = ptr->delta;
        this->maxNumIter = ptr->maxNumIter;
        this->minImprovement = ptr->minImprovement;
        this->models = ptr->models;
        
        return copyBaseVariables( classifier );
    }
    return false;
}
    
bool HMM::train(LabelledClassificationData &trainingData){
    errorLog << "train(LabelledClassificationData &trainingData) - The HMM classifier should be trained using the train(LabelledTimeSeriesClassificationData &trainingData) method" << endl;
    return false;
}
    
    
bool HMM::train(LabelledTimeSeriesClassificationData &trainingData){
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << "train(LabelledTimeSeriesClassificationData &trainingData) - There are no training samples to train the HMM classifer!" << endl;
        return false;
    }
    
    if( trainingData.getNumDimensions() != 1 ){
        errorLog << "train(LabelledTimeSeriesClassificationData &trainingData) - The number of dimensions in the training data must be 1. If your training data is not 1 dimensional then you must quantize the training data using one of the GRT quantization algorithms" << endl;
        return false;
    }

	//Reset the HMM
    trained = false;
    useScaling = false;
    numFeatures = trainingData.getNumDimensions();
	numClasses = trainingData.getNumClasses();
	models.clear();
    classLabels.clear();
	models.resize( numClasses );
    classLabels.resize( numClasses );

	//Init the models
	for(UINT k=0; k<numClasses; k++){
		models[k].resetModel(numStates,numSymbols,modelType,delta);
		models[k].maxNumIter = maxNumIter;
		models[k].minImprovement = minImprovement;
	}
    
    //Train each of the models
    for(UINT k=0; k<numClasses; k++){
        //Get the class ID of this gesture
        UINT classID = trainingData.getClassTracker()[k].classLabel;
        classLabels[k] = classID;
        
        //Convert this classes training data into a list of observation sequences
        LabelledTimeSeriesClassificationData classData = trainingData.getClassData( classID );
        vector< vector< UINT > > observationSequences;
        if( !convertDataToObservationSequence( classData, observationSequences ) ){
            return false;
        }
        
        //Train the model
		if( !models[k].train( observationSequences ) ){
            errorLog << "train(LabelledTimeSeriesClassificationData &trainingData) - Failed to train HMM for class " << classID << endl;
            return false;
        }
	}
    
    //Compute the rejection thresholds
    nullRejectionThresholds.resize(numClasses);
    
    for(UINT k=0; k<numClasses; k++){
        //Get the class ID of this gesture
        UINT classID = trainingData.getClassTracker()[k].classLabel;
        classLabels[k] = classID;
        
        //Convert this classes training data into a list of observation sequences
        LabelledTimeSeriesClassificationData classData = trainingData.getClassData( classID );
        vector< vector< UINT > > observationSequences;
        if( !convertDataToObservationSequence( classData, observationSequences ) ){
            return false;
        }
        
        //Test the model
        double loglikelihood = 0;
        double avgLoglikelihood = 0;
        for(UINT i=0; i<observationSequences.size(); i++){
            loglikelihood = models[k].predict( observationSequences[i] );
            avgLoglikelihood += fabs( loglikelihood );
            cout << "Class: " << classID << " PredictedLogLikelihood: " << -loglikelihood << endl;
        }
        nullRejectionThresholds[k] = -( avgLoglikelihood / double( observationSequences.size() ) );
        cout << "Class: " << classID << " NullRejectionThreshold: " << nullRejectionThresholds[k] << endl;
	}
    
    for(UINT k=0; k<numClasses; k++){
        models[k].printAB();
    }
    
    trained = true;

	return true;
}
    
bool HMM::predict(VectorDouble inputVector){
    
    predictedClassLabel = 0;
	maxLikelihood = -10000;
    
    if( !trained ){
        errorLog << "predict(VectorDouble inputVector) - The HMM classifier has not been trained!" << endl;
        return false;
    }
    
	if( inputVector.size() != numFeatures ){
        errorLog << "predict(VectorDouble inputVector) - The size of the input vector (" << inputVector.size() << ") does not match the num features in the model (" << numFeatures << endl;
		return false;
	}
    
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses,0);
    
    double minDist = -99e+99;
    UINT bestIndex = 0;
    UINT newSample = (UINT)inputVector[0];
	for(UINT k=0; k<numClasses; k++){
		classDistances[k] = models[k].predict( newSample );
        
        //At this point the class likelihoods and class distances are the same thing
        classLikelihoods[k] = classDistances[k];
        
        //The loglikelihood values are negative so we want the values closest to 0
		if( classDistances[k] > minDist ){
			minDist = classDistances[k];
			bestIndex = k;
		}
        
        //printf("[%i] %f \t",k,classDistances[k]);
    }
    //printf("\n");
    
    maxLikelihood = minDist;
    predictedClassLabel = classLabels[bestIndex];
    
    if( useNullRejection ){
        if( maxLikelihood > nullRejectionThresholds[ bestIndex ] ){
            predictedClassLabel = classLabels[bestIndex];
        }else predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
    }
    
    return true;
}
    
    /*

UINT HMM::predict(vector<UINT> &obs,vector<double> &likelihoodResults){

	likelihoodResults.resize(numClasses);
	UINT bestIndex = 0;
	double bestResult = -1000;
	//Init the models
	for(UINT k=0; k<numClasses; k++){
		likelihoodResults[k] = hmmClassBuffer[k].predictLogLikelihood(obs);
		if(likelihoodResults[k]>bestResult){
			bestResult = likelihoodResults[k];
			bestIndex = k;
		}
	}

	//If the best result is greater than the threshold for that class then return the index + 1
	//cout<<"Best Index: "<<bestIndex<<" LogLikelihood: "<<predictionResults[bestIndex]<<" Threshold: "<<hmmClassBuffer[bestIndex].cThreshold<<endl;
	if(likelihoodResults[bestIndex]>=hmmClassBuffer[bestIndex].cThreshold){
		return bestIndex+1;
	}
	//Otherwise return 0
	return 0;
}

bool HMM::saveHMMModelsToFile(const char* fileName){

	std::fstream file; 
	file.open(fileName, std::ios::out);
	
	if(!file.is_open())
	{
		cout<<"HMM: Could not open file to save model\n";
		return false;
	}

	//Write the header info
	file<<"HMM_MODEL_FILE_V1.0\n";
	file<<"NumClasses: "<<numClasses<<endl;
	file<<"NumStates: "<<numStates<<endl;
	file<<"NumSymbols: "<<numSymbols<<endl;
	file<<"ModelType: "<<modelType<<endl;
	file<<"Delta: "<<delta<<endl;
	file<<endl;

	//Right each of the models
	for(UINT k=0; k<numClasses; k++){
		file<<"*************_MODEL_*************\n";
		file<<"Model_ID: "<<k+1<<endl;
		file<<"NumStates: "<<hmmClassBuffer[k].numStates<<endl;
		file<<"NumSymbols: "<<hmmClassBuffer[k].numSymbols<<endl;
		file<<"ModelType: "<<hmmClassBuffer[k].modelType<<endl;
		file<<"Delta: "<<hmmClassBuffer[k].delta<<endl;
		file<<"Threshold: "<<hmmClassBuffer[k].cThreshold<<endl;
		file<<"NumTestSpawns: "<<hmmClassBuffer[k].numTestSpawns<<endl;
		file<<"MaxNumTestIter: "<<hmmClassBuffer[k].maxNumTestIter<<endl;
		file<<"MaxNumIter: "<<hmmClassBuffer[k].maxNumIter<<endl;

		file<<"A:\n";
		for(UINT i=0; i<hmmClassBuffer[k].numStates; i++){
			for(UINT j=0; j<hmmClassBuffer[k].numStates; j++){
				file<<hmmClassBuffer[k].a[i][j]<<"\t";
			}file<<endl;
		}file<<endl;

		file<<"B:\n";
		for(UINT i=0; i<hmmClassBuffer[k].numStates; i++){
			for(UINT j=0; j<hmmClassBuffer[k].numSymbols; j++){
				file<<hmmClassBuffer[k].b[i][j]<<"\t";
			}file<<endl;
		}file<<endl;

		file<<"Pi:\n";
		for(UINT i=0; i<hmmClassBuffer[k].numStates; i++){
		 file<<hmmClassBuffer[k].pi[i]<<"\t";
		}file<<endl;

		file<<"*********************************\n";
	}

	file.close();

	return true;

}

bool HMM::loadHMMModelsFromFile(const char* fileName){

	std::fstream file; 
	file.open(fileName, std::ios::in);
	numClasses = 0;
	trained = false;
	numStates = 0;
	numSymbols = 0;
	modelType = 0; 
	delta = 0;
	maxNumIter = 100;
	hmmClassBuffer.clear();
	
	if(!file.is_open())
	{
		cout<<"HMM: Could not open file to load model\n";
		return false;
	}

	std::string word;

	//Find the file type header
	file >> word;
	if(word != "HMM_MODEL_FILE_V1.0"){
		cout<<"HMM_ERROR: Could not find Model File Header! \n";
		return false;
	}

	file >> word;
	if(word != "NumClasses:"){
		cout<<"HMM_ERROR: Could not find NumClasses \n";
		return false;
	}
	file >> numClasses;

	file >> word;
	if(word != "NumStates:"){
		cout<<"HMM_ERROR: Could not find NumStates \n";
		return false;
	}
	file >> numStates;

	file >> word;
	if(word != "NumSymbols:"){
		cout<<"HMM_ERROR: Could not find NumSymbols \n";
		return false;
	}
	file >> numSymbols;

	file >> word;
	if(word != "ModelType:"){
		cout<<"HMM_ERROR: Could not find ModelType \n";
		return false;
	}
	file >> modelType;

	file >> word;
	if(word != "Delta:"){
		cout<<"HMM_ERROR: Could not find Delta \n";
		return false;
	}
	file >> delta;

	//Resize the buffer
	hmmClassBuffer.resize(numClasses);

	//Load each of the K classes
	for(UINT k=0; k<numClasses; k++){
		UINT modelID;
		file >> word;
		if(word != "*************_MODEL_*************"){
			cout<<"HMM_ERROR: Could not find header for the "<<k+1<<"th model\n";
			return false;
		}

		file >> word;
		if(word != "Model_ID:"){
			cout<<"HMM_ERROR: Could not find model ID for the "<<k+1<<"th model\n";
			return false;
		}
		file >> modelID;

		if(modelID-1!=k){
			cout<<"HMM_ERROR: Model ID does not match the current class ID for the "<<k+1<<"th model\n";
			return false;
		}

		file >> word;
		if(word != "NumStates:"){
			cout<<"HMM_ERROR: Could not find the NumStates for the "<<k+1<<"th model\n";
			return false;
		}
		file >> hmmClassBuffer[k].numStates;

		file >> word;
		if(word != "NumSymbols:"){
			cout<<"HMM_ERROR: Could not find the NumSymbols for the "<<k+1<<"th model\n";
			return false;
		}
		file >> hmmClassBuffer[k].numSymbols;

		file >> word;
		if(word != "ModelType:"){
			cout<<"HMM_ERROR: Could not find the modelType for the "<<k+1<<"th model\n";
			return false;
		}
		file >> hmmClassBuffer[k].modelType;

		file >> word;
		if(word != "Delta:"){
			cout<<"HMM_ERROR: Could not find the Delta for the "<<k+1<<"th model\n";
			return false;
		}
		file >> hmmClassBuffer[k].delta;

		file >> word;
		if(word != "Threshold:"){
			cout<<"HMM_ERROR: Could not find the Threshold for the "<<k+1<<"th model\n";
			return false;
		}
		file >> hmmClassBuffer[k].cThreshold;

		file >> word;
		if(word != "NumTestSpawns:"){
			cout<<"HMM_ERROR: Could not find the NumTestSpawns for the "<<k+1<<"th model\n";
			return false;
		}
		file >> hmmClassBuffer[k].numTestSpawns;

		file >> word;
		if(word != "MaxNumTestIter:"){
			cout<<"HMM_ERROR: Could not find the MaxNumTestIter for the "<<k+1<<"th model\n";
			return false;
		}
		file >> hmmClassBuffer[k].maxNumTestIter;

		file >> word;
		if(word != "MaxNumIter:"){
			cout<<"HMM_ERROR: Could not find the MaxNumIter for the "<<k+1<<"th model\n";
			return false;
		}
		file >> hmmClassBuffer[k].maxNumIter;

		hmmClassBuffer[k].a.resize(hmmClassBuffer[k].numStates,hmmClassBuffer[k].numStates);
		hmmClassBuffer[k].b.resize(hmmClassBuffer[k].numStates,hmmClassBuffer[k].numSymbols);
		hmmClassBuffer[k].pi.resize(hmmClassBuffer[k].numStates);

		//Load the A, B and Pi matrices
		file >> word;
		if(word != "A:"){
			cout<<"HMM_ERROR: Could not find the A matrix for the "<<k+1<<"th model\n";
			return false;
		}

		//Load A
		for(UINT i=0; i<hmmClassBuffer[k].numStates; i++){
			for(UINT j=0; j<hmmClassBuffer[k].numStates; j++){
				double value;
				file >> value;
				hmmClassBuffer[k].a[i][j] = value;
			}
		}

		file >> word;
		if(word != "B:"){
			cout<<"HMM_ERROR: Could not find the B matrix for the "<<k+1<<"th model\n";
			return false;
		}

		//Load B
		for(UINT i=0; i<hmmClassBuffer[k].numStates; i++){
			for(UINT j=0; j<hmmClassBuffer[k].numSymbols; j++){
				double value;
				file >> value;
				hmmClassBuffer[k].b[i][j] = value;
			}
		}

		file >> word;
		if(word != "Pi:"){
			cout<<"HMM_ERROR: Could not find the Pi matrix for the "<<k+1<<"th model\n";
			return false;
		}

		//Load Pi
		for(UINT i=0; i<hmmClassBuffer[k].numStates; i++){
			double value;
			file >> value;
			hmmClassBuffer[k].pi[i] = value;
		}

		file >> word;
		if(word != "*********************************"){
			cout<<"HMM_ERROR: Could not find the model footer for the "<<k+1<<"th model\n";
			return false;
		}
	}

	file.close();

	return true;

}
     
     */
    
bool HMM::convertDataToObservationSequence( LabelledTimeSeriesClassificationData &classData, vector< vector< UINT > > &observationSequences ){
        
    observationSequences.resize( classData.getNumSamples() );
    
    for(UINT i=0; i<classData.getNumSamples(); i++){
        MatrixDouble &timeseries = classData[i].getData();
        observationSequences[i].resize( timeseries.getNumRows() );
        for(UINT j=0; j<timeseries.getNumRows(); j++){
            if( timeseries[j][0] >= numSymbols ){
                errorLog << "train(LabelledTimeSeriesClassificationData &trainingData) - Found an observation sequence with a value outside of the symbol range! Value: " << timeseries[j][0] << endl;
                return false;
            }
            observationSequences[i][j] = (UINT)timeseries[j][0];
        }
    }
    
    return true;
}

}//End of namespace GRT