/* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and /or associated documentation files
 * (the "Materials "), to deal in the Materials without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Materials,
 * and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR
 * THE USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *
 */

#include "EGLState.h"
#include "EGLDisplay.h"
#include "EGLProcess.h"
#include "EGLThread.h"
#include "EGLContext.h"
#include "EGLConfig.h"
#include "EGLOs.h"
#include "EGLImage.h"
#include "EGLPbufferSurface.h"
#include "EGLWindowSurface.h"
#include <EGL/eglext.h>

#include <string.h>

CEGLState::CEGLState(void) :
    m_initialized( false ),
    m_currentProcess( NULL ),
    m_VGLib( NULL ),
    m_VGInterface( NULL ),
    m_GLES1Lib( NULL ),
    m_GLES1Interface( NULL ),
    m_GLES2Lib( NULL ),
    m_GLES2Interface( NULL ),
	m_hostGL( NULL ),
    m_supportedApis( 0 ),
	m_defaultDisplay( 0 ),
	m_dummyWindow( NULL )
    {

#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    CEGLProcess* p = EGLI_NEW CEGLProcess(0);
    EGLI_ASSERT( p != NULL );
    AddObject<CEGLProcess>( m_processes, p );
    m_currentProcess = p;
#endif
    }

CEGLState::~CEGLState(void)
    {
    DestroyPointerVector<CEGLProcess>( m_processes );
    DestroyPointerVector<CEGLDisplay>( m_displays );
    DestroyPointerVector<CEGLConfig>( m_configs );
    if( m_VGLib ) EGLI_ASSERT( CEGLOs::FreeClientLibrary(m_VGLib) );
    if( m_GLES1Lib ) EGLI_ASSERT( CEGLOs::FreeClientLibrary(m_GLES1Lib) );
    if( m_GLES2Lib ) EGLI_ASSERT( CEGLOs::FreeClientLibrary(m_GLES2Lib) );
#if defined(_WIN32)
#   if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    if( m_defaultDisplay )
        {
        // \todo If at some point EGL_DEFAULT_DISPLAY is created
        //       with CreateDC() the DC needs to be released by
        //       calling DeleteDC().
        //CEGLOs::DestroyDefaultDisplay( m_defaultDisplay );
        ReleaseDC( m_dummyWindow, m_defaultDisplay );
        }
    // \note Dummy window created in CEGLDisplay::Initialize(). See comment
    //       in CEGLOs::CreateDefaultDisplay().
    if( m_dummyWindow )
        {
        CEGLOs::DestroyNativeWindow( m_dummyWindow );
        }
#   endif
#endif
    }

bool CEGLState::Initialize()
    {
    if( !m_initialized )
        {
        if( !(CreateConfigs()) ) return false;
        m_initialized = true;
        m_VGInterface = CEGLOs::LoadVGInterface( m_VGLib );
        if( m_VGInterface ) 
            {
            m_VGInterface->SetEGLInterface( this );
            m_supportedApis |= EGL_OPENVG_BIT;
            }

        m_GLES1Interface = CEGLOs::LoadGLES1Interface( m_GLES1Lib );
        if( m_GLES1Interface ) 
            {
            m_GLES1Interface->SetEGLInterface( this );
            m_supportedApis |= EGL_OPENGL_ES_BIT;
            }

	    m_GLES2Interface = CEGLOs::LoadGLES2Interface( m_GLES2Lib );
        if( m_GLES2Interface ) 
            {
            m_GLES2Interface->SetEGLInterface( this );
            m_supportedApis |= EGL_OPENGL_ES2_BIT;
            }

        if( m_supportedApis & EGL_OPENGL_ES_BIT || (m_supportedApis & EGL_OPENGL_ES2_BIT) )
            {
            m_hostGL = CEGLOs::LoadHostGL();
            if( !m_hostGL ) m_initialized = false;
            }
        if( !m_supportedApis )
            {
            m_initialized = false;
            }

		//m_defaultDisplay = CEGLOs::CreateDefaultDisplay();
        // \note Creating a dummy window to get DC from. See comment
        //       in CEGLOs::CreateDefaultDisplay().
		m_dummyWindow = CEGLOs::CreateNativeWindow(0, 0);
        if( m_dummyWindow )
			{
#if defined(_WIN32)
			m_defaultDisplay = (EGLINativeDisplayType)GetDC( (HWND)m_dummyWindow );
#else // Linux
			EGLI_ASSERT( false );
#endif
			if( !m_defaultDisplay )
				{
				CEGLOs::DestroyNativeWindow( m_dummyWindow );
				m_initialized = false;
				}
			}
        }
    return m_initialized;
    }

