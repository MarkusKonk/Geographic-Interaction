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

#include "GestureRecognitionPipeline.h"

namespace GRT{

GestureRecognitionPipeline::GestureRecognitionPipeline(void)
{
    initialized = false;
    trained = false;
    pipelineMode = PIPELINE_MODE_NOT_SET;
    inputVectorDimensions = 0;
    outputVectorDimensions = 0;
    predictedClassLabel = 0;
    predictionModuleIndex = 0;
    numTrainingSamples = 0;
    numTestSamples = 0;
    testAccuracy = 0;
    testRMSError = 0;
    testSquaredError = 0;
    testRejectionPrecision = 0;
    testRejectionRecall = 0;
    testTime = 0;
    trainingTime = 0;
    classifier = NULL;
    regressifier = NULL;
    contextModules.resize( NUM_CONTEXT_LEVELS );
    
    warningMessage = "";
    errorMessage = "";
    debugLog.setProceedingText("[DEBUG GRP]");
    errorLog.setProceedingText("[ERROR GRP]");
    warningLog.setProceedingText("[WARNING GRP]");
    testingLog.setProceedingText("[TEST GRP]");
}

GestureRecognitionPipeline::GestureRecognitionPipeline(const GestureRecognitionPipeline &rhs){
	
	initialized = false;
    trained = false;
    pipelineMode = PIPELINE_MODE_NOT_SET;
    inputVectorDimensions = 0;
    outputVectorDimensions = 0;
    predictedClassLabel = 0;
    predictionModuleIndex = 0;
    numTrainingSamples = 0;
    numTestSamples = 0;
    testAccuracy = 0;
    testRMSError = 0;
    testSquaredError = 0;
    testRejectionPrecision = 0;
    testRejectionRecall = 0;
    testTime = 0;
    trainingTime = 0;
    classifier = NULL;
    regressifier = NULL;
    contextModules.resize( NUM_CONTEXT_LEVELS );
    
    warningMessage = "";
    errorMessage = "";
    debugLog.setProceedingText("[DEBUG GRP]");
    errorLog.setProceedingText("[ERROR GRP]");
    warningLog.setProceedingText("[WARNING GRP]");
    testingLog.setProceedingText("[TEST GRP]");

	*this = rhs;
}


GestureRecognitionPipeline& GestureRecognitionPipeline::operator=(const GestureRecognitionPipeline &rhs){
	
	if( this != &rhs ){
		clearAll();
		
		this->initialized = rhs.initialized;
	    this->trained = rhs.trained;
	    this->inputVectorDimensions = rhs.inputVectorDimensions;
	    this->outputVectorDimensions = rhs.outputVectorDimensions;
	    this->predictedClassLabel = rhs.predictedClassLabel;
	    this->pipelineMode = rhs.pipelineMode;
	    this->predictionModuleIndex = rhs.predictionModuleIndex;
        this->numTrainingSamples = rhs.numTrainingSamples;
        this->numTestSamples = rhs.numTestSamples;
	    this->testAccuracy = rhs.testAccuracy;
	    this->testRMSError = rhs.testRMSError;
        this->testSquaredError = rhs.testSquaredError;
	    this->testTime = rhs.testTime;
	    this->trainingTime = rhs.trainingTime;
	    this->testFMeasure = rhs.testFMeasure;
	    this->testPrecision = rhs.testPrecision;
	    this->testRecall = rhs.testRecall;
	    this->regressionData = rhs.regressionData;
	    this->testRejectionPrecision = rhs.testRejectionPrecision;
	    this->testRejectionRecall = rhs.testRejectionRecall;
	    this->testConfusionMatrix = rhs.testConfusionMatrix;
	    this->testClassificationResults = rhs.testClassificationResults;
        this->warningMessage = rhs.warningMessage;
        this->errorMessage = rhs.errorMessage;
	
		for(unsigned int i=0; i<rhs.preProcessingModules.size(); i++){
			this->addPreProcessingModule( *(rhs.preProcessingModules[i]) );
		}
		
		for(unsigned int i=0; i<rhs.featureExtractionModules.size(); i++){
			this->addFeatureExtractionModule( *(rhs.featureExtractionModules[i]) );
		}
		
		if( rhs.getIsPipelineInClassificationMode() ){
			setClassifier( *rhs.classifier );
		}
		
		if( rhs.getIsPipelineInRegressionMode() ){
			setRegressifier( *rhs.regressifier );
		}
		
		for(unsigned int i=0; i<rhs.postProcessingModules.size(); i++){
			this->addPostProcessingModule( *(rhs.postProcessingModules[i]) );
		}
		
		for(unsigned int k=0; k<NUM_CONTEXT_LEVELS; k++){
			for(unsigned int i=0; i<rhs.contextModules[k].size(); i++){
				this->addContextModule( *(rhs.contextModules[k][i]), k );
			}
		}
	}
	
	return *this;
}

GestureRecognitionPipeline::~GestureRecognitionPipeline(void)
{
    //Clean up the memory
    deleteAllPreProcessingModules();
    deleteAllFeatureExtractionModules();
    deleteClassifier();
    deleteRegressifier();
    deleteAllPostProcessingModules();
    deleteAllContextModules();
} 
    
bool GestureRecognitionPipeline::train(LabelledClassificationData trainingData){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsClassifierSet() ){
        errorLog << "train(LabelledClassificationData trainingData) - Failed To Train Classifier, the classifier has not been set!" << endl;
        return false;
    }
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << "train(LabelledClassificationData trainingData) - Failed To Train Classifier, there is no training data!" << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Set the input vector dimension size
    inputVectorDimensions = trainingData.getNumDimensions();
    
    //Pass the training data through any pre-processing or feature extraction units
    UINT numDimensions = trainingData.getNumDimensions();
    
    //If there are any preprocessing or feature extraction modules, then get the size of the last module
    if( getIsPreProcessingSet() || getIsFeatureExtractionSet() ){
        if( getIsFeatureExtractionSet() ){
            numDimensions = featureExtractionModules[ featureExtractionModules.size()-1 ]->getNumOutputDimensions();
        }else{
            numDimensions = preProcessingModules[ preProcessingModules.size()-1 ]->getNumOutputDimensions();
        }
    }
    
    //Start the training timer
    Timer timer;
    timer.start();
    
    LabelledClassificationData processedTrainingData( numDimensions );
    
    for(UINT i=0; i<trainingData.getNumSamples(); i++){
        bool okToAddProcessedData = true;
        UINT classLabel = trainingData[i].getClassLabel();
        VectorDouble trainingSample = trainingData[i].getSample();
        
        //Perform any preprocessing
        if( getIsPreProcessingSet() ){
            for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
                if( !preProcessingModules[moduleIndex]->process( trainingSample ) ){
                    errorMessage = "train(LabelledClassificationData trainingData) - Failed to PreProcess Training Data. PreProcessingModuleIndex: ";
                    errorMessage += Util::toString( moduleIndex );
                    errorLog << errorMessage << endl;
                    return false;
                }
                trainingSample = preProcessingModules[moduleIndex]->getProcessedData();
            }
        }
        
