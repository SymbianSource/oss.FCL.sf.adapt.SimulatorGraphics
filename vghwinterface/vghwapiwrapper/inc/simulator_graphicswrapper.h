#ifndef _SIMULATOR_GRAPHICSWRAPPER_H
#define _SIMULATOR_GRAPHICSWRAPPER_H

#pragma once
					  
#ifdef WIN32
#ifdef SIMULATOR_GRAPHICSWRAPPER_EXPORTS
#define SIMULATOR_GRAPHICSWRAPPER_API __declspec(dllexport)
#else
#define SIMULATOR_GRAPHICSWRAPPER_API __declspec(dllimport)
#endif
#else
#define SIMULATOR_GRAPHICSWRAPPER_API
#endif

Psu::PLATFORM_SEMAPHORE_T m_outputBufferSemaphore;
int (*m_pythonCallBack)(int); 
KhronosAPIWrapper* m_wrapper;
														   

// Derived from GraphicsVirtualHW.lisa
static const int VVHW_BUFFER (0x3000000);
static const int VVHW_INPUT_BUFFER (0x1000000);
static const int VVHW_INPUT_BASE(0x0);
static const int VVHW_OUTPUT_BUFFER (0x1000000);
static const int VVHW_OUTPUT_BASE(0x1000000);
static const int VVHW_FRAME_BUFFER (0x1000000);
static const int VVHW_FRAME_BASE(0x2000000);

class SimulatorGraphicsWrapper : public MGraphicsVHWCallback
    {
    public:

        SimulatorGraphicsWrapper();
        ~SimulatorGraphicsWrapper();

        int Reset( uint32_t *aGraphicsMemBase,  uint32_t *aCommandMemBase );

        virtual void LockOutputBuffer();
	    virtual void ReleaseOutputBuffer();
	    virtual void ProcessingDone(int i);

    private:
    };


#endif // _SIMULATOR_GRAPHICSWRAPPER_H
