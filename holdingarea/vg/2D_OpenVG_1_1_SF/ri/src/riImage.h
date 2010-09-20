#ifndef __RIIMAGE_H
#define __RIIMAGE_H

/*------------------------------------------------------------------------
 *
 * OpenVG 1.1 Reference Implementation
 * -----------------------------------
 *
 * Copyright (c) 2007 The Khronos Group Inc.
 * Portions copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *//**
 * \file
 * \brief	Color and Image classes.
 * \note
 *//*-------------------------------------------------------------------*/

#ifndef _OPENVG_H
#include "VG/openvg.h"
#endif

#ifndef __RIMATH_H
#include "riMath.h"
#endif

#ifndef __RIARRAY_H
#include "riArray.h"
#endif

#include "sfAlphaRcp.h"
#include "sfGammaLUT.h"
#include "riUtils.h"

//==============================================================================================

namespace OpenVGRI
{

class VGContext;
class DynamicBlitter;

/*-------------------------------------------------------------------*//*!
* \brief	A class representing rectangles.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

class Rectangle
{
public:
    Rectangle() : x(0), y(0), width(0), height(0) {}
    Rectangle(int rx, int ry, int rw, int rh) : x(rx), y(ry), width(rw), height(rh) {}
    void		intersect(const Rectangle& r)
    {
        if(width >= 0 && r.width >= 0 && height >= 0 && r.height >= 0)
        {
            int x1 = RI_INT_MIN(RI_INT_ADDSATURATE(x, width), RI_INT_ADDSATURATE(r.x, r.width));
            x = RI_INT_MAX(x, r.x);
            width = RI_INT_MAX(x1 - x, 0);

            int y1 = RI_INT_MIN(RI_INT_ADDSATURATE(y, height), RI_INT_ADDSATURATE(r.y, r.height));
            y = RI_INT_MAX(y, r.y);
            height = RI_INT_MAX(y1 - y, 0);
        }
        else
        {
            x = 0;
            y = 0;
            width = 0;
            height = 0;
        }
    }
    bool isEmpty() const { return width == 0 || height == 0; }

    int			x;
    int			y;
    int			width;
    int			height;
};

/*-------------------------------------------------------------------*//*!
* \brief	A class representing color for processing and converting it
*			to and from various surface formats.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

class Color
{
public:
    enum FormatSize
    {
        SIZE_1      = 0,
        SIZE_4      = 1,
        SIZE_8      = 2,
        SIZE_16     = 3,
        SIZE_24     = 4,
        SIZE_32     = 5
    };

    enum Shape
    {
        SHAPE_RGBA            = 0,
        SHAPE_RGBX            = 1,
        SHAPE_RGB             = 2,
        SHAPE_LA              = 3,
        SHAPE_L               = 4,
        SHAPE_A               = 5,
        SHAPE_ARGB            = 6,
        SHAPE_XRGB            = 7,
        SHAPE_AL              = 8,
        SHAPE_BGRA            = 9,
        SHAPE_BGRX            = 10,
        SHAPE_BGR             = 11,
        SHAPE_ABGR            = 12,
        SHAPE_XBGR            = 13
    };
    enum InternalFormat
    {
        lRGBA			= 0,
        sRGBA			= 1,
        lRGBA_PRE		= 2,
        sRGBA_PRE		= 3,
        lLA				= 4,
        sLA				= 5,
        lLA_PRE			= 6,
        sLA_PRE			= 7
    };
    enum FormatBits
    {
        NONLINEAR		= (1<<0),
        PREMULTIPLIED	= (1<<1),
        LUMINANCE		= (1<<2)
    };
    struct SmallDescriptor
    {
        RIuint32        toUint32()
        {
            RIuint32 ret = 0;
            ret = (RIuint32)size;
            ret |= (RIuint32)shape << 3;
            ret |= (RIuint32)internalFormat << (3 + 4);
            return ret;
        }
        FormatSize      size;
        Shape           shape;
        InternalFormat  internalFormat;
    };
    class Descriptor
    {
    public:
        Descriptor() {};
        RI_INLINE Descriptor(int dredBits, int dredShift, int dgreenBits, int dgreenShift, int dblueBits, int dblueShift, int dalphaBits, int dalphaShift, int dluminanceBits, int dluminanceShift, InternalFormat dinternalFormat, int dbpp, Shape shape);
        RI_INLINE bool      isNonlinear() const                                 { return (internalFormat & NONLINEAR) ? true : false; }
        RI_INLINE void      setNonlinear(bool nonlinear);
        RI_INLINE bool      isPremultiplied() const                             { return (internalFormat & PREMULTIPLIED) ? true : false; }
        RI_INLINE bool      isLuminance() const                                 { return (internalFormat & LUMINANCE) ? true : false; }
        RI_INLINE bool      isAlphaOnly() const                                 { return (alphaBits && (redBits+greenBits+blueBits+luminanceBits) == 0) ? true : false; }
        RI_INLINE bool      isBW() const { return isLuminance() && (luminanceBits == 1); }
        RI_INLINE bool      hasAlpha() const { return alphaBits > 0; }
        RI_INLINE bool      operator==(const Descriptor& rhs) const;
        RI_INLINE bool      isShiftConversionToLower(const Descriptor& rhs) const;
        RI_INLINE bool      isShiftConversion(const Descriptor& rhs) const;
        RI_INLINE bool      isZeroConversion(const Descriptor& rhs) const;
        RI_INLINE bool      maybeUnsafe() const { return internalFormat & PREMULTIPLIED ? true : false; };
        static RI_INLINE RIuint32  crossConvertToLower(RIuint32 c, const Descriptor& src, const Descriptor& dst);
        void                toSmallDescriptor(SmallDescriptor& smallDesc) const;
        RI_INLINE RIuint32  toIndex() const;
        static Descriptor   getDummyDescriptor();
        Shape               getShape() const;

        int				redBits;
        int				redShift;
        int				greenBits;
        int				greenShift;
        int				blueBits;
        int				blueShift;
        int				alphaBits;
        int				alphaShift;
        int				luminanceBits;
        int				luminanceShift;
        Shape           shape;
        VGImageFormat   vgFormat; // \note Storage only
        InternalFormat	internalFormat;
        int				bitsPerPixel;
        // Derived info:
        int             bytesPerPixel;
        int             maskBits;
        int             maskShift;
    };

    RI_INLINE Color() : r(0.0f), g(0.0f), b(0.0f), a(0.0f), m_format(sRGBA_PRE)													{}
    RI_INLINE Color(RIfloat cl, RIfloat ca, InternalFormat cs) : r(cl), g(cl), b(cl), a(ca), m_format(cs)							{ RI_ASSERT(cs == lLA || cs == sLA || cs == lLA_PRE || cs == sLA_PRE); }
    RI_INLINE Color(RIfloat cr, RIfloat cg, RIfloat cb, RIfloat ca, InternalFormat cs) : r(cr), g(cg), b(cb), a(ca), m_format(cs)	{ RI_ASSERT(cs == lRGBA || cs == sRGBA || cs == lRGBA_PRE || cs == sRGBA_PRE || cs == lLA || cs == sLA || cs == lLA_PRE || cs == sLA_PRE); }
    RI_INLINE Color(const Color& c) : r(c.r), g(c.g), b(c.b), a(c.a), m_format(c.m_format)									{}
    RI_INLINE Color& operator=(const Color&c)										{ r = c.r; g = c.g; b = c.b; a = c.a; m_format = c.m_format; return *this; }
    RI_INLINE void operator*=(RIfloat f)											{ r *= f; g *= f; b *= f; a*= f; }
    RI_INLINE void operator+=(const Color& c1)										{ RI_ASSERT(m_format == c1.getInternalFormat()); r += c1.r; g += c1.g; b += c1.b; a += c1.a; }
    RI_INLINE void operator-=(const Color& c1)										{ RI_ASSERT(m_format == c1.getInternalFormat()); r -= c1.r; g -= c1.g; b -= c1.b; a -= c1.a; }

    void						set(RIfloat cl, RIfloat ca, InternalFormat cs)							{ RI_ASSERT(cs == lLA || cs == sLA || cs == lLA_PRE || cs == sLA_PRE); r = cl; g = cl; b = cl; a = ca; m_format = cs; }
    void						set(RIfloat cr, RIfloat cg, RIfloat cb, RIfloat ca, InternalFormat cs)	{ RI_ASSERT(cs == lRGBA || cs == sRGBA || cs == lRGBA_PRE || cs == sRGBA_PRE); r = cr; g = cg; b = cb; a = ca; m_format = cs; }
    void						unpack(unsigned int inputData, const Descriptor& inputDesc);
    unsigned int				pack(const Descriptor& outputDesc) const;
    RI_INLINE InternalFormat	getInternalFormat() const							{ return m_format; }

    //clamps nonpremultiplied colors and alpha to [0,1] range, and premultiplied alpha to [0,1], colors to [0,a]
    void						clamp()												{ a = RI_CLAMP(a,0.0f,1.0f); RIfloat u = (m_format & PREMULTIPLIED) ? a : (RIfloat)1.0f; r = RI_CLAMP(r,0.0f,u); g = RI_CLAMP(g,0.0f,u); b = RI_CLAMP(b,0.0f,u); }
    void						convert(InternalFormat outputFormat);
    void						premultiply()										{ if(!(m_format & PREMULTIPLIED)) { r *= a; g *= a; b *= a; m_format = (InternalFormat)(m_format | PREMULTIPLIED); } }
    void						unpremultiply()										{ if(m_format & PREMULTIPLIED) { RIfloat ooa = (a != 0.0f) ? 1.0f/a : (RIfloat)0.0f; r *= ooa; g *= ooa; b *= ooa; m_format = (InternalFormat)(m_format & ~PREMULTIPLIED); } }
    void                        luminanceToRGB()                                    { if(m_format & LUMINANCE) { RI_ASSERT(r == g && g == b); m_format = (InternalFormat)(m_format & ~LUMINANCE); } }

    bool                        isNonlinear() const                                 { return (m_format & NONLINEAR) ? true : false; }
    bool                        isPremultiplied() const                             { return (m_format & PREMULTIPLIED) ? true : false; }
    bool                        isLuminance() const                                 { return (m_format & LUMINANCE) ? true : false; }

    RI_INLINE void              assertConsistency() const;

    // \note Why are these in the color class instead of descriptor?
    static VGImageFormat        descriptorToVGImageFormat(const Descriptor& desc);
    RI_INLINE static Descriptor formatToDescriptorConst(VGImageFormat format);
    static Descriptor			formatToDescriptor(VGImageFormat format);
    static bool					isValidDescriptor(const Descriptor& desc);

    RIfloat		r;
    RIfloat		g;
    RIfloat		b;
    RIfloat		a;
private:
    InternalFormat	m_format;
};

RI_INLINE Color::Descriptor::Descriptor(int dredBits, int dredShift, int dgreenBits, int dgreenShift, int dblueBits, int dblueShift, int dalphaBits, int dalphaShift, int dluminanceBits, int dluminanceShift, InternalFormat dinternalFormat, int dbpp, Shape shape) :
    redBits(dredBits),
    redShift(dredShift),
    greenBits(dgreenBits),
    greenShift(dgreenShift),
    blueBits(dblueBits),
    blueShift(dblueShift),
    alphaBits(dalphaBits),
    alphaShift(dalphaShift),
    luminanceBits(dluminanceBits),
    luminanceShift(dluminanceShift),
    shape(shape),
    internalFormat(dinternalFormat),
    bitsPerPixel(dbpp)
{
    bytesPerPixel = bitsPerPixel / 8;

    if (alphaBits)
    {
        maskBits = alphaBits;
        maskShift = alphaShift;
    }
    else if (!this->isLuminance())
    {
        maskBits = redBits;
        maskShift = redShift;
    }
    else
    {
        maskBits = luminanceBits;
        maskShift = luminanceShift;
    }
    RI_ASSERT(getShape() == shape);
}

RI_INLINE void Color::Descriptor::setNonlinear(bool nonlinear)
{
    if (nonlinear)
        internalFormat = (InternalFormat)(((RIuint32)internalFormat)|NONLINEAR);
    else
        internalFormat = (InternalFormat)(((RIuint32)internalFormat)&(~NONLINEAR));
}

/**
 * \brief	Creates a pixel format descriptor out of VGImageFormat
 * \todo    The formats without alpha were non-premultiplied in the reference
 *          implementation, but wouldn't it make more sense to consider them
 *          premultiplied? This would make sense at least when blitting to
 *          windows, etc., where the output color should have the alpha
 *          multiplied "in".
 */
RI_INLINE Color::Descriptor Color::formatToDescriptorConst(VGImageFormat format)
{
    switch(format)
    {
    case VG_sRGBX_8888:
        return Color::Descriptor(8, 24, 8, 16, 8, 8, 0, 0, 0, 0, Color::sRGBA, 32, SHAPE_RGBX);
    case VG_sRGBA_8888:
        return Color::Descriptor(8, 24, 8, 16, 8, 8, 8, 0, 0, 0, Color::sRGBA, 32, SHAPE_RGBA);
    case VG_sRGBA_8888_PRE:
        return Color::Descriptor(8, 24, 8, 16, 8, 8, 8, 0, 0, 0, Color::sRGBA_PRE, 32, SHAPE_RGBA);
    case VG_sRGB_565:
        return Color::Descriptor(5, 11, 6, 5, 5, 0, 0, 0, 0, 0, Color::sRGBA, 16, SHAPE_RGB);
    case VG_sRGBA_5551:
        return Color::Descriptor(5, 11, 5, 6, 5, 1, 1, 0, 0, 0, Color::sRGBA, 16, SHAPE_RGBA);
    case VG_sRGBA_4444:
        return Color::Descriptor(4, 12, 4, 8, 4, 4, 4, 0, 0, 0, Color::sRGBA, 16, SHAPE_RGBA);
    case VG_sL_8:
        return Color::Descriptor(0, 0, 0, 0, 0, 0, 0, 0, 8, 0, Color::sLA, 8, SHAPE_L);
    case VG_lRGBX_8888:
        return Color::Descriptor(8, 24, 8, 16, 8, 8, 0, 0, 0, 0, Color::lRGBA, 32, SHAPE_RGBX);
    case VG_lRGBA_8888:
        return Color::Descriptor(8, 24, 8, 16, 8, 8, 8, 0, 0, 0, Color::lRGBA, 32, SHAPE_RGBA);
    case VG_lRGBA_8888_PRE:
        return Color::Descriptor(8, 24, 8, 16, 8, 8, 8, 0, 0, 0, Color::lRGBA_PRE, 32, SHAPE_RGBA);
    case VG_lL_8:
        return Color::Descriptor(0, 0, 0, 0, 0, 0, 0, 0, 8, 0, Color::lLA, 8, SHAPE_L);
    case VG_A_8:
        return Color::Descriptor(0, 0, 0, 0, 0, 0, 8, 0, 0, 0, Color::lRGBA, 8, SHAPE_A);
    case VG_BW_1:
        return Color::Descriptor(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, Color::lLA, 1, SHAPE_L);
    case VG_A_1:
        return Color::Descriptor(0, 0, 0, 0, 0, 0, 1, 0, 0, 0, Color::lRGBA, 1, SHAPE_A);
    case VG_A_4:
        return Color::Descriptor(0, 0, 0, 0, 0, 0, 4, 0, 0, 0, Color::lRGBA, 4, SHAPE_A);

  /* {A,X}RGB channel ordering */
    case VG_sXRGB_8888:
        return Color::Descriptor(8, 16, 8, 8, 8, 0, 0, 0, 0, 0, Color::sRGBA, 32, SHAPE_XRGB);
    case VG_sARGB_8888:
        return Color::Descriptor(8, 16, 8, 8, 8, 0, 8, 24, 0, 0, Color::sRGBA, 32, SHAPE_ARGB);
    case VG_sARGB_8888_PRE:
        return Color::Descriptor(8, 16, 8, 8, 8, 0, 8, 24, 0, 0, Color::sRGBA_PRE, 32, SHAPE_ARGB);
    case VG_sARGB_1555:
        return Color::Descriptor(5, 10, 5, 5, 5, 0, 1, 15, 0, 0, Color::sRGBA, 16, SHAPE_ARGB);
    case VG_sARGB_4444:
        return Color::Descriptor(4, 8, 4, 4, 4, 0, 4, 12, 0, 0, Color::sRGBA, 16, SHAPE_ARGB);
    case VG_lXRGB_8888:
        return Color::Descriptor(8, 16, 8, 8, 8, 0, 0, 0, 0, 0, Color::lRGBA, 32, SHAPE_XRGB);
    case VG_lARGB_8888:
        return Color::Descriptor(8, 16, 8, 8, 8, 0, 8, 24, 0, 0, Color::lRGBA, 32, SHAPE_ARGB);
    case VG_lARGB_8888_PRE:
        return Color::Descriptor(8, 16, 8, 8, 8, 0, 8, 24, 0, 0, Color::lRGBA_PRE, 32, SHAPE_ARGB);

  /* BGR{A,X} channel ordering */
    case VG_sBGRX_8888:
        return Color::Descriptor(8, 8, 8, 16, 8, 24, 0, 0, 0, 0, Color::sRGBA, 32, SHAPE_BGRX);
    case VG_sBGRA_8888:
        return Color::Descriptor(8, 8, 8, 16, 8, 24, 8, 0, 0, 0, Color::sRGBA, 32, SHAPE_BGRA);
    case VG_sBGRA_8888_PRE:
        return Color::Descriptor(8, 8, 8, 16, 8, 24, 8, 0, 0, 0, Color::sRGBA_PRE, 32, SHAPE_BGRA);
    case VG_sBGR_565:
        return Color::Descriptor(5, 0, 6, 5, 5, 11, 0, 0, 0, 0, Color::sRGBA, 16, SHAPE_BGR);
    case VG_sBGRA_5551:
        return Color::Descriptor(5, 1, 5, 6, 5, 11, 1, 0, 0, 0, Color::sRGBA, 16, SHAPE_BGRA);
    case VG_sBGRA_4444:
        return Color::Descriptor(4, 4, 4, 8, 4, 12, 4, 0, 0, 0, Color::sRGBA, 16, SHAPE_BGRA);
    case VG_lBGRX_8888:
        return Color::Descriptor(8, 8, 8, 16, 8, 24, 0, 0, 0, 0, Color::lRGBA, 32, SHAPE_BGRX);
    case VG_lBGRA_8888:
        return Color::Descriptor(8, 8, 8, 16, 8, 24, 8, 0, 0, 0, Color::lRGBA, 32, SHAPE_BGRA);
    case VG_lBGRA_8888_PRE:
        return Color::Descriptor(8, 8, 8, 16, 8, 24, 8, 0, 0, 0, Color::lRGBA_PRE, 32, SHAPE_BGRA);

  /* {A,X}BGR channel ordering */
    case VG_sXBGR_8888:
        return Color::Descriptor(8, 0, 8, 8, 8, 16, 0, 0, 0, 0, Color::sRGBA, 32, SHAPE_XBGR);
    case VG_sABGR_8888:
        return Color::Descriptor(8, 0, 8, 8, 8, 16, 8, 24, 0, 0, Color::sRGBA, 32, SHAPE_ABGR);
    case VG_sABGR_8888_PRE:
        return Color::Descriptor(8, 0, 8, 8, 8, 16, 8, 24, 0, 0, Color::sRGBA_PRE, 32, SHAPE_ABGR);
    case VG_sABGR_1555:
        return Color::Descriptor(5, 0, 5, 5, 5, 10, 1, 15, 0, 0, Color::sRGBA, 16, SHAPE_ABGR);
    case VG_sABGR_4444:
        return Color::Descriptor(4, 0, 4, 4, 4, 8, 4, 12, 0, 0, Color::sRGBA, 16, SHAPE_ABGR);
    case VG_lXBGR_8888:
        return Color::Descriptor(8, 0, 8, 8, 8, 16, 0, 0, 0, 0, Color::lRGBA, 32, SHAPE_XBGR);
    case VG_lABGR_8888:
        return Color::Descriptor(8, 0, 8, 8, 8, 16, 8, 24, 0, 0, Color::lRGBA, 32, SHAPE_ABGR);
    default:
    //case VG_lABGR_8888_PRE:
        RI_ASSERT(format == VG_lABGR_8888_PRE);
        return Color::Descriptor(8, 0, 8, 8, 8, 16, 8, 24, 0, 0, Color::lRGBA_PRE, 32, SHAPE_ABGR);
    }
}

RI_INLINE bool 	Color::Descriptor::operator==(const Descriptor& rhs) const
{
    return memcmp(this, &rhs, sizeof(Descriptor)) ? false : true;
}

RI_INLINE bool Color::Descriptor::isZeroConversion(const Descriptor& rhs) const
{
    return (shape == rhs.shape) &&
        (internalFormat == rhs.internalFormat) &&
        (redBits == rhs.redBits) &&
        (greenBits == rhs.greenBits) &&
        (blueBits == rhs.blueBits) &&
        (alphaBits == rhs.alphaBits) &&
        (luminanceBits == rhs.luminanceBits);
}

RI_INLINE bool Color::Descriptor::isShiftConversion(const Descriptor& rhs) const
{
    // \note BW conversion is always forced to full at the moment.
    if (isBW() != rhs.isBW()) 
        return false;

    return (isPremultiplied() == rhs.isPremultiplied())
            && (isNonlinear() == rhs.isNonlinear())
            && (isLuminance() == rhs.isLuminance());
}

RI_INLINE bool Color::Descriptor::isShiftConversionToLower(const Descriptor& rhs) const
{
    // \note BW conversion is always forced to full at the moment.
    if (isBW() != rhs.isBW()) 
        return false;
    // \note Mask bits are not checked because they are derived information.
    return (isShiftConversion(rhs)
            && (rhs.redBits <= redBits)
            && (rhs.greenBits <= greenBits)
            && (rhs.blueBits <= blueBits)
            && (rhs.alphaBits <= alphaBits)
            && (rhs.luminanceBits <= luminanceBits));

}

/**
 * \brief   In-place conversion of packed color to lower bit-depth
 * \param   c   Input packed color
 * \param   src Source color descriptor
 * \param   dst Destination color descriptor
 */
RI_INLINE RIuint32  Color::Descriptor::crossConvertToLower(RIuint32 c, const Descriptor& src, const Descriptor& dst)
{
    RIuint32 r = 0;

    RI_ASSERT(dst.redBits <= src.redBits);
    RI_ASSERT(dst.greenBits <= src.greenBits);
    RI_ASSERT(dst.blueBits <= src.blueBits);
    RI_ASSERT(dst.alphaBits <= src.alphaBits);

    if (src.isLuminance())
    {
        RI_ASSERT(dst.isLuminance());
        r = ((c >> (src.luminanceShift + src.luminanceBits - dst.luminanceBits)) & ((1u<<dst.luminanceBits)-1)) << dst.luminanceShift;
    } else
    {
        r = ((c >> (src.redShift + src.redBits - dst.redBits)) & ((1u<<dst.redBits)-1)) << dst.redShift;
        r |= ((c >> (src.greenShift + src.greenBits - dst.greenBits)) & ((1u<<dst.greenBits)-1)) << dst.greenShift;
        r |= ((c >> (src.blueShift + src.blueBits - dst.blueBits)) & ((1u<<dst.blueBits)-1)) << dst.blueShift;
    }

    if (src.hasAlpha())
    {
        if (dst.hasAlpha())
            r |= ((c >> (src.alphaShift + src.alphaBits - dst.alphaBits)) & ((1u<<dst.alphaBits)-1)) << dst.alphaShift;
        else
        {
            // Make sure that the alpha is applied to the color if doing only a shift conversion.
            RI_ASSERT(src.isPremultiplied() == dst.isPremultiplied());
        }
    }

    return r;
}

RI_INLINE RIuint32 Color::Descriptor::toIndex() const
{
    SmallDescriptor smallDesc;
    toSmallDescriptor(smallDesc);
    return smallDesc.toUint32();
}

RI_INLINE Color operator*(const Color& c, RIfloat f)			{ return Color(c.r*f, c.g*f, c.b*f, c.a*f, c.getInternalFormat()); }
RI_INLINE Color operator*(RIfloat f, const Color& c)			{ return Color(c.r*f, c.g*f, c.b*f, c.a*f, c.getInternalFormat()); }
RI_INLINE Color operator+(const Color& c0, const Color& c1)		{ RI_ASSERT(c0.getInternalFormat() == c1.getInternalFormat()); return Color(c0.r+c1.r, c0.g+c1.g, c0.b+c1.b, c0.a+c1.a, c0.getInternalFormat()); }
RI_INLINE Color operator-(const Color& c0, const Color& c1)		{ RI_ASSERT(c0.getInternalFormat() == c1.getInternalFormat()); return Color(c0.r-c1.r, c0.g-c1.g, c0.b-c1.b, c0.a-c1.a, c0.getInternalFormat()); }
RI_INLINE void  Color::assertConsistency() const
{
    RI_ASSERT(r >= 0.0f && r <= 1.0f);
    RI_ASSERT(g >= 0.0f && g <= 1.0f);
    RI_ASSERT(b >= 0.0f && b <= 1.0f);
    RI_ASSERT(a >= 0.0f && a <= 1.0f);
    RI_ASSERT(!isPremultiplied() || (r <= a && g <= a && b <= a));	//premultiplied colors must have color channels less than or equal to alpha
    RI_ASSERT((isLuminance() && r == g && r == b) || !isLuminance());	//if luminance, r=g=b
}

class IntegerColor
{
public:

