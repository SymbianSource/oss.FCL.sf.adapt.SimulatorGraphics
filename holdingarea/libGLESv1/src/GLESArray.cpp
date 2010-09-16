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

#include "GLESArray.h"
#include "glesInternal.h"
#include "GLESBuffer.h"
#include "GLESContext.h"

GLESArrayPointer::GLESArrayPointer() :
    m_pointer(NULL),
    m_stride(0),
    m_size(4),
    m_type(GL_FLOAT)
{
}

GLESArrayPointer::GLESArrayPointer(const void* pointer, int stride, int size, GLenum type) :
    m_pointer(static_cast<const char*>(pointer)),
    m_stride(stride),
    m_size(size),
    m_type(type)
{
}

GLESArray::GLESArray() :
    m_size(4),
    m_type(GL_FLOAT),
    m_stride(0),
    m_pointer(NULL),
    m_ownsData(false),
    m_buffer(NULL)
{
}

GLESArray::GLESArray(int size, GLenum type, int stride, void* pointer, const GLESBuffer* buffer, bool ownsData) :
    m_size(size),
    m_type(type),
    m_stride(stride),
    m_pointer(pointer),
    m_ownsData(ownsData),
    m_buffer(buffer)
{
}

GLESArray::~GLESArray()
{
    if(m_ownsData)
    {
        delete[] m_pointer;
    }
}

GLESArrayPointer GLESArray::ArrayPointer() const
{
	int stride;
	if(m_stride)
	{
		stride = m_stride;
	}
	else
	{
		switch(m_type)
		{
		case GL_BYTE:
			stride = sizeof(GLbyte);
			break;
		case GL_SHORT:
			stride = sizeof(GLshort);
			break;
		case GL_FLOAT:
			stride = sizeof(GLfloat);
			break;
		case GL_UNSIGNED_BYTE:
			stride = sizeof(GLubyte);
			break;
		default:
			GLES_ASSERT(false);
		}
	}
	return GLESArrayPointer(m_pointer, m_size * stride, m_size, m_type);
}

GLESArray* GLESArray::Convert(int count) const
{
	if(m_type == GL_BYTE)
	{
		const char* oldPtr = NULL;
		unsigned int size = m_size;
		if(m_buffer)
		{
			oldPtr = static_cast<const char*>(m_buffer->data);
			size = GLES_MIN(m_buffer->size / sizeof(GLbyte), count * size);
		}
		else
		{
			oldPtr = static_cast<const char*>(m_pointer);
		}

		GLshort* newArr = GLES_NEW GLshort[count * m_size];
		if(newArr == NULL)
		{
			return NULL;
		}
		int stride = m_stride ? m_stride : sizeof(GLbyte);
		for(unsigned int i = 0; i < count * size; i++)
		{
			newArr[i] = *reinterpret_cast<const GLbyte*>(oldPtr);
			oldPtr += stride;
		}
		return GLES_NEW GLESArray(size, GL_SHORT, 0, newArr, NULL, true);
	}
	else if(m_type == GL_FIXED)
	{
		const char* oldPtr = NULL;
		unsigned int size = m_size;
		if(m_buffer)
		{
			oldPtr = static_cast<const char*>(m_buffer->data);
			size = GLES_MIN(m_buffer->size / sizeof(GLfixed), count * size);
		}
		else
		{
			oldPtr = static_cast<const char*>(m_pointer);
		}

		GLfloat* newArr = GLES_NEW GLfloat[count * m_size];
		if(newArr == NULL)
		{
			return NULL;
		}
		int stride = m_stride ? m_stride : sizeof(GLfixed);
		for(unsigned int i = 0; i < count * size; i++)
		{
			newArr[i] = X_TO_F(*reinterpret_cast<const GLfixed*>(oldPtr));
			oldPtr += stride;
		}
		return GLES_NEW GLESArray(size, GL_FLOAT, 0, newArr, NULL, true);
	}
	else
	{
		return GLES_NEW GLESArray(m_size, m_type, m_stride, m_buffer ? m_buffer->data : m_pointer, NULL);
	}

    return NULL;
}
