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

#include "ColorDescriptor.h"
#include "eglInternal.h"

CColorDescriptor::CColorDescriptor(EGLint red, EGLint green, EGLint blue, 
                             EGLint luminance, bool nonlinear, EGLint alpha, bool premultiplied,
                             EGLint alphaMask, EGLint bpp) :
    m_bpp( bpp ),
    m_redSize( red ),
    m_greenSize( green ),
    m_blueSize( blue ),
    m_luminanceSize( luminance ),
    m_alphaSize( alpha ),
    m_alphaMaskSize( alphaMask )
    {
    unsigned int f  = 0;

    if (nonlinear)
        f |= NONLINEAR;
   
    if (premultiplied)
        f |= PREMULTIPLIED;
   
    if ( luminance )
        f |= LUMINANCE;

    m_format = (ColorFormat)f;

    }

void CColorDescriptor::Set( int red, int green, int blue, int luminance,
                           bool nonlinear, int alpha, bool premultiplied, int alphaMask, int bpp)
    {
    m_bpp = bpp;
    m_redSize = red;
    m_greenSize = green;
    m_blueSize = blue;
    m_luminanceSize = luminance;
    m_alphaSize = alpha;
    m_alphaMaskSize = alphaMask;

    unsigned int f  = 0;
    if (nonlinear)
        f |= NONLINEAR;
   
    if (premultiplied)
        f |= PREMULTIPLIED;
   
    if ( luminance )
        f |= LUMINANCE;

    m_format = (ColorFormat)f;
    }

void CColorDescriptor::SetAttribute( EGLint attribute, EGLint value )
    {
    switch( attribute )
        {
        case EGL_BUFFER_SIZE:
            {
            m_bpp = value;
            break;
            }
        case EGL_RED_SIZE:
            {
            m_redSize = value;
            break;
            }
        case EGL_GREEN_SIZE:
            {
            m_greenSize = value;
            break;
            }
        case EGL_BLUE_SIZE:
            {
            m_blueSize = value;
            break;
            }
        case EGL_LUMINANCE_SIZE:
            {
            m_luminanceSize = value;
            break;
            }
        case EGL_ALPHA_SIZE:
            {
            m_alphaSize = value;
            break;
            }
        case EGL_ALPHA_MASK_SIZE:
            {
            m_alphaMaskSize = value;
            break;
            }
        case EGL_COLOR_BUFFER_TYPE:
            {
            SetColorBufferType(value);
            break;
            }
        case EGL_SURFACE_TYPE:
            SetSurfaceType(value);
            break;
        default:
            EGLI_ASSERT( false );
        }
    }

EGLint CColorDescriptor::GetAttribute( EGLint attribute ) const
    {
    switch( attribute )
        {
        case EGL_RED_SIZE:
            {
            return m_redSize;
            }
        case EGL_GREEN_SIZE:
            {
            return m_greenSize;
            }
        case EGL_BLUE_SIZE:
            {
            return m_blueSize;
            }
        case EGL_LUMINANCE_SIZE:
            {
            return m_luminanceSize;
            }
        case EGL_ALPHA_SIZE:
            {
            return m_alphaSize;
            }
        case EGL_ALPHA_MASK_SIZE:
            {
            return m_alphaMaskSize;
            }
        case EGL_COLOR_BUFFER_TYPE:
            {
            if( isLuminance() )
                return EGL_LUMINANCE_BUFFER;
            else
                return EGL_RGB_BUFFER;
            }
        case EGL_SURFACE_TYPE:
            return GetSurfaceType();
            break;
        default:
            EGLI_ASSERT( false );
            return 0;
        }
    }

/**
 * \brief Set alpha type and color space using bits
 * \note  Be careful not to mix with enumerations. Use the two-parameter version
 *        to set from enums.
 */
