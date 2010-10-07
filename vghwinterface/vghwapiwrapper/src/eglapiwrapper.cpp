// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// 

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN                       // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include "eglapiwrapper.h"
#include "serializedfunctioncall.h"
#include "remotefunctioncall.h"
#include "khronosapiwrapperdefs.h"
#include "KhronosAPIWrapper.h"
#include "DriverAPIWrapper.h"


EGLAPIWrapper::EGLAPIWrapper( RemoteFunctionCallData& currentFunctionCallData, 
	APIWrapperStack* stack, 
	void* result, 
	MGraphicsVHWCallback* serviceIf, 
	void* surfacebuffer,
	KhronosAPIWrapper* aAPIWrapper ):
	APIWrapper( currentFunctionCallData, stack, result, serviceIf ),
	m_currentFunctionCall( m_currentFunctionCallData ),
	m_surfaceBuffer( surfacebuffer ),
	m_APIWrapper( aAPIWrapper )
{
    TRACE("EGLAPIWrapper::EGLAPIWrapper()\n");
}

void EGLAPIWrapper::SetProcessInformation( TUint32 aProcess, TUint32 aThread )
{
    TRACE("EGLAPIWrapper::SetProcessInformation()\n");
	::eglSimulatorSetProcessInformation( aProcess, aThread );
}

void EGLAPIWrapper::Cleanup( TUint32 aProcess, TUint32 aThread )
{
    TRACE("EGLAPIWrapper::Cleanup()\n");
	::eglSimulatorSetProcessInformation( aProcess, aThread );
	::eglReleaseThread();
}

int EGLAPIWrapper::WriteReply()
{
    TRACE("EGLAPIWrapper::WriteReply()\n");
#ifdef LOG_ERROR
	int operationid = (int)m_currentFunctionCall.Data().Header().iOpCode;
	int eglerror = ::eglSimulatorGetError();
	if ( m_lastEglError != eglerror )
	{
		if ( EGL_SUCCESS != eglerror )
			{
			TRACE("EGL error 0x%X, for request %d\n", eglerror, operationid );
			}
		m_lastEglError = eglerror;
	}
#endif
	return APIWrapper::WriteReply();
}

int EGLAPIWrapper::eglGetError()
{
    TRACE("EGLAPIWrapper::eglGetError()\n");
    EGLint error = ::eglGetError();
    m_currentFunctionCall.SetReturnValue( error );
    return WriteReply();
}


