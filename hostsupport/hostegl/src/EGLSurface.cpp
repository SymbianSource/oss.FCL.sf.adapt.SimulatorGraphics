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

#include "EGLSurface.h"
#include "SurfaceDescriptor.h"
#include "EGLConfig.h"
#include "EGLContext.h"
#include "EGLOs.h"

#include <string.h>
// Required for free. \todo Shouldn't this use C++ new[]/delete[] instead?
#include <stdlib.h>

CEGLSurface::CEGLSurface( CEGLSurface::SurfaceType type, EGLint colorSpace, EGLint alphaFormat, EGLint renderBuffer, CEGLConfig* config ) :
    m_type( type ),
    m_colorSpace( colorSpace ),
    m_alphaFormat( alphaFormat ),
    m_renderBuffer( renderBuffer ),
    m_config( config ),
    m_boundContext( NULL ),
    m_thread( NULL ),
    m_mipmapLevel( 0 ),
    m_multisampleResolve( EGL_MULTISAMPLE_RESOLVE_DEFAULT ),
    m_swapBehavior( /*EGL_BUFFER_DESTROYED*/EGL_BUFFER_PRESERVED ),
    m_vgDirty(false),
    m_gles1Dirty(false),
    m_gles2Dirty(false),
    m_isLocked(false)
    {
    EGLI_ASSERT( m_config != NULL );
	memset( &m_textureBinding, 0, sizeof(struct EGLITextureBinding) );
    memset( &m_vgBuffers, 0, sizeof(struct BufferContainer) );
    memset( &m_glesBuffers, 0, sizeof(struct BufferContainer) );
    CEGLOs::InitializeLock( &m_lock );
    }

CEGLSurface::~CEGLSurface(void)
    {
    if( m_glesBuffers.m_colorBuffer )
        free (m_glesBuffers.m_colorBuffer );
    CEGLOs::DestroyLock( &m_lock );
    }

void CEGLSurface::BindToContext( CEGLContext* context ) 
    { 
    m_boundContext = context;
    }

bool CEGLSurface::Resize( EGLint width, EGLint height )
    {
    if( width == m_width && height == m_height && m_glesBuffers.m_colorBuffer )
        {
        // Size didn't change and we have gles color buffer already
        return true;
        }
        
    if( width != m_width )
        {
        SetStride( (width * m_config->GetDescriptor()->Bpp() + 7)/8 );
        }
    m_width = width;
    m_height = height;
    if( m_glesBuffers.m_colorBuffer )
        free( m_glesBuffers.m_colorBuffer );
    m_glesBuffers.m_colorBuffer = malloc( height * m_stride );
    if( !(m_glesBuffers.m_colorBuffer) ) return false;
    return true;
    }

SurfaceDescriptor* CEGLSurface::Descriptor()
    {
    CColorDescriptor* colorDesc = m_config->GetDescriptor();
    EGLI_ASSERT( colorDesc != NULL );
    m_surfaceDescriptor.m_width = Width();
    m_surfaceDescriptor.m_height = Height();
    m_surfaceDescriptor.m_stride = Stride();
    m_surfaceDescriptor.m_colorDescriptor = *colorDesc;
    m_surfaceDescriptor.m_colorDescriptor.SetSurfaceType(m_colorSpace, m_alphaFormat);

    EGLint alphaSize = colorDesc->AlphaSize();
    EGLint blueSize = colorDesc->BlueSize();
    EGLint greenSize = colorDesc->GreenSize();
    EGLint redSize = colorDesc->RedSize();

    if( colorDesc->isLuminance() )
        {
        // \todo LA AL?
        m_surfaceDescriptor.m_redShift = 0;
        m_surfaceDescriptor.m_greenShift = 0;
        m_surfaceDescriptor.m_blueShift = 0;
        m_surfaceDescriptor.m_alphaShift = 0;
        m_surfaceDescriptor.m_luminanceShift = alphaSize;
        }
    else
        {
        m_surfaceDescriptor.m_luminanceShift = 0;
        /*  Found these from CGuestEGL::egliGetSymbianPixelFormat()
            in guestegl/src/guestegl.cpp. Pixel format
            should be transfered through serialization.

            EUidPixelFormatARGB_8888
            EUidPixelFormatARGB_8888_PRE
            EUidPixelFormatXRGB_8888

            EUidPixelFormatARGB_4444
            EUidPixelFormatXRGB_4444

            EUidPixelFormatRGB_565
            EUidPixelFormatARGB_1555
        */
        // \todo We need pixel format through serialization.
#if defined(EGLI_USE_SIMULATOR_EXTENSIONS)
        m_surfaceDescriptor.m_blueShift = 0;
        m_surfaceDescriptor.m_greenShift = blueSize;
        m_surfaceDescriptor.m_redShift = blueSize + greenSize;
        m_surfaceDescriptor.m_alphaShift = blueSize + greenSize + redSize;
#else
        // \note TeroP, changing these to match OpenVG color formats. This should make blitting
        // to _win32_ window fast, but a solution for Symbian vs. Win32 native build should be
        // defined.
        // That is: This has to match the EGLtoVGInterface::CopyBuffers VGImageFormat somehow.
        // Currently: VG_sARGB_8888_PRE;

        m_surfaceDescriptor.m_blueShift = 0;
        m_surfaceDescriptor.m_greenShift = blueSize;
        m_surfaceDescriptor.m_redShift = blueSize + greenSize;
        m_surfaceDescriptor.m_alphaShift = redSize + greenSize + blueSize;
#endif //EGLI_USE_SIMULATOR_EXTENSIONS
        }
    
    m_surfaceDescriptor.m_maskSize = m_surfaceDescriptor.m_colorDescriptor.AlphaMaskSize();
    m_surfaceDescriptor.m_stencilSize = m_config->GetAttribute( EGL_STENCIL_SIZE );
    m_surfaceDescriptor.m_depthSize = m_config->GetAttribute( EGL_DEPTH_SIZE );
    return &m_surfaceDescriptor;
    }

void CEGLSurface::SetAttribute( EGLint attrib, EGLint value )
    {
    switch( attrib )
        {
        case EGL_MIPMAP_LEVEL:
            {
            m_mipmapLevel = value;
            break;
            }
        case EGL_MULTISAMPLE_RESOLVE:
            {
            m_multisampleResolve = value;
            break;
            }
        case EGL_SWAP_BEHAVIOR:
            {
            m_swapBehavior = value;
            break;
            }
        }
    }

EGLint CEGLSurface::Attribute( EGLint attrib ) const
    {
    EGLint ret = EGL_BAD_ATTRIBUTE;
    switch( attrib )
        {
        case EGL_MIPMAP_LEVEL:
            {
            ret = m_mipmapLevel;
            break;
            }
        case EGL_MULTISAMPLE_RESOLVE:
            {
            ret = m_multisampleResolve;
            break;
            }
        case EGL_SWAP_BEHAVIOR:
            {
            ret = m_swapBehavior;
            break;
            }
        }
    return ret;
    }

void CEGLSurface::Lock()
    {
    CEGLOs::GetLock( &m_lock );
    m_isLocked = true;
    }
void CEGLSurface::Unlock()
    {
    CEGLOs::ReleaseLock( &m_lock );
    m_isLocked = false;
    }

