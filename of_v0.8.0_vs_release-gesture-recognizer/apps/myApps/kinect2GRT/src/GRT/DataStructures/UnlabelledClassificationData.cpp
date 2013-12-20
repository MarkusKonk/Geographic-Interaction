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

#include "UnlabelledClassificationData.h"

namespace GRT{

UnlabelledClassificationData::UnlabelledClassificationData(UINT numDimensions,string datasetName,string infoText):debugLog("[DEBUG ULCD]"),errorLog("[ERROR ULCD]"),warningLog("[WARNING ULCD]"){
    this->datasetName = datasetName;
    this->numDimensions = numDimensions;
    this->infoText = infoText;
    totalNumSamples = 0;
    crossValidationSetup = false;
    useExternalRanges = false;
    if( numDimensions > 0 ) setNumDimensions( numDimensions );
}

UnlabelledClassificationData::UnlabelledClassificationData(const UnlabelledClassificationData &rhs):debugLog("[DEBUG ULCD]"),errorLog("[ERROR ULCD]"),warningLog("[WARNING ULCD]"){
    *this = rhs;
}

UnlabelledClassificationData::~UnlabelledClassificationData(){}
    
UnlabelledClassificationData& UnlabelledClassificationData::operator=(const UnlabelledClassificationData &rhs){
    if( this != &rhs){
        this->datasetName = rhs.datasetName;
        this->infoText = rhs.infoText;
        this->numDimensions = rhs.numDimensions;
        this->totalNumSamples = rhs.totalNumSamples;
        this->kFoldValue = rhs.kFoldValue;
        this->crossValidationSetup = rhs.crossValidationSetup;
        this->useExternalRanges = rhs.useExternalRanges;
        this->externalRanges = rhs.externalRanges;
        this->data = rhs.data;
        this->crossValidationIndexs = rhs.crossValidationIndexs;
        this->debugLog = rhs.debugLog;
        this->errorLog = rhs.errorLog;
        this->warningLog = rhs.warningLog;
    }
    return *this;
}

void UnlabelledClassificationData::clear(){
	totalNumSamples = 0;
	data.clear();
    crossValidationSetup = false;
    crossValidationIndexs.clear();
}

bool UnlabelledClassificationData::setNumDimensions(UINT numDimensions){

    if( numDimensions > 0 ){
        //Clear any previous training data
        clear();

        //Set the dimensionality of the data
        this->numDimensions = numDimensions;

        //Clear the external ranges
        useExternalRanges = false;
        externalRanges.clear();

        return true;
    }
    return false;
}

bool UnlabelledClassificationData::setDatasetName(string datasetName){

    //Make sure there are no spaces in the string
    if( datasetName.find(" ") == string::npos ){
        this->datasetName = datasetName;
        return true;
    }

    return false;
}

bool UnlabelledClassificationData::setInfoText(string infoText){
    this->infoText = infoText;
    return true;
}

bool UnlabelledClassificationData::addSample(vector<double> sample){
	if( sample.size() != numDimensions ) return false;

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

	data.push_back( sample );
	totalNumSamples++;

	return true;
}

bool UnlabelledClassificationData::removeLastSample(){

    if( totalNumSamples > 0 ){

        //The dataset has changed so flag that any previous cross validation setup will now not work
        crossValidationSetup = false;
        crossValidationIndexs.clear();

        //If there is only one sample then we just need to clear the buffer
        if( totalNumSamples == 1 ){
            data.clear();
            return true;
        }

        //Create a temporary Matrix to copy the data into
        MatrixDouble tempData(data.getNumRows()-1,data.getNumCols());

        //Copy the data
        for(UINT i=0; i<tempData.getNumRows()-1; i++){
            for(UINT j=0; j<tempData.getNumCols(); j++){
                tempData[i][j] = data[i][j];
            }
        }

        return true;

    }else return false;

}

bool UnlabelledClassificationData::setExternalRanges(vector< MinMax > externalRanges, bool useExternalRanges){

    if( externalRanges.size() != numDimensions ) return false;

    this->externalRanges = externalRanges;
    this->useExternalRanges = useExternalRanges;

    return true;
}

bool UnlabelledClassificationData::enableExternalRangeScaling(bool useExternalRanges){
    if( externalRanges.size() == numDimensions ){
        this->useExternalRanges = useExternalRanges;
        return true;
    }
    return false;
}

bool UnlabelledClassificationData::scale(double minTarget,double maxTarget){
    vector< MinMax > ranges = getRanges();
    return scale(ranges,minTarget,maxTarget);
}

bool UnlabelledClassificationData::scale(vector<MinMax> ranges,double minTarget,double maxTarget){
    if( ranges.size() != numDimensions ) return false;

    //Scale the training data
    for(UINT i=0; i<totalNumSamples; i++){
        for(UINT j=0; j<numDimensions; j++){
            data[i][j] = Util::scale(data[i][j],ranges[j].minValue,ranges[j].maxValue,minTarget,maxTarget);
        }
    }

    return true;
}

bool UnlabelledClassificationData::saveDatasetToFile(string filename){

	std::fstream file;
	file.open(filename.c_str(), std::ios::out);

	if( !file.is_open() ){
        errorLog << "saveDatasetToFile(string filename) - Failed to open file!" << endl;
		return false;
	}

	file << "GRT_UNLABELLED_CLASSIFICATION_DATA_FILE_V1.0\n";
    file << "DatasetName: " << datasetName << endl;
    file << "InfoText: " << infoText << endl;
	file << "NumDimensions: " << numDimensions << endl;
	file << "TotalNumTrainingExamples: " << totalNumSamples << endl;

    file << "UseExternalRanges: " << useExternalRanges << endl;

    if( useExternalRanges ){
        for(UINT i=0; i<externalRanges.size(); i++){
            file << externalRanges[i].minValue << "\t" << externalRanges[i].maxValue << endl;
        }
    }

	file << "UnlabelledTrainingData:\n";

	for(UINT i=0; i<totalNumSamples; i++){
		for(UINT j=0; j<numDimensions; j++){
            if( j != 0 ) file << "\t";
			file << data[i][j];
		}
		file << endl;
	}

	file.close();
	return true;
}

bool UnlabelledClassificationData::loadDatasetFromFile(string filename){

	std::fstream file;
	file.open(filename.c_str(), std::ios::in);
	clear();

	if( !file.is_open() ){
        errorLog << "loadDatasetFromFile(string filename) - could not open file!" << endl;
		return false;
	}

	string word;

	//Check to make sure this is a file with the Training File Format
	file >> word;
	if(word != "GRT_UNLABELLED_CLASSIFICATION_DATA_FILE_V1.0"){
        errorLog << "loadDatasetFromFile(string filename) - could not find file header!" << endl;
		file.close();
		return false;
	}

    //Get the name of the dataset
	file >> word;
	if(word != "DatasetName:"){
        errorLog << "loadDatasetFromFile(string filename) - failed to find DatasetName!" << endl;
		file.close();
		return false;
	}
	file >> datasetName;

    file >> word;
	if(word != "InfoText:"){
        errorLog << "loadDatasetFromFile(string filename) - failed to find InfoText!" << endl;
		file.close();
		return false;
	}

    //Load the info text
    file >> word;
    infoText = "";
    while( word != "NumDimensions:" ){
        infoText += word + " ";
        file >> word;
    }

	//Get the number of dimensions in the training data
	if(word != "NumDimensions:"){
        errorLog << "loadDatasetFromFile(string filename) - failed to find DatasetName!" << endl;
		file.close();
		return false;
	}
	file >> numDimensions;

	//Get the total number of training examples in the training data
	file >> word;
	if(word != "TotalNumTrainingExamples:"){
        errorLog << "loadDatasetFromFile(string filename) - failed to find DatasetName!" << endl;
		file.close();
		return false;
	}
	file >> totalNumSamples;

    //Check if the dataset should be scaled using external ranges
	file >> word;
	if(word != "UseExternalRanges:"){
        errorLog << "loadDatasetFromFile(string filename) - failed to find DatasetName!" << endl;
		file.close();
		return false;
	}
    file >> useExternalRanges;

    //If we are using external ranges then load them
    if( useExternalRanges ){
        externalRanges.resize(numDimensions);
        for(UINT i=0; i<externalRanges.size(); i++){
            file >> externalRanges[i].minValue;
            file >> externalRanges[i].maxValue;
        }
    }

	//Get the main training data
	file >> word;
	if(word != "UnlabelledTrainingData:"){
        errorLog << "loadDatasetFromFile(string filename) - failed to find DatasetName!" << endl;
		file.close();
		return false;
	}
	data.resize( totalNumSamples, numDimensions );

	for(UINT i=0; i<totalNumSamples; i++){
		for(UINT j=0; j<numDimensions; j++){
			file >> data[i][j];
		}
	}

	file.close();
	return true;
}


bool UnlabelledClassificationData::saveDatasetToCSVFile(string filename){

    std::fstream file;
	file.open(filename.c_str(), std::ios::out );

	if( !file.is_open() ){
        errorLog << "saveDatasetToCSVFile(string filename) - Failed to open file!" << endl;
		return false;
	}

    //Write the data to the CSV file
    for(UINT i=0; i<totalNumSamples; i++){
		for(UINT j=0; j<numDimensions; j++){
            if( j != 0 ) file << ",";
			file << data[i][j];
		}
		file << endl;
	}

	file.close();

    return true;
}

bool UnlabelledClassificationData::loadDatasetFromCSVFile(string filename){

    string value;
    datasetName = "NOT_SET";
    infoText = "";

    //Clear any previous data
    clear();
    
    //Parse the CSV file
    FileParser parser;
    
    if( !parser.parseCSVFile(filename,true) ){
        errorLog << "loadDatasetFromCSVFile(string filename) - Failed to parse CSV file!" << endl;
        return false;
    }
    
    if( !parser.getConsistentColumnSize() ){
        errorLog << "loadDatasetFromCSVFile(string filename) - The CSV file does not have a consistent number of columns!" << endl;
        return false;
    }
    
    //Setup the labelled classification data
    numDimensions = parser.getColumnSize();

    VectorDouble sample(numDimensions);
    for(UINT i=0; i<parser.getRowSize(); i++){
        
        //Get the input vector
        for(UINT j=0; j<numDimensions; j++){
            sample[j] = Util::stringToDouble( parser[i][j] );
        }
        
        //Add the labelled sample to the dataset
        if( !addSample(sample) ){
            warningLog << "loadDatasetFromCSVFile(string filename) - Could not add sample " << i << " to the dataset!" << endl;
        }
    }
    
    return true;
}

UnlabelledClassificationData UnlabelledClassificationData::partition(UINT trainingSizePercentage){

    //Partitions the dataset into a training dataset (which is kept by this instance of the UnlabelledClassificationData) and
	//a testing/validation dataset (which is return as a new instance of the UnlabelledClassificationData).  The trainingSizePercentage
	//therefore sets the size of the data which remains in this instance and the remaining percentage of data is then added to
	//the testing/validation dataset

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

	const UINT numTrainingExamples = (UINT) floor( double(totalNumSamples) / 100.0 * double(trainingSizePercentage) );

	UnlabelledClassificationData trainingSet(numDimensions);
	UnlabelledClassificationData testSet(numDimensions);
	vector< UINT > indexs( totalNumSamples );

	//Create the random partion indexs
	Random random;
    UINT randomIndex = 0;
	for(UINT i=0; i<totalNumSamples; i++) indexs[i] = i;
	for(UINT x=0; x<totalNumSamples; x++){
        //Pick a random index
		randomIndex = random.getRandomNumberInt(0,totalNumSamples);

        //Swap the indexs
        SWAP( indexs[ x ] , indexs[ randomIndex ] );
	}

	//Add the data to the training and test sets
	for(UINT i=0; i<numTrainingExamples; i++){
		trainingSet.addSample( data.getRowVector( indexs[i] ) );
	}
	for(UINT i=numTrainingExamples; i<totalNumSamples; i++){
		testSet.addSample( data.getRowVector( indexs[i] )  );
	}

	//Overwrite the training data in this instance with the training data of the trainingSet
	data = trainingSet.getDataAsMatrixDouble();
	totalNumSamples = trainingSet.getNumSamples();

	return testSet;
}

bool UnlabelledClassificationData::merge(UnlabelledClassificationData &unlabelledData){

    if( unlabelledData.getNumDimensions() != numDimensions ){
        errorLog << "merge(UnlabelledClassificationData &unlabelledData) - The number of dimensions in the unlabelledData (" << unlabelledData.getNumDimensions() << ") does not match the number of dimensions of this dataset (" << numDimensions << ")" << endl;
        return false;
    }

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

    //Add the data from the labelledData to this instance
    for(UINT i=0; i<unlabelledData.getNumSamples(); i++){
        addSample( unlabelledData[i] );
    }

    return true;
}

bool UnlabelledClassificationData::spiltDataIntoKFolds(UINT K){

    crossValidationSetup = false;
    crossValidationIndexs.clear();

    //K can not be zero
    if( K > totalNumSamples ){
        errorLog << "spiltDataIntoKFolds(UINT K) - K can not be zero!" << endl;
        return false;
    }

    //K can not be larger than the number of examples
    if( K > totalNumSamples ){
        errorLog << "spiltDataIntoKFolds(UINT K) - K can not be larger than the total number of samples in the dataset!" << endl;
        return false;
    }

    //Setup the dataset for k-fold cross validation
    kFoldValue = K;
    vector< UINT > indexs( totalNumSamples );

    //Work out how many samples are in each fold, the last fold might have more samples than the others
    UINT numSamplesPerFold = (UINT) floor( totalNumSamples/double(K) );

    //Add the random indexs to each fold
    crossValidationIndexs.resize(K);

	//Create the random partion indexs
	Random random;
    UINT randomIndex = 0;

    //Randomize the order of the data
    for(UINT i=0; i<totalNumSamples; i++) indexs[i] = i;
    for(UINT x=0; x<totalNumSamples; x++){
        //Pick a random index
        randomIndex = random.getRandomNumberInt(0,totalNumSamples);

        //Swap the indexs
        SWAP( indexs[ x ] , indexs[ randomIndex ] );
    }

    UINT counter = 0;
    UINT foldIndex = 0;
    for(UINT i=0; i<totalNumSamples; i++){
        //Add the index to the current fold
        crossValidationIndexs[ foldIndex ].push_back( indexs[i] );

        //Move to the next fold if ready
        if( ++counter == numSamplesPerFold && foldIndex < K-1 ){
            foldIndex++;
            counter = 0;
        }
    }

    crossValidationSetup = true;
    return true;

}

UnlabelledClassificationData UnlabelledClassificationData::getTrainingFoldData(UINT foldIndex){
    UnlabelledClassificationData trainingData;

    if( !crossValidationSetup ){
        errorLog << "getTrainingFoldData(UINT foldIndex) - Cross Validation has not been setup! You need to call the spiltDataIntoKFolds(UINT K) function first before calling this function!" << endl;
       return trainingData;
    }

    if( foldIndex >= kFoldValue ) return trainingData;

    trainingData.setNumDimensions( numDimensions );

    //Add the data to the training set, this will consist of all the data that is NOT in the foldIndex
    UINT index = 0;
    for(UINT k=0; k<kFoldValue; k++){
        if( k != foldIndex ){
            for(UINT i=0; i<crossValidationIndexs[k].size(); i++){

                index = crossValidationIndexs[k][i];
                trainingData.addSample( data.getRowVector(index) );
            }
        }
    }

    return trainingData;
}

UnlabelledClassificationData UnlabelledClassificationData::getTestFoldData(UINT foldIndex){
    UnlabelledClassificationData testData;

    if( !crossValidationSetup ) return testData;

    if( foldIndex >= kFoldValue ) return testData;

    //Add the data to the training
    testData.setNumDimensions( numDimensions );

    UINT index = 0;
	for(UINT i=0; i<crossValidationIndexs[ foldIndex ].size(); i++){

        index = crossValidationIndexs[ foldIndex ][i];
		testData.addSample( data.getRowVector( index ) );
	}

    return testData;
}

vector<MinMax> UnlabelledClassificationData::getRanges(){

    //If the dataset should be scaled using the external ranges then return the external ranges
    if( useExternalRanges ) return externalRanges;
    
    vector< MinMax > ranges(numDimensions);

    //Otherwise return the min and max values for each column in the dataset
    if( totalNumSamples > 0 ){
        for(UINT j=0; j<numDimensions; j++){
            ranges[j].minValue = data[0][0];
            ranges[j].maxValue = data[0][0];
            for(UINT i=0; i<totalNumSamples; i++){
                if( data[i][j] < ranges[j].minValue ){ ranges[j].minValue = data[i][j]; }		//Search for the min value
                else if( data[i][j] > ranges[j].maxValue ){ ranges[j].maxValue = data[i][j]; }	//Search for the max value
            }
        }
    }
    return ranges;
}

}; //End of namespace GRT
