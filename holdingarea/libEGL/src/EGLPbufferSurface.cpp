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

#include "EGLPbufferSurface.h"
#include "EGLOs.h"

CEGLPbufferSurface::CEGLPbufferSurface(void) :
    CEGLSurface( CEGLSurface::PBUFFER_SURFACE,
                 0,
                 0,
                 0,
                 NULL ),
    m_clientBuffer( NULL ),
    m_container( NULL )
    {
    }

CEGLPbufferSurface::CEGLPbufferSurface( EGLint colorSpace, EGLint alphaFormat, EGLint renderBuffer, CEGLConfig* config, EGLint largestPbuffer, EGLint textureFormat, EGLint textureTarget, EGLint mipmapTexture, EGLClientBuffer clientBuf ) :
    CEGLSurface( CEGLSurface::PBUFFER_SURFACE,
                 colorSpace,
                 alphaFormat,
                 renderBuffer,
                 config ),
    m_largestPbuffer( largestPbuffer ),
    m_textureFormat( textureFormat ),
	m_textureTarget( textureTarget ),
	m_mipmapTexture( mipmapTexture ),
    m_clientBuffer( clientBuf ),
    m_container( NULL )
    {

    }

CEGLPbufferSurface::~CEGLPbufferSurface(void)
    {
    if( m_container )
        {
        // \todo errors
        CEGLOs::DestroyNativePbuffer( m_container );
        delete m_container;
        m_container = NULL;
        }
    }

void CEGLPbufferSurface::SetNativePbufferContainer( EGLINativePbufferContainer* container )
    {
    if( m_container )
        {
        // \todo errors
        CEGLOs::DestroyNativePbuffer( m_container );
        delete m_container;
        m_container = NULL;
        }
    m_container = container;
    }

bool CEGLPbufferSurface::BindCopyContext()
    {
    m_tmpContext = CEGLOs::CurrentNativeContext();
    // \todo read/draw?
    m_tmpDisplay = CEGLOs::CurrentNativeSurface();

    if( !m_container )
        {
        return false;
        }
    
    return CEGLOs::MakeNativeContextCurrent( NativeGLFunctions(), NativeDisplay(), NativeDisplay(), CopyContext() );
    }

bool CEGLPbufferSurface::ReleaseCopyContext()
    {
    // \todo read/draw?
    return CEGLOs::MakeNativeContextCurrent( NativeGLFunctions(), m_tmpDisplay, m_tmpDisplay, m_tmpContext );
    }
