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

#include "EGLThread.h"
#include "EGLContext.h"
#include "EGLSurface.h"

CEGLThread::CEGLThread( EGLI_THREAD_ID threadId, EGLint supportedApis ) :
    m_id( threadId ),
    m_currentVGContext( NULL ),
    m_currentVGSurface( NULL ),
    m_currentGLESContext( NULL ),
	m_currentGLESReadSurface( NULL ),
    m_currentGLESDrawSurface( NULL ),
    m_currentApi( EGL_OPENGL_ES_API ),
    m_lastError( EGL_SUCCESS ),
    m_supportedApis( supportedApis )
    {
    }

CEGLThread::~CEGLThread(void)
    {
    }

bool CEGLThread::IsSupportedApi( EGLenum api ) const
    {
    bool ret = false;
    switch( api )
        {
        case EGL_OPENVG_API:
            {
            if( m_supportedApis & EGL_OPENVG_BIT )
                ret = true;
            break;
            }
        case EGL_OPENGL_ES_API:
            {
            if( m_supportedApis & EGL_OPENGL_ES_BIT ||
                m_supportedApis & EGL_OPENGL_ES2_BIT )
                ret = true;
            break;
            }
        }
    return ret;
    }

CEGLContext* CEGLThread::SwitchVGContext( CEGLContext* vgContext )
    {
    CEGLContext* ret = m_currentVGContext;
    if( vgContext != m_currentVGContext )
        {
        m_currentVGContext = vgContext;
        if( vgContext ) vgContext->SetThread( this );
        if( ret ) ret->SetThread( NULL );
        }
    return ret;
    }

CEGLSurface* CEGLThread::SwitchVGSurface( CEGLSurface* vgSurface )
    {
    CEGLSurface* ret = m_currentVGSurface;
    if( vgSurface != m_currentVGSurface )
        {
        m_currentVGSurface = vgSurface;
        if( vgSurface ) vgSurface->SetThread( this );
        if( ret ) ret->SetThread( NULL );
        }
    return ret;
    }

CEGLContext* CEGLThread::SwitchGLESContext( CEGLContext* glesContext )
    {
    CEGLContext* ret = m_currentGLESContext;
    if( glesContext != m_currentGLESContext )
        {
        m_currentGLESContext = glesContext;
        if( glesContext ) glesContext->SetThread( this );
        if( ret ) ret->SetThread( NULL );
        }
    return ret;
    }

void CEGLThread::SwitchGLESSurfaces( CEGLSurface* read, CEGLSurface* draw, CEGLSurface*& previousRead, CEGLSurface*& previousDraw )
    {
    previousRead = m_currentGLESReadSurface;
    previousDraw = m_currentGLESDrawSurface;
    m_currentGLESReadSurface = read;
    if( read ) read->SetThread( this );
    if( previousRead && previousRead != read ) previousRead->SetThread( NULL );
    m_currentGLESDrawSurface = draw;
    if( draw ) draw->SetThread( this );
    if( previousDraw && previousDraw != draw ) previousDraw->SetThread( NULL );
    }

void CEGLThread::CurrentGLESSurfaces( CEGLSurface** read, CEGLSurface** draw ) const
    {
    if( read ) *read = m_currentGLESReadSurface;
    if( draw ) *draw = m_currentGLESDrawSurface;
    }
