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

#ifndef _EGLINTERFACE_H_
#define _EGLINTERFACE_H_

#include "GLESInterface.h"
#include "glesInternal.h"
#include <set>

extern "C"
{
GLES_API_CALL IEGLtoGLESInterface* getGLESInterface(void);

void glesReleaseTexImage(void* surface, int name, int level);
}

class GLESContext;

class EGLtoGLESInterface : public IEGLtoGLESInterface
{
public:
    EGLtoGLESInterface();

    void SetEGLInterface(IGLEStoEGLInterface* egl);
	void* CreateContext(void* nativeContext);
	bool ReleaseContext(void* context);
	void* GetNativeContext(void* context);
	fpGLProc GetGLProcAddress( const char *procname );
    int BindTexImage( void* surface, int level, bool generateMipmap, const SurfaceDescriptor* desc, void* buffer );
	bool ReleaseTexImage( int name, int level );
    bool CopyBuffers( void* buf, const SurfaceDescriptor* desc );
    bool UpdateBuffers( void* buf, const SurfaceDescriptor* desc );
    void Flush();
    void Finish();

	// Methods for local OpenGL ES -implementation. To be called only from OpenGL ES internals.
    static IGLEStoEGLInterface* GetEGLInterface();

private:
    IGLEStoEGLInterface* m_egl;
	std::set<GLESContext*> m_contexts;
};

#endif // _EGLINTERFACE_H_