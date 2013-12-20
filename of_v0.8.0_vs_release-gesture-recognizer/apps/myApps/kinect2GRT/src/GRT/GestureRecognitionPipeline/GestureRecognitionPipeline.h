/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @section LICENSE
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
 
 @section DESCRIPTION
 This file contains the GestureRecognitionPipeline class.  The GestureRecognitionPipeline is the core module of the Gesture Recognition Toolkit.
 You can use the GestureRecognitionPipeline to link the various GRT modules together, for instace you can link a PreProcessing module to a FeatureExtraction
 module to a Classification module and then to a PostProcessing module.
 */

#ifndef GRT_GESTURE_RECOGNITION_PIPELINE_HEADER
#define GRT_GESTURE_RECOGNITION_PIPELINE_HEADER

#include "PreProcessing.h"
#include "FeatureExtraction.h"
#include "Classifier.h"
#include "Regressifier.h"
#include "PostProcessing.h"
#include "Context.h"
#include "../DataStructures/LabelledContinuousTimeSeriesClassificationData.h"
#include "../Util/ClassificationResult.h"
#include "../Util/TestResult.h"

namespace GRT{
    
#define INSERT_AT_END_INDEX 99999

class GestureRecognitionPipeline : public GRTBase
{
public:
	/**
     Default Constructor
	*/
	GestureRecognitionPipeline(void);
	
	/**
     Copy Constructor. Performs a depp copy of the data from the rhs pipeline into this pipeline.
	*/
	GestureRecognitionPipeline(const GestureRecognitionPipeline &rhs);
	
	/**
     Default Destructor
	*/
	~GestureRecognitionPipeline(void);
	
	/**
     Equals Constructor. Performs a depp copy of the data from the rhs pipeline into this pipeline.
	*/
	GestureRecognitionPipeline& operator=(const GestureRecognitionPipeline &rhs);
    
    /**
     This is the main training interface for training a Classifier with LabelledClassificationData.  This function will pass the trainingData through 
     any PreProcessing or FeatureExtraction modules that have been added to the GestureRecognitionPipeline, and then calls the training function of 
     the Classification module that has been added to the GestureRecognitionPipeline.  
     The function will return true if the classifier was trained successfully, false otherwise.

	@param LabelledClassificationData trainingData: the labelled classification training data that will be used to train the classifier at the core of the pipeline
	@return bool returns true if the classifier was trained successfully, false otherwise
	*/
    bool train(LabelledClassificationData trainingData);

	/**
     This is the main training interface for training a Classifier with LabelledClassificationData using K-fold cross validation.  This function will pass 
     the trainingData through any PreProcessing or FeatureExtraction modules that have been added to the GestureRecognitionPipeline, and then calls the 
     training function of the Classification module that has been added to the GestureRecognitionPipeline.  
     The function will return true if the classifier was trained successfully, false otherwise.

	@param LabelledClassificationData trainingData: the labelled classification training data that will be used to train the classifier at the core of the pipeline
	@param const UINT kFoldValue: the number of cross validation folds, this should be a value between in the range of [1 M-1], where M is the number of training samples int the LabelledClassificationData
	@param const bool useStratifiedSampling: sets if stratified sampling should be used during the cross validation training
	@return bool returns true if the classifier was trained successfully, false otherwise
	*/
    bool train(LabelledClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling = false );

	/**
     This is the main training interface for training a Classifier with LabelledTimeSeriesClassificationData.  This function will pass 
     the trainingData through any PreProcessing or FeatureExtraction modules that have been added to the GestureRecognitionPipeline, and then calls the 
     training function of the Classification module that has been added to the GestureRecognitionPipeline.  
     The function will return true if the classifier was trained successfully, false otherwise.

    @param LabelledTimeSeriesClassificationData trainingData: the labelled time-series classification training data that will be used to train the classifier at the core of the pipeline
    @return bool returns true if the classifier was trained successfully, false otherwise
	*/
    bool train(LabelledTimeSeriesClassificationData trainingData);
    
    /**
     This is the main training interface for training a Classifier with LabelledTimeSeriesClassificationData using K-fold cross validation.  
     This function will pass the trainingData through any PreProcessing or FeatureExtraction modules that have been added to the GestureRecognitionPipeline, and then calls the training function of the Classification module that has been added to the GestureRecognitionPipeline.
     The function will return true if the classifier was trained successfully, false otherwise.
     
     @param LabelledTimeSeriesClassificationData trainingData: the labelled time-series classification training data that will be used to train the classifier at the core of the pipeline
     @param const UINT kFoldValue: the number of cross validation folds, this should be a value between in the range of [1 M-1], where M is the number of training samples in the LabelledClassificationData
     @param const bool useStratifiedSampling: sets if stratified sampling should be used during the cross validation training
     @return bool returns true if the classifier was trained and tested successfully, false otherwise
     */
    bool train(LabelledTimeSeriesClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling = false);

	/**
     This is the main training interface for training a regression module with LabelledRegressionData.  This function will pass 
     the trainingData through any PreProcessing or FeatureExtraction modules that have been added to the GestureRecognitionPipeline, and then calls the 
     training function of the regression module that has been added to the GestureRecognitionPipeline.  
     The function will return true if the classifier was trained successfully, false otherwise.

    @param LabelledRegressionData trainingData: the labelled regression training data that will be used to train the regression module at the core of the pipeline
    @return bool returns true if the regression module was trained successfully, false otherwise
	*/
    bool train(LabelledRegressionData trainingData);
    
