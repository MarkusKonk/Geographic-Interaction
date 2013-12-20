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

#include "KNN.h"

namespace GRT{
    
//Register the DTW module with the Classifier base class
RegisterClassifierModule< KNN > KNN::registerModule("KNN");

KNN::KNN(unsigned int K,bool useScaling,bool useNullRejection,double nullRejectionCoeff,bool searchForBestKValue,UINT minKSearchValue,UINT maxKSearchValue){
    this->K = K;
    this->distanceMethod = EUCLIDEAN_DISTANCE;
    this->useScaling = useScaling;
    this->useNullRejection = useNullRejection;
    this->nullRejectionCoeff = nullRejectionCoeff;
    this->searchForBestKValue = searchForBestKValue;
    this->minKSearchValue = minKSearchValue;
    this->maxKSearchValue = maxKSearchValue;
    classifierType = "KNN";
    classifierMode = STANDARD_CLASSIFIER_MODE;
    distanceMethod = EUCLIDEAN_DISTANCE;
    debugLog.setProceedingText("[DEBUG KNN]");
    errorLog.setProceedingText("[ERROR KNN]");
    trainingLog.setProceedingText("[TRAINING KNN]");
    warningLog.setProceedingText("[WARNING KNN]");
}

KNN::~KNN(void)
{
}
    
KNN& KNN::operator=(const KNN &rhs){
    if( this != &rhs ){
        //KNN variables
        this->K = rhs.K;
        this->distanceMethod = rhs.distanceMethod;
        this->searchForBestKValue = rhs.searchForBestKValue;
        this->minKSearchValue = rhs.minKSearchValue;
        this->maxKSearchValue = rhs.maxKSearchValue;
        this->trainingData = rhs.trainingData;
        this->trainingMu = rhs.trainingMu;
        this->trainingSigma = rhs.trainingSigma;
        this->rejectionThresholds = rhs.rejectionThresholds;
        
        //Classifier variables
        copyBaseVariables( (Classifier*)&rhs );
    }
    return *this;
}
    
bool KNN::clone(const Classifier *classifier){
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        //Invoke the equals operator
        KNN *ptr = (KNN*)classifier;
        
        this->K = ptr->K;
        this->distanceMethod = ptr->distanceMethod;
        this->searchForBestKValue = ptr->searchForBestKValue;
        this->minKSearchValue = ptr->minKSearchValue;
        this->maxKSearchValue = ptr->maxKSearchValue;
        this->trainingData = ptr->trainingData;
        this->trainingMu = ptr->trainingMu;
        this->trainingSigma = ptr->trainingSigma;
        this->rejectionThresholds = ptr->rejectionThresholds;
        
        //Classifier variables
        return copyBaseVariables( classifier );
    }
    return false;
}