    IntegerColor() {r = g = b = a = 0;}
    IntegerColor(const Color& color);

    RI_INLINE           IntegerColor(RIuint32 packedColor, const Color::Descriptor& desc) { fromPackedColor(packedColor, desc); }
    RI_INLINE           IntegerColor(RIuint32 cr, RIuint32 cg, RIuint32 cb, RIuint32 ca) { r = cr; g = cg; b = cb; a = ca; }
    RI_INLINE void      asFixedPoint(const Color& color);
    RI_INLINE void      fromPackedColor(RIuint32 packedColor, const Color::Descriptor& desc);
    RI_INLINE void      expandColor(const Color::Descriptor& desc);
    RI_INLINE void      truncateColor(const Color::Descriptor& desc);
    RI_INLINE void      clampToAlpha();
    RI_INLINE RIuint32  getPackedColor(const Color::Descriptor& desc) const;
    RI_INLINE RIuint32  getPackedMaskColor(const Color::Descriptor& desc) const;
    RI_INLINE void      premultiply(bool luminance = false);
    RI_INLINE void      unpremultiply(bool luminance = false);
    //RI_INLINE void      linearToGamma(bool luminance, bool premultipliedIn, bool premultipliedOut);
    RI_INLINE void      linearToGamma(bool luminance = false);
    RI_INLINE void      gammaToLinear(bool luminance = false);
    RI_INLINE void      fromPackedMask(RIuint32 packedColor, const Color::Descriptor& desc);
    RI_INLINE void      expandMask(const Color::Descriptor& desc);
    RI_INLINE void      truncateMask(const Color::Descriptor& desc);
    RI_INLINE void      fullLuminanceToRGB(bool premultipliedIn, bool gammaIn, bool premultipliedOut, bool gammaOut);
    RI_INLINE void      fullRGBToLuminance(bool premultipliedIn, bool gammaIn, bool premultipliedOut, bool gammaOut);
    RI_INLINE void      luminanceToRGB();
    RI_INLINE void      rgbToLuminance();
    RI_INLINE void      convertToFrom(const Color::Descriptor& dst, const Color::Descriptor& src, bool srcIsMask);

