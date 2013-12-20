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

#include "ANBC.h"

namespace GRT{

//Register the ANBC module with the Classifier base class
RegisterClassifierModule< ANBC > ANBC::registerModule("ANBC");

ANBC::ANBC(bool useScaling,bool useNullRejection,double nullRejectionCoeff)
{
    this->useScaling = useScaling;
    this->useNullRejection = useNullRejection;
    this->nullRejectionCoeff = nullRejectionCoeff;
    weightsDataSet = false;
    classifierType = "ANBC";
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG ANBC]");
    errorLog.setProceedingText("[ERROR ANBC]");
    trainingLog.setProceedingText("[TRAINING ANBC]");
    warningLog.setProceedingText("[WARNING ANBC]");
}

ANBC::~ANBC(void)
{
} 

ANBC& ANBC::operator=(const ANBC &rhs){
	if( this != &rhs ){
        //ANBC variables
        this->weightsDataSet = rhs.weightsDataSet;
        this->weightsData = rhs.weightsData;
		this->models = rhs.models;
        
        //Classifier variables
        copyBaseVariables( (Classifier*)&rhs );
	}
	return *this;
}

bool ANBC::clone(const Classifier *classifier){
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){

        ANBC *ptr = (ANBC*)classifier;
        //Clone the ANBC values 
        this->weightsDataSet = ptr->weightsDataSet;
        this->weightsData = ptr->weightsData;
		this->models = ptr->models;
        
        //Clone the classifier variables
        return copyBaseVariables( classifier );
    }
    return false;
}
    
bool ANBC::train(LabelledClassificationData &trainingData){
    
    return train(trainingData,nullRejectionCoeff);
}

/* double predict(vVectorDouble x)
 This method returns the ID of the most likely class given the observation x and the trained models
 */
bool ANBC::predict(VectorDouble inputVector){
    
    if( !trained ){
        errorLog << "predict(VectorDouble inputVector) - ANBC Model Not Trained!" << endl;
        return false;
    }
    
    predictedClassLabel = 0;
	maxLikelihood = -10000;
    
    if( !trained ) return false;
    
	if( inputVector.size() != numFeatures ){
        errorLog << "predict(VectorDouble inputVector) - The size of the input vector (" << inputVector.size() << ") does not match the num features in the model (" << numFeatures << endl;
		return false;
	}
    
    if( useScaling ){
        for(UINT n=0; n<numFeatures; n++){
            inputVector[n] = scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, MIN_SCALE_VALUE, MAX_SCALE_VALUE);
        }
    }
    
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses,0);
    
    double classLikelihoodsSum = 0;
    double minDist = -99e+99;
	for(UINT k=0; k<numClasses; k++){
		classDistances[k] = models[k].predict( inputVector );
        
        //At this point the class likelihoods and class distances are the same thing
        classLikelihoods[k] = classDistances[k];
        
        //If the distances are very far away then they could be -inf or nan so catch this so the sum still works
        if( isinf(classLikelihoods[k]) || isnan(classLikelihoods[k]) ) classLikelihoods[k] = -10000;
        
        //Make the 
        classLikelihoods[k] = exp( classLikelihoods[k] );
        classLikelihoodsSum += classLikelihoods[k];

        //The loglikelihood values are negative so we want the values closest to 0
		if( classDistances[k] > minDist ){
			minDist = classDistances[k];
			predictedClassLabel = k;
		}
    }
    
    //Normalize the classlikelihoods
    for(UINT k=0; k<numClasses; k++){
        if( classLikelihoodsSum == 0 ) classLikelihoods[k] = 0;
        else classLikelihoods[k] /= classLikelihoodsSum;
    }
    maxLikelihood = classLikelihoods[predictedClassLabel];
    
    if( useNullRejection ){
        //Check to see if the best result is greater than the models threshold
        if( minDist >= models[predictedClassLabel].threshold ) predictedClassLabel = models[predictedClassLabel].classLabel;
        else predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
    }else predictedClassLabel = models[predictedClassLabel].classLabel;
    
    return true;
}

