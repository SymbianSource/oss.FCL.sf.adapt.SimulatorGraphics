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

#ifndef _TRANSPARENTCOLOR_H_
#define _TRANSPARENTCOLOR_H_

#include "eglInternal.h"

class CTransparentColor
{
public:
    CTransparentColor(void) :
        m_transparentType( EGL_NONE ),
        m_redValue( 0 ),
        m_greenValue( 0 ),
        m_blueValue( 0 )
        {
        }
	~CTransparentColor(void);
    CTransparentColor( EGLint type, EGLint redValue, EGLint greenValue, EGLint blueValue );

public:
    void SetAttribute( EGLint attribute, EGLint value );
    inline EGLint Type() const { return m_transparentType; }
    inline EGLint RedValue() const { return m_redValue; }
    inline EGLint GreenValue() const { return m_greenValue; }
    inline EGLint BlueValue() const { return m_blueValue; }

private:
    EGLint  m_transparentType;
    EGLint  m_redValue;
    EGLint  m_greenValue;
    EGLint  m_blueValue;
};
#endif // _TRANSPARENTCOLOR_H_