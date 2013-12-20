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

#include "LabelledTimeSeriesClassificationData.h"

namespace GRT{

LabelledTimeSeriesClassificationData::LabelledTimeSeriesClassificationData(UINT numDimensions,string datasetName,string infoText){
    debugLog.setProceedingText("[DEBUG LTSCD]");
    errorLog.setProceedingText("[ERROR LTSCD]");
    warningLog.setProceedingText("[WARNING LTSCD]");

    this->numDimensions = numDimensions;
    this->datasetName = datasetName;
    this->infoText = infoText;
    totalNumSamples = 0;
    crossValidationSetup = false;
    useExternalRanges = false;
    allowNullGestureClass = false;
    if( numDimensions > 0 ){
        setNumDimensions(numDimensions);
    }
}

LabelledTimeSeriesClassificationData::LabelledTimeSeriesClassificationData(const LabelledTimeSeriesClassificationData &rhs){
    *this = rhs;
}

LabelledTimeSeriesClassificationData::~LabelledTimeSeriesClassificationData(){}
    
LabelledTimeSeriesClassificationData& LabelledTimeSeriesClassificationData::operator=(const LabelledTimeSeriesClassificationData &rhs){
    if( this != &rhs){
        this->datasetName = rhs.datasetName;
        this->infoText = rhs.infoText;
        this->numDimensions = rhs.numDimensions;
        this->useExternalRanges = rhs.useExternalRanges;
        this->allowNullGestureClass = rhs.allowNullGestureClass;
        this->crossValidationSetup = rhs.crossValidationSetup;
        this->crossValidationIndexs = rhs.crossValidationIndexs;
        this->totalNumSamples = rhs.totalNumSamples;
        this->data = rhs.data;
        this->classTracker = rhs.classTracker;
        this->externalRanges = rhs.externalRanges;
        this->debugLog = rhs.debugLog;
        this->errorLog = rhs.errorLog;
        this->warningLog = rhs.warningLog;
    }
    return *this;
}

void LabelledTimeSeriesClassificationData::clear(){
	totalNumSamples = 0;
	data.clear();
	classTracker.clear();
}

bool LabelledTimeSeriesClassificationData::setNumDimensions(UINT numDimensions){
    if( numDimensions > 0 ){
        //Clear any previous training data
        clear();

        //Set the dimensionality of the training data
        this->numDimensions = numDimensions;

        useExternalRanges = false;
        externalRanges.clear();

        return true;
    }

    errorLog << "setNumDimensions(UINT numDimensions) - The number of dimensions of the dataset must be greater than zero!" << endl;
    return false;
}

bool LabelledTimeSeriesClassificationData::setDatasetName(string datasetName){

    //Make sure there are no spaces in the string
    if( datasetName.find(" ") == string::npos ){
        this->datasetName = datasetName;
        return true;
    }

    errorLog << "setDatasetName(string datasetName) - The dataset name cannot contain any spaces!" << endl;
    return false;
}

bool LabelledTimeSeriesClassificationData::setInfoText(string infoText){
    this->infoText = infoText;
    return true;
}

bool LabelledTimeSeriesClassificationData::setClassNameForCorrespondingClassLabel(string className,UINT classLabel){

    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel == classLabel ){
            classTracker[i].className = className;
            return true;
        }
    }

    return false;
}
    
bool LabelledTimeSeriesClassificationData::setAllowNullGestureClass(bool allowNullGestureClass){
    this->allowNullGestureClass = allowNullGestureClass;
    return true;
}