bool ANBC::train(LabelledClassificationData &labelledTrainingData,double gamma){
    
    const unsigned int M = labelledTrainingData.getNumSamples();
    const unsigned int N = labelledTrainingData.getNumDimensions();
    const unsigned int K = labelledTrainingData.getNumClasses();
    trained = false;
    models.clear();
    classLabels.clear();
    
    if( M == 0 ){
        errorLog << "train(LabelledClassificationData &labelledTrainingData,double gamma) - Training data has zero samples!" << endl;
        return false;
    }
    
    if( weightsDataSet ){
        if( weightsData.getNumDimensions() != N ){
            errorLog << "train(LabelledClassificationData &labelledTrainingData,double gamma) - The number of dimensions in the weights data (" << weightsData.getNumDimensions() << ") is not equal to the number of dimensions of the training data (" << N << ")" << endl;
            return false;
        }    
    }

    numFeatures = N;
    numClasses = K;
    models.resize(K);
    classLabels.resize(K);
    ranges = labelledTrainingData.getRanges();
    
    //Train each of the models
	for(UINT k=0; k<numClasses; k++){
        
        //Get the class label for the kth class
        UINT classLabel = labelledTrainingData.getClassTracker()[k].classLabel;
        
        //Set the kth class label
        classLabels[k] = classLabel;
        
        //Get the weights for this class
		VectorDouble weights(numFeatures);
        if( weightsDataSet ){
            bool weightsFound = false;
            for(UINT i=0; i<weightsData.getNumSamples(); i++){
                if( weightsData[i].getClassLabel() == classLabel ){
                    weights = weightsData[i].getSample();
                    weightsFound = true;
                    break;
                }
            }
            
            if( !weightsFound ){
                errorLog << "train(LabelledClassificationData &labelledTrainingData,double gamma) - Failed to find the weights for class " << classLabel << endl;
                return false;
            }
        }else{
            //If the weights data has not been set then all the weights are 1
            for(UINT j=0; j<numFeatures; j++) weights[j] = 1.0;
        }
        
        //Get all the training data for this class
        LabelledClassificationData classData = labelledTrainingData.getClassData(classLabel);
        MatrixDouble data(classData.getNumSamples(),N);
        
        //Copy the training data into a matrix, scaling the training data if needed
        for(UINT i=0; i<data.getNumRows(); i++){
            for(UINT j=0; j<data.getNumCols(); j++){
                if( useScaling ){
                    data[i][j] = scale(classData[i][j],ranges[j].minValue,ranges[j].maxValue,MIN_SCALE_VALUE,MAX_SCALE_VALUE);
                }else data[i][j] = classData[i][j];
            }
        }
        
        //Train the model for this class
		models[k].gamma = gamma;
		if( !models[k].train(classLabel,data,weights) ){
            errorLog << "train(LabelledClassificationData &labelledTrainingData,double gamma) - Failed to train model for class: " << classLabel << endl;
            
            //Try and work out why the training failed
            if( models[k].N == 0 ){
                errorLog << "train(LabelledClassificationData &labelledTrainingData,double gamma) - N == 0!" << endl;
                models.clear();
                return false;
            }
            for(UINT j=0; j<numFeatures; j++){
                if( models[k].mu[j] == 0 ){
                    errorLog << "train(LabelledClassificationData &labelledTrainingData,double gamma) - The mean of column " << j+1 << " is zero! Check the training data" << endl;
                    models.clear();
                    return false;
                }
            }
            models.clear();
            return false;
        }
        
	}
    
    //Store the null rejection thresholds
    nullRejectionThresholds.resize(numClasses);
    for(UINT k=0; k<numClasses; k++) {
        nullRejectionThresholds[k] = models[k].threshold;
    }
    
    //Flag that the models have been trained
    trained = true;
    return trained;
    
}

