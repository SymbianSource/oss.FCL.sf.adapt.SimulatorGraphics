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

#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>//EGL operations will be called a lot from here
#include <vg/openvg.h>

#include "driverapiwrapper.h"
#include "KhronosAPIWrapper.h"


bool DriverAPIWrapper::IsSgBackingVGImage( VGImage aImage )
	{
		for( std::list<VGImage>::iterator cur = m_SgImageVGImages.begin();
			 cur != m_SgImageVGImages.end();
			 ++cur )
		{
			if( (*cur) == aImage )
			{
				return true;
			}
		}
		return false;
	}

bool DriverAPIWrapper::IsSgBackingPBuffer( EGLSurface aSurface )
	{
		for( std::list<EGLSurface>::iterator cur = m_SgImagePbuffers.begin();
			 cur != m_SgImagePbuffers.end();
			 ++cur )
		{
			if( (*cur) == aSurface )
			{
				return true;
			}
		}
		return false;
	}

int DriverAPIWrapper::CreatePoolContext()
	{
	//Initialize EGL on the first run, as per 5.1 in the "Implementing SgImage" design doc
	if( m_initialized ){ return 0; }
	TRACE("DriverAPIWrapper::CreatePoolContext() ->\n");
    EGLDisplay display(0);

    EGLConfig config(0);
	EGLint num_config;

	EGLContext PoolContext(0);//for sharing
    EGLContext ContextWithFormatARGB_8888_PRE(0);//for sharing
    EGLContext ContextWithFormatARGB_8888(0);
    EGLContext ContextWithFormatXRGB_8888(0);
    EGLContext ContextWithFormatRGB_565(0);
    EGLContext ContextWithFormatA_8(0);
    EGLSurface DudSurface(0);

	int red(0);
	int green(0);
	int blue(0);
	int alpha(0);
    
	display = ::eglGetDisplay( EGL_DEFAULT_DISPLAY );//Notice: there is no support for multiple displays here

	if( !::eglInitialize(display, NULL, NULL) )
	{
		TRACE("DriverAPIWrapper::CreatePoolContext() <- (eglInitialize failed)\n");
		return 0;
	}

	m_Dpy = display;
	//Pool Context
    //get a Configuration, then get the context
	if( ::eglChooseConfig(	display,
							getColorAttributes( EUidPixelFormatARGB_8888, red, green, blue, alpha ),
							&config,
							1,
							&num_config ) == EGL_TRUE )
	{
		/* create an EGL rendering context */
		PoolContext = ContextWithFormatARGB_8888 = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
		m_PoolConfig = config;
	}


    /* get an appropriate EGL frame buffer configuration */
	if( ::eglChooseConfig(	display,
							getColorAttributes( EUidPixelFormatXRGB_8888, red, green, blue, alpha ),
							&config,
							1,
							&num_config ) == EGL_TRUE )
	{
		/* create an EGL rendering context */
		PoolContext = ContextWithFormatXRGB_8888 = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
	}


    //get a Configuration, then get the context
	if( ::eglChooseConfig(	display,
							getColorAttributes( EUidPixelFormatARGB_8888_PRE, red, green, blue, alpha ),
							&config,
							1,
							&num_config ) == EGL_TRUE )
	{
		/* create an EGL rendering context */
		ContextWithFormatARGB_8888_PRE = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
	}
	

    //get a Configuration, then get the context
	if( ::eglChooseConfig(	display,
							getColorAttributes( EUidPixelFormatRGB_565, red, green, blue, alpha ),
							&config,
							1,
							&num_config ) == EGL_TRUE )
	{
		/* create an EGL rendering context */
		ContextWithFormatRGB_565 = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
	}
	

    //get a Configuration, then get the context
	if( ::eglChooseConfig(	display,
							getColorAttributes( EUidPixelFormatA_8, red, green, blue, alpha ),
							&config,
							1,
							&num_config ) == EGL_TRUE )
	{
		/* create an EGL rendering context */
		ContextWithFormatA_8 = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
	}


	EGLint attribs[] = { EGL_NONE };
	DudSurface = ::eglCreatePbufferSurface( display, PoolContext, attribs );


	TRACE( "PoolContext = %u\n", PoolContext );
	TRACE( "ContextWithFormatARGB_8888_PRE = %u\n", ContextWithFormatARGB_8888_PRE );
	TRACE( "ContextWithFormatARGB_8888 = %u\n", ContextWithFormatARGB_8888 );
	TRACE( "ContextWithFormatXRGB_8888 = %u\n", ContextWithFormatXRGB_8888 );
	TRACE( "ContextWithFormatRGB_565 = %u\n", ContextWithFormatRGB_565 );
	TRACE( "ContextWithFormatA_8 = %u\n", ContextWithFormatA_8 );
	TRACE( "DummySurface = %u\n", DudSurface );

	m_Dpy								= display;
	m_PoolContext						= PoolContext;
    m_ContextWithFormatARGB_8888_PRE	= ContextWithFormatARGB_8888_PRE;
    m_ContextWithFormatARGB_8888		= ContextWithFormatARGB_8888;
    m_ContextWithFormatXRGB_8888		= ContextWithFormatXRGB_8888;
    m_ContextWithFormatRGB_565			= ContextWithFormatRGB_565;
    m_ContextWithFormatA_8				= ContextWithFormatA_8;
    m_DudSurface						= DudSurface;

	//m_currentFunctionCall.AppendEGLContext( PoolContext );//for sharing
    //m_currentFunctionCall.AppendEGLContext( ContextWithFormatARGB_8888_PRE );//for sharing
    //m_currentFunctionCall.AppendEGLContext( ContextWithFormatARGB_8888 );
    //m_currentFunctionCall.AppendEGLContext( ContextWithFormatXRGB_8888 );
    //m_currentFunctionCall.AppendEGLContext( ContextWithFormatRGB_565 );
    //m_currentFunctionCall.AppendEGLContext( ContextWithFormatA_8 );
	//m_currentFunctionCall.AppendEGLSurface( DudSurface );

	TRACE("DriverAPIWrapper::CreatePoolContext() <-\n");
	m_initialized = true;
	return 1;
	}




