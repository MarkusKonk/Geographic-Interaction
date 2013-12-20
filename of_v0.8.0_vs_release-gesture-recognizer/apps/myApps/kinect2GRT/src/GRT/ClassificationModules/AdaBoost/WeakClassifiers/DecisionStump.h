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
 */

#ifndef GRT_DECISION_STUMP_HEADER
#define GRT_DECISION_STUMP_HEADER

#include "WeakClassifier.h"

namespace GRT{
    
class DecisionStump : public WeakClassifier{
public:
    DecisionStump(UINT numSteps=100);
    
    virtual ~DecisionStump();
    
    DecisionStump(const DecisionStump &rhs);
    
    DecisionStump& operator=(const DecisionStump &rhs);
    
    virtual bool clone(const WeakClassifier *weakClassifer);
    
    virtual bool train(LabelledClassificationData &trainingData, VectorDouble &weights);
    
    virtual double predict(const VectorDouble &x);
    
    virtual bool saveModelToFile(fstream &file);
    
    virtual bool loadModelFromFile(fstream &file);
    
    virtual void print() const;
    
    UINT getDecisionFeatureIndex();
    
    UINT getDirection();
    
    UINT getNumSteps();
    
    double getDecisionValue();
protected:
    UINT decisionFeatureIndex;
    UINT direction;
    UINT numSteps;
    double decisionValue;
    
    static RegisterWeakClassifierModule< DecisionStump > registerModule;
};

} //End of namespace GRT

#endif //GRT_DECISION_STUMP_HEADER