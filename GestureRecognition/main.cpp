#include "NiTE.h"

#include <NiteSampleUtilities.h>
#include <Windows.h>

nite::Point3f start;
float xnew, ynew, znew, xold, yold, zold, xcursorpos, ycursorpos;

int main(int argc, char** argv)
{
	nite::HandTracker handTracker;
	nite::Status niteRc;

	niteRc = nite::NiTE::initialize();
	if (niteRc != nite::STATUS_OK)
	{
		printf("NiTE initialization failed\n");
		return 1;
	}

	niteRc = handTracker.create();
	if (niteRc != nite::STATUS_OK)
	{
		printf("Couldn't create user tracker\n");
		return 3;
	}

	handTracker.startGestureDetection(nite::GESTURE_WAVE);
	printf("\nWave your hand to start tracking it...\n");

	//put cursor in the middle of the screen: SPI_GETWORKAREA
	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);
	SetCursorPos(screenX / 2, screenY / 2);
	xcursorpos = 500;
	ycursorpos = 500;

	nite::HandTrackerFrameRef handTrackerFrame;
	while (!wasKeyboardHit())
	{
		//check whether readframe works
		niteRc = handTracker.readFrame(&handTrackerFrame);
		if (niteRc != nite::STATUS_OK)
		{
			printf("Get next frame failed\n");
			continue;
		}

		//recognize the wave of the user and get the current position of the hand
		const nite::Array<nite::GestureData>& gestures = handTrackerFrame.getGestures();
		for (int i = 0; i < gestures.getSize(); ++i)
		{

			if (gestures[i].isComplete())
			{
				nite::HandId newId;
				handTracker.startHandTracking(gestures[i].getCurrentPosition(), &newId);
				nite::Point3f start = gestures[i].getCurrentPosition();
			}
		}

		xold = start.x;
		yold = start.y;
		zold = start.z;


		//track the hand and print the current position 
		const nite::Array<nite::HandData>& hands = handTrackerFrame.getHands();
		for (int i = 0; i < hands.getSize(); ++i)
		{
			const nite::HandData& hand = hands[i];
			float xposnew, yposnew;

			if (hand.isTracking())
			{
				printf("%d. (%5.2f, %5.2f, %5.2f)\n", hand.getId(), hand.getPosition().x, hand.getPosition().y, hand.getPosition().z);
				xnew = hand.getPosition().x;
				ynew = hand.getPosition().y;
				znew = hand.getPosition().z;
			}

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
			zold = znew;

		}
	}

	nite::NiTE::shutdown();

}
