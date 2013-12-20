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

#ifndef GRT_LABELLED_CLASSIFICATION_SAMPLE_HEADER
#define GRT_LABELLED_CLASSIFICATION_SAMPLE_HEADER

#include "../Util/GRTCommon.h"

namespace GRT{

class LabelledClassificationSample{
public:
	LabelledClassificationSample();
	LabelledClassificationSample(UINT numDimensions_);
	LabelledClassificationSample(UINT classLabel,const VectorDouble &sample);
	LabelledClassificationSample(const LabelledClassificationSample &rhs);
	~LabelledClassificationSample();

	LabelledClassificationSample& operator= (const LabelledClassificationSample &rhs){
		if( this != &rhs){
			this->classLabel = rhs.classLabel;
			this->sample = rhs.sample;
			this->numDimensions = rhs.numDimensions;
		}
		return *this;
	}

	inline double& operator[] (const UINT &n){
		return sample[n];
	}

	void clear();

    //Getters
    UINT getNumDimensions(){ return numDimensions; }
    UINT getClassLabel(){ return classLabel; }
    VectorDouble getSample(){ return sample; }
    
    //Setters
	void set(UINT classLabel,const VectorDouble &sample);

private:
	UINT numDimensions;
	UINT classLabel;
	VectorDouble sample;

};

} //End of namespace GRT

#endif // GRT_LABELLED_CLASSIFICATION_SAMPLE_HEADER