bool LabelledTimeSeriesClassificationData::addSample(UINT classLabel,const MatrixDouble &trainingSample){
	
    if( trainingSample.getNumCols() != numDimensions ){
        errorLog << "addSample(UINT classLabel, MatrixDouble trainingSample) - The dimensionality of the training sample (" << trainingSample.getNumCols() << ") does not match that of the dataset (" << numDimensions << ")" << endl;
        return false;
    }
    
    //The class label must be greater than zero (as zero is used for the null rejection class label
    if( classLabel == GRT_DEFAULT_NULL_CLASS_LABEL && !allowNullGestureClass ){
        errorLog << "addSample(UINT classLabel, MatrixDouble sample) - the class label can not be 0!" << endl;
        return false;
    }

	LabelledTimeSeriesClassificationSample newSample(classLabel,trainingSample);
	data.push_back( newSample );
	totalNumSamples++;

	if( classTracker.size() == 0 ){
		ClassTracker tracker(classLabel,1);
		classTracker.push_back(tracker);
	}else{
		bool labelFound = false;
		for(UINT i=0; i<classTracker.size(); i++){
			if( classLabel == classTracker[i].classLabel ){
				classTracker[i].counter++;
				labelFound = true;
				break;
			}
		}
		if( !labelFound ){
			ClassTracker tracker(classLabel,1);
			classTracker.push_back(tracker);
		}
	}
	return true;
}

UINT LabelledTimeSeriesClassificationData::eraseAllSamplesWithClassLabel(UINT classLabel){
	UINT numExamplesRemoved = 0;
	UINT numExamplesToRemove = 0;

	//Find out how many training examples we need to remove
	for(UINT i=0; i<classTracker.size(); i++){
		if( classTracker[i].classLabel == classLabel ){
			numExamplesToRemove = classTracker[i].counter;
			classTracker.erase(classTracker.begin()+i);
			break;
		}
	}

	//Remove the samples with the matching class ID
	if( numExamplesToRemove > 0 ){
		UINT i=0;
		while( numExamplesRemoved < numExamplesToRemove ){
			if( data[i].getClassLabel() == classLabel ){
				data.erase(data.begin()+i);
				numExamplesRemoved++;
			}else if( ++i == data.size() ) break;
		}
	}

	totalNumSamples = (UINT)data.size();

	return numExamplesRemoved;
}

bool LabelledTimeSeriesClassificationData::removeLastSample(){

	if( totalNumSamples > 0 ){

		//Find the corresponding class ID for the last training example
		UINT classLabel = data[ totalNumSamples-1 ].getClassLabel();

		//Remove the training example from the buffer
		data.erase(data.end()-1);

		totalNumSamples = (UINT)data.size();

		//Remove the value from the counter
		for(UINT i=0; i<classTracker.size(); i++){
			if( classTracker[i].classLabel == classLabel ){
				classTracker[i].counter--;
				break;
			}
		}

		return true;

	}else return false;

}

bool LabelledTimeSeriesClassificationData::relabelAllSamplesWithClassLabel(UINT oldClassLabel,UINT newClassLabel){
    bool oldClassLabelFound = false;
    bool newClassLabelAllReadyExists = false;
    UINT indexOfOldClassLabel = 0;
    UINT indexOfNewClassLabel = 0;

    //Find out how many training examples we need to relabel
    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel == oldClassLabel ){
            indexOfOldClassLabel = i;
            oldClassLabelFound = true;
        }
        if( classTracker[i].classLabel == newClassLabel ){
            indexOfNewClassLabel = i;
            newClassLabelAllReadyExists = true;
        }
    }

    //If the old class label was not found then we can't do anything
    if( !oldClassLabelFound ){
        return false;
    }

    //Relabel the old class labels
    for(UINT i=0; i<totalNumSamples; i++){
        if( data[i].getClassLabel() == oldClassLabel ){
            data[i].setTrainingSample(newClassLabel, data[i].getData());
        }
    }

    //Update the class label counters
    if( newClassLabelAllReadyExists ){
        //Add the old sample count to the new sample count
        classTracker[ indexOfNewClassLabel ].counter += classTracker[ indexOfOldClassLabel ].counter;

        //Erase the old class tracker
        classTracker.erase( classTracker.begin() + indexOfOldClassLabel );
    }else{
        //Create a new class tracker
        classTracker.push_back( ClassTracker(newClassLabel,classTracker[ indexOfOldClassLabel ].counter,classTracker[ indexOfOldClassLabel ].className) );
    }

    return true;
}

bool LabelledTimeSeriesClassificationData::setExternalRanges(vector< MinMax > externalRanges, bool useExternalRanges){

    if( externalRanges.size() != numDimensions ) return false;

    this->externalRanges = externalRanges;
    this->useExternalRanges = useExternalRanges;

    return true;
}

