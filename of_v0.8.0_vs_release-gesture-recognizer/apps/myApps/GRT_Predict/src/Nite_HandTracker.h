#include <NiTE.h>
using namespace nite;

#pragma once
class Nite_HandTracker
{
public:
	Nite_HandTracker(void);
	~Nite_HandTracker(void);
	int initHandTracker(void);
	Point3f getLeftHandCoordinates(void);
	void updateHandTracker(void);
	Point3f getRightHandCoordinates(void);
//	bool isLeftHandRecognized(void);
	bool isRightHandTracked(void);
	bool isLeftHandTracked(void);
};

