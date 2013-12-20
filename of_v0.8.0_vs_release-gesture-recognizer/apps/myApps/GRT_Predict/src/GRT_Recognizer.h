#include <string>
#include "GRT/GRT.h"

using namespace GRT;

#pragma once
class GRT_Recognizer
{
public:
	GRT_Recognizer(void);
	~GRT_Recognizer(void);
	bool initPipeline(string trainingdatafile, int dimension);
	std::string findGesture(VectorDouble input);
	GRT::GestureRecognitionPipeline &getPipeline();
	std::string twoHandedLabelMapping(int label);
	std::string oneHandedLabelMapping(int label);

	
	GestureRecognitionPipeline pipeline;   
};

