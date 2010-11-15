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

#ifndef TEXTURE_H_
#define TEXTURE_H_

typedef enum DGLTextureType
{
	DGLES2_TEXTURE_2D,
	DGLES2_TEXTURE_CUBE_MAP
} DGLTextureType;

typedef struct DGLTextureLevel
{
	GLboolean specified;
	GLenum format;
	GLsizei width;
	GLsizei height;
	void* bound_surface; // The EGLSurface bound to this image with eglBindTexImage.
} DGLTextureLevel;

typedef struct DGLTexture
{
	struct DGLObject obj;

	DGLTextureType type;

	// One for each face in case of a cube map texture.
	int num_levels[6];
	DGLTextureLevel* levels[6];
	GLeglImageOES egl_image[6]; // EGLimage this texture is a sibling of.
} DGLTexture;

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

DGLTexture*			DGLTexture_create(GLuint name, DGLTextureType type, GLint num_levels);
void				DGLTexture_destroy(DGLTexture* texture);
GLboolean			DGLTexture_isComplete(const DGLTexture* texture);
GLboolean			DGLTexture_hasLevelZero(const DGLTexture* texture);
GLboolean			DGLTexture_hasLevelsOtherThanZero(const DGLTexture* texture);
DGLTextureLevel*	DGLTexture_getLevel(DGLTexture* texture, GLenum target, GLint level);
void				DGLTexture_setLevel(DGLTexture* texture, GLenum target, GLint level, GLenum format, GLsizei width, GLsizei height);
GLeglImageOES		DGLTexture_getEGLImage(DGLTexture* texture, GLenum target);
void				DGLTexture_setEGLImage(DGLTexture* texture, GLenum target, GLeglImageOES image);
void				DGLTexture_generateMipmap(DGLTexture* texture);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // TEXTURE_H_
