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

#include "EGLImage.h"
#include "EGLUtils.h"
#include "EGLState.h"
#include <EGL/eglext.h>
#include <VG/openvg.h>

#include <stdlib.h>

CEGLImage::CEGLImage( EGLenum target, EGLClientBuffer buffer, 
                      SurfaceDescriptor desc, void* data) :
    m_buffer( buffer ),
    m_target( target ),
	m_data(data)
    {
    m_siblings = EGLI_NEW EGLImageSibling;
    m_siblings->buffer = buffer;
    m_siblings->target = target;
    m_siblings->next   = NULL;
    m_SurfDesc.m_alphaShift     = desc.m_alphaShift;
    m_SurfDesc.m_blueShift      = desc.m_blueShift;
    m_SurfDesc.m_depthSize      = desc.m_depthSize;
    m_SurfDesc.m_greenShift     = desc.m_greenShift;
    m_SurfDesc.m_height         = desc.m_height;
    m_SurfDesc.m_luminanceShift = desc.m_luminanceShift;
    m_SurfDesc.m_maskSize       = desc.m_maskSize;
    m_SurfDesc.m_redShift       = desc.m_redShift;
    m_SurfDesc.m_stencilSize    = desc.m_stencilSize;
    m_SurfDesc.m_stride         = desc.m_stride;
    m_SurfDesc.m_width          = desc.m_width;

    m_SurfDesc.m_colorDescriptor.m_alphaMaskSize = desc.m_colorDescriptor.m_alphaMaskSize;
    m_SurfDesc.m_colorDescriptor.m_alphaSize     = desc.m_colorDescriptor.m_alphaSize;
    m_SurfDesc.m_colorDescriptor.m_blueSize      = desc.m_colorDescriptor.m_blueSize;
    m_SurfDesc.m_colorDescriptor.m_bpp           = desc.m_colorDescriptor.m_bpp;
    m_SurfDesc.m_colorDescriptor.m_format        = desc.m_colorDescriptor.m_format;
    m_SurfDesc.m_colorDescriptor.m_greenSize     = desc.m_colorDescriptor.m_greenSize;
    m_SurfDesc.m_colorDescriptor.m_luminanceSize = desc.m_colorDescriptor.m_luminanceSize;
    m_SurfDesc.m_colorDescriptor.m_redSize       = desc.m_colorDescriptor.m_redSize;
    }

CEGLImage::~CEGLImage(void)
    {
    // tero 2010-08-02, changed to free, since delete is _certainly_ incorrect.
    if( m_data )
    free(m_data);
    m_data = NULL;
    DestroyAllSiblings();
    }

void CEGLImage::AddSibling( EGLenum target, EGLClientBuffer buffer )
    {
    EGLImageSibling* newp;
    EGLImageSibling* curr = m_siblings;
    newp = EGLI_NEW EGLImageSibling;
    newp->buffer = buffer;
    newp->target = target;
    newp->next   = NULL;

    while( curr->next )
        {
        curr = curr->next;
        }      
    curr->next = newp;
    }

void CEGLImage::UpdateData( CEGLState *state, void* data )
    {
    EGLImageSibling* curr = m_siblings;
    while( curr )
        {
        switch ( curr->target )
            {
            case EGL_VG_PARENT_IMAGE_KHR:
                {
                // OpenVG

                }
                break;
            case EGL_GL_TEXTURE_2D_KHR:
		    case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X_KHR:
            case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X_KHR:
            case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y_KHR:
            case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_KHR:
            case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z_KHR:
            case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_KHR:
                {
                // GLES
                IEGLtoGLES2Interface* iFace = (IEGLtoGLES2Interface*)state->GLESInterface(2);
                //iFace->UpdateBuffers(
                }
                break;
            default:
                break;
            }
        curr = curr->next;
        }
    }

void CEGLImage::DestroyAllSiblings()
    {
    EGLImageSibling* curr = m_siblings;
    EGLImageSibling* next = curr->next;

    while( curr )
        {
        next = curr->next;
        free( curr );
        curr = next;
        }
    }
