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

#include "EGLWindowSurface.h"
#include "EGLConfig.h"
#include "ColorDescriptor.h"
#include "EGLOs.h"

// \todo Use new[]/free[] instead?
#include <stdlib.h>

CEGLWindowSurface::CEGLWindowSurface() :
    CEGLSurface( CEGLSurface::WINDOW_SURFACE,
                 0,
                 0,
                 0,
                 NULL ),
    m_buffer0( NULL ),
    m_buffer1( NULL ),
    m_currentBuffer( NULL ),
    m_swapInterval( 1 ),
    m_osContext( NULL )
    {
    }

CEGLWindowSurface::CEGLWindowSurface( CEGLConfig* config, EGLint colorSpace, EGLint alphaFormat, EGLint renderBuffer, EGLIOsWindowContext* osContext) :
    CEGLSurface( CEGLSurface::WINDOW_SURFACE,
                 colorSpace,
                 alphaFormat,
                 renderBuffer,
                 config ),
    m_buffer0( NULL ),
    m_buffer1( NULL ),
    m_currentBuffer( NULL ),
    m_swapInterval( 1 ),
    m_osContext( osContext )
    {
    EGLI_ASSERT( m_osContext != NULL );
    }

CEGLWindowSurface::~CEGLWindowSurface(void)
    {
#if !defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    if( m_currentBuffer ) free( m_currentBuffer );
#endif
    if( m_osContext )
        CEGLOs::DestroyOSWindowContext( m_osContext );
    }

void* CEGLWindowSurface::CurrentBuffer()
    {
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
    m_currentBuffer = (m_currentBuffer && m_currentBuffer == m_buffer0) ? m_buffer1 : m_buffer0;
    EGLI_ASSERT( m_currentBuffer != NULL );
    return m_currentBuffer;
#else
    if( !m_currentBuffer || m_osContext->width != Width() || m_osContext->height != Height() )
        {
        if( m_currentBuffer ) free( m_currentBuffer );
        m_currentBuffer = malloc(m_height*m_stride);
        }
    return m_currentBuffer;//NULL;
#endif
    }

void CEGLWindowSurface::SetSwapInterval( EGLint interval )
    {
    // \todo MIN, MAX
    m_swapInterval = interval;
    }

void CEGLWindowSurface::SetOsContext( EGLIOsWindowContext* osContext ) 
    {
    if( m_osContext )
        CEGLOs::DestroyOSWindowContext( m_osContext );
    m_osContext = osContext;
    }
