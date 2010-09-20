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

#include "TransparentColor.h"

CTransparentColor::~CTransparentColor(void)
    {
    }

CTransparentColor::CTransparentColor( EGLint type, EGLint redValue, EGLint greenValue, EGLint blueValue ) :
    m_transparentType( type ),
    m_redValue( redValue ),
    m_greenValue( greenValue ),
    m_blueValue( blueValue )
    {
    }

void CTransparentColor::SetAttribute( EGLint attribute, EGLint value )
    {
    switch( attribute )
        {
        case EGL_TRANSPARENT_TYPE:
            {
            m_transparentType = value;
            break;
            }
        case EGL_TRANSPARENT_RED_VALUE:
            {
            m_redValue = value;
            break;
            }
        case EGL_TRANSPARENT_GREEN_VALUE:
            {
            m_greenValue = value;
            break;
            }
        case EGL_TRANSPARENT_BLUE_VALUE:
            {
            m_blueValue = value;
            break;
            }
        default:
            EGLI_ASSERT( false );
        }
    }
