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

#include "GRTBase.h"

namespace GRT{
    
GRTBase::GRTBase(void):debugLog("[DEBUG]"),errorLog("[ERROR]"),trainingLog("[TRAINING]"),testingLog("[TESTING]"),warningLog("[WARNING]"){
    warningMessage = "";
    errorMessage = "";
}
    
GRTBase::~GRTBase(void){
}
    
bool GRTBase::copyGRTBaseVariables(const GRTBase *base){
    
    if( base == NULL ){
        errorMessage = "copyBaseVariables(const GRTBase *base) - base pointer is NULL!";
        errorLog << errorMessage << endl;
        return false;
    }
    
    this->warningMessage = base->warningMessage;
    this->errorMessage = base->errorMessage;
    this->debugLog = base->debugLog;
    this->errorLog = base->errorLog;
    this->trainingLog = base->trainingLog;
    this->testingLog = base->testingLog;
    this->warningLog = base->warningLog;
    
    return true;
}
    
    
string GRTBase::getLastWarningMessage() const {
    return warningMessage;
}

string GRTBase::getLastErrorMessage() const {
    return errorMessage;
}
    
string GRTBase::getGRTVersion(bool returnRevision) const {
    string version = GRT_VERSION;
    if( returnRevision ) version += " revision: " + getGRTRevison();
    return version;
}

string GRTBase::getGRTRevison() const{
    return GRT_REVISION;
}

} //End of namespace GRT