bool KNN::train(LabelledClassificationData &trainingData){

	if( !searchForBestKValue ){
        return train_(trainingData,K);
    }

    UINT index = 0;
    double bestAccuracy = 0;
    vector< IndexedDouble > trainingAccuracyLog;

    for(UINT k=minKSearchValue; k<=maxKSearchValue; k++){
        //Randomly spilt the data and use 80% to train the algorithm and 20% to test it
        LabelledClassificationData trainingSet(trainingData);
        LabelledClassificationData testSet = trainingSet.partition(80,true);

        if( !train_(trainingSet, k) ){
            errorLog << "Failed to train model for a k value of " << k << endl;
        }else{

            //Compute the classification error
            double accuracy = 0;
            for(UINT i=0; i<testSet.getNumSamples(); i++){

                vector< double > sample = testSet[i].getSample();

                if( !predict( sample ) ){
                    errorLog << "Failed to predict label for test sample with a k value of " << k << endl;
                    return false;
                }

                if( testSet[i].getClassLabel() == predictedClassLabel ){
                    accuracy++;
                }
            }

            accuracy = accuracy /double( testSet.getNumSamples() ) * 100.0;
            trainingAccuracyLog.push_back( IndexedDouble(k,accuracy) );
			
			trainingLog << "K:\t" << k << "\tAccuracy:\t" << accuracy << endl;

            if( accuracy > bestAccuracy ){
                bestAccuracy = accuracy;
            }

            index++;
        }

    }

    if( bestAccuracy > 0 ){
        //Sort the training log by value
        std::sort(trainingAccuracyLog.begin(),trainingAccuracyLog.end(),IndexedDouble::sortIndexedDoubleByValueDescending);

        //Copy the top matching values into a temporary buffer
        vector< IndexedDouble > tempLog;

        //Add the first value
        tempLog.push_back( trainingAccuracyLog[0] );

        //Keep adding values until the value changes
        for(UINT i=1; i<trainingAccuracyLog.size(); i++){
            if( trainingAccuracyLog[i].value == tempLog[0].value ){
                tempLog.push_back( trainingAccuracyLog[i] );
            }else break;
        }

        //Sort the temp values by index (the index is the K value so we want to get the minimum K value with the maximum accuracy)
        std::sort(tempLog.begin(),tempLog.end(),IndexedDouble::sortIndexedDoubleByIndexAscending);

		trainingLog << "Best K Value: " << tempLog[0].index << "\tAccuracy:\t" << tempLog[0].value << endl;

        //Use the minimum index, this should give us the best accuracy with the minimum K value
        return train_(trainingData,tempLog[0].index);
    }

    return false;
}

bool KNN::train_(LabelledClassificationData &trainingData,UINT K){

    //Clear any previous models
    clear();

    if( trainingData.getNumSamples() == 0 ){
        errorLog << "train(LabelledClassificationData &trainingData) - Training data has zero samples!" << endl;
        return false;
    }

    //Set the dimensionality of the input data
    this->K = K;
    this->numFeatures = trainingData.getNumDimensions();
    this->numClasses = trainingData.getNumClasses();

    //TODO: In the future need to build a kdtree from the training data to allow better realtime prediction
    this->trainingData = trainingData;

    if( useScaling ){
        ranges = this->trainingData.getRanges();
        this->trainingData.scale(ranges, 0, 1);
    }

    //Set the class labels
    classLabels.resize(numClasses);
    for(UINT k=0; k<numClasses; k++){
        classLabels[k] = trainingData.getClassTracker()[k].classLabel;
    }

    //Flag that the algorithm has been trained so we can compute the rejection thresholds
    trained = true;
    
    //If null rejection is enabled then compute the null rejection thresholds
    if( useNullRejection ){

        //Set the null rejection to false so we can compute the values for it (this will be set back to its current value later)
        bool tempUseNullRejection = useNullRejection;
        useNullRejection = false;
        rejectionThresholds.clear();

        //Compute the rejection thresholds for each of the K classes
        VectorDouble counter(numClasses,0);
        trainingMu.resize( numClasses, 0 );
        trainingSigma.resize( numClasses, 0 );
        rejectionThresholds.resize( numClasses, 0 );

        //Compute Mu for each of the classes
        const unsigned int numTrainingExamples = trainingData.getNumSamples();
        vector< IndexedDouble > predictionResults( numTrainingExamples );
        for(UINT i=0; i<numTrainingExamples; i++){
            predict( trainingData[i].getSample(), K);

            UINT classLabelIndex = 0;
            for(UINT k=0; k<numClasses; k++){
                if( predictedClassLabel == classLabels[k] ){
                    classLabelIndex = k;
                    break;
                }
            }

            predictionResults[ i ].index = classLabelIndex;
            predictionResults[ i ].value = classDistances[ classLabelIndex ];

            trainingMu[ classLabelIndex ] += predictionResults[ i ].value;
            counter[ classLabelIndex ]++;
        }

        for(UINT j=0; j<numClasses; j++){
            trainingMu[j] /= counter[j];
        }

        //Compute Sigma for each of the classes
        for(UINT i=0; i<numTrainingExamples; i++){
            trainingSigma[predictionResults[i].index] += SQR(predictionResults[i].value - trainingMu[predictionResults[i].index]);
        }

        for(UINT j=0; j<numClasses; j++){
            double count = counter[j];
            if( count > 1 ){
                trainingSigma[ j ] = sqrt( trainingSigma[j] / (count-1) );
            }else{
                trainingSigma[ j ] = 1.0;
            }
        }

        //Check to see if any of the mu or sigma values are zero or NaN
        bool errorFound = false;
        for(UINT j=0; j<numClasses; j++){
            if( trainingMu[j] == 0 ){
                warningLog << "TrainingMu[ " << j << " ] is zero for a K value of " << K << endl;
            }
            if( trainingSigma[j] == 0 ){
                warningLog << "TrainingSigma[ " << j << " ] is zero for a K value of " << K << endl;
            }
            if( isnan( trainingMu[j] ) ){
                errorLog << "TrainingMu[ " << j << " ] is NAN for a K value of " << K << endl;
                errorFound = true;
            }
            if( isnan( trainingSigma[j] ) ){
                errorLog << "TrainingSigma[ " << j << " ] is NAN for a K value of " << K << endl;
                errorFound = true;
            }
        }

        if( errorFound ){
            trained = false;
            return false;
        }

        //Recompute the rejection thresholds
        recomputeNullRejectionThresholds();

        //Restore the actual state of the null rejection
        useNullRejection = tempUseNullRejection;
        
    }else{
        //Resize the rejection thresholds but set the values to 0
        rejectionThresholds.clear();
        rejectionThresholds.resize( numClasses, 0 );
    }

    return true;
}

