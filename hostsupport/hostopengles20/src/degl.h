/* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *
 */

#ifndef DEGL_H_
#define DEGL_H_

#include "common.h"

#ifdef __cplusplus

#include "GLES2Interface.h"

#include <set>

extern "C"
{
#endif /* __cplusplus */

struct DGLContext;

void* deglGetHostProcAddress(char const* proc);
struct DGLContext* deglGetCurrentContext(void);
int deglLockSurface(int read, int draw);
int deglUnlockSurface(void);
void deglGetImageInfo(GLeglImageOES image, GLenum* internalformat, GLsizei* width, GLsizei* height, GLsizei* stride, GLenum* dataformat, GLenum* datatype);
void* deglGetImageData(GLeglImageOES image);
void deglRegisterImageTarget(GLeglImageOES image, GLenum target, GLuint name);
void deglUnregisterImageTarget(GLeglImageOES image, GLenum target, GLuint name);
void deglUpdateImageSiblings(GLeglImageOES image, GLenum target, GLuint name);
void deglReleaseTexImage(void* surface, int name, int level);

#ifdef __cplusplus
DGLES2_EXPORT IEGLtoGLES2Interface* getGLES2Interface(void);
}

class EGLtoGLES2Interface : public IEGLtoGLES2Interface
{
public:
    EGLtoGLES2Interface();
    virtual ~EGLtoGLES2Interface() {}

	void SetEGLInterface( IGLEStoEGLInterface* );
	void* CreateContext( void* nativeContext );
	bool ReleaseContext( void* context );
	void* GetNativeContext( void* context );
	fpGLProc GetGLProcAddress( const char *procname );
	int BindTexImage( void* surface, int level, bool generateMipmap, const SurfaceDescriptor* desc, void* buffer );
	bool ReleaseTexImage( int name, int level );
    EStatus GetTextureInfo( void* context, EImageTarget target, void* texture, GLint textureLevel, SurfaceDescriptor& desc );
    EStatus GetTextureData( void* context, EImageTarget target, void* texture, GLint textureLevel, void* data );
    bool CopyBuffers( void* buf, const SurfaceDescriptor* desc );
    bool UpdateBuffers( void* buf, const SurfaceDescriptor* desc );
	bool UpdateImageSibling( EImageTarget target, void* name );
    void Flush();
    void Finish();

	// Methods for local OpenGL ES -implementation. To be called only from OpenGL ES internals.
    static IGLEStoEGLInterface* GetEGLInterface();

private:
    IGLEStoEGLInterface* m_egl;
	std::set<DGLContext*> m_contexts;
};

#endif /* __cplusplus */

#endif /* DEGL_H */
