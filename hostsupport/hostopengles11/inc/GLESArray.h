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

#ifndef _GLESARRAY_H_
#define _GLESARRAY_H_

#include <GLES/gl.h>
#include <cstddef>
#include "GLESBuffer.h"

class GLESArrayPointer
{
public:
	GLESArrayPointer();
	GLESArrayPointer(const void* pointer, int stride, int size, GLenum type);

	int Size() { return m_size; }
	GLenum Type() { return m_type; }

	bool operator!=(void* pointer) { return m_pointer != pointer; }
	GLESArrayPointer& operator+=(int step) { m_pointer += step * m_stride; return *this; }
	const void* operator[](int i) { return m_pointer + i * m_stride; }

private:
	const char* m_pointer;
	int m_stride;
	int m_size;
	GLenum m_type;
};

class GLESArray
{
public:
	GLESArray();
	GLESArray(int size, GLenum type, int stride, void* pointer, const GLESBuffer* buffer, bool ownsData=false);
	~GLESArray();

	int Size() const { return m_size; }
	GLenum Type() const { return m_type; }
	int Stride() const { return m_stride; }
	const void* Pointer() const { return m_pointer; }
	GLESArrayPointer ArrayPointer() const;
	const GLESBuffer* Buffer() const { return m_buffer; }
	unsigned int BufferName() const { return m_buffer ? m_buffer->name : 0; }
	const void* BufferPointer() const { return m_buffer ? m_buffer->data : NULL; }

	GLESArray* Convert(int count) const;

	void ReleaseBuffer() { m_buffer = NULL; }
	
private:
	int m_size;
	GLenum m_type;
	int m_stride;
	void* m_pointer;
	bool m_ownsData;
	const GLESBuffer* m_buffer;
};

#endif // _GLESARRAY_H_
