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

#include "GLESContext.h"
#include "GLESTexture.h"

GLESContext::GLESContext(void* nativeContext) :
	m_nativeContext(nativeContext),
	m_texCoordArray(NULL),
	m_initialized(false)
{
}

GLESContext::~GLESContext()
{
	delete[] m_texCoordArray;

	{
		BufferMap::iterator iter;
		for(iter = m_buffers.begin(); iter != m_buffers.end(); ++iter)
		{
			delete iter->second;
		}
	}

	{
		TextureMap::iterator iter;
		for(iter = m_textures.begin(); iter != m_textures.end(); ++iter)
		{
			delete iter->second;
		}
	}
}

bool GLESContext::Initialize()
{
	GLES_ASSERT(!m_initialized);
	
	if(!m_dgl.Load())
	{
		return false;
	}

	// Initialize state.
	m_error = GL_NO_ERROR;
	m_clientActiveTexture = 0;
	m_arrayBufferBinding = 0;
	m_elementArrayBufferBinding = 0;
	m_enabledArrays = 0;

	int maxTextureUnits;
	int maxClipPlanes;
	int maxLights;
	m_dgl.glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits);
	m_dgl.glGetIntegerv(GL_MAX_CLIP_PLANES, &maxClipPlanes);
	m_dgl.glGetIntegerv(GL_MAX_LIGHTS, &maxLights);
	// The maximum number of texture units supported by the wrapper depends on the number
	// of bits in the array state variable (four bits are used by vertex, normal, color
	// and point size arrays).
	m_maxTextureUnits = GLES_MIN(maxTextureUnits, sizeof(m_enabledArrays) * 8 - 4);
	m_maxClipPlanes = maxClipPlanes;
	m_maxLights = maxLights;

	int maxTextureSize;
	m_dgl.glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	m_maxTextureLevel = glesLog2(maxTextureSize);

	m_texCoordArray = GLES_NEW GLESArray[m_maxTextureUnits];
	if(m_texCoordArray == NULL)
	{
		return false;
	}

	// Create texture named zero.
	BindTexture(0);

	m_initialized = true;

	return true;
}

GLenum GLESContext::GetHostError()
{
	GLenum host_error = m_dgl.glGetError();
	if(host_error != GL_NO_ERROR)
	{
		m_error = host_error;
	}
	return host_error;
}

GLESArray& GLESContext::TexCoordArray(unsigned int texture)
{
	GLES_ASSERT(texture >= 0 && texture < m_maxTextureUnits);
	return m_texCoordArray[texture];
}

GLESArray& GLESContext::TexCoordArray()
{
	return m_texCoordArray[m_clientActiveTexture];
}

const GLESArray& GLESContext::TexCoordArray(unsigned int texture) const
{
	GLES_ASSERT(texture >= 0 && texture < m_maxTextureUnits);
	return m_texCoordArray[texture];
}

const GLESArray& GLESContext::TexCoordArray() const
{
	return m_texCoordArray[m_clientActiveTexture];
}

void GLESContext::SetVertexArray(int size, GLenum type, int stride, const void *pointer)
{
	m_vertexArray = GLESArray(size, type, stride, const_cast<void*>(pointer), m_buffers[m_arrayBufferBinding]);
}

void GLESContext::SetNormalArray(GLenum type, int stride, const void *pointer)
{
	m_normalArray = GLESArray(3, type, stride, const_cast<void*>(pointer), m_buffers[m_arrayBufferBinding]);
}

void GLESContext::SetColorArray(int size, GLenum type, int stride, const void *pointer)
{
	m_colorArray = GLESArray(size, type, stride, const_cast<void*>(pointer), m_buffers[m_arrayBufferBinding]);
}

void GLESContext::SetPointerSizeArray(GLenum type, int stride, const void *pointer)
{
	m_pointSizeArray = GLESArray(1, type, stride, const_cast<void*>(pointer), m_buffers[m_arrayBufferBinding]);
}

void GLESContext::SetTexCoordArray(int size, GLenum type, int stride, const void *pointer)
{
	m_texCoordArray[m_clientActiveTexture] = GLESArray(size, type, stride,
													   const_cast<void*>(pointer), m_buffers[m_arrayBufferBinding]);
}