CEGLDisplay* CEGLState::AddDisplay( EGLINativeDisplayType nativeType, EGLint processId )
    {
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    CEGLDisplay* display = EGLI_NEW CEGLDisplay( nativeType, processId );
#else
    CEGLDisplay* display = EGLI_NEW CEGLDisplay( nativeType, m_currentProcess->Id() );
#endif
    if( display )
        {
        AddObject<CEGLDisplay>( m_displays, display );
        }
    return display;
    }

void CEGLState::RemoveDisplay( EGLDisplay display )
    {
    DeleteObjectByPointer<CEGLDisplay>( m_displays, display );
    }

CEGLDisplay* CEGLState::GetDisplayByNativeType( EGLINativeDisplayType nativeType, EGLint processId ) const
    {
    CEGLDisplay* ret = NULL;
    for( std::vector<CEGLDisplay*>::const_iterator iter = m_displays.begin();
        iter != m_displays.end();
        iter++)
        {
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
        if( (*iter)->NativeType() == nativeType && (*iter)->ProcessId() == processId )
#else
        if( (*iter)->NativeType() == nativeType )
#endif
            {
            ret = (*iter);
            break;
            }
        }
    return ret;
    }

CEGLDisplay* CEGLState::GetDisplay( EGLDisplay display ) const
    {
    return FindObjectByPointer<CEGLDisplay>( m_displays, display, NULL);
    }

CEGLProcess* CEGLState::AddProcess( EGLint processId, bool setCurrent )
    {
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    CEGLProcess* process = EGLI_NEW CEGLProcess( processId );
    if( process )
        {
        AddObject<CEGLProcess>( m_processes, process );
        if( setCurrent && process )
            {
            m_currentProcess = process;
            }
        }
    return process;
#else
    EGLI_ASSERT( false );
    return NULL;
#endif
    }

void CEGLState::RemoveProcess( EGLint processId )
    {
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    if( m_currentProcess && m_currentProcess->Id() == processId )
        {
        m_currentProcess = NULL;
        }
    DeleteObjectById<CEGLProcess>( m_processes, processId );
#else
    EGLI_ASSERT( false );
#endif
    }

CEGLProcess* CEGLState::GetProcess( EGLint processId ) const
    {
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    return FindObjectById<CEGLProcess>( m_processes, processId, NULL );
#else
    return m_currentProcess;
#endif
    }

void CEGLState::SetCurrentProcessThread( EGLint processId, EGLI_THREAD_ID threadId )
    {
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    if( m_currentProcess && m_currentProcess->Id() == processId )
        {
        m_currentProcess->SetCurrentThread( threadId );
        }
    else
        {
        CEGLProcess* process = FindObjectById<CEGLProcess>( m_processes, processId, NULL);
        // processes are created in eglSimulatorSetProcessInformation()
        EGLI_ASSERT( process != NULL );
        process->SetCurrentThread( threadId );
        m_currentProcess = process;
        }
#else
    EGLI_ASSERT( false );
#endif
    }

void CEGLState::FillConfigs( EGLConfig* configs, EGLint size ) const
    {
    EGLI_ASSERT( size <= ConfigCount() );
    for( EGLint i=0; i < size; i++ )
        {
        configs[i] = (EGLConfig)m_configs[i];
        }
    }

CEGLConfig* CEGLState::FindConfigById( EGLint id ) const
    {
    return FindObjectById<CEGLConfig>( m_configs, id, NULL );
    }

EGLint CEGLState::MatchConfigs( CEGLConfig* filter, EGLConfig* configs, EGLint maxCount ) const
    {
    maxCount = EGLI_MIN( m_configs.size(), (unsigned int)maxCount );
    EGLint count = 0;
    std::vector<CEGLConfig*>::const_iterator iter = m_configs.begin();
    while( iter != m_configs.end() )
        {
        if( (*iter)->Match( *filter ) )
            {
            if( configs && count < maxCount )
                configs[count++] = (EGLConfig)(*iter);
            else if( configs )
                break;
            else
                count++;
            }
        iter++;
        }
    if( configs && count > 1 )
        {
        for( int i=0; i < count-1; i++ )
            for( int j=i+1; j < count; j++ )
                {
                if( *((CEGLConfig*)configs[j]) > *((CEGLConfig*)configs[i]) )
                    {
                    EGLConfig tmp = configs[i];
                    configs[i] = configs[j];
                    configs[j] = tmp;
                    }
                }
        }
    return count;
    }