/**
* This code is supposed to create a pbuffer to back an sgImage
* TODO: not tested at all
*/
int DriverAPIWrapper::CreatePbufferSg(  )
	{
	TRACE("DriverAPIWrapper::CreatePbufferSg ->\n");
	EGLint w;
	EGLint h;

	if( !m_initialized )
		{
		CreatePoolContext();
		}
	
	m_currentFunctionCall.GetEGLint( w, 0 );
	m_currentFunctionCall.GetEGLint( h, 1 );

	//eglCreatePbufferSurface
	EGLint pbuffer_surface_attribs[] =
	{
      EGL_WIDTH,    w,
	  EGL_HEIGHT,   h,
      EGL_NONE
	};
	
    EGLSurface surface = ::eglCreatePbufferSurface( m_Dpy, m_PoolConfig, pbuffer_surface_attribs );
	if( surface != EGL_NO_SURFACE )
	{
		m_SgImagePbuffers.push_back( surface );
	}
	TRACE("DriverAPIWrapper::CreatePbufferSg surface=%u \n", surface );
    m_currentFunctionCall.SetReturnValue( (TUint32)surface );
    TRACE("DriverAPIWrapper::CreatePbufferSg <-\n");
    return WriteReply();
	}


/**
* This code is supposed to create a pbuffer to back an sgImage
* TODO: not tested at all
*/
int DriverAPIWrapper::CreateVGImageSg(  )
	{
	TRACE("DriverAPIWrapper::CreateVGImageSg ->\n");
	EGLint w;
	EGLint h;
	EGLint pixelformat;
	VGImage img;

	if( !m_initialized )
	{
		CreatePoolContext();
	}
	
	EGLenum previousApi = ::eglQueryAPI();
	::eglBindAPI( EGL_OPENVG_API );

	m_currentFunctionCall.GetEGLint( w, 0 );
	m_currentFunctionCall.GetEGLint( h, 1 );
	m_currentFunctionCall.GetEGLint( pixelformat, 2 );

	::eglMakeCurrent( m_Dpy, m_DudSurface, m_DudSurface, m_PoolContext );

	//eglCreatePbufferSurface
	EGLint pbuffer_surface_attribs[] =
	{
      EGL_WIDTH,    w,
	  EGL_HEIGHT,   h,
      EGL_NONE
	};
	VGImageFormat format = getVGColorFormat( pixelformat );
	img = ::vgCreateImage(format, w, h, VG_IMAGE_QUALITY_NONANTIALIASED);//todo: quality; how to decide (TSgImageInfo only provides w,h, pixelformat)?
		if( img != VG_INVALID_HANDLE )
	{
		m_SgImageVGImages.push_back( img );
	}

    m_currentFunctionCall.SetReturnValue( (TUint32)img );
	::eglBindAPI( previousApi );
    TRACE("DriverAPIWrapper::CreateVGImageSg <-\n");
    return WriteReply();
	}

