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

#ifndef _EGLPBUFFERSURFACE_H_
#define _EGLPBUFFERSURFACE_H_

#include "EGLSurface.h"

class CEGLPbufferSurface : public CEGLSurface
{
public:
    CEGLPbufferSurface( EGLint colorSpace,
                        EGLint alphaFormat,
                        EGLint renderBuffer,
                        CEGLConfig* config,
                        EGLint largestPbuffer,
                        EGLint textureFormat,
						EGLint textureTarget,
						EGLint mipmapTexture,
                        EGLClientBuffer clientBuf = NULL );
	~CEGLPbufferSurface(void);

public:
    inline EGLint IsLargestPbuffer() const { return m_largestPbuffer; }
    inline EGLint TextureFormat() const { return m_textureFormat; }
	inline EGLint TextureTarget() const { return m_textureTarget; }
	inline EGLint MipmapTexture() const { return m_mipmapTexture; }
    inline EGLClientBuffer ClientBuffer() const { return m_clientBuffer; }

    void SetNativePbufferContainer( EGLINativePbufferContainer* container );
    inline EGLINativePbufferType NativePbuffer() const { return (m_container ? m_container->pbuffer : NULL); }
    inline EGLINativeContextType CopyContext() const { return (m_container ? m_container->copyContext : NULL); }
    inline EGLINativeDisplayType NativeDisplay() const { return (m_container ? m_container->display : NULL); }
    inline struct EGLINativeGLFunctions* NativeGLFunctions() const { if( m_container ) return &(m_container->functions); else return NULL; }

    bool BindCopyContext();
    bool ReleaseCopyContext();

private:
    CEGLPbufferSurface(void);

private:
    EGLint                              m_largestPbuffer;
    EGLint                              m_textureFormat;
	EGLint                              m_textureTarget;
	EGLint								m_mipmapTexture;
    EGLClientBuffer                     m_clientBuffer;
    struct EGLINativePbufferContainer*  m_container;
    EGLINativeContextType               m_tmpContext;
    EGLINativeDisplayType               m_tmpDisplay;
};
#endif //_EGLPBUFFERSURFACE_H_