    RI_INLINE static IntegerColor linearBlendNS(const IntegerColor& c0, const IntegerColor& c1, int k);

    RIuint32 r;
    RIuint32 g;
    RIuint32 b;
    RIuint32 a;

};

/**
 * \brief   Blend two colors linearly. The output will not be scaled into original range.
 * \param   k   Blend coefficient. Must be [0..255] for correct results.
 * \todo    Parameterize against bits in k? To perform well, that setup must be compiled rt.
 */
RI_INLINE IntegerColor IntegerColor::linearBlendNS(const IntegerColor& c0, const IntegerColor& c1, int k)
{
    RI_ASSERT(k >= 0 && k <= 255);
    IntegerColor ret;
    RIuint32 ik = 255 - k;

    ret.r = ik * c0.r + k * c1.r;
    ret.g = ik * c0.g + k * c1.g;
    ret.b = ik * c0.b + k * c1.b;
    ret.a = ik * c0.a + k * c1.a;

    return ret;
}

/**
 *	\note 	Assumes that each individual component is in proper range (usually indicated by the
 *			corresponding shift).
 */
RI_INLINE RIuint32 packRGBAInteger(RIuint32 cr, int rs, RIuint32 cg, int gs, RIuint32 cb, int bs, RIuint32 ca, int as)
{
    return (cr << rs) | (cg << gs) | (cb << bs) | (ca << as);
}

/**
 * \brief   Packs a color into RIuint32.
 * \note    The color must have been truncated to contain correct amount of bits per channel
 * \note    This function is efficient only if runtime compilation is used.
 */
RI_INLINE RIuint32 IntegerColor::getPackedColor(const Color::Descriptor& desc) const
{
    RIuint32 res = 0;
    if (desc.luminanceBits)
    {
        RI_ASSERT(desc.redBits == 0 && desc.greenBits == 0 && desc.blueBits == 0);
        RI_ASSERT(r < (1u<<desc.luminanceBits));
        res = r << desc.luminanceShift;
    }
    else if (desc.redBits)
    {
        RI_ASSERT(r < (1u<<desc.redBits));
        res = r << desc.redShift;
        if (desc.greenBits)
        {
            RI_ASSERT(desc.blueBits);
            RI_ASSERT(g < (1u<<desc.greenBits));
            RI_ASSERT(b < (1u<<desc.blueBits));
            res |= g << desc.greenShift;
            res |= b << desc.blueShift;
        }
    }

    if (desc.alphaBits)
    {
        RI_ASSERT(a < (1u<<desc.alphaBits));
        res |= a << desc.alphaShift;
    }

    return res;
}

RI_INLINE RIuint32 IntegerColor::getPackedMaskColor(const Color::Descriptor& desc) const
{
    if (desc.alphaBits)
        return packRGBAInteger(0, desc.redShift, 0, desc.greenShift, 0, desc.blueShift, a, desc.alphaShift);
    else if(desc.redBits)
        return packRGBAInteger(a, desc.redShift, 0, desc.greenShift, 0, desc.blueShift, 0, desc.alphaShift);
    else
    {
        RI_ASSERT(desc.luminanceBits);
        return packRGBAInteger(a, desc.luminanceBits, 0, desc.greenShift, 0, desc.blueShift, 0, desc.alphaShift);
    }

}

RI_INLINE void IntegerColor::premultiply(bool luminance)
{
    // \todo Check the round!!!
    RIuint32 fxa = a + (a>>7);
    r = (r * fxa); r = (r + (1<<7))>>8;

    if (!luminance)
    {
        g = (g * fxa); g = (g + (1<<7))>>8;
        b = (b * fxa); b = (b + (1<<7))>>8;
    }
}

RI_INLINE void IntegerColor::unpremultiply(bool luminance)
{
    RI_ASSERT(a <= 255);

    RIuint32 rcp = sc_alphaRcp[a];
    r = (r * rcp) >> 8;

    if (!luminance)
    {
        g = (g * rcp) >> 8;
        b = (b * rcp) >> 8;
    }
}

RI_INLINE void IntegerColor::linearToGamma(bool luminance)
{
    RI_ASSERT(r <= 255 && g <= 255 && b <= 255 && a <= 255);

    r = sc_lRGB_to_sRGB[r];

    if (!luminance)
    {
        g = sc_lRGB_to_sRGB[g];
        b = sc_lRGB_to_sRGB[b];
    }

    // \note Alpha is _not_ converted and it must be considered linear always
}

RI_INLINE void IntegerColor::gammaToLinear(bool luminance)
{
    RI_ASSERT(r <= 255 && g <= 255 && b <= 255 && a <= 255);

    r = sc_sRGB_to_lRGB[r];
    if (!luminance)
    {
        g = sc_sRGB_to_lRGB[g];
        b = sc_sRGB_to_lRGB[b];
    }

    // \note Alpha is _not_ converted and it must be considered linear always
}

RI_INLINE void IntegerColor::asFixedPoint(const Color& color)
{
    r = (RIuint32)(color.r * 256.0f + 0.5f);
    g = (RIuint32)(color.g * 256.0f + 0.5f);
    b = (RIuint32)(color.b * 256.0f + 0.5f);
    a = (RIuint32)(color.a * 256.0f + 0.5f);
}

RI_INLINE void IntegerColor::fromPackedColor(RIuint32 packedColor, const Color::Descriptor& desc)
{
    /* \note Expand MUST be done separately! */

    if (desc.luminanceBits)
    {
        r = (packedColor >> desc.luminanceShift) & ((1u << desc.luminanceBits)-1);
        g = b = r;
    }
    else
    {
        r = (packedColor >> desc.redShift) & ((1u << desc.redBits)-1);
        g = (packedColor >> desc.greenShift) & ((1u << desc.greenBits)-1);
        b = (packedColor >> desc.blueShift) & ((1u << desc.blueBits)-1);
    }

    if (desc.alphaBits)
        a = (packedColor >> desc.alphaShift) & ((1u << desc.alphaBits)-1);
    else
        a = 255;
}

/**
 * \brief   Expand color to larger (or same) bit depth as in the OpenVG specification.
 * \todo    1 and 2 bpp!
 */
RI_INLINE RIuint32 expandComponent(RIuint32 c, RIuint32 srcBits)
{
    const RIuint32 destBits = 8;
    RI_ASSERT(destBits >= srcBits);

    if (!srcBits) return 0;

    if (srcBits == destBits) return c;

    switch (srcBits)
    {
    case 6:
        return (c << 2) | (c >> 4);
    case 5:
        return (c << 3) | (c >> 2);
    case 4:
        return (c << 4) | c;
    case 2:
        return c | (c << 2) | (c << 4) | (c << 6);
    default:
        RI_ASSERT(srcBits == 1);
        if (c) return 0xff;
        return 0;
    }
}

/**
 * \brief   Expands integer color representation to internal format (8-bits per component atm.).
 * \todo    Do nothing when bits == 8.
 */
RI_INLINE void IntegerColor::expandColor(const Color::Descriptor& desc)
{
    if (desc.luminanceBits)
    {
        r = expandComponent(r, desc.luminanceBits);
        g = b = r;
        a = 255;
    } else
    {
        if (desc.redBits < 8 || desc.luminanceBits < 8)
            r = expandComponent(r, desc.redBits);
        if (desc.greenBits < 8)
            g = expandComponent(g, desc.greenBits);
        if (desc.blueBits < 8)
            b = expandComponent(b, desc.blueBits);
    }

    if (desc.alphaBits && desc.alphaBits < 8)
        a = expandComponent(a, desc.alphaBits);

    if (desc.isAlphaOnly())
    {
        if (!desc.isPremultiplied())
            r = g = b = 255;
        else
            r = g = b = a;
    }
}

/**
 * \brief   Convert IntegerColor components to destination bitdepth (from internal) by
 *          shifting. Rounding does not take place.
 */
RI_INLINE void IntegerColor::truncateColor(const Color::Descriptor& desc)
{
    if (desc.luminanceBits)
    {
        RI_ASSERT(desc.redBits == 0 && desc.greenBits == 0 && desc.blueBits == 0);
        if (desc.luminanceBits == 1)
        {
            // Round the 1-bit case a bit better?
            r = (r + 128)>>8;
        } else if (desc.luminanceBits < 8)
            r >>= (8 - desc.luminanceBits);
    }
    else
    {
        if (desc.redBits < 8)
            r >>= (8 - desc.redBits);
        if (desc.greenBits < 8)
            g >>= (8 - desc.greenBits);
        if (desc.blueBits < 8)
            b >>= (8 - desc.blueBits);
    }

    if (desc.alphaBits < 8)
    {
        if (desc.alphaBits == 1)
            a = (a+128)>>8;
        else
            a >>= (8 - desc.alphaBits);
    }
}

RI_INLINE void IntegerColor::truncateMask(const Color::Descriptor& desc)
{
    if (desc.redBits < 8 || desc.luminanceBits < 8)
        r >>= (8 - desc.maskBits);
    if (desc.greenBits < 8)
        g >>= (8 - desc.maskBits);
    if (desc.blueBits < 8)
        b >>= (8 - desc.maskBits);
    if (desc.alphaBits < 8)
        a >>= (8 - desc.maskBits);
}

RI_INLINE void IntegerColor::clampToAlpha()
{
    if (r > a) r = a;
    if (g > a) g = a;
    if (b > a) b = a;
}

RI_INLINE void IntegerColor::fromPackedMask(RIuint32 packedMask, const Color::Descriptor& desc)
{
    RI_ASSERT(desc.maskBits);
    a = (packedMask >> desc.maskShift) & ((1u << desc.maskBits)-1);
}

RI_INLINE void IntegerColor::expandMask(const Color::Descriptor& desc)
{
    a = expandComponent(a, desc.maskBits);
    r = g = b = a;
}

#if 0
RI_INLINE void IntegerColor::truncateMask(const Color::Descriptor& desc)
{
    a >>= (8 - desc.maskBits);
}
#endif

RI_INLINE void IntegerColor::fullLuminanceToRGB(bool premultipliedIn, bool gammaIn, bool premultipliedOut, bool gammaOut)
{
    if (premultipliedIn)
        unpremultiply();

    luminanceToRGB();

    if (gammaIn != gammaOut)
    {
        if (gammaIn)
            gammaToLinear();
        else
            linearToGamma();
    }

    if (premultipliedOut)
        premultiply();

}

RI_INLINE void IntegerColor::fullRGBToLuminance(bool premultipliedIn, bool gammaIn, bool premultipliedOut, bool gammaOut)
{
    if (premultipliedIn)
        unpremultiply();

    if (gammaIn)
        gammaToLinear();

    rgbToLuminance();

    if (gammaOut)
        linearToGamma();

    if (premultipliedOut)
        premultiply();

}


// \todo This should not be needed (only r-channel is used anyway)
RI_INLINE void IntegerColor::luminanceToRGB()
{
    g = b = r;
}

// \todo Only write to R!
RI_INLINE void IntegerColor::rgbToLuminance()
{
    enum { Rx = 871, Gx = 2929, Bx = 296, Bits = 12 };
    //enum { Rx = 54, Gx = 183, Bx = 18, Bits = 8 };
    RIuint32 l = Rx * r + Gx * g + Bx * b;
    r = g = b = l >> Bits;
}

#if 0
RI_INLINE void IntegerColor::convertFromInternal(const Color::Descriptor& dst)
{
}
#endif

/**
 * \brief   Convert color from one format to another using integer operations.
 * \note    Currently expands the color to intermediate format first (8 bits
 *          per component.
 */
RI_INLINE void IntegerColor::convertToFrom(const Color::Descriptor& dst, const Color::Descriptor& src, bool srcIsMask)
{
    if (src.isZeroConversion(dst))
        return;

    if (src.isShiftConversionToLower(dst))
    {
        if (dst.luminanceBits)
        {
            if (dst.luminanceBits == 1)
            {
                RI_ASSERT(src.luminanceBits == 8);
                r = (r + 128)>>8;
            }
            else
                r = r >> (src.luminanceBits - dst.luminanceBits);
        } else
        {
            r = r >> (src.redBits - dst.redBits);
            g = g >> (src.greenBits - dst.greenBits);
            b = b >> (src.blueBits - dst.blueBits);
        }
        if (dst.alphaBits)
        {
                //a = (a+128)>>8;
            if (dst.alphaBits == 1)
                a = (a+(1<<(src.alphaBits-1)))>>src.alphaBits;
            else
                a = a >> (src.alphaBits - dst.alphaBits);
        }

        return;
    }

    if (!srcIsMask)
        expandColor(src);
    else
        expandMask(src);


    if (dst.isLuminance() != src.isLuminance())
    {
        if (src.isLuminance())
            fullLuminanceToRGB(src.isPremultiplied(), src.isNonlinear(), dst.isPremultiplied(), dst.isNonlinear());
        else
            fullRGBToLuminance(src.isPremultiplied(), src.isNonlinear(), dst.isPremultiplied(), dst.isNonlinear());
    }
    else if (dst.isNonlinear() != src.isNonlinear())
    {
        // No luminance/rgb change.
        // Change of gamma requires unpremultiplication:
        if (src.isPremultiplied() && !(src.isAlphaOnly()))
            unpremultiply();

        if (src.isNonlinear())
            gammaToLinear(src.isLuminance());
        else
            linearToGamma(src.isLuminance());

        if (dst.isPremultiplied() && !(dst.isAlphaOnly()))
            premultiply();
    }
    else
    if ((dst.isPremultiplied() != src.isPremultiplied()) && !(dst.isAlphaOnly() || dst.isAlphaOnly()))
    {
        // \todo Make sure non-alpha formats are properly handled.
        if (src.isPremultiplied())
            unpremultiply(dst.isLuminance());
        else
            premultiply(dst.isLuminance());
    }

    truncateColor(dst);
}

//==============================================================================================

/*-------------------------------------------------------------------*//*!
* \brief	Storage and operations for VGImage.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

class Surface;
class Image
{
public:
    Image(const Color::Descriptor& desc, int width, int height, VGbitfield allowedQuality);	//throws bad_alloc
    //use data from a memory buffer. NOTE: data is not copied, so it is user's responsibility to make sure the data remains valid while the Image is in use.
    Image(const Color::Descriptor& desc, int width, int height, int stride, RIuint8* data);	//throws bad_alloc
    //child image constructor
    Image(Image* parent, int x, int y, int width, int height);	//throws bad_alloc
    ~Image();

    const Color::Descriptor&	getDescriptor() const		{ return m_desc; }
    int					getWidth() const					{ return m_width; }
    int					getHeight() const					{ return m_height; }
    int					getStride() const					{ return m_stride; }
    Image*				getParent() const					{ return m_parent; }
    VGbitfield			getAllowedQuality() const			{ return m_allowedQuality; }
    void				addInUse()							{ m_inUse++; }
    void				removeInUse()						{ RI_ASSERT(m_inUse > 0); m_inUse--; }
    int					isInUse() const						{ return m_inUse; }
    RIuint8*			getData() const						{ return m_data; }
    void				addReference()						{ m_referenceCount++; }
    int					removeReference()					{ m_referenceCount--; RI_ASSERT(m_referenceCount >= 0); return m_referenceCount; }
    bool				overlaps(const Image* src) const;
    void                setUnsafe(bool unsafe) { if (unsafe && m_desc.maybeUnsafe()) m_unsafeData = unsafe; else m_unsafeData = false; }
    bool                isUnsafe() const { return m_unsafeData; }

    void				clear(const Color& clearColor, int x, int y, int w, int h);
    void				blit(VGContext* context, const Image* src, int sx, int sy, int dx, int dy, int w, int h, Array<Rectangle>* scissors = NULL, bool dither = false);	//throws bad_alloc

    RI_INLINE static const void* incrementPointer(const void* ptr, int bpp, RIint32 x);
    RI_INLINE static void* calculateAddress(const void* basePtr, int bpp, int x, int y, int stride);

    static RI_INLINE RIuint32   readPackedPixelFromAddress(const void *ptr, int bpp, int x);
    static RI_INLINE void       writePackedPixelToAddress(void* ptr, int bpp, int x, RIuint32 packedColor);

    RI_INLINE RIuint32 			readPackedPixel(int x, int y) const;
    Color				readPixel(int x, int y) const;
    RI_INLINE void      writePackedPixelToAddress(void* ptr, int x, RIuint32 packedColor);
    void				writePackedPixel(int x, int y, RIuint32 packedColor);
    void				writePixel(int x, int y, const Color& c);

    void                fillPacked(RIuint32 packedColor);

    static RI_INLINE void   fillPackedPixels(void* data, int bpp, int x, int y, int stride, int nPixels, RIuint32 packedColor);
    RI_INLINE void		    fillPackedPixels(int x, int y, int nPixels, RIuint32 packedColor);
    RI_INLINE void          fillPackedRectangle(int x0, int y0, int width, int height, RIuint32 packedColor);

    void				writeFilteredPixel(int x, int y, const Color& c, VGbitfield channelMask);

    RIfloat				readMaskPixel(int x, int y) const;		//can read any image format
    void				writeMaskPixel(int x, int y, RIfloat m);	//can write only to VG_A_x

    Color				resample(RIfloat x, RIfloat y, const Matrix3x3& surfaceToImage, VGImageQuality quality, VGTilingMode tilingMode, const Color& tileFillColor);	//throws bad_alloc
    void				makeMipMaps();	//throws bad_alloc

    void				colorMatrix(const Image& src, const RIfloat* matrix, bool filterFormatLinear, bool filterFormatPremultiplied, VGbitfield channelMask);
    void				convolve(const Image& src, int kernelWidth, int kernelHeight, int shiftX, int shiftY, const RIint16* kernel, RIfloat scale, RIfloat bias, VGTilingMode tilingMode, const Color& edgeFillColor, bool filterFormatLinear, bool filterFormatPremultiplied, VGbitfield channelMask);
    void				separableConvolve(const Image& src, int kernelWidth, int kernelHeight, int shiftX, int shiftY, const RIint16* kernelX, const RIint16* kernelY, RIfloat scale, RIfloat bias, VGTilingMode tilingMode, const Color& edgeFillColor, bool filterFormatLinear, bool filterFormatPremultiplied, VGbitfield channelMask);
    void				gaussianBlur(const Image& src, RIfloat stdDeviationX, RIfloat stdDeviationY, VGTilingMode tilingMode, const Color& edgeFillColor, bool filterFormatLinear, bool filterFormatPremultiplied, VGbitfield channelMask);
    void				lookup(const Image& src, const RIuint8 * redLUT, const RIuint8 * greenLUT, const RIuint8 * blueLUT, const RIuint8 * alphaLUT, bool outputLinear, bool outputPremultiplied, bool filterFormatLinear, bool filterFormatPremultiplied, VGbitfield channelMask);
    void				lookupSingle(const Image& src, const RIuint32 * lookupTable, VGImageChannel sourceChannel, bool outputLinear, bool outputPremultiplied, bool filterFormatLinear, bool filterFormatPremultiplied, VGbitfield channelMask);

    RI_INLINE static int descriptorToStride(const Color::Descriptor& desc, int width) { return (width*desc.bitsPerPixel+7)/8; };

    void getStorageOffset(int& x, int& y) const { x = m_storageOffsetX; y = m_storageOffsetY; }

private:
    Image(const Image&);					//!< Not allowed.
    void operator=(const Image&);			//!< Not allowed.

#if defined(RI_DEBUG)
    bool                ptrInImage(const void* ptr) const;
#endif
    Color				readTexel(int u, int v, int level, VGTilingMode tilingMode, const Color& tileFillColor) const;

    Color::Descriptor	m_desc;
    int					m_width;
    int					m_height;
    VGbitfield			m_allowedQuality;
    int					m_inUse;
    int					m_stride;
    RIuint8*			m_data;
    int					m_referenceCount;
    bool				m_ownsData;
    Image*				m_parent;
    int					m_storageOffsetX;
    int					m_storageOffsetY;
    bool                m_unsafeData; // Data may contain incorrect pixel data

#ifndef RI_COMPILE_LLVM_BYTECODE

#endif /* RI_COMPILE_LLVM_BYTECODE */
};

