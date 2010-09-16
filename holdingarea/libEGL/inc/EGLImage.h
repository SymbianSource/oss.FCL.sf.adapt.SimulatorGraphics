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

#ifndef _EGLIMAGE_H_
#define _EGLIMAGE_H_

#include <EGL/egl.h>
#include <vector>
#include "SurfaceDescriptor.h"

// Forward declarations
class CEGLState;

class CEGLImage
{
public:
    CEGLImage( EGLenum target, EGLClientBuffer buffer, SurfaceDescriptor desc, void* data );
    virtual ~CEGLImage(void);
    inline EGLenum Target() const { return m_target; }
    EGLClientBuffer ClientBuffer() const { return m_buffer; }
    inline EGLint DataStride() const { return m_SurfDesc.m_stride; }
    inline EGLint Height() const { return m_SurfDesc.m_height; }
    inline EGLint Width() const { return m_SurfDesc.m_width; }
    inline EGLint DataSize() const { return (m_SurfDesc.m_height * m_SurfDesc.m_stride); }
	inline const SurfaceDescriptor& SurfaceDesc() const { return m_SurfDesc; }
    inline void* Data() const { return m_data; }
    void AddSibling( EGLenum target, EGLClientBuffer buffer);
    void UpdateData( CEGLState *state, void* data );
    void DestroyAllSiblings();


public:
    struct EGLImageSibling
        {
        EGLenum target;
        EGLClientBuffer buffer;
        EGLImageSibling* next;
        };
    
private:
    
    EGLenum m_target;
    EGLClientBuffer m_buffer;
    void* m_data;
    SurfaceDescriptor m_SurfDesc;
    EGLImageSibling* m_siblings;
    
};
#endif // _EGLIMAGE_H_
