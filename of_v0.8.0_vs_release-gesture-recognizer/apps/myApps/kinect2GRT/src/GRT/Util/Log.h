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

#ifndef GRT_LOG_HEADER
#define GRT_LOG_HEADER

#include <iostream>
#include <string.h>

namespace GRT{

class Log{
public:
    Log(std::string proceedingText = ""){ setProceedingText(proceedingText); loggingEnabledPtr = NULL; writeProceedingText = true; }
    ~Log(){}
    
    Log& operator<< (bool val ){
        if( *loggingEnabledPtr ){
            std::cout << proceedingText.c_str() << val;
        }
        return *this;
    }
    
    Log& operator<< (short val ){
        if( *loggingEnabledPtr ){
            if( writeProceedingText ){
                writeProceedingText = false;
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
        }
        return *this;
    }

    Log& operator<< (unsigned short val ){
        if( *loggingEnabledPtr ){
            if( writeProceedingText ){
                writeProceedingText = false;
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
        }
        return *this;
    }

    Log& operator<< (int val ){
        if( *loggingEnabledPtr ){
            if( writeProceedingText ){
                writeProceedingText = false;
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
        }
        return *this;
    }

    Log& operator<< (unsigned int val ){
        if( *loggingEnabledPtr ){
            if( writeProceedingText ){
                writeProceedingText = false;
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
        }
        return *this;
    }

    Log& operator<< (long val ){
        if( *loggingEnabledPtr ){
            if( writeProceedingText ){
                writeProceedingText = false;
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
        }
        return *this;    }

    Log& operator<< (unsigned long val ){
        if( *loggingEnabledPtr ){
            if( writeProceedingText ){
                writeProceedingText = false;
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
        }
        return *this;    }

    Log& operator<< (float val ){
        if( *loggingEnabledPtr ){
            if( writeProceedingText ){
                writeProceedingText = false;
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
        }
        return *this;    }

    Log& operator<< (double val ){
        if( *loggingEnabledPtr ){
            if( writeProceedingText ){
                writeProceedingText = false;
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
        }
        return *this;
    }

    Log& operator<< (long double val ){
        if( *loggingEnabledPtr ){
            if( writeProceedingText ){
                writeProceedingText = false;
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
        }
        return *this;    }

    Log& operator<< (void* val ){
        if( *loggingEnabledPtr ){
            if( writeProceedingText ){
                writeProceedingText = false;
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
        }
        return *this;
    }
    
    Log& operator<< (std::string val ){
        if( *loggingEnabledPtr ){
            if( writeProceedingText ){
                writeProceedingText = false;
                std::cout << proceedingText.c_str();
            }
            std::cout << val.c_str();
        }
        return *this;
    }
    
    Log& operator<< (const char* val ){
        if( *loggingEnabledPtr ){
            if( writeProceedingText ){
                writeProceedingText = false;
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
        }
        return *this;
    }
    
    // this is the type of std::cout
    typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
    
    // this is the function signature of std::endl
    typedef CoutType& (*StandardEndLine)(CoutType&);
    
    // define an operator<< to take in std::endl
    Log& operator<<(StandardEndLine manip)
    {
        if( *loggingEnabledPtr ){
            // call the function, but we cannot return it's value
            manip(std::cout);
            writeProceedingText = true;
        }
        
        return *this;
    }

    
    //Getters
    virtual bool getLoggingEnabled(){ return false; }
    std::string getProceedingText(){ return proceedingText; }
    virtual std::string getLastMessage(){ return ""; }
    
    //Setters
    void setProceedingText(std::string proceedingText){ this->proceedingText = proceedingText; this->proceedingText+=": "; }

protected:
    std::string proceedingText;
    bool *loggingEnabledPtr;
    bool writeProceedingText;
};

}; //End of namespace GRT

#endif //GRT_LOG_HEADER