bool LabelledTimeSeriesClassificationData::enableExternalRangeScaling(bool useExternalRanges){
    if( externalRanges.size() == numDimensions ){
        this->useExternalRanges = useExternalRanges;
        return true;
    }
    return false;
}

bool LabelledTimeSeriesClassificationData::scale(double minTarget,double maxTarget){
    vector< MinMax > ranges = getRanges();
    return scale(ranges,minTarget,maxTarget);
}

bool LabelledTimeSeriesClassificationData::scale(vector<MinMax> ranges,double minTarget,double maxTarget){
    if( ranges.size() != numDimensions ) return false;

    //Scale the training data
    for(UINT i=0; i<totalNumSamples; i++){
        for(UINT x=0; x<data[i].getLength(); x++){
            for(UINT j=0; j<numDimensions; j++){
                data[i][x][j] = Util::scale(data[i][x][j],ranges[j].minValue,ranges[j].maxValue,minTarget,maxTarget);
            }
        }
    }

    return true;
}

bool LabelledTimeSeriesClassificationData::saveDatasetToFile(string fileName){

	std::fstream file;
	file.open(fileName.c_str(), std::ios::out);

	if( !file.is_open() ){
        errorLog << "saveDatasetToFile(string fileName) -  Failed to open file!" << endl;
		return false;
	}

	file << "GRT_LABELLED_TIME_SERIES_CLASSIFICATION_DATA_FILE_V1.0\n";
    file << "DatasetName: " << datasetName << endl;
    file << "InfoText: " << infoText << endl;
	file << "NumDimensions: "<<numDimensions<<endl;
	file << "TotalNumTrainingExamples: "<<totalNumSamples<<endl;
	file << "NumberOfClasses: "<<classTracker.size()<<endl;
	file << "ClassIDsAndCounters: "<<endl;

	for(UINT i=0; i<classTracker.size(); i++){
		file << classTracker[i].classLabel << "\t" << classTracker[i].counter << endl;
	}

    file << "UseExternalRanges: " << useExternalRanges << endl;

    if( useExternalRanges ){
        for(UINT i=0; i<externalRanges.size(); i++){
            file << externalRanges[i].minValue << "\t" << externalRanges[i].maxValue << endl;
        }
    }

	file << "LabelledTimeSeriesTrainingData:\n";

	for(UINT x=0; x<totalNumSamples; x++){
		file << "************TIME_SERIES************\n";
		file << "ClassID: "<<data[x].getClassLabel() <<endl;
		file << "TimeSeriesLength: "<<data[x].getLength()<<endl;
		file << "TimeSeriesData: \n";
		for(UINT i=0; i<data[x].getLength(); i++){
			for(UINT j=0; j<numDimensions; j++){
				file << data[x][i][j];
				if( j<numDimensions-1 ) file << "\t";
			}file << endl;
		}
	}

	file.close();
	return true;
}