bool KNN::predict(VectorDouble inputVector){

    if( !trained ){
        errorLog << "predict(VectorDouble inputVector) - KNN model has not been trained" << endl;
        return false;
    }

    if( inputVector.size() != numFeatures ){
        errorLog << "predict(VectorDouble inputVector) - the size of the input vector " << inputVector.size() << " does not match the number of features " << numFeatures <<  endl;
        return false;
    }

    return predict(inputVector,K);
}

bool KNN::predict(VectorDouble inputVector,UINT K){

    if( !trained ){
        errorLog << "predict(VectorDouble inputVector,UINT K) - KNN model has not been trained" << endl;
        return false;
    }

    if( inputVector.size() != numFeatures ){
        errorLog << "predict(VectorDouble inputVector) - the size of the input vector " << inputVector.size() << " does not match the number of features " << numFeatures <<  endl;
        return false;
    }

    if( K > trainingData.getNumSamples() ){
        errorLog << "predict(VectorDouble inputVector,UINT K) - K Is Greater Than The Number Of Training Samples" << endl;
        return false;
    }

    if( useScaling ){
        for(UINT i=0; i<inputVector.size(); i++){
            inputVector[i] = scale(inputVector[i], ranges[i].minValue, ranges[i].maxValue, 0, 1);
        }
    }

    //TODO - need to build a kdtree of the training data to allow better realtime prediction
    const UINT M = trainingData.getNumSamples();
    vector< IndexedDouble > neighbours;

    for(UINT i=0; i<M; i++){
        double dist = 0;
        UINT classLabel = trainingData[i].getClassLabel();
        VectorDouble trainingSample = trainingData[i].getSample();

        switch( distanceMethod ){
            case EUCLIDEAN_DISTANCE:
                dist = computeEuclideanDistance(inputVector,trainingSample);
                break;
            case COSINE_DISTANCE:
                dist = computeCosineDistance(inputVector,trainingSample);
                break;
            case MANHATTAN_DISTANCE:
                dist = computeManhattanDistance(inputVector, trainingSample);
                break;
            default:
                errorLog << "predict(vector< double > inputVector) - unkown distance measure!" << endl;
                return false;
                break;
        }

        if( neighbours.size() < K ){
            neighbours.push_back( IndexedDouble(classLabel,dist) );
        }else{
            //Find the maximum value in the neighbours buffer
            double maxValue = neighbours[0].value;
            UINT maxIndex = 0;
            for(UINT n=1; n<neighbours.size(); n++){
                if( neighbours[n].value > maxValue ){
                    maxValue = neighbours[n].value;
                    maxIndex = n;
                }
            }

            //If the dist is less than the maximum value in the buffer, then replace that value with the new dist
            if( dist < maxValue ){
                neighbours[ maxIndex ] = IndexedDouble(classLabel,dist);
            }
        }
    }

    //Predict the class ID using the labels of the K nearest neighbours
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses,0);
    else for(UINT i=0; i<classLikelihoods.size(); i++){ classLikelihoods[i] = 0; }
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses,0);
    else for(UINT i=0; i<classDistances.size(); i++){ classDistances[i] = 0; }

    //Count the classes
    for(UINT k=0; k<neighbours.size(); k++){
        UINT classLabel = neighbours[k].index;
        if( classLabel == 0 ){
            errorLog << "predict(VectorDouble inputVector) - Class label of training example can not be zero!" << endl;
            return false;
        }

		//Find the index of the classLabel
		UINT classLabelIndex = 0;
		for(UINT j=0; j<numClasses; j++){
			if( classLabel == classLabels[j] ){
				classLabelIndex = j;
				break;
			}
		}
        classLikelihoods[ classLabelIndex ] += 1;
        classDistances[ classLabelIndex ] += neighbours[k].value;
    }

    //Get the max count
    double maxCount = classLikelihoods[0];
    UINT maxIndex = 0;
    for(UINT i=1; i<classLikelihoods.size(); i++){
        if( classLikelihoods[i] > maxCount ){
            maxCount = classLikelihoods[i];
            maxIndex = i;
        }
    }

    //Compute the average distances per class
    for(UINT i=0; i<classDistances.size(); i++){
        if( classLikelihoods[i] > 0 )   classDistances[i] /= classLikelihoods[i];
        else classDistances[i] = BIG_DISTANCE;
    }

    //Normalize the likelihoods
    for(UINT i=0; i<classLikelihoods.size(); i++){
        classLikelihoods[i] /= double( neighbours.size() );
    }

    //Set the maximum likelihood value
    maxLikelihood = classLikelihoods[ maxIndex ];

    if( useNullRejection ){
        if( classDistances[ maxIndex ] <= rejectionThresholds[ maxIndex ] ){
            predictedClassLabel = classLabels[maxIndex];
        }else{
            predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL; //Set the gesture label as the null label
        }
    }else{
        predictedClassLabel = classLabels[maxIndex];
    }

    return true;
}

