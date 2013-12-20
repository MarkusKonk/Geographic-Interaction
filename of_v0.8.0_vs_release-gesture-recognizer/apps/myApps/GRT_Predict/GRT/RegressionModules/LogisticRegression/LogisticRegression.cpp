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

#include "LogisticRegression.h"

namespace GRT{

//Register the LogisticRegression module with the Classifier base class
RegisterRegressifierModule< LogisticRegression >  LogisticRegression::registerModule("LogisticRegression");

LogisticRegression::LogisticRegression(bool useScaling)
{
    this->useScaling = useScaling;
    minChange = 1.0e-10;
    maxNumIterations = 1000;
    learningRate = 0.01;
    regressifierType = "LogisticRegression";
    debugLog.setProceedingText("[DEBUG LogisticRegression]");
    errorLog.setProceedingText("[ERROR LogisticRegression]");
    trainingLog.setProceedingText("[TRAINING LogisticRegression]");
    warningLog.setProceedingText("[WARNING LogisticRegression]");
}

LogisticRegression::~LogisticRegression(void)
{
}
    
LogisticRegression& LogisticRegression::operator=(const LogisticRegression &rhs){
	if( this != &rhs ){
        this->learningRate = rhs.learningRate;
        this->minChange = rhs.minChange;
        this->maxNumIterations = rhs.maxNumIterations;
        this->w0 = rhs.w0;
        this->w = rhs.w;
        
        //Copy the base variables
        copyBaseVariables( (Regressifier*)&rhs );
	}
	return *this;
}

bool LogisticRegression::clone(const Regressifier *regressifier){
    if( regressifier == NULL ) return false;
    
    if( this->getRegressifierType() == regressifier->getRegressifierType() ){
        *this = *(LogisticRegression*)regressifier;
        return true;
    }
    return false;
}

bool LogisticRegression::train(LabelledRegressionData &trainingData){
    
    const unsigned int M = trainingData.getNumSamples();
    const unsigned int N = trainingData.getNumInputDimensions();
    const unsigned int K = trainingData.getNumTargetDimensions();
    trained = false;
    trainingResults.clear();
    
    if( M == 0 ){
        errorMessage = "train(LabelledRegressionData &trainingData) - Training data has zero samples!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    if( K == 0 ){
        errorMessage = "train(LabelledRegressionData &trainingData) - The number of target dimensions is not 1!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    numFeatures = N;
    numOutputDimensions = 1; //Logistic Regression will have 1 output
    inputVectorRanges.clear();
    targetVectorRanges.clear();
    
    //Scale the training and validation data, if needed
	if( useScaling ){
		//Find the ranges for the input data
        inputVectorRanges = trainingData.getInputRanges();
        
        //Find the ranges for the target data
		targetVectorRanges = trainingData.getTargetRanges();
        
		//Scale the training data
		trainingData.scale(inputVectorRanges,targetVectorRanges,0.0,1.0);
	}
    
    //Reset the weights
    Random rand;
    w0 = rand.getRandomNumberUniform(-0.1,0.1);
    w.resize(N);
    for(UINT j=0; j<N; j++){
        w[j] = rand.getRandomNumberUniform(-0.1,0.1);
    }

    double error = 0;
    double squaredError = 0;
    double lastSquaredError = 0;
    double delta = 0;
    UINT iter = 0;
    bool keepTraining = true;
    Random random;
    vector< UINT > randomTrainingOrder(M);
    TrainingResult result;
    trainingResults.reserve(M);
    
    //In most cases, the training data is grouped into classes (100 samples for class 1, followed by 100 samples for class 2, etc.)
    //This can cause a problem for stochastic gradient descent algorithm. To avoid this issue, we randomly shuffle the order of the
    //training samples. This random order is then used at each epoch.
    for(UINT i=0; i<M; i++){
        randomTrainingOrder[i] = i;
    }
    std::random_shuffle(randomTrainingOrder.begin(), randomTrainingOrder.end());
    
    //Run the main stochastic gradient descent training algorithm
    while( keepTraining ){
        
        //Run one epoch of training using stochastic gradient descent
        squaredError = 0;
        for(UINT m=0; m<M; m++){
            
            //Select the random sample
            UINT i = randomTrainingOrder[m];
            
            //Compute the error, given the current weights
            VectorDouble x = trainingData[i].getInputVector();
            VectorDouble y = trainingData[i].getTargetVector();
            double h = w0;
            for(UINT j=0; j<N; j++){
                h += x[j] * w[j];
            }
            error = y[0] - sigmoid( h );
            squaredError += SQR(error);
            
            //Update the weights
            for(UINT j=0; j<N; j++){
                w[j] += learningRate  * error * x[j];
            }
            w0 += learningRate * error;
        }
        
        //Compute the error
        delta = fabs( squaredError-lastSquaredError );
        lastSquaredError = squaredError;
        
        //Check to see if we should stop
        if( delta <= minChange ){
            keepTraining = false;
        }
        
        if( ++iter >= maxNumIterations ){
            keepTraining = false;
        }
        
        if( isinf( squaredError ) || isnan( squaredError ) ){
            errorMessage = "train(LabelledRegressionData &trainingData) - Training failed! Total squared error is NAN. If scaling is not enabled then you should try to scale your data and see if this solves the issue.";
            errorLog << errorMessage << endl;
            return false;
        }
        
        //Store the training results
        result.setRegressionResult(iter,totalSquaredTrainingError,rootMeanSquaredTrainingError);
        trainingResults.push_back( result );
        
        //Notify any observers of the new training data
        trainingResultsObserverManager.notifyObservers( result );
        
        trainingLog << "Epoch: " << iter << " SSE: " << squaredError << " Delta: " << delta << endl;
    }
    
    //Flag that the algorithm has been trained
    regressionData.resize(1,0);
    trained = true;
    return trained;
}

bool LogisticRegression::predict(VectorDouble inputVector){
    
    if( !trained ){
        errorLog << "predict(VectorDouble inputVector) - Model Not Trained!" << endl;
        return false;
    }
    
    if( !trained ) return false;
    
	if( inputVector.size() != numFeatures ){
        errorMessage = "predict(VectorDouble inputVector) - The size of the input vector (" + Util::toString((int)inputVector.size()) + ") does not match the num features in the model (" + Util::toString(numFeatures);
        errorLog << errorMessage << endl;
		return false;
	}
    
    if( useScaling ){
        for(UINT n=0; n<numFeatures; n++){
            inputVector[n] = scale(inputVector[n], inputVectorRanges[n].minValue, inputVectorRanges[n].maxValue, 0, 1);
        }
    }
    
    regressionData[0] =  w0;
    for(UINT j=0; j<numFeatures; j++){
        regressionData[0] += inputVector[j] * w[j];
    }
	regressionData[0] = sigmoid( regressionData[0] );
    
    if( useScaling ){
        for(UINT n=0; n<numOutputDimensions; n++){
            regressionData[n] = scale(regressionData[n], 0, 1, targetVectorRanges[n].minValue, targetVectorRanges[n].maxValue);
        }
    }
    
    return true;
}
    
bool LogisticRegression::saveModelToFile(string filename){

    if( !trained ) return false;
    
	std::fstream file; 
	file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }

	file.close();

	return true;
}
    
bool LogisticRegression::saveModelToFile(fstream &file){
    
    if(!file.is_open())
	{
        errorMessage = "loadModelFromFile(fstream &file) - The file is not open!";
        errorLog << errorMessage << endl;
		return false;
	}
    
	//Write the header info
	file<<"GRT_LOGISTIC_REGRESSION_MODEL_FILE_V1.0\n";
    file<<"NumFeatures: "<<numFeatures<<endl;
	file<<"NumOutputDimensions: "<<numOutputDimensions<<endl;
    file <<"UseScaling: " << useScaling << endl;
	
    ///Write the ranges if needed
    if( useScaling ){
		file << "InputVectorRanges: \n";
		for(UINT j=0; j<numFeatures; j++){
			file << inputVectorRanges[j].minValue << "\t" << inputVectorRanges[j].maxValue << endl;
		}
		file << endl;
        
		file << "OutputVectorRanges: \n";
		for(UINT j=0; j<numOutputDimensions; j++){
			file << targetVectorRanges[j].minValue << "\t" << targetVectorRanges[j].maxValue << endl;
		}
		file << endl;
	}
    
    file << "Weights: ";
    file << w0;
    for(UINT j=0; j<numFeatures; j++){
        file << " " << w[j];
    }
    file << endl;
     
    return true;
}

bool LogisticRegression::loadModelFromFile(string filename){

	std::fstream file; 
	file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    file.close();

	return true;

}
    
bool LogisticRegression::loadModelFromFile(fstream &file){
    
    trained = false;
    numFeatures = 0;
    w0 = 0;
    w.clear();
    
    if(!file.is_open())
    {
        errorMessage = "loadModelFromFile(string filename) - Could not open file to load model";
        errorLog << errorMessage << endl;
        return false;
    }
    
    std::string word;
    
    //Find the file type header
    file >> word;
    if(word != "GRT_LOGISTIC_REGRESSION_MODEL_FILE_V1.0"){
        errorMessage = "loadModelFromFile(string filename) - Could not find Model File Header";
        errorLog << errorMessage << endl;
        return false;
    }
    
    file >> word;
    if(word != "NumFeatures:"){
        errorMessage = "loadModelFromFile(string filename) - Could not find NumFeatures!";
        errorLog << errorMessage << endl;
        return false;
    }
    file >> numFeatures;
    
    file >> word;
    if(word != "NumOutputDimensions:"){
        errorMessage = "loadModelFromFile(string filename) - Could not find NumOutputDimensions!";
        errorLog << errorMessage << endl;
        return false;
    }
    file >> numOutputDimensions;
    
    file >> word;
    if(word != "UseScaling:"){
        errorMessage = "loadModelFromFile(string filename) - Could not find UseScaling!";
        errorLog << errorMessage << endl;
        return false;
    }
    file >> useScaling;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        inputVectorRanges.resize(numFeatures);
        targetVectorRanges.resize(numOutputDimensions);
        
        //Load the ranges
		file >> word;
		if(word != "InputVectorRanges:"){
			file.close();
            errorMessage = "loadModelFromFile(string filename) - Failed to find InputVectorRanges!";
            errorLog << errorMessage << endl;
			return false;
		}
		for(UINT j=0; j<inputVectorRanges.size(); j++){
			file >> inputVectorRanges[j].minValue;
			file >> inputVectorRanges[j].maxValue;
		}
        
		file >> word;
		if(word != "OutputVectorRanges:"){
			file.close();
            errorMessage = "loadModelFromFile(string filename) - Failed to find OutputVectorRanges!";
            errorLog << errorMessage << endl;
			return false;
		}
		for(UINT j=0; j<targetVectorRanges.size(); j++){
			file >> targetVectorRanges[j].minValue;
			file >> targetVectorRanges[j].maxValue;
		}
    }
    
