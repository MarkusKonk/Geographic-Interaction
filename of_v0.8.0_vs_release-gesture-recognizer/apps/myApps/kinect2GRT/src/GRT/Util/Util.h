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
 This file contains the Util class, a wrapper for a number of generic functions that are used throughout the GRT.
 This includes functions for scaling data, finding the minimum or maximum values in a double or UINT vector, etc.
 Many of these functions are static functions, which enables you to use them without having to create a new Util instance, 
 for instance, you can directly call: Util::sleep( 1000 ); to use the sleep function. 
 */

#ifndef GRT_UTIL_HEADER
#define GRT_UTIL_HEADER

//Include the GRTVersionInfo header to find which operating system we are building for
#include "GRTVersionInfo.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <math.h>

#ifdef __GRT_WINDOWS_BUILD__
//Include any Windows specific headers
#include <windows.h>
//Hey User: Make sure you add the path to the Kernel32.lib to your lib search paths
//#pragma comment(lib,"Kernel32.lib")
//The min and max macros cause major headaches, so undefine them
#undef min
#undef max
#endif

#ifdef __GRT_OSX_BUILD__
//Include any OSX specific headers
#include <unistd.h>
#endif

#ifdef __GRT_LINUX_BUILD__
//Include any Linux specific headers
#include <unistd.h>
#endif

namespace GRT{

class Util{
public:
	/**
	Default constructor.
	*/
    Util(){}

	/**
	Default destructor.
	*/
    ~Util(){}
    
	/**
	A cross platform function to cause a function to sleep for a set number of milliseconds.
	
	@param unsigned int numMilliseconds: the number of milliseconds to sleep for, 1000 milliseconds = 1 second.
	@return returns true if the sleep function was called successfully, false otherwise
	*/
    static bool sleep(unsigned int numMilliseconds);
    
    /**
     Performs minmax scaling. The input value (x) will be scaled from the source range to the target range.
     
	 @param double x: the input value to be scaled
	 @param double minSource: the minimum source value (that x originates from)
	 @param double maxSource: the maximum source value (that x originates from)
	 @param double minTarget: the minimum target value (that x will be scaled to)
	 @param double maxTarget: the maximum target value (that x will be scaled to)
	 @return the scaled value
     */
    static double scale(double x,double minSource,double maxSource,double minTarget,double maxTarget);

    /**
    Converts an int to a string.

    @param int i: the value you want to convert to a string
    @return std::string: the value as a string
	*/
    static std::string intToString(int i);

    /**
    Converts an unsigned int to a string.

    @param unsigned int i: the value you want to convert to a string
    @return std::string: the value as a string
	*/
    static std::string intToString(unsigned int i);

    /**
    Converts an unsigned int to a string.

    @param unsigned int i: the value you want to convert to a string
    @return std::string: the value as a string
	*/
    static std::string toString(int i);

	/**
    Converts an unsigned int to a string.

    @param unsigned int i: the value you want to convert to a string
	@return std::string: the value as a string
	*/
    static std::string toString(unsigned int i);

	/**
    Converts a double to a string.

    @param double v: the value you want to convert to a string
    @return std::string: the value as a string
	*/
    static std::string toString(double v);

	/**
    Converts a float to a string.

    @param float v: the value you want to convert to a string
    @return std::string: the value as a string
	*/
    static std::string toString(float v);

    /**
    Converts a string to an int.

    @param const std::string &s: the value you want to convert to an int
	@return int: the value as an int
	*/
    static int stringToInt(const std::string &s);

    /**
    Converts a string to a double.

    @param const std::string &s: the value you want to convert to a double
	@return double: the value as a double
	*/
    static double stringToDouble(const std::string &s);
    
    /**
     Limits the input value so it is between the range of minValue and maxValue.
     If the input value is below the minValue then the output of the function will be the minValue.
     If the input value is above the maxValue then the output of the function will be the maxValue.
     Otherwise, the out of the function will be the input.
     
     @param double value: the input value that should be limited
     @param double minValue: the minimum value that should be limited
     @param double maxValue: the maximum value that should be limited
     @return the limited input value
     */
    static double limit(double value,double minValue,double maxValue);

