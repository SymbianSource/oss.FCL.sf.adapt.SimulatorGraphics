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

#ifndef _EGLCONFIG_H_
#define _EGLCONFIG_H_

#include <EGL/egl.h>
#include "ColorDescriptor.h"
#include "TransparentColor.h"

class CEGLConfig
{
public:
	CEGLConfig(void);
    CEGLConfig( CColorDescriptor&   colorDescriptor,
                EGLBoolean          bindToTexRGB,           /* EGL_BIND_TO_TEXTURE_RGB*/
                EGLBoolean          bindToTexRGBA,          /* EGL_BIND_TO_TEXTURE_RGBA */
                EGLenum             configCaveat,           /* EGL_CONFIG_CAVEAT */
                EGLint              configId,               /* EGL_CONFIG_ID */
                EGLint              conformant,             /* EGL_CONFORMANT */
                EGLint              depthSize,              /* EGL_DEPTH_SIZE */
                EGLint              level,                  /* EGL_LEVEL */
                EGLint              maxSwapInterval,        /* EGL_MAX_SWAP_INTERVAL */
                EGLint              minSwapInterval,        /* EGL_MIN_SWAP_INTERVAL */
                EGLBoolean          nativeRenderable,       /* EGL_NATIVE_RENDERABLE */
                EGLint              nativeVisualType,       /* EGL_NATIVE_VIDUAL_TYPE */
                EGLint              renderableType,         /* EGL_RENDERABLE_TYPE */
                EGLint              sampleBuffers,          /* EGL_SAMPLE_BUFFERS */
                EGLint              samples,                /* EGL_SAMPLES */
                EGLint              stencilSize,            /* EGL_STENCIL_SIZE */
                EGLint              surfaceType,
                CTransparentColor*  transparentColor );

	~CEGLConfig(void);

    /*
    Setting up config values. 
    Return EGL_NO_ERROR if all ok and
    EGL_BAD_PARAMETER if some conflict with parameters
    */
	EGLint SetUpConfig( EGLint red, EGLint green, EGLint blue, 
                        EGLint luminance, EGLint alpha, 
                        EGLint alphaMask, EGLint samples, EGLint bpp,
                        EGLint id );
                    
public:
    inline void SetId( int id ) { m_configId = id; }
    inline EGLint Id() const { return m_configId; }
    EGLBoolean SetAttribute( EGLint attribute, EGLint value );
    EGLint GetAttribute( EGLint attribute ) const;
    inline CColorDescriptor* GetDescriptor() { return &m_colorDescriptor; }

    bool operator>( const CEGLConfig& rhs ) const;
    bool operator<( const CEGLConfig& rhs ) const;
    bool Match( const CEGLConfig& filter) const;
    CEGLConfig& operator=( const CEGLConfig& rhs );
    bool IsCompatible( const CEGLConfig& config, const EGLenum contextApi ) const;

private:
	EGLint              m_bufferSize;             /* EGL_BUFFER_SIZE */
    CColorDescriptor    m_colorDescriptor;       
    EGLBoolean          m_bindToTexRGB;           /* EGL_BIND_TO_TEXTURE_RGB*/
    EGLBoolean          m_bindToTexRGBA;          /* EGL_BIND_TO_TEXTURE_RGBA */
    EGLenum             m_configCaveat;           /* EGL_CONFIG_CAVEAT */
    EGLint              m_configId;               /* EGL_CONFIG_ID */
    EGLint              m_conformant;             /* EGL_CONFORMANT */
    EGLint              m_depthSize;              /* EGL_DEPTH_SIZE */
    EGLint              m_level;                  /* EGL_LEVEL */
    EGLint              m_maxPbufferWidth;        /* EGL_MAX_PBUFFER_WIDTH */
    EGLint              m_maxPbufferHeight;       /* EGL_MAX_PBUFFER_HEIGHT */
    EGLint              m_maxPbufferPixels;       /* EGL_MAX_PBUFFER_PIXELS */
    EGLint              m_maxSwapInterval;        /* EGL_MAX_SWAP_INTERVAL */
    EGLint              m_minSwapInterval;        /* EGL_MIN_SWAP_INTERVAL */
    EGLBoolean          m_nativeRenderable;       /* EGL_NATIVE_RENDERABLE */
    EGLint              m_nativeVisualId;         /* EGL_NATIVE_VISUAL_ID */
    EGLint              m_nativeVisualType;       /* EGL_NATIVE_VISUAL_TYPE */
    EGLint              m_renderableType;         /* EGL_RENDERABLE_TYPE */
    EGLint              m_sampleBuffers;          /* EGL_SAMPLE_BUFFERS */
    EGLint              m_samples;                /* EGL_SAMPLES */
    EGLint              m_stencilSize;            /* EGL_STENCIL_SIZE */

    EGLint              m_surfaceType;            /* EGL_SURFACE_TYPE = 
                                                     EGL_PBUFFER_BIT, 
                                                     EGL_PIXMAP_BIT, 
                                                     EGL_WINDOW_BIT, 
                                                     EGL_VG_COLORSPACE_LINEAR_BIT, 
                                                     EGL_VG_ALPHA_FORMAT_PRE_BIT,
                                                     EGL_MULTISAMPLE_RESOLVE_BOX_BIT,
                                                     EGL_SWAP_BEHAVIOR_PRESERVED_BIT */

    CTransparentColor  m_transparentColor;
};
#endif //_EGLCONFIG_H_