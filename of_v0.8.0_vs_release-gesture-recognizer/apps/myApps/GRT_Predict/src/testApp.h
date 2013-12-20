#pragma once

#include "ofMain.h"
#include "GRT/GRT.h"


#include "NiTE.h"
#include "MouseControl.h"
#include "Nite_HandTracker.h"
#include "GRT_Recognizer.h"
#include "GrabProxie.h";
#include "openniproxie.h";

using namespace GRT;
using namespace nite;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
};
