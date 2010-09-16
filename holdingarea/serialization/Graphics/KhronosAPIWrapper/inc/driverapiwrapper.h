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

#ifndef DRIVERAPIWRAPPER_H
#define DRIVERAPIWRAPPER_H

#include <list>

#include "apiwrapper.h"
#include "graphicsvhwcallback.h"        //callback interface Wrapper=>Virtual HW
#include "khronosapiwrapperdefs.h"
#include "driverrfc.h"

#include "eglapiwrapper.h"//needed to request thread switch for egl during some big operations
class KhronosAPIWrapper;

#ifndef __PIXELFORMATS_H__
#define __PIXELFORMATS_H__
/*
typedef enum
	{
    VG_sXRGB_8888                               =  0 | (1 << 6),
	VG_sARGB_8888                               =  1 | (1 << 6),
	VG_sARGB_8888_PRE                           =  2 | (1 << 6),
	VG_sRGB_565                                 =  3,
	VG_A_8                                      = 11,
	} VGImageFormat;*///TODO: get these from somewhere (copy sgconst.h and pixelformats.h maybe)

typedef enum
	{
	/**
	 32-bit RGB pixel format without alpha, with 8 bits are reserved for each color
	 */
	EUidPixelFormatXRGB_8888 = 0x10275496,
	/**
	 32-bit ARGB pixel format with alpha, using 8 bits per channel
	 */
	EUidPixelFormatARGB_8888 = 0x10275498,
	/**
	 32-bit ARGB pixel format with alpha (pre-multiplied), using 8 bits per channel
	 */
	EUidPixelFormatARGB_8888_PRE = 0x1027549A,
	/**
	 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for
	 blue
	 */
	EUidPixelFormatRGB_565 = 0x1027549E,
	/**
	 8-bit alpha only
	 */
	EUidPixelFormatA_8 = 0x102754A3,


	} TUidPixelFormat;//TODO: get these from somewhere (copy sgconst.h and pixelformats.h maybe)

#endif




class DriverAPIWrapper: public APIWrapper
{
public:
	DriverAPIWrapper( RemoteFunctionCallData& currentFunctionCallData, APIWrapperStack* stack,
        void* result,
		MGraphicsVHWCallback* serviceIf,
		KhronosAPIWrapper* aAPIWrapper ):
	APIWrapper( currentFunctionCallData, stack, result, serviceIf ),
	m_currentFunctionCall( m_currentFunctionCallData ),
	m_APIWrapper( aAPIWrapper ),
	m_Dpy( EGL_NO_DISPLAY ),
	m_PoolConfig( 0 ),
	m_PoolContext( EGL_NO_CONTEXT ),
    m_ContextWithFormatARGB_8888_PRE( EGL_NO_CONTEXT ),
    m_ContextWithFormatARGB_8888( EGL_NO_CONTEXT ),
    m_ContextWithFormatXRGB_8888( EGL_NO_CONTEXT ),
    m_ContextWithFormatRGB_565( EGL_NO_CONTEXT ),
    m_ContextWithFormatA_8( EGL_NO_CONTEXT ),
    m_DudSurface( EGL_NO_SURFACE )
	{
		m_initialized = false;
	}

	bool IsSgBackingVGImage( VGImage aImage );

	bool IsSgBackingPBuffer( EGLSurface aSurface );

	void SetProcessInformation( TUint32 aProcess, TUint32 aThread )
	{
	}

	void Cleanup( TUint32 aProcess, TUint32 aThread )
	{
	}

    int DispatchRequest( unsigned long aCode )
	{
		TRACE("DriverAPIWrapper DispatchRequest() in proc/thread %u / %u \n", m_currentFunctionCall.Data().Header().iProcessId, m_currentFunctionCall.Data().Header().iProcessId );
		int ret(0);
		switch ( aCode )
		{
            case DriverRFC::EDrvCreatePoolContext:
			{
			ret = CreatePoolContext();
			break;
			}
		case DriverRFC::EDrvCreatePbufferSg:
			{
			ret = CreatePbufferSg();
			break;
			}
		case DriverRFC::EDrvCreateVGImageSg:
			{
			ret = CreateVGImageSg();
			break;
			}
		case DriverRFC::EDrvDeleteSgImage:
			{
			ret = DeleteSgImage();
			break;
			}
		case DriverRFC::EDrvSyncVGImageFromPBuffer:
			{
			//ret = SyncVGImageFromPBuffer();
			break;
			}
		default:
			{
			ASSERT( FALSE );
			break;
			}
		}
		return ret;
	}
	EGLContext GetPoolContext(){ return m_PoolContext; }

	//API for external users, such as the egl wrapper
	void SyncVGImageFromPBuffer( EGLSurface aPbuffer, VGImage aVGImage );
	int SyncPBufferFromVGImage( VGImage aSrc, VGint aWidth, VGint aHeight );

	EGLint* getColorAttributes( int aPixelFormat, int& aRed, int& aGreen, int& aBlue, int& aAlpha );
	EGLDisplay getDisplay(){ if( !m_initialized ){ CreatePoolContext(); }return m_Dpy; }

protected:
//Request handlers
	int ClientShutdown()
	{
	TRACE("KhronosAPIWrapper ClientShutdown()\n");
    return WriteReply();
	}

	int CreatePoolContext();

	int CreatePbufferSg();

	int CreateVGImageSg();

	int DeleteSgImage();


	VGImageFormat getVGColorFormat( int aPixelFormat );

private:
	DriverRFC m_currentFunctionCall;
	KhronosAPIWrapper* m_APIWrapper;//not owned here

	std::list<VGImage> m_SgImageVGImages;
	std::list<EGLSurface> m_SgImagePbuffers;

	bool m_initialized;

	EGLDisplay m_Dpy;
	EGLConfig  m_PoolConfig;
	EGLContext m_PoolContext;
    EGLContext m_ContextWithFormatARGB_8888_PRE;
    EGLContext m_ContextWithFormatARGB_8888;
    EGLContext m_ContextWithFormatXRGB_8888;
    EGLContext m_ContextWithFormatRGB_565;
    EGLContext m_ContextWithFormatA_8;
    EGLSurface m_DudSurface;
};
#endif//DRIVERAPIWRAPPER