CEGLConfig* CEGLState::GetConfig( EGLConfig config ) const
    {
    return FindObjectByPointer<CEGLConfig>( m_configs, config, NULL );
    }

void* CEGLState::GetVGContext()
    {
    CEGLOs::GetLock( &g_eglLock );
    void* ret = NULL;
    if( m_currentProcess &&
        m_currentProcess->CurrentThread() &&
        m_currentProcess->CurrentThread()->CurrentVGContext() )
        {
        ret = m_currentProcess->CurrentThread()->CurrentVGContext()->ClientContext();
        }
    CEGLOs::ReleaseLock( &g_eglLock );
    return ret;
    }

void* CEGLState::GetHostProcAddress(const char* proc)
	{
    return CEGLOs::GetGLProcAddress(m_hostGL, proc);
	}

void* CEGLState::GetGLESContext()
    {
    CEGLOs::GetLock( &g_eglLock );
    void* ret = NULL;
    if( m_currentProcess &&
        m_currentProcess->CurrentThread() &&
        m_currentProcess->CurrentThread()->CurrentGLESContext() )
        {
        ret = m_currentProcess->CurrentThread()->CurrentGLESContext()->ClientContext();
        }
    CEGLOs::ReleaseLock( &g_eglLock );
    return ret;
    }

bool CEGLState::IsImageInUse( void* image )
    {
    //TODO
    return false;
    }

bool CEGLState::LockVGSurface( bool read, bool write )
    {
    return LockSurface( EGL_OPENVG_API, read, write );
    }

bool CEGLState::UnlockVGSurface()
    {
    return UnlockSurface( EGL_OPENVG_API );
    }

void CEGLState::GetDescForImage( void* image, SurfaceDescriptor& ImageDesc )
    {
    CEGLImage* eglImage = (CEGLImage*)image;
    ImageDesc.m_stride = -1;
    ImageDesc.m_height = -1;
    ImageDesc.m_width  = -1;
    // Check that image is not null.
    if( !eglImage )
        {        
        return;
        }

    // Check that image is not from VGImage target. -> EGL_VG_PARENT_IMAGE_KHR
    if( eglImage->Target() == EGL_VG_PARENT_IMAGE_KHR )
        {
        return;
        }

	memcpy(&ImageDesc, &eglImage->SurfaceDesc(), sizeof(ImageDesc));
    }


void* CEGLState::GetDataForImage( void* image )
    {
    CEGLImage* eglImage = (CEGLImage*)image;
    // Check that image is not null.
    if( !eglImage )
        {
        // if null then error
        return NULL;
        }
	return eglImage->Data();
    }

void CEGLState::RegisterImageTarget( void* image, EImageTarget target, void* buffer )
	{
	// \todo Implement
	}

void CEGLState::UnregisterImageTarget( void* image, EImageTarget target, void* buffer )
	{
	// \todo Implement
	}

void CEGLState::UpdateImageSiblings( void* image, EImageTarget target, void* buffer )
	{
	// \todo Implement
	}

void CEGLState::ReleaseTexImage(void* surface, int name, int level)
{
	CEGLSurface* eglSurface = (CEGLSurface*)surface;
	EGLITextureBinding& binding = eglSurface->TextureBinding();
	if(binding.name == name && binding.level == level)
	{
		eglSurface->Unlock();
		binding.name = 0;
	}
}

bool CEGLState::LockGLESSurface( bool read, bool write )
    {
    return LockSurface( EGL_OPENGL_ES_API, read, write );
    }
bool CEGLState::UnlockGLESSurface()
    {
    return UnlockSurface( EGL_OPENGL_ES_API );
    }

