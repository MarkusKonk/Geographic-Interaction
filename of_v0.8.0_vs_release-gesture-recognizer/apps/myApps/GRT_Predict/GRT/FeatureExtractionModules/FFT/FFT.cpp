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

#include "FFT.h"

namespace GRT{
    
//Register the FFT module with the FeatureExtraction base class
RegisterFeatureExtractionModule< FFT > FFT::registerModule("FFT");

FFT::FFT(UINT fftWindowSize,UINT hopSize,UINT numDimensions,UINT fftWindowFunction,bool computeMagnitude,bool computePhase):tempBuffer(NULL){ 
    featureExtractionType = "FFT"; 
    initialized = false; 
    featureDataReady = false;
    numInputDimensions = 0;
    numOutputDimensions = 0;
    
    if( isPowerOfTwo(fftWindowSize) && hopSize > 0 && numDimensions > 0 ){
        init(fftWindowSize,hopSize,numDimensions,fftWindowFunction,computeMagnitude,computePhase);
    }
}
    
FFT::FFT(const FFT &rhs){
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}
    
FFT::~FFT(void){
    if( tempBuffer != NULL ){
        delete[] tempBuffer;
        tempBuffer = NULL;
    }
}
    
FFT& FFT::operator=(const FFT &rhs){
    if( this != &rhs ){
        if( tempBuffer != NULL ){
            delete[] tempBuffer;
            tempBuffer = NULL;
        }
        
        this->hopSize = rhs.hopSize;
        this->dataBufferSize = rhs.dataBufferSize;
        this->fftWindowSize = rhs.fftWindowSize;
        this->fftWindowFunction = rhs.fftWindowFunction;
        this->hopCounter = rhs.hopCounter;
        this->computeMagnitude = rhs.computeMagnitude;
        this->computePhase = rhs.computePhase;
        this->dataBuffer = rhs.dataBuffer;
        this->fft = rhs.fft;
        this->windowSizeMap = rhs.windowSizeMap;
		this->tempBuffer = NULL;
		if( this->dataBufferSize > 0 ) this->tempBuffer = new double[ this->dataBufferSize ];
        
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}

//Clone method
bool FFT::clone(const FeatureExtraction *featureExtraction){ 
        
    if( featureExtraction == NULL ) return false;
    
    if( this->getFeatureExtractionType() == featureExtraction->getFeatureExtractionType() ){
        //Invoke the equals operator to copy the data from the rhs instance to this instance
        *this = *(FFT*)featureExtraction;
        return true;
    }
    
    errorLog << "clone(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << endl;
    
    return false;
}
    
bool FFT::saveSettingsToFile(string filename){
    
    if( !initialized ){
        errorLog << "saveSettingsToFile(string filename) - The feature extraction module has not been initialized" << endl;
        return false;
    }
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveSettingsToFile( file ) ){
        file.close();
        return false;
    }
    
    file.close();
    
    return true;
}

bool FFT::loadSettingsFromFile(string filename){
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadSettingsFromFile( file ) ){
        file.close();
        initialized = false;
        return false;
    }
    
    file.close();
    
    return true;
}

bool FFT::saveSettingsToFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "saveSettingsToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    //Write the file header
    file << "GRT_FFT_FILE_V1.0" << endl;
    
    //Save the base settings to the file
    if( !saveBaseSettingsToFile( file ) ){
        errorLog << "saveSettingsToFile(fstream &file) - Failed to save base feature extraction settings to file!" << endl;
        return false;
    }
    
    //Write the FFT settings
    file << "HopSize: " << hopSize << endl;
    file << "FftWindowSize: " << fftWindowSize << endl;
    file << "FftWindowFunction: " << fftWindowFunction << endl;
    file << "ComputeMagnitude: " << computeMagnitude << endl;
    file << "ComputePhase: " << computePhase << endl;
    
    return true;
}

