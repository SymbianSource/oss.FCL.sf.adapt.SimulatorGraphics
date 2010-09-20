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

#ifndef _GLESINTERFACE_H_
#define _GLESINTERFACE_H_

#include "ImageTarget.h"

struct SurfaceDescriptor;

typedef void (*fpGLProc)(void);

class IGLEStoEGLInterface
    {
public:
    virtual void* GetGLESContext() = 0;
    virtual bool IsImageInUse( void* image ) = 0;
    virtual bool LockGLESSurface( bool read, bool draw ) = 0;
    virtual bool UnlockGLESSurface() = 0;
	virtual void* GetHostProcAddress( const char* proc ) = 0;
	virtual void GetDescForImage( void* image, SurfaceDescriptor& ImageDesc ) = 0;
    virtual void* GetDataForImage( void* image ) = 0;
	virtual void RegisterImageTarget( void* image, EImageTarget target, void* buffer ) = 0;
	virtual void UnregisterImageTarget( void* image, EImageTarget target, void* buffer ) = 0;
	virtual void UpdateImageSiblings( void* image, EImageTarget target, void* buffer ) = 0;
	virtual void ReleaseTexImage(void* surface, int name, int level) = 0;
    };

// GLES services for EGL 
class IEGLtoGLESInterface
    {
public:
    virtual void SetEGLInterface( IGLEStoEGLInterface* ) = 0;
	virtual void* CreateContext( void* nativeContext ) = 0;
	virtual bool ReleaseContext( void* context ) = 0;
	virtual void* GetNativeContext( void* context ) = 0;
	virtual fpGLProc GetGLProcAddress( const char *procname ) = 0;
    virtual int BindTexImage( void* surface, int level, bool generateMipmap, const SurfaceDescriptor* desc, void* buffer ) = 0;
	virtual bool ReleaseTexImage( int name, int level ) = 0;
    virtual bool CopyBuffers( void* buf, const SurfaceDescriptor* desc ) = 0;
    virtual bool UpdateBuffers( void* buf, const SurfaceDescriptor* desc ) = 0;
    virtual void Flush() = 0;
    virtual void Finish() = 0;
    };

typedef IEGLtoGLESInterface* (*fpGetGLES1Interface)(void);
IEGLtoGLESInterface* LoadGLES1Interface( void*& libraryHandle );

#endif // _GLESINTERFACE_H_