#if defined(RI_DEBUG)
RI_INLINE bool Image::ptrInImage(const void* ptr) const
{
    RIuint8* p = (RIuint8*)ptr;

    if (p < m_data) return false;
    if (p >= (m_data + m_height * m_stride)) return false;
    return true;
}
#endif

RI_INLINE const void* Image::incrementPointer(const void* ptr, int bpp, int x)
{
    if (bpp >= 8)
        return (((RIuint8*)ptr) + (bpp >> 3));
    // Increment the pointer only when the byte is actually about to change.
    int mask;
    if (bpp == 4)
        mask = 1;
    else if (bpp == 2)
        mask = 3;
    else
        mask = 7;
    if ((x & mask) == mask)
        return ((RIuint8*)ptr + 1);
    return ptr;
}

RI_INLINE void* Image::calculateAddress(const void* basePtr, int bpp, int x, int y, int stride)
{
    if (bpp >= 8)
    {
        return (void*)((RIuint8*)basePtr + y * stride + x * (bpp >> 3));
    } else
    {
        // 4, 2, or 1 bits per pixel
        RI_ASSERT(bpp == 4 || bpp == 2 || bpp == 1);
        return (void*)((RIuint8*)basePtr + y * stride + ((x * bpp) >> 3));
    }
}

RI_INLINE RIuint32 Image::readPackedPixel(int x, int y) const
{
    RI_ASSERT(m_data);
    RI_ASSERT(x >= 0 && x < m_width);
    RI_ASSERT(y >= 0 && y < m_height);
    RI_ASSERT(m_referenceCount > 0);

    RIuint32 p = 0;

    void* ptr = Image::calculateAddress(m_data, m_desc.bitsPerPixel, x+m_storageOffsetX, y+m_storageOffsetY, m_stride);
    p = readPackedPixelFromAddress(ptr, m_desc.bitsPerPixel, x+m_storageOffsetX);

    return p;
}