bool KNN::saveModelToFile(string filename){

    if( !trained ) return false;

	std::fstream file;
	file.open(filename.c_str(), std::ios::out);

	if( !saveModelToFile( file ) ){
        return false;
    }

	file.close();

    return true;
}
    
bool KNN::saveModelToFile(fstream &file){
    
    if(!file.is_open())
    {
        errorLog << "saveModelToFile(fstream &file) - Could not open file to save model!" << endl;
        return false;
    }
    
    //Write the header info
    file<<"GRT_KNN_MODEL_FILE_V1.0\n";
    file<<"NumFeatures: " << numFeatures << endl;
    file<<"NumClasses: " << numClasses << endl;
    file<<"K: "<<K<<endl;
    file<<"DistanceMethod: "<<distanceMethod<<endl;
    file<<"SearchForBestKValue: " << searchForBestKValue << endl;
    file<<"MinKSearchValue: " << minKSearchValue << endl;
    file<<"MaxKSearchValue: " << maxKSearchValue << endl;
    file<<"UseScaling: " << useScaling << endl;
    file<<"UseNullRejection: " << useNullRejection << endl;
    file<<"NullRejectionCoeff: " << nullRejectionCoeff << endl;
    
    if( useScaling ){
        file << "Ranges: \n";
        for(UINT n=0; n<ranges.size(); n++){
            file << ranges[n].minValue << "\t" << ranges[n].maxValue << endl;
        }
    }
    
    file <<"TrainingMu: ";
    for(UINT j=0; j<trainingMu.size(); j++){
        file << trainingMu[j] << "\t";
    }file << endl;
    
    file <<"TrainingSigma: ";
    for(UINT j=0; j<trainingSigma.size(); j++){
        file << trainingSigma[j] << "\t";
    }file << endl;
    
    file <<"NumTrainingSamples: " << trainingData.getNumSamples() << endl;
    file <<"TrainingData: \n";
    
    //Right each of the models
    for(UINT i=0; i<trainingData.getNumSamples(); i++){
        file<< trainingData[i].getClassLabel() << "\t";
        
        for(UINT j=0; j<numFeatures; j++){
            file << trainingData[i][j] << "\t";
        }
        file << endl;
    }
    
    return true;
}

