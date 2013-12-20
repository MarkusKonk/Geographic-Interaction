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
 The UnlabelledClassificationData class is the main data container for supporting unsupervised learning.
 */

#ifndef GRT_UNLABLELLED_CLASSIFICATION_DATA_HEADER
#define GRT_UNLABLELLED_CLASSIFICATION_DATA_HEADER

#include "../Util/GRTCommon.h"

namespace GRT{

class UnlabelledClassificationData{
public:
    
    /**
     Constructor, sets the name of the dataset and the number of dimensions of the training data.
     The name of the dataset should not contain any spaces.
	 
     @param UINT numDimensions: the number of dimensions of the training data, should be an unsigned integer greater than 0
     @param string datasetName: the name of the dataset, should not contain any spaces
     @param string infoText: some info about the data in this dataset, this can contain spaces
     */
    UnlabelledClassificationData(UINT numDimensions = 0,string datasetName = "NOT_SET",string infoText = "");

	/**
     Copy Constructor, copies the UnlabelledClassificationData from the rhs instance to this instance
     
	 @param const UnlabelledClassificationData &rhs: another instance of the UnlabelledClassificationData class from which the data will be copied to this instance
	*/
	UnlabelledClassificationData(const UnlabelledClassificationData &rhs);

	/**
     Default Destructor
    */
	~UnlabelledClassificationData();

	/**
     Sets the equals operator, copies the data from the rhs instance to this instance
     
	 @param const UnlabelledClassificationData &rhs: another instance of the UnlabelledClassificationData class from which the data will be copied to this instance
	 @return a reference to this instance of UnlabelledClassificationData
	*/
	UnlabelledClassificationData& operator= (const UnlabelledClassificationData &rhs);

	/**
     Array Subscript Operator, returns the UnlabelledClassificationData at index i.  
	 It is up to the user to ensure that i is within the range of [0 totalNumSamples-1]

	 @param const UINT &i: the index of the training sample you want to access.  Must be within the range of [0 totalNumSamples-1]
     @return the i'th row vector
    */
	inline VectorDouble operator[] (const UINT &i){
		return data.getRowVector(i);
	}

	/**
     Clears any previous training data and counters
    */
	void clear();
    
    /**
     Sets the number of dimensions in the training data. 
	 This should be an unsigned integer greater than zero.  
	 This will clear any previous training data and counters.
     This function needs to be called before any new samples can be added to the dataset, unless the numDimensions variable was set in the 
     constructor or some data was already loaded from a file
     
	 @param UINT numDimensions: the number of dimensions of the training data.  Must be an unsigned integer greater than zero
     @return true if the number of dimensions was correctly updated, false otherwise
     */
    bool setNumDimensions(UINT numDimensions);
    
    /**
     Sets the name of the dataset.
     There should not be any spaces in the name.
     Will return true if the name is set, or false otherwise.
     
	 @return returns true if the name is set, or false otherwise
     */
    bool setDatasetName(string datasetName);
    
    /**
     Sets the info string.
	 This can be any string with information about how the training data was recorded for example.
     
	 @param string infoText: the infoText
     @return true if the infoText was correctly updated, false otherwise
     */
    bool setInfoText(string infoText);

	/**
     Adds a new unlabelled sample to the dataset.  
     The dimensionality of the sample should match the number of dimensions in the UnlabelledClassificationData.

     @param UINT vector<double> sample: the new sample you want to add to the dataset.  The dimensionality of this sample should match the number of dimensions in the UnlabelledClassificationData
	 @return true if the sample was correctly added to the dataset, false otherwise
    */
	bool addSample(VectorDouble sample);
    
    /**
     Removes the last training sample added to the dataset.
     
	 @return true if the last sample was removed, false otherwise
     */
	bool removeLastSample();
    
    /**
     Sets the external ranges of the dataset, also sets if the dataset should be scaled using these values.  
     The dimensionality of the externalRanges vector should match the number of dimensions of this dataset.
     
	 @param vector< MinMax > externalRanges: an N dimensional vector containing the min and max values of the expected ranges of the dataset.
     @param bool useExternalRanges: sets if these ranges should be used to scale the dataset, default value is false.
	 @return returns true if the external ranges were set, false otherwise
     */
    bool setExternalRanges(vector< MinMax > externalRanges, bool useExternalRanges = false);
    
    /**
     Sets if the dataset should be scaled using an external range (if useExternalRanges == true) or the ranges of the dataset (if false).
     The external ranges need to be set FIRST before calling this function, otherwise it will return false.
     
     @param bool useExternalRanges: sets if these ranges should be used to scale the dataset
	 @return returns true if the useExternalRanges variable was set, false otherwise
     */
    bool enableExternalRangeScaling(bool useExternalRanges);

	/**
     Scales the dataset to the new target range.

	 @return true if the data was scaled correctly, false otherwise
    */
    bool scale(double minTarget,double maxTarget);
    
	/**
     Scales the dataset to the new target range, using the vector of ranges as the min and max source ranges.

	 @return true if the data was scaled correctly, false otherwise
    */
	bool scale(vector<MinMax> ranges,double minTarget,double maxTarget);
	
	/**
     Saves the unlabelled classification data to a custom file format.

	 @param string filename: the name of the file the data will be saved to
	 @return true if the data was saved successfully, false otherwise
    */
	bool saveDatasetToFile(string filename);
	