RI_INLINE void Image::writePackedPixelToAddress(void* ptr, int bpp, int x, RIuint32 packedColor)
{
    // \note packedColor must contain the whole data (including < 8 bpp data)?
    switch(bpp)
    {
    case 32:
    {
        RIuint32* s = ((RIuint32*)ptr);
        *s = (RIuint32)packedColor;
        break;
    }

    case 16:
    {
        RIuint16* s = ((RIuint16*)ptr);
        *s = (RIuint16)packedColor;
        break;
    }

    case 8:
    {
        RIuint8* s = ((RIuint8*)ptr);
        *s = (RIuint8)packedColor;
        break;
    }
    case 4:
    {
        RIuint8* s = ((RIuint8*)ptr);
        *s = (RIuint8)((packedColor << ((x&1)<<2)) | ((unsigned int)*s & ~(0xf << ((x&1)<<2))));
        break;
    }

    case 2:
    {
        RIuint8* s = ((RIuint8*)ptr);
        *s = (RIuint8)((packedColor << ((x&3)<<1)) | ((unsigned int)*s & ~(0x3 << ((x&3)<<1))));
        break;
    }

    default:
    {
        RI_ASSERT(bpp == 1);
        RIuint8* s = ((RIuint8*)ptr);
        *s = (RIuint8)((packedColor << (x&7)) | ((unsigned int)*s & ~(0x1 << (x&7))));
        break;
    }
    }
    // m_mipmapsValid = false; // \note Will never do this, must be handled outside this class somehow!
}