bool KNN::loadModelFromFile(string filename){

    std::fstream file;
	file.open(filename.c_str(), std::ios::in);

    //Clear any previous models or data
	clear();

	if( !loadModelFromFile( file ) ){
        return false;
    }
    
    file.close();

    return true;
}
    
bool KNN::loadModelFromFile(fstream &file){
    
    if(!file.is_open())
    {
        errorLog << "loadModelFromFile(fstream &file) - Could not open file to load model!" << endl;
        return false;
    }
    
    std::string word;
    
    //Find the file type header
    file >> word;
    if(word != "GRT_KNN_MODEL_FILE_V1.0"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find Model File Header!" << endl;
        return false;
    }
    
    //Find the file type header
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find NumFeatures!" << endl;
        return false;
    }
    file >> numFeatures;
    
    file >> word;
    if(word != "NumClasses:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find NumClasses!" << endl;
        return false;
    }
    file >> numClasses;
    
    file >> word;
    if(word != "K:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find K!" << endl;
        return false;
    }
    file >> K;
    
    file >> word;
    if(word != "DistanceMethod:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find DistanceMethod!" << endl;
        return false;
    }
    file >> distanceMethod;
    
    file >> word;
    if(word != "SearchForBestKValue:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find SearchForBestKValue!" << endl;
        return false;
    }
    file >> searchForBestKValue;
    
    file >> word;
    if(word != "MinKSearchValue:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find MinKSearchValue!" << endl;
        return false;
    }
    file >> minKSearchValue;
    
    file >> word;
    if(word != "MaxKSearchValue:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find MaxKSearchValue!" << endl;
        return false;
    }
    file >> maxKSearchValue;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find UseScaling!" << endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if(word != "UseNullRejection:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find UseNullRejection!" << endl;
        return false;
    }
    file >> useNullRejection;
    
    file >> word;
    if(word != "NullRejectionCoeff:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find NullRejectionCoeff!" << endl;
        return false;
    }
    file >> nullRejectionCoeff;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        ranges.resize( numFeatures );
        
        file >> word;
        if(word != "Ranges:"){
            errorLog << "loadModelFromFile(fstream &file) - Could not find Ranges!" << endl;
            cout << "Word: " << word << endl;
            return false;
        }
        for(UINT n=0; n<ranges.size(); n++){
            file >> ranges[n].minValue;
            file >> ranges[n].maxValue;
        }
    }
    
    //Resize the buffers
    trainingMu.resize(numClasses,0);
    trainingSigma.resize(numClasses,0);
    
    file >> word;
    if(word != "TrainingMu:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find TrainingMu!" << endl;
        return false;
    }
    
    //Load the trainingMu data
    for(UINT j=0; j<numClasses; j++){
        file >> trainingMu[j];
    }
    
    file >> word;
    if(word != "TrainingSigma:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find TrainingSigma!" << endl;
        return false;
    }
    
    //Load the trainingSigma data
    for(UINT j=0; j<numClasses; j++){
        file >> trainingSigma[j];
    }
    
    file >> word;
    if(word != "NumTrainingSamples:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find NumTrainingSamples!" << endl;
        return false;
    }
    unsigned int numTrainingSamples = 0;
    file >> numTrainingSamples;
    
    file >> word;
    if(word != "TrainingData:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find TrainingData!" << endl;
        return false;
    }
    
    //Load the training data
    trainingData.setNumDimensions(numFeatures);
    unsigned int classLabel = 0;
    vector< double > sample(numFeatures,0);
    for(UINT i=0; i<numTrainingSamples; i++){
        //Read the class label
        file >> classLabel;
        
        //Read the feature vector
        for(UINT j=0; j<numFeatures; j++){
            file >> sample[j];
        }
        
        //Add it to the training data
        trainingData.addSample(classLabel, sample);
    }
    
    //Flag that the model has been trained
    trained = true;
    
    //Compute the null rejection thresholds
    recomputeNullRejectionThresholds();
    
    return true;
}