	/**
     Loads the unlabelled classification data from a custom file format.

	 @param string filename: the name of the file the data will be loaded from
	 @return true if the data was loaded successfully, false otherwise
    */
	bool loadDatasetFromFile(string filename);
    
    /**
     Saves the unlabelled classification data to a CSV file.
     This will save the sample data as N columns, where N is the number of dimensions in the data.  
     Each row will represent a sample.
     
	 @param string filename: the name of the file the data will be saved to
	 @return true if the data was saved successfully, false otherwise
     */
	bool saveDatasetToCSVFile(string filename);
	
	/**
     Loads the unlabelled classification data from a CSV file.
     This assumes the data is formatted with each row representing a sample.
     The sample data should be formatted as N columns, where N is the number of dimensions in the data.
     Each row should represent a sample
     
	 @param string filename: the name of the file the data will be loaded from
	 @return true if the data was loaded successfully, false otherwise
     */
	bool loadDatasetFromCSVFile(string filename);

	/**
     Partitions the dataset into a training dataset (which is kept by this instance of the UnlabelledClassificationData) and
	 a testing/validation dataset (which is returned as a new instance of a UnlabelledClassificationData).

	 @param UINT partitionPercentage: sets the percentage of data which remains in this instance, the remaining percentage of data is then returned as the testing/validation dataset
	 @return a new UnlabelledClassificationData instance, containing the remaining data not kept but this instance
    */
	UnlabelledClassificationData partition(UINT partitionPercentage);
    
    /**
     Adds the data in the unlabelledData set to the current instance of the UnlabelledClassificationData.
     The number of dimensions in both datasets must match.
     
	 @param UnlabelledClassificationData &unlabelledData: the dataset to add to this dataset
	 @return returns true if the datasets were merged, false otherwise
    */
    bool merge(UnlabelledClassificationData &unlabelledData);
    
    /**
     This function prepares the dataset for k-fold cross validation and should be called prior to calling the getTrainingFold(UINT foldIndex) or getTestingFold(UINT foldIndex) functions.  It will spilt the dataset into K-folds, as long as K < M, where M is the number of samples in the dataset.
     
	 @param UINT K: the number of folds the dataset will be split into, K should be less than the number of samples in the dataset
	 @return returns true if the dataset was split correctly, false otherwise
    */
    bool spiltDataIntoKFolds(UINT K);
    
    /**
     Returns the training dataset for the k-th fold for cross validation.  The spiltDataIntoKFolds(UINT K) function should have been called once before using this function.
     The foldIndex should be in the range [0 K-1], where K is the number of folds the data was spilt into.
     
	 @param UINT foldIndex: the index of the fold you want the training data for, this should be in the range [0 K-1], where K is the number of folds the data was spilt into 
	 @return returns a training dataset
    */
    UnlabelledClassificationData getTrainingFoldData(UINT foldIndex);
    
    /**
     Returns the test dataset for the k-th fold for cross validation.  The spiltDataIntoKFolds(UINT K) function should have been called once before using this function.
     The foldIndex should be in the range [0 K-1], where K is the number of folds the data was spilt into.
     
	 @param UINT foldIndex: the index of the fold you want the test data for, this should be in the range [0 K-1], where K is the number of folds the data was spilt into 
	 @return returns a test dataset
    */
    UnlabelledClassificationData getTestFoldData(UINT foldIndex);
    
    /**
     Gets the name of the dataset.
     
	 @return returns the name of the dataset
    */
    string getDatasetName(){ return datasetName; }
    
    /**
     Gets the infotext for the dataset
     
	 @return returns the infotext of the dataset
     */
    string getInfoText(){ return infoText; }
    
	/**
     Gets the number of dimensions of the labelled classification data.
     
	 @return an unsigned int representing the number of dimensions in the classification data
    */
	UINT inline getNumDimensions(){ return numDimensions; }
	
	/**
     Gets the number of samples in the classification data across all the classes.
     
	 @return an unsigned int representing the total number of samples in the classification data
    */
	UINT inline getNumSamples(){ return totalNumSamples; }
	
	/**
     Gets the ranges of the classification data.
     
	 @return a vector of minimum and maximum values for each dimension of the data
    */
	vector<MinMax> getRanges();
    
	/**
     Gets the unlabelled classification data as a matrix.
     
	 @return a Matrix containing the  UnlabelledClassificationSamples
    */
	MatrixDouble getDataAsMatrixDouble(){ return data; }

private:
    string datasetName;                                     ///< The name of the dataset
    string infoText;                                        ///< Some infoText about the dataset
	UINT numDimensions;										///< The number of dimensions in the dataset
	UINT totalNumSamples;                                   ///< The total number of samples in the dataset
    UINT kFoldValue;                                        ///< The number of folds the dataset has been spilt into for cross valiation
    bool crossValidationSetup;                              ///< A flag to show if the dataset is ready for cross validation
    bool useExternalRanges;                                 ///< A flag to show if the dataset should be scaled using the externalRanges values
    vector< MinMax > externalRanges;                        ///< A vector containing a set of externalRanges set by the user
	
	MatrixDouble data;                                  ///< The labelled classification data
    vector< vector< UINT > >    crossValidationIndexs;      ///< A vector to hold the indexs of the dataset for the cross validation
    
    DebugLog debugLog;                                      ///< Default debugging log
    ErrorLog errorLog;                                      ///< Default error log
    WarningLog warningLog;                                  ///< Default warning log
        
};

} //End of namespace GRT

#endif //GRT_UNLABLELLED_CLASSIFICATION_DATA_HEADER