/**
 * \brief   Write packed pixel into address.
 * \param   x   Which x-coordinate (starting from the start of the scanline
 *              pointed to) is addressed? This is only required for formats
 *              that have less than 8 bpp.
 */
void Image::writePackedPixelToAddress(void* address, int x, RIuint32 packedColor)
{
    writePackedPixelToAddress(address, m_desc.bitsPerPixel, x, packedColor);
}

/**
 * \brief   Read a packed pixel from a given address. Notice the use of param x!
 * \param   x   Check which part of byte to return if bpp < 8
 */
RI_INLINE RIuint32 Image::readPackedPixelFromAddress(const void *ptr, int bpp, int x)
{
    switch(bpp)
    {
    case 32:
    {
        RIuint32* s = (RIuint32*)ptr;
        return *s;
    }

    case 16:
    {
        RIuint16* s = (RIuint16*)ptr;
        return (RIuint32)*s;
    }

    case 8:
    {
        RIuint8* s = (RIuint8*)ptr;
        return (RIuint32)*s;
    }
    case 4:
    {
        RIuint8* s = ((RIuint8*)ptr);
        return (RIuint32)(*s >> ((x&1)<<2)) & 0xf;
    }

    case 2:
    {
        RIuint8* s = ((RIuint8*)ptr);
        return (RIuint32)(*s >> ((x&3)<<1)) & 0x3;
    }

    default:
    {
        RI_ASSERT(bpp == 1);
        RIuint8* s = ((RIuint8*)ptr);
        return (RIuint32)(*s >> (x&7)) & 0x1;
    }
    }
}

