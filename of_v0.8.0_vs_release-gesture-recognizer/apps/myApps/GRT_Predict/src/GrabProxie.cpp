#include "grabProxie.h"



GrabProxie::GrabProxie(void)
{

}


GrabProxie::~GrabProxie(void)
{
	if(m_grabDetector!=NULL)
	{
		PSLabs::ReleaseGrabDetector(m_grabDetector);
	}

	delete m_grabListener;
}


//Initializes new GrabDetector object and events
openni::Status GrabProxie::initGrabDetector( openni::Device &m_device )
{
	
	//Create object
	m_grabDetector = PSLabs::CreateGrabDetector(m_device);
	if(m_grabDetector == NULL || m_grabDetector->GetLastEvent(NULL) != openni::STATUS_OK)
	{
		printf("Error - cannot initialize grab detector: status %d \n", m_grabDetector->GetLastEvent(NULL));
		return openni::STATUS_ERROR;
	}

	//Initialize GrabDetector
	m_grabListener = new GrabEventListener(this);
	m_grabDetector->AddListener(m_grabListener);

	return openni::STATUS_OK;
}


void GrabProxie::processGrabEvent( PSLabs::IGrabEventListener::GrabEventType Type )
{
	printf("Got ");
	if(Type == PSLabs::IGrabEventListener::GRAB_EVENT)
		printf("Grab");
	else if(Type == PSLabs::IGrabEventListener::RELEASE_EVENT)
		printf("Release");
	else if(Type == PSLabs::IGrabEventListener::NO_EVENT)
		printf("No Event?!");
	printf(" event\n");
}


void GrabProxie::updateAlgorithm(bool &handLost, bool &handTracked, Point3f &handCoord, 	openni::VideoFrameRef& m_depthFrame, openni::VideoFrameRef& m_colorFrame)
{
	/*	m_depthStream.readFrame(&m_depthFrame);
	if(m_colorStream.isValid())
		m_colorStream.readFrame(&m_colorFrame);*/
	//
	//bool handLost = false, gestureComplete = false, handTracked = false;

	//Update NiTE trackers and get their result
	//UpdateNiTETrackers(&handLost, &gestureComplete, &handTracked, &handX, &handY, &handZ);

	if(m_grabDetector != NULL)
	{
		//If the hand is lost, we need to reset the grab detector
		if(handLost)
			m_grabDetector->Reset();
		//If a gesture is just complete, or the hand is already being tracked, we have valid coordinates and can set them to the detector
		else if(handTracked)
			m_grabDetector->SetHandPosition(handCoord.x, handCoord.y, handCoord.z);

		//Update algorithm with the newly read frames. We prefer both frames, but can work only with one
		if(m_depthFrame.isValid() && m_colorFrame.isValid())
			m_grabDetector->UpdateFrame(m_depthFrame, m_colorFrame);
		else if(m_depthFrame.isValid())
			m_grabDetector->UpdateFrame(m_depthFrame);
	}
}
