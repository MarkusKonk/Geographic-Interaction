#include "testApp.h"



MouseControl mouseControl;
GRT_Recognizer recognizer;
GRT_Recognizer oneHandrecognizer;
Nite_HandTracker tracker;
GrabProxie grab;
openniProxie openniP;

//--------------------------------------------------------------
void testApp::setup(){
	//mouseControl.startMouseControl();
	recognizer.initPipeline("TrainingData_v3_zoomIn_ZoomOut.txt", 6);
	oneHandrecognizer.initPipeline("TrainingData_A_X_S.txt", 3);
	openniP.initOpenNi();
	tracker.initHandTracker();
	grab.initGrabDetector(openniP.m_device);
}

//--------------------------------------------------------------
void testApp::update(){
	float xnew, ynew;
	VectorDouble inputGRT(6);
	VectorDouble inputGRT2(3);
	//retrieve data from HandTracker
	tracker.updateHandTracker();
	openniP.update();
	if(tracker.isRightHandTracked()){
		inputGRT2[0] = inputGRT[0] = xnew = tracker.getRightHandCoordinates().x;
		inputGRT2[1] = inputGRT[1] = ynew = tracker.getRightHandCoordinates().y;
		inputGRT2[2] = inputGRT[2] = tracker.getRightHandCoordinates().z;
		mouseControl.updateMouseControl(xnew, ynew);
		
		GestureRecognitionPipeline &pipeline = oneHandrecognizer.pipeline;
		pipeline.predict(inputGRT2);
		//printf("Input: (%d, %d, %d)\n", inputGRT2[0], inputGRT2[1], inputGRT2[2]);
		string message = oneHandrecognizer.oneHandedLabelMapping(pipeline.getPredictedClassLabel());
		double likelyhood= pipeline.getMaximumLikelihood();
			if(message != "" && likelyhood>0.7)
				printf("\nGesture: %s\n", message.c_str());

			bool lost = false;
			bool track = true;
			grab.updateAlgorithm(lost, track, tracker.getRightHandCoordinates(), openniP.m_depthFrame, openniP.m_colorFrame);
			
		if(tracker.isLeftHandTracked()){
			inputGRT[3] = tracker.getLeftHandCoordinates().x;
			inputGRT[4] = tracker.getLeftHandCoordinates().y;
			inputGRT[5] = tracker.getLeftHandCoordinates().z;
			
			GestureRecognitionPipeline &pipeline = recognizer.pipeline;
			pipeline.predict(inputGRT);
			string message = recognizer.twoHandedLabelMapping(pipeline.getPredictedClassLabel());
			double likelyhood= pipeline.getMaximumLikelihood();
			if(message != "" && likelyhood>0.9)
				printf("\nGesture: %s\n", message.c_str());
		}	
	}



}

//--------------------------------------------------------------
void testApp::draw(){
	   
    ofBackground(0, 0, 0);
    
    string text;
    int textX = 20;
    int textY = 20;
    
    //Draw the training info
    ofSetColor(255, 255, 255);
 
    text = "------------------- Prediction Info -------------------";
    ofDrawBitmapString(text, textX,textY);
    GestureRecognitionPipeline &pipeline = recognizer.pipeline;

    /*textY += 15;
    text =  pipeline.getTrained() ? "Model Trained: YES" : "Model Trained: NO";
    ofDrawBitmapString(text, textX,textY);*/
    
    textY += 15;
    text = "PredictedClassLabel: " + ofToString(pipeline.getPredictedClassLabel());
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "Likelihood: " + ofToString(pipeline.getMaximumLikelihood());
    ofDrawBitmapString(text, textX,textY);
	  textY += 15;
	    text = "------------------- Prediction Info2 -------------------";
    ofDrawBitmapString(text, textX,textY);
    
	GestureRecognitionPipeline &pipeline2 = oneHandrecognizer.pipeline;
    /*textY += 15;
    text =  pipeline2.getTrained() ? "Model Trained: YES" : "Model Trained: NO";
    ofDrawBitmapString(text, textX,textY);*/
    
    textY += 15;
    text = "PredictedClassLabel: " + ofToString(pipeline2.getPredictedClassLabel());
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "Likelihood: " + ofToString(pipeline2.getMaximumLikelihood());
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "SampleRate: " + ofToString(ofGetFrameRate(),2);
    ofDrawBitmapString(text, textX,textY);
    
    /*
    //Draw the info text
    textY += 30;
    text = "InfoText: " + infoText;
    ofDrawBitmapString(text, textX,textY);*/

	//Draw number of hands currently dragged
	ofSetColor(255, 0, 0);
    textY += 15;
	text = "Left Hand is tracked: "+ ofToString(tracker.isLeftHandTracked());
	ofDrawBitmapString(text, textX,textY);
	    textY += 15;
	text = "Right Hand is tracked: "+ ofToString(tracker.isRightHandTracked());
	ofDrawBitmapString(text, textX,textY);
    ofSetColor(255, 255, 255);

    //Draw the timeseries data
   // if( record  && noOfHands == noOfTrackedHands){
        ofFill();
            //double r = ofMap(i,0,timeseries.getNumRows(),0,255);
            //double g = 0;
            //double b = 255-r;
		    //ofSetColor(r,g,b); 
            ofSetColor(250,0,0);
			int x = tracker.getLeftHandCoordinates().x;
			int	y = tracker.getLeftHandCoordinates().y;
            ofEllipse(250+x,500-y,5,5);

		 //  if(noOfHands >= 2){
				ofSetColor(0,80,255);
				x = tracker.getRightHandCoordinates().x;
				y = tracker.getRightHandCoordinates().y;
				ofEllipse(350+x,500-y,5,5);
			//}
        
   // }
 
    

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}