bool FFT::loadSettingsFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadSettingsFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_FFT_FILE_V1.0" ){
        errorLog << "loadSettingsFromFile(fstream &file) - Invalid file format!" << endl;
        return false;     
    }
    
    if( !loadBaseSettingsFromFile( file ) ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << endl;
        return false;
    }
    
    file >> word;
    if( word != "HopSize:" ){
        errorLog << "loadSettingsFromFile(fstream &file) - Failed to read HopSize header!" << endl;
        return false;     
    }
    file >> hopSize;
    
    file >> word;
    if( word != "FftWindowSize:" ){
        errorLog << "loadSettingsFromFile(fstream &file) - Failed to read FftWindowSize header!" << endl;
        return false;     
    }
    file >> fftWindowSize;
    
    file >> word;
    if( word != "FftWindowFunction:" ){
        errorLog << "loadSettingsFromFile(fstream &file) - Failed to read FftWindowFunction header!" << endl;
        return false;     
    }
    file >> fftWindowFunction;
    
    file >> word;
    if( word != "ComputeMagnitude:" ){
        errorLog << "loadSettingsFromFile(fstream &file) - Failed to read ComputeMagnitude header!" << endl;
        return false;     
    }
    file >> computeMagnitude;
    
    file >> word;
    if( word != "ComputePhase:" ){
        errorLog << "loadSettingsFromFile(fstream &file) - Failed to read ComputePhase header!" << endl;
        return false;     
    }
    file >> computePhase;
    
    //Init the FFT module to ensure everything is initialized correctly
    return init(fftWindowSize,hopSize,numInputDimensions,fftWindowFunction,computeMagnitude,computePhase);
}

bool FFT::init(UINT fftWindowSize,UINT hopSize,UINT numDimensions,UINT fftWindowFunction,bool computeMagnitude,bool computePhase){
    
    initialized = false;
    
    if( !isPowerOfTwo(fftWindowSize) ){
        errorLog << "init(UINT fftWindowSize,UINT hopSize,UINT numDimensions,UINT fftWindowFunction,bool computeMagnitude,bool computePhase) - fftWindowSize is not a power of two!" << endl;
        return false;
    }
    
    if( !validateFFTWindowFunction( fftWindowFunction ) ){
        errorLog << "init(UINT fftWindowSize,UINT hopSize,UINT numDimensions,UINT fftWindowFunction,bool computeMagnitude,bool computePhase) - Unkown Window Function!" << endl;
        return false;
    }
    
    if( tempBuffer != NULL ){
        delete[] tempBuffer;
        tempBuffer = NULL;
    }
       
    this->dataBufferSize = fftWindowSize;
    this->fftWindowSize = fftWindowSize;
    this->hopSize = hopSize;
    this->fftWindowFunction = fftWindowFunction;
    this->computeMagnitude = computeMagnitude;
    this->computePhase = computePhase;
    hopCounter = 0;
    featureDataReady = false;
    tempBuffer = new double[ dataBufferSize ];
    numInputDimensions = numDimensions;
    
    //Set the output size, the fftWindowSize is divided by 2 because the FFT is symmetrical so only half the actual FFT is returned
    numOutputDimensions = 0;
    if( computePhase ) numOutputDimensions += fftWindowSize/2 * numDimensions;
    if( computeMagnitude ) numOutputDimensions += fftWindowSize/2 * numDimensions;
    
    //Resize the output feature vector
    featureVector.clear();
    featureVector.resize( numOutputDimensions, 0);
    
    dataBuffer.clear();
    dataBuffer.resize(dataBufferSize,VectorDouble(numDimensions,0));
    fft.clear();
    fft.resize(numDimensions);
    
    for(unsigned int i=0; i<numDimensions; i++){
        if( !fft[i].init(fftWindowSize,fftWindowFunction,computeMagnitude,computePhase) ){
            errorLog << "init(UINT fftWindowSize,UINT hopSize,UINT numDimensions,UINT fftWindowFunction,bool computeMagnitude,bool computePhase) - Failed to initialize fft!" << endl;
            return false;
        }
    }
    
    initialized = true;

    return true;
}
    
bool FFT::computeFeatures(VectorDouble inputVector){ 
#ifdef GRT_SAFE_CHECKING
    if( !initialized ){
        errorLog << "computeFeatures(VectorDouble inputVector) - Not initialized!" << endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "computeFeatures(VectorDouble inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the FeatureExtraction (" << numInputDimensions << ")!" << endl;
        return false;
    }
#endif
    
    return update(inputVector);
}
    
bool FFT::update(double x){
#ifdef GRT_SAFE_CHECKING
    if( !initialized ){
        errorLog << "update(double x) - Not initialized!" << endl;
        return false;
    }
    
    if( numInputDimensions != 1 ){
        errorLog << "update(double x) - The size of the input (1) does not match that of the FeatureExtraction (" << numInputDimensions << ")!" << endl;
        return false;
    }
#endif
    
    return update(VectorDouble(1,x));
}

