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

#ifndef GRT_MATRIX_DOUBLE_HEADER
#define GRT_MATRIX_DOUBLE_HEADER

#include "Matrix.h"
#include "MinMax.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include "DebugLog.h"
#include "ErrorLog.h"
#include "WarningLog.h"
#include "GRTTypedefs.h"

using namespace std;

namespace GRT{

class MatrixDouble : public Matrix<double>{
public:
    MatrixDouble();
    MatrixDouble(unsigned int rows,unsigned int cols);
    MatrixDouble(const MatrixDouble &rhs);
    MatrixDouble(const Matrix<double> &rhs);
    ~MatrixDouble();
    
    MatrixDouble& operator=(const MatrixDouble &rhs);
    MatrixDouble& operator=(const Matrix<double> &rhs);
    MatrixDouble& operator=(const vector< VectorDouble> &rhs);
    
    bool resize(unsigned int rows,unsigned int cols);
    
    bool print(string title="");
    bool transpose();
    bool multiple(double value);
    
    /**
        Performs the multiplcation of this matrix by the vector b.
        c = a * b;
     */
    VectorDouble multiple(const VectorDouble &b);
    
    /**
     Performs the multiplcation of this matrix by the matrix b.
     c = a * b;
     */
    MatrixDouble multiple(const MatrixDouble &b);
    
    VectorDouble getMean() const;
    VectorDouble getStdDev() const;
    MatrixDouble getCovarianceMatrix() const;
    
    std::vector< MinMax > getRanges() const;
    
    double getTrace() const;
    
protected:
    WarningLog warningLog;
    
};
    
} //End of namespace GRT

#endif //GRT_MATRIX_DOUBLE_HEADER