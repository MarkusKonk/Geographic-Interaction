#include "GRT_Recognizer.h"

GestureRecognitionPipeline pipeline;    

GRT_Recognizer::GRT_Recognizer(void)
{
}


GRT_Recognizer::~GRT_Recognizer(void)
{

}


bool GRT_Recognizer::initPipeline(string trainingdatafile, int dimension)
{
			    //Initialize the training and info variables
   // infoText = "";
   // trainingClassLabel = 1;
   // noOfHands = 2;
	//noOfTrackedHands = 0;
    
	
	//The input to the training data will be the R[x y z]L[x y z] from the left end right hand
	// so we set the number of dimensions to 6
	LabelledTimeSeriesClassificationData trainingData; 
    //trainingData.setNumDimensions(6);
	trainingData.loadDatasetFromFile(trainingdatafile);
    
    //Initialize the DTW classifier
    DTW dtw;
    
    //Turn on null rejection, this lets the classifier output the predicted class label of 0 when the likelihood of a gesture is low
    dtw.enableNullRejection( true);
    
    //Set the null rejection coefficient to 3, this controls the thresholds for the automatic null rejection
    //You can increase this value if you find that your real-time gestures are not being recognized
    //If you are getting too many false positives then you should decrease this value
    dtw.setNullRejectionCoeff(2);

    
    //Turn on the automatic data triming, this will remove any sections of none movement from the start and end of the training samples
    dtw.enableTrimTrainingData(true, 0.1, 90);
    
    //Offset the timeseries data by the first sample, this makes your gestures (more) invariant to the location the gesture is performed
    dtw.setOffsetTimeseriesUsingFirstSample(true);
  

    //Add the classifier to the pipeline (after we do this, we don't need the DTW classifier anymore)
    pipeline.setClassifier( dtw );
	//pipeline.addPreProcessingModule(MovingAverageFilter(5,dimension));
	//pipeline.addFeatureExtractionModule(FFT(16,1, dimension));
	/*ClassLabelFilter myFilter = ClassLabelFilter();
	myFilter.setBufferSize(6);
	myFilter.setBufferSize(2);*/

	pipeline.addPostProcessingModule(ClassLabelChangeFilter());
	pipeline.train(trainingData);

	return true;
}


std::string GRT_Recognizer::findGesture(VectorDouble input)
{
	
		if( pipeline.getTrained()){
			pipeline.predict(input);
			UINT label = pipeline.getPredictedClassLabel();
			if(pipeline.getMaximumLikelihood() < 0.6)
				return "";




		}
		return "";
}


GRT::GestureRecognitionPipeline &getPipeline(){
	return pipeline;
};


string GRT_Recognizer::oneHandedLabelMapping(int label){
					switch(label){
				case 1:
					return "A";
				case 2:
					return "X";
				case 3:
					return "S";
				/*case 4:
					return "PAN LEFT";
				case 5:
					return "PAN RIGHT";
				case 6:
					return "PAN UP";
				case 7:
					return "PAN DOWN";
				case 8:
					return "";*/
				default:
					return "";
						
			};

}


string GRT_Recognizer::twoHandedLabelMapping(int label){
				switch(label){
				case 1:
					return "ZOOM IN";
				case 2:
					return "ZOOM OUT";
				/*case 3:
					return "ZOOM OUT";
				case 4:
					return "PAN LEFT";
				case 5:
					return "PAN RIGHT";
				case 6:
					return "PAN UP";
				case 7:
					return "PAN DOWN";
				case 8:
					return "";*/
				default:
					return "";
						
			};

}
