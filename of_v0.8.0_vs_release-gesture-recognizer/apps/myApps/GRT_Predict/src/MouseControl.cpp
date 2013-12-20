#include "MouseControl.h"
#include "NiteSampleUtilities.h"

//isMouseControledV - different from isMouseControled() to prevent errors
bool isMouseControledV;
bool initStartValue=false;

MouseControl::MouseControl(void)
{
	isMouseControledV = false;
}


MouseControl::~MouseControl(void)
{

}


bool MouseControl::startMouseControl(void)
{
		if(! isMouseControledV)
			isMouseControledV = true;

	//put cursor in the middle of the screen: SPI_GETWORKAREA
	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);
	SetCursorPos(screenX / 2, screenY / 2);
	xcursorpos = 500;
	ycursorpos = 500;
	initStartValue=true;
	return true;
}


bool MouseControl::stopMouseControl(void)
{
	isMouseControledV=false;
	return true;
}


bool MouseControl::updateMouseControl(float xnew, float ynew)
{
	if(!MouseControl::isMouseControled())
		return false;
	
	if(initStartValue){
		xold=xnew;
		yold=ynew;
		initStartValue=false;
		return true;
	}

	float xposnew, yposnew;
	//move right: coordinates become larger
	//move left: coordinates become smaller, perhaps smaller then 0
	//move down: coordinates become smaller, perhaps smaller then 0
	//move up: coordinates become larger
			
	//detect movement
	if ((xnew > xold) && (ynew<yold))		//move right and down (really down???)
	{
			xposnew = xcursorpos + (xnew - xold);

		if (ynew<0)
		{
			yposnew = ycursorpos + (ynew*(-1) + yold);
		}
		else
		{
			yposnew = ycursorpos + (ynew + yold);
		}//end check movement of y

		SetCursorPos(xposnew, yposnew);
	}
	else				//else1		
	{
		if ((xnew < xold) && (ynew < yold)) //move left and down
		{

				
		if (xnew < 0)
		{
			xposnew = xcursorpos - (xold + (xnew*(-1)));
		}
		else
		{
			xposnew = xcursorpos - (xold - xnew);
		}		//end check movement of x

		if (ynew < 0)
		{
			yposnew = ycursorpos + (ynew*(-1) + yold);
		}
		else
		{
			yposnew = ycursorpos + (ynew + yold);
		}	//end check movement of y

		SetCursorPos(xposnew, yposnew);
		}//end if left and down
		else		//else2
		{
			if ((xnew > xold) && (ynew > yold))		//move right and up
			{
					xposnew = xcursorpos + (xnew - xold);

					yposnew = ycursorpos - (ynew - yold);

				SetCursorPos(xposnew, yposnew);
			}//end move right and up
			else		//else3
			{
				if ((xnew < xold) && (ynew > yold)) //move left and up
				{

					if (xnew < 0)
					{
						xposnew = xcursorpos - (xold + (xnew*(-1)));
					}
					else
					{
						xposnew = xcursorpos - (xold - xnew);
					}		//end check movement of x


						yposnew = ycursorpos - (ynew - yold);


					SetCursorPos(xposnew, yposnew);
				}//end if left and down
			}//end else3
		}//end else
	}//end else1

	xold = xnew;
	yold = ynew;
	

	return true;
}


bool MouseControl::isMouseControled(void)
{
	return isMouseControledV;
}