int DriverAPIWrapper::DeleteSgImage()
{
	EGLSurface surface;
	VGImage image;
	m_currentFunctionCall.GetEGLSurface( surface, 0 );
	m_currentFunctionCall.GetVGParamValue( image, 1 );

	eglDestroySurface( m_Dpy, surface );
	vgDestroyImage( image );

	return WriteReply();
}

void DriverAPIWrapper::SyncVGImageFromPBuffer( EGLSurface aPbuffer, VGImage aVGImage )
{
	TRACE("DriverAPIWrapper::SyncVGImageFromPBuffer ->\n");
	//Store the previous current
	EGLDisplay disp = ::eglGetCurrentDisplay();
	EGLSurface draw = ::eglGetCurrentSurface( EGL_DRAW );
	EGLSurface read = ::eglGetCurrentSurface( EGL_READ );
	EGLContext ctx  = ::eglGetCurrentContext();

	EGLint width(0);
	EGLint height(0);
	eglSurfaceAttrib( disp, aPbuffer, EGL_WIDTH, width );
	eglSurfaceAttrib( disp, aPbuffer, EGL_HEIGHT, height );

	::eglMakeCurrent( m_Dpy, aPbuffer, aPbuffer, m_PoolContext );
	::vgGetPixels( aVGImage, 0, 0, 0, 0, width, height );
	//Set the previous current
	::eglMakeCurrent( disp, draw, read, ctx );
	TRACE("DriverAPIWrapper::SyncVGImageFromPBuffer <-\n");
}


int DriverAPIWrapper::SyncPBufferFromVGImage( VGImage aSrc, VGint aWidth, VGint aHeight )
{
	TRACE("DriverAPIWrapper::SyncPBufferFromVGImage ->\n");
	if( ((VGboolean)vgGeti( VG_SCISSORING )) == VG_TRUE )
	{
		::vgSeti( VG_SCISSORING, VG_FALSE );
		//The surface is the same size, as the vgimage
		::vgSetPixels( 0, 0, aSrc, 0, 0, aWidth, aHeight );
		::vgSeti( VG_SCISSORING, VG_TRUE );
	}
	else
	{
		::vgSetPixels( 0, 0, aSrc, 0, 0, aWidth, aHeight );
	}
	TRACE("DriverAPIWrapper::SyncPBufferFromVGImage <-\n");
	return 0;
}


VGImageFormat DriverAPIWrapper::getVGColorFormat( int aPixelFormat )
{
	switch( aPixelFormat )
		{
		case  EUidPixelFormatXRGB_8888:
			{
			return VG_sXRGB_8888;
			}
		case EUidPixelFormatARGB_8888:
			{
			return VG_sARGB_8888;
			}
		case EUidPixelFormatARGB_8888_PRE:
			{
			return VG_sARGB_8888_PRE;
			}
		case EUidPixelFormatRGB_565:
			{
			return VG_sRGB_565;
			}
		case EUidPixelFormatA_8:
			{
			return VG_A_8;
			}
		default:
			{
			return VG_sARGB_8888;
			}
		}
}

EGLint* DriverAPIWrapper::getColorAttributes( int aPixelFormat, int& aRed, int& aGreen, int& aBlue, int& aAlpha )
	{
	switch( aPixelFormat )
		{
		case  EUidPixelFormatXRGB_8888:
			{
			aRed = aGreen = aBlue = aAlpha = 8;
			break;
			}
		case EUidPixelFormatARGB_8888:
			{
			aRed = aGreen = aBlue = aAlpha = 8;
			break;
			}
		case EUidPixelFormatARGB_8888_PRE:
			{
			aRed = aGreen = aBlue = aAlpha = 8;
			break;
			}
		case EUidPixelFormatRGB_565:
			{
			aRed = 5; aGreen = 6; aBlue = 5; aAlpha = 0;
			break;
			}
		case EUidPixelFormatA_8:
			{
			aRed = aGreen = aBlue = 0; aAlpha = 8;
			break;
			}
		default:
			{
			aRed = aGreen = aBlue = aAlpha = 0;
			break;
			}
		}
		static EGLint attribs[] =
		{
		  EGL_RED_SIZE,   aRed,
		  EGL_GREEN_SIZE, aGreen,
		  EGL_BLUE_SIZE,  aBlue,
		  EGL_ALPHA_SIZE, aAlpha,
	      EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
		  EGL_SURFACE_TYPE,   EGL_PBUFFER_BIT,
		  EGL_NONE
		};
	return attribs;
	}

