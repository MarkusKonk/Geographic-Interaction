#include <string>
#include "GRT/GRT.h"

using namespace GRT;

#pragma once
class Static_Recognizer
{
public:
	Static_Recognizer(void);
	~Static_Recognizer(void);
		bool initPipeline(string trainingdatafile, int dimension);
	std::string findGesture(VectorDouble input);
//	GRT::GestureRecognitionPipeline &getPipeline();
	std::string twoHandedLabelMapping(int label);
	std::string oneHandedLabelMapping(int label);
};

