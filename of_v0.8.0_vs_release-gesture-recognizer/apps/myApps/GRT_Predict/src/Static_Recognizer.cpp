#include "Static_Recognizer.h"
  UINT trainingClassLabel;

Static_Recognizer::Static_Recognizer(void)
{
}


Static_Recognizer::~Static_Recognizer(void)
{
}
GestureRecognitionPipeline pipeline_anbc;   

bool Static_Recognizer::initPipeline(string trainingdatafile, int dimension)
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
    
     //Setup the training data
    trainingData.setNumDimensions( 6 );
    trainingClassLabel = 1;

    //Setup the classifier
    ANBC anbc;
    anbc.enableNullRejection(true);
    anbc.setNullRejectionCoeff(5);
    pipeline_anbc.setClassifier( anbc );

	pipeline_anbc.addPostProcessingModule(ClassLabelChangeFilter());
	pipeline_anbc.train(trainingData);

	return true;
}


std::string Static_Recognizer::findGesture(VectorDouble input)
{
	
		if( pipeline_anbc.getTrained()){
			pipeline_anbc.predict(input);
			UINT label = pipeline_anbc.getPredictedClassLabel();
			if(pipeline_anbc.getMaximumLikelihood() < 0.6)
				return "";




		}
		return "";
}




string Static_Recognizer::oneHandedLabelMapping(int label){
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


string Static_Recognizer::twoHandedLabelMapping(int label){
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