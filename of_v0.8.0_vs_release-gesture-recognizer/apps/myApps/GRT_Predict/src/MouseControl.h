#include "NiTE.h"


#pragma once
class MouseControl
{
public:
	MouseControl(void);
	~MouseControl(void);
	bool startMouseControl(void);
	bool stopMouseControl(void);
	bool updateMouseControl(float xnew, float ynew);
	bool isMouseControled(void);

	

	float xold, yold;
	float xcursorpos, ycursorpos;


};