    /**
     This is the main training interface for training a Regressifier with LabelledRegressionData using K-fold cross validation.  This function will pass
     the trainingData through any PreProcessing or FeatureExtraction modules that have been added to the GestureRecognitionPipeline, and then calls the
     training function of the Regression module that has been added to the GestureRecognitionPipeline.
     The function will return true if the regressifier was trained successfully, false otherwise.
     
     @param LabelledRegressionData trainingData: the labelled regression training data that will be used to train the regressifier at the core of the pipeline
     @param const UINT kFoldValue: the number of cross validation folds, this should be a value between in the range of [1 M-1], where M is the number of training samples in the LabelledRegressionData
     @return bool returns true if the regressifier was trained and tested successfully, false otherwise
     */
    bool train(LabelledRegressionData trainingData,const UINT kFoldValue);
    
    /**
     This function is the main interface for testing the accuracy of a pipeline with LabelledClassificationData.  This function will pass
     the testData through any PreProcessing or FeatureExtraction modules that have been added to the GestureRecognitionPipeline, and then calls the 
     predict function of the classification module that has been added to the GestureRecognitionPipeline.  
     The function will return true if the pipeline was tested successfully, false otherwise.

     @param LabelledClassificationData testData: the labelled classification data that will be used to test the accuracy of the pipeline
     @return bool returns true if the pipeline was tested successfully, false otherwise
	*/
    bool test(LabelledClassificationData testData);

    /**
     This function is the main interface for testing the accuracy of a pipeline with LabelledTimeSeriesClassificationData.  This function will pass
     the testData through any PreProcessing or FeatureExtraction modules that have been added to the GestureRecognitionPipeline, and then calls the 
     predict function of the classification module that has been added to the GestureRecognitionPipeline.  
     The function will return true if the pipeline was tested successfully, false otherwise.

     @param LabelledTimeSeriesClassificationData testData: the labelled timeseries classification data that will be used to test the accuracy of the pipeline
     @return bool returns true if the pipeline was tested successfully, false otherwise
	*/
    bool test(LabelledTimeSeriesClassificationData testData);

    /**
     This function is the main interface for testing the accuracy of a pipeline with LabelledContinuousTimeSeriesClassificationData.  This function will pass
     the testData through any PreProcessing or FeatureExtraction modules that have been added to the GestureRecognitionPipeline, and then calls the 
     predict function of the classification module that has been added to the GestureRecognitionPipeline.  
     The function will return true if the pipeline was tested successfully, false otherwise.

     @param LabelledContinuousTimeSeriesClassificationData testData: the labelled continuous timeseries classification data that will be used to test the accuracy of the pipeline
     @return bool returns true if the pipeline was tested successfully, false otherwise
	*/
    bool test(LabelledContinuousTimeSeriesClassificationData testData);

    /**
     This function is the main interface for testing the accuracy of a pipeline with LabelledRegressionData.  This function will pass
     the testData through any PreProcessing or FeatureExtraction modules that have been added to the GestureRecognitionPipeline, and then calls the 
     predict function of the regression module that has been added to the GestureRecognitionPipeline.  
     The function will return true if the pipeline was tested successfully, false otherwise.

     @param LabelledRegressionData testData: the labelled regression data that will be used to test the accuracy of the pipeline
     @return bool returns true if the pipeline was tested successfully, false otherwise
	*/
    bool test(LabelledRegressionData testData);
    
    /**
     This function is the main interface for all predictions using the gesture recognition pipeline.  You can use this function for both classification
     and regression.  You should only call this function if you  have trained the pipeline.  The input vector should be the same size as your training data.

     @param VectorDouble inputVector: the input data that will be passed through the pipeline for classification or regression
     @return bool returns true if the prediction was successful, false otherwise
	*/
    bool predict(VectorDouble inputVector);
    
    /**
     This function is an interface for predictions using timeseries or Matrix data.
     You should only call this function if you  have trained the pipeline.  The input matrix should have the same number of columns as your training data.
     
     @param MatrixDouble inputMatrix: the input atrix that will be passed through the pipeline for classification
     @return bool returns true if the prediction was successful, false otherwise
     */
    bool predict(MatrixDouble inputMatrix);

    /**
     This function is now depreciated, you should use the predict function instead.

     This function used to be the main interface for all regression using the gesture recognition pipeline.  
     You should only call this function if you  have trained the pipeline.  The input vector should be the same size as your training data.

     @param VectorDouble inputVector: the input data that will be passed through the pipeline for regression
     @return bool returns true if the regression was successful, false otherwise
	*/
    bool map(VectorDouble inputVector);
    
    /**
     This function is the main interface for resetting the entire gesture recognition pipeline.  This function will call reset on all the modules in 
     the current pipeline.

     @return bool returns true if the reset was successful, false otherwise
	*/
    bool reset();

    /**
     This function will save the entire pipeline to a file.  This includes all the modules types, settings, and models.

     @param string filename: the name of the file you want to save the pipeline to
     @return bool returns true if the pipeline was saved successful, false otherwise
	*/
    bool savePipelineToFile(string filename);

