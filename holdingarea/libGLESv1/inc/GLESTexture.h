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

#ifndef _GLESTEXTURE_H_
#define _GLESTEXTURE_H_

#include <GLES/gl.h>

struct GLESTextureLevel
{
	GLenum format;
	GLsizei width;
	GLsizei height;
	void* boundSurface; // The EGLSurface bound to this image with eglBindTexImage.
};

class GLESTexture
{
public:
	GLESTexture(GLuint name);
	bool AllocateLevels(int numLevels);
	~GLESTexture();

	GLuint Name() const { return m_name; }
	GLESTextureLevel* Level(int level);

	void SetLevel(int level, GLenum format, GLsizei width, GLsizei height);
	void GenerateMipmap();

private:
	GLuint m_name;
	int m_numLevels;
	GLESTextureLevel* m_levels;
};

bool glesIsValidCompressedFormat(GLenum format);
GLenum glesMapCompressedToBaseFormat(GLenum format);
void* glesUncompressImage(int level, GLenum format, int width, int height, int imageSize, const void* data);

#endif // _GLES_TEXTURE_H_