bool ANBC::recomputeNullRejectionThresholds(){

    if( trained ){
        if( nullRejectionThresholds.size() != numClasses )
            nullRejectionThresholds.resize(numClasses);
        for(UINT k=0; k<numClasses; k++) {
            models[k].recomputeThresholdValue(nullRejectionCoeff);
            nullRejectionThresholds[k] = models[k].threshold;
        }
        return true;
    }
    return false;
}
    
bool ANBC::setNullRejectionCoeff(double nullRejectionCoeff){
    
    if( nullRejectionCoeff > 0 ){
        this->nullRejectionCoeff = nullRejectionCoeff;
        recomputeNullRejectionThresholds();
        return true;
    }
    return false;
}
    
bool ANBC::setWeights(LabelledClassificationData weightsData){
    
    if( weightsData.getNumSamples() > 0 ){
        weightsDataSet = true;
        this->weightsData = weightsData;
        return true;
    }
    return false;
}
    
bool ANBC::saveModelToFile(string filename){

    if( !trained ) return false;
    
	std::fstream file; 
	file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }

	file.close();

	return true;
}
    
bool ANBC::saveModelToFile(fstream &file){
    
    if(!file.is_open())
	{
		errorLog <<"saveANBCModelToFile(fstream &file) - The file is not open!" << endl;
		return false;
	}
    
	//Write the header info
	file<<"GRT_ANBC_MODEL_FILE_V1.0\n";
    file<<"NumFeatures: "<<numFeatures<<endl;
	file<<"NumClasses: "<<numClasses<<endl;
    file <<"UseScaling: " << useScaling << endl;
    file<<"UseNullRejection: " << useNullRejection << endl;
	
    ///Write the ranges if needed
    if( useScaling ){
        file << "Ranges: \n";
        for(UINT n=0; n<ranges.size(); n++){
            file << ranges[n].minValue << "\t" << ranges[n].maxValue << endl;
        }
    }
    
	//Write each of the models
	for(UINT k=0; k<numClasses; k++){
		file<<"*************_MODEL_*************\n";
		file<<"Model_ID: "<<k+1<<endl;
		file<<"N: "<<models[k].N<<endl;
        file<<"ClassLabel: "<<models[k].classLabel<<endl;
		file<<"Threshold: "<<models[k].threshold<<endl;
		file<<"Gamma: "<<models[k].gamma<<endl;
		file<<"TrainingMu: "<<models[k].trainingMu<<endl;
		file<<"TrainingSigma: "<<models[k].trainingSigma<<endl;
		
		file<<"Mu:\n";
		for(UINT j=0; j<models[k].N; j++){
            file<<models[k].mu[j]<<"\t";
		}file<<endl;
        
		file<<"Sigma:\n";
		for(UINT j=0; j<models[k].N; j++){
            file<<models[k].sigma[j]<<"\t";
		}file<<endl;
        
		file<<"Weights:\n";
		for(UINT j=0; j<models[k].N; j++){
            file<<models[k].weights[j]<<"\t";
		}file<<endl;
        
		file<<"*********************************\n";
	}
    
    return true;
}

bool ANBC::loadModelFromFile(string filename){

	std::fstream file; 
	file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    file.close();

	return true;

}
    