    /**
     This function will load an entire pipeline from a file.  This includes all the modules types, settings, and models.

     @param string filename: the name of the file you want to load the pipeline from
     @return bool returns true if the pipeline was loaded successful, false otherwise
	*/
    bool loadPipelineFromFile(string filename);
    
    /**
     This function will pass the input vector through any preprocessing or feature extraction modules added to the pipeline.  This function
     can be useful for testing and validating a preprocessing or feature extraction module, without having to acutally train a classification or
     regression module.  The second parameter controls if any feature extraction module should be used.  If set to true then both preprocessing and
     feature extraction modules will be used, if false then just preprocessing modules will be used.

     The function will not pass the preprocessed data through the classification or regression module.

     After calling this function, you can access the preprocessed results via the getPreProcessedData() or getFeatureExtractionData() functions.

     @param: VectorDouble inputVector: the input data that should be processed
     @param: bool computeFeatures: sets if just the preprocessing modules should be used (false), or both the preprocessing and feature extraction modules should be used (true)
     @return bool returns true if the data was passed through the preprocessing modules successful, false otherwise
	*/
    bool preProcessData(VectorDouble inputVector,bool computeFeatures = true);
    
    /**
    This function returns true if the pipeline has been initialized.  The pipeline is initialized if either a classifier or regressifier module has been set at the core of the pipeline.
    
    @return bool returns true if the pipeline has been initialized, false otherwise.
	*/
    bool getIsInitialized() const;

    /**
	 This function returns true if the classifier or regressifier at the core of the pipeline has been trained.
     
     @return bool returns true if the classifier or regressifier at the core of the pipeline has been trained, false otherwise.
     */
    bool getTrained() const;

    /**
	 This function returns true if any preprocessing modules have been added to the pipeline.
	
	@return bool returns true if any preprocessing modules have been added to the pipeline, false otherwise.
	*/
    bool getIsPreProcessingSet() const;

    /**
	 This function returns true if any feature extraction modules have been added to the pipeline.
	
	@return bool returns true if any feature extraction modules have been added to the pipeline, false otherwise.
	*/
    bool getIsFeatureExtractionSet() const;

    /**
	 This function returns true if a classifier has been added to the pipeline.
	
	@return bool returns true if a classifier has been added to the pipeline, false otherwise.
	*/
    bool getIsClassifierSet() const;

    /**
	 This function returns true if a regressifier has been added to the pipeline.
	
	@return bool returns true if a regressifier has been added to the pipeline, false otherwise.
	*/
    bool getIsRegressifierSet() const;

    /**
	 This function returns true if any post processing modules have been added to the pipeline.
	
	@return bool returns true if any post processing modules have been added to the pipeline, false otherwise.
	*/
    bool getIsPostProcessingSet() const;

    /**
	 This function returns true if any context modules have been added to the pipeline.
	
	@return bool returns true if any context modules have been added to the pipeline, false otherwise.
	*/
    bool getIsContextSet() const;

    /**
	 This function returns true if either a classifier or regressifier has been added to the pipeline.
	
	@return bool returns true if either a classifier or regressifier has been added to the pipeline, false otherwise.
	*/
    bool getIsPipelineModeSet() const;

    /**
	 This function returns true if a classifier has been added to the pipeline (and the pipeline is therefore in classification mode).
	
	@return bool returns true if a classifier has been added to the pipeline, false otherwise.
	*/
    bool getIsPipelineInClassificationMode() const;

    /**
	 This function returns true if a regressifier has been added to the pipeline (and the pipeline is therefore in regression mode).
	
	@return bool returns true if a regressifier has been added to the pipeline, false otherwise.
	*/
    bool getIsPipelineInRegressionMode() const;
    
    /**
	 This function returns the size of the expected input vector to the pipeline.  This size comes from the size of the training data used to train the pipeline.
	
	@return UINT representing the size of the input vector to the pipeline.
	*/
    UINT getInputVectorDimensionsSize() const;

    /**
	 This function returns the size of the expected output vector from the pipeline.  This size comes from the size of the training data used to train the pipeline.
	 This is only relevant when the pipeline is in regression mode.
	
	@return UINT representing the size of the output vector from the pipeline.
	*/
    UINT getOutputVectorDimensionsSize() const;

    /**
	 This function is now depreciated. You should use #getNumClasses() instead. 
	
	@return UINT representing the number of classes in the model.
	*/
    UINT getNumClassesInModel() const;
    
    /**
	 This function returns the number of classes in the model. If the model has not been trained then the number of classes will be zero.
	 This is only relevant when the pipeline is in classification mode.
     
     @return UINT representing the number of classes in the model.
     */
    UINT getNumClasses() const;

    /**
	 This function returns the number of preprocessing modules that have been added to the pipeline.
	
	@return UINT representing the number of preprocessing modules that have been added to the pipeline.
	*/
    UINT getNumPreProcessingModules() const;

    /**
	 This function returns the number of feature extraction modules that have been added to the pipeline.
	
	@return UINT representing the number of feature extraction modules that have been added to the pipeline.
	*/
    UINT getNumFeatureExtractionModules() const;