bool FFT::update(const VectorDouble &x){
#ifdef GRT_SAFE_CHECKING
    if( !initialized ){
        errorLog << "update(const VectorDouble &x) - Not initialized!" << endl;
        return false;
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "update(const VectorDouble &x) - The size of the input (" << x.size() << ") does not match that of the FeatureExtraction (" << numInputDimensions << ")!" << endl;
        return false;
    }
#endif

    //Add the current input to the data buffers
    dataBuffer.push_back(x);
    
    featureDataReady = false;
    
    if( ++hopCounter == hopSize ){
        hopCounter = 0;
        //Compute the FFT for each dimension
        for(UINT j=0; j<numInputDimensions; j++){
            
            //Copy the input data for this dimension into the temp buffer
            for(UINT i=0; i<dataBufferSize; i++){
                tempBuffer[i] = dataBuffer[i][j];
            }
            
            //Compute the FFT
            if( !fft[j].computeFFT( tempBuffer ) ){
                errorLog << "update(const VectorDouble &x) - Failed to compute FFT!" << endl;
                return false;
            }
        }
        
        //Flag that the fft was computed during this update
        featureDataReady = true;
        
        //Copy the FFT data to the feature vector
        UINT index = 0;
        for(UINT j=0; j<numInputDimensions; j++){
            if( computeMagnitude ){
                double *mag = fft[j].getMagnitudeDataPtr();
                for(UINT i=0; i<fft[j].getFFTSize()/2; i++){
                    featureVector[index++] = *mag++;
                }
            }
            if( computePhase ){
                double *phase = fft[j].getPhaseDataPtr();
                for(UINT i=0; i<fft[j].getFFTSize()/2; i++){
                    featureVector[index++] = *phase++;
                }
            }
        }
    }
    
    return true;
}
    
bool FFT::reset(){ 
    if( initialized ) return init(fftWindowSize,hopSize,numInputDimensions,fftWindowFunction,computeMagnitude,computePhase);
    return false;
}
    
UINT FFT::getHopSize(){ 
    if(initialized){ return hopSize; } 
    return 0; 
}
    
UINT FFT::getDataBufferSize(){ 
    if(initialized){ return dataBufferSize; } 
    return 0; 
}
    
UINT FFT::getFFTWindowSize(){ 
    
    if( !initialized ) return 0;
    return fftWindowSize;
}
    
UINT FFT::getFFTWindowFunction(){ 
    if(initialized){ return fftWindowFunction; } 
    return 0; 
}
    
UINT FFT::getHopCounter(){ 
    if(initialized){ return hopCounter; } 
    return 0; 
}
    
bool FFT::setHopSize(UINT hopSize){
    if( hopSize > 0 ){
        this->hopSize = hopSize;
        hopCounter = 0;
        return true;
    }
    errorLog << "setHopSize(UINT hopSize) - The hopSize value must be greater than zero!" << endl;
    return false;
}

bool FFT::setFFTWindowSize(UINT fftWindowSize){
    if( isPowerOfTwo(fftWindowSize) ){
        if( initialized ) return init(fftWindowSize, hopSize, numInputDimensions, fftWindowFunction, computeMagnitude, computePhase);
        this->fftWindowSize = fftWindowSize;
        return true;
    }
    errorLog << "setFFTWindowSize(UINT fftWindowSize) - fftWindowSize must be a power of two!" << endl;
    return false;

}
    
bool FFT::setFFTWindowFunction(UINT fftWindowFunction){
    if( validateFFTWindowFunction( fftWindowFunction ) ){
        this->fftWindowFunction = fftWindowFunction;
        return true;
    }
    return false;
}
    
bool FFT::setComputeMagnitude(bool computeMagnitude){
    if( initialized ) return init(fftWindowSize, hopSize, numInputDimensions, fftWindowFunction, computeMagnitude, computePhase);
    this->computeMagnitude = computeMagnitude;
    return true;
}
    
bool FFT::setComputePhase(bool computePhase){
    if( initialized ) return init(fftWindowSize, hopSize, numInputDimensions, fftWindowFunction, computeMagnitude, computePhase);
    this->computePhase = computePhase;
    return true;

}

bool FFT::isPowerOfTwo(unsigned int x){
    if (x < 2) return false;
    if (x & (x - 1)) return false;
    return true;
}
    
bool FFT::validateFFTWindowFunction(UINT fftWindowFunction){
    if( fftWindowFunction != RECTANGULAR_WINDOW && fftWindowFunction != BARTLETT_WINDOW && 
        fftWindowFunction != HAMMING_WINDOW && fftWindowFunction != HANNING_WINDOW ){
        return false;
    }
    return true;
}
    
    
}//End of namespace GRT