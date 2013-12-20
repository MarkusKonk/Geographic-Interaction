#pragma once

#include <OpenNI.h>

class openniProxie
{
public:
	openniProxie(void);
	~openniProxie(void);
	int initOpenNi(void);

	openni::Device	m_device;
	openni::VideoStream	m_depthStream;
	openni::VideoStream	m_colorStream;
	openni::VideoStream**	m_streams;

	openni::VideoFrameRef		m_depthFrame;
	openni::VideoFrameRef		m_colorFrame;


	void update(void);
};