bool LabelledTimeSeriesClassificationData::loadDatasetFromFile(string filename){

	std::fstream file;
	file.open(filename.c_str(), std::ios::in);
	UINT numClasses = 0;
	clear();

	if( !file.is_open() ){
		errorLog << "loadDatasetFromFile(string filename) - FILE NOT OPEN!" << endl;
		return false;
	}

	string word;

	//Check to make sure this is a file with the Training File Format
	file >> word;
	if(word != "GRT_LABELLED_TIME_SERIES_CLASSIFICATION_DATA_FILE_V1.0"){
		file.close();
        clear();
        errorLog << "loadDatasetFromFile(string filename) - Failed to find file header!" << endl;
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
		file.close();
        clear();
        errorLog << "loadDatasetFromFile(string filename) - Failed to find NumDimensions!" << endl;
		return false;
	}
	file >> numDimensions;

	//Get the total number of training examples in the training data
	file >> word;
	if(word != "TotalNumTrainingExamples:"){
		file.close();
        clear();
        errorLog << "loadDatasetFromFile(string filename) - Failed to find TotalNumTrainingExamples!" << endl;
		return false;
	}
	file >> totalNumSamples;

	//Get the total number of classes in the training data
	file >> word;
	if(word != "NumberOfClasses:"){
		file.close();
        clear();
        errorLog << "loadDatasetFromFile(string filename) - Failed to find NumberOfClasses!" << endl;
		return false;
	}
	file >> numClasses;

	//Resize the class counter buffer and load the counters
	classTracker.resize(numClasses);

	//Get the total number of classes in the training data
	file >> word;
	if(word != "ClassIDsAndCounters:"){
		file.close();
        clear();
        errorLog << "loadDatasetFromFile(string filename) - Failed to find ClassIDsAndCounters!" << endl;
		return false;
	}

	for(UINT i=0; i<classTracker.size(); i++){
		file >> classTracker[i].classLabel;
		file >> classTracker[i].counter;
	}

    //Get the UseExternalRanges
	file >> word;
	if(word != "UseExternalRanges:"){
		file.close();
        clear();
        errorLog << "loadDatasetFromFile(string filename) - Failed to find UseExternalRanges!" << endl;
		return false;
	}

    file >> useExternalRanges;

    if( useExternalRanges ){
        externalRanges.resize(numDimensions);
        for(UINT i=0; i<externalRanges.size(); i++){
            file >> externalRanges[i].minValue;
            file >> externalRanges[i].maxValue;
        }
    }

	//Get the main training data
	file >> word;
	if(word != "LabelledTimeSeriesTrainingData:"){
        file.close();
        clear();
        errorLog << "loadDatasetFromFile(string filename) - Failed to find LabelledTimeSeriesTrainingData!" << endl;
		return false;
	}

	//Reset the memory
	data.resize( totalNumSamples, LabelledTimeSeriesClassificationSample() );

	//Load each of the time series
	for(UINT x=0; x<totalNumSamples; x++){
		UINT classLabel = 0;
		UINT timeSeriesLength = 0;

		file >> word;
		if( word != "************TIME_SERIES************" ){
			file.close();
            clear();
            errorLog << "loadDatasetFromFile(string filename) - Failed to find TimeSeries Header!" << endl;
            return false;
		}

		file >> word;
		if( word != "ClassID:" ){
			file.close();
            clear();
            errorLog << "loadDatasetFromFile(string filename) - Failed to find ClassID!" << endl;
            return false;
		}
		file >> classLabel;

		file >> word;
		if( word != "TimeSeriesLength:" ){
			file.close();
            clear();
            errorLog << "loadDatasetFromFile(string filename) - Failed to find TimeSeriesLength!" << endl;
            return false;
		}
		file >> timeSeriesLength;

		file >> word;
		if( word != "TimeSeriesData:" ){
			file.close();
            clear();
            errorLog << "loadDatasetFromFile(string filename) - Failed to find TimeSeriesData!" << endl;
            return false;
		}

		//Load the time series data
		Matrix<double> trainingExample(timeSeriesLength,numDimensions);
		for(UINT i=0; i<timeSeriesLength; i++){
			for(UINT j=0; j<numDimensions; j++){
				file >> trainingExample[i][j];
			}
		}

		data[x].setTrainingSample(classLabel,trainingExample);
	}

	file.close();
	return true;
}
    
bool LabelledTimeSeriesClassificationData::printStats(){
    
    cout << "DatasetName:\t" << datasetName << endl;
    cout << "DatasetInfo:\t" << infoText << endl;
    cout << "Number of Dimensions:\t" << numDimensions << endl;
    cout << "Number of Samples:\t" << totalNumSamples << endl;
    cout << "Number of Classes:\t" << getNumClasses() << endl;
    cout << "ClassStats:\n";
    
    for(UINT k=0; k<getNumClasses(); k++){
        cout << "ClassLabel:\t" << classTracker[k].classLabel;
        cout << "\tNumber of Samples:\t" << classTracker[k].counter;
        cout << "\tClassName:\t" << classTracker[k].className << endl;
    }
    
    vector< MinMax > ranges = getRanges();
    
    cout << "Dataset Ranges:\n";
    for(UINT j=0; j<ranges.size(); j++){
        cout << "[" << j+1 << "] Min:\t" << ranges[j].minValue << "\tMax: " << ranges[j].maxValue << endl;
    }
    
    cout << "Timeseries Lengths:\n";
    UINT M = (UINT)data.size();
    for(UINT j=0; j<M; j++){
        cout << "ClassLabel: " << data[j].getClassLabel() << " Length:\t" << data[j].getLength() << endl;
    }
    
    return true;
}
    