bool CEGLState::SyncSurface( EGLenum api, EGLint apiVersion, CEGLSurface* surface, CEGLSurface* currentGLESReadSurface )
	{
	switch( api )
		{
		case EGL_OPENVG_API:
			{
			EGLint glesVersion = 0;
			if( surface->IsGLES1Dirty() )
				{
				glesVersion = 1;
				}
			else if( surface->IsGLES2Dirty() )
				{
				glesVersion = 2;
				}
			if( glesVersion )
				{
				if( surface != currentGLESReadSurface )
					{
					bool pbuffer = ( surface->Type() == CEGLSurface::PBUFFER_SURFACE );
					if( pbuffer )
						{
						if( !(((CEGLPbufferSurface*)surface)->BindCopyContext()) )
							{
							((CEGLPbufferSurface*)surface)->ReleaseCopyContext();
							
							return false;
							}
						}
					else if( surface->Type() == CEGLSurface::WINDOW_SURFACE )
						{
						// \todo Handle window surface sync properly
						}
					if( !(GLESInterface(glesVersion)->CopyBuffers(surface->GLESColorBuffer(), surface->Descriptor())) )
						{
						if( pbuffer )
							{
							((CEGLPbufferSurface*)surface)->ReleaseCopyContext();
							}
						return false;
						}
					if( pbuffer )
						{
						((CEGLPbufferSurface*)surface)->ReleaseCopyContext();
						}
					else if( surface->Type() == CEGLSurface::WINDOW_SURFACE )
						{
						// \todo Remove this or handle window surface sync properly
						//EGLI_ASSERT( false );
						}
					}
				else
					{
					if( !(GLESInterface(glesVersion)->CopyBuffers(surface->GLESColorBuffer(), surface->Descriptor())) )
						return false;
					}

				if( VGInterface() )
                    {
                    VGInterface()->UpdateBuffers(surface->GLESColorBuffer(), surface->Stride(), surface->Descriptor());
                    }
                surface->SetGLES1Dirty( false );
				surface->SetGLES2Dirty( false );
				}
			break;
			}

		case EGL_OPENGL_ES_API:
			{
			if( surface->IsVGDirty() )
                {
				if( !(GLESInterface(apiVersion)->UpdateBuffers(surface->VGColorBuffer(), surface->Descriptor())) )
                    {
                    return false;
                    }
                surface->SetVGDirty( false );
                }
			break;
			}
		}

	return true;
	}

bool CEGLState::LockSurface( EGLenum api, bool read, bool write )
    {
    CEGLOs::GetLock( &g_eglLock );
    CEGLSurface* readSurface = NULL;
    CEGLSurface* drawSurface = NULL;
	CEGLSurface* glesReadSurface = NULL;
	EGLint apiVersion;
    CEGLThread* thread = m_currentProcess->CurrentThread();
    if( !thread )
        {
        CEGLOs::ReleaseLock( &g_eglLock );
        return false;
        }
    switch( api )
        {
        case EGL_OPENVG_API:
            {
            drawSurface = thread->CurrentVGSurface();
            readSurface = drawSurface;
			apiVersion = 0;
            break;
            }
        case EGL_OPENGL_ES_API:
            {
            thread->CurrentGLESSurfaces( &readSurface, &drawSurface );
			apiVersion = thread->CurrentGLESContext()->ClientVersion();
            break;
            }
        }
    if( !readSurface && !drawSurface )
        {
        CEGLOs::ReleaseLock( &g_eglLock );
        return false;
        }

	if( api == EGL_OPENVG_API )
		{
		thread->CurrentGLESSurfaces( &readSurface, NULL );
		}
    
    if( read && readSurface )
        {
        readSurface->Lock();
		if( !SyncSurface( api, apiVersion, readSurface, glesReadSurface ) )
			{
			CEGLOs::ReleaseLock( &g_eglLock );
			return false;
			}
        }
    if( write && drawSurface && (!read || drawSurface != readSurface) )
        {
        drawSurface->Lock();
		if( !SyncSurface( api, apiVersion, drawSurface, glesReadSurface ) )
			{
			CEGLOs::ReleaseLock( &g_eglLock );
			return false;
			}
        }
    CEGLOs::ReleaseLock( &g_eglLock );
    return true;
    }

