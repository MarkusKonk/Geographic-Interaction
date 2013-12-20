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
 The TrainingResult class provides a data structure for storing the results of a classification or regression training iteration.
 */

#ifndef GRT_TRAINING_RESULT_HEADER
#define GRT_TRAINING_RESULT_HEADER

namespace GRT {

class TrainingResult{
public:
    /**
     Default Constructor.
     
     Initializes the TrainingResult instance.
     */
    TrainingResult(){
        trainingMode = CLASSIFICATION_MODE;
        trainingIteration = 0;
        accuracy = 0;
        totalSquaredTrainingError = 0;
        rootMeanSquaredTrainingError = 0;
    }
    
    /**
     Copy Constructor.
     
     Initializes this instance by copying the data from the rhs instance
     
     @param const TrainingResult &rhs: another instance of the TrainingResult class
     */
    TrainingResult(const TrainingResult &rhs){
        *this = rhs;
    }
    
    /**
     Default Destructor.
     */
    ~TrainingResult(){
        
    }
    
    unsigned int getTrainingMode() const{
        return trainingMode;
    }
    
    unsigned int getTrainingIteration() const{
        return trainingIteration;
    }
    
    double getAccuracy() const{
        return accuracy;
    }
    
    double getTotalSquaredTrainingError() const{
        return rootMeanSquaredTrainingError;
    }
    
    double getRootMeanSquaredTrainingError() const{
        return rootMeanSquaredTrainingError;
    }
    
    bool setClassificationResult(unsigned int trainingIteration,double accuracy){
        this->trainingMode = CLASSIFICATION_MODE;
        this->trainingIteration = trainingIteration;
        this->accuracy = accuracy;
        return true;
    }
    
    bool setRegressionResult(unsigned int trainingIteration,double totalSquaredTrainingError,double rootMeanSquaredTrainingError){
        trainingMode = REGRESSION_MODE;
        this->trainingIteration = trainingIteration;
        this->totalSquaredTrainingError = totalSquaredTrainingError;
        this->rootMeanSquaredTrainingError = rootMeanSquaredTrainingError;
        return true;
    }

protected:
    
    unsigned int trainingMode;
    unsigned int trainingIteration;
    double accuracy;
    double totalSquaredTrainingError;
    double rootMeanSquaredTrainingError;
    
public:
    
    enum TrainingMode{CLASSIFICATION_MODE=0,REGRESSION_MODE};

};

}//End of namespace GRT

#endif //GRT_TRAINING_RESULT_HEADER