RI_INLINE void Image::writePackedPixel(int x, int y, RIuint32 packedColor)
{
    RI_ASSERT(m_data);
    RI_ASSERT(x >= 0 && x < m_width);
    RI_ASSERT(y >= 0 && y < m_height);
    RI_ASSERT(m_referenceCount > 0);

    x += m_storageOffsetX;
    y += m_storageOffsetY;

    RIuint8* scanline = m_data + y * m_stride;
    switch(m_desc.bitsPerPixel)
    {
    case 32:
    {
        RIuint32* s = ((RIuint32*)scanline) + x;
        *s = (RIuint32)packedColor;
        break;
    }

    case 16:
    {
        RIuint16* s = ((RIuint16*)scanline) + x;
        *s = (RIuint16)packedColor;
        break;
    }

    case 8:
    {
        RIuint8* s = ((RIuint8*)scanline) + x;
        *s = (RIuint8)packedColor;
        break;
    }
    case 4:
    {
        RIuint8* s = ((RIuint8*)scanline) + (x>>1);
        *s = (RIuint8)((packedColor << ((x&1)<<2)) | ((unsigned int)*s & ~(0xf << ((x&1)<<2))));
        break;
    }

    case 2:
    {
        RIuint8* s = ((RIuint8*)scanline) + (x>>2);
        *s = (RIuint8)((packedColor << ((x&3)<<1)) | ((unsigned int)*s & ~(0x3 << ((x&3)<<1))));
        break;
    }

    default:
    {
        RI_ASSERT(m_desc.bitsPerPixel == 1);
        RIuint8* s = ((RIuint8*)scanline) + (x>>3);
        *s = (RIuint8)((packedColor << (x&7)) | ((unsigned int)*s & ~(0x1 << (x&7))));
        break;
    }
    }
    //m_mipmapsValid = false;
}


/**
 * \brief   Unsafe static method for setting image pixels
 */
RI_INLINE void Image::fillPackedPixels(void* data, int bpp, int x, int y, int stride, int nPixels, RIuint32 packedColor)
{
    RI_ASSERT(nPixels > 0);
    RI_ASSERT(data);

    RIuint8* scanline = (RIuint8*)data + y * stride;

    switch(bpp)
    {
    case 32:
    {
        RIuint32* s = ((RIuint32*)scanline) + x;

        for (int i = 0; i < nPixels; i++)
            s[i] = packedColor;

        break;
    }

    case 16:
    {
        RIuint16* s = ((RIuint16*)scanline) + x;

        for (int i = 0; i < nPixels; i++)
            s[i] = (RIuint16)packedColor;

        break;
    }

    case 8:
    {
        RIuint8* s = ((RIuint8*)scanline) + x;

        for (int i = 0; i < nPixels; i++)
            s[i] = (RIuint8)packedColor;

        break;
    }
    case 4:
    {
        //RI_ASSERT((packedColor & 0xf) == 0);
        //packedColor &= 0xf;
        RIuint8* s = ((RIuint8*)scanline) + (x>>1);
        if (x & 1)
        {
            *s = (RIuint8)((packedColor << ((x&1)<<2)) | ((unsigned int)*s & ~(0xf << ((x&1)<<2))));
            s++;
            x++;
            nPixels--;
        }
        RI_ASSERT(!(x&1));

        int c = nPixels / 2;
        RIuint8 bytePacked = packedColor | (packedColor << 4);
        while (c)
        {
            *s++ = bytePacked;
            c--;
            x+=2;
        }
        nPixels &= 1;

        if (nPixels)
        {
            *s = (RIuint8)((packedColor << ((x&1)<<2)) | ((unsigned int)*s & ~(0xf << ((x&1)<<2))));
            s++;
            x++;
            nPixels--;
        }
        RI_ASSERT(nPixels == 0);
        break;
    }

    case 2:
    {
        // This case should not be needed!
        RI_ASSERT(false);
        RIuint8* s = ((RIuint8*)scanline) + (x>>2);
        *s = (RIuint8)((packedColor << ((x&3)<<1)) | ((unsigned int)*s & ~(0x3 << ((x&3)<<1))));
        break;
    }

    default:
    {
        RI_ASSERT(bpp == 1);
        RIuint8* s = ((RIuint8*)scanline) + (x>>3);
        // \todo Get this as input instead?
        RI_ASSERT(packedColor == 1 || packedColor == 0);
        RIuint8 fullyPacked = (RIuint8)(-(RIint8)packedColor);

        if (x & 7)
        {
            // Handle the first byte:
            RIuint8 o = *s;
            int a = x&7;
            RI_ASSERT(a>=1);
            int b = RI_INT_MIN(a + nPixels, 8);
            RI_ASSERT(b > a);
            RIuint8 emask = (1u << b)-1;
            RIuint8 mask = (0xffu<<a) & emask;
            RI_ASSERT(mask>0);
            RI_ASSERT(mask<=254);
            *s++ = (o&(~mask))|(fullyPacked & mask);
            nPixels -= 8-(x&7);
            x += 8-(x&7);
        }

        if (nPixels < 0)
            return;

        RI_ASSERT(!(x&1));

        int c = nPixels/8;
        while (c)
        {
            *s++ = fullyPacked;
            c--;
            x+=8;
        }
        nPixels -= ((nPixels/8) * 8);


        if (nPixels)
        {
            RI_ASSERT((x&7) == 0);

            RIuint8 o = *s;
            int b = nPixels;
            RI_ASSERT(b<=7);
            RIuint8 mask = (1u<<b)-1;
            RI_ASSERT(mask <= 127);
            *s++ = (o&(~mask))|(fullyPacked & mask);
        }
        break;
    }
    }
    //m_mipmapsValid = false;
}
RI_INLINE void Image::fillPackedPixels(int x, int y, int nPixels, RIuint32 packedColor)
{
    fillPackedPixels((void*)m_data, m_desc.bitsPerPixel, x + m_storageOffsetX, y + m_storageOffsetY, m_stride, nPixels, packedColor);
}

