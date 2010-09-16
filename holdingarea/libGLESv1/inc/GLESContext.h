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

#ifndef _GLESCONTEXT_H_
#define _GLESCONTEXT_H_

#include <GLES/gl.h>
#include <map>
#include <iostream>
#include "GLESArray.h"
#include "glesInternal.h"
#include "GLESDesktopGL.h"
#include "glesOS.h"
#include "EGLInterface.h"

struct GLESBuffer;
class GLESTexture;
struct GLESTextureLevel;

namespace
{
typedef std::map<unsigned int, GLESBuffer*> BufferMap;
typedef std::map<unsigned int, GLESTexture*> TextureMap;
}

enum GLESArrayFlag
{
	GLES_INVALID_ARRAY			= 0,
	GLES_VERTEX_ARRAY			= 1 << 0,
	GLES_NORMAL_ARRAY			= 1 << 1,
	GLES_COLOR_ARRAY			= 1 << 2,
	GLES_POINT_SIZE_ARRAY		= 1 << 3,
	GLES_TEXTURE_COORD_ARRAY	= 1 << 4
};

class GLESContext {
public:
	GLESContext(void* nativeContext);
	~GLESContext();

	bool Initialize();

	bool IsInitialized() const { return m_initialized; }
	const GLESDesktopGL& DGL() const { return m_dgl; }
	void* NativeContext() const { return m_nativeContext; }
	GLenum Error() const { return m_error; }
	GLenum GetHostError();
	unsigned int MaxTextureLevel() const { return m_maxTextureLevel; }
	unsigned int MaxTextureUnits() const { return m_maxTextureUnits; }
	unsigned int MaxClipPlanes() const { return m_maxClipPlanes; }
	unsigned int MaxLights() const { return m_maxLights; }
	int ClientActiveTexture() const { return m_clientActiveTexture; }
	GLESArray& VertexArray() { return m_vertexArray; }
	const GLESArray& VertexArray() const { return m_vertexArray; }
	GLESArray& NormalArray() { return m_normalArray; }
	const GLESArray& NormalArray() const { return m_normalArray; }
	GLESArray& ColorArray() { return m_colorArray; }
	const GLESArray& ColorArray() const { return m_colorArray; }
	const GLESArray& PointSizeArray() const { return m_pointSizeArray; }
	GLESArray& TexCoordArray(unsigned int texture);
	GLESArray& TexCoordArray();
	const GLESArray& TexCoordArray(unsigned int texture) const;
	const GLESArray& TexCoordArray() const;
	unsigned int ArrayBufferBinding() const { return m_arrayBufferBinding; }
	GLESBuffer* ArrayBuffer();
	unsigned int ElementArrayBufferBinding() const { return m_elementArrayBufferBinding; }
	GLESBuffer* ElementArrayBuffer();
	const GLESBuffer* Buffer(unsigned int buffer) const;
	unsigned int TextureBinding() const { return m_textureBinding; }
	GLESTexture* Texture(unsigned int name);
	const GLESTextureLevel* TextureLevel(GLint level) const;

	bool IsArrayEnabled(GLESArrayFlag array) const { return !!(m_enabledArrays & array); }

	void SetError(GLenum error) { m_error = error; }
	void SetClientActiveTexture(int texture) { m_clientActiveTexture = texture; }
	void SetVertexArray(int size, GLenum type, int stride, const void *pointer);
	void SetNormalArray(GLenum type, int stride, const void *pointer);
	void SetColorArray(int size, GLenum type, int stride, const void *pointer);
	void SetPointerSizeArray(GLenum type, int stride, const void *pointer);
	void SetTexCoordArray(int size, GLenum type, int stride, const void *pointer);

	void EnableArray(GLESArrayFlag array) { m_enabledArrays |= array; }
	void DisableArray(GLESArrayFlag array) { m_enabledArrays &= ~array; }

	void ReserveBufferNames(int num, unsigned int* names);
	void DeleteBuffer(unsigned int buffer);
	
	bool BindArrayBuffer(unsigned int buffer);
	bool BindElementArrayBuffer(unsigned int buffer);

	void DeleteTexture(unsigned int texture);
	bool BindTexture(unsigned int texture);

private:
	bool m_initialized;
	void* m_nativeContext;
	GLESDesktopGL m_dgl;
	GLenum m_error;
	unsigned int m_maxTextureLevel;
	unsigned int m_maxTextureUnits;
	unsigned int m_maxClipPlanes;
	unsigned int m_maxLights;
	int m_clientActiveTexture;
	GLESArray m_vertexArray;
	GLESArray m_normalArray;
	GLESArray m_colorArray;
	GLESArray m_pointSizeArray;
	GLESArray* m_texCoordArray;
	unsigned int m_enabledArrays;
	unsigned int m_arrayBufferBinding;
	unsigned int m_elementArrayBufferBinding;
	unsigned int m_textureBinding;
	BufferMap m_buffers;
	TextureMap m_textures;
};

void* glesGetCurrentGLESContext();

#define GLES_ENTER_RET(RETVAL) \
	GLESContext* ctx; \
	do \
	{ \
		glesGetLock(); \
		ctx = (GLESContext*)glesGetCurrentGLESContext(); \
		if(!ctx) \
		{ \
			glesReleaseLock(); \
			return RETVAL; \
		} \
		if(!ctx->IsInitialized()) \
		{ \
			if(!ctx->Initialize()) \
			{ \
				std::cerr << "Failed to initialize context" << std::endl; \
				exit(1); \
			} \
		} \
	} while(0)

#define GLES_ENTER() GLES_ENTER_RET(GLES_NO_RETVAL)

// Check the host error after every API function call so that the error flag
// in the wrapper always contains the latest error.
#define GLES_LEAVE_RET(RETVAL) \
	do \
	{ \
		ctx->GetHostError(); \
		glesReleaseLock(); \
		return RETVAL; \
	} while(0)

#define GLES_LEAVE() GLES_LEAVE_RET(GLES_NO_RETVAL)

#define GLES_LEAVE_NO_ERROR_CHECK_RET(RETVAL) \
	do \
	{ \
		glesReleaseLock(); \
		return RETVAL; \
	} while(0)

#define GLES_LEAVE_NO_ERROR_CHECK() GLES_LEAVE_NO_ERROR_CHECK_RET(GLES_NO_RETVAL)

#define GLES_ERROR_IF_RET(COND, ERROR, RETVAL) \
	do \
	{ \
		if(COND) \
		{ \
			ctx->SetError(ERROR); \
			return RETVAL; \
		} \
	} while(0)

#define GLES_ERROR_IF(COND, ERROR) GLES_ERROR_IF_RET(COND, ERROR, GLES_NO_RETVAL)
#define GLES_ERROR_RET(ERROR, RETVAL) GLES_ERROR_IF_RET(true, ERROR, RETVAL)
#define GLES_ERROR(ERROR) GLES_ERROR_IF(true, ERROR)

#endif // _GLESCONTEXT_H_