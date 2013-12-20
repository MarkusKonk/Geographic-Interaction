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
 Linear Regression is a simple but effective regression algorithm that can map an N-dimensional signal 
 to a 1-dimensional signal.
 */

#ifndef GRT_LINEAR_REGRESSION_HEADER
#define GRT_LINEAR_REGRESSION_HEADER

#include "../../GestureRecognitionPipeline/Regressifier.h"

namespace GRT{

class LinearRegression : public Regressifier
{
public:
    /**
     Default Constructor

     @param bool useScaling: sets if the training and real-time data should be scaled between [0 1]. Default value = false
     */
	LinearRegression(bool useScaling=false);
    
    /**
     Default Destructor
     */
	virtual ~LinearRegression(void);
    
    /**
     Defines how the data from the rhs LinearRegression should be copied to this LinearRegression
     
     @param const LRC &rhs: another instance of a LinearRegression
     @return returns a pointer to this instance of the LinearRegression
     */
	LinearRegression &operator=(const LinearRegression &rhs);
    
    /**
     This is required for the Gesture Recognition Pipeline for when the pipeline.setRegressifier(...) method is called.  
     It clones the data from the Base Class Regressifier pointer (which should be pointing to an Logistic Regression instance) into this instance
     
     @param Regressifier *regressifier: a pointer to the Regressifier Base Class, this should be pointing to another Logistic Regression instance
     @return returns true if the clone was successfull, false otherwise
    */
	virtual bool clone(const Regressifier *regressifier);
    
    /**
     This trains the Logistic Regression model, using the labelled regression data.
     This overrides the train function in the Regression base class.
     
     @param LabelledRegressionData &trainingData: a reference to the training data
     @return returns true if the LRC model was trained, false otherwise
    */
    virtual bool train(LabelledRegressionData &trainingData);
    
    /**
     This performs the regression by mapping the inputVector using the current Logistic Regression model.
     This overrides the predict function in the Regressifier base class.
     
     @param VectorDouble inputVector: the input vector to classify
     @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict(VectorDouble inputVector);
    
    /**
     This saves the trained Logistic Regression model to a file.
     This overrides the saveModelToFile function in the ML base class.
     
     @param string filename: the name of the file to save the Logistic Regression model to
     @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool saveModelToFile(string filename);
    
    /**
     This saves the trained Logistic Regression model to a file.
     This overrides the saveModelToFile function in the ML base class.
     
     @param fstream &file: a reference to the file the Logistic Regression model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file);
    
    /**
     This loads a trained Logistic Regression model from a file.
     This overrides the loadModelFromFile function in the ML base class.
     
     @param string filename: the name of the file to load the Logistic Regression model from
     @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool loadModelFromFile(string filename);
    
    /**
     This loads a trained Logistic Regression model from a file.
     This overrides the loadModelFromFile function in the Logistic Regression base class.
     
     @param fstream &file: a reference to the file the Logistic Regression model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);
    
    /**
     Sets the learningRate. This is used to update the weights at each step of the stochastic gradient descent.
     The learningRate value must be greater than zero.
     
     @param double learningRate: the learningRate value used during the training phase, must be greater than zero
     @return returns true if the value was updated successfully, false otherwise
     */
    bool setLearningRate(double learningRate);
    
    /**
     Sets the minimum change that must be achieved between two training epochs for the training to continue.
     The minChange value must be greater than zero.
     
     @param double minChange: the minimum change value, must be greater than zero
     @return returns true if the value was updated successfully, false otherwise
     */
    bool setMinChange(double minChange);
    
    /**
     Sets the maximum number of iterations that can be run during the training phase.
     The maxNumIterations value must be greater than zero.
     
     @param UINT maxNumIterations: the maximum number of iterations value, must be greater than zero
     @return returns true if the value was updated successfully, false otherwise
     */
    bool setMaxNumIterations(UINT maxNumIterations);
    
    /**
     Gets the current learningRate value, this is value used to update the weights at each step of the stochastic gradient descent.
     
     @return returns the current learningRate value
     */
    double getLearningRate();

    /**
     Gets the current min change value, this is the difference that must be achieved between two training epochs
     for the training to continue.
     
     @return returns the current min change value
     */
    double getMinChange();
    
    /**
     Gets the current maxNumIterations value, this is the maximum number of iterations that can be run during the training phase.
     
     @return returns the maxNumIterations value
     */
    UINT getMaxNumIterations();

private:
	
    double learningRate;
    double minChange;
    UINT maxNumIterations;
    double w0;
    VectorDouble w;
    static RegisterRegressifierModule< LinearRegression > registerModule;
};

} //End of namespace GRT

#endif //GRT_LINEAR_REGRESSION_HEADER