    //Resize the weights
    w.resize(numFeatures);
    
    //Load the weights
    file >> word;
    if(word != "Weights:"){
        errorMessage = "loadModelFromFile(string filename) - Could not find the Weights!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    file >> w0;
    for(UINT j=0; j<numFeatures; j++){
        file >> w[j];
    
    }
    
    //Resize the regression data vector
    regressionData.resize(1,0);
    
    //Flag that the model has been trained
    trained = true;

    return true;
}
    
bool LogisticRegression::setLearningRate(double learningRate){
    if( learningRate > 0 ){
        this->learningRate = learningRate;
        return true;
    }
    return false;
}
   
bool LogisticRegression::setMinChange(double minChange){
    if( minChange > 0 ){
        this->minChange = minChange;
        return true;
    }
    return false;
}

bool LogisticRegression::setMaxNumIterations(UINT maxNumIterations){
    if( maxNumIterations > 0 ){
        this->maxNumIterations = maxNumIterations;
        return true;
    }
    return false;
}
    
double LogisticRegression::getLearningRate(){
    return learningRate;
}

double LogisticRegression::getMinChange(){
    return minChange;
}

UINT LogisticRegression::getMaxNumIterations(){
    return maxNumIterations;
}

double LogisticRegression::sigmoid(double x){
	return 1.0 / (1 + exp(-x));
}

} //End of namespace GRT

