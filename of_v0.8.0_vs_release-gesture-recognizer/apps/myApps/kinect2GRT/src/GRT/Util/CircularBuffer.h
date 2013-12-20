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

#ifndef GRT_CIRCULAR_BUFFER_HEADER
#define GRT_CIRCULAR_BUFFER_HEADER
#include <iostream>
#include <vector>
#include "ErrorLog.h"
using namespace std;

namespace GRT{
    
template <class T>
class CircularBuffer{
public:
    CircularBuffer(){
        errorLog.setProceedingText("[ERROR: CircularBuffer]");
        bufferSize = 0;
        numValuesInBuffer = 0;
        readPtr = 0;
        writePtr = 0;
        bufferInit = false;
    }
    
    CircularBuffer(const CircularBuffer &rhs){
        errorLog.setProceedingText("[ERROR: CircularBuffer]");
        this->bufferSize = 0;
        this->numValuesInBuffer = 0;
        this->readPtr = 0;
        this->writePtr = 0;
        this->bufferInit = false;
        
        if( rhs.bufferInit ){
            this->resize( rhs.bufferSize );
            this->readPtr = 0;
            this->writePtr = 0;
            for(unsigned int i=0; i<rhs.numValuesInBuffer; i++){
                this->push_back( rhs.buffer[ i ] );
            }
            this->readPtr = rhs.readPtr;
            this->writePtr = rhs.writePtr;
        }
    }
    
    CircularBuffer(unsigned int bufferSize){
        errorLog.setProceedingText("[ERROR: CircularBuffer]");
        bufferInit = false;
        resize(bufferSize);
    }
    
    ~CircularBuffer(){
        if( bufferInit ){
            clear();
        }
    }
    
    CircularBuffer& operator=(const CircularBuffer &rhs){
        if(this!=&rhs){
            this->clear();
            
            if( rhs.bufferInit ){
                this->resize( rhs.bufferSize );
                this->readPtr = 0;
                this->writePtr = 0;
                for(unsigned int i=0; i<rhs.numValuesInBuffer; i++){
                    this->push_back( rhs.buffer[ i ] );
                }
                this->readPtr = rhs.readPtr;
                this->writePtr = rhs.writePtr;
            }
        }
        return *this;
    }
    
    //This is the main access operator and will return a value relative to the current read pointer
    inline T& operator[](const unsigned int &index){
        return buffer[ (readPtr + index) % bufferSize ];
    }
    
    //Same as above but const
    inline const T& operator[](const unsigned int &index) const{
        return buffer[ (readPtr + index) % bufferSize ];
    }
    
    //This is a special access operator that will return the value at the specific index (regardless of the position of the read pointer)
    inline T& operator()(const unsigned int &index){
        return buffer[ index ];
    }
    
    //Same as above but const
    inline const T& operator()(const unsigned int &index) const{
        return buffer[ index ];
    }
    
    bool resize(unsigned int newBufferSize){
        return resize(newBufferSize,T());
    }
    
    bool resize(unsigned int newBufferSize,T defaultValue){
        
        //Cleanup the old memory
        clear();
        
        if( newBufferSize == 0 ) return false;
        
        //Setup the memory for the new buffer
        bufferSize = newBufferSize;
        buffer.resize(bufferSize,defaultValue);
        numValuesInBuffer = 0;
        readPtr = 0;
        writePtr = 0;
        
        //Flag that the filter has been initialised
        bufferInit = true;
        
        return true;
    }
    
    bool push_back(const T &value){
        
        if( !bufferInit ){
            errorLog << "Can't push_back value to circular buffer as the buffer has not been initialized!" << endl;
            return false;
        }
    
		//Add the value to the buffer
        buffer[ writePtr ] = value;

		//Update the write pointer
        writePtr = ++writePtr % bufferSize;
        
        //Check if the buffer is full
        if( ++numValuesInBuffer > bufferSize ){
            numValuesInBuffer = bufferSize;

			//Only update the read pointer if the buffer has been filled
	        readPtr = ++readPtr % bufferSize;
        }

        return true;
    }
    
    bool setAllValues(const T &value){
        if( !bufferInit ){
            return false;
        }
        
        for(unsigned int i=0; i<bufferSize; i++){
            buffer[i] = value;
        }
        
        return true;
    }
    
    void clear(){
		numValuesInBuffer = 0;
		readPtr = 0;
		writePtr = 0;
		buffer.clear();
		bufferInit = false;
    }
    
    vector< T > getDataAsVector() const{
        if( bufferInit ){
            vector< T > data( bufferSize );
            for(unsigned int i=0; i<bufferSize; i++){
                data[i] = (*this)[i];
            }
            return data;
        }
        return vector< T >();
    }
    
    bool getInit() const { return bufferInit; }
    bool getBufferFilled() const { return bufferInit ? numValuesInBuffer==bufferSize : false; }
    unsigned int getSize() const { return bufferInit ? bufferSize : 0; }
    unsigned int getNumValuesInBuffer() const { return bufferInit ? numValuesInBuffer : 0; }
    unsigned int getReadPointerPosition() const { return bufferInit ? readPtr : 0; }
    unsigned int getWritePointerPosition() const { return bufferInit ? writePtr : 0; }
    
protected:
    unsigned int bufferSize;
    unsigned int numValuesInBuffer;
    unsigned int readPtr;
    unsigned int writePtr;
    vector< T > buffer;
    bool bufferInit;
    
    ErrorLog errorLog;
};


}//End of namespace GRT

#endif //GRT_CIRCULAR_BUFFER_HEADER