LabelledTimeSeriesClassificationData LabelledTimeSeriesClassificationData::partition(UINT trainingSizePercentage,bool useStratifiedSampling){

    //Partitions the dataset into a training dataset (which is kept by this instance of the LabelledTimeSeriesClassificationData) and
    //a testing/validation dataset (which is return as a new instance of the LabelledTimeSeriesClassificationData).  The trainingSizePercentage
    //therefore sets the size of the data which remains in this instance and the remaining percentage of data is then added to
    //the testing/validation dataset

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

    LabelledTimeSeriesClassificationData trainingSet(numDimensions);
    LabelledTimeSeriesClassificationData testSet(numDimensions);
    trainingSet.setAllowNullGestureClass(allowNullGestureClass);
    testSet.setAllowNullGestureClass(allowNullGestureClass);
    vector< UINT > indexs( totalNumSamples );

    //Create the random partion indexs
    Random random;
    UINT randomIndex = 0;

    if( useStratifiedSampling ){
        //Break the data into seperate classes
        vector< vector< UINT > > classData( getNumClasses() );

        //Add the indexs to their respective classes
        for(UINT i=0; i<totalNumSamples; i++){
            classData[ getClassLabelIndexValue( data[i].getClassLabel() ) ].push_back( i );
        }

        //Randomize the order of the indexs in each of the class index buffers
        for(UINT k=0; k<getNumClasses(); k++){
            UINT numSamples = (UINT)classData[k].size();
            for(UINT x=0; x<numSamples; x++){
                //Pick a random index
                randomIndex = random.getRandomNumberInt(0,numSamples);

                //Swap the indexs
                SWAP( classData[k][ x ] ,classData[k][ randomIndex ] );
            }
        }

        //Loop over each class and add the data to the trainingSet and testSet
        for(UINT k=0; k<getNumClasses(); k++){
            UINT numTrainingExamples = (UINT) floor( double(classData[k].size()) / 100.0 * double(trainingSizePercentage) );

            //Add the data to the training and test sets
            for(UINT i=0; i<numTrainingExamples; i++){
                trainingSet.addSample( data[ classData[k][i] ].getClassLabel(), data[ classData[k][i] ].getData() );
            }
            for(UINT i=numTrainingExamples; i<classData[k].size(); i++){
                testSet.addSample( data[ classData[k][i] ].getClassLabel(), data[ classData[k][i] ].getData() );
            }
        }

        //Overwrite the training data in this instance with the training data of the trainingSet
        data = trainingSet.getClassificationData();
        totalNumSamples = trainingSet.getNumSamples();
    }else{

        const UINT numTrainingExamples = (UINT) floor( double(totalNumSamples) / 100.0 * double(trainingSizePercentage) );
        //Create the random partion indexs
        Random random;
        for(UINT i=0; i<totalNumSamples; i++) indexs[i] = i;
        for(UINT x=0; x<totalNumSamples; x++){
            //Pick a random index
            randomIndex = random.getRandomNumberInt(0,totalNumSamples);

            //Swap the indexs
            SWAP( indexs[ x ] , indexs[ randomIndex ] );
        }

        //Add the data to the training and test sets
        for(UINT i=0; i<numTrainingExamples; i++){
            trainingSet.addSample( data[ indexs[i] ].getClassLabel(), data[ indexs[i] ].getData() );
        }
        for(UINT i=numTrainingExamples; i<totalNumSamples; i++){
            testSet.addSample( data[ indexs[i] ].getClassLabel(), data[ indexs[i] ].getData() );
        }

        //Overwrite the training data in this instance with the training data of the trainingSet
        data = trainingSet.getClassificationData();
        totalNumSamples = trainingSet.getNumSamples();
    }

    return testSet;
}