const GLESBuffer* GLESContext::Buffer(unsigned int buffer) const
{
	BufferMap::const_iterator iter = m_buffers.find(buffer);
	if(iter == m_buffers.end())
	{
		// Not found
		return NULL;
	}

	return iter->second;
}

GLESBuffer* GLESContext::ArrayBuffer()
{
	if(m_arrayBufferBinding)
	{
		return m_buffers[m_arrayBufferBinding];
	}
	else
	{
		return NULL;
	}
}

GLESBuffer* GLESContext::ElementArrayBuffer()
{
	if(m_elementArrayBufferBinding)
	{
		return m_buffers[m_elementArrayBufferBinding];
	}
	else
	{
		return NULL;
	}
}

void GLESContext::ReserveBufferNames(int num, unsigned int* names)
{
	GLES_ASSERT(num >= 0);
	unsigned int candidate = 1;
	while(num && candidate > 0)
	{
		if(m_buffers.find(candidate) == m_buffers.end())
		{
			m_buffers[candidate] = NULL;
			names[num-1] = candidate;
			num--;
		}
		candidate++;
	}
}

void GLESContext::DeleteBuffer(unsigned int buffer)
{
	if(m_vertexArray.BufferName() == buffer)
	{
		m_vertexArray.ReleaseBuffer();
	}
	
	if(m_normalArray.BufferName() == buffer)
	{
		m_normalArray.ReleaseBuffer();
	}

	if(m_colorArray.BufferName() == buffer)
	{
		m_colorArray.ReleaseBuffer();
	}

	for(unsigned int i = 0; i < m_maxTextureUnits; i++)
	{
		if(m_texCoordArray[i].BufferName() == buffer)
		{
			m_texCoordArray[i].ReleaseBuffer();
		}
	}

	if(m_arrayBufferBinding == buffer)
	{
		m_arrayBufferBinding = 0;
	}

	delete Buffer(buffer);
	m_buffers.erase(buffer);
}

bool GLESContext::BindArrayBuffer(unsigned int buffer)
{
	if(buffer != 0 && Buffer(buffer) == NULL)
	{
		// A new buffer must be created
		m_buffers[buffer] = GLES_NEW GLESBuffer(buffer);
		if(m_buffers[buffer] == NULL)
		{
			return false;
		}
	}
	m_arrayBufferBinding = buffer;

	return true;
}

bool GLESContext::BindElementArrayBuffer(unsigned int buffer)
{
	if(buffer != 0 && Buffer(buffer) == NULL)
	{
		// A new buffer must be created
		m_buffers[buffer] = GLES_NEW GLESBuffer(buffer);
		if(m_buffers[buffer] == NULL)
		{
			return false;
		}
	}
	m_elementArrayBufferBinding = buffer;

	return true;
}

void GLESContext::DeleteTexture(unsigned int texture)
{
	if(texture == 0)
	{
		// The texture named zero cannot be destroyed.
		return;
	}

	// Unbind texture.
	if(m_textureBinding == texture)
	{
		m_textureBinding = 0;
	}

	delete Texture(texture);
	m_textures.erase(texture);
}

bool GLESContext::BindTexture(unsigned int texture)
{
	if(Texture(texture) == NULL)
	{
		// A new texture must be created
		m_textures[texture] = GLES_NEW GLESTexture(texture);
		if(m_textures[texture] == NULL)
		{
			return false;
		}

		if(!m_textures[texture]->AllocateLevels(m_maxTextureLevel))
		{
			delete m_textures[texture];
			m_textures[texture] = NULL;
		}
	}
	m_textureBinding = texture;

	return true;
}

GLESTexture* GLESContext::Texture(unsigned int texture)
{
	TextureMap::const_iterator iter = m_textures.find(texture);
	if(iter == m_textures.end())
	{
		// Not found
		return NULL;
	}

	return iter->second;
}

void* glesGetCurrentGLESContext()
{
	void* context = EGLtoGLESInterface::GetEGLInterface()->GetGLESContext();
	return context;
}