bool KNN::recomputeNullRejectionThresholds(){
    if( !trained ){
        return false;
    }

    rejectionThresholds.clear();
    rejectionThresholds.resize(numClasses,0);

    for(unsigned int j=0; j<numClasses; j++){
        rejectionThresholds[j] = trainingMu[j] + (trainingSigma[j]*nullRejectionCoeff);
    }

    return true;
}

bool KNN::setK(UINT K){
    if( K > 0 ){
        this->K = K;
        return true;
    }
    return false;
}

bool KNN::setMinKSearchValue(UINT minKSearchValue){
    this->minKSearchValue = minKSearchValue;
    return true;
}

bool KNN::setMaxKSearchValue(UINT maxKSearchValue){
    this->maxKSearchValue = maxKSearchValue;
    return true;
}

bool KNN::enableBestKValueSearch(bool searchForBestKValue){
    this->searchForBestKValue = searchForBestKValue;
    return true;
}

bool KNN::setNullRejectionCoeff(double nullRejectionCoeff){
    if( nullRejectionCoeff > 0 ){
        this->nullRejectionCoeff = nullRejectionCoeff;
        recomputeNullRejectionThresholds();
        return true;
    }
    return false;
}

bool KNN::setDistanceMethod(UINT distanceMethod){
    if( distanceMethod == EUCLIDEAN_DISTANCE || distanceMethod == COSINE_DISTANCE || distanceMethod == MANHATTAN_DISTANCE ){
        this->distanceMethod = distanceMethod;
        return true;
    }
    return false;
}

void KNN::clear(){
    trained = false;
    numClasses = 0;
    numFeatures = 0;
    trainingMu.clear();
    trainingSigma.clear();
    rejectionThresholds.clear();
}

double KNN::computeEuclideanDistance(const VectorDouble &a,const VectorDouble &b){
    double dist = 0;
    for(UINT j=0; j<numFeatures; j++){
        dist += SQR( a[j] - b[j] );
    }
    return sqrt( dist );
}

double KNN::computeCosineDistance(const VectorDouble &a,const VectorDouble &b){
    double dist = 0;

    double dotAB = 0;
    double magA = 0;
    double magB = 0;

    for(UINT j=0; j<numFeatures; j++){
        dotAB += a[j] * b[j];
        magA += SQR(a[j]);
        magB += SQR(b[j]);
    }

    dist = dotAB / (sqrt(magA) * sqrt(magB));

    return dist;
}

double KNN::computeManhattanDistance(const VectorDouble &a,const VectorDouble &b){
    double dist = 0;

    for(UINT j=0; j<numFeatures; j++){
        dist += fabs( a[j] - b[j] );
    }

    return dist;
}

} //End of namespace GRT