bool LabelledTimeSeriesClassificationData::merge(LabelledTimeSeriesClassificationData &labelledData){

    if( labelledData.getNumDimensions() != numDimensions ){
        errorLog << "merge(LabelledTimeSeriesClassificationData &labelledData) - The number of dimensions in the labelledData (" << labelledData.getNumDimensions() << ") does not match the number of dimensions of this dataset (" << numDimensions << ")" << endl;
        return false;
    }

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

    //Add the data from the labelledData to this instance
    for(UINT i=0; i<labelledData.getNumSamples(); i++){
        addSample(labelledData[i].getClassLabel(), labelledData[i].getData());
    }

    //Set the class names from the dataset
    vector< ClassTracker > classTracker = labelledData.getClassTracker();
    for(UINT i=0; i<classTracker.size(); i++){
        setClassNameForCorrespondingClassLabel(classTracker[i].className, classTracker[i].classLabel);
    }

    return true;
}

bool LabelledTimeSeriesClassificationData::spiltDataIntoKFolds(UINT K,bool useStratifiedSampling){

    crossValidationSetup = false;
    crossValidationIndexs.clear();

    //K can not be zero
    if( K > totalNumSamples ){
        errorLog << "spiltDataIntoKFolds(UINT K) - K can not be zero!" << endl;
        return false;
    }

    //K can not be larger than the number of examples
    if( K > totalNumSamples ){
        errorLog << "spiltDataIntoKFolds(UINT K,bool useStratifiedSampling) - K can not be larger than the total number of samples in the dataset!" << endl;
        return false;
    }

    //K can not be larger than the number of examples in a specific class if the stratified sampling option is true
    if( useStratifiedSampling ){
        for(UINT c=0; c<classTracker.size(); c++){
            if( K > classTracker[c].counter ){
                errorLog << "spiltDataIntoKFolds(UINT K,bool useStratifiedSampling) - K can not be larger than the number of samples in any given class!" << endl;
                return false;
            }
        }
    }

    //Setup the dataset for k-fold cross validation
    kFoldValue = K;
    vector< UINT > indexs( totalNumSamples );

    //Work out how many samples are in each fold, the last fold might have more samples than the others
    UINT numSamplesPerFold = (UINT) floor( totalNumSamples/double(K) );

    //Resize the cross validation indexs buffer
    crossValidationIndexs.resize( K );

    //Create the random partion indexs
    Random random;
    UINT randomIndex = 0;

    if( useStratifiedSampling ){
        //Break the data into seperate classes
        vector< vector< UINT > > classData( getNumClasses() );

        //Add the indexs to their respective classes
        for(UINT i=0; i<totalNumSamples; i++){
            classData[ getClassLabelIndexValue( data[i].getClassLabel() ) ].push_back( i );
        }

        //Randomize the order of the indexs in each of the class index buffers
        for(UINT c=0; c<getNumClasses(); c++){
            UINT numSamples = (UINT)classData[c].size();
            for(UINT x=0; x<numSamples; x++){
                //Pick a random index
                randomIndex = random.getRandomNumberInt(0,numSamples);

                //Swap the indexs
                SWAP( classData[c][ x ] , classData[c][ randomIndex ] );
            }
        }

        //Loop over each of the classes and add the data equally to each of the k folds until there is no data left
        vector< UINT >::iterator iter;
        for(UINT c=0; c<getNumClasses(); c++){
            iter = classData[ c ].begin();
            UINT k = 0;
            while( iter != classData[c].end() ){
                crossValidationIndexs[ k ].push_back( *iter );
                iter++;
                k = ++k % K;
            }
        }

    }else{
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
    }

    crossValidationSetup = true;
    return true;

}

LabelledTimeSeriesClassificationData LabelledTimeSeriesClassificationData::getTrainingFoldData(UINT foldIndex){

    LabelledTimeSeriesClassificationData trainingData;

    if( !crossValidationSetup ){
        errorLog << "getTrainingFoldData(UINT foldIndex) - Cross Validation has not been setup! You need to call the spiltDataIntoKFolds(UINT K,bool useStratifiedSampling) function first before calling this function!" << endl;
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
                trainingData.addSample( data[ index ].getClassLabel(), data[ index ].getData() );
            }
        }
    }

    return trainingData;
}

