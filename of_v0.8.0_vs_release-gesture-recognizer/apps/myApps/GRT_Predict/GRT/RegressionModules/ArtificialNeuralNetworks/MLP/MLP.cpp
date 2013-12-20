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

#include "MLP.h"

namespace GRT{
    
//Register the MLP module with the Regressifier base class
RegisterRegressifierModule< MLP > MLP::registerModule("MLP");

MLP::MLP(){
    inputLayerActivationFunction = Neuron::LINEAR;
    hiddenLayerActivationFunction = Neuron::LINEAR;
    outputLayerActivationFunction = Neuron::LINEAR;
	minNumEpochs = 10;
    maxNumEpochs = 100;
    numRandomTrainingIterations = 10;
    validationSetSize = 20;	//20% of the training data will be set aside for the validation set
    minChange = 1.0e-10;
	trainingRate = 0.1;
	momentum = 0.5;
	gamma = 2.0;
    trainingError = 0;
    nullRejectionCoeff = 0.9;
    nullRejectionThreshold = 0;
	useValidationSet = true;
	randomiseTrainingOrder = false;
    useMultiThreadingTraining = false;
	useScaling = true;
	trained = false;
    initialized = false;
    classificationModeActive = false;
    useNullRejection = true;
    clear();
    regressifierType = "MLP";
    debugLog.setProceedingText("[DEBUG MLP]");
    errorLog.setProceedingText("[ERROR MLP]");
    trainingLog.setProceedingText("[TRAINING MLP]");
    warningLog.setProceedingText("[WARNING MLP]");
}

MLP::~MLP(){
    clear();
}
    
MLP& MLP::operator=(const MLP &rhs){
    if( this != &rhs ){
        //MLP variables
        this->numInputNeurons = rhs.numInputNeurons;
        this->numHiddenNeurons = rhs.numHiddenNeurons;
        this->numOutputNeurons = rhs.numOutputNeurons;
        this->inputLayerActivationFunction = rhs.inputLayerActivationFunction;
        this->hiddenLayerActivationFunction = rhs.hiddenLayerActivationFunction;
        this->outputLayerActivationFunction = rhs.outputLayerActivationFunction;
        this->minNumEpochs = rhs.minNumEpochs;
        this->maxNumEpochs = rhs.maxNumEpochs;
        this->numRandomTrainingIterations = rhs.numRandomTrainingIterations;
        this->validationSetSize = rhs.validationSetSize;
        this->minChange = rhs.minChange;
        this->trainingRate = rhs.trainingRate;
        this->momentum = rhs.momentum;
        this->trainingError = rhs.trainingError;
        this->gamma = rhs.gamma;
        this->useValidationSet = rhs.useValidationSet;
        this->randomiseTrainingOrder = rhs.randomiseTrainingOrder;
        this->useMultiThreadingTraining = rhs.useMultiThreadingTraining;
        this->initialized = rhs.initialized;
        this->inputLayer = rhs.inputLayer;
        this->hiddenLayer = rhs.hiddenLayer;
        this->outputLayer = rhs.outputLayer;
        this->inputVectorRanges = rhs.inputVectorRanges;
        this->targetVectorRanges = rhs.targetVectorRanges;
        this->trainingErrorLog = rhs.trainingErrorLog;
        
        this->classificationModeActive = rhs.classificationModeActive;
        this->useNullRejection = rhs.useNullRejection;
        this->predictedClassLabel = rhs.predictedClassLabel;
        this->nullRejectionCoeff = rhs.nullRejectionCoeff;
        this->nullRejectionThreshold = rhs.nullRejectionThreshold;
        this->maxLikelihood = rhs.maxLikelihood;
        this->classLikelihoods = rhs.classLikelihoods;
        
        //Copy the base variables
        copyBaseVariables( (Regressifier*)&rhs );
    }
    return *this;
}
    
bool MLP::clone(const Regressifier *regressifier){
    
    if( regressifier == NULL ){
        errorMessage = "clone(Regressifier *regressifier) - regressifier is NULL!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    if( this->getRegressifierType() != regressifier->getRegressifierType() ){
        errorMessage = "clone(Regressifier *regressifier) - regressifier is not the correct type!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    *this = *(MLP*)regressifier;
    
    return true;
}
    
    
//Classifier interface
bool MLP::train(LabelledClassificationData &trainingData){
    
    if( !initialized ){
        errorMessage = "train(LabelledClassificationData &trainingData) - The MLP has not been initialized!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    if( trainingData.getNumDimensions() != numInputNeurons ){
        errorMessage = "train(LabelledRegressionData trainingData) - The number of input dimensions in the training data (" + Util::toString( trainingData.getNumDimensions() ) + ") does not match that of the MLP (" + Util::toString( numInputNeurons ) + ")";
        errorLog << errorMessage << endl;
        return false;
    }
    if( trainingData.getNumClasses() != numOutputNeurons ){
        errorMessage = "train(LabelledRegressionData trainingData) - The number of classes in the training data (" + Util::toString( trainingData.getNumClasses() ) + ") does not match that of the MLP (" + Util::toString( numOutputNeurons ) + ")";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Reformat the LabelledClassificationData as LabelledRegressionData
    LabelledRegressionData regressionData = trainingData.reformatAsLabelledRegressionData();
    
    //Flag that the MLP is being used for classification, not regression
    classificationModeActive = true;
    
    return train(regressionData);
}
    
//Classifier interface
bool MLP::predict(VectorDouble inputVector){
    
    if( !trained ){
        errorMessage = "predict(VectorDouble inputVector) - Model not trained!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    if( inputVector.size() != numInputNeurons ){
        errorMessage = "predict(VectorDouble inputVector) - The sie of the input vector (" + Util::toString( int(inputVector.size()) ) + ") does not match that of the number of input dimensions (" + Util::toString( numInputNeurons ) + ") ";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Set the mapped data as the classLikelihoods
    regressionData = feedforward(inputVector);
    
    if( classificationModeActive ){
        double bestValue = classLikelihoods[0];
        UINT bestIndex = 0;
        for(UINT i=1; i<classLikelihoods.size(); i++){
            if( classLikelihoods[i] > bestValue ){
                bestValue = classLikelihoods[i];
                bestIndex = i;
            }
        }
        
        //Set the maximum likelihood and predicted class label
        maxLikelihood = bestValue;
        predictedClassLabel = bestIndex+1;
        
        if( useNullRejection ){
            if( maxLikelihood < nullRejectionCoeff ){
                predictedClassLabel = 0;
            }
        }
    }
    
    return true;
}

bool MLP::init(UINT numInputNeurons,UINT numHiddenNeurons,UINT numOutputNeurons,UINT inputLayerActivationFunction,
                   UINT hiddenLayerActivationFunction,UINT outputLayerActivationFunction){
    
    //Clear any previous models
    clear();

	//Initialize the random seed
	random.setSeed( (UINT)time(NULL) );
    
    if( numInputNeurons == 0 || numHiddenNeurons == 0 || numOutputNeurons == 0 ){
        if( numInputNeurons == 0 ){  errorMessage = "init(...) - The number of input neurons is zero!" ; errorLog << errorMessage << endl; }
        if( numHiddenNeurons == 0 ){  errorMessage = "init(...) - The number of hidden neurons is zero!"; errorLog << errorMessage << endl; }
        if( numOutputNeurons == 0 ){  errorMessage = "init(...) - The number of output neurons is zero!"; errorLog << errorMessage << endl; }
        return false;
    }
    
    //Validate the activation functions
    if( !validateActivationFunction(inputLayerActivationFunction) || !validateActivationFunction(hiddenLayerActivationFunction) || !validateActivationFunction(outputLayerActivationFunction) ){
        errorMessage = "init(...) - One Of The Activation Functions Failed The Validation Check";
        errorLog << errorMessage << endl;
        return false;
    }

    //Set the size of the MLP
    this->numInputNeurons = numInputNeurons;
    this->numHiddenNeurons = numHiddenNeurons;
    this->numOutputNeurons = numOutputNeurons;
       
    //Set the validation layers
    this->inputLayerActivationFunction = inputLayerActivationFunction;
    this->hiddenLayerActivationFunction = hiddenLayerActivationFunction;
    this->outputLayerActivationFunction = outputLayerActivationFunction;
    
    //Setup the neurons for each of the layers
    inputLayer.resize(numInputNeurons);
    hiddenLayer.resize(numHiddenNeurons);
    outputLayer.resize(numOutputNeurons);
    
    //Init the neuron memory for each of the layers
    for(UINT i=0; i<numInputNeurons; i++){
        inputLayer[i].init(1,inputLayerActivationFunction);
        inputLayer[i].weights[0] = 1.0; //The weights for the input layer should always be 1
		inputLayer[i].bias = 0.0; //The bias for the input layer should always be 0
		inputLayer[i].gamma = gamma;
    }
    
    for(UINT i=0; i<numHiddenNeurons; i++){
        hiddenLayer[i].init(numInputNeurons,hiddenLayerActivationFunction);
		hiddenLayer[i].gamma = gamma;
    }
    
    for(UINT i=0; i<numOutputNeurons; i++){
        outputLayer[i].init(numHiddenNeurons,outputLayerActivationFunction);
		outputLayer[i].gamma = gamma;
    }
    
    initialized = true;
    
    return true;

}

bool MLP::clear(){
    numInputNeurons = 0;
    numHiddenNeurons = 0;
    numOutputNeurons = 0;
    inputLayer.clear();
    hiddenLayer.clear();
    outputLayer.clear();
	trained = false;
    initialized = false;
    return true;
}

bool MLP::train(LabelledRegressionData &trainingData_){

    trained = false;
    trainingResults.clear();
    
    if( !initialized ){
        errorMessage = "train(LabelledRegressionData trainingData) - The MLP has not be initialized!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    
    if( trainingData_.getNumSamples() == 0 ){
        errorMessage = "train(LabelledRegressionData trainingData) - The training data is empty!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Copy the training data
    LabelledRegressionData trainingData = trainingData_;
    
    //Create a validation dataset, if needed
	LabelledRegressionData validationData;
	if( useValidationSet ){
		validationData = trainingData.partition( 100 - validationSetSize );
	}

    //Clear the ranges of the input vector and target vectors
	inputVectorRanges.clear();
    targetVectorRanges.clear();

    const UINT M = trainingData.getNumSamples();
    const UINT N = trainingData.getNumInputDimensions();
    const UINT T = trainingData.getNumTargetDimensions();
	const UINT numTestingExamples = useValidationSet ? validationData.getNumSamples() : M;

    if( N != numInputNeurons ){
        errorMessage = "train(LabelledRegressionData trainingData) - The number of input dimensions in the training data (" + Util::toString( N ) + ") does not match that of the MLP (" + Util::toString( numInputNeurons ) + ")" ;
        errorLog << errorMessage << endl;
        return false;
    }
    if( T != numOutputNeurons ){
        errorMessage = "train(LabelledRegressionData trainingData) - The number of target dimensions in the training data (" + Util::toString( T ) + ") does not match that of the MLP (" + Util::toString( numOutputNeurons ) + ")";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Set the Regressifier input and output dimensions
    numFeatures = numInputNeurons;
    numOutputDimensions = numOutputNeurons;

    //Scale the training and validation data, if needed
	if( useScaling ){
		//Find the ranges for the input data
        inputVectorRanges = trainingData.getInputRanges();
        
        //Find the ranges for the target data
		targetVectorRanges = trainingData.getTargetRanges();

		//Now scale the training data and the validation data if required
		trainingData.scale(inputVectorRanges,targetVectorRanges,0.0,1.0);
        
		if( useValidationSet ){
			validationData.scale(inputVectorRanges,targetVectorRanges,0.0,1.0);
		}
	}
    
    //Setup the training loop
    bool keepTraining = true;
    UINT epoch = 0;
    double alpha = trainingRate;
	double beta = momentum;
    UINT bestIter = 0;
    MLP bestNetwork;
    totalSquaredTrainingError = 0;
    rootMeanSquaredTrainingError = 0;
    trainingError = 0;
    double error = 0;
    double lastError = 0;
    double accuracy = 0;
    double trainingSetAccuracy = 0;
    double trainingSetTotalSquaredError = 0;
    double bestError = numeric_limits< double >::max();
    double bestTSError = numeric_limits< double >::max();
    double bestRMSError = numeric_limits< double >::max();
    double bestAccuracy = 0;
	vector< UINT > indexList(M);
	vector< vector< double > > tempTrainingErrorLog;
	trainingErrorLog.clear();
    TrainingResult result;
    trainingResults.reserve(M);

    //Reset the indexList, this is used to randomize the order of the training examples, if needed
	for(UINT i=0; i<M; i++) indexList[i] = i;
	
    for(UINT iter=0; iter<numRandomTrainingIterations; iter++){
        
        epoch = 0;
        keepTraining = true;
		tempTrainingErrorLog.clear();
        
		//Randomise the start values of the neurons
        init(numInputNeurons,numHiddenNeurons,numOutputNeurons);
        
        if( randomiseTrainingOrder ){
            for(UINT i=0; i<M; i++){
                SWAP(indexList[ i ], indexList[ random.getRandomNumberInt(0, M) ]);
            }
        }
        
        while( keepTraining ){
            
            //Perform one training epoch
            accuracy = 0;
            totalSquaredTrainingError = 0;
            
            for(UINT i=0; i<M; i++){
                //Get the i'th training and target vectors
                VectorDouble trainingExample = trainingData[ indexList[i] ].getInputVector();
                VectorDouble targetVector = trainingData[ indexList[i] ].getTargetVector();
                
                //Perform the back propagation
                double backPropError = back_prop(trainingExample,targetVector,alpha,beta);

                //Compute the error for the i'th example
				if( classificationModeActive ){
                    VectorDouble y = feedforward(trainingExample);
                    
                    //Get the class label
                    double bestValue = targetVector[0];
                    UINT bestIndex = 0;
                    for(UINT i=1; i<targetVector.size(); i++){
                        if( targetVector[i] > bestValue ){
                            bestValue = targetVector[i];
                            bestIndex = i;
                        }
                    }
                    UINT classLabel = bestIndex + 1;
                    
                    //Get the predicted class label
                    bestValue = y[0];
                    bestIndex = 0;
                    for(UINT i=1; i<y.size(); i++){
                        if( y[i] > bestValue ){
                            bestValue = y[i];
                            bestIndex = i;
                        }
                    }
                    predictedClassLabel = bestIndex+1;
                    
                    if( classLabel == predictedClassLabel ){
                        accuracy++;
                    }
                    
                }else{
                    totalSquaredTrainingError += backPropError; //The backPropError is already squared
                }

				if( checkForNAN() ){
					keepTraining = false;
                    errorMessage = "train(LabelledRegressionData trainingData) - NaN found!";
                    errorLog << errorMessage << endl;
					break;
				}
            }
            
            //Compute the error over all the training/validation examples
			if( useValidationSet ){
                trainingSetAccuracy = accuracy;
                trainingSetTotalSquaredError = totalSquaredTrainingError;
				accuracy = 0;
                totalSquaredTrainingError = 0;
				//We don't need to scale the validation data as it is already scaled, so make sure scaling is set to off
				bool tempScalingState = useScaling;
				useScaling = false;
                
                //Iterate over the validation samples
                UINT numValidationSamples = validationData.getNumSamples();
				for(UINT i=0; i<numValidationSamples; i++){
					VectorDouble trainingExample = validationData[i].getInputVector();
					VectorDouble targetVector = validationData[i].getTargetVector();
                    
                    VectorDouble y = feedforward(trainingExample);
                    
                    if( classificationModeActive ){
                        //Get the class label
                        double bestValue = targetVector[0];
                        UINT bestIndex = 0;
                        for(UINT i=1; i<targetVector.size(); i++){
                            if( targetVector[i] > bestValue ){
                                bestValue = targetVector[i];
                                bestIndex = i;
                            }
                        }
                        UINT classLabel = bestIndex + 1;
                        
                        //Get the predicted class label
                        bestValue = y[0];
                        bestIndex = 0;
                        for(UINT i=1; i<y.size(); i++){
                            if( y[i] > bestValue ){
                                bestValue = y[i];
                                bestIndex = i;
                            }
                        }
                        predictedClassLabel = bestIndex+1;
                        
                        if( classLabel == predictedClassLabel ){
                            accuracy++;
                        }
                        
                    }else{
                        //Update the total squared error
                        for(UINT j=0; j<T; j++){
                            totalSquaredTrainingError += SQR( targetVector[j]-y[j] );
                        }
                    }
				}
                //Reset the scaling flag
				useScaling = tempScalingState;
                
                accuracy = (accuracy/double(numValidationSamples))*double(numValidationSamples);
                rootMeanSquaredTrainingError = sqrt( totalSquaredTrainingError / double(numValidationSamples) );
                
			}else{//We are not using a validation set
                accuracy = (accuracy/double(M))*double(M);
                rootMeanSquaredTrainingError = sqrt( totalSquaredTrainingError / double(M) );
            }
            
            //Store the errors
            VectorDouble temp(2);
            if( classificationModeActive ){
                temp[0] = 100.0 - trainingSetAccuracy;
                temp[1] = 100.0 - accuracy;
                tempTrainingErrorLog.push_back( temp );
                
                error = 100.0 - accuracy;
                
                //Store the training results
                result.setClassificationResult(iter,accuracy);
                trainingResults.push_back( result );
                trainingLog << "Random Training Iteration: " << iter+1 << " Epoch: " << epoch << " Accuracy: " << error << endl;
            }else{
                vector< double > temp(2);
                temp[0] = trainingSetTotalSquaredError;
                temp[1] = rootMeanSquaredTrainingError;
                tempTrainingErrorLog.push_back( temp );
                
                error = rootMeanSquaredTrainingError;
                
                //Store the training results
                result.setRegressionResult(iter,totalSquaredTrainingError,rootMeanSquaredTrainingError);
                trainingResults.push_back( result );
                trainingLog << "Random Training Iteration: " << iter+1 << " Epoch: " << epoch << " RMSError: " << error << endl;
            }
            
            //Check to see if we should stop training
            if( ++epoch >= maxNumEpochs ){
                keepTraining = false;
            }
            if( fabs( error - lastError ) <= minChange && epoch >= minNumEpochs ){
                keepTraining = false;
            }
            
            //Update the last error
            lastError = error;
            
            //Notify any observers of the new training data
            trainingResultsObserverManager.notifyObservers( result );
            
        }//End of While( keepTraining )
        
        if( lastError < bestError ){
            bestIter = iter;
            bestError = lastError;
            bestTSError = totalSquaredTrainingError;
            bestRMSError = rootMeanSquaredTrainingError;
            bestAccuracy = accuracy;
            bestNetwork = *this;
			trainingErrorLog = tempTrainingErrorLog;
        }

    }//End of For( numRandomTrainingIterations )
    
    if( classificationModeActive ) trainingLog << "Best Accuracy: " << bestAccuracy << " in Random Training Iteration: " << bestIter+1 << endl;
    else trainingLog << "Best RMSError: " << bestRMSError << " in Random Training Iteration: " << bestIter+1 << endl;

	//Check to make sure the best network has not got any NaNs in it
	if( checkForNAN() ){
        errorMessage = "train(LabelledRegressionData trainingData) - NAN Found!";
        errorLog << errorMessage << endl;
		return false;
	}
    
    //Set the MLP model to the model that best during training
    *this = bestNetwork;
    trainingError = classificationModeActive ? bestAccuracy : bestRMSError;
    
    //Compute the rejection threshold
    if( classificationModeActive ){
        double averageValue = 0;
        VectorDouble classificationPredictions;
        
        for(UINT i=0; i<numTestingExamples; i++){
            VectorDouble inputVector = useValidationSet ? validationData[i].getInputVector() : trainingData[i].getInputVector();
            VectorDouble targetVector = useValidationSet ? validationData[i].getTargetVector() : trainingData[i].getTargetVector();
            
            //Make the prediction
            VectorDouble y = feedforward(inputVector);
            
            //Get the class label
            double bestValue = targetVector[0];
            UINT bestIndex = 0;
            for(UINT i=1; i<targetVector.size(); i++){
                if( targetVector[i] > bestValue ){
                    bestValue = targetVector[i];
                    bestIndex = i;
                }
            }
            UINT classLabel = bestIndex + 1;
            
            //Get the predicted class label
            bestValue = y[0];
            bestIndex = 0;
            for(UINT i=1; i<y.size(); i++){
                if( y[i] > bestValue ){
                    bestValue = y[i];
                    bestIndex = i;
                }
            }
            predictedClassLabel = bestIndex+1;
            
            //Only add the max value if the prediction is correct
            if( classLabel == predictedClassLabel ){
                classificationPredictions.push_back( bestValue );
                averageValue += bestValue;
            }
        }
        
        averageValue /= double(classificationPredictions.size());
        double stdDev = 0;
        for(UINT i=0; i<classificationPredictions.size(); i++){
            stdDev += SQR(classificationPredictions[i]-averageValue);
        }
        stdDev = sqrt( stdDev / double(classificationPredictions.size()-1) );
        
        nullRejectionThreshold = averageValue-(stdDev*nullRejectionCoeff);
    }
    
    //Flag that the model has been successfully trained
	trained = true;

    return true;
}

double MLP::back_prop(VectorDouble &trainingExample,VectorDouble &targetVector,double alpha,double beta){
    
    double update = 0;
    VectorDouble inputNeuronsOuput;
    VectorDouble hiddenNeuronsOutput;
    VectorDouble outputNeuronsOutput;
    VectorDouble deltaO(numOutputNeurons);
    VectorDouble deltaH(numHiddenNeurons);
    
    //Forward propagation
    feedforward(trainingExample,inputNeuronsOuput,hiddenNeuronsOutput,outputNeuronsOutput);
    
    //Compute the error of the output layer: the derivative of the function times the error of the output
    for(UINT i=0; i<numOutputNeurons; i++){
		deltaO[i] = outputLayer[i].der(outputNeuronsOutput[i]) * (targetVector[i]-outputNeuronsOutput[i]);
    }
    
    //Compute the error of the hidden layer
    for(UINT i=0; i<numHiddenNeurons; i++){
        double sum = 0;
        for(UINT j=0; j<numOutputNeurons; j++){
            sum += outputLayer[j].weights[i] * deltaO[j];
        }
		deltaH[i] = hiddenLayer[i].der(hiddenNeuronsOutput[i]) * sum;
    }
    
    //Update the hidden weights: old hidden weights + (learningRate * inputToTheHiddenNeuron * deltaHidden )
    for(UINT i=0; i<numHiddenNeurons; i++){
        for(UINT j=0; j<numInputNeurons; j++){
			//Compute the update
            update = alpha * (beta * hiddenLayer[i].previousUpdate[j] + (1.0 - beta) * inputNeuronsOuput[j] * deltaH[i]);

			//Update the weights
			hiddenLayer[i].weights[j] += update;

			//Store the previous update
			hiddenLayer[i].previousUpdate[j] = update; 
        }
    }
    
    //Update the output weights
    for(UINT i=0; i<numOutputNeurons; i++){
        for(UINT j=0; j<numHiddenNeurons; j++){
			//Compute the update
            update = alpha * (beta * outputLayer[i].previousUpdate[j] + (1.0 - beta) * hiddenNeuronsOutput[j] * deltaO[i]);

			//Update the weights
			outputLayer[i].weights[j] += update;

			//Store the update
			outputLayer[i].previousUpdate[j] = update;

        }
    }
    
    //Update the hidden bias
    for(UINT i=0; i<numHiddenNeurons; i++){
		//Compute the update
		update = alpha * (beta * hiddenLayer[i].previousBiasUpdate + (1.0 - beta) * deltaH[i]);

		//Update the bias
        hiddenLayer[i].bias += update;

		//Store the update
		hiddenLayer[i].previousBiasUpdate = update;
    }
    
    //Update the output bias
    for(UINT i=0; i<numOutputNeurons; i++){
		//Compute the update
		update = alpha * (beta * outputLayer[i].previousBiasUpdate + (1.0 - beta) * deltaO[i]);

		//Update the bias
        outputLayer[i].bias += update;
		
		//Stire the update
		outputLayer[i].previousBiasUpdate = update;
    }
    
    //Compute the error 
    double error = 0;
    for(UINT i=0; i<numOutputNeurons; i++){
        error += (targetVector[i]-outputNeuronsOutput[i]) * (targetVector[i]-outputNeuronsOutput[i]);
    }

    return error;
}

VectorDouble MLP::feedforward(VectorDouble trainingExample){
    
    VectorDouble inputNeuronsOuput(numInputNeurons,0);
    VectorDouble hiddenNeuronsOutput(numHiddenNeurons,0);
    VectorDouble outputNeuronsOutput(numOutputNeurons,0);

	//Scale the input vector if required
	if( useScaling ){
		for(UINT i=0; i<numInputNeurons; i++){
			trainingExample[i] = scale(trainingExample[i],inputVectorRanges[i].minValue,inputVectorRanges[i].maxValue,0.0,1.0);
		}
	}
    
    //Input layer
	VectorDouble input(1);
    for(UINT i=0; i<numInputNeurons; i++){
        input[0] = trainingExample[i];
        inputNeuronsOuput[i] = inputLayer[i].fire(input);
    }
    
    //Hidden Layer
    for(UINT i=0; i<numHiddenNeurons; i++){
        hiddenNeuronsOutput[i] = hiddenLayer[i].fire(inputNeuronsOuput);
    }
    
    //Output Layer
    for(UINT i=0; i<numOutputNeurons; i++){
        outputNeuronsOutput[i] = outputLayer[i].fire(hiddenNeuronsOutput);
    }

	//Scale the output vector if required
	if( useScaling ){
		for(unsigned int i=0; i<numOutputNeurons; i++){
			outputNeuronsOutput[i] = scale(outputNeuronsOutput[i],0.0,1.0,targetVectorRanges[i].minValue,targetVectorRanges[i].maxValue);
		}
	}
    
    return outputNeuronsOutput;
    
}

void MLP::feedforward(VectorDouble &trainingExample,VectorDouble &inputNeuronsOuput,
                      VectorDouble &hiddenNeuronsOutput,VectorDouble &outputNeuronsOutput){
    
    inputNeuronsOuput.resize(numInputNeurons,0);
    hiddenNeuronsOutput.resize(numHiddenNeurons,0);
    outputNeuronsOutput.resize(numOutputNeurons,0);
    
    //Input layer
	VectorDouble input(1);
    for(UINT i=0; i<numInputNeurons; i++){
        input[0] = trainingExample[i];
        inputNeuronsOuput[i] = inputLayer[i].fire(input);
    }
    
    //Hidden Layer
    for(UINT i=0; i<numHiddenNeurons; i++){
        hiddenNeuronsOutput[i] = hiddenLayer[i].fire(inputNeuronsOuput);
    }
    
    //Output Layer
    for(UINT i=0; i<numOutputNeurons; i++){
        outputNeuronsOutput[i] = outputLayer[i].fire(hiddenNeuronsOutput);
    }
    
}

void MLP::printNetwork(){
    cout<<"***************** MLP *****************\n";
    cout<<"NumInputNeurons: "<<numInputNeurons<<endl;
    cout<<"NumHiddenNeurons: "<<numHiddenNeurons<<endl;
    cout<<"NumOutputNeurons: "<<numOutputNeurons<<endl;
    
    cout<<"InputWeights:\n";
    for(UINT i=0; i<numInputNeurons; i++){
        cout<<"Neuron: "<<i<<" Bias: " << inputLayer[i].bias << " Weights: ";
        for(UINT j=0; j<inputLayer[i].weights.size(); j++){
            cout<<inputLayer[i].weights[j]<<"\t";
        }cout<<endl;
    }
    
    cout<<"HiddenWeights:\n";
    for(UINT i=0; i<numHiddenNeurons; i++){
        cout<<"Neuron: "<<i<<" Bias: " << hiddenLayer[i].bias << " Weights: ";
        for(UINT j=0; j<hiddenLayer[i].weights.size(); j++){
            cout<<hiddenLayer[i].weights[j]<<"\t";
        }cout<<endl;
    }
    
    cout<<"OutputWeights:\n";
    for(UINT i=0; i<numOutputNeurons; i++){
        cout<<"Neuron: "<<i<<" Bias: " << outputLayer[i].bias << " Weights: ";
        for(UINT j=0; j<outputLayer[i].weights.size(); j++){
            cout<<outputLayer[i].weights[j]<<"\t";
        }cout<<endl;
    }
    
}

bool MLP::checkForNAN(){
    
    for(UINT i=0; i<numInputNeurons; i++){
        if( isNAN(inputLayer[i].bias) ) return true;
        for(UINT j=0; j<inputLayer[i].weights.size(); j++){
            if( isNAN(inputLayer[i].weights[j]) ) return true;
        }
    }
    
    for(UINT i=0; i<numHiddenNeurons; i++){
        if( isNAN(hiddenLayer[i].bias) ) return true;
        for(UINT j=0; j<hiddenLayer[i].weights.size(); j++){
            if( isNAN(hiddenLayer[i].weights[j]) ) return true;
        }
    }
    
    for(UINT i=0; i<numOutputNeurons; i++){
        if( isNAN(outputLayer[i].bias) ) return true;
        for(UINT j=0; j<outputLayer[i].weights.size(); j++){
            if( isNAN(outputLayer[i].weights[j]) ) return true;
        }
    }
    
    return false;
}

bool inline MLP::isNAN(double v){
    if( v != v ) return true;
    return false;
}
    
bool MLP::saveModelToFile(string filename){
    
    if( !trained ) return false;
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }
    
    file.close();
    
    return true;
}

bool MLP::saveModelToFile(fstream &file){

	if( !file.is_open() ){
        errorMessage = "saveModelToFile(fstream &file) - File is not open!";
        errorLog << errorMessage << endl;
		return false;
	}

	file << "GRT_MLP_FILE_V1.0\n";
	file << "NumInputNeurons: "<<numInputNeurons<<endl;
	file << "NumHiddenNeurons: "<<numHiddenNeurons<<endl;
	file << "NumOutputNeurons: "<<numOutputNeurons<<endl;
	file << "InputLayerActivationFunction: " <<activationFunctionToString(inputLayerActivationFunction)<< endl;
	file << "HiddenLayerActivationFunction: " <<activationFunctionToString(hiddenLayerActivationFunction)<< endl;
	file << "OutputLayerActivationFunction: " <<activationFunctionToString(outputLayerActivationFunction)<< endl;
	file << "MinNumEpochs: " << minNumEpochs << endl;
	file << "MaxNumEpochs: " << maxNumEpochs << endl;
	file << "NumRandomTrainingIterations: " << numRandomTrainingIterations << endl;
	file << "ValidationSetSize: " << validationSetSize << endl;
	file << "MinChange: " << minChange << endl;
	file << "TrainingRate: " << trainingRate << endl;
	file << "Momentum: " << momentum << endl;
	file << "Gamma: " << gamma << endl;
	file << "UseValidationSet: " << useValidationSet << endl;
	file << "RandomiseTrainingOrder: " << randomiseTrainingOrder << endl;
	file << "UseScaling: " << useScaling << endl;
    file << "ClassificationMode: " << classificationModeActive << endl;
    file << "UseNullRejection: " << useNullRejection << endl;
    file << "RejectionThreshold: " << nullRejectionThreshold << endl;
	
	file << "InputLayer: \n";
	for(UINT i=0; i<numInputNeurons; i++){
		file << "InputNeuron: " << i+1 << endl;
		file << "NumInputs: " << inputLayer[i].numInputs << endl;
		file << "Bias: " << inputLayer[i].bias << endl;
		file << "Gamma: " << inputLayer[i].gamma << endl;
		file << "Weights: " << endl;
		for(UINT j=0; j<inputLayer[i].numInputs; j++){
			file << inputLayer[i].weights[j] << "\t";
		}
		file << endl;
	}
	file << "\n";

	file << "HiddenLayer: \n";
	for(UINT i=0; i<numHiddenNeurons; i++){
		file << "HiddenNeuron: " << i+1 << endl;
		file << "NumInputs: " << hiddenLayer[i].numInputs << endl;
		file << "Bias: " << hiddenLayer[i].bias << endl;
		file << "Gamma: " << hiddenLayer[i].gamma << endl;
		file << "Weights: " << endl;
		for(UINT j=0; j<hiddenLayer[i].numInputs; j++){
			file << hiddenLayer[i].weights[j] << "\t";
		}
		file << endl;
	}
	file << "\n";

	file << "OutputLayer: \n";
	for(UINT i=0; i<numOutputNeurons; i++){
		file << "OutputNeuron: " << i+1 << endl;
		file << "NumInputs: " << outputLayer[i].numInputs << endl;
		file << "Bias: " << outputLayer[i].bias << endl;
		file << "Gamma: " << outputLayer[i].gamma << endl;
		file << "Weights: " << endl;
		for(UINT j=0; j<outputLayer[i].numInputs; j++){
			file << outputLayer[i].weights[j] << "\t";
		}
		file << endl;
	}

	if( useScaling ){
		file << "InputVectorRanges: \n";
		for(UINT j=0; j<numInputNeurons; j++){
			file << inputVectorRanges[j].minValue << "\t" << inputVectorRanges[j].maxValue << endl;
		}
		file << endl;

		file << "OutputVectorRanges: \n";
		for(UINT j=0; j<numOutputNeurons; j++){
			file << targetVectorRanges[j].minValue << "\t" << targetVectorRanges[j].maxValue << endl;
		}
		file << endl;
	}

	return true;
}
    
bool MLP::loadModelFromFile(string filename){
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    //Close the file
    file.close();
    
    return true;
    
}
    
bool MLP::loadModelFromFile(fstream &file){

	string activationFunction;

	//Clear any previous models
	clear();

	trained = false;

	if( !file.is_open() ){
        errorMessage = "loadModelFromFile(fstream &file) - File is not open!";
        errorLog << errorMessage << endl;
		return false;
	}

	string word;

	//Check to make sure this is a file with the MLP File Format
	file >> word;
	if(word != "GRT_MLP_FILE_V1.0"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find file header!";
        errorLog << errorMessage << endl;
		return false;
	}

	file >> word;
	if(word != "NumInputNeurons:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find NumInputNeurons!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> numInputNeurons;

	file >> word;
	if(word != "NumHiddenNeurons:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find NumHiddenNeurons!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> numHiddenNeurons;

	file >> word;
	if(word != "NumOutputNeurons:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find NumOutputNeurons!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> numOutputNeurons;

	file >> word;
	if(word != "InputLayerActivationFunction:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find InputLayerActivationFunction!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> activationFunction;
	inputLayerActivationFunction = activationFunctionFromString(activationFunction);

	file >> word;
	if(word != "HiddenLayerActivationFunction:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find HiddenLayerActivationFunction!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> activationFunction;
	hiddenLayerActivationFunction = activationFunctionFromString(activationFunction);

	file >> word;
	if(word != "OutputLayerActivationFunction:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find OutputLayerActivationFunction!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> activationFunction;
	outputLayerActivationFunction = activationFunctionFromString(activationFunction);

	file >> word;
	if(word != "MinNumEpochs:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find MinNumEpochs!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> minNumEpochs;

	file >> word;
	if(word != "MaxNumEpochs:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find MaxNumEpochs!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> maxNumEpochs;

	file >> word;
	if(word != "NumRandomTrainingIterations:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find NumRandomTrainingIterations!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> numRandomTrainingIterations;

	file >> word;
	if(word != "ValidationSetSize:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find ValidationSetSize!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> validationSetSize;

	file >> word;
	if(word != "MinChange:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find MinChange!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> minChange;

	file >> word;
	if(word != "TrainingRate:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find TrainingRate!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> trainingRate;

	file >> word;
	if(word != "Momentum:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find Momentum!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> momentum;

	file >> word;
	if(word != "Gamma:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find Gamma!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> gamma;

	file >> word;
	if(word != "UseValidationSet:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find UseValidationSet!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> useValidationSet;

	file >> word;
	if(word != "RandomiseTrainingOrder:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find RandomiseTrainingOrder!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> randomiseTrainingOrder;

	file >> word;
	if(word != "UseScaling:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find UseScaling!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> useScaling;
    
    file >> word;
	if(word != "ClassificationMode:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find ClassificationMode!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> classificationModeActive;
    
    file >> word;
	if(word != "UseNullRejection:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find UseNullRejection!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> useNullRejection;
    
    file >> word;
	if(word != "RejectionThreshold:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find RejectionThreshold!";
        errorLog << errorMessage << endl;
		return false;
	}
	file >> nullRejectionThreshold;

	//Resize the layers
	inputLayer.resize( numInputNeurons );
	hiddenLayer.resize( numHiddenNeurons );
	outputLayer.resize( numOutputNeurons );

	//Load the neuron data
	file >> word;
	if(word != "InputLayer:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find InputLayer!";
        errorLog << errorMessage << endl;
		return false;
	}

	for(UINT i=0; i<numInputNeurons; i++){
		UINT tempNeuronID = 0;

		file >> word;
		if(word != "InputNeuron:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find InputNeuron!";
            errorLog << errorMessage << endl;
			return false;
		}
		file >> tempNeuronID;

		if( tempNeuronID != i+1 ){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - InputNeuron ID does not match!";
            errorLog << errorMessage << endl;
			return false;
		}

		file >> word;
		if(word != "NumInputs:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find NumInputs!";
            errorLog << errorMessage << endl;
			return false;
		}
		file >> inputLayer[i].numInputs;

		//Resize the buffers
		inputLayer[i].weights.resize( inputLayer[i].numInputs );

		file >> word;
		if(word != "Bias:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find Bias!";
            errorLog << errorMessage << endl;
			return false;
		}
		file >> inputLayer[i].bias;

		file >> word;
		if(word != "Gamma:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find Gamma!";
            errorLog << errorMessage << endl;
			return false;
		}
		file >> inputLayer[i].gamma;

		file >> word;
		if(word != "Weights:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find Weights!";
            errorLog << errorMessage << endl;
			return false;
		}

		for(UINT j=0; j<inputLayer[i].numInputs; j++){
			file >> inputLayer[i].weights[j];
		}
	}

	//Load the Hidden Layer
	file >> word;
	if(word != "HiddenLayer:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find HiddenLayer!";
        errorLog << errorMessage << endl;
		return false;
	}

	for(UINT i=0; i<numHiddenNeurons; i++){
		UINT tempNeuronID = 0;

		file >> word;
		if(word != "HiddenNeuron:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find HiddenNeuron!";
            errorLog << errorMessage << endl;
			return false;
		}
		file >> tempNeuronID;

		if( tempNeuronID != i+1 ){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find HiddenNeuron ID does not match!";
            errorLog << errorMessage << endl;
			return false;
		}

		file >> word;
		if(word != "NumInputs:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find NumInputs!";
            errorLog << errorMessage << endl;
			return false;
		}
		file >> hiddenLayer[i].numInputs;

		//Resize the buffers
		hiddenLayer[i].weights.resize( hiddenLayer[i].numInputs );

		file >> word;
		if(word != "Bias:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find Bias!";
            errorLog << errorMessage << endl;
			return false;
		}
		file >> hiddenLayer[i].bias;

		file >> word;
		if(word != "Gamma:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find Gamma!";
            errorLog << errorMessage << endl;
			return false;
		}
		file >> hiddenLayer[i].gamma;

		file >> word;
		if(word != "Weights:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find Weights!";
            errorLog << errorMessage << endl;
			return false;
		}

		for(unsigned int j=0; j<hiddenLayer[i].numInputs; j++){
			file >> hiddenLayer[i].weights[j];
		}
	}

	//Load the Output Layer
	file >> word;
	if(word != "OutputLayer:"){
        file.close();
        errorMessage = "loadModelFromFile(fstream &file) - Failed to find OutputLayer!";
        errorLog << errorMessage << endl;
		return false;
	}

	for(UINT i=0; i<numOutputNeurons; i++){
		UINT tempNeuronID = 0;

		file >> word;
		if(word != "OutputNeuron:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find OutputNeuron!";
            errorLog << errorMessage << endl;
			return false;
		}
		file >> tempNeuronID;

		if( tempNeuronID != i+1 ){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find OuputNeuron ID does not match!!";
            errorLog << errorMessage << endl;
			return false;
		}

		file >> word;
		if(word != "NumInputs:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find NumInputs!";
            errorLog << errorMessage << endl;
			return false;
		}
		file >> outputLayer[i].numInputs;

		//Resize the buffers
		outputLayer[i].weights.resize( outputLayer[i].numInputs );

		file >> word;
		if(word != "Bias:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find Bias!";
            errorLog << errorMessage << endl;
			return false;
		}
		file >> outputLayer[i].bias;

		file >> word;
		if(word != "Gamma:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find Gamma!";
            errorLog << errorMessage << endl;
			return false;
		}
		file >> outputLayer[i].gamma;

		file >> word;
		if(word != "Weights:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find Weights!";
            errorLog << errorMessage << endl;
			return false;
		}

		for(UINT j=0; j<outputLayer[i].numInputs; j++){
			file >> outputLayer[i].weights[j];
		}
	}

	if( useScaling ){
		//Resize the ranges buffers
		inputVectorRanges.resize( numInputNeurons );
		targetVectorRanges.resize( numOutputNeurons );

		//Load the ranges
		file >> word;
		if(word != "InputVectorRanges:"){
            file.close();
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find InputVectorRanges!";
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
            errorMessage = "loadModelFromFile(fstream &file) - Failed to find OutputVectorRanges!";
            errorLog << errorMessage << endl;
			return false;
		}
		for(UINT j=0; j<targetVectorRanges.size(); j++){
			file >> targetVectorRanges[j].minValue;
			file >> targetVectorRanges[j].maxValue;
		}
	}

    initialized = true;
	trained = true;

	return true;
}
    
UINT MLP::getNumClasses(){
    if( classificationModeActive )
        return numOutputNeurons;
    return 0;
}
    
UINT MLP::getNumInputNeurons(){
    return numInputNeurons;
}

UINT MLP::getNumHiddenNeurons(){
    return numHiddenNeurons;
}
    
UINT MLP::getNumOutputNeurons(){
    return numOutputNeurons;
}
    
UINT MLP::getInputLayerActivationFunction(){
    return inputLayerActivationFunction;
}
    
UINT MLP::getHiddenLayerActivationFunction(){
    return hiddenLayerActivationFunction;
}
    
UINT MLP::getOutputLayerActivationFunction(){
    return outputLayerActivationFunction;
}
    
UINT MLP::getMinNumEpochs(){
    return minNumEpochs;
}
    
UINT MLP::getMaxNumEpochs(){
    return maxNumEpochs;
}
    
UINT MLP::getNumRandomTrainingIterations(){
    return numRandomTrainingIterations;
}
    
UINT MLP::getValidationSetSize(){
    return validationSetSize;
}
    
double MLP::getMinChange(){
    return minChange;
}
    
double MLP::getTrainingRate(){
    return trainingRate;
}
    
double MLP::getMomentum(){
    return momentum;
}
    
double MLP::getGamma(){
    return gamma;
}
    
double MLP::getTrainingError(){
    return trainingError;
}
    
bool MLP::getUseValidationSet(){
    return useValidationSet;
}
    
bool MLP::getRandomiseTrainingOrder(){
    return randomiseTrainingOrder;
}
    
bool MLP::getClassificationModeActive(){
    return classificationModeActive;
}
    
bool MLP::getRegressionModeActive(){
    return !classificationModeActive;
}
    
vector< Neuron > MLP::getInputLayer(){
    return inputLayer;
}
    
vector< Neuron > MLP::getHiddenLayer(){
    return hiddenLayer;
}
    
vector< Neuron > MLP::getOutputLayer(){
    return outputLayer;
}
    
vector< MinMax > MLP::getInputRanges(){
    return inputVectorRanges;
}
    
vector< MinMax > MLP::getOutputRanges(){
    return targetVectorRanges;
}
    
vector< vector< double > > MLP::getTrainingLog(){
    return trainingErrorLog;
}
    
bool MLP::getNullRejectionEnabled(){
    return useNullRejection;
}

double MLP::getNullRejectionCoeff(){
    return nullRejectionCoeff;
}
    
double MLP::getNullRejectionThreshold(){
    return nullRejectionThreshold;
}
    
double MLP::getMaximumLikelihood(){
    if( trained ) return maxLikelihood;
    return DEFAULT_NULL_LIKELIHOOD_VALUE;
}

VectorDouble MLP::getClassLikelihoods(){
    if( trained && classificationModeActive ) return classLikelihoods;
    return VectorDouble();
}

UINT MLP::getPredictedClassLabel(){
    if( trained && classificationModeActive ) return predictedClassLabel;
    return 0;
}

string MLP::activationFunctionToString(unsigned int activationFunction){
	string activationName;

	switch(activationFunction){
		case(Neuron::LINEAR):
			activationName = "LINEAR";
			break;
		case(Neuron::SIGMOID):
			activationName = "SIGMOID";
			break;
		case(Neuron::BIPOLAR_SIGMOID):
			activationName = "BIPOLAR_SIGMOID";
			break;
		default:
			activationName = "UNKNOWN";
			break;
	}

	return activationName;
}

UINT MLP::activationFunctionFromString(string activationName){
	UINT activationFunction = 0;

	if(activationName == "LINEAR" ){
		activationFunction = 0;
		return activationFunction;
	}
	if(activationName == "SIGMOID" ){
		activationFunction = 1;
		return activationFunction;
	}
	if(activationName == "BIPOLAR_SIGMOID" ){
		activationFunction = 2;
		return activationFunction;
	}
	return activationFunction;
}

bool MLP::validateActivationFunction(UINT actvationFunction){
	if( actvationFunction >= Neuron::LINEAR && actvationFunction < Neuron::NUMBER_OF_ACTIVATION_FUNCTIONS ) return true;
	return false;
}

bool MLP::setMinChange(double minChange){
	this->minChange = minChange;
	return true;
}

bool MLP::setTrainingRate(double trainingRate){
	if( trainingRate >= 0 && trainingRate <= 1.0 ){
		this->trainingRate = trainingRate;
		return true;
	}
	return false;
}

bool MLP::setMomentum(double momentum){
	if( momentum >= 0 && momentum <= 1.0 ){
		this->momentum = momentum;
		return true;
	}
	return false;
}

bool MLP::setGamma(double gamma){
	this->gamma = gamma;
	return true;
}

bool MLP::setUseValidationSet(bool useValidationSet){
	this->useValidationSet = useValidationSet;
	return true;
}

bool MLP::setRandomiseTrainingOrder(bool randomiseTrainingOrder){
	this->randomiseTrainingOrder = randomiseTrainingOrder;
	return true;
}
    
bool MLP::setUseMultiThreadingTraining(bool useMultiThreadingTraining){
    this->useMultiThreadingTraining = useMultiThreadingTraining;
    return true;
}
bool MLP::setMinNumEpochs(UINT minNumEpochs){
    if( minNumEpochs > 0 ){
        this->minNumEpochs = minNumEpochs;
        return true;
    }
    return false;
}
bool MLP::setMaxNumEpochs(UINT maxNumEpochs){
    if( maxNumEpochs > 0 ){
        this->maxNumEpochs = maxNumEpochs;
        return true;
    }
    return false;

}
bool MLP::setNumRandomTrainingIterations(UINT numRandomTrainingIterations){
    if( numRandomTrainingIterations > 0 ){
        this->numRandomTrainingIterations = numRandomTrainingIterations;
        return true;
    }
    return false;
}
bool MLP::setValidationSetSize(UINT validationSetSize){
    if( validationSetSize > 0 && validationSetSize < 100 ){
        this->validationSetSize = validationSetSize;
        return true;
    }
    return false;
}
    
bool MLP::setNullRejection(bool useNullRejection){
    this->useNullRejection = useNullRejection;
    return true;
}
    
bool MLP::setNullRejectionCoeff(double nullRejectionCoeff){
    if( nullRejectionCoeff > 0 ){
        this->nullRejectionCoeff = nullRejectionCoeff;
        return true;
    }
    return false;
}
    
} //End of namespace GRT