    /**
	 This function returns the number of post processing modules that have been added to the pipeline.
	
	@return UINT representing the number of post processing modules that have been added to the pipeline.
	*/
    UINT getNumPostProcessingModules() const;

    /**
	 This function returns the current position of the prediction module index. The prediction module index indicates how far along the pipeline a data sample gets before the pipeline 
	 exits during a prediction.  For example, if you have two preprocessing modules, one feature extraction module, a classifier, and one post processing module in your pipeline and the
	 prediction module index is 3 after you call the predict(...) function, then the data only got as far as the feature extraction module but did not get passed through the classifier or
	 post processing modules.
	
	@return UINT representing the number of post processing modules that have been added to the pipeline.
	*/
    UINT getPredictionModuleIndexPosition() const;

    /**
	 This function returns the predicted class label from the most recent predict(...) function call.  This value is only relevant when the pipeline is in prediction mode.
	
	@return UINT representing the predicted class label from the most recent prediction.
	*/
    UINT getPredictedClassLabel() const;

    /**
	 This function returns the unprocessed predicted class label from the most recent predict(...) function call.  The unprocessed predicted class label is the class label output by
	 the classifier before any post processing modules filter the value.  This value is only relevant when the pipeline is in prediction mode.
	
	@return UINT representing the unprocessed predicted class label from the most recent prediction.
	*/
    UINT getUnProcessedPredictedClassLabel() const;
    
    /**
	 This function returns the maximum likelihood value from the most likely class from the most recent prediction.  This value is only relevant when the pipeline is in prediction mode.
	
	@return double representing the maximum likelihood value from the most likely class from the most recent prediction.
	*/
    double getMaximumLikelihood() const;

    /**
	 This function returns the cross validation value from the most recent cross validation test.  If the pipeline is in prediction mode, then the cross
     validation accuracy will be the average accuracy across each fold of testing. If the pipeline is in regression mode, then the cross validation 
     accuracy will be the average RMS error across each fold of testing.
	
	@return double representing the cross validation value from the most recent cross validation test.
	*/
    double getCrossValidationAccuracy() const;

    /**
	 This function returns the accuracy value from the most recent test.  This will be a value between [0 100]. This value is only relevant when the pipeline is in prediction mode.
	
	@return double representing the accuracy value from the most recent test.
	*/
    double getTestAccuracy() const;

    /**
	 This function returns the root mean squared error value from the most recent test.  This value is only relevant when the pipeline is in regression mode.
	
	@return double representing the root mean squared error value from the most recent test.
	*/
    double getTestRMSError() const;

    /**
	 This function returns the f-measure from the most recent test for the class with the matching classLabel.  This value is only relevant when the pipeline is in classification mode.
	 If the classLabel is not valid then the function will return -1.
	
	@param UINT classLabel: the label of the class you want to get the test fMeasure value for
	@return double representing the f-measure from the most recent test for the class with the matching classLabel.
	*/
    double getTestFMeasure(UINT classLabel) const;

    /**
	 This function returns the precision from the most recent test for the class with the matching classLabel.  This value is only relevant when the pipeline is in classification mode.
	 If the classLabel is not valid then the function will return -1. 
	
	@param UINT classLabel: the label of the class you want to get the test precision value for
	@return double representing the precision from the most recent test for the class with the matching classLabel.
	*/
    double getTestPrecision(UINT classLabel) const;

    /**
	 This function returns the recall from the most recent test for the class with the matching classLabel.  This value is only relevant when the pipeline is in classification mode.
	 If the classLabel is not valid then the function will return -1.
	
	@param UINT classLabel: the label of the class you want to get the test recall value for
	@return double representing the recall from the most recent test for the class with the matching classLabel.
	*/
    double getTestRecall(UINT classLabel) const;

    /**
	 This function returns the precision for any null examples in your dataset (examples with the class label of 0) from the most recent test.  
	 This value is only relevant when the pipeline is in classification mode.
	
	@return double representing the precision for any null examples in your dataset.
	*/
    double getTestRejectionPrecision() const;

    /**
	 This function returns the recall for any null examples in your dataset (examples with the class label of 0) from the most recent test.  
	 This value is only relevant when the pipeline is in classification mode.
	
	@return double representing the recall for any null examples in your dataset.
	*/
    double getTestRejectionRecall() const;

    /**
	 This function returns the total test time (in milliseconds) for the most recent test.
	
	@return double representing the total test time (in milliseconds) for the most recent test.
	*/
    double getTestTime() const;

    /**
	 This function returns the total training time (in milliseconds) for the most recent training.
	
	@return double representing the total training time (in milliseconds) for the most recent training.
	*/
    double getTrainingTime() const;

    /**
	 This function returns the confusion matrix for the most recent round of testing.  If null rejection is enabled then the first row and column of the confusion matrix will
	 represent the null class (class label 0).
	
	@return MatrixDouble representing the confusion matrix for the most recent round of testing.
	*/
    MatrixDouble getTestConfusionMatrix() const;
    
    /**
     This function returns all the results from the most recent round of testing.
     The TestResult contains all of the test metrics, such as the accuracy, precision, recall, training and test times, etc. These are the
     same results that you would get if you called the individaul functions (such as getTestAccuracy()).
     
     @return returns an instance of TestResult containing all the results from the most recent round of testing, an empty TestResult will be returned if the model has not been tested
     */
    TestResult getTestResults() const;
    