LabelledTimeSeriesClassificationData LabelledTimeSeriesClassificationData::getTestFoldData(UINT foldIndex){
    LabelledTimeSeriesClassificationData testData;

    if( !crossValidationSetup ) return testData;

    if( foldIndex >= kFoldValue ) return testData;

    //Add the data to the training
    testData.setNumDimensions( numDimensions );

    UINT index = 0;
    for(UINT i=0; i<crossValidationIndexs[ foldIndex ].size(); i++){

        index = crossValidationIndexs[ foldIndex ][i];
        testData.addSample( data[ index ].getClassLabel(), data[ index ].getData() );
    }

    return testData;
}

LabelledTimeSeriesClassificationData LabelledTimeSeriesClassificationData::getClassData(UINT classLabel){
    LabelledTimeSeriesClassificationData classData(numDimensions);
    for(UINT x=0; x<totalNumSamples; x++){
        if( data[x].getClassLabel() == classLabel ){
            classData.addSample( classLabel, data[x].getData() );
        }
    }
    return classData;
}

UnlabelledClassificationData LabelledTimeSeriesClassificationData::reformatAsUnlabelledClassificationData(){

    UnlabelledClassificationData unlabelledData;

    if( totalNumSamples == 0 ){
        return unlabelledData;
    }

    unlabelledData.setNumDimensions( numDimensions );

    for(UINT i=0; i<totalNumSamples; i++){
        for(UINT x=0; x<data[i].getLength(); x++){
            unlabelledData.addSample( data[i].getData().getRowVector( x ) );
        }
    }

    return unlabelledData;
}

UINT LabelledTimeSeriesClassificationData::getMinimumClassLabel(){
    UINT minClassLabel = 99999;

    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel < minClassLabel ){
            minClassLabel = classTracker[i].classLabel;
        }
    }

    return minClassLabel;
}


UINT LabelledTimeSeriesClassificationData::getMaximumClassLabel(){
    UINT maxClassLabel = 0;

    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel > maxClassLabel ){
            maxClassLabel = classTracker[i].classLabel;
        }
    }

    return maxClassLabel;
}

UINT LabelledTimeSeriesClassificationData::getClassLabelIndexValue(UINT classLabel){
    for(UINT k=0; k<classTracker.size(); k++){
        if( classTracker[k].classLabel == classLabel ){
            return k;
        }
    }
    warningLog << "getClassLabelIndexValue(UINT classLabel) - Failed to find class label: " << classLabel << " in class tracker!" << endl;
    return 0;
}

string LabelledTimeSeriesClassificationData::getClassNameForCorrespondingClassLabel(UINT classLabel){

    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel == classLabel ){
            return classTracker[i].className;
        }
    }
    return "CLASS_LABEL_NOT_FOUND";
}

vector<MinMax> LabelledTimeSeriesClassificationData::getRanges(){

    if( useExternalRanges ) return externalRanges;

    vector<MinMax> ranges(numDimensions);

    if( totalNumSamples > 0 ){
        for(UINT j=0; j<numDimensions; j++){
            ranges[j].minValue = data[0][0][0];
            ranges[j].maxValue = data[0][0][0];
            for(UINT x=0; x<totalNumSamples; x++){
                for(UINT i=0; i<data[x].getLength(); i++){
                    if( data[x][i][j] < ranges[j].minValue ){ ranges[j].minValue = data[x][i][j]; }		//Search for the min value
                    else if( data[x][i][j] > ranges[j].maxValue ){ ranges[j].maxValue = data[x][i][j]; }	//Search for the max value
                }
            }
        }
    }
    return ranges;
}
    
MatrixDouble LabelledTimeSeriesClassificationData::getDataAsMatrixDouble(){
    
    //Count how many samples are in the entire dataset
    UINT M = 0;
    UINT index = 0;
    for(UINT x=0; x<totalNumSamples; x++){
        M += data[x].getLength();
    }
    
    if( M == 0 ) MatrixDouble();
    
    //Get all the data and concatenate it into 1 matrix
    MatrixDouble matrixData(M,numDimensions);
    for(UINT x=0; x<totalNumSamples; x++){
        for(UINT i=0; i<data[x].getLength(); i++){
            for(UINT j=0; j<numDimensions; j++){
                matrixData[index][j] = data[x][i][j];
            }
            index++;
        }
    }
    return matrixData;
}

} //End of namespace GRT