    /**
    Computes the sum of the vector x.

    @param const std::vector<double> &x: the vector of values you want to sum
	@return double: the sum of the input vector x
	*/
    static double sum(const std::vector<double> &x);

    /**
    Computes the dot product between the two input vectors. The two input vectors must have the same size.

    @param const std::vector<double> &a: the first vector for the dot product
    @param const std::vector<double> &a: the second vector for the dot product
	@return double: the dot product between the two input vectors, if the two input vectors are not the same size then the dist will be INF
	*/
    static double dotProduct(const std::vector<double> &a,const std::vector<double> &b);

    /**
    Computes the euclidean distance between the two input vectors. The two input vectors must have the same size.

    @param const std::vector<double> &a: the first vector for the euclidean distance
    @param const std::vector<double> &a: the second vector for the euclidean distance
	@return double: the euclidean distance between the two input vectors, if the two input vectors are not the same size then the dist will be INF
	*/
    static double euclideanDistance(const std::vector<double> &a,const std::vector<double> &b);

    /**
    Computes the manhattan distance between the two input vectors. The two input vectors must have the same size.
    The manhattan distance is also known as the L1 norm, taxicab distance, city block distance, or rectilinear distance.

    @param const std::vector<double> &a: the first vector for the manhattan distance
    @param const std::vector<double> &a: the second vector for the manhattan distance
	@return double: the manhattan distance between the two input vectors, if the two input vectors are not the same size then the dist will be INF
	*/
    static double manhattanDistance(const std::vector<double> &a,const std::vector<double> &b);

    /**
    Computes the cosine distance between the two input vectors. The two input vectors must have the same size.
    The cosine distance can be used as a similarity measure, the distance ranges from −1 meaning exactly opposite, to 1 meaning exactly the same, 
    with 0 usually indicating independence, and in-between values indicating intermediate similarity or dissimilarity.

    @param const std::vector<double> &a: the first vector for the cosine distance
    @param const std::vector<double> &a: the second vector for the cosine distance
	@return double: the cosine distance between the two input vectors, if the two input vectors are not the same size then the dist will be INF
	*/
    static double cosineDistance(const std::vector<double> &a,const std::vector<double> &b);

    /**
    Computes the sum of the vector x and then uses this to normalize the vector

    @param const std::vector<double> &x: the vector of values you want to normalize
	@return std::vector<double>: the normalized input vector (the sum of which will be 1)
	*/
	static std::vector<double> normalize(const std::vector<double> &x);
	
	/**
    Gets the minimum value in the input vector.

    @param const std::vector<double> &x: the vector of values you want to find the minimum value for
	@return double: the minimum value in the input vector, this will be INF if the input vector size is 0
	*/
    static double getMin(const std::vector< double > &x);

    /**
    Gets the maximum value in the input vector.

    @param const std::vector<double> &x: the vector of values you want to find the maximum value for
	@return double: the maximum value in the input vector, this will be INF if the input vector size is 0
	*/
    static double getMax(const std::vector< double > &x);

    /**
    Gets the minimum value in the input vector.

    @param const std::vector<unsigned int> &x: the vector of values you want to find the minimum value for
	@return unsigned int: the minimum value in the input vector, this will be INF if the input vector size is 0
	*/
    static unsigned int getMin(const std::vector< unsigned int > &x);

    /**
    Gets the maximum value in the input vector.

    @param const std::vector<unsigned int> &x: the vector of values you want to find the maximum value for
	@return unsigned int: the maximum value in the input vector, this will be INF if the input vector size is 0
	*/
    static unsigned int getMax(const std::vector< unsigned int > &x);
    
    /**
     Converts the cartesian values {x y} into polar values {r theta}
     
     @param double x: the x cartesian value
     @param double y: the y cartesian value
     @param double &r: the return radius value
     @param double &theta: the return theta value
     @return void
     */
    static void cartToPolar(double x,double y,double &r, double &theta);

};
    
} //End of namespace GRT

#endif // GRT_UTIL_HEADER