    /**
     Gets a vector of the precision results for each class from the most recent round of testing.  
     This will be a K-dimensional vector, where K is the number of classes in the model.
     
     @return returns a vector of the precision results for each class from the last prediction, an empty vector will be returned if the model has not been tested
     */
    VectorDouble getTestPrecision() const;
    
    /**
     Gets a vector of the recall results for each class from the most recent round of testing.
     This will be a K-dimensional vector, where K is the number of classes in the model.
     
     @return returns a vector of the recall results for each class from the last prediction, an empty vector will be returned if the model has not been tested
     */
    VectorDouble getTestRecall() const;
    
    /**
     Gets a vector of the fMeasure results for each class from the most recent round of testing.
     This will be a K-dimensional vector, where K is the number of classes in the model.
     
     @return returns a vector of the fMeasure results for each class from the last prediction, an empty vector will be returned if the model has not been tested
     */
    VectorDouble getTestFMeasure() const;

    /**
     Gets a vector of the class likelihoods from the last prediction, this will be an N-dimensional vector, where N is the number of classes in the model.  
     The exact form of these likelihoods depends on the classification algorithm.
     
     @return returns a vector of the class likelihoods from the last prediction, an empty vector will be returned if the model has not been trained
     */
    VectorDouble getClassLikelihoods() const;

    /**
     Gets a vector of the class distances from the last prediction, this will be an N-dimensional vector, where N is the number of classes in the model.  
     The exact form of these distances depends on the classification algorithm.
     
     @return returns a vector of the class distances from the last prediction, an empty vector will be returned if the model has not been trained
     */
    VectorDouble getClassDistances() const;

    /**
     Gets a vector containing the null rejection thresholds for each class, this will be an N-dimensional vector, where N is the number of classes in the model.  
     
     @return returns a vector containing the null rejection thresholds for each class, an empty vector will be returned if the model has not been trained
     */
    VectorDouble getNullRejectionThresholds() const;

    /**
     Gets a vector containing the regression data output by the pipeline, this will be an M-dimensional vector, where M is the number of output dimensions in the model.  
     
     @return returns a vector containing the regression data output by the pipeline, an empty vector will be returned if the model has not been trained
     */
    VectorDouble getRegressionData() const;

    /**
     Gets a vector containing the regression data output by the regression algorithm, this will be an M-dimensional vector, where M is the number of output dimensions in the model.  
     
     @return returns a vector containing the regression data output by the regression algorithm, an empty vector will be returned if the model has not been trained
     */
    VectorDouble getUnProcessedRegressionData() const;

    /**
     Gets a vector containing the output of the last preprocessing module, this will be an M-dimensional vector, where M is the output size of the last preprocessing module.  
     
     @return returns a vector containing the output of the last preprocessing module, an empty vector will be returned if there are no preprocessing modules.
     */
    VectorDouble getPreProcessedData() const;

    /**
     Gets a vector containing the output of the preprocessing module at index X, this will be an M-dimensional vector, where M is the output size of the preprocessing module.  
     
     @param UINT moduleIndex: the index of the pre processing module you want
     @return returns a vector containing the output of the preprocessing module at index X, an empty vector will be returned if there is no preprocessing module at that index.
     */
    VectorDouble getPreProcessedData(UINT moduleIndex) const;

    /**
     Gets a vector containing the output of the last feature extraction module, this will be an M-dimensional vector, where M is the output size of the last feature extraction module.  
     
     @return returns a vector containing the output of the last feature extraction module, an empty vector will be returned if there are no feature extraction modules.
     */
    VectorDouble getFeatureExtractionData() const;

    /**
     Gets a vector containing the output of the feature extraction module at index X, this will be an M-dimensional vector, where M is the output size of the feature extraction module.  
     
     @param UINT moduleIndex: the index of the feature extraction module you want
     @return returns a vector containing the output of the feature extraction module at index X, an empty vector will be returned if there is no feature extraction module at that index.
     */
    VectorDouble getFeatureExtractionData(UINT moduleIndex) const;

    /**
     Gets a vector containing the label each class represents, this will be an N-dimensional vector, where N is the number of classes in the model. 
     This is useful if the model was trained with non-monotonically class labels (i.e. class labels such as [1, 3, 6, 9, 12] instead of [1, 2, 3, 4, 5]).
     
     @return returns a vector containing the class labels for each class, an empty vector will be returned if the model has not been trained
     */
    vector< UINT > getClassLabels() const;

    /**
     Gets a vector containing the classification results from the most recent test.  Each element in the vector represents the results for the corresponding test sample.  
     
     @return returns a vector containing the test classification results from the most recent test, an empty vector will be returned if no test has been run.
     */
    vector< ClassificationResult > getTestClassificationResults() const;

    /**
     Gets a pointer to the preprocessing module at the specific moduleIndex.
     
     @param UINT moduleIndex: the index of the pre processing module you want
     @return returns a pointer to the preprocessing module at the specific moduleIndex, or NULL if the moduleIndex is invalid
     */
    PreProcessing* getPreProcessingModule(UINT moduleIndex);