bool ANBC::loadModelFromFile(fstream &file){
    
    trained = false;
    numFeatures = 0;
    numClasses = 0;
    models.clear();
    classLabels.clear();
    
    if(!file.is_open())
    {
        errorLog << "loadANBCModelFromFile(string filename) - Could not open file to load model" << endl;
        return false;
    }
    
    std::string word;
    
    //Find the file type header
    file >> word;
    if(word != "GRT_ANBC_MODEL_FILE_V1.0"){
        errorLog << "loadANBCModelFromFile(string filename) - Could not find Model File Header" << endl;
        return false;
    }
    
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << "loadANBCModelFromFile(string filename) - Could not find NumFeatures " << endl;
        return false;
    }
    file >> numFeatures;
    
    file >> word;
    if(word != "NumClasses:"){
        errorLog << "loadANBCModelFromFile(string filename) - Could not find NumClasses" << endl;
        return false;
    }
    file >> numClasses;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "loadANBCModelFromFile(string filename) - Could not find UseScaling" << endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if(word != "UseNullRejection:"){
        errorLog << "loadANBCModelFromFile(string filename) - Could not find UseNullRejection" << endl;
        return false;
    }
    file >> useNullRejection;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        ranges.resize(numFeatures);
        
        file >> word;
        if(word != "Ranges:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the Ranges" << endl;
            return false;
        }
        for(UINT n=0; n<ranges.size(); n++){
            file >> ranges[n].minValue;
            file >> ranges[n].maxValue;
        }
    }
    
    //Resize the buffer
    models.resize(numClasses);
    classLabels.resize(numClasses);
    
    //Load each of the K models
    for(UINT k=0; k<numClasses; k++){
        UINT modelID;
        file >> word;
        if(word != "*************_MODEL_*************"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find header for the "<<k+1<<"th model" << endl;
            return false;
        }
        
        file >> word;
        if(word != "Model_ID:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find model ID for the "<<k+1<<"th model" << endl;
            return false;
        }
        file >> modelID;
        
        if(modelID-1!=k){
            cout<<"ANBC: Model ID does not match the current class ID for the "<<k+1<<"th model" << endl;
            return false;
        }
        
        file >> word;
        if(word != "N:"){
            cout<<"ANBC: Could not find N for the "<<k+1<<"th model" << endl;
            return false;
        }
        file >> models[k].N;
        
        file >> word;
        if(word != "ClassLabel:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find ClassLabel for the "<<k+1<<"th model" << endl;
            return false;
        }
        file >> models[k].classLabel;
        classLabels[k] = models[k].classLabel;
        
        file >> word;
        if(word != "Threshold:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the threshold for the "<<k+1<<"th model" << endl;
            return false;
        }
        file >> models[k].threshold;
        
        file >> word;
        if(word != "Gamma:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the gamma parameter for the "<<k+1<<"th model" << endl;
            return false;
        }
        file >> models[k].gamma;
        
        file >> word;
        if(word != "TrainingMu:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the training mu parameter for the "<<k+1<<"th model" << endl;
            return false;
        }
        file >> models[k].trainingMu;
        
        file >> word;
        if(word != "TrainingSigma:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the training sigma parameter for the "<<k+1<<"th model" << endl;
            return false;
        }
        file >> models[k].trainingSigma;
        
        //Resize the buffers
        models[k].mu.resize(numFeatures);
        models[k].sigma.resize(numFeatures);
        models[k].weights.resize(numFeatures);
        
        //Load Mu, Sigma and Weights
        file >> word;
        if(word != "Mu:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the Mu vector for the "<<k+1<<"th model" << endl;
            return false;
        }
        
        //Load Mu
        for(UINT j=0; j<models[k].N; j++){
            double value;
            file >> value;
            models[k].mu[j] = value;
        }
        
        file >> word;
        if(word != "Sigma:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the Sigma vector for the "<<k+1<<"th model" << endl;
            return false;
        }
        
        //Load Sigma
        for(UINT j=0; j<models[k].N; j++){
            double value;
            file >> value;
            models[k].sigma[j] = value;
        }
        
        file >> word;
        if(word != "Weights:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the Weights vector for the "<<k+1<<"th model" << endl;
            return false;
        }
        
        //Load Weights
        for(UINT j=0; j<models[k].N; j++){
            double value;
            file >> value;
            models[k].weights[j] = value;
        }
        
        file >> word;
        if(word != "*********************************"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the model footer for the "<<k+1<<"th model" << endl;
            return false;
        }
    }
    
    //Flag that the model is trained
    trained = true;
    
    //Recompute the null rejection thresholds
    recomputeNullRejectionThresholds();
    
    //Resize the prediction results to make sure it is setup for realtime prediction
    maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
    bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
    classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
    classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    
    return true;
}
    
bool ANBC::reset(){
    return true;
}
    
VectorDouble ANBC::getNullRejectionThresholds() const{
    if( !trained ) return VectorDouble();
    return nullRejectionThresholds;
}

} //End of namespace GRT