void CColorDescriptor::SetSurfaceType(EGLint surfaceType)
{
    unsigned int f = m_format;

    f &= ~PREMULTIPLIED;

    if (surfaceType & EGL_VG_ALPHA_FORMAT_PRE_BIT)
        f |= PREMULTIPLIED;     

    f &= ~NONLINEAR;
    if (!(surfaceType & EGL_VG_COLORSPACE_LINEAR_BIT))
        f |= NONLINEAR;

    m_format = (ColorFormat)f;    
}

/**
 * \brief Set alpha type and color space using enumerations.
 */
void CColorDescriptor::SetSurfaceType(EGLint colorSpace, EGLint alphaType)
{
    unsigned int f = m_format;

    f &= ~PREMULTIPLIED;

    if (alphaType == EGL_VG_ALPHA_FORMAT_PRE)
        f |= PREMULTIPLIED;     

    f &= ~NONLINEAR;
    if (colorSpace == EGL_VG_COLORSPACE_sRGB)
        f |= NONLINEAR;

    m_format = (ColorFormat)f;    
}


EGLint CColorDescriptor::GetSurfaceType() const
{
    EGLint ret = 0;

    if (!(m_format & NONLINEAR))
        ret |= EGL_VG_COLORSPACE_LINEAR_BIT;

    if (m_format & PREMULTIPLIED)
        ret |= EGL_VG_ALPHA_FORMAT_PRE_BIT;

    return ret;
}

void CColorDescriptor::SetColorBufferType( EGLint colorBufferType )
{
    unsigned int f = m_format & (~LUMINANCE);

    switch(colorBufferType)
    {
    case EGL_LUMINANCE_BUFFER:
        f |= LUMINANCE;
        break;
    default:
        EGLI_ASSERT(colorBufferType == EGL_RGB_BUFFER);
        break;
    }

    m_format = (ColorFormat)f;
}

CColorDescriptor& CColorDescriptor::operator=( const CColorDescriptor& rhs )
    {
    if( this == &rhs ) return *this;
    m_bpp = rhs.m_bpp;
    m_redSize = rhs.m_redSize;
    m_greenSize = rhs.m_greenSize;
    m_blueSize = rhs.m_blueSize;
    m_luminanceSize = rhs.m_luminanceSize;
    m_alphaSize = rhs.m_alphaSize;
    m_alphaMaskSize = rhs.m_alphaMaskSize;
    m_format = rhs.m_format;
    return *this;
    }

bool CColorDescriptor::operator==( const CColorDescriptor& rhs ) const
    {
    if( this == &rhs ) return true;
    
    if( isNonlinear() != rhs.isNonlinear() ) return false;
    if( isPremultiplied() != rhs.isPremultiplied() ) return false;

    if( m_alphaSize != rhs.m_alphaSize ) return false;
    if( m_alphaMaskSize != rhs.m_alphaMaskSize ) return false;
    
    if( isLuminance() && rhs.isLuminance()  )
        {
        if( m_luminanceSize != rhs.m_luminanceSize ) return false;        
        }
    else
        {
        if( m_bpp != rhs.m_bpp ) return false;
        if( m_redSize != rhs.m_redSize ) return false;
        if( m_greenSize != rhs.m_greenSize ) return false;
        if( m_blueSize != rhs.m_blueSize ) return false;
        }
    return true;
    }

bool CColorDescriptor::operator!=( const CColorDescriptor& rhs ) const
    {
    return !(operator==(rhs));
    }

bool CColorDescriptor::MatchBitDepth( const CColorDescriptor& desc ) const
    {
    if( m_alphaSize != desc.m_alphaSize ) return false;
    if( isPremultiplied() != desc.isPremultiplied() ) return false;
    if( isLuminance() )
        {
        if( !desc.isLuminance() || (m_luminanceSize != desc.m_luminanceSize ) ) return false;
        }
    else
        {
        if( m_redSize != desc.m_redSize ) return false;
        if( m_greenSize != desc.m_greenSize ) return false;
        if( m_blueSize != desc.m_blueSize ) return false;
        if( isNonlinear() != desc.isNonlinear() ) return false;
        }
    return true;
    }