    /**
     Gets a pointer to the feature extraction module at the specific moduleIndex.
     
     @param UINT moduleIndex: the index of the feature extraction module you want
     @return returns a pointer to the feature extraction module at the specific moduleIndex, or NULL if the moduleIndex is invalid
     */
    FeatureExtraction* getFeatureExtractionModule(UINT moduleIndex);

    /**
     Gets a pointer to the classifier module.
     
     @return returns a pointer to the classifier module, or NULL if the classifier has not been set
     */
    Classifier* getClassifier();

    /**
     Gets a pointer to the regressifier module.
     
     @return returns a pointer to the regressifier module, or NULL if the regressifier has not been set
     */
    Regressifier* getRegressifier();

    /**
     Gets a pointer to the post processing module at the specific moduleIndex.
     
     @param UINT moduleIndex: the index of the post processing module you want
     @return returns a pointer to the post processing module at the specific moduleIndex, or NULL if the moduleIndex is invalid
     */
    PostProcessing* getPostProcessingModule(UINT moduleIndex);

    /**
     Gets a pointer to the context module at the specific contextLevel and moduleIndex.
     
     @param UINT contextLevel: the context level that contains the context module you want
     @param UINT moduleIndex: the index of the context module you want
     @return returns a pointer to the context module at the specific contextLevel and moduleIndex, or NULL if the contextLevel or moduleIndex are invalid
     */
    Context* getContextModule(UINT contextLevel,UINT moduleIndex);
    
    /**
     Gets a pointer to the preprocessing module at the specific moduleIndex.  You should make sure that the type of the preprocessing module matches the template type. 
     
     @param UINT moduleIndex: the index of the pre processing module you want
     @return returns a pointer to the preprocessing module at the specific moduleIndex, or NULL if the moduleIndex is invalid
     */
    template <class T> T* getPreProcessingModule(UINT moduleIndex){ 
        if( moduleIndex < preProcessingModules.size() ){
            return (T*)preProcessingModules[ moduleIndex ];
        }
        return NULL;
    }
    
    /**
     Gets a pointer to the feature extraction module at the specific moduleIndex.  You should make sure that the type of the feature extraction module matches the template type. 
     
     @param UINT moduleIndex: the index of the feature extraction module you want
     @return returns a pointer to the feature extraction module at the specific moduleIndex, or NULL if the moduleIndex is invalid
     */
    template <class T> T* getFeatureExtractionModule(UINT moduleIndex){ 
        if( moduleIndex < featureExtractionModules.size() ){
            return (T*)featureExtractionModules[ moduleIndex ];
        }
        return NULL;
    }

    /**
     Gets a pointer to the classifier module. If the classifier has not been set, or the template type T does not match the current
     classifier type then the function will return NULL.
     
     @return returns a pointer to the classifier module, or NULL if the classifier has not been set
     */
    template <class T> T* getClassifier(){
        
        if( classifier == NULL ) return NULL;
        
        T temp;
        
        if( temp.getClassifierType() == classifier->getClassifierType() ){
            return (T*)classifier;
        }
        
        return NULL;
    }

    /**
     Gets a pointer to the regressifier module.  If the regressifier has not been set, or the template type T does not match the current
     regressifier type then the function will return NULL.
     
     @return returns a pointer to the regressifier module, or NULL if the regressifier has not been set
     */
    template <class T> T* getRegressifier(){
        
        if( regressifier == NULL ) return NULL;
        
        T temp;
        
        if( temp.getRegressifierType() == regressifier->getRegressifierType() ){
            return (T*)regressifier;
        }
        
        return NULL;
    }
    
    /**
     Gets a pointer to the post processing module at the specific moduleIndex.  You should make sure that the type of the post processing module matches the template type. 
     
     @param UINT moduleIndex: the index of the post processing module you want
     @return returns a pointer to the post processing module at the specific moduleIndex, or NULL if the moduleIndex is invalid
     */
    template <class T> T* getPostProcessingModule(UINT moduleIndex){ 
        if( moduleIndex < postProcessingModules.size() ){
            return (T*)postProcessingModules[ moduleIndex ];
        }
        return NULL;
    }
    
    /**
     Gets a pointer to the context module at the specific contextLevel and moduleIndex.  You should make sure that the type of the context module matches the template type. 
     
     @param UINT contextLevel: the context level that contains the context module you want
     @param UINT moduleIndex: the index of the context module you want
     @return returns a pointer to the context module at the specific contextLevel and moduleIndex, or NULL if the contextLevel or moduleIndex are invalid
     */
    template <class T> T* getContextModule(UINT contextLevel,UINT moduleIndex){ 
        if( contextLevel < contextModules.size() ){
            if( moduleIndex < contextModules[ contextLevel ].size() ){
                return (T*)contextModules[ contextLevel ][ moduleIndex ];
            }
        }
        return NULL;
    }
    
    /**
     Gets the pipeline mode as a string, this will be either "PIPELINE_MODE_NOT_SET","CLASSIFICATION_MODE", or "REGRESSION_MODE".
     
     @return returns the pipeline mode as a string
     */
    string getPipelineModeAsString() const;

