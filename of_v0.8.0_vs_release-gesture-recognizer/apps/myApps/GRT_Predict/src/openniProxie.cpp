#include "openniProxie.h"
#include <OpenNI.h>
#include <fstream>



openniProxie::openniProxie(void)
{
	

	//openni::VideoStream**	m_streams(NULL);
	/*
	//const char* strSampleName, openni::Device& device, openni::VideoStream& depth, openni::VideoStream& color) :
	m_device(device); 
	m_depthStream(depth); 
	m_colorStream(color); 
	m_streams(NULL);
	//m_eViewState(DEFAULT_DISPLAY_MODE), 
	//m_pTexMap(NULL), 
	m_grabListener(NULL); 
	m_grabDetector(NULL);*/
	
	/*openni::Device			m_device;
	openni::VideoStream	m_depthStream;
	openni::VideoStream	m_colorStream;
	openni::VideoStream**	m_streams;*/

}


openniProxie::~openniProxie(void)
{
	if (m_streams != NULL)
	{
		delete []m_streams;
	}
}


int openniProxie::initOpenNi(void)
{

	openni::Status rc = openni::STATUS_OK;
	
	const char* deviceURI = openni::ANY_DEVICE;
	/*if (argc > 1)
	{
		deviceURI = argv[1];
	}*/

	rc = openni::OpenNI::initialize();

	if (rc != openni::STATUS_OK)
	{
		printf("OpenNI Initialization failed:\n%s\n", openni::OpenNI::getExtendedError());
		return -1;
	}

	/*if (nite::NiTE::initialize() != nite::STATUS_OK)
	{
		printf("NiTE2 Initialization failed!");
		return -1;
	}*/

	rc = m_device.open(deviceURI);
	if (rc != openni::STATUS_OK)
	{
		printf("Device open failed:\n%s\n", openni::OpenNI::getExtendedError());
		openni::OpenNI::shutdown();
		return -1;
	}
	rc = m_device.setDepthColorSyncEnabled(true);

	openni::VideoMode videoMode;
	int resX = 640, resY = 480, fps = 30;

	rc = m_depthStream.create(m_device, openni::SENSOR_DEPTH);
	if (rc == openni::STATUS_OK)
	{
		videoMode = m_depthStream.getVideoMode();
		videoMode.setFps(fps);
		videoMode.setResolution(resX, resY);
		rc = m_depthStream.setVideoMode(videoMode);
		if(rc != openni::STATUS_OK)
		{
			printf("Couldn't set depth mode:\n%s\n", openni::OpenNI::getExtendedError());
			return -1;
		}
		
		rc = m_depthStream.start();
		if (rc != openni::STATUS_OK)
		{
			printf("Couldn't start depth stream:\n%s\n", openni::OpenNI::getExtendedError());
			m_depthStream.destroy();
			return -1;
		}
	}
	else
	{
		printf("Couldn't find depth stream:\n%s\n", openni::OpenNI::getExtendedError());
		return -1;
	}


	rc =m_colorStream.create(m_device, openni::SENSOR_COLOR);
	if (rc == openni::STATUS_OK)
	{
		videoMode =m_colorStream.getVideoMode();
		videoMode.setFps(fps);
		videoMode.setResolution(resX, resY);
		rc =m_colorStream.setVideoMode(videoMode);
		if(rc != openni::STATUS_OK)
		{
			printf("Couldn't set color mode:\n%s\n", openni::OpenNI::getExtendedError());
			return -1;
		}

		rc =m_colorStream.start();
		if (rc != openni::STATUS_OK)
		{
			printf("Couldn't startm_colorStream stream:\n%s\n", openni::OpenNI::getExtendedError());
			m_colorStream.destroy();
			return -1;
		}
	}
	else
	{
		printf("Couldn't find color stream:\n%s\n", openni::OpenNI::getExtendedError());
		return -1;
	}

	//Set stream parameters
	m_depthStream.setMirroringEnabled(true);
	m_colorStream.setMirroringEnabled(true);

	//Set registration. This is very important as we cannot use image stream if they are not registered
	rc = m_device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
	if (rc != openni::STATUS_OK)
	{
		printf("Couldn't set image to depth registration. Disabling image stream.\n%s\n", openni::OpenNI::getExtendedError());
		m_colorStream.stop();
		m_colorStream.destroy();
	}
	
	if (!m_depthStream.isValid())
	{
		printf("No valid depth stream. Exiting\n");
		openni::OpenNI::shutdown();
		return -1;
	}

	/*std::ifstream file("Data/grab_gesture.dat");
	if (!file)
	{
		printf("Cannot find \"Data/grab_gesture.dat\"");
		openni::OpenNI::shutdown();
		return -1;
	}
	file.close();*/

	m_streams = new openni::VideoStream*[2];
	m_streams[0] = &m_depthStream;
	m_streams[1] = &m_colorStream;

}


void openniProxie::update(void)
{
	int changedIndex = 0;
	openni::Status rc = openni::STATUS_OK;
	
	//Read frames
	rc = openni::OpenNI::waitForAnyStream(m_streams, 2, &changedIndex);
	if (rc != openni::STATUS_OK)
	{
		printf("Wait failed\n");
		return;
	}
	m_depthStream.readFrame(&m_depthFrame);
	if(m_colorStream.isValid())
		m_colorStream.readFrame(&m_colorFrame);
}
