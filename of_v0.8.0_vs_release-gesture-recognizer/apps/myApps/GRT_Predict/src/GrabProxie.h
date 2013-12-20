#include "GrabDetector.h";
#include <NiTE.h>
using namespace nite;

#pragma once
class GrabProxie
{
		
public:
	GrabProxie(void);
	~GrabProxie(void);

	PSLabs::IGrabDetector* m_grabDetector;
	openni::Status GrabProxie::initGrabDetector( openni::Device &m_device );
	void processGrabEvent( PSLabs::IGrabEventListener::GrabEventType Type );
	void GrabProxie::updateAlgorithm(bool &handlost, bool &handTracked, Point3f &handCoord, openni::VideoFrameRef& m_depthFrame, openni::VideoFrameRef& m_colorFrame);

	class GrabEventListener : public PSLabs::IGrabEventListener
	{
	public:
		GrabEventListener(GrabProxie* proxie) : m_proxie(proxie)
		{

		}
		virtual void DLL_CALL ProcessGrabEvent( const EventParams& params )
		{
			m_proxie->processGrabEvent(params.Type);
		}

		GrabProxie* m_proxie;
	};

		GrabEventListener* m_grabListener;
};