    /**
     Converts a string to the pipeline mode.
     
     @param string pipelineMode: the pipeline mode as a string, i.e. "CLASSIFICATION_MODE"
     @return returns an unsigned integer representing the current pipeline mode
     */
	UINT getPipelineModeFromString(string pipelineMode) const;
    
    /**
     Adds a new pre processing module to the pipeline.  The user can specify the position at which the new module should be inserted into the list of preprocessing modules.  
     The default position is to insert the new module at the end of the list.
     
     @param const PreProcessing &preProcessingModule: a reference to the pre processing module you want to add
     @param UINT insertIndex: the index of where you want to insert the new pre processing module. Default is to insert the new module at the end of the list
     @return returns true if the preprocessing module was added successfully, false otherwise
     */
    bool addPreProcessingModule(const PreProcessing &preProcessingModule,UINT insertIndex = INSERT_AT_END_INDEX);

	/**
	 This function removes any existing pre processing modules, then it adds the new pre processing module.
     
     @param const PreProcessing &preProcessingModule: a reference to the pre processing module you want to add
     @return returns true if the preprocessing module was added successfully, false otherwise
     */
    bool setPreProcessingModule(const PreProcessing &preProcessingModule);

    /**
     Adds a new feature extraction module to the pipeline.  The user can specify the position at which the new module should be inserted into the list of feature extraction modules.  
     The default position is to insert the new module at the end of the list.
     
     @param const FeatureExtraction &featureExtractionModule: a reference to the feature extraction module you want to add
     @param UINT insertIndex: the index of where you want to insert the new feature extraction module. Default is to insert the new module at the end of the list
     @return returns true if the feature extraction module was added successfully, false otherwise
     */
    bool addFeatureExtractionModule(const FeatureExtraction &featureExtractionModule,UINT insertIndex = INSERT_AT_END_INDEX);

	/**
	 This function removes any existing feature extraction modules, then it adds the new feature extraction module.
     
     @param const FeatureExtraction &featureExtractionModule: a reference to the feature extraction module you want to add
     @return returns true if the feature extraction module was added successfully, false otherwise
     */
    bool setFeatureExtractionModule(const FeatureExtraction &featureExtractionModule);

    /**
     Sets the classifier at the core of the pipeline.  A pipeline can only have one classifier or regressifier, setting a new classifier will override any previous classifier or regressifier.
     
     @param const Classifier &classifier: a reference to the classifier module you want to add to the pipeline
     @return returns true if the classifier module was set successfully, false otherwise
     */
    bool setClassifier(const Classifier &classifier);

    /**
     Sets the regressifier at the core of the pipeline.  A pipeline can only have one classifier or regressifier, setting a new regressifier will override any previous classifier or regressifier.
     
     @param const Regressifier &regressifier: a reference to the regression module you want to add to the pipeline
     @return returns true if the regressifier module was set successfully, false otherwise
     */
    bool setRegressifier(const Regressifier &regressifier);

    /**
     Adds a new post processing module to the pipeline.  The user can specify the position at which the new module should be inserted into the list of post processing modules.  
     The default position is to insert the new module at the end of the list.
     
     @param const PostProcessing &postProcessingModule: a reference to the post processing module you want to add
     @param UINT insertIndex: the index of where you want to insert the new post processing module. Default is to insert the new module at the end of the list
     @return returns true if the post processing module was added successfully, false otherwise
     */
    bool addPostProcessingModule(const PostProcessing &postProcessingModule,UINT insertIndex = INSERT_AT_END_INDEX);

	/**
     This function removes any existing post processing modules, then it adds the new post processing module.
     
     @param const PostProcessing &postProcessingModule: a reference to the post processing module you want to add
     @return returns true if the post processing module was added successfully, false otherwise
     */
    bool setPostProcessingModule(const PostProcessing &postProcessingModule);
    
	/**
	 Adds a new context module to the pipeline.  The user can specify the position at which the new module should be inserted into the list of context modules.  
	 The default position is to insert the new module at the end of the list.
 
     @param const Context &contextModule: a reference to the context module that you want to add
     @param UINT contextLevel: the context level where you want to add the new context module
     @param UINT insertIndex: the index of where you want to insert the new context module. Default is to insert the new module at the end of the list
	 @return returns true if the context module was added successfully, false otherwise
	 */
	bool addContextModule(const Context &contextModule,UINT contextLevel,UINT insertIndex = INSERT_AT_END_INDEX);
    
    /**
	 Updates the context module at the specific contextLevel and moduleIndex.  
	 The user can specify the position at which the new module should be inserted into the list of context modules.  
	 The default position is to insert the new module at first contextLevel and first moduleIndex
 
     @param UINT contextLevel: the context level that contains the context module you want to update
     @param UINT moduleIndex: the index of the module you want to update
	 @return returns true if the context module was updated successfully, false otherwise
	 */
	bool updateContextModule(bool value,UINT contextLevel = 0,UINT moduleIndex = 0);

    /**
	 Removes all the preprocessing modules from the current pipeline. If the pipeline has been trained it will need to be retrained before it can be used.
 
	 @return returns true if preprocessing modules were removed successfully, false otherwise
	 */
    bool removeAllPreProcessingModules();