RI_INLINE void Image::fillPackedRectangle(int x0, int y0, int width, int height, RIuint32 packedColor)
{
    int y = y0;
    while (height)
    {
        fillPackedPixels(x0, y, width, packedColor);
        y++;
        height--;
    }
}

/*-------------------------------------------------------------------*//*!
* \brief	Surface class abstracting multisampled rendering surface.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

class Surface
{
public:
    Surface(const Color::Descriptor& desc, int width, int height, int numSamples);	//throws bad_alloc
    Surface(Image* image);	//throws bad_alloc
    Surface(const Color::Descriptor& desc, int width, int height, int stride, RIuint8* data);	//throws bad_alloc
    ~Surface();

    RI_INLINE const Image* getImage() const {return m_image;}
    RI_INLINE const Color::Descriptor&	getDescriptor() const		{ return m_image->getDescriptor(); }
    RI_INLINE int		getWidth() const							{ return m_width; }
    RI_INLINE int		getHeight() const							{ return m_height; }
    RI_INLINE int		getNumSamples() const						{ return m_numSamples; }
    RI_INLINE void		addReference()								{ m_referenceCount++; }
    RI_INLINE int		removeReference()							{ m_referenceCount--; RI_ASSERT(m_referenceCount >= 0); return m_referenceCount; }
    RI_INLINE int		isInUse() const								{ return m_image->isInUse(); }
    RI_INLINE bool		isInUse(Image* image) const					{ return image == m_image ? true : false; }

    void				clear(const Color& clearColor, int x, int y, int w, int h, const Array<Rectangle>* scissors = NULL);
#if 0
    // Currently does not support msaa surfaces
    void				blit(const Image& src, int sx, int sy, int dx, int dy, int w, int h);	//throws bad_alloc
    void				blit(const Image& src, int sx, int sy, int dx, int dy, int w, int h, const Array<Rectangle>& scissors);	//throws bad_alloc
    void				blit(const Surface* src, int sx, int sy, int dx, int dy, int w, int h);	//throws bad_alloc
    void				blit(const Surface* src, int sx, int sy, int dx, int dy, int w, int h, const Array<Rectangle>& scissors);	//throws bad_alloc
#endif
    void				mask(DynamicBlitter& blitter, const Image* src, VGMaskOperation operation, int x, int y, int w, int h);

    RI_INLINE void      writePackedPixelToAddress(void* address, int x, RIuint32 p)        { m_image->writePackedPixelToAddress(address, x, p); }
   RI_INLINE RIuint32 	readPackedSample(int x, int y, int sample) const			 { return m_image->readPackedPixel(x*m_numSamples+sample, y); }
    RI_INLINE Color		readSample(int x, int y, int sample) const                   { return m_image->readPixel(x*m_numSamples+sample, y); }
    RI_INLINE void		writePackedSample(int x, int y, int sample, RIuint32 p)		 { m_image->writePackedPixel(x*m_numSamples+sample, y, p); }
    RI_INLINE void		writeSample(int x, int y, int sample, const Color& c)        { m_image->writePixel(x*m_numSamples+sample, y, c); }
    RI_INLINE void		fillPackedSamples(int x, int y, int nPixels, RIuint32 p);

    RIfloat				readMaskCoverage(int x, int y) const;
    void				writeMaskCoverage(int x, int y, RIfloat m);
    unsigned int		readMaskMSAA(int x, int y) const;
    void				writeMaskMSAA(int x, int y, unsigned int m);

    RIuint32 			FSAAResolvePacked(int x, int y) const;
    Color				FSAAResolve(int x, int y) const;	//for fb=>img: vgGetPixels, vgReadPixels

private:
    Surface(const Surface&);			//!< Not allowed.
    void operator=(const Surface&);			//!< Not allowed.

    struct ScissorEdge
    {
        ScissorEdge() : x(0), miny(0), maxy(0), direction(0) {}
        bool operator<(const ScissorEdge& e) const	{ return x < e.x; }
        int			x;
        int			miny;
        int			maxy;
        int			direction;		//1 start, -1 end
    };

    int				m_width;
    int				m_height;
    int				m_numSamples;
    int				m_referenceCount;

public:
    // \todo TERO: Broke the design of this by making it public, make proper
    // friend/etc. C++ accessor for optimized pixel-pipelines. Combine with the
    // removal of (remnants of) the FSAA support.
    Image*			m_image;
};

RI_INLINE void Surface::fillPackedSamples(int x, int y, int nPixels, RIuint32 p)
{
    m_image->fillPackedPixels(x, y, nPixels, p);
}


/*-------------------------------------------------------------------*//*!
* \brief	Drawable class for encapsulating color and mask buffers.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

class Drawable
{
public:
    Drawable(const Color::Descriptor& desc, int width, int height, int numSamples, int maskBits);	//throws bad_alloc
    Drawable(Image* image, int maskBits);	//throws bad_alloc
    Drawable(const Color::Descriptor& desc, int width, int height, int stride, RIuint8* data, int maskBits);	//throws bad_alloc
    ~Drawable();

    RI_INLINE const Color::Descriptor&	getDescriptor() const		{ return m_color->getDescriptor(); }
    RI_INLINE int       getNumMaskBits() const                      { if(!m_mask) return 0; return m_mask->getDescriptor().alphaBits; }
    RI_INLINE int		getWidth() const							{ return m_color->getWidth(); }
    RI_INLINE int		getHeight() const							{ return m_color->getHeight(); }
    RI_INLINE int		getNumSamples() const						{ return m_color->getNumSamples(); }
    RI_INLINE void		addReference()								{ m_referenceCount++; }
    RI_INLINE int		removeReference()							{ m_referenceCount--; RI_ASSERT(m_referenceCount >= 0); return m_referenceCount; }
    RI_INLINE int		isInUse() const								{ return m_color->isInUse() || (m_mask && m_mask->isInUse()); }
    RI_INLINE bool		isInUse(Image* image) const					{ return m_color->isInUse(image) || (m_mask && m_mask->isInUse(image)); }
    RI_INLINE Surface*  getColorBuffer() const                      { return m_color; }
    RI_INLINE Surface*  getMaskBuffer() const                       { return m_mask; }

    void				resize(VGContext* context, int newWidth, int newHeight);	//throws bad_alloc
private:
    Drawable(const Drawable&);			//!< Not allowed.
    void operator=(const Drawable&);	//!< Not allowed.

    int                 m_referenceCount;
    Surface*			m_color;
    Surface*            m_mask;
};

//==============================================================================================

}	//namespace OpenVGRI

//==============================================================================================

#endif /* __RIIMAGE_H */