int EGLAPIWrapper::eglGetDisplay()
{
    TRACE("EGLAPIWrapper::eglGetDisplay() ->\n");
    EGLNativeDisplayType display_id;
    m_currentFunctionCall.GetEGLNativeDisplayType( display_id, 0 );
    /*EGLint display*/EGLDisplay display = ::eglGetDisplay( display_id );
    m_currentFunctionCall.SetReturnValue( (TUint32)display );
    TRACE("EGLAPIWrapper::eglGetDisplay() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglInitialize()
{
    TRACE("EGLAPIWrapper::eglInitialize() ->\n");
    EGLDisplay dpy;
    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    // The major and minor version numbers are set from the host side
    EGLBoolean ret = ::eglInitialize( dpy, NULL, NULL);
    m_currentFunctionCall.SetReturnValue(ret);
    TRACE("EGLAPIWrapper::eglInitialize() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglTerminate()
{
    TRACE("EGLAPIWrapper::eglTerminate ->()\n");
    EGLDisplay dpy;
    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    EGLBoolean result = ::eglTerminate( dpy );
    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglTerminate <-()\n");
    return WriteReply();
}


int EGLAPIWrapper::eglQueryString()
{
    TRACE("EGLAPIWrapper::eglQueryString() ->\n");
    EGLDisplay dpy;
    EGLint name;

    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    m_currentFunctionCall.GetEGLint( name, 1 );
    const char* result = ::eglQueryString( dpy, name );
    m_currentFunctionCall.SetReturnValue( result ? 1 : 0 );
    TRACE("EGLAPIWrapper::eglQueryString() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglGetConfigs()
{
    TRACE("EGLAPIWrapper::eglGetConfigs() ->\n");
    EGLDisplay dpy;
    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );

    EGLint num_config = 0;
    EGLint config_size = 0;
    EGLint config_limit = 0;
    EGLConfig* configs = 0;
    EGLint* num_config_data = 0;
    EGLint num_config_size = 0;

    m_currentFunctionCall.GetEGLConfigVectorData( configs, config_size, 1 );
    if ( config_size )
    {
        configs = (EGLConfig*)iStack->AllocFromStack( config_size*m_currentFunctionCall.GetTypeSize( EglRFC::EEGLConfig),
            m_currentFunctionCall.GetTypeAlignment( EglRFC::EEGLConfig) );
    }
    m_currentFunctionCall.GetEGLint( config_limit, 2 );
    m_currentFunctionCall.GetEGLintVectorData( num_config_data, num_config_size, 3 );
    EGLBoolean result = ::eglGetConfigs( dpy, configs, config_limit, num_config_size ? &num_config : NULL);
    
    if(configs)
    {
        m_currentFunctionCall.SetEGLConfigVectorData( configs, config_limit, 1 );
    }
    if (num_config_size)
    {
        if (config_size && num_config > config_limit)
            num_config = config_limit;
        m_currentFunctionCall.SetEGLintVectorData( &num_config, 1, 3 );
    }

    m_currentFunctionCall.SetReturnValue( result );
    int reply = WriteReply();

    iStack->ClearStack();
    configs = NULL;
    TRACE("EGLAPIWrapper::eglGetConfigs() <-\n");
    return reply;
}


int EGLAPIWrapper::eglChooseConfig()
{
    TRACE("EGLAPIWrapper::eglChooseConfig() ->\n");
    EGLDisplay dpy;
    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );

    EGLint num_config = 0;
    EGLint config_size = 0;
    EGLint config_limit = 0;
    EGLConfig* configs = 0;
    EGLint* attribs = 0;
    EGLint attrib_size = 0;
    EGLint* num_config_data = 0;
    EGLint num_config_size = 0;

    m_currentFunctionCall.GetEGLintVectorData( attribs, attrib_size, 1 );
    m_currentFunctionCall.GetEGLConfigVectorData( configs, config_size, 2 );
    if ( config_size )
    {
        configs = (EGLConfig*)iStack->AllocFromStack( config_size*m_currentFunctionCall.GetTypeSize( EglRFC::EEGLConfig),
            m_currentFunctionCall.GetTypeAlignment( EglRFC::EEGLConfig) );
    }
    m_currentFunctionCall.GetEGLint( config_limit, 3 );
    m_currentFunctionCall.GetEGLintVectorData( num_config_data, num_config_size, 4 );
    EGLBoolean result = ::eglChooseConfig( dpy, attribs, configs, config_limit, num_config_size ? &num_config : NULL );
    if(configs)
    {
        m_currentFunctionCall.SetEGLConfigVectorData( configs, config_limit, 2 );
    }
    if (num_config_size)
    {
        if (config_size && num_config > config_limit)
            num_config = config_limit;
        m_currentFunctionCall.SetEGLintVectorData( &num_config, 1, 4 );
    }

    m_currentFunctionCall.SetReturnValue( result );
    int reply = WriteReply();

    iStack->ClearStack();
    configs = NULL;
    TRACE("EGLAPIWrapper::eglChooseConfig() <-\n");
    return reply;
}


int EGLAPIWrapper::eglGetConfigAttrib()
{
    TRACE("EGLAPIWrapper::eglGetConfigAttrib() ->\n");
    EGLDisplay dpy;
    EGLConfig config;
    EGLint attrib;
    EGLint value = 0;
    EGLint* value_data;
    EGLint value_size;

    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    m_currentFunctionCall.GetEGLConfig( config, 1 );
    m_currentFunctionCall.GetEGLint( attrib, 2 );
    m_currentFunctionCall.GetEGLintVectorData( value_data, value_size, 3 );
    EGLBoolean result = ::eglGetConfigAttrib( dpy, config, attrib, value_size ? &value : NULL);
    
	if (value_size)
	{
        m_currentFunctionCall.SetEGLintVectorData( &value, 1, 3 );
	}

    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglGetConfigAttrib() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglCreateWindowSurface()
{
    TRACE("EGLAPIWrapper::eglCreateWindowSurface() ->\n");
    EGLDisplay dpy;
    EGLConfig config;
    EGLNativeWindowType window;

    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    m_currentFunctionCall.GetEGLConfig( config, 1 );
    m_currentFunctionCall.GetEGLNativeWindowType( window, 2 );

    EGLint* attribs;
    EGLint attrib_size;

    m_currentFunctionCall.GetEGLintVectorData( attribs, attrib_size, 3 );

	int width;
	int height;
	int horizontalPitch;
    int verticalPitch;
	m_currentFunctionCall.GetEGLint( width, 4);
	m_currentFunctionCall.GetEGLint( height, 5);
	m_currentFunctionCall.GetEGLint( horizontalPitch, 6);
	m_currentFunctionCall.GetEGLint( verticalPitch, 7);

	SymbianWindow* nativeWindow = new SymbianWindow;
	nativeWindow->id = (int)window;
	nativeWindow->width = width;
	nativeWindow->height = height;
	nativeWindow->horizontalPitch = horizontalPitch;
    nativeWindow->verticalPitch = verticalPitch;

    EGLSurface surface = ::eglCreateWindowSurface( dpy, config, (EGLNativeWindowType)nativeWindow, attribs );

	if ( EGL_NO_SURFACE != surface )
	{
		//Takes ownership of nativeWindow
		//m_windowMap.insert( std::pair<std::pair<int,int>,SymbianWindow*>( std::pair<int,int>(dpy, surface), nativeWindow ) );
		m_windowMap.insert( std::pair<TSurfaceKey,TSurfaceData>( TSurfaceKey((int)dpy, (int)surface, m_currentFunctionCall.Data().Header().iProcessId ), TSurfaceData( nativeWindow ) ) );
	}
	else
	{
		delete nativeWindow;
	}

    m_currentFunctionCall.SetReturnValue( (TUint32)surface );
    TRACE("EGLAPIWrapper::eglCreateWindowSurface() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglCreatePbufferSurface()
{
    TRACE("EGLAPIWrapper::eglCreatePbufferSurface() ->\n");
    EGLDisplay dpy;
    EGLConfig config;

    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    m_currentFunctionCall.GetEGLConfig( config, 1 );

    EGLint* attribs;
    EGLint attrib_size;

    m_currentFunctionCall.GetEGLintVectorData( attribs, attrib_size, 2 );

    EGLSurface surface = ::eglCreatePbufferSurface( dpy, config, attribs );
    m_currentFunctionCall.SetReturnValue( (TUint32)surface );
    TRACE("EGLAPIWrapper::eglCreatePbufferSurface() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglCreatePixmapSurface()
{
    TRACE("EGLAPIWrapper::eglCreatePixmapSurface() ->\n");
    EGLDisplay display;
    EGLConfig config;
    EGLNativePixmapType pixmap;
    EGLint format;
    EGLint width;
    EGLint height;
    EGLint stride;
    EGLint pixmapType;

	m_currentFunctionCall.GetEGLDisplay( display, 0 );
    m_currentFunctionCall.GetEGLConfig( config, 1 );
    m_currentFunctionCall.GetEGLNativePixmapType( pixmap, 2 );
    m_currentFunctionCall.GetEGLint( format, 3 );
    m_currentFunctionCall.GetEGLint( width, 4 );
    m_currentFunctionCall.GetEGLint( height, 5 );
    m_currentFunctionCall.GetEGLint( stride, 6 );
    m_currentFunctionCall.GetEGLint( pixmapType, 7 );

	SymbianPixmap* nativeBitmap = new SymbianPixmap;
	nativeBitmap->data = new unsigned long[(stride*height+3)/4];
	nativeBitmap->format = (TDisplayMode)format;
	nativeBitmap->width = width;
	nativeBitmap->height = height;
	nativeBitmap->stride = stride;
	nativeBitmap->type = pixmapType;

    EGLint* attribs;
    EGLint attrib_size;

    m_currentFunctionCall.GetEGLintVectorData( attribs, attrib_size, 8 );

	//Takes ownership of nativeBitmap 
    EGLSurface surface = ::eglCreatePixmapSurface( display, config, (EGLNativePixmapType)nativeBitmap, attribs );
    m_currentFunctionCall.SetReturnValue( (TUint32)surface );
    TRACE("EGLAPIWrapper::eglCreatePixmapSurface() <-\n");
	return WriteReply();
}


int EGLAPIWrapper::eglDestroySurface()
{
    TRACE("EGLAPIWrapper::eglDestroySurface() ->\n");
    EGLDisplay dpy;
    EGLConfig surface;

    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    m_currentFunctionCall.GetEGLSurface( surface, 1 );

    EGLBoolean result = ::eglDestroySurface( dpy, surface );

	if ( result )
	{
		if ( EGL_NO_SURFACE != surface )
		{
			TSurfaceMap::iterator cur = m_windowMap.find( TSurfaceKey((int)dpy, (int)surface, m_currentFunctionCall.Data().Header().iProcessId ) );
			if ( m_windowMap.end() != cur )
			{
				SymbianWindow* nativeWindow( cur->second.iWindow );
				m_windowMap.erase(cur);
				delete nativeWindow;
			}
		}
	}

    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglDestroySurface() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglQuerySurface()
{
    TRACE("EGLAPIWrapper::eglQuerySurface() ->\n");
    EGLDisplay dpy;
    EGLConfig surface;
    EGLint attrib;
    EGLint value = 0;
    EGLint* value_data;
    EGLint value_size;

    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    m_currentFunctionCall.GetEGLSurface( surface, 1 );
    m_currentFunctionCall.GetEGLint( attrib, 2 );
    m_currentFunctionCall.GetEGLintVectorData( value_data, value_size, 3 );
    EGLBoolean result = ::eglQuerySurface( dpy, surface, attrib, value_size ? &value : NULL);
    if (value_size)
        m_currentFunctionCall.SetEGLintVectorData( &value, 1, 3 );

    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglQuerySurface() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglBindAPI()
{
    TRACE("EGLAPIWrapper::eglBindAPI() ->\n");
    EGLenum api;
    m_currentFunctionCall.GetEGLenum( api, 0 );
    EGLBoolean result = ::eglBindAPI( api );
    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglBindAPI() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglQueryAPI()
{
    TRACE("EGLAPIWrapper::eglQueryAPI() ->\n");
    EGLenum result = ::eglQueryAPI();
    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglQueryAPI() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglWaitClient()
{
    TRACE("EGLAPIWrapper::eglWaitClient() ->\n");
    EGLBoolean result = ::eglWaitClient();
    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglWaitClient() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglReleaseThread()
{
    TRACE("EGLAPIWrapper::eglReleaseThread() ->\n");
    EGLBoolean result = ::eglReleaseThread();
    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglReleaseThread() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglCreatePbufferFromClientBuffer()
{
    TRACE("EGLAPIWrapper::eglCreatePbufferFromClientBuffer() ->\n");
    EGLDisplay dpy;
    EGLenum buftype;
    EGLClientBuffer buffer;
    EGLConfig config;
	
    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    m_currentFunctionCall.GetEGLenum( buftype, 1 );
    m_currentFunctionCall.GetEGLClientBuffer( buffer, 2 );
    m_currentFunctionCall.GetEGLConfig( config, 3 );

	if( buftype == EGL_OPENVG_IMAGE &&
		m_APIWrapper->GetDriverWrapper()->IsSgBackingVGImage( (VGImage)buffer ) )
	{
		//TODO EGL_BAD_ADDRESS
	}

    EGLint* attribs;
    EGLint attrib_size;

    m_currentFunctionCall.GetEGLintVectorData( attribs, attrib_size, 4 );

    EGLSurface surface = ::eglCreatePbufferFromClientBuffer( dpy, buftype, buffer, config, attribs );
    m_currentFunctionCall.SetReturnValue( (TUint32)surface );
    TRACE("EGLAPIWrapper::eglCreateBufferFromClientBuffer() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglSurfaceAttrib()
{
    TRACE("EGLAPIWrapper::eglSurfaceAttrib() ->\n");
    EGLDisplay dpy;
    EGLSurface surface;
    EGLint attrib;
    EGLint value;

    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    m_currentFunctionCall.GetEGLSurface( surface, 1 );
    m_currentFunctionCall.GetEGLint( attrib, 2 );
    m_currentFunctionCall.GetEGLint( value, 3 );

    EGLBoolean result = ::eglSurfaceAttrib( dpy, surface, attrib, value );
    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglSurfaceAttrib() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglBindTexImage()
{
    TRACE("EGLAPIWrapper::eglBindTexImage() ->\n");
    EGLDisplay dpy;
    EGLSurface surface;
    EGLint buffer;

    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    m_currentFunctionCall.GetEGLSurface( surface, 1 );
    m_currentFunctionCall.GetEGLint( buffer, 2 );

    EGLBoolean result = ::eglBindTexImage( dpy, surface, buffer );
    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglBindTexImage() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglReleaseTexImage()
{
    TRACE("EGLAPIWrapper::eglReleaseTexImage() ->\n");
    EGLDisplay dpy;
    EGLSurface surface;
    EGLint buffer;

    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    m_currentFunctionCall.GetEGLSurface( surface, 1 );
    m_currentFunctionCall.GetEGLint( buffer, 2 );

    EGLBoolean result = ::eglReleaseTexImage( dpy, surface, buffer );
    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglReleaseTexImage() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglSwapInterval()
{
    TRACE("EGLAPIWrapper::eglSwapInterval() ->\n");
    EGLDisplay dpy;
    EGLint interval;

    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    m_currentFunctionCall.GetEGLint( interval, 1 );

    EGLBoolean result = ::eglSwapInterval( dpy, interval );
    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglSwapInterval() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglCreateContext()
{
    TRACE("EGLAPIWrapper::eglCreateContext() ->\n");
    EGLDisplay dpy;
    EGLConfig config;
    EGLContext share_context;
	EGLBoolean sgSupporting(EGL_FALSE);

    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    m_currentFunctionCall.GetEGLConfig( config, 1 );
    m_currentFunctionCall.GetEGLContext( share_context, 2 );

	if( share_context == EGL_NO_CONTEXT )
	{
		share_context = m_APIWrapper->GetDriverWrapper()->GetPoolContext();
	}

    EGLint* attribs;
    EGLint attrib_size;

    m_currentFunctionCall.GetEGLintVectorData( attribs, attrib_size, 3 );

    EGLContext context = ::eglCreateContext( dpy, config, share_context, attribs );
    m_currentFunctionCall.SetReturnValue( (TUint32)context );
    TRACE("EGLAPIWrapper::eglCreateContext() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglDestroyContext()
{
    TRACE("EGLAPIWrapper::eglDestroyContext() ->\n");
    EGLDisplay dpy;
    EGLContext context;

    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    m_currentFunctionCall.GetEGLContext( context, 1 );

    EGLBoolean result = ::eglDestroyContext( dpy, context );
    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglDestroyContext() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglMakeCurrent()
{
    TRACE("EGLAPIWrapper::eglMakeCurrent() ->\n");
    EGLDisplay dpy;
    EGLSurface draw;
    EGLSurface read;
    EGLContext context;
	EGLint syncNeed(0);
	EGLint src(0);

    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    m_currentFunctionCall.GetEGLSurface( draw, 1 );
    m_currentFunctionCall.GetEGLSurface( read, 2 );
    m_currentFunctionCall.GetEGLContext( context, 3 );
	if( m_currentFunctionCall.Data().Header().iParameterCount > 4 )
	{
		m_currentFunctionCall.GetEGLint( syncNeed, 4 );
		m_currentFunctionCall.GetEGLint( src, 5 );
	}

	//Call eglMakeCurrent first
	EGLBoolean result = ::eglMakeCurrent( dpy, draw, read, context );

	//If the surface is "dirty", then it needs to be drawn the vgImage onto
	if( syncNeed & KSyncReadSurface )
	{
		int width;
		int height;
		::eglQuerySurface(dpy, read, EGL_WIDTH, &width );
		::eglQuerySurface(dpy, read, EGL_HEIGHT, &height );		
		m_APIWrapper->GetDriverWrapper()->SyncPBufferFromVGImage( (int)src, width, height );
	}
	if( syncNeed & KSyncDrawSurface )
	{
		int width;
		int height;
		::eglQuerySurface(dpy, draw, EGL_WIDTH, &width );
		::eglQuerySurface(dpy, draw, EGL_HEIGHT, &height );
		m_APIWrapper->GetDriverWrapper()->SyncPBufferFromVGImage( (int)src, width, height );
	}
    
    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglMakeCurrent() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglGetCurrentContext()
{
    TRACE("EGLAPIWrapper::eglGetCurrentContext() ->\n");
    EGLContext context = ::eglGetCurrentContext();
    m_currentFunctionCall.SetReturnValue( (TUint32)context );
    TRACE("EGLAPIWrapper::eglGetCurrentContext() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglGetCurrentSurface()
{
    TRACE("EGLAPIWrapper::eglGetCurrentSurface() ->\n");
    EGLint readdraw;

    m_currentFunctionCall.GetEGLint( readdraw, 0 );

    EGLSurface surface = ::eglGetCurrentSurface( readdraw );
    m_currentFunctionCall.SetReturnValue( (TUint32)surface );
    TRACE("EGLAPIWrapper::eglGetCurrentSurface() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglGetCurrentDisplay()
{
    TRACE("EGLAPIWrapper::eglGetCurrentDisplay() ->\n");
    EGLDisplay display = ::eglGetCurrentDisplay();
    m_currentFunctionCall.SetReturnValue( (TUint32)display );
    TRACE("EGLAPIWrapper::eglGetCurrentDisplay() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglQueryContext()
{
    TRACE("EGLAPIWrapper::eglQueryContext() ->\n");
    EGLDisplay dpy;
    EGLContext context;
    EGLint attrib;
    EGLint value = 0;
    EGLint* value_data;
    EGLint value_size;

    m_currentFunctionCall.GetEGLDisplay( dpy, 0 );
    m_currentFunctionCall.GetEGLContext( context, 1 );
    m_currentFunctionCall.GetEGLint( attrib, 2 );
    m_currentFunctionCall.GetEGLintVectorData( value_data, value_size, 3 );
    EGLBoolean result = ::eglQueryContext( dpy, context, attrib, value_size ? &value : NULL);
    if (value_size)
        m_currentFunctionCall.SetEGLintVectorData( &value, 1, 3 );
    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglQueryContext() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglWaitGL()
{
    TRACE("EGLAPIWrapper::eglWaitGL() ->\n");
    EGLBoolean result = ::eglWaitGL();
    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglWaitNative()
{
    TRACE("EGLAPIWrapper::eglWaitNative() ->\n");
    EGLint engine;

    m_currentFunctionCall.GetEGLint( engine, 0 );

    EGLBoolean result = ::eglWaitNative( engine );
    m_currentFunctionCall.SetReturnValue( result );
    TRACE("EGLAPIWrapper::eglWaitNative() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglSwapBuffers()
{
    TRACE("EGLAPIWrapper::eglSwapBuffers() ->\n");
    EGLDisplay display;
    EGLSurface surface;
	int width;
	int height;
    m_currentFunctionCall.GetEGLDisplay( display, 0 );
    m_currentFunctionCall.GetEGLSurface( surface, 1 );
    m_currentFunctionCall.GetEGLint( width, 2 );
    m_currentFunctionCall.GetEGLint( height, 3 );
	//TWindowMap::const_iterator cur  = m_windowMap.find(std::pair<int,int>( display, surface ));
	TSurfaceMap::const_iterator cur  = m_windowMap.find(TSurfaceKey( (int)display, (int)surface, m_currentFunctionCall.Data().Header().iProcessId ));

	//Only found for windows surfaces
	if ( m_windowMap.end() != cur )
	{
		cur->second.iWindow->width = width;
		cur->second.iWindow->height = height;
	}

    EGLBoolean result = ::eglSwapBuffers( display, surface );

    m_currentFunctionCall.SetReturnValue( (TUint32)result );
    TRACE("EGLAPIWrapper::eglSwapBuffers() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglCopyBuffers()
{
    TRACE("EGLAPIWrapper::eglCopyBuffers() ->\n");
    EGLDisplay display(0);
    EGLSurface surface(0);

    m_currentFunctionCall.GetEGLDisplay( display, 0 );
    m_currentFunctionCall.GetEGLSurface( surface, 1 );

	void* data(NULL);
    int size(0);
    m_currentFunctionCall.GetVectorData( data, size, 2 );

	SymbianPixmap* bitmap(NULL);
	if ( size )
	{
		bitmap = (SymbianPixmap*)iStack->AllocFromStack( sizeof(SymbianPixmap),
            m_currentFunctionCall.GetTypeAlignment( EglRFC::EEGLVoid ) );
		bitmap->data = (unsigned long*)iStack->AllocFromStack( size,
            m_currentFunctionCall.GetTypeAlignment( EglRFC::EEGLVoid ) );
		EGLint format;
		m_currentFunctionCall.GetEGLint( format, 3 );
		bitmap->format = (TDisplayMode)format;
		m_currentFunctionCall.GetEGLint( bitmap->width, 4 );
		m_currentFunctionCall.GetEGLint( bitmap->height, 5 );
		m_currentFunctionCall.GetEGLint( bitmap->stride, 6 );
		m_currentFunctionCall.GetEGLint( bitmap->type, 7 );
	}
	else
	{
		data = NULL;
	}
	
    EGLBoolean result = ::eglCopyBuffers( display, surface, (EGLNativePixmapType)bitmap );
    
	if ( result )
		{
		m_currentFunctionCall.SetVectorData( bitmap->data, size, 2 );
		}

    m_currentFunctionCall.SetReturnValue( result );
	int ret = WriteReply();
	iStack->ClearStack();
	TRACE("EGLAPIWrapper::eglCopyBiffers() <-\n");
    return ret;
}


int EGLAPIWrapper::eglSimulatorSetSurfaceParams()
{
    TRACE("EGLAPIWrapper::eglSimulatorSetSurfaceParams() ->\n");
    EGLDisplay display;
    EGLSurface surface;
    EGLint width;
    EGLint height;
    EGLint stride;
    EGLint buffer0Offset(0);
    EGLint buffer1Offset(0);
	NativeWindowType win(0);

    m_currentFunctionCall.GetEGLDisplay( display, 0 );
    m_currentFunctionCall.GetEGLSurface( surface, 1 );
    m_currentFunctionCall.GetEGLNativeWindowType( win, 2 );
    m_currentFunctionCall.GetEGLint( width, 3 );
    m_currentFunctionCall.GetEGLint( height, 4 );
    m_currentFunctionCall.GetEGLint( stride, 5 );
    m_currentFunctionCall.GetEGLint( buffer0Offset, 6 );
    m_currentFunctionCall.GetEGLint( buffer1Offset, 7 );

    void* buffer0 = (void*)((EGLint)m_surfaceBuffer + buffer0Offset);
    void* buffer1 = (void*)((EGLint)m_surfaceBuffer + buffer1Offset);

	::eglSimulatorSetSurfaceParams(display, surface, width, height, stride, buffer0, buffer1);
	m_currentFunctionCall.SetReturnValue( 0 );
	TRACE("EGLAPIWrapper::eglSimulatorSetSurfaceParams() <-\n");
    return WriteReply();
}


int EGLAPIWrapper::eglSimulatorCopyImageData()
{
    TRACE("EGLAPIWrapper::eglSimulatorCopyImageData() ->\n");
    void* data(NULL);
    int size(0);
    m_currentFunctionCall.GetVectorData( data, size, 0 );

	if ( size )
		{
		EGLDisplay display = ::eglGetCurrentDisplay();
		EGLSurface surface = ::eglGetCurrentSurface(EGL_DRAW);
		NativePixmapType nativePixmap = eglSimulatorGetPixmapSurfaceBitmap(display,surface);
		SymbianPixmap* symbianBitmap = (SymbianPixmap*)nativePixmap;
		if ( symbianBitmap )
			{
			m_currentFunctionCall.SetVectorData( symbianBitmap->data, size, 0 );
			}
		}	
    TRACE("EGLAPIWrapper::eglSimulatorCopyImageData() <-\n");
    return WriteReply();
}

int EGLAPIWrapper::eglPixmapSurfaceSizeChanged()
{
    TRACE("EGLAPIWrapper::eglPixmapSurfaceSizeChanged() ->\n");
	EGLDisplay display;
	m_currentFunctionCall.GetEGLDisplay( display, 0 );
	EGLSurface surface;
	m_currentFunctionCall.GetEGLSurface( surface, 1 );
	NativePixmapType dtype;
    m_currentFunctionCall.GetEGLNativePixmapType( dtype, 2);
	EGLint format;
	m_currentFunctionCall.GetEGLint( format, 3 );
	EGLint width;
    m_currentFunctionCall.GetEGLint( width, 4 );
	EGLint height;
    m_currentFunctionCall.GetEGLint( height, 5 );
	EGLint stride;
    m_currentFunctionCall.GetEGLint( stride, 6 );

	NativePixmapType nativePixmap = eglSimulatorGetPixmapSurfaceBitmap(display,surface);
	SymbianPixmap* symbianBitmap = (SymbianPixmap*)nativePixmap;
	if ( symbianBitmap )
	{
		symbianBitmap->format = (TDisplayMode)format;
		symbianBitmap->width = width;
		symbianBitmap->height = height;
		symbianBitmap->stride = stride;
		delete [] symbianBitmap->data;
		symbianBitmap->data = new unsigned long[(stride*height+3)/4];
	}
    TRACE("EGLAPIWrapper::eglPixmapSurfaceSizeChanged() <-\n");
    return WriteReply();
}

int EGLAPIWrapper::eglMetaGetConfigs()
{
    TRACE("EGLAPIWrapper::eglMetaGetConfigs() ->\n");
	bool abortOper = false;
	int fetchMode(0);
	m_currentFunctionCall.GetEGLint( fetchMode, 1 );//2nd parameter

	EGLint* attributes;

	EGLint red, green, blue, alpha;

	//For storing the sg-compatible configs (for a single call to eglChooseConfig)
	EGLConfig* sgConfigs = 0;
	EGLint     sgConfigsSize = 0;//size of the array
	EGLint     sgConfigsCnt = 0;//Number of actually received configurations

	//The sgConfigs for returning them
	EGLConfig* sgConfigsToReturn;
	EGLint     sgConfigsToReturnCnt;
	int        sgConfigsToReturnIndex = 0;

	//For storing all the configs
	EGLConfig*  allConfigs = 0;
	EGLint      allConfigsSize = 0;//size of the array
	EGLint      allConfigsCnt = 0;//Number of actually received configurations

	//Allocate temporary storage for the attributes for each config
	//This is actually obtained from the remote function call
	EGLint* attributesForAllConfigs(0);
	int     sizeAttributesForAllConfigs(0);
	int     sizeAttributesForAllConfigsMax(0);


	static EGLint SgPixelFormats[] =
		{
		EUidPixelFormatXRGB_8888,
		EUidPixelFormatARGB_8888,
		EUidPixelFormatARGB_8888_PRE,
		EUidPixelFormatRGB_565,
		EUidPixelFormatA_8
		};

	int sgPixelFormatsCnt = sizeof(SgPixelFormats)/sizeof(EGLint);

	//This is for storing all the attributes. This struct's values will be queried from the configs
	static EGLint configAttributes[] =
	{
		EGL_BUFFER_SIZE,
		EGL_RED_SIZE,
		EGL_GREEN_SIZE,
		EGL_BLUE_SIZE,
		EGL_LUMINANCE_SIZE,
		EGL_ALPHA_SIZE,
		EGL_ALPHA_MASK_SIZE,
		EGL_BIND_TO_TEXTURE_RGB,
		EGL_BIND_TO_TEXTURE_RGBA,
		EGL_COLOR_BUFFER_TYPE,
		EGL_CONFIG_CAVEAT,
		EGL_CONFIG_ID,
		EGL_CONFORMANT,
		EGL_DEPTH_SIZE,
		EGL_LEVEL,
		EGL_MATCH_NATIVE_PIXMAP,
		EGL_MAX_SWAP_INTERVAL,
		EGL_MIN_SWAP_INTERVAL,
		EGL_NATIVE_RENDERABLE,
		EGL_NATIVE_VISUAL_TYPE,
		EGL_RENDERABLE_TYPE,
		EGL_SAMPLE_BUFFERS,
		EGL_SAMPLES,
		EGL_STENCIL_SIZE,
		EGL_SURFACE_TYPE,
		EGL_TRANSPARENT_TYPE,
		EGL_TRANSPARENT_RED_VALUE,
		EGL_TRANSPARENT_GREEN_VALUE,
		EGL_TRANSPARENT_BLUE_VALUE
	};
	int cntConfigAttributes = sizeof(configAttributes) / sizeof(EGLint);

	sizeAttributesForAllConfigsMax = cntConfigAttributes * KConfigsMaxCnt;
	TRACE("\nEGLAPIWrapper::eglMetaGetConfigs get Vector datas\n" );
	m_currentFunctionCall.GetEGLConfigVectorData( sgConfigsToReturn, sgConfigsToReturnCnt, 0 );
	//m_currentFunctionCall.GetEGLConfigVectorData( attributesForAllConfigs, sizeAttributesForAllConfigs, 1 );
    m_currentFunctionCall.GetEGLintVectorData( attributesForAllConfigs, sizeAttributesForAllConfigs, 1 );
	TRACE("\nEGLAPIWrapper::eglMetaGetConfigs eglGetConfigs\n" );
	//Get the total number of all the configs
	::eglGetConfigs( m_APIWrapper->GetDriverWrapper()->getDisplay(), NULL, 0, &allConfigsCnt );
	TRACE("\nEGLAPIWrapper::eglMetaGetConfigs all configs count=%d\n", allConfigsCnt );
	ASSERT( allConfigsCnt <= KConfigsMaxCnt );

    if ( sizeAttributesForAllConfigs <= sizeAttributesForAllConfigsMax )
    {
		attributesForAllConfigs = (EGLint*)iStack->AllocFromStack( sizeAttributesForAllConfigs*m_currentFunctionCall.GetTypeSize( EglRFC::EEGLint ),
					m_currentFunctionCall.GetTypeAlignment( EglRFC::EEGLint ) );
		//Allocate room for holding the configs
		TRACE("\nEGLAPIWrapper::eglMetaGetConfigs attribs cnt = %d\n", cntConfigAttributes );			
		switch( fetchMode )
		{
		case EMetaGetConfigsSg:
			{
			for( int i = 0; i < sgPixelFormatsCnt; ++i )//Go through all the pixel formats, fetching configs matching them
			{
				TRACE("\nEGLAPIWrapper::eglMetaGetConfigs sg pixel formats %d\n", i );
				//This is for temporary storage. We are only interested in picking the attribute values for each configuration
				sgConfigs = new EGLConfig[KConfigsMaxCnt];//(EGLConfig*)iStack->AllocFromStack( KConfigsMaxCnt*m_currentFunctionCall.GetTypeSize( EglRFC::EEGLConfig),
							//m_currentFunctionCall.GetTypeAlignment( EglRFC::EEGLConfig) );
				sgConfigsSize = KConfigsMaxCnt;
				attributes = m_APIWrapper->GetDriverWrapper()->getColorAttributes( SgPixelFormats[i], red, green, blue, alpha );
				if( ::eglChooseConfig(	m_APIWrapper->GetDriverWrapper()->getDisplay(), 
										attributes,
										sgConfigs,
										sgConfigsSize,
										&sgConfigsCnt ) )
				{
					//TRACE("\nEGLAPIWrapper::eglMetaGetConfigs sg configs cnt / size = %d / %d\n", sgConfigsCnt, sgConfigsSize );
					sizeAttributesForAllConfigs = sgConfigsCnt * sizeof(EGLint) * cntConfigAttributes;
					for( int config_i = 0; config_i < sgConfigsCnt; ++config_i )
					{
						*(sgConfigsToReturn + (sgConfigsToReturnIndex++)) = sgConfigs[config_i]; 
						//Go through all the configs, recording the attributes
						for( int attr_i = 0; attr_i < cntConfigAttributes; ++attr_i )
						{
							EGLint attribValue;
							if( ::eglGetConfigAttrib( m_APIWrapper->GetDriverWrapper()->getDisplay(), sgConfigs[config_i], configAttributes[attr_i], &attribValue ) )
							{
								//                           #config                          #attribute
								*( attributesForAllConfigs + config_i * cntConfigAttributes + attr_i ) = attribValue;
								//TRACE("\nEGLAPIWrapper::eglMetaGetConfigs sg pixformat %d, config %d, attr %d = %d\n", i, config_i, attr_i, attribValue );
							}
							else
							{
								//Could not obtain the config attribute, now we are going to abort everything
								goto ATTRIBUTESARRAYDELETE;
							}
						}//end for all attribs within a config
					}//end for all configs returned
ATTRIBUTESARRAYDELETE:
					//We have not been able to populate the attributes list, just abort now
					delete[] sgConfigs;
					abortOper = true;
				}//end if eglGetConfigs succeeded
				else
				{
					//Problems: can't get the configs. Maybe just abort the whole business
					abortOper = true;
					break;
				}
			}//end for all pixel formats
			break;
			}//end case EMetaGetConfigsSg
		}//end switch fetchMode
	
		if( !abortOper )
		{
			//Param 0:
			TRACE("EGLAPIWrapper::eglMetaGetConfigs sgConfigsToReturnIndex = %d", sgConfigsToReturnIndex );
			//m_currentFunctionCall.SetEGLintVectorData( sgConfigsToReturn, sgConfigsToReturnIndex+1, 0 );
            m_currentFunctionCall.SetEGLConfigVectorData( sgConfigsToReturn, sgConfigsToReturnIndex+1, 0 );
			TRACE("EGLAPIWrapper::eglMetaGetConfigs sizeAttributesForAllConfigs = %d", sizeAttributesForAllConfigs / sizeof( EGLint ) );
			m_currentFunctionCall.SetEGLintVectorData( attributesForAllConfigs, sizeAttributesForAllConfigs / sizeof( EGLint ), 1 );
		}
	}//end if config_size >= KConfigsMaxCnt
	else
	{
		abortOper = true;
	}
	m_currentFunctionCall.SetReturnValue( abortOper?EGL_FALSE:EGL_TRUE );
    int reply = WriteReply();
    iStack->ClearStack();
    sgConfigs = NULL;
	allConfigs = NULL;
    TRACE("EGLAPIWrapper::eglGetConfigs() <-\n");
    return reply;
}

int EGLAPIWrapper::DispatchRequest( unsigned long aCode )
{
    int ret(0);

    switch ( aCode )
    {        
		case EglRFC::EeglGetError:
        {
            ret = eglGetError();
            break;
        }
        case EglRFC::EeglGetDisplay:
        {
            ret = eglGetDisplay();
            break;
        }
        case EglRFC::EeglInitialize:
        {
            ret = eglInitialize();
            break;
        }
        case EglRFC::EeglTerminate:
        {
            ret = eglTerminate();
            break;
        }
        case EglRFC::EeglQueryString:
        {
            ret = eglQueryString();
            break;
        }
        case EglRFC::EeglGetConfigs:
        {
            ret = eglGetConfigs();
            break;
        }
        case EglRFC::EeglChooseConfig:
        {
            ret = eglChooseConfig();
            break;
        }
        case EglRFC::EeglGetConfigAttrib:
        {
            ret = eglGetConfigAttrib();
            break;
        }
        case EglRFC::EeglCreateWindowSurface:
        {
            ret = eglCreateWindowSurface();
            break;
        }
        case EglRFC::EeglCreatePbufferSurface:
        {
            ret = eglCreatePbufferSurface();
            break;
        }
        case EglRFC::EeglCreatePixmapSurface:
        {
            ret = eglCreatePixmapSurface();
            break;
        }
        case EglRFC::EeglDestroySurface:
        {
            ret = eglDestroySurface();
            break;
        }
        case EglRFC::EeglQuerySurface:
        {
            ret = eglQuerySurface();
            break;
        }
        case EglRFC::EeglBindAPI:
        {
            ret = eglBindAPI();
            break;
        }
        case EglRFC::EeglQueryAPI:
        {
            ret = eglQueryAPI();
            break;
        }
        case EglRFC::EeglWaitClient:
        {
            ret = eglWaitClient();
            break;
        }
        case EglRFC::EeglReleaseThread:
        {
            ret = eglReleaseThread();
            break;
        }
        case EglRFC::EeglCreatePbufferFromClientBuffer:
        {
            ret = eglCreatePbufferFromClientBuffer();
            break;
        }
        case EglRFC::EeglSurfaceAttrib:
        {
            ret = eglSurfaceAttrib();
            break;
        }
        case EglRFC::EeglBindTexImage:
        {
            ret = eglBindTexImage();
            break;
        }
        case EglRFC::EeglReleaseTexImage:
        {
            ret = eglReleaseTexImage();
            break;
        }
        case EglRFC::EeglSwapInterval:
        {
            ret = eglSwapInterval();
            break;
        }
        case EglRFC::EeglCreateContext:
        {
            ret = eglCreateContext();
            break;
        }
        case EglRFC::EeglDestroyContext:
        {
            ret = eglDestroyContext();
            break;
        }
        case EglRFC::EeglMakeCurrent:
        {
            ret = eglMakeCurrent();
            break;
        }
		case EglRFC::EeglMakeCurrentSg:
		{
			break;
		}
        case EglRFC::EeglGetCurrentContext:
        {
            ret = eglGetCurrentContext();
            break;
        }
        case EglRFC::EeglGetCurrentSurface:
        {
            ret = eglGetCurrentSurface();
            break;
        }
        case EglRFC::EeglGetCurrentDisplay:
        {
            ret = eglGetCurrentDisplay();
            break;
        }
        case EglRFC::EeglQueryContext:
        {
            ret = eglQueryContext();
            break;
        }
        case EglRFC::EeglWaitGL:
        {
            ret = eglWaitGL();
            break;
        }
        case EglRFC::EeglWaitNative:
        {
            ret = eglWaitNative();
            break;
        }
        case EglRFC::EeglSwapBuffers:
        {
            ret = eglSwapBuffers();
            break;
        }
        case EglRFC::EeglCopyBuffers:
        {
            ret = eglCopyBuffers();
            break;
        }
        case EglRFC::EeglSimulatorSetSurfaceParams:
        {
            ret = eglSimulatorSetSurfaceParams();
            break;
        }
		case EglRFC::EeglSimulatorCopyImageData:
        {
            ret = eglSimulatorCopyImageData();
            break;
        }
		case EglRFC::EeglPixmapSurfaceSizeChanged:
        {
            ret = eglPixmapSurfaceSizeChanged();
            break;
        }
		case EglRFC::EeglMetaGetConfigs:
		{
			ret = eglMetaGetConfigs();
			break;
		}
        default:
            TRACE("Unimplemented EGL Op code %u\n",aCode);
            break;
    }
    return ret;
}