    /**
	 Removes the specific preprocessing module at the module index from the current pipeline. If the pipeline has been trained it will need to be retrained before it can be used.
 
     @param UINT moduleIndex: the index of the module you want to remove
	 @return returns true if the preprocessing module was removed successfully, false otherwise
	 */
    bool removePreProcessingModule(UINT moduleIndex);

    /**
	 Removes all the feature extraction modules from the current pipeline. If the pipeline has been trained it will need to be retrained before it can be used.
 
	 @return returns true if feature extraction modules were removed successfully, false otherwise
	 */
    bool removeAllFeatureExtractionModules();

    /**
	 Removes the specific feature extraction module at the module index from the current pipeline. If the pipeline has been trained it will need to be retrained before it can be used.
 
     @param UINT moduleIndex: the index of the module you want to remove
	 @return returns true if the feature extraction module was removed successfully, false otherwise
	 */
    bool removeFeatureExtractionModule(UINT moduleIndex);

    /**
	 Removes the classifier from the current pipeline.
 
	 @return returns true if classifier was removed successfully, false otherwise
	 */
    bool removeClassifier(){ deleteClassifier(); return true; }

    /**
	 Removes the regressifier from the current pipeline.
 
	 @return returns true if regressifier was removed successfully, false otherwise
	 */
	bool removeRegressifier(){ deleteRegressifier(); return true; }
	
	/**
	 Removes all the post processing extraction modules from the current pipeline. If the pipeline has been trained it will need to be retrained before it can be used.
 
	 @return returns true if post processing modules were removed successfully, false otherwise
	 */
    bool removeAllPostProcessingModules();

    /**
	 Removes the specific post processing module at the module index from the current pipeline. If the pipeline has been trained it will need to be retrained before it can be used.
 
     @param UINT moduleIndex: the index of the module you want to remove
	 @return returns true if the post processing module was removed successfully, false otherwise
	 */
    bool removePostProcessingModule(UINT moduleIndex);
    
    /**
	 Removes all the context modules from the current pipeline. If the pipeline has been trained it will need to be retrained before it can be used.
 
	 @return returns true if context modules were removed successfully, false otherwise
	 */
    bool removeAllContextModules();

    /**
	 Removes the specific context module at the module index from the current pipeline. If the pipeline has been trained it will need to be retrained before it can be used.
 
     @param UINT contextLevel: the context level that contains the specific context module you want to remove
     @param UINT moduleIndex: the index of the context module you want to remove
	 @return returns true if the context module was removed successfully, false otherwise
	 */
    bool removeContextModule(UINT contextLevel,UINT moduleIndex);

    /**
	 Removes everything from the pipeline.
 
	 @return returns true if all the modules were removed successfully, false otherwise
	 */
	bool clearAll();
    
    /**
	 Resets all the test results to zero.
     
	 @return returns true if the test results were cleared successfully, false otherwise
	 */
    bool clearTestResults();

protected:
	bool predict_classifier(VectorDouble &inputVector);
	bool predict_regressifier(VectorDouble &inputVector);
    void deleteAllPreProcessingModules();
    void deleteAllFeatureExtractionModules();
    void deleteClassifier();
    void deleteRegressifier();
    void deleteAllPostProcessingModules();
    void deleteAllContextModules();
    bool updateTestMetrics(const UINT classLabel,const UINT predictedClassLabel,VectorDouble &precisionCounter,VectorDouble &recallCounter,double &rejectionPrecisionCounter,double &rejectionRecallCounter,VectorDouble &confusionMatrixCounter);
    bool computeTestMetrics(VectorDouble &precisionCounter,VectorDouble &recallCounter,double &rejectionPrecisionCounter,double &rejectionRecallCounter,VectorDouble &confusionMatrixCounter,const UINT numTestSamples);
    inline double SQR(double x){ return x*x; }
    
    bool initialized;
    bool trained;
    UINT inputVectorDimensions;
    UINT outputVectorDimensions;
    UINT predictedClassLabel;
    UINT pipelineMode;
    UINT predictionModuleIndex;
    UINT numTrainingSamples;
    UINT numTestSamples;
    double testAccuracy;
    double testRMSError;
    double testSquaredError;
    double testTime;
    double trainingTime;
    VectorDouble testFMeasure;
    VectorDouble testPrecision;
    VectorDouble testRecall;
    VectorDouble regressionData;
    double testRejectionPrecision;
    double testRejectionRecall;
    MatrixDouble testConfusionMatrix;
    vector< ClassificationResult > testClassificationResults;
    
    vector< PreProcessing* > preProcessingModules;
    vector< FeatureExtraction* > featureExtractionModules;
    Classifier *classifier;
    Regressifier *regressifier;
    vector< PostProcessing* > postProcessingModules;
    vector< vector< Context* > > contextModules;
    
    enum PipelineModes{PIPELINE_MODE_NOT_SET=0,CLASSIFICATION_MODE,REGRESSION_MODE};
    
public:
    enum ContextLevels{START_OF_PIPELINE=0,AFTER_PREPROCESSING,AFTER_FEATURE_EXTRACTION,AFTER_CLASSIFIER,END_OF_PIPELINE,NUM_CONTEXT_LEVELS};
    
};

} //End of namespace GRT

#endif //GRT_GESTURE_RECOGNITION_PIPELINE_HEADER


