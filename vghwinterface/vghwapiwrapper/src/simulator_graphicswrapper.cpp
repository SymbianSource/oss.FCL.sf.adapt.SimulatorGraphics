// This is the main DLL file.

#include "platformtypes.h"
#include <KhronosAPIWrapper.h>
#include <guestvideodriverinterfaceconstants.h>  //Registers and enums 
#include <platformthreading.h>  //mutex
#include <graphicsvhwcallback.h>
#include "simulator_graphicswrapper.h"

SimulatorGraphicsWrapper::SimulatorGraphicsWrapper()
    {															
    }

SimulatorGraphicsWrapper::~SimulatorGraphicsWrapper()
    {
    delete m_wrapper;
    m_wrapper = 0;

    Psu::platform_release_semaphore(m_outputBufferSemaphore);
    }

int SimulatorGraphicsWrapper::Reset( uint32_t *aGraphicsMemBase,  uint32_t *aCommandMemBase )
    {
    int ret = -1;
    if ( m_wrapper )
        {
        delete m_wrapper;
	    }
    m_wrapper = NULL;

    uint8_t *cmd_buffer = (uint8_t *)aCommandMemBase;
    uint8_t *frame_buffer = (uint8_t *)aGraphicsMemBase;
    if( (cmd_buffer != NULL) && (frame_buffer != NULL) )
        {
        m_wrapper = new KhronosAPIWrapper( (MGraphicsVHWCallback*)this,
            frame_buffer, &cmd_buffer[VVHW_INPUT_BASE], &cmd_buffer[VVHW_OUTPUT_BASE] );
        //Reset synchronisation mechanisms
        Psu::platform_release_semaphore(m_outputBufferSemaphore);
        Psu::platform_create_semaphore(m_outputBufferSemaphore, 1, 1);
        ret = 0;
        }
    else
        {
        ret = -1;
        }
    return ret;
    }

void SimulatorGraphicsWrapper::LockOutputBuffer()
    {
    #ifdef KHRONOS_API_W_MULTITHREAD
    Psu::platform_wait_for_signal(m_outputBufferSemaphore);
    #endif
    }

void SimulatorGraphicsWrapper::ReleaseOutputBuffer(){}

void SimulatorGraphicsWrapper::ProcessingDone(int i)
    {
    m_pythonCallBack( i );
    }

extern "C"
    {
    SIMULATOR_GRAPHICSWRAPPER_API SimulatorGraphicsWrapper* create_SimulatorGraphicsWrapper()
        {
        return new SimulatorGraphicsWrapper();
        }
    SIMULATOR_GRAPHICSWRAPPER_API int initialize_SimulatorGraphicsWrapper( SimulatorGraphicsWrapper* aSimulatorGraphicsWrapper )
        {
        Psu::platform_create_semaphore(m_outputBufferSemaphore, 1, 1);
        // Change to proper error handling
        return 0;
        }

    SIMULATOR_GRAPHICSWRAPPER_API int set_GraphicsCallBack( SimulatorGraphicsWrapper* aSimulatorGraphicsWrapper, int (*aGraphicsCallBack) (int) )
        {
        m_pythonCallBack = aGraphicsCallBack;
        // Change to proper error handling
        return 0;
        }

    SIMULATOR_GRAPHICSWRAPPER_API int reset_SimulatorGraphicsWrapper(  SimulatorGraphicsWrapper* aSimulatorGraphicsWrapper, uint32_t *aGraphicsMemBase,  uint32_t *aCommandMemBase )
        {
        return aSimulatorGraphicsWrapper->Reset( aGraphicsMemBase, aCommandMemBase );
        }

    SIMULATOR_GRAPHICSWRAPPER_API uint32_t get_InputBufferTail(  SimulatorGraphicsWrapper* aSimulatorGraphicsWrapper )
        {
        return m_wrapper->InputBufferTail();
        }
    SIMULATOR_GRAPHICSWRAPPER_API uint32_t get_InputBufferHead(  SimulatorGraphicsWrapper* aSimulatorGraphicsWrapper )
        {
        return m_wrapper->InputBufferHead( );
        }
    SIMULATOR_GRAPHICSWRAPPER_API uint32_t get_InputBufferReadCount(  SimulatorGraphicsWrapper* aSimulatorGraphicsWrapper )
        {
        return m_wrapper->InputBufferReadCount( );
        }
    SIMULATOR_GRAPHICSWRAPPER_API uint32_t get_InputBufferWriteCount(  SimulatorGraphicsWrapper* aSimulatorGraphicsWrapper )
        {
        return m_wrapper->InputBufferWriteCount( );
        }
    SIMULATOR_GRAPHICSWRAPPER_API uint32_t get_InputMaxTailIndex(  SimulatorGraphicsWrapper* aSimulatorGraphicsWrapper )
        {
        return m_wrapper->InputMaxTailIndex( );
        }
    SIMULATOR_GRAPHICSWRAPPER_API uint32_t get_cmd_memsize( void )
        {
        return (VVI_PARAMETERS_INPUT_MEMORY_SIZE +
                VVI_PARAMETERS_OUTPUT_MEMORY_SIZE);
        }
    SIMULATOR_GRAPHICSWRAPPER_API uint32_t get_framebuffer_memsize( void )
        {
        return VVI_SURFACEBUFFER_BASE_ADDRESS;
        }

    
    SIMULATOR_GRAPHICSWRAPPER_API unsigned int execute_command(  SimulatorGraphicsWrapper* aSimulatorGraphicsWrapper )
        {
        return m_wrapper->Execute( );
        }
    SIMULATOR_GRAPHICSWRAPPER_API void set_InputBufferTail(  SimulatorGraphicsWrapper* aSimulatorGraphicsWrapper, uint32_t aVal )
        {
        m_wrapper->SetInputBufferTail( aVal );
        }
    SIMULATOR_GRAPHICSWRAPPER_API void set_InputBufferHead(  SimulatorGraphicsWrapper* aSimulatorGraphicsWrapper, uint32_t aVal )
        {
        m_wrapper->SetInputBufferHead( aVal );
        }
    SIMULATOR_GRAPHICSWRAPPER_API void set_InputBufferReadCount(  SimulatorGraphicsWrapper* aSimulatorGraphicsWrapper, uint32_t aVal )
        {
        m_wrapper->SetInputBufferReadCount( aVal );
        }
    SIMULATOR_GRAPHICSWRAPPER_API void set_InputBufferWriteCount(  SimulatorGraphicsWrapper* aSimulatorGraphicsWrapper, uint32_t aVal )
        {
        m_wrapper->SetInputBufferWriteCount( aVal );
        }
    SIMULATOR_GRAPHICSWRAPPER_API void set_InputMaxTailIndex(  SimulatorGraphicsWrapper* aSimulatorGraphicsWrapper, uint32_t aVal )
        {
        m_wrapper->SetInputMaxTailIndex( aVal );
        }
    SIMULATOR_GRAPHICSWRAPPER_API void signal_outputbuffer_semafore(  SimulatorGraphicsWrapper* aSimulatorGraphicsWrapper )
        {
        #ifdef KHRONOS_API_W_MULTITHREAD
            Psu::platform_signal_semaphore(m_outputBufferSemaphore);
        #endif
        }

    }