        //Compute any features
        if( getIsFeatureExtractionSet() ){
            for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
                if( !featureExtractionModules[moduleIndex]->computeFeatures( trainingSample ) ){
                    errorMessage = "train(LabelledClassificationData trainingData) - Failed to Compute Features from Training Data. FeatureExtractionModuleIndex ";
                    errorMessage += Util::toString( moduleIndex );
                    errorLog << errorMessage << endl;
                    return false;
                }
                if( featureExtractionModules[moduleIndex]->getFeatureDataReady() ){
                    trainingSample = featureExtractionModules[moduleIndex]->getFeatureVector();
                }else{
                   okToAddProcessedData = false;
                   break;
                }
            }
        }

        if( okToAddProcessedData ){
            //Add the training sample to the processed training data
            processedTrainingData.addSample(classLabel, trainingSample);
        }
        
    }
    
    if( processedTrainingData.getNumSamples() != trainingData.getNumSamples() ){
        
        warningMessage = "train(LabelledClassificationData trainingData) - Lost " + Util::toString(trainingData.getNumSamples()-processedTrainingData.getNumSamples()) + " of " + Util::toString( trainingData.getNumSamples() ) + " training samples due to the processing stage!";
        warningLog << warningMessage << endl;
    }
    
    numTrainingSamples = processedTrainingData.getNumSamples();
    
    //Train the classifier
    trained = classifier->train( processedTrainingData );
    if( !trained ){
        errorMessage = "train(LabelledClassificationData trainingData) - Failed To Train Classifier: " + classifier->getLastErrorMessage();
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::train(LabelledClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsClassifierSet() ){
        errorMessage = "train(LabelledClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling) - Failed To Train Classifier, the classifier has not been set!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    if( trainingData.getNumSamples() == 0 ){
        errorMessage = "train(LabelledClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling) - Failed To Train Classifier, there is no training data!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Start the training timer
    Timer timer;
    timer.start();

    //Spilt the data into K folds
    bool spiltResult = trainingData.spiltDataIntoKFolds(kFoldValue, useStratifiedSampling);
    
    if( !spiltResult ){
        return false;
    }
    
    //Run the k-fold training and testing
    double crossValidationAccuracy = 0;
    LabelledClassificationData foldTrainingData;
    LabelledClassificationData foldTestData;

    for(UINT k=0; k<kFoldValue; k++){
        ///Train the classification system
		foldTrainingData = trainingData.getTrainingFoldData(k);
        
        if( !train( foldTrainingData ) ){
            return false;
        }
        
        //Test the classification system
        foldTestData = trainingData.getTestFoldData(k);
        
        if( !test( foldTestData ) ){
            return false;
        }
        
        crossValidationAccuracy += getTestAccuracy();
    }
    
    //Set the accuracy of the classification system averaged over the kfolds
    testAccuracy = crossValidationAccuracy / double(kFoldValue);
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}

bool GestureRecognitionPipeline::train(LabelledTimeSeriesClassificationData trainingData){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsClassifierSet() ){
        errorMessage = "train(LabelledTimeSeriesClassificationData trainingData) - Failed To Train Classifier, the classifier has not been set!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    if( trainingData.getNumSamples() == 0 ){
        errorMessage =  "train(LabelledTimeSeriesClassificationData trainingData) - Failed To Train Classifier, there is no training data!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Start the training timer
    Timer timer;
    timer.start();
    
    //Set the input vector dimension size of the pipeline
    inputVectorDimensions = trainingData.getNumDimensions();
    
    LabelledTimeSeriesClassificationData processedTrainingData( trainingData.getNumDimensions() );
    LabelledTimeSeriesClassificationData labelledTimeseriesClassificationData;
    LabelledClassificationData labelledClassificationData;
    
    //Setup the data structure, if the classifier works with timeseries data then we use LabelledTimeSeriesClassificationData
    //otherwise we format the data as LabelledClassificationData
    if( classifier->getTimeseriesCompatible() ){
        UINT trainingDataInputDimensionSize = trainingData.getNumDimensions();
        if( getIsPreProcessingSet() ){
            trainingDataInputDimensionSize = preProcessingModules[ preProcessingModules.size()-1 ]->getNumOutputDimensions();
        }
        if( getIsFeatureExtractionSet() ){
            trainingDataInputDimensionSize = featureExtractionModules[ featureExtractionModules.size()-1 ]->getNumOutputDimensions();
        }
        labelledTimeseriesClassificationData.setNumDimensions( trainingDataInputDimensionSize );
    }else{
        UINT trainingDataInputDimensionSize = trainingData.getNumDimensions();
        if( getIsPreProcessingSet() ){
            trainingDataInputDimensionSize = preProcessingModules[ preProcessingModules.size()-1 ]->getNumOutputDimensions();
        }
        if( getIsFeatureExtractionSet() ){
            trainingDataInputDimensionSize = featureExtractionModules[ featureExtractionModules.size()-1 ]->getNumOutputDimensions();
        }
        labelledClassificationData.setNumDimensions( trainingDataInputDimensionSize );
    }
    
    //Pass the timeseries data through any pre-processing modules and add it to the processedTrainingData structure
    for(UINT i=0; i<trainingData.getNumSamples(); i++){
        UINT classLabel = trainingData[i].getClassLabel();
        MatrixDouble trainingSample = trainingData[i].getData();
        
        if( getIsPreProcessingSet() ){
            
            //Try to process the matrix data row-by-row
            bool resetPreprocessingModule = true;
            for(UINT r=0; r<trainingSample.getNumRows(); r++){
                VectorDouble sample = trainingSample.getRowVector( r );
                
                for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
                    
                    if( resetPreprocessingModule ){
                        preProcessingModules[moduleIndex]->reset();
                    }
                    
                    //Validate the input and output dimensions match!
                    if( preProcessingModules[moduleIndex]->getNumInputDimensions() != preProcessingModules[moduleIndex]->getNumOutputDimensions() ){
                        errorMessage = "train(LabelledTimeSeriesClassificationData trainingData) - Failed To PreProcess Training Data. The number of inputDimensions (";
                        errorMessage += Util::toString( preProcessingModules[moduleIndex]->getNumInputDimensions() );
                        errorMessage += ") in  PreProcessingModule ";
                        errorMessage += Util::toString( moduleIndex );
                        errorMessage += " do not match the number of outputDimensions (";
                        errorMessage += Util::toString( preProcessingModules[moduleIndex]->getNumOutputDimensions() );
                        errorLog << errorMessage << endl;
                        return false;
                    }
                    
                    if( !preProcessingModules[moduleIndex]->process( sample ) ){
                        errorMessage = "train(LabelledTimeSeriesClassificationData trainingData) - Failed To PreProcess Training Data. PreProcessingModuleIndex: ";
                        errorMessage += Util::toString( moduleIndex );
                        errorLog << errorMessage << endl;
                        return false;
                    }
                    sample = preProcessingModules[moduleIndex]->getProcessedData();
                }
                
                //The preprocessing modules should only be reset when r==0
                resetPreprocessingModule = false;
                
                //Overwrite the original training sample with the preProcessed sample
                for(UINT c=0; c<sample.size(); c++){
                    trainingSample[r][c] = sample[c];
                }
            }
            
        }
        
        //Add the training sample to the processed training data
        processedTrainingData.addSample(classLabel,trainingSample);
    }
    
    //Loop over the processed training data, perfrom any feature extraction if needed
    //Add the data to either the timeseries or classification data structures
    for(UINT i=0; i<processedTrainingData.getNumSamples(); i++){
        UINT classLabel = processedTrainingData[i].getClassLabel();
        MatrixDouble trainingSample = processedTrainingData[i].getData();
        bool featureDataReady = false;
        bool resetFeatureExtractionModules = true;
        
        MatrixDouble featureData;
        //Try to process the matrix data row-by-row
        for(UINT r=0; r<trainingSample.getNumRows(); r++){
            VectorDouble inputVector = trainingSample.getRowVector( r );
            featureDataReady = true;
            
             //Pass the processed training data through the feature extraction
            if( getIsFeatureExtractionSet() ){
            
                for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
                    
                    if( resetFeatureExtractionModules ){
                        featureExtractionModules[moduleIndex]->reset();
                    }
                    
                    if( !featureExtractionModules[moduleIndex]->computeFeatures( inputVector ) ){
                        errorMessage = "train(LabelledTimeSeriesClassificationData trainingData) - Failed To Compute Features For Training Data. FeatureExtractionModuleIndex: ";
                        errorMessage += Util::toString( moduleIndex );
                        errorLog << errorMessage << endl;
                        return false;
                    }
                    
                    //Overwrite the input vector with the features so this can either be input to the next feature module 
                    //or converted to the LabelledClassificationData format
                    inputVector = featureExtractionModules[moduleIndex]->getFeatureVector();
                    featureDataReady = featureExtractionModules[moduleIndex]->getFeatureDataReady();
                }
                
                //The feature extraction modules should only be reset on r == 0
                resetFeatureExtractionModules = false;
                
                if( featureDataReady ){
                    
                    if( classifier->getTimeseriesCompatible() ){
                        if( !featureData.push_back( inputVector ) ){
                            errorMessage = "train(LabelledTimeSeriesClassificationData trainingData) - Failed To add feature vector to feature data matrix! FeatureExtractionModuleIndex: ";
                            errorLog << errorMessage << endl;
                            return false;
                        }
                    }else labelledClassificationData.addSample(classLabel, inputVector);
                }
                
            }else{
                if( classifier->getTimeseriesCompatible() ){
                    if( !featureData.push_back( inputVector ) ){
                        errorMessage = "train(LabelledTimeSeriesClassificationData trainingData) - Failed To add feature vector to feature data matrix! FeatureExtractionModuleIndex: ";
                        errorLog << errorMessage << endl;
                        return false;
                    }
                }
                else labelledClassificationData.addSample(classLabel, inputVector);
            }
        }
        
        if( classifier->getTimeseriesCompatible() ) labelledTimeseriesClassificationData.addSample(classLabel, featureData);
        
    }
        
    //Train the classification system
    if( classifier->getTimeseriesCompatible() ){
        numTrainingSamples = labelledTimeseriesClassificationData.getNumSamples();
        trained = classifier->train( labelledTimeseriesClassificationData );
    }else{
        numTrainingSamples = labelledClassificationData.getNumSamples();
        trained = classifier->train( labelledClassificationData );
    }

    if( !trained ){
        errorMessage = "train(LabelledTimeSeriesClassificationData trainingData) - Failed To Train Classifier" + classifier->getLastErrorMessage();
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::train(LabelledTimeSeriesClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsClassifierSet() ){
        errorMessage = "train(LabelledTimeSeriesClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling) - Failed To Train Classifier, the classifier has not been set!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    if( trainingData.getNumSamples() == 0 ){
        errorMessage = "train(LabelledTimeSeriesClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling) - Failed To Train Classifier, there is no training data!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Start the training timer
    Timer timer;
    timer.start();
    
    //Spilt the data into K folds
    if( !trainingData.spiltDataIntoKFolds(kFoldValue, useStratifiedSampling) ){
        errorMessage = "train(LabelledTimeSeriesClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling) - Failed To Spilt Dataset into KFolds!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Run the k-fold training and testing
    double crossValidationAccuracy = 0;
    LabelledTimeSeriesClassificationData foldTrainingData;
    LabelledTimeSeriesClassificationData foldTestData;
    
    for(UINT k=0; k<kFoldValue; k++){
        ///Train the classification system
		foldTrainingData = trainingData.getTrainingFoldData(k);
        
        if( !train( foldTrainingData ) ){
            errorMessage = "train(LabelledTimeSeriesClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling) - Failed to train pipeline for fold " + Util::toString(k) + ".";
            errorLog << errorMessage << endl;
            return false;
        }
        
        //Test the classification system
        foldTestData = trainingData.getTestFoldData(k);
        
        if( !test( foldTestData ) ){
            errorMessage =  "train(LabelledTimeSeriesClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling) - Failed to test pipeline for fold " + Util::toString(k) + ".";
            errorLog << errorMessage << endl;
            return false;
        }
        
        crossValidationAccuracy += getTestAccuracy();
    }
    
    //Set the accuracy of the classification system averaged over the kfolds
    testAccuracy = crossValidationAccuracy / double(kFoldValue);
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::train(LabelledRegressionData trainingData){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    //Reset all the modules
    reset();
    
    //Start the training timer
    Timer timer;
    timer.start();
    
    //Set the input vector dimension size
    inputVectorDimensions = trainingData.getNumInputDimensions();
    
    //Pass the training data through any pre-processing or feature extraction units
    LabelledRegressionData processedTrainingData;
    
    //Set the dimensionality of the data
    UINT numInputs = 0;
    UINT numTargets = trainingData.getNumTargetDimensions();
    if( !getIsPreProcessingSet() && !getIsFeatureExtractionSet() ){
        numInputs = trainingData.getNumInputDimensions();
    }else{
        
        if( getIsPreProcessingSet() && !getIsFeatureExtractionSet() ){
            numInputs = preProcessingModules[ preProcessingModules.size()-1 ]->getNumOutputDimensions();
        }
        
        if( !getIsPreProcessingSet() && getIsFeatureExtractionSet() ){
            numInputs = featureExtractionModules[ featureExtractionModules.size()-1 ]->getNumOutputDimensions();
        }
        
        if( getIsPreProcessingSet() && getIsFeatureExtractionSet() ){
            numInputs = featureExtractionModules[ featureExtractionModules.size()-1 ]->getNumOutputDimensions();
        }
    }
    
    processedTrainingData.setInputAndTargetDimensions(numInputs, numTargets);
    
    for(UINT i=0; i<trainingData.getNumSamples(); i++){
        VectorDouble inputVector = trainingData[i].getInputVector();
        VectorDouble targetVector = trainingData[i].getTargetVector();
        
        if( getIsPreProcessingSet() ){
            for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
                if( !preProcessingModules[ moduleIndex ]->process( inputVector ) ){
                    errorMessage = "train(const LabelledRegressionData trainingData) - Failed To Compute Features For Training Data. PreProcessingModuleIndex: " + Util::toString( moduleIndex );
                    errorLog << errorMessage << endl;
                    return false;
                }
                
                inputVector = preProcessingModules[ moduleIndex ]->getProcessedData();
            }
        }
        
        if( getIsFeatureExtractionSet() ){
            for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
                if( !featureExtractionModules[ moduleIndex ]->computeFeatures( inputVector ) ){
                    errorMessage = "train(const LabelledRegressionData trainingData) - Failed To Compute Features For Training Data. FeatureExtractionModuleIndex: " + Util::toString( moduleIndex );
                    errorLog << errorMessage << endl;
                    return false;
                }
                
                inputVector = featureExtractionModules[ moduleIndex ]->getFeatureVector();
            }
        }
        
        //Add the training sample to the processed training data
        if( !processedTrainingData.addSample(inputVector,targetVector) ){
            errorMessage = "train(const LabelledRegressionData trainingData) - Failed to add processed training sample to training data";
            errorLog << errorMessage << endl;
            return false;
        }
    }
    
    numTrainingSamples = processedTrainingData.getNumSamples();
    
    //Train the classification system
    if( getIsRegressifierSet() ){
        trained =  regressifier->train( processedTrainingData );
        if( !trained ){
            errorMessage = "train(const LabelledRegressionData trainingData) - Failed To Train Regressifier: " + regressifier->getLastErrorMessage();
            errorLog << errorMessage << endl;
            return false;
        }
    }else{
        errorMessage = "train(const LabelledRegressionData trainingData) - Classifier is not set";
        errorLog << errorMessage << endl;
        return false;
    }

    trained = true;
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::train(LabelledRegressionData trainingData,const UINT kFoldValue){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsRegressifierSet() ){
        errorMessage = "train(LabelledRegressionData trainingData,const UINT kFoldValue) - Failed To Train Regressifier, the regressifier has not been set!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    if( trainingData.getNumSamples() == 0 ){
        errorMessage = "train(LabelledRegressionData trainingData,const UINT kFoldValue) - Failed To Train Regressifier, there is no training data!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Start the training timer
    Timer timer;
    timer.start();
    
    //Spilt the data into K folds
    bool spiltResult = trainingData.spiltDataIntoKFolds(kFoldValue);
    
    if( !spiltResult ){
        return false;
    }
    
    //Run the k-fold training and testing
    double crossValidationAccuracy = 0;
    LabelledRegressionData foldTrainingData;
    LabelledRegressionData foldTestData;
    for(UINT k=0; k<kFoldValue; k++){
        ///Train the classification system
        foldTrainingData = trainingData.getTrainingFoldData(k);
        
        if( !train( foldTrainingData ) ){
            return false;
        }
        
        //Test the classification system
        foldTestData = trainingData.getTestFoldData(k);
        
        if( !test( foldTestData ) ){
            return false;
        }

        crossValidationAccuracy += getTestRMSError();
        
    }

    cout << "CrossValidationAccuracy: " << crossValidationAccuracy << endl;
    testAccuracy = crossValidationAccuracy / double(kFoldValue);
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::test(LabelledClassificationData testData){
    
    //Clear any previous test results
    clearTestResults();
    
    //Make sure the classification model has been trained
    if( !trained ){
        errorMessage = "test(LabelledClassificationData testData) - Classifier is not trained";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Make sure the dimensionality of the test data matches the input vector's dimensions
    if( testData.getNumDimensions() != inputVectorDimensions ){
        errorMessage = "test(LabelledClassificationData testData) - The dimensionality of the test data (" + Util::toString(testData.getNumDimensions()) + ") does not match that of the input vector dimensions of the pipeline (" + Util::toString( inputVectorDimensions ) + ")";
        errorLog << errorMessage << endl;
        return false;
    }
    
    if( !getIsClassifierSet() ){
        errorMessage = "test(LabelledClassificationData testData) - The classifier has not been set";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Reset all the modules
    reset();

	//Validate that the class labels in the test data match the class labels in the model
	bool classLabelValidationPassed = true;
	for(UINT i=0; i<testData.getNumClasses(); i++){
		bool labelFound = false;
		for(UINT k=0; k<classifier->getNumClasses(); k++){
			if( testData.getClassTracker()[i].classLabel == classifier->getClassLabels()[k] ){
				labelFound = true;
				break;
			}
		}

		if( !labelFound ){
			classLabelValidationPassed = false;
            errorMessage =  "test(LabelledClassificationData testData) - The test dataset contains a class label (" + Util::toString(testData.getClassTracker()[i].classLabel) + ") that is not in the model!";
			errorLog << errorMessage << endl;
		}
	}

	if( !classLabelValidationPassed ){
        errorMessage = "test(LabelledClassificationData testData) -  Model Class Labels: ";
        for(UINT k=0; k<classifier->getNumClasses(); k++){
			errorMessage += Util::toString( classifier->getClassLabels()[k]) + "\t";
		}
        errorLog << errorMessage << endl;
        return false;
    }

    double rejectionPrecisionCounter = 0;
    double rejectionRecallCounter = 0;
    unsigned int confusionMatrixSize = classifier->getNullRejectionEnabled() ? classifier->getNumClasses()+1 : classifier->getNumClasses();
    VectorDouble precisionCounter(classifier->getNumClasses(), 0);
    VectorDouble recallCounter(classifier->getNumClasses(), 0);
    VectorDouble confusionMatrixCounter(confusionMatrixSize,0);
    
    //Resize the test matrix
    testConfusionMatrix.resize(confusionMatrixSize, confusionMatrixSize);
    testConfusionMatrix.setAllValues(0);
    
    //Resize the precision and recall vectors
    testPrecision.clear();
    testRecall.clear();
    testFMeasure.clear();
    testPrecision.resize(getNumClassesInModel(), 0);
    testRecall.resize(getNumClassesInModel(), 0);
    testFMeasure.resize(getNumClassesInModel(), 0);
    testClassificationResults.resize(testData.getNumSamples());
    numTestSamples = testData.getNumSamples();
    
    //Start the test timer
    Timer timer;
    timer.start();

    //Run the test
    for(UINT i=0; i<testData.getNumSamples(); i++){
        UINT classLabel = testData[i].getClassLabel();
        VectorDouble testSample = testData[i].getSample();
        
        //Pass the test sample through the pipeline
        if( !predict( testSample ) ){
            errorMessage = "test(LabelledClassificationData testData) - Prediction failed for test sample at index: " + Util::toString(i);
            errorLog << errorMessage << endl;
            return false;
        }
        
        //Update the test metrics
        UINT predictedClassLabel = getPredictedClassLabel();
        
        if( !updateTestMetrics(classLabel,predictedClassLabel,precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter) ){
            errorMessage = "test(LabelledClassificationData testData) - Failed to update test metrics at test sample index: " + Util::toString(i);
            errorLog << errorMessage << endl;
            return false;
        }
        
        //Keep track of the classification results encase the user needs them later
        testClassificationResults[i].classLabel = testData[i].getClassLabel();
        testClassificationResults[i].predictedClassLabel = predictedClassLabel;
        testClassificationResults[i].unprocessedPredictedClassLabel = getUnProcessedPredictedClassLabel();
        testClassificationResults[i].maximumLikelihood = getMaximumLikelihood();
        testClassificationResults[i].classLikelihoods = getClassLikelihoods();
        testClassificationResults[i].classDistances = getClassDistances();
    }
    
    if( !computeTestMetrics(precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter, testData.getNumSamples()) ){
        errorMessage = "test(LabelledClassificationData testData) - Failed to compute test metrics!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    testTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::test(LabelledTimeSeriesClassificationData testData){

    //Clear any previous test results
    clearTestResults();
    
    //Make sure the classification model has been trained
    if( !trained ){
        errorMessage =  "test(LabelledTimeSeriesClassificationData testData) - The classifier has not been trained";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Make sure the dimensionality of the test data matches the input vector's dimensions
    if( testData.getNumDimensions() != inputVectorDimensions ){
        errorMessage = "test(LabelledTimeSeriesClassificationData testData) - The dimensionality of the test data (" + Util::toString(testData.getNumDimensions()) + ") does not match that of the input vector dimensions of the pipeline (" + Util::toString(inputVectorDimensions) + ")";
        errorLog << errorMessage << endl;
        return false;
    }
    
    if( !getIsClassifierSet() ){
        errorMessage = "test(LabelledTimeSeriesClassificationData testData) - The classifier has not been set";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    double rejectionPrecisionCounter = 0;
    double rejectionRecallCounter = 0;
    const UINT K = classifier->getNumClasses();
    UINT confusionMatrixSize = classifier->getNullRejectionEnabled() ? K+1 : K;
    VectorDouble precisionCounter(K, 0);
    VectorDouble recallCounter(K, 0);
    VectorDouble confusionMatrixCounter(confusionMatrixSize,0);
    
    //Resize the test matrix
    testConfusionMatrix.resize(confusionMatrixSize,confusionMatrixSize);
    testConfusionMatrix.setAllValues(0);
    
    //Resize the precision and recall vectors
    testPrecision.resize(K, 0);
    testRecall.resize(K, 0);
    testFMeasure.resize(K, 0);
    numTestSamples = testData.getNumSamples();
    
    //Start the test timer
    Timer timer;
    timer.start();
    
    //Run the test
	const UINT M = testData.getNumSamples();
    for(UINT i=0; i<M; i++){
        UINT classLabel = testData[i].getClassLabel();
        MatrixDouble timeseries = testData[i].getData();
            
        //Pass the test timeseries through the pipeline
        if( !predict( timeseries ) ){
            errorMessage = "test(LabelledTimeSeriesClassificationData testData) - Failed to run prediction for test sample index: " + Util::toString(i);
            errorLog << errorMessage << endl;
            return false;
        }
        
        //Update the test metrics
        UINT predictedClassLabel = getPredictedClassLabel();
        
        if( !updateTestMetrics(classLabel,predictedClassLabel,precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter) ){
            errorMessage = "test(LabelledTimeSeriesClassificationData testData) - Failed to update test metrics at test sample index: " + Util::toString(i);
            errorLog << errorMessage << endl;
            return false;
        }
        
    }
        
    if( !computeTestMetrics(precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter, M) ){
        errorMessage = "test(LabelledTimeSeriesClassificationData testData) - Failed to compute test metrics!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    testTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::test(LabelledContinuousTimeSeriesClassificationData testData){
    
    //Clear any previous test results
    clearTestResults();
    
    //Make sure the classification model has been trained
    if( !trained ){
        errorMessage = "test(LabelledContinuousTimeSeriesClassificationData testData) - The classifier has not been trained";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Make sure the dimensionality of the test data matches the input vector's dimensions
    if( testData.getNumDimensions() != inputVectorDimensions ){
        errorMessage = "test(LabelledContinuousTimeSeriesClassificationData testData) - The dimensionality of the test data (" + Util::toString(testData.getNumDimensions()) + ") does not match that of the input vector dimensions of the pipeline (" + Util::toString(inputVectorDimensions) + ")";
        errorLog << errorMessage << endl;
        return false;
    }
    
    if( !getIsClassifierSet() ){
        errorMessage = "test(LabelledContinuousTimeSeriesClassificationData testData) - The classifier has not been set";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    double rejectionPrecisionCounter = 0;
    double rejectionRecallCounter = 0;
    UINT confusionMatrixSize = classifier->getNullRejectionEnabled() ? classifier->getNumClasses()+1 : classifier->getNumClasses();
    VectorDouble precisionCounter(getNumClassesInModel(), 0);
    VectorDouble recallCounter(getNumClassesInModel(), 0);
    VectorDouble confusionMatrixCounter(confusionMatrixSize,0);
    
    //Resize the test matrix
    testConfusionMatrix.resize(confusionMatrixSize,confusionMatrixSize);
    testConfusionMatrix.setAllValues(0);
    
    //Resize the precision and recall vectors
    testPrecision.resize(getNumClassesInModel(), 0);
    testRecall.resize(getNumClassesInModel(), 0);
    testFMeasure.resize(getNumClassesInModel(), 0);
    
    //Resize the classification results vector
    testClassificationResults.resize(testData.getNumSamples());
    numTestSamples = testData.getNumSamples();
    
    testingLog << "Starting Test..." << endl;
    
    //Start the test timer
    Timer timer;
    timer.start();
    
    //Run the test
    testData.resetPlaybackIndex(0); //Make sure that the test data start at 0
    for(UINT i=0; i<testData.getNumSamples(); i++){
        LabelledClassificationSample sample = testData.getNextSample();
        UINT classLabel = sample.getClassLabel();
        VectorDouble testSample = sample.getSample();
            
        //Pass the test sample through the pipeline
        if( !predict( testSample ) ){
            errorMessage = "test(LabelledContinuousTimeSeriesClassificationData testData) - Prediction Failed! " +  classifier->getLastErrorMessage();
            errorLog << errorMessage << endl;
            return false;
        }
        
        //Update the test metrics
        UINT predictedClassLabel = getPredictedClassLabel();
        
        testClassificationResults[i].classLabel = classLabel;
        testClassificationResults[i].predictedClassLabel = predictedClassLabel;
        testClassificationResults[i].unprocessedPredictedClassLabel = getUnProcessedPredictedClassLabel();
        testClassificationResults[i].maximumLikelihood = getMaximumLikelihood();
        testClassificationResults[i].classLikelihoods = getClassLikelihoods();
        testClassificationResults[i].classDistances = getClassDistances();
        
        /* //TODO - Need to update this!
        if( !updateTestMetrics(classLabel,predictedClassLabel,precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter) ){
            errorLog << "test(LabelledContinuousTimeSeriesClassificationData &testData) - Failed to update test metrics at test sample index: " << i << endl;
            return false;
        }
         */
        
        if( classLabel == predictedClassLabel ) testAccuracy++;
        
        testingLog << "test iteration: " << i;
        testingLog << "\tClassLabel: " << classLabel;
        testingLog << "\tPredictedClassLabel: " << predictedClassLabel;
        testingLog << "\tLikelihood: " << getMaximumLikelihood() << endl;
    }
    
    /* //TODO - Need to update this!
    if( !computeTestMetrics(precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter, testData.getNumSamples()) ){
        errorLog << "test(LabelledContinuousTimeSeriesClassificationData &testData) - Failed to compute test metrics !" << endl;
        return false;
    }
     */
    
    testTime = timer.getMilliSeconds();
    testAccuracy = testAccuracy / double( testData.getNumSamples() ) * 100.0;
    
    testingLog << "Test complete. Total testing time: " << testTime << endl;
    
    return true;
}
    
bool GestureRecognitionPipeline::test(LabelledRegressionData testData){
    
    //Clear any previous test results
    clearTestResults();
    
    //Make sure the classification model has been trained
    if( !trained ){
        errorMessage = "test(LabelledRegressionData testData) - Regressifier is not trained";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Make sure the dimensionality of the test data matches the input vector's dimensions
    if( testData.getNumInputDimensions() != inputVectorDimensions ){
        errorMessage = "test(LabelledRegressionData testData) - The dimensionality of the test data (" + Util::toString(testData.getNumInputDimensions()) + ") does not match that of the input vector dimensions of the pipeline (" + Util::toString(inputVectorDimensions) + ")";
        errorLog << errorMessage << endl;
        return false;
    }
    
    if( !getIsRegressifierSet() ){
        errorMessage = "test(LabelledRegressionData testData) - The regressifier has not been set";
        errorLog << errorMessage << endl;
        return false;
    }
    
    if( regressifier->getNumOutputDimensions() != testData.getNumTargetDimensions() ){
        errorMessage = "test(LabelledRegressionData testData) - The size of the output of the regressifier (" + Util::toString(regressifier->getNumOutputDimensions()) + ") does not match that of the size of the number of target dimensions (" + Util::toString(testData.getNumTargetDimensions()) + ")";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    numTestSamples = testData.getNumSamples();
    
    //Start the test timer
    Timer timer;
    timer.start();
    
    //Run the test
    testSquaredError = 0;
    testRMSError = 0;
    for(UINT i=0; i<testData.getNumSamples(); i++){
        VectorDouble inputVector = testData[i].getInputVector();
        VectorDouble targetVector = testData[i].getTargetVector();
        
        //Pass the test sample through the pipeline
        if( !map( inputVector ) ){
            errorMessage = "test(LabelledRegressionData testData) - Failed to map input vector!";
            errorLog << errorMessage << endl;
            return false;
        }
        
        //Update the RMS error
        double sum = 0;
        VectorDouble regressionData = regressifier->getRegressionData();
        for(UINT j=0; j<targetVector.size(); j++){
            sum += SQR( regressionData[j]-targetVector[j] );
        }

        testSquaredError += sum;
    }
    
    //Compute the test metrics
    testRMSError = sqrt( testSquaredError / double( testData.getNumSamples() ) );

    cout << "TestSquaredError: " << testSquaredError << endl;
    cout << "TestRMSError: " << testRMSError << endl;
    
    testTime = timer.getMilliSeconds();
    
    return true;
}

bool GestureRecognitionPipeline::predict(VectorDouble inputVector){
	
	//Make sure the classification model has been trained
    if( !trained ){
        errorMessage = "predict(VectorDouble inputVector) - The classifier has not been trained";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Make sure the dimensionality of the input vector matches the inputVectorDimensions
    if( inputVector.size() != inputVectorDimensions ){
        errorMessage = "predict(VectorDouble inputVector) - The dimensionality of the input vector (" + Util::toString((int)inputVector.size()) + ") does not match that of the input vector dimensions of the pipeline (" + Util::toString(inputVectorDimensions) + ")";
        errorLog << errorMessage << endl;
        return false;
    }

	if( getIsClassifierSet() ){
        return predict_classifier( inputVector );
    }

	if( getIsRegressifierSet() ){
        return predict_regressifier( inputVector );
    }

    errorMessage = "predict(VectorDouble inputVector) - Neither a classifier or regressifer is not set";
	errorLog << errorMessage << endl;
	return false;
}

bool GestureRecognitionPipeline::predict(MatrixDouble inputMatrix){
	
	//Make sure the classification model has been trained
    if( !trained ){
        errorMessage = "predict(MatrixDouble inputMatrix) - The classifier has not been trained";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Make sure the dimensionality of the input matrix matches the inputVectorDimensions
    if( inputMatrix.getNumCols() != inputVectorDimensions ){
        errorMessage = "predict(MatrixDouble inputMatrix) - The dimensionality of the input matrix (" + Util::toString(inputMatrix.getNumCols()) + ") does not match that of the input vector dimensions of the pipeline (" + Util::toString(inputVectorDimensions) + ")";
        errorLog << errorMessage << endl;
        return false;
    }

	if( !getIsClassifierSet() ){
        errorMessage = "predict(MatrixDouble inputMatrix) - A classifier has not been set";
        errorLog << errorMessage << endl;
		return false;
    }

	predictedClassLabel = 0;
    
    //Update the context module
    predictionModuleIndex = START_OF_PIPELINE;
    
    //Perform any pre-processing
    if( getIsPreProcessingSet() ){
		
        for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
			MatrixDouble tmpMatrix( inputMatrix.getNumRows(), preProcessingModules[moduleIndex]->getNumOutputDimensions() );
			
			for(UINT i=0; i<inputMatrix.getNumRows(); i++){
            	if( !preProcessingModules[moduleIndex]->process( inputMatrix.getRowVector(i) ) ){
                    errorMessage = "predict(MatrixDouble inputMatrix) - Failed to PreProcess Input Matrix. PreProcessingModuleIndex: " + Util::toString(moduleIndex);
                	errorLog << errorMessage << endl;
                	return false;
            	}
            	tmpMatrix.setRowVector( preProcessingModules[moduleIndex]->getProcessedData(), i );
			}
			
			//Update the input matrix with the preprocessed data
			inputMatrix = tmpMatrix;
        }
    }
    
    //Update the context module
    predictionModuleIndex = AFTER_PREPROCESSING;
    //Todo
    
    //Perform any feature extraction
    if( getIsFeatureExtractionSet() ){
	
	    for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
			MatrixDouble tmpMatrix( inputMatrix.getNumRows(), featureExtractionModules[moduleIndex]->getNumOutputDimensions() );
			
			for(UINT i=0; i<inputMatrix.getNumRows(); i++){
            	if( !featureExtractionModules[moduleIndex]->computeFeatures( inputMatrix.getRowVector(i) ) ){
                    errorMessage  =  "predict(MatrixDouble inputMatrix) - Failed to PreProcess Input Matrix. FeatureExtractionModuleIndex: " + Util::toString(moduleIndex);
                	errorLog << errorMessage << endl;
                	return false;
            	}
            	tmpMatrix.setRowVector( featureExtractionModules[moduleIndex]->getFeatureVector(), i );
			}
			
			//Update the input matrix with the preprocessed data
			inputMatrix = tmpMatrix;
        }
    }
    
    //Update the context module
    predictionModuleIndex = AFTER_FEATURE_EXTRACTION;
    //Todo
    
    //Perform the classification
    if( !classifier->predict( inputMatrix ) ){
        errorMessage =  "predict(MatrixDouble inputMatrix) - Prediction Failed! " + classifier->getLastErrorMessage();
        errorLog << errorMessage << endl;
        return false;
    }
    predictedClassLabel = classifier->getPredictedClassLabel();
    
    //Update the context module
    //Todo
    
    //Perform any post processing
    predictionModuleIndex = AFTER_CLASSIFIER;
    if( getIsPostProcessingSet() ){
        
        if( pipelineMode != CLASSIFICATION_MODE){
            errorMessage = "predict_classifier(VectorDouble inputVector) - Pipeline Mode Is Not in CLASSIFICATION_MODE!";
            errorLog << errorMessage << endl;
            return false;
        }
        
        VectorDouble data;
        for(UINT moduleIndex=0; moduleIndex<postProcessingModules.size(); moduleIndex++){
            
            //Select which input we should give the postprocessing module
            if( postProcessingModules[moduleIndex]->getIsPostProcessingInputModePredictedClassLabel() ){
                //Set the input
                data.resize(1);
                data[0] = predictedClassLabel;
                
                //Verify that the input size is OK
                if( data.size() != postProcessingModules[moduleIndex]->getNumInputDimensions() ){
                    errorMessage = "predict_classifier(VectorDouble inputVector) - The size of the data vector (" + Util::toString((int)data.size()) + ") does not match that of the postProcessingModule (" + Util::toString(postProcessingModules[moduleIndex]->getNumInputDimensions()) + ") at the moduleIndex: " + Util::toString(moduleIndex);
                    errorLog << errorMessage <<endl;
                    return false;
                }
                
                //Postprocess the data
                if( !postProcessingModules[moduleIndex]->process( data ) ){
                    errorMessage = "predict_classifier(VectorDouble inputVector) - Failed to post process data. PostProcessing moduleIndex: " + Util::toString(moduleIndex);
                    errorLog << errorMessage <<endl;
                    return false;
                }
                
                //Select which output we should update
                data = postProcessingModules[moduleIndex]->getProcessedData();  
            }
            
            //Select which output we should update
            if( postProcessingModules[moduleIndex]->getIsPostProcessingOutputModePredictedClassLabel() ){
                //Get the processed predicted class label
                data = postProcessingModules[moduleIndex]->getProcessedData(); 
                
                //Verify that the output size is OK
                if( data.size() != 1 ){
                    errorMessage = "predict_classifier(VectorDouble inputVector) - The size of the processed data vector ("+ Util::toString((int)data.size()) + ") from postProcessingModule at the moduleIndex: " + Util::toString(moduleIndex) + " is not equal to 1 even though it is in OutputModePredictedClassLabel!";
                    errorLog << errorMessage << endl;
                    return false;
                }
                
                //Update the predicted class label
                predictedClassLabel = (UINT)data[0];
            }
                  
        }
    } 
    
    //Update the context module
    //TODO
    predictionModuleIndex = END_OF_PIPELINE;

	return true;
}

bool GestureRecognitionPipeline::map(VectorDouble inputVector){
	return predict_regressifier( inputVector );
}

bool GestureRecognitionPipeline::predict_classifier(VectorDouble &inputVector){
    
    predictedClassLabel = 0;
    
    //Update the context module
    predictionModuleIndex = START_OF_PIPELINE;
    if( contextModules[ START_OF_PIPELINE ].size() > 0 ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ START_OF_PIPELINE ].size(); moduleIndex++){
            if( !contextModules[ START_OF_PIPELINE ][moduleIndex]->process( inputVector ) ){
                errorMessage = "predict_classifier(VectorDouble inputVector) - Context Module Failed at START_OF_PIPELINE. ModuleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            if( !contextModules[ START_OF_PIPELINE ][moduleIndex]->getOK() ){
                return true;
            }
            inputVector = contextModules[ START_OF_PIPELINE ][moduleIndex]->getProcessedData();
        }
    }
    
    //Perform any pre-processing
    if( getIsPreProcessingSet() ){
        for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
            if( !preProcessingModules[moduleIndex]->process( inputVector ) ){
                errorMessage = "predict_classifier(VectorDouble inputVector) - Failed to PreProcess Input Vector. PreProcessingModuleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            inputVector = preProcessingModules[moduleIndex]->getProcessedData();
        }
    }
    
    //Update the context module
    predictionModuleIndex = AFTER_PREPROCESSING;
    if( contextModules[ AFTER_PREPROCESSING ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ AFTER_PREPROCESSING ].size(); moduleIndex++){
            if( !contextModules[ AFTER_PREPROCESSING ][moduleIndex]->process( inputVector ) ){
                errorMessage = "predict_classifier(VectorDouble inputVector) - Context Module Failed at AFTER_PREPROCESSING. ModuleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            if( !contextModules[ AFTER_PREPROCESSING ][moduleIndex]->getOK() ){
                predictionModuleIndex = AFTER_PREPROCESSING;
                return false;
            }
            inputVector = contextModules[ AFTER_PREPROCESSING ][moduleIndex]->getProcessedData();
        }
    }
    
    //Perform any feature extraction
    if( getIsFeatureExtractionSet() ){
        for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
            if( !featureExtractionModules[moduleIndex]->computeFeatures( inputVector ) ){
                errorMessage =  "predict_classifier(VectorDouble inputVector) - Failed to compute features from data. FeatureExtractionModuleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            inputVector = featureExtractionModules[moduleIndex]->getFeatureVector();
        }
    }
    
    //Update the context module
    predictionModuleIndex = AFTER_FEATURE_EXTRACTION;
    if( contextModules[ AFTER_FEATURE_EXTRACTION ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ AFTER_FEATURE_EXTRACTION ].size(); moduleIndex++){
            if( !contextModules[ AFTER_FEATURE_EXTRACTION ][moduleIndex]->process( inputVector ) ){
                errorMessage = "predict_classifier(VectorDouble inputVector) - Context Module Failed at AFTER_FEATURE_EXTRACTION. ModuleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            if( !contextModules[ AFTER_FEATURE_EXTRACTION ][moduleIndex]->getOK() ){
                predictionModuleIndex = AFTER_FEATURE_EXTRACTION;
                return false;
            }
            inputVector = contextModules[ AFTER_FEATURE_EXTRACTION ][moduleIndex]->getProcessedData();
        }
    }
    
    //Perform the classification
    if( !classifier->predict(inputVector) ){
        errorMessage = "predict_classifier(VectorDouble inputVector) - Prediction Failed! " + classifier->getLastErrorMessage();
        errorLog << errorMessage << endl;
        return false;
    }
    predictedClassLabel = classifier->getPredictedClassLabel();
    
    //Update the context module
    if( contextModules[ AFTER_CLASSIFIER ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ AFTER_CLASSIFIER ].size(); moduleIndex++){
            if( !contextModules[ AFTER_CLASSIFIER ][moduleIndex]->process( vector<double>(1,predictedClassLabel) ) ){
                errorMessage = "predict_classifier(VectorDouble inputVector) - Context Module Failed at AFTER_CLASSIFIER. ModuleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            if( !contextModules[ AFTER_CLASSIFIER ][moduleIndex]->getOK() ){
                predictionModuleIndex = AFTER_CLASSIFIER;
                return false;
            }
            predictedClassLabel = (UINT)contextModules[ AFTER_CLASSIFIER ][moduleIndex]->getProcessedData()[0];
        }
    }
    
    //Perform any post processing
    predictionModuleIndex = AFTER_CLASSIFIER;
    if( getIsPostProcessingSet() ){
        
        if( pipelineMode != CLASSIFICATION_MODE){
            errorMessage = "predict_classifier(VectorDouble inputVector) - Pipeline Mode Is Not in CLASSIFICATION_MODE!";
            errorLog << errorMessage << endl;
            return false;
        }
        
        VectorDouble data;
        for(UINT moduleIndex=0; moduleIndex<postProcessingModules.size(); moduleIndex++){
            
            //Select which input we should give the postprocessing module
            if( postProcessingModules[moduleIndex]->getIsPostProcessingInputModePredictedClassLabel() ){
                //Set the input
                data.resize(1);
                data[0] = predictedClassLabel;
                
                //Verify that the input size is OK
                if( data.size() != postProcessingModules[moduleIndex]->getNumInputDimensions() ){
                    errorMessage = "predict_classifier(VectorDouble inputVector) - The size of the data vector (" + Util::toString((int)data.size()) + ") does not match that of the postProcessingModule (" + Util::toString(postProcessingModules[moduleIndex]->getNumInputDimensions()) + ") at the moduleIndex: " + Util::toString(moduleIndex);
                    errorLog << errorMessage <<endl;
                    return false;
                }
                
                //Postprocess the data
                if( !postProcessingModules[moduleIndex]->process( data ) ){
                    errorMessage = "predict_classifier(VectorDouble inputVector) - Failed to post process data. PostProcessing moduleIndex: " + Util::toString(moduleIndex);
                    errorLog << errorMessage <<endl;
                    return false;
                }
                
                //Select which output we should update
                data = postProcessingModules[moduleIndex]->getProcessedData();  
            }
            
            //Select which output we should update
            if( postProcessingModules[moduleIndex]->getIsPostProcessingOutputModePredictedClassLabel() ){
                //Get the processed predicted class label
                data = postProcessingModules[moduleIndex]->getProcessedData(); 
                
                //Verify that the output size is OK
                if( data.size() != 1 ){
                    errorMessage =  "predict_classifier(VectorDouble inputVector) - The size of the processed data vector (" + Util::toString((int)data.size()) + ") from postProcessingModule at the moduleIndex: " + Util::toString(moduleIndex) + " is not equal to 1 even though it is in OutputModePredictedClassLabel!";
                    errorLog << errorMessage << endl;
                    return false;
                }
                
                //Update the predicted class label
                predictedClassLabel = (UINT)data[0];
            }
                  
        }
    } 
    
    //Update the context module
    predictionModuleIndex = END_OF_PIPELINE;
    if( contextModules[ END_OF_PIPELINE ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ END_OF_PIPELINE ].size(); moduleIndex++){
            if( !contextModules[ END_OF_PIPELINE ][moduleIndex]->process( vector<double>(1,predictedClassLabel) ) ){
                errorMessage = "predict_classifier(VectorDouble inputVector) - Context Module Failed at END_OF_PIPELINE. ModuleIndex: " + Util::toString( moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            if( !contextModules[ END_OF_PIPELINE ][moduleIndex]->getOK() ){
                predictionModuleIndex = END_OF_PIPELINE;
                return false;
            }
            predictedClassLabel = (UINT)contextModules[ END_OF_PIPELINE ][moduleIndex]->getProcessedData()[0];
        }
    }
    
    return true;
}
    
bool GestureRecognitionPipeline::predict_regressifier(VectorDouble &inputVector){
    
    //Update the context module
    predictionModuleIndex = START_OF_PIPELINE;
    if( contextModules[ START_OF_PIPELINE ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ START_OF_PIPELINE ].size(); moduleIndex++){
            if( !contextModules[ START_OF_PIPELINE ][moduleIndex]->process( inputVector ) ){
                errorMessage = "predict_regressifier(VectorDouble inputVector) - Context Module Failed at START_OF_PIPELINE. ModuleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            if( !contextModules[ START_OF_PIPELINE ][moduleIndex]->getOK() ){
                return true;
            }
            inputVector = contextModules[ START_OF_PIPELINE ][moduleIndex]->getProcessedData();
        }
    }
    
    //Perform any pre-processing
    if( getIsPreProcessingSet() ){
        for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
            if( !preProcessingModules[moduleIndex]->process( inputVector ) ){
                errorMessage = "predict_regressifier(VectorDouble inputVector) - Failed to PreProcess Input Vector. PreProcessingModuleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            inputVector = preProcessingModules[moduleIndex]->getProcessedData();
        }
    }
    
    //Update the context module
    predictionModuleIndex = AFTER_PREPROCESSING;
    if( contextModules[ AFTER_PREPROCESSING ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ AFTER_PREPROCESSING ].size(); moduleIndex++){
            if( !contextModules[ AFTER_PREPROCESSING ][moduleIndex]->process( inputVector ) ){
                errorMessage = "predict_regressifier(VectorDouble inputVector) - Context Module Failed at AFTER_PREPROCESSING. ModuleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            if( !contextModules[ AFTER_PREPROCESSING ][moduleIndex]->getOK() ){
                predictionModuleIndex = AFTER_PREPROCESSING;
                return false;
            }
            inputVector = contextModules[ AFTER_PREPROCESSING ][moduleIndex]->getProcessedData();
        }
    }
    
    //Perform any feature extraction
    if( getIsFeatureExtractionSet() ){
        for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
            if( !featureExtractionModules[moduleIndex]->computeFeatures( inputVector ) ){
                errorMessage =  "predict_regressifier(VectorDouble inputVector) - Failed to compute features from data. FeatureExtractionModuleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            inputVector = featureExtractionModules[moduleIndex]->getFeatureVector();
        }
    }
    
    //Update the context module
    predictionModuleIndex = AFTER_FEATURE_EXTRACTION;
    if( contextModules[ AFTER_FEATURE_EXTRACTION ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ AFTER_FEATURE_EXTRACTION ].size(); moduleIndex++){
            if( !contextModules[ AFTER_FEATURE_EXTRACTION ][moduleIndex]->process( inputVector ) ){
                errorMessage =  "predict_regressifier(VectorDouble inputVector) - Context Module Failed at AFTER_FEATURE_EXTRACTION. ModuleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            if( !contextModules[ AFTER_FEATURE_EXTRACTION ][moduleIndex]->getOK() ){
                predictionModuleIndex = AFTER_FEATURE_EXTRACTION;
                return false;
            }
            inputVector = contextModules[ AFTER_FEATURE_EXTRACTION ][moduleIndex]->getProcessedData();
        }
    }
    
    //Perform the regression
    if( !regressifier->predict(inputVector) ){
        errorMessage = "predict_regressifier(VectorDouble inputVector) - Prediction Failed! " + regressifier->getLastErrorMessage();
        errorLog << errorMessage << endl;
        return false;
    }
    regressionData = regressifier->getRegressionData();
    
    //Update the context module
    if( contextModules[ AFTER_CLASSIFIER ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ AFTER_CLASSIFIER ].size(); moduleIndex++){
            if( !contextModules[ AFTER_CLASSIFIER ][moduleIndex]->process( regressionData ) ){
                errorMessage = "predict_regressifier(VectorDouble inputVector) - Context Module Failed at AFTER_CLASSIFIER. ModuleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            if( !contextModules[ AFTER_CLASSIFIER ][moduleIndex]->getOK() ){
                predictionModuleIndex = AFTER_CLASSIFIER;
                return false;
            }
            regressionData = contextModules[ AFTER_CLASSIFIER ][moduleIndex]->getProcessedData();
        }
    }
    
    //Perform any post processing
    predictionModuleIndex = AFTER_CLASSIFIER;
    if( getIsPostProcessingSet() ){
        
        if( pipelineMode != REGRESSION_MODE ){
            errorMessage = "predict_regressifier(VectorDouble inputVector) - Pipeline Mode Is Not In RegressionMode!";
            errorLog << errorMessage << endl;
            return false;
        }
          
        for(UINT moduleIndex=0; moduleIndex<postProcessingModules.size(); moduleIndex++){
            if( regressionData.size() != postProcessingModules[moduleIndex]->getNumInputDimensions() ){
                errorMessage = "predict_regressifier(VectorDouble inputVector) - The size of the regression vector (" + Util::toString((int)regressionData.size()) + ") does not match that of the postProcessingModule (" + Util::toString(postProcessingModules[moduleIndex]->getNumInputDimensions()) + ") at the moduleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage <<endl;
                return false;
            }
            
            if( !postProcessingModules[moduleIndex]->process( regressionData ) ){
                errorMessage = "predict_regressifier(VectorDouble inputVector) - Failed to post process data. PostProcessing moduleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage <<endl;
                return false;
            }
            regressionData = postProcessingModules[moduleIndex]->getProcessedData();        
        }
        
    } 
    
    //Update the context module
    predictionModuleIndex = END_OF_PIPELINE;
    if( contextModules[ END_OF_PIPELINE ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ END_OF_PIPELINE ].size(); moduleIndex++){
            if( !contextModules[ END_OF_PIPELINE ][moduleIndex]->process( inputVector ) ){
                errorMessage = "predict_regressifier(VectorDouble inputVector) - Context Module Failed at END_OF_PIPELINE. ModuleIndex: " + Util::toString( moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            if( !contextModules[ END_OF_PIPELINE ][moduleIndex]->getOK() ){
                predictionModuleIndex = END_OF_PIPELINE;
                return false;
            }
            regressionData = contextModules[ END_OF_PIPELINE ][moduleIndex]->getProcessedData();
        }
    }
    
    return true;
}
    
bool GestureRecognitionPipeline::reset(){
    
    //Reset any pre processing
    if( getIsPreProcessingSet() ){
        for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
            if( !preProcessingModules[ moduleIndex ]->reset() ){
                errorMessage = "Failed To Reset PreProcessingModule " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
        }
    }
    
    //Reset any feature extraction
    if( getIsFeatureExtractionSet() ){
        for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
            if( !featureExtractionModules[ moduleIndex ]->reset() ){
                errorMessage = "Failed To Reset FeatureExtractionModule " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
        }
    }
    
    //Reset the classifier
    if( getIsClassifierSet() ){
        if( !classifier->reset() ){
            errorMessage = "Failed To Reset Classifier! " + classifier->getLastErrorMessage();
            errorLog << errorMessage << endl;
            return false;
        }
    }
    
    //Reset the regressiier
    if( getIsRegressifierSet() ){
        if( !regressifier->reset() ){
            errorMessage = "Failed To Reset Regressifier! " + regressifier->getLastErrorMessage();
            errorLog << errorMessage << endl;
            return false;
        }
    }
    
    //Reset any post processing
    if( getIsPostProcessingSet() ){
        for(UINT moduleIndex=0; moduleIndex<postProcessingModules.size(); moduleIndex++){
            if( !postProcessingModules[ moduleIndex ]->reset() ){
                errorMessage = "Failed To Reset PostProcessingModule " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
        }
    } 
    
    return true;
}
    
bool GestureRecognitionPipeline::savePipelineToFile(string filename){
    
    if( !initialized ){
        errorMessage =  "Failed to write pipeline to file as the pipeline has not been initialized yet!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    fstream file;
    
    file.open(filename.c_str(), iostream::out );
    
    if( !file.is_open() ){
        errorMessage = "Failed to open file with filename: " + filename;
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Write the pipeline header info
    file << "GRT_PIPELINE_FILE_V1.0\n";
    file << "PipelineMode: " << getPipelineModeAsString() << endl;
    file << "NumPreprocessingModules: " << getNumPreProcessingModules() << endl;
    file << "NumFeatureExtractionModules: " << getNumFeatureExtractionModules() << endl;
    file << "NumPostprocessingModules: " << getNumPostProcessingModules() << endl;
    file << "Trained: " << getTrained() << endl;
    
    //Write the module datatype names
    file << "PreProcessingModuleDatatypes:";
    for(UINT i=0; i<getNumPreProcessingModules(); i++){
        file << "\t" << preProcessingModules[i]->getPreProcessingType();
    }
    file << endl;
    
    file << "FeatureExtractionModuleDatatypes:";
    for(UINT i=0; i<getNumFeatureExtractionModules(); i++){
        file << "\t" << featureExtractionModules[i]->getFeatureExtractionType();
    }
    file << endl;
    
    switch( pipelineMode ){
        case PIPELINE_MODE_NOT_SET:
            break;
        case CLASSIFICATION_MODE:
            if( getIsClassifierSet() ) file << "ClassificationModuleDatatype:\t" << classifier->getClassifierType() << endl;
            else file << "ClassificationModuleDatatype:\tCLASSIFIER_NOT_SET" << endl;
            break;
        case REGRESSION_MODE:
            if( getIsRegressifierSet() ) file << "RegressionnModuleDatatype:\t" << regressifier->getRegressifierType() << endl;
            else file << "RegressionnModuleDatatype:\tREGRESSIFIER_NOT_SET" << endl;
            break;
        default:
            break;
    }
    
    file << "PostProcessingModuleDatatypes:";
    for(UINT i=0; i<getNumPostProcessingModules(); i++){
        file << "\t" << postProcessingModules[i]->getPostProcessingType();
    }
    file << endl;
    
    //Write the preprocessing module data to the file
    for(UINT i=0; i<getNumPreProcessingModules(); i++){
        file << "PreProcessingModule_" << Util::intToString(i+1) << endl;
        if( !preProcessingModules[i]->saveSettingsToFile( file ) ){
            errorMessage = "Failed to write preprocessing module " + Util::toString(i) + " settings to file!";
            errorLog << errorMessage << endl;
            file.close();
            return false;
        }
    }
    
    //Write the feature extraction module data to the file
    for(UINT i=0; i<getNumFeatureExtractionModules(); i++){
        file << "FeatureExtractionModule_" << Util::intToString(i+1) << endl;
        if( !featureExtractionModules[i]->saveSettingsToFile( file ) ){
            errorMessage = "Failed to write feature extraction module " + Util::toString(i) + " settings to file!";
            errorLog << errorMessage << endl;
            file.close();
            return false;
        }
    }
    
    switch( pipelineMode ){
        case PIPELINE_MODE_NOT_SET:
            break;
        case CLASSIFICATION_MODE:
            if( getIsClassifierSet() ){
                if( !classifier->saveModelToFile( file ) ){
                    errorMessage = "Failed to write classifier model to file!";
                    errorLog << errorMessage << endl;
                    file.close();
                    return false;
                }
            }
            break;
        case REGRESSION_MODE:
            if( getIsRegressifierSet() ){
                if( !regressifier->saveModelToFile( file ) ){
                    errorMessage = "Failed to write regressifier model to file!";
                    errorLog << errorMessage << endl;
                    file.close();
                    return false;
                }
            }
            break;
        default:
            break;
    }
    
    //Write the post processing module data to the file
    for(UINT i=0; i<getNumPostProcessingModules(); i++){
        file << "PostProcessingModule_" << Util::intToString(i+1) << endl;
        if( !postProcessingModules[i]->saveSettingsToFile( file ) ){
            errorMessage = "Failed to write post processing module " + Util::toString(i) + " settings to file!";
            errorLog << errorMessage << endl;
            file.close();
            return false;
        }
    }
    
    //Close the file
    file.close();
    
    return true;
}

bool GestureRecognitionPipeline::loadPipelineFromFile(string filename){
    
    fstream file;

	//Clear any previous setup
	clearAll();
    
    file.open(filename.c_str(), iostream::in );
    
    if( !file.is_open() ){
        errorMessage = "loadPipelineFromFile(string filename) - Failed to open file with filename: " + filename;
        errorLog << errorMessage << endl;
        return false;
    }

	string word;
	
	//Load the file header
	file >> word;
	if( word != "GRT_PIPELINE_FILE_V1.0" ){
        errorMessage = "loadPipelineFromFile(string filename) - Failed to read file header";
		errorLog << errorMessage << endl;
		file.close();
        return false;
	}
	
	//Load the pipeline mode
	file >> word;
	if( word != "PipelineMode:" ){
        errorMessage = "loadPipelineFromFile(string filename) - Failed to read PipelineMode";
		errorLog << errorMessage << endl;
		file.close();
        return false;
	}
	file >> word;
	pipelineMode = getPipelineModeFromString(word);
	
	//Load the NumPreprocessingModules
	file >> word;
	if( word != "NumPreprocessingModules:" ){
        errorMessage = "loadPipelineFromFile(string filename) - Failed to read NumPreprocessingModules";
		errorLog << errorMessage << endl;
		file.close();
        return false;
	}
	unsigned int numPreprocessingModules;
	file >> numPreprocessingModules;
	
	//Load the NumFeatureExtractionModules
	file >> word;
	if( word != "NumFeatureExtractionModules:" ){
        errorMessage = "loadPipelineFromFile(string filename) - Failed to read NumFeatureExtractionModules";
		errorLog << errorMessage << endl;
		file.close();
        return false;
	}
	unsigned int numFeatureExtractionModules;
	file >> numFeatureExtractionModules;
	
	//Load the NumPostprocessingModules
	file >> word;
	if( word != "NumPostprocessingModules:" ){
        errorMessage = "loadPipelineFromFile(string filename) - Failed to read NumPostprocessingModules";
		errorLog << errorMessage << endl;
		file.close();
        return false;
	}
	unsigned int numPostprocessingModules;
	file >> numPostprocessingModules;
	
	//Load if the pipeline has been trained
	file >> word;
	if( word != "Trained:" ){
        errorMessage = "loadPipelineFromFile(string filename) - Failed to read Trained";
		errorLog << errorMessage << endl;
		file.close();
        return false;
	}
	file >> trained;
	
	//Resize the modules
	if( numPreprocessingModules > 0 ) preProcessingModules.resize(numPreprocessingModules,NULL);
	if( numFeatureExtractionModules > 0 ) featureExtractionModules.resize(numFeatureExtractionModules,NULL);
	if( numPostprocessingModules > 0 ) postProcessingModules.resize(numPostprocessingModules,NULL);
	
	//Load the preprocessing module datatypes and initialize the modules
	file >> word;
	if( word != "PreProcessingModuleDatatypes:" ){
        errorMessage = "loadPipelineFromFile(string filename) - Failed to read PreProcessingModuleDatatypes";
		errorLog << errorMessage << endl;
		file.close();
        return false;
	}
    for(UINT i=0; i<numPreprocessingModules; i++){
		file >> word;
		preProcessingModules[i] = PreProcessing::createInstanceFromString( word );
		if( preProcessingModules[i] == NULL ){
            errorMessage = "loadPipelineFromFile(string filename) - Failed to create preprocessing instance from string: " + word;
			errorLog << errorMessage << endl;
			file.close();
	        return false;
		}
    }

	//Load the feature extraction module datatypes and initialize the modules
	file >> word;
	if( word != "FeatureExtractionModuleDatatypes:" ){
        errorMessage = "loadPipelineFromFile(string filename) - Failed to read FeatureExtractionModuleDatatypes";
		errorLog << errorMessage << endl;
		file.close();
        return false;
	}
    for(UINT i=0; i<numFeatureExtractionModules; i++){
		file >> word;
		featureExtractionModules[i] = FeatureExtraction::createInstanceFromString( word );
		if( featureExtractionModules[i] == NULL ){
            errorMessage = "loadPipelineFromFile(string filename) - Failed to create feature extraction instance from string: " + word;
			errorLog << errorMessage << endl;
			file.close();
	        return false;
		}
    }

	switch( pipelineMode ){
        case PIPELINE_MODE_NOT_SET:
            break;
        case CLASSIFICATION_MODE:
			file >> word;
			if( word != "ClassificationModuleDatatype:" ){
                errorMessage = "loadPipelineFromFile(string filename) - Failed to read ClassificationModuleDatatype" ;
				errorLog << errorMessage << endl;
				file.close();
		        return false;
			}
			//Load the classifier type
			file >> word;
			
			//Initialize the classifier
			classifier = Classifier::createInstanceFromString( word );
			if( classifier == NULL ){
                errorMessage = "loadPipelineFromFile(string filename) - Failed to create classifier instance from string: " + word;
				errorLog << errorMessage << endl;
				file.close();
		        return false;
			}
            break;
        case REGRESSION_MODE:
			file >> word;
			if( word != "RegressionnModuleDatatype:" ){
                errorMessage = "loadPipelineFromFile(string filename) - Failed to read RegressionnModuleDatatype";
				errorLog << errorMessage << endl;
				file.close();
		        return false;
			}
			//Load the regressifier type
			file >> word;
			
			//Initialize the regressifier
			regressifier = Regressifier::createInstanceFromString( word );
			if( regressifier == NULL ){
                errorMessage = "loadPipelineFromFile(string filename) - Failed to create regressifier instance from string: " + word;
				errorLog << errorMessage << endl;
				file.close();
		        return false;
			}
            break;
        default:
            break;
    }

	//Load the post processing module datatypes and initialize the modules
	file >> word;
	if( word != "PostProcessingModuleDatatypes:" ){
        errorMessage = "loadPipelineFromFile(string filename) - Failed to read PostProcessingModuleDatatypes";
		errorLog << errorMessage << endl;
		file.close();
	    return false;
	}
	for(UINT i=0; i<numPostprocessingModules; i++){
		file >> word;
		postProcessingModules[i] = PostProcessing::createInstanceFromString( word );
	}
	
	//Load the preprocessing module data from the file
    for(UINT i=0; i<numPreprocessingModules; i++){
		//Load the preprocessing module header
		file >> word;
        if( !preProcessingModules[i]->loadSettingsFromFile( file ) ){
            errorMessage = "Failed to load preprocessing module " + Util::toString(i) + " settings from file!";
            errorLog << errorMessage << endl;
            file.close();
            return false;
        }
    }

	//Load the feature extraction module data from the file
	for(UINT i=0; i<numFeatureExtractionModules; i++){
		//Load the feature extraction module header
		file >> word;
	    if( !featureExtractionModules[i]->loadSettingsFromFile( file ) ){
            errorMessage = "Failed to load feature extraction module " + Util::toString(i) + " settings from file!";
	        errorLog << errorMessage << endl;
	        file.close();
	        return false;
	    }
	}
	
	//Load the classifier or regressifer data
	switch( pipelineMode ){
        case PIPELINE_MODE_NOT_SET:
            break;
        case CLASSIFICATION_MODE:
               if( !classifier->loadModelFromFile( file ) ){
                   errorMessage = "Failed to load classifier model from file!";
                   errorLog << errorMessage << endl;
                   file.close();
                   return false;
               }
            break;
        case REGRESSION_MODE:
               if( !regressifier->loadModelFromFile( file ) ){
                   errorMessage = "Failed to load regressifier model from file!";
                   errorLog << errorMessage << endl;
                   file.close();
                   return false;
               }
            break;
        default:
            break;
    }

	//Load the post processing module data from the file
    for(UINT i=0; i<numPostprocessingModules; i++){
		//Load the post processing module header
		file >> word;
        if( !postProcessingModules[i]->loadSettingsFromFile( file ) ){
            errorMessage = "Failed to load post processing module " + Util::toString(i) + " settings from file!";
            errorLog << errorMessage << endl;
            file.close();
            return false;
        }
    }
    
    //Close the file
    file.close();
    
    //Set the expected input vector size
    inputVectorDimensions = 0;
    
    if( numPreprocessingModules > 0 ){
        inputVectorDimensions = preProcessingModules[0]->getNumInputDimensions();
    }else{
        if( numFeatureExtractionModules > 0 ){
            inputVectorDimensions = featureExtractionModules[0]->getNumInputDimensions();
        }else{
            switch( pipelineMode ){
                case PIPELINE_MODE_NOT_SET:
                    break;
                case CLASSIFICATION_MODE:
                    inputVectorDimensions = classifier->getNumInputFeatures();
                    break;
                case REGRESSION_MODE:
                    inputVectorDimensions = regressifier->getNumInputFeatures();
                    break;
                default:
                    break;
            }
        }
    }
    
    return true;
}
    
bool GestureRecognitionPipeline::preProcessData(VectorDouble inputVector,bool computeFeatures){
    
    if( getIsPreProcessingSet() ){
        for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
            
            if( inputVector.size() != preProcessingModules[ moduleIndex ]->getNumInputDimensions() ){
                errorMessage = "preProcessData(VectorDouble inputVector) - The size of the input vector (" + Util::toString(preProcessingModules[ moduleIndex ]->getNumInputDimensions()) + ") does not match that of the PreProcessing Module at moduleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            
            if( !preProcessingModules[ moduleIndex ]->process( inputVector ) ){
                errorMessage = "preProcessData(VectorDouble inputVector) - Failed To PreProcess Input Vector. PreProcessing moduleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            inputVector = preProcessingModules[ moduleIndex ]->getProcessedData();
        }
    }
    
    //Perform any feature extraction
    if( getIsFeatureExtractionSet() && computeFeatures ){
        for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
            if( inputVector.size() != featureExtractionModules[ moduleIndex ]->getNumInputDimensions() ){
                
                errorMessage = "preProcessData(VectorDouble inputVector) - The size of the input vector (" + Util::toString(featureExtractionModules[ moduleIndex ]->getNumInputDimensions()) + ") does not match that of the FeatureExtraction Module at moduleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            
            if( !featureExtractionModules[ moduleIndex ]->computeFeatures( inputVector ) ){
                errorMessage =  "preProcessData(VectorDouble inputVector) - Failed To Compute Features from Input Vector. FeatureExtraction moduleIndex: " + Util::toString(moduleIndex);
                errorLog << errorMessage << endl;
                return false;
            }
            inputVector = featureExtractionModules[ moduleIndex ]->getFeatureVector();
        }
    }
    
    return true;
}
 
/////////////////////////////// GETTERS ///////////////////////////////
bool GestureRecognitionPipeline::getIsInitialized() const{ 
    return initialized; 
}
    
bool GestureRecognitionPipeline::getTrained() const{ 
    return trained;
}
    
bool GestureRecognitionPipeline::getIsPreProcessingSet() const{ 
    return preProcessingModules.size() > 0; 
} 
    
bool GestureRecognitionPipeline::getIsFeatureExtractionSet() const{ 
    return featureExtractionModules.size() > 0; 
}
    
bool GestureRecognitionPipeline::getIsClassifierSet() const{ 
    return (classifier!=NULL); 
}
    
bool GestureRecognitionPipeline::getIsRegressifierSet() const{ 
    return (regressifier!=NULL); 
}
    
bool GestureRecognitionPipeline::getIsPostProcessingSet() const{ 
    return postProcessingModules.size() > 0; 
}
    
bool GestureRecognitionPipeline::getIsContextSet() const{ 
    for(UINT i=0; i<NUM_CONTEXT_LEVELS; i++){
        if( contextModules[i].size() > 0 ) return true;
    }
    return false;
}
    
bool GestureRecognitionPipeline::getIsPipelineModeSet() const{ 
    return (pipelineMode!=PIPELINE_MODE_NOT_SET); 
}
    
bool GestureRecognitionPipeline::getIsPipelineInClassificationMode() const{ 
    return (pipelineMode==CLASSIFICATION_MODE); 
}
    
bool GestureRecognitionPipeline::getIsPipelineInRegressionMode() const{ 
    return pipelineMode==REGRESSION_MODE; 
}

UINT GestureRecognitionPipeline::getInputVectorDimensionsSize() const{ 
    
    if( getIsPreProcessingSet() ){
        return preProcessingModules[0]->getNumInputDimensions();
    }
    
    if( getIsFeatureExtractionSet() ){
        return featureExtractionModules[0]->getNumInputDimensions();
    }
    
    if( getIsPipelineInClassificationMode() && getIsClassifierSet() ){
        return classifier->getNumInputFeatures();
    }
    if( getIsPipelineInRegressionMode() && getIsRegressifierSet() ){
        return regressifier->getNumInputFeatures(); 
    }
    return 0; 
}
    
UINT GestureRecognitionPipeline::getOutputVectorDimensionsSize() const{ 
    if( getIsClassifierSet() ) return 1;    //The output of the pipeline for classification will always be 1
    if( getIsRegressifierSet() ){
        return regressifier->getNumOutputDimensions();  
    }
    return 0;
}
    
UINT GestureRecognitionPipeline::getNumClassesInModel() const{ 
    return getNumClasses();
}
    
UINT GestureRecognitionPipeline::getNumClasses() const{
    return (getIsClassifierSet() ? classifier->getNumClasses() : 0); 
}
    
UINT GestureRecognitionPipeline::getNumPreProcessingModules() const{ 
    return (UINT)preProcessingModules.size(); 
}
    
UINT GestureRecognitionPipeline::getNumFeatureExtractionModules() const{ 
    return (UINT)featureExtractionModules.size(); 
}
    
UINT GestureRecognitionPipeline::getNumPostProcessingModules() const{ 
    return (UINT)postProcessingModules.size(); 
}
    
UINT GestureRecognitionPipeline::getPredictionModuleIndexPosition() const{ 
    return predictionModuleIndex; 
}
    
UINT GestureRecognitionPipeline::getPredictedClassLabel() const{ 
    return (getIsClassifierSet() ? predictedClassLabel : 0); 
}
    
UINT GestureRecognitionPipeline::getUnProcessedPredictedClassLabel() const{ 
    return (getIsClassifierSet() ? classifier->getPredictedClassLabel() : 0); 
}

double GestureRecognitionPipeline::getMaximumLikelihood() const{ 
    return (getIsClassifierSet() ? classifier->getMaximumLikelihood() : 0); 
}
    
double GestureRecognitionPipeline::getCrossValidationAccuracy() const{ 
    return (getIsClassifierSet()||getIsRegressifierSet() ? testAccuracy : 0);
}
    
double GestureRecognitionPipeline::getTestAccuracy() const{ 
    return testAccuracy; 
}
    
double GestureRecognitionPipeline::getTestRMSError() const{ 
    return testRMSError; 
}
    
double GestureRecognitionPipeline::getTestFMeasure(UINT classLabel) const{
    
    if( !getIsClassifierSet() ) return -1;
    if( getClassLabels().size() != testFMeasure.size() ) return -1;
    
    for(UINT i=0; i<testFMeasure.size(); i++){
        if( getClassLabels()[i] == classLabel ){
            return testFMeasure[i];
        }
    }
    return -1;
}

double GestureRecognitionPipeline::getTestPrecision(UINT classLabel) const{
    
    if( !getIsClassifierSet() ) return -1;
    if( getClassLabels().size() != testPrecision.size() ) return -1;
    
    for(UINT i=0; i<testPrecision.size(); i++){
        if( getClassLabels()[i] == classLabel ){
            return testPrecision[i];
        }
    }
    return -1;
}

double GestureRecognitionPipeline::getTestRecall(UINT classLabel) const{
    
    if( !getIsClassifierSet() ) return -1;
    if( getClassLabels().size() != testRecall.size() ) return -1;
    
    for(UINT i=0; i<testRecall.size(); i++){
        if( getClassLabels()[i] == classLabel ){
            return testRecall[i];
        }
    }
    return -1;
}

double GestureRecognitionPipeline::getTestRejectionPrecision() const{ 
    return testRejectionPrecision; 
}
    
double GestureRecognitionPipeline::getTestRejectionRecall() const{ 
    return testRejectionRecall; 
}
    
double GestureRecognitionPipeline::getTestTime() const{
    return testTime;
}

double GestureRecognitionPipeline::getTrainingTime() const{
    return trainingTime;
}
    
MatrixDouble GestureRecognitionPipeline::getTestConfusionMatrix() const{ 
    return testConfusionMatrix; 
}
    
TestResult GestureRecognitionPipeline::getTestResults() const {
    TestResult testResults;
    testResults.numTrainingSamples = numTrainingSamples;
    testResults.numTestSamples = numTestSamples;
    testResults.accuracy = testAccuracy;
    testResults.rmsError = testRMSError;
    testResults.totalSquaredError = testSquaredError;
    testResults.trainingTime = trainingTime;
    testResults.testTime = testTime;
    testResults.rejectionPrecision = testRejectionPrecision;
    testResults.rejectionRecall = testRejectionRecall;
    testResults.precision = testPrecision;
    testResults.recall = testRecall;
    testResults.fMeasure = testFMeasure;
    testResults.confusionMatrix = testConfusionMatrix;
    return testResults;
}
    
VectorDouble GestureRecognitionPipeline::getTestPrecision() const{
    return testPrecision;
}

VectorDouble GestureRecognitionPipeline::getTestRecall() const{
    return testRecall;
}

VectorDouble GestureRecognitionPipeline::getTestFMeasure() const{
    return testFMeasure;
}

VectorDouble GestureRecognitionPipeline::getClassLikelihoods() const{ 
    if( getIsClassifierSet() ){ return classifier->getClassLikelihoods(); }
    else{ return VectorDouble(); } 
}

VectorDouble GestureRecognitionPipeline::getClassDistances() const{ 
    if( getIsClassifierSet() ){ return classifier->getClassDistances(); }
    else{ return VectorDouble(); } 
}

VectorDouble GestureRecognitionPipeline::getNullRejectionThresholds() const{
    if( getIsClassifierSet() ){ return classifier->getNullRejectionThresholds(); }
    else{ return VectorDouble(); } 
}

VectorDouble GestureRecognitionPipeline::getRegressionData() const{ 
    if( getIsRegressifierSet() ){
        if( getIsPostProcessingSet() ){ 
            return postProcessingModules[ postProcessingModules.size()-1 ]->getProcessedData(); 
        }
        return regressifier->getRegressionData();
    }
    return VectorDouble();
}

VectorDouble GestureRecognitionPipeline::getUnProcessedRegressionData() const{ 
    if( getIsRegressifierSet() ) {
        return regressifier->getRegressionData();
    }
    return VectorDouble();
}
    
VectorDouble GestureRecognitionPipeline::getPreProcessedData() const{
    if( getIsPreProcessingSet() ){ 
        return preProcessingModules[ preProcessingModules.size()-1 ]->getProcessedData(); 
    }
    return VectorDouble();
}

VectorDouble GestureRecognitionPipeline::getPreProcessedData(UINT moduleIndex) const{
    if( getIsPreProcessingSet() ){ 
        if( moduleIndex < preProcessingModules.size() ){
            return preProcessingModules[ moduleIndex ]->getProcessedData(); 
        }
    }
    return VectorDouble();
}

VectorDouble GestureRecognitionPipeline::getFeatureExtractionData() const{
    if( getIsFeatureExtractionSet() ){ 
        return featureExtractionModules[ featureExtractionModules.size()-1 ]->getFeatureVector(); 
    }
    return VectorDouble();
}
    
VectorDouble GestureRecognitionPipeline::getFeatureExtractionData(UINT moduleIndex) const{
    if( getIsFeatureExtractionSet() ){ 
        if( moduleIndex < featureExtractionModules.size() ){
            return featureExtractionModules[ moduleIndex ]->getFeatureVector(); 
        }
    }
    return VectorDouble();
}
    
vector< UINT > GestureRecognitionPipeline::getClassLabels() const{ 
    if( trained && getIsClassifierSet() )
        return classifier->getClassLabels(); 
    return vector< UINT>(); 
}
    
vector< ClassificationResult > GestureRecognitionPipeline::getTestClassificationResults() const{
    return testClassificationResults;
}
    
PreProcessing* GestureRecognitionPipeline::getPreProcessingModule(UINT moduleIndex){ 
    if( moduleIndex < preProcessingModules.size() ){
        return preProcessingModules[ moduleIndex ];
    }
    return NULL;
}
    
FeatureExtraction* GestureRecognitionPipeline::getFeatureExtractionModule(UINT moduleIndex){ 
    if( moduleIndex < featureExtractionModules.size() ){
        return featureExtractionModules[ moduleIndex ];
    }
    return NULL;
}
    
Classifier* GestureRecognitionPipeline::getClassifier(){
    return classifier; 
}
    
Regressifier* GestureRecognitionPipeline::getRegressifier(){ 
    return regressifier; 
}
    
PostProcessing* GestureRecognitionPipeline::getPostProcessingModule(UINT moduleIndex){ 
    if( moduleIndex < postProcessingModules.size() ){
        return postProcessingModules[ moduleIndex ];
    }
    return NULL;
}
    
Context* GestureRecognitionPipeline::getContextModule(UINT contextLevel,UINT moduleIndex){ 
    if( contextLevel < contextModules.size() ){
        if( moduleIndex < contextModules[ contextLevel ].size() ){
            return contextModules[ contextLevel ][ moduleIndex ];
        }
    }
    return NULL;
}
    
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
///////////////////////////////////////////                SETTERS                    ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    
bool GestureRecognitionPipeline::addPreProcessingModule(const PreProcessing &preProcessingModule,UINT insertIndex){
    
    //Validate the insertIndex is valid
    if( insertIndex != INSERT_AT_END_INDEX && insertIndex >= preProcessingModules.size() ){
        errorMessage = "addPreProcessingModule(const PreProcessing &preProcessingModule) - Invalid insertIndex value!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Create a new instance of the preProcessing and then clone the values across from the reference preProcessing
    PreProcessing *newInstance = preProcessingModule.createNewInstance();
    
    //Verify that the clone was successful
    if( !newInstance->clone( &preProcessingModule ) ){
        delete newInstance;
        newInstance = NULL;
        errorMessage = "addPreProcessingModule(const PreProcessing &preProcessingModule) - PreProcessing Module Not Set!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Add the new instance to the preProcessingModules
    vector< PreProcessing* >::iterator iter = preProcessingModules.begin();
    
    if( insertIndex == INSERT_AT_END_INDEX ) iter = preProcessingModules.end(); 
    else iter = preProcessingModules.begin() + insertIndex;
    
    preProcessingModules.insert(iter, newInstance);

	//The pipeline has been changed, so flag that the pipeline is no longer trained
	trained = false;
    
    return true;
}

bool GestureRecognitionPipeline::setPreProcessingModule(const PreProcessing &preProcessingModule){
	removeAllPreProcessingModules();
	return addPreProcessingModule( preProcessingModule );
}

bool GestureRecognitionPipeline::addFeatureExtractionModule(const FeatureExtraction &featureExtractionModule,UINT insertIndex){
    
    //Validate the insertIndex is valid
    if( insertIndex != INSERT_AT_END_INDEX && insertIndex >= featureExtractionModules.size() ){
        errorMessage = "addFeatureExtractionModule(const FeatureExtraction &featureExtractionModule) - Invalid insertIndex value!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Create a new instance of the preProcessing and then clone the values across from the reference preProcessing
    FeatureExtraction *newInstance = featureExtractionModule.createNewInstance();
    
    //Verify that the clone was successful
    if( !newInstance->clone( &featureExtractionModule ) ){
        delete newInstance;
        newInstance = NULL;
        errorMessage = "addFeatureExtractionModule(const FeatureExtraction &featureExtractionModule - FeatureExtraction Module Not Set!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Add the new instance to the preProcessingModules
    vector< FeatureExtraction* >::iterator iter = featureExtractionModules.begin();
    
    if( insertIndex == INSERT_AT_END_INDEX ) iter = featureExtractionModules.end(); 
    else iter = featureExtractionModules.begin() + insertIndex;
    
    featureExtractionModules.insert(iter, newInstance);

	//The pipeline has been changed, so flag that the pipeline is no longer trained
	trained = false;
    
    return true;
}

bool GestureRecognitionPipeline::setFeatureExtractionModule(const FeatureExtraction &featureExtractionModule){
	removeAllFeatureExtractionModules();
	return addFeatureExtractionModule( featureExtractionModule );
}

bool GestureRecognitionPipeline::setClassifier(const Classifier &classifier){
    //Delete any previous classifier and regressifier
    deleteClassifier();
    deleteRegressifier();
    
    //Create a new instance of the classifier and then clone the values across from the reference classifier
    this->classifier = classifier.createNewInstance();
    
    if( this->classifier == NULL ){
        errorMessage = "setClassifier(const Classifier classifier) - Classifier Module Not Set!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Validate that the classifier was cloned correctly
    if( !this->classifier->clone( &classifier ) ){
        deleteClassifier();
        pipelineMode = PIPELINE_MODE_NOT_SET;
        errorMessage = "setClassifier(const Classifier classifier) - Classifier Module Not Set!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Set the mode of the pipeline to classification mode
    pipelineMode = CLASSIFICATION_MODE;
    
    //Flag that the key part of the pipeline has now been initialized
    initialized = true;

	//The pipeline has been changed, so flag that the pipeline is no longer trained
	trained = false;
    
    return true;
}

bool GestureRecognitionPipeline::setRegressifier(const Regressifier &regressifier){
    //Delete any previous classifier and regressifier
    deleteClassifier();
    deleteRegressifier();
    
    //Set the mode of the pipeline to regression mode
    pipelineMode = REGRESSION_MODE;
    
    //Create a new instance of the regressifier and then clone the values across from the reference regressifier
    this->regressifier = regressifier.createNewInstance();
    
    //Validate that the regressifier was cloned correctly
    if( !this->regressifier->clone( &regressifier ) ){
        deleteRegressifier();
        pipelineMode = PIPELINE_MODE_NOT_SET;
        errorMessage = "setRegressifier(const Regressifier &regressifier) - Regressifier Module Not Set!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Flag that the key part of the pipeline has now been initialized
    initialized = true;

	//The pipeline has been changed, so flag that the pipeline is no longer trained
	trained = false;
    
    return true;
}

bool GestureRecognitionPipeline::addPostProcessingModule(const PostProcessing &postProcessingModule,UINT insertIndex){
    
    //Validate the insertIndex is valid
    if( insertIndex != INSERT_AT_END_INDEX && insertIndex >= postProcessingModules.size() ){
        errorMessage = "addPostProcessingModule((const PostProcessing &postProcessingModule) - Invalid insertIndex value!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Create a new instance of the preProcessing and then clone the values across from the reference preProcessing
    PostProcessing *newInstance = postProcessingModule.createNewInstance();
    
    //Verify that the clone was successful
    if( !newInstance->clone( &postProcessingModule ) ){
        delete newInstance;
        newInstance = NULL;
        errorMessage = "addPostProcessingModule(const PostProcessing &postProcessingModule) - PostProcessing Module Not Set!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Add the new instance to the postProcessingModules
    vector< PostProcessing* >::iterator iter = postProcessingModules.begin();
    
    if( insertIndex == INSERT_AT_END_INDEX ) iter = postProcessingModules.end(); 
    else iter = postProcessingModules.begin() + insertIndex;
    
    postProcessingModules.insert(iter, newInstance);

	//The pipeline has been changed, so flag that the pipeline is no longer trained
	trained = false;
    
    return true;
}

bool GestureRecognitionPipeline::setPostProcessingModule(const PostProcessing &postProcessingModule){
	removeAllPostProcessingModules();
	return addPostProcessingModule( postProcessingModule );
}

bool GestureRecognitionPipeline::addContextModule(const Context &contextModule,UINT contextLevel,UINT insertIndex){
	
	//Validate the contextLevel is valid
    if( contextLevel >= contextModules.size() ){
        errorMessage = "addContextModule(...) - Invalid contextLevel value!";
        errorLog << errorMessage << endl;
        return false;
    }

	//Validate the insertIndex is valid
    if( insertIndex != INSERT_AT_END_INDEX && insertIndex >= contextModules[contextLevel].size() ){
        errorMessage = "addContextModule(...) - Invalid insertIndex value!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Create a new instance of the preProcessing and then clone the values across from the reference preProcessing
    Context *newInstance = contextModule.createNewInstance();
    
    //Verify that the clone was successful
    if( !newInstance->clone( &contextModule ) ){
        delete newInstance;
        newInstance = NULL;
        errorMessage = "addContextModule(...) - Context Module Not Set!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Add the new instance to the contextModules
    vector< Context* >::iterator iter = contextModules[ contextLevel ].begin();
    
    if( insertIndex == INSERT_AT_END_INDEX ) iter = contextModules[ contextLevel ].end(); 
    else iter = contextModules[ contextLevel ].begin() + insertIndex;
    
    contextModules[ contextLevel ].insert(iter, newInstance);
    
    return true;
}

bool GestureRecognitionPipeline::updateContextModule(bool value,UINT contextLevel,UINT moduleIndex){
    
    //Validate the contextLevel is valid
    if( contextLevel >= contextModules.size() ){
        errorMessage = "updateContextModule(...) - Context Level is out of bounds!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Validate the moduleIndex is valid
    if( moduleIndex >= contextModules[contextLevel].size() ){
        errorMessage = "updateContextModule(...) - Invalid contextLevel value!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    return contextModules[contextLevel][moduleIndex]->updateContext( value );
}
    
bool GestureRecognitionPipeline::removeAllPreProcessingModules(){
    deleteAllPreProcessingModules();
    return true;
}
    
bool GestureRecognitionPipeline::removePreProcessingModule(UINT moduleIndex){
    if( moduleIndex >= preProcessingModules.size() ){
        errorMessage =  "removePreProcessingModule(UINT moduleIndex) - Invalid moduleIndex " + Util::toString(moduleIndex) + ". The size of the preProcessingModules vector is " + Util::toString((int)preProcessingModules.size());
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Delete the module
    delete preProcessingModules[ moduleIndex ];
    preProcessingModules[ moduleIndex ] = NULL;
    preProcessingModules.erase( preProcessingModules.begin() + moduleIndex );

	//The pipeline has been changed, so flag that the pipeline is no longer trained
	trained = false;
    
    return true;
}
   
bool GestureRecognitionPipeline::removeAllFeatureExtractionModules(){
    deleteAllFeatureExtractionModules();
    return true;
}
        
bool GestureRecognitionPipeline::removeFeatureExtractionModule(UINT moduleIndex){
    if( moduleIndex >= featureExtractionModules.size() ){
        errorMessage = "removeFeatureExtractionModule(UINT moduleIndex) - Invalid moduleIndex " + Util::toString(moduleIndex) + ". The size of the featureExtractionModules vector is " + Util::toString((int)featureExtractionModules.size());
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Delete the module
    delete featureExtractionModules[ moduleIndex ];
    featureExtractionModules[ moduleIndex ] = NULL;
    featureExtractionModules.erase( featureExtractionModules.begin() + moduleIndex );

	//The pipeline has been changed, so flag that the pipeline is no longer trained
	trained = false;
    
    return true;
}

bool GestureRecognitionPipeline::removeAllPostProcessingModules(){
    deleteAllPostProcessingModules();
    return true;
}

bool GestureRecognitionPipeline::removePostProcessingModule(UINT moduleIndex){
    if( moduleIndex >= postProcessingModules.size() ){
        errorMessage =  "removePostProcessingModule(UINT moduleIndex) - Invalid moduleIndex " + Util::toString( moduleIndex ) + ". The size of the postProcessingModules vector is " + Util::toString( (int)postProcessingModules.size() );
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Delete the module
    delete postProcessingModules[ moduleIndex ];
    postProcessingModules[ moduleIndex ] = NULL;
    postProcessingModules.erase( postProcessingModules.begin() + moduleIndex );

	//The pipeline has been changed, so flag that the pipeline is no longer trained
	trained = false;
    
    return true;
}
    
bool GestureRecognitionPipeline::removeContextModule(UINT contextLevel,UINT moduleIndex){
    if( contextLevel >= NUM_CONTEXT_LEVELS ){
        errorMessage = "removeContextModule(UINT contextLevel,UINT moduleIndex) - Invalid moduleIndex " + Util::toString(moduleIndex) + " is out of bounds!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    if( moduleIndex >= contextModules[contextLevel].size() ){
        errorMessage = "removePostProcessingModule(UINT moduleIndex) - Invalid moduleIndex " + Util::toString(moduleIndex) + ". The size of the contextModules vector at context level " + Util::toString(contextLevel) + " is " + Util::toString((int)contextModules[contextLevel].size());
        errorLog << errorMessage << endl;
        return false;
    }
    
    //Delete the module
    delete contextModules[contextLevel][moduleIndex];
    contextModules[contextLevel][moduleIndex] = NULL;
    contextModules[contextLevel].erase( contextModules[contextLevel].begin() + moduleIndex );
    return true;
}
    
bool GestureRecognitionPipeline::removeAllContextModules(){
    deleteAllContextModules();
    return true;
}

bool GestureRecognitionPipeline::clearAll(){
	
	removeAllPreProcessingModules();
	removeAllFeatureExtractionModules();
	removeClassifier();
	removeRegressifier();
	removeAllPostProcessingModules();
	removeAllContextModules();
	
	return true;
}
    
bool GestureRecognitionPipeline::clearTestResults(){
    
    numTestSamples = 0;
    testAccuracy = 0;
    testRMSError = 0;
    testSquaredError = 0;
    testTime = 0;
    testFMeasure.clear();
    testPrecision.clear();
    testRecall.clear();
    testRejectionPrecision = 0;
    testRejectionRecall = 0;
    testConfusionMatrix.clear();
    testClassificationResults.clear();
    
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
///////////////////////////////////////////          PROTECTED FUNCTIONS              ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    
void GestureRecognitionPipeline::deleteAllPreProcessingModules(){
    if( preProcessingModules.size() != 0 ){
        for(UINT i=0; i<preProcessingModules.size(); i++){
            delete preProcessingModules[i];
            preProcessingModules[i] = NULL;
        }
        preProcessingModules.clear();
        trained = false;
    }
}
    
void GestureRecognitionPipeline::deleteAllFeatureExtractionModules(){
    if( featureExtractionModules.size() != 0 ){
        for(UINT i=0; i<featureExtractionModules.size(); i++){
            delete featureExtractionModules[i];
            featureExtractionModules[i] = NULL;
        }
        featureExtractionModules.clear();
        trained = false;
    }
}
    
void GestureRecognitionPipeline::deleteClassifier(){
    if( classifier != NULL ){
        delete classifier;
        classifier = NULL;
    }
    trained = false;
    initialized = false;
}
    
void GestureRecognitionPipeline::deleteRegressifier(){
    if( regressifier != NULL ){
        delete regressifier;
        regressifier = NULL;
    }
    trained = false;
    initialized = false;
}
    
void GestureRecognitionPipeline::deleteAllPostProcessingModules(){
    if( postProcessingModules.size() != 0 ){
        for(UINT i=0; i<postProcessingModules.size(); i++){
            delete postProcessingModules[i];
            postProcessingModules[i] = NULL;
        }
        postProcessingModules.clear();
        trained = false;
    }
}
    
void GestureRecognitionPipeline::deleteAllContextModules(){
    for(UINT i=0; i<contextModules.size(); i++){
        for(UINT j=0; j<contextModules[i].size(); j++){
            delete contextModules[i][j];
            contextModules[i][j] = NULL;
        }
        contextModules[i].clear();
    }
}
    
bool GestureRecognitionPipeline::updateTestMetrics(const UINT classLabel,const UINT predictedClassLabel,VectorDouble &precisionCounter,VectorDouble &recallCounter,double &rejectionPrecisionCounter,double &rejectionRecallCounter,VectorDouble &confusionMatrixCounter){

    //Find the index of the classLabel
    UINT predictedClassLabelIndex =0;
    bool predictedClassLabelIndexFound = false;
    if( predictedClassLabel != 0 ){
        for(UINT k=0; k<getNumClassesInModel(); k++){
            if( predictedClassLabel == classifier->getClassLabels()[k] ){
                predictedClassLabelIndex = k;
                predictedClassLabelIndexFound = true;
                break;
            }
        }
        
        if( !predictedClassLabelIndexFound ){
            errorMessage = "Failed to find class label index for label: " + Util::toString(predictedClassLabel);
            errorLog << errorMessage << endl;
            return false;
        }
    }

    //Find the index of the class label
    UINT actualClassLabelIndex = 0;
    if( classLabel != 0 ){
        for(UINT k=0; k<getNumClassesInModel(); k++){
            if( classLabel == classifier->getClassLabels()[k] ){
                actualClassLabelIndex = k;
                break;
            }
        }
    }

    //Update the classification accuracy
    if( classLabel == predictedClassLabel ){
        testAccuracy++;
    }

    //Update the precision
    if( predictedClassLabel != 0 ){
        if( classLabel == predictedClassLabel ){
            //Update the precision value
            testPrecision[ predictedClassLabelIndex ]++;
        }
        //Update the precision counter
        precisionCounter[ predictedClassLabelIndex ]++;
    }

    //Update the recall
    if( classLabel != 0 ){
        if( classLabel == predictedClassLabel ){
            //Update the recall value
            testRecall[ predictedClassLabelIndex ]++;
        }
        //Update the recall counter
        recallCounter[ actualClassLabelIndex ]++;
    }

    //Update the rejection precision
    if( predictedClassLabel == 0 ){
        if( classLabel == 0 ) testRejectionPrecision++;
        rejectionPrecisionCounter++;
    }

    //Update the rejection recall
    if( classLabel == 0 ){
        if( predictedClassLabel == 0 ) testRejectionRecall++;
        rejectionRecallCounter++;
    }

    //Update the confusion matrix
    if( classifier->getNullRejectionEnabled() ){
        if( classLabel == 0 ) actualClassLabelIndex = 0;
        else actualClassLabelIndex++;
        if( predictedClassLabel == 0 ) predictedClassLabelIndex = 0;
        else predictedClassLabelIndex++;
    }
    testConfusionMatrix[ actualClassLabelIndex  ][ predictedClassLabelIndex ]++;
    confusionMatrixCounter[ actualClassLabelIndex ]++;
    
    return true;
}

bool GestureRecognitionPipeline::computeTestMetrics(VectorDouble &precisionCounter,VectorDouble &recallCounter,double &rejectionPrecisionCounter,double &rejectionRecallCounter,VectorDouble &confusionMatrixCounter,const UINT numTestSamples){
        
    //Compute the test metrics
    testAccuracy = testAccuracy/double(numTestSamples) * 100.0;
    
    for(UINT k=0; k<getNumClassesInModel(); k++){
        if( precisionCounter[k] > 0 ) testPrecision[k] /= precisionCounter[k];
        else testPrecision[k] = 0;
        if( recallCounter[k] > 0 ) testRecall[k] /= recallCounter[k];
        else testRecall[k] = 0;
        
        if( precisionCounter[k] + recallCounter[k] > 0 )
            testFMeasure[k] = 2 * ((testPrecision[k]*testRecall[k])/(testPrecision[k]+testRecall[k]));
        else testFMeasure[k] = 0;
    }
    if( rejectionPrecisionCounter > 0 ) testRejectionPrecision /= rejectionPrecisionCounter;
    if( rejectionRecallCounter > 0 ) testRejectionRecall /= rejectionRecallCounter;
    
    
    for(UINT r=0; r<confusionMatrixCounter.size(); r++){
        if( confusionMatrixCounter[r] > 0 ){
            for(UINT c=0; c<testConfusionMatrix.getNumCols(); c++){
                testConfusionMatrix[r][c] /= confusionMatrixCounter[r];
            }
        }
    }
    
    return true;
}
    
string GestureRecognitionPipeline::getPipelineModeAsString() const{
    switch( pipelineMode ){
        case PIPELINE_MODE_NOT_SET:
            return "PIPELINE_MODE_NOT_SET";
            break;
        case CLASSIFICATION_MODE:
            return "CLASSIFICATION_MODE";
            break;
        case REGRESSION_MODE:
            return "REGRESSION_MODE";
            break;
        default:
            return "ERROR_UNKNWON_PIPELINE_MODE";
            break;
    }
    
    return "ERROR_UNKNWON_PIPELINE_MODE";
}

UINT GestureRecognitionPipeline::getPipelineModeFromString(string pipelineModeAsString) const{
	if( pipelineModeAsString == "PIPELINE_MODE_NOT_SET" ){
		return PIPELINE_MODE_NOT_SET;
	}
	if( pipelineModeAsString == "CLASSIFICATION_MODE" ){
		return CLASSIFICATION_MODE;
	}
	if( pipelineModeAsString == "REGRESSION_MODE" ){
		return REGRESSION_MODE;
	}
	return PIPELINE_MODE_NOT_SET;
}

} //End of namespace GRT

