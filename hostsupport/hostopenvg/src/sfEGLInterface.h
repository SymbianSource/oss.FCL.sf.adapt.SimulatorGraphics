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
 */

#ifndef _SFEGLINTERFACE_H_
#define _SFEGLINTERFACE_H_

#include "riImage.h"
#include "VGInterface.h"


// Get pointer to global interface object.
extern "C" { RI_API_CALL IEGLtoVGInterface* getVGInterface(void); }
extern "C" { RI_API_CALL VGErrorCode vgSimulatorGetError(void); }

namespace OpenVGRI 
{
    void* eglvgGetCurrentVGContext(void);
    bool  eglvgIsInUse(void* image);
	bool  eglvgLockSurface(bool read, bool write);
    bool  eglvgUnlockSurface();
	void  OSAcquireMutex(void);
	void  OSReleaseMutex(void);
    void  eglvgGetImageDescriptor( void* image, Color::Descriptor &desc, int &width, int &height, int &stride );
    void* eglvgGetImageData( void* image );
}

// Services from OpenVG to EGL
class EGLtoVGInterface : public IEGLtoVGInterface
{
public:
    EGLtoVGInterface();
    virtual ~EGLtoVGInterface();

    void    SetEGLInterface( IVGtoEGLInterface* egl );
    void*   CreateContext( void* shareContext );
    bool    ReleaseContext( void* context );
    bool    CreateSurface( const SurfaceDescriptor* desc, BufferContainer* buffers, void* image );
    bool    ReleaseSurface(void* surface);
    bool    SetCurrentSurface( void* context, void* surface );
    bool    ResizeSurface( void* context, void* surface, int width, int height, BufferContainer* buffers );
    bool    IsValidImage( void* image, CColorDescriptor* colorDesc, int* width, int* height );
    bool    IsImageInUse( void* image );
    void*   CreateImage();
    bool    ReleaseImage();
    void    Flush();
    void    Finish();
    fpVGProc GetVGProcAddress( const char *procname );
    void    CopyBuffers( void* buffer, int stride, void* surface );
    void    UpdateBuffers( void* buffer, int stride, const SurfaceDescriptor* desc );
    bool    IsRootImage( void* buffer );
    void    GetImageData( void* image, SurfaceDescriptor& desc, void* data );
    void    AddRef( void* image );
    void    RemoveRef( void* image );

    // Methods for local OpenVG-implementation. To be called only from OpenVG internals.
    static IVGtoEGLInterface* GetEGLInterface();
    static OpenVGRI::Color::Descriptor vgDescriptorFromSurfaceDescriptor(const SurfaceDescriptor* sdesc); // \todo static
    
private:    
    int     findContext(OpenVGRI::VGContext* contextPtr);
    bool    isValidImage(void* image);

private:
    OpenVGRI::Array<OpenVGRI::VGContext*>   m_contexts;
    IVGtoEGLInterface*                      m_egl;

};

#endif

