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

#ifndef _COLORDESCRIPTOR_H_
#define _COLORDESCRIPTOR_H_

class CColorDescriptor
{
public:

    enum ColorFormat {
        lRGBA			= 0,
        sRGBA			= 1,
        lRGBA_PRE		= 2,
        sRGBA_PRE		= 3,
        lLA				= 4,
        sLA				= 5,
        lLA_PRE			= 6,
        sLA_PRE			= 7
    };

    enum ColorFormatBits
    {
        NONLINEAR		= (1<<0),
        PREMULTIPLIED	= (1<<1),
        LUMINANCE		= (1<<2)
    };

    CColorDescriptor(void) {
        m_bpp           = 0;
        m_redSize       = 0;
        m_greenSize     = 0;
        m_blueSize      = 0;
        m_luminanceSize = 0;
        m_alphaSize     = 0;
        m_alphaMaskSize = 0;
        m_format        = lRGBA_PRE;
        }

	CColorDescriptor(int red, int green, int blue, 
                     int luminance, bool nonlinear, int alpha, bool premultiplied,
                     int alphaMask, int bpp);
    ~CColorDescriptor(void) {};

    void Set( int red, int green, int blue, int luminance, bool nonlinear, int alpha,
        bool premultiplied, int alphaMask, int bpp);

    void SetAttribute( int attribute, int value );
    int GetAttribute( int attribute ) const;
    inline int Bpp() const { return m_bpp; }
    inline int RedSize() const { return m_redSize; }
    inline int GreenSize() const { return m_greenSize; }
    inline int BlueSize() const { return m_blueSize; }
    inline int LuminanceSize() const { return m_luminanceSize; }
    inline int AlphaSize() const { return m_alphaSize; }
    inline int AlphaMaskSize() const { return m_alphaMaskSize; }
    inline bool isLuminance() const { return (m_format & LUMINANCE) != 0; }
    inline bool isNonlinear() const { return (m_format & NONLINEAR) != 0; }
    inline bool isPremultiplied() const { return (m_format & PREMULTIPLIED) != 0; }
    ColorFormat Format() const { return m_format; }

    int GetSurfaceType() const;
    void SetSurfaceType(int surfaceType);
    void SetSurfaceType(int colorSpace, int alphaFormat);
    void SetColorBufferType( int colorBufferType );

    CColorDescriptor& operator=( const CColorDescriptor& rhs );
    bool operator==( const CColorDescriptor& rhs ) const;
    bool operator!=( const CColorDescriptor& rhs ) const;
    bool MatchBitDepth( const CColorDescriptor& desc ) const;

public:
    int  m_bpp;            /* Bits per pixel */
	int  m_redSize;        /* EGL_RED_SIZE */
    int  m_greenSize;      /* EGL_GREEN_SIZE */
    int  m_blueSize;       /* EGL_BLUE_SIZE */
    int  m_luminanceSize;  /* EGL_LUMINANCE_SIZE */
    int  m_alphaSize;      /* EGL_ALPHA_SIZE */
    int  m_alphaMaskSize;  /* EGL_ALPHA_MASK_SIZE */
    ColorFormat  m_format; /* Color format */
};
#endif //_COLORDESCRIPTOR_H_
