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

#ifndef _EGLSURFACE_H_
#define _EGLSURFACE_H_

#include <EGL/egl.h>
#include "eglInternal.h"
#include "EGLStructs.h"
#include "BufferContainer.h"
#include "SurfaceDescriptor.h"

class CEGLContext;
class CEGLConfig;
class CEGLThread;

class CEGLSurface : public RefCountingObject
{
public:
    enum SurfaceType
        {
        WINDOW_SURFACE = 1,
        PBUFFER_SURFACE,
        PIXMAP_SURFACE
        };

protected:
    CEGLSurface( CEGLSurface::SurfaceType type, EGLint colorSpace, EGLint alphaFormat, EGLint renderBuffer, CEGLConfig* config );

public:
	virtual ~CEGLSurface(void);

public:
    void BindToContext( CEGLContext* context );
    inline CEGLContext* BoundContext() const { return m_boundContext; }
    inline void SetThread( CEGLThread* thread ) { m_thread = thread; }
    inline CEGLThread* Thread() const { return m_thread; }
    inline CEGLConfig* Config() const { return m_config; }
    inline CEGLSurface::SurfaceType Type() const { return m_type; }
    inline EGLint RenderBuffer() const { return m_renderBuffer; }
    inline EGLint AlphaFormat() const { return m_alphaFormat; }
    inline EGLint ColorSpace() const { return m_colorSpace; }
    inline EGLint MipmapLevel() const { return m_mipmapLevel; }
    inline EGLint MultisampleResolve() const { return m_multisampleResolve; };
    inline EGLint SwapBehavior() const { return m_swapBehavior; }
	inline EGLITextureBinding& TextureBinding() { return m_textureBinding; }
    
    EGLint Width() const { return m_width; }
    EGLint Height() const { return m_height; }
    void SetStride( EGLint stride ) { m_stride = stride; }
    EGLint Stride() const { return m_stride; }
    bool Resize( EGLint width, EGLint height );
    void Size( int& width, int& height ) { width = m_width; height = m_height; }

    BufferContainer* VGBuffers() { return &m_vgBuffers; }
    BufferContainer* GLESBuffers() { return &m_glesBuffers; }
    void* VGColorBuffer() const { return m_vgBuffers.m_colorBuffer; }
    void* GLESColorBuffer() const { return m_glesBuffers.m_colorBuffer; }
    void* VGClientSurface() const { return m_vgBuffers.m_clientSurface; }
    void* GLESClientSurface() const { return m_glesBuffers.m_clientSurface; }

    SurfaceDescriptor* Descriptor();

    void SetAttribute( EGLint attrib, EGLint value );
    EGLint Attribute( EGLint attrib ) const;

    void Lock();
    void Unlock();
    bool IsLocked() const { return m_isLocked; }
    void SetVGDirty( bool dirty ) { m_vgDirty = dirty; }
    bool IsVGDirty() const { return m_vgDirty; }
    void SetGLES1Dirty( bool dirty ) { m_gles1Dirty = dirty; }
    bool IsGLES1Dirty() const { return m_gles1Dirty; }
    void SetGLES2Dirty( bool dirty ) { m_gles2Dirty = dirty; }
    bool IsGLES2Dirty() const { return m_gles2Dirty; }

private:
	SurfaceType     m_type;
    EGLint          m_colorSpace;
    EGLint          m_alphaFormat;
    EGLint          m_renderBuffer;
    CEGLConfig*     m_config; // not owned
    CEGLContext*    m_boundContext; // not owned
    CEGLThread*     m_thread; // not owned

protected:
    EGLint              m_width;
    EGLint              m_height;
    EGLint              m_stride;
    BufferContainer     m_vgBuffers;
    BufferContainer     m_glesBuffers;
    SurfaceDescriptor   m_surfaceDescriptor;
    EGLint              m_mipmapLevel;
    EGLint              m_multisampleResolve;
    EGLint              m_swapBehavior;
	EGLITextureBinding	m_textureBinding;
    EGLI_LOCK           m_lock;
    bool                m_vgDirty;
    bool                m_gles1Dirty;
    bool                m_gles2Dirty;
    bool                m_isLocked;
};

#endif //_EGLSURFACE_H_