bool CEGLState::UnlockSurface( EGLenum api )
    {
    CEGLOs::GetLock( &g_eglLock );
    CEGLSurface* drawSurface = NULL;
	CEGLSurface* readSurface = NULL;
    CEGLThread* thread = m_currentProcess->CurrentThread();
    if( !thread )
        {
        CEGLOs::ReleaseLock( &g_eglLock );
        return false;
        }
    switch( api )
        {
        case EGL_OPENVG_API:
            {
            drawSurface = thread->CurrentVGSurface();
			readSurface = drawSurface;
			if( drawSurface && drawSurface->IsLocked() )
                drawSurface->SetVGDirty( true );
            break;
            }
        case EGL_OPENGL_ES_API:
            {
            thread->CurrentGLESSurfaces( &readSurface, &drawSurface );
			if( drawSurface && drawSurface->IsLocked() )
				{
				EGLint glesVersion = thread->CurrentGLESContext()->ClientVersion();
				if(glesVersion == 1)
					{
					drawSurface->SetGLES1Dirty( true );
					}
				else
					{
					EGLI_ASSERT(glesVersion == 2);
					drawSurface->SetGLES2Dirty( true );
					}
				}
            break;
            }
        }
    if( drawSurface && drawSurface->IsLocked() )
        {
        drawSurface->Unlock();
        }
    if( readSurface && readSurface->IsLocked() )
        {
        readSurface->Unlock();
        }

    CEGLOs::ReleaseLock( &g_eglLock );
    return true;
    }

bool CEGLState::CreateConfigs()
    {    
    // ** Config list START **
    // Initial config count. If adding configs to the list below
    // remember to add this count too.
    int count = 21;
    for( int i=0; i < count; i++ )
        {        
        CEGLConfig* config = EGLI_NEW CEGLConfig();
        // \note AddObject destroys the object if fails to take ownership 
        AddObject<CEGLConfig>( m_configs, config );
        if( !config )
            {
            DestroyPointerVector<CEGLConfig>( m_configs );
            return false;
            }
        }
    int configId = 0;
    CEGLConfig* config = m_configs[configId];
    //                                                 r, g, b, l, a, am, s, bpp, id
    // XRGB_8888 and ARGB_8888
                                  config->SetUpConfig( 8, 8, 8, 0, 8, 0,  0, 32,  ++configId ); //  1
    config = m_configs[configId]; config->SetUpConfig( 8, 8, 8, 0, 8, 1,  0, 32,  ++configId ); //  2
    config = m_configs[configId]; config->SetUpConfig( 8, 8, 8, 0, 8, 4,  0, 32,  ++configId ); //  3
    config = m_configs[configId]; config->SetUpConfig( 8, 8, 8, 0, 8, 8,  0, 32,  ++configId ); //  4    
    config = m_configs[configId]; config->SetUpConfig( 8, 8, 8, 0, 0, 0,  0, 32,  ++configId ); //  5
    config = m_configs[configId]; config->SetUpConfig( 8, 8, 8, 0, 0, 1,  0, 32,  ++configId ); //  6
    config = m_configs[configId]; config->SetUpConfig( 8, 8, 8, 0, 0, 4,  0, 32,  ++configId ); //  7
    config = m_configs[configId]; config->SetUpConfig( 8, 8, 8, 0, 0, 8,  0, 32,  ++configId ); //  8
    // RGB_565
    config = m_configs[configId]; config->SetUpConfig( 5, 6, 5, 0, 0, 0,  0, 16,  ++configId ); //  9
    config = m_configs[configId]; config->SetUpConfig( 5, 6, 5, 0, 0, 1,  0, 16,  ++configId ); // 10
    config = m_configs[configId]; config->SetUpConfig( 5, 6, 5, 0, 0, 4,  0, 16,  ++configId ); // 11
    config = m_configs[configId]; config->SetUpConfig( 5, 6, 5, 0, 0, 8,  0, 16,  ++configId ); // 12
    // LA_88
    config = m_configs[configId]; config->SetUpConfig( 0, 0, 0, 8, 8, 0,  0, 16,  ++configId ); // 13
    config = m_configs[configId]; config->SetUpConfig( 0, 0, 0, 8, 8, 1,  0, 16,  ++configId ); // 14
    config = m_configs[configId]; config->SetUpConfig( 0, 0, 0, 8, 8, 4,  0, 16,  ++configId ); // 15
    config = m_configs[configId]; config->SetUpConfig( 0, 0, 0, 8, 8, 8,  0, 16,  ++configId ); // 16
    // L_8
    config = m_configs[configId]; config->SetUpConfig( 0, 0, 0, 8, 0, 0,  0,  8,  ++configId ); // 17
    config = m_configs[configId]; config->SetUpConfig( 0, 0, 0, 8, 0, 1,  0,  8,  ++configId ); // 18
    config = m_configs[configId]; config->SetUpConfig( 0, 0, 0, 8, 0, 4,  0,  8,  ++configId ); // 19
    config = m_configs[configId]; config->SetUpConfig( 0, 0, 0, 8, 0, 8,  0,  8,  ++configId ); // 20
    // A_8
    config = m_configs[configId]; config->SetUpConfig( 0, 0, 0, 0, 8, 0,  0,  8,  ++configId ); // 21
    // ** Config list END **
    
    std::vector<CEGLConfig*> newConfigs;

    // stencil buffers
    std::vector<CEGLConfig*>::const_iterator iter = m_configs.begin();
    while( iter != m_configs.end() )
        {
        config = EGLI_NEW CEGLConfig();
        if( config )
            {
            *config = *(*iter);
            config->SetId( ++configId );
            config->SetAttribute( EGL_STENCIL_SIZE, 8 );
            }
        // \note AddObject destroys the object if fails to take ownership 
        AddObject<CEGLConfig>( newConfigs, config );
        if( !config )
            {
            DestroyPointerVector<CEGLConfig>( newConfigs );
            DestroyPointerVector<CEGLConfig>( m_configs );
            return false;
            }
        iter++;
        }
    try
        {
        m_configs.insert( m_configs.end(), newConfigs.begin(), newConfigs.end() );
        }
    catch( std::exception )
        {
        DestroyPointerVector<CEGLConfig>( newConfigs );
        DestroyPointerVector<CEGLConfig>( m_configs );
        return false;
        }
    newConfigs.clear();

    // depth buffers
    // 32bpp, 24bpp and 16bpp support
    for(int depthSize = 32; depthSize >= 16; depthSize -= 8 )
        {
        iter = m_configs.begin();
        while( iter != m_configs.end() )
            {
            config = EGLI_NEW CEGLConfig();
            if( config )
                {
                *config = *(*iter);
                config->SetId( ++configId );
                config->SetAttribute( EGL_DEPTH_SIZE, depthSize );
                }            
            // \note AddObject destroys the object if fails to take ownership    
            AddObject<CEGLConfig>( newConfigs, config );
            if( !config )
                {
                DestroyPointerVector<CEGLConfig>( newConfigs );
                DestroyPointerVector<CEGLConfig>( m_configs );
                return false;
                }
            iter++;
            }
        }
    try
        {
        m_configs.insert( m_configs.end(), newConfigs.begin(), newConfigs.end() );
        }
    catch( std::exception )
        {
        DestroyPointerVector<CEGLConfig>( newConfigs );
        DestroyPointerVector<CEGLConfig>( m_configs );
        return false;
        }
    newConfigs.clear();

    // multi sampling
    for( int samples = 16; samples >= 4; samples -= 12 )
        {
        iter = m_configs.begin();
        while( iter != m_configs.end() )
            {
            config = EGLI_NEW CEGLConfig();
            if( config )
                {
                *config = *(*iter);
                config->SetId( ++configId );
                config->SetAttribute( EGL_SAMPLES, samples );
                config->SetAttribute( EGL_SAMPLE_BUFFERS, 1 );
                }
            // \note AddObject destroys the object if fails to take ownership
            AddObject<CEGLConfig>( newConfigs, config );
            if( !config )
                {
                DestroyPointerVector<CEGLConfig>( newConfigs );
                DestroyPointerVector<CEGLConfig>( m_configs );
                return false;
                }
            iter++;
            }
        }
    try
        {
        m_configs.insert( m_configs.end(), newConfigs.begin(), newConfigs.end() );
        }
    catch( std::exception )
        {
        DestroyPointerVector<CEGLConfig>( newConfigs );
        DestroyPointerVector<CEGLConfig>( m_configs );
        return false;
        }
    newConfigs.clear();  
    return true;
    }

IEGLtoGLESInterface* CEGLState::GLESInterface( EGLint clientVersion ) const
	{
	EGLI_ASSERT(clientVersion == 1 || clientVersion == 2);
	return clientVersion == 1 ? m_GLES1Interface : m_GLES2Interface;
	}
