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
 */

// This file contains the generated pixel-pipeline code and provides
// interface to compile and run them.

#ifndef __RIRASTERIZER_H
#   include "riRasterizer.h"
#endif

#ifndef __RIPIXELPIPE_H
#   include "riPixelPipe.h"
#endif

#ifndef __SFDYNAMICPIXELPIPE_H
#   include "sfDynamicPixelPipe.h"
#endif

#ifndef __RIUTILS_H
#   include "riUtils.h"
#endif

#ifndef __SFMASK_H
#   include "sfMask.h"
#endif

#ifndef __RIIMAGE_H
#   include "riImage.h"
#endif

#if defined(RI_DEBUG)
#   include <stdio.h>
#endif

namespace OpenVGRI {

RI_INLINE static bool alwaysLoadDst(const PixelPipe::SignatureState& state)
{
    if (!state.isRenderToMask)
    {
        if (state.hasImage)
            return true;

        VGBlendMode bm = state.blendMode;

        if (bm == VG_BLEND_SRC_IN ||
                bm == VG_BLEND_DST_OVER ||
                bm == VG_BLEND_DST_IN ||
                bm == VG_BLEND_ADDITIVE ||
                bm == VG_BLEND_MULTIPLY ||
                bm == VG_BLEND_SCREEN ||
                bm == VG_BLEND_DARKEN ||
                bm == VG_BLEND_LIGHTEN)
        {
            return true;
        } else
        {
            return false;
        }
    }
    else
    {
        switch (state.maskOperation)
        {
        case VG_SET_MASK:
            return false;
        default:
            return true;
        }
    }
}

RI_INLINE static bool canSolidFill(const PixelPipe::SignatureState& state)
{
    if (state.isRenderToMask)
    {
        if (state.maskOperation == VG_SET_MASK ||
            state.maskOperation == VG_UNION_MASK)
            return true;
        // \note SUBTRACT is also possible.
        return false;
    }

    if (state.paintType != VG_PAINT_TYPE_COLOR)
        return false;

    if (state.hasImage)
        return false;

    // Some blendmodes can use dst color even if coverage == 1.0
    if (state.blendMode != VG_BLEND_SRC && state.blendMode != VG_BLEND_SRC_OVER)
        return false;

    if (state.hasMasking)
        return false;

    if (state.fillColorTransparent)
        return false;

    if (state.hasColorTransform)
        return false; // \todo Trace solid color alpha -> 1.0

    return true;
}

RI_INLINE static int intReflectRepeat(int n, int bits)
{
    const int mask = (1<<bits)-1;
    return (n ^ (n << (31 - bits) >> 31)) & mask;
}

RI_INLINE static void applyGradientRepeat(int& sx0, int& sx1, PixelPipe::TilingMode sm)
{
    switch (sm)
    {
    case PixelPipe::TILING_MODE_PAD:
        sx0 = RI_INT_CLAMP(sx0, 0, PixelPipe::SAMPLE_MASK);
        sx1 = RI_INT_CLAMP(sx1, 0, PixelPipe::SAMPLE_MASK);
        break;
    case PixelPipe::TILING_MODE_REFLECT:
        sx0 = intReflectRepeat(sx0, PixelPipe::SAMPLE_BITS);
        sx1 = intReflectRepeat(sx1, PixelPipe::SAMPLE_BITS);
        break;
    default:
        RI_ASSERT(sm == PixelPipe::TILING_MODE_REPEAT);

        sx0 = sx0 & PixelPipe::SAMPLE_MASK;
        sx1 = sx1 & PixelPipe::SAMPLE_MASK;
        break;
    }

    RI_ASSERT(sx0 >= 0 && sx0 < (1<<Paint::GRADIENT_LUT_BITS));
    RI_ASSERT(sx1 >= 0 && sx1 < (1<<Paint::GRADIENT_LUT_BITS));

}

RI_INLINE static IntegerColor readLUTColor(const PixelPipe::PPUniforms& uniforms, int i)
{
    RI_ASSERT(i >= 0 && i <= Paint::GRADIENT_LUT_MASK);
    return uniforms.gradientLookup[i];
}


/**
 * \brief   Sample linear gradient using integer-arithmetic.
 * \note    The actual gradient computation is done piecewise within the
 *          pixel-pipeline.
 */
RI_INLINE static IntegerColor intLinearGradient(const PixelPipe::SignatureState& state, const PixelPipe::PPUniforms& u, const PixelPipe::PPVariants& v)
{
    RIint32 sx0 = v.sx >> (PixelPipe::GRADIENT_BITS - PixelPipe::SAMPLE_BITS);
    RIint32 sx1 = sx0 + 1;

    applyGradientRepeat(sx0, sx1, state.paintTilingMode);

    IntegerColor ic0 = readLUTColor(u, sx0 >> (PixelPipe::SAMPLE_BITS - Paint::GRADIENT_LUT_BITS));

    if (true)
    {
        return ic0;
    } else
    {
        // bilinear interpolation
        //RIint32 f1 = sx0; 
        readLUTColor(u, sx1 >> (PixelPipe::SAMPLE_BITS - Paint::GRADIENT_LUT_BITS));
        RI_ASSERT(false);
        return IntegerColor(0,0,0,0);
    }
} 

/**
 * \brief   Radial gradient implementation for the integer-pipeline. Will use float at least
 *          for the square-root. Will return integer-color always.
 */
RI_INLINE static IntegerColor intRadialGradient(const PixelPipe::SignatureState& state, const PixelPipe::PPUniforms& u, const PixelPipe::PPVariants& v)
{
    RGScalar a = (v.rx * u.rfxp) + (v.ry * u.rfyp);
    RGScalar b = u.rsqrp * (RI_SQR(v.rx) + RI_SQR(v.ry));
    RGScalar c = RI_SQR((v.rx * u.rfyp) - (v.ry * u.rfxp));
    RGScalar d = b - c;
    RI_ASSERT(!RI_ISNAN(d) ? d >= 0.0f : true);
    RGScalar g = (a + sqrtf(d));

    int sx0 = RI_FLOAT_TO_FX(g, PixelPipe::SAMPLE_BITS);
    int sx1 = sx0 + 1;

    applyGradientRepeat(sx0, sx1, state.paintTilingMode);

    IntegerColor ic0 = readLUTColor(u, sx0 >> (PixelPipe::SAMPLE_BITS - Paint::GRADIENT_LUT_BITS));
    RI_ASSERT(ic0.r <= 255);
    RI_ASSERT(ic0.g <= 255);
    RI_ASSERT(ic0.b <= 255);
    RI_ASSERT(ic0.a <= 255);

    if (false)
    {
        // Linear interpolation of 2 gradient samples.
        IntegerColor ic1 = readLUTColor(u, sx1 >> (PixelPipe::SAMPLE_BITS - Paint::GRADIENT_LUT_BITS));
        //int fx0 = sx0 & PixelPipe::SAMPLE_MASK;
        //int fx1 = PixelPipe::SAMPLE_MASK - fx0;
        
    }

    return ic0;
}

RI_INLINE static bool applyPatternRepeat(int &x, int &y, PixelPipe::TilingMode tilingMode)
{
    switch (tilingMode)
    {
    case PixelPipe::TILING_MODE_PAD:
        x = RI_INT_CLAMP(x, 0, PixelPipe::GRADIENT_MASK);
        y = RI_INT_CLAMP(y, 0, PixelPipe::GRADIENT_MASK);
        break; 
    case PixelPipe::TILING_MODE_REPEAT:
        x = x & PixelPipe::GRADIENT_MASK;
        y = y & PixelPipe::GRADIENT_MASK;
        break;
    case PixelPipe::TILING_MODE_REFLECT:
        x = intReflectRepeat(x, PixelPipe::GRADIENT_BITS);
        y = intReflectRepeat(y, PixelPipe::GRADIENT_BITS);
        break;
    default:
        RI_ASSERT(tilingMode == PixelPipe::TILING_MODE_FILL);
        // Do nothing -> Fill is checked on integer coordinates.
        break;
    }
    return false;
}

/**
 * \brief   Same as applyPatternRepeat, but with pattern-space integer coordinates without
 *          fractional part.
 * \note    Assumes that the coordinate is in range [0,width or height].
 */
RI_INLINE static bool applyPatternSampleRepeat(int &x, int &y, int w, int h, PixelPipe::TilingMode tilingMode)
{

    switch (tilingMode)
    {
    case PixelPipe::TILING_MODE_PAD:
        RI_ASSERT(x >= 0 && x <= w);
        RI_ASSERT(y >= 0 && y <= h);
        if (x >= w) x = w-1;
        if (y >= h) y = h-1;
        break;
    case PixelPipe::TILING_MODE_REPEAT:
        RI_ASSERT(x >= 0 && x <= w);
        RI_ASSERT(y >= 0 && y <= h);
        if (x >= w) x = 0;
        if (y >= h) y = 0;
        break;
    case PixelPipe::TILING_MODE_REFLECT:
        RI_ASSERT(x >= 0 && x <= w);
        RI_ASSERT(y >= 0 && y <= h);
        if (x >= w) x = w-1; // w-2?
        if (y >= h) y = h-1; // h-2?
        break;
    default:
        RI_ASSERT(tilingMode == PixelPipe::TILING_MODE_FILL);
        if (x < 0 || x >= w) return true;
        if (y < 0 || y >= h) return true;
        break;
    }

    return false;
}

RI_INLINE IntegerColor readPattern(const void* basePtr, int stride, const Color::Descriptor& desc, int ix, int iy, const IntegerColor* fillColor, bool fill)
{
    const void* ptr = Image::calculateAddress(basePtr, desc.bitsPerPixel, ix, iy, stride);

    if (!fill)
        return IntegerColor(Image::readPackedPixelFromAddress(ptr, desc.bitsPerPixel, ix), desc);
    else
    {
        RI_ASSERT(fillColor);
        return *fillColor; 
    }

}

/**
 * \brief   Rescale the result of bilinear interpolation.
 * \todo    See if this or individual shifts and rounds are faster on x86
 */
RI_INLINE static RIuint32 bilinearDiv(unsigned int c)
{
    RIuint32 rcp = 33026;

    RIuint64 m = (RIuint64)c * rcp;
    RIuint32 d = (RIuint32)(m >> 30);
    return (d >> 1) + (d & 1);
}

/**
 * \brief   Read an optionally filtered sample from an image. For multiple samples, apply repeat
 *          for all the generated sampling points. This only implements a simple sampling: nearest
 *          or Linear filtering and is much simpler than the original RI.
 * \param   image       Image to sample from
 * \param   sx0         Sample x in .8 fixed point. MUST be within the image except for FILL.
 * \param   sy0         Sample y in .8 fixed point. MUST be within the image except for FILL.
 * \param   samplerType Type of the sampler used.
 * \param   tilingMode  Tiling mode for generated sample points, if required.
 * \param   fillColor   Color to use for TILING_MODE_FILL
 * \todo Where should we determine if a NN-sample needs to be unpacked?
 *       -> It is also easy to just read that sample separately.
 */
RI_INLINE static IntegerColor intSampleImage(
    const void* ptr,
    int stride,
    int w,
    int h,
    const Color::Descriptor& desc,
    RIint32 sx0, 
    RIint32 sy0, 
    PixelPipe::SamplerType samplerType, 
    PixelPipe::TilingMode tilingMode, 
    const IntegerColor* fillColor)
{
    RI_ASSERT(fillColor || (tilingMode != PixelPipe::TILING_MODE_FILL));
     
    // \todo The following code is between low- and high-level representation of sampling.
    // It should probably be modified to appear fully as low-level, since we want as many
    // optimizations as possible.

    const bool bilinear = samplerType == PixelPipe::SAMPLER_TYPE_LINEAR;

    IntegerColor retColor;
    bool maybeFill = tilingMode == PixelPipe::TILING_MODE_FILL;
    bool fillSample = false;

    RIint32 ix, iy;

    IntegerColor ic00;

    RIint32 fx = sx0 & 0xff;
    RIint32 fy = sy0 & 0xff;

    ix = sx0 >> PixelPipe::SAMPLE_BITS;
    iy = sy0 >> PixelPipe::SAMPLE_BITS;

    if (maybeFill)
    {
        if (ix < 0 || ix >= w)
            fillSample = true;
        if (iy < 0 || iy >= h)
            fillSample = true;
    }

    ic00 = readPattern(ptr, stride, desc, ix, iy, fillColor, fillSample);

    if (!bilinear)
    {
        retColor = ic00;
        retColor.expandColor(desc); // \todo Handling of bilinear?
    }
    else
    {
        // Bilinear filtering.

        IntegerColor ic01, ic10, ic11;
        IntegerColor t0, t1;

        int xs = ix + 1;
        int ys = iy;

        fillSample = applyPatternSampleRepeat(xs, ys, w, h, tilingMode);
        ic01 = readPattern(ptr, stride, desc, xs, ys, fillColor, fillSample);

        t0 = IntegerColor::linearBlendNS(ic00, ic01, fx);

        xs = ix;
        ys = iy+1;
        fillSample = applyPatternSampleRepeat(xs, ys, w, h, tilingMode);
        ic10 = readPattern(ptr, stride, desc, xs, ys, fillColor, fillSample);

        xs = ix+1;
        ys = iy+1;
        fillSample = applyPatternSampleRepeat(xs, ys, w, h, tilingMode);
        ic11 = readPattern(ptr, stride, desc, xs, ys, fillColor, fillSample);
    
        t1 = IntegerColor::linearBlendNS(ic10, ic11, fx);
    
        retColor = IntegerColor::linearBlendNS(t0, t1, fy);

        retColor.r = bilinearDiv(retColor.r);
        retColor.g = bilinearDiv(retColor.g);
        retColor.b = bilinearDiv(retColor.b);
        retColor.a = bilinearDiv(retColor.a);

        return retColor;
    }

    return retColor;
}

RI_INLINE static RIint32 gradientToFixedCoords(RIint32 gradCoord, RIint32 dim)
{
    return (RIint32)(((RIint64)dim * gradCoord) >> (PixelPipe::GRADIENT_BITS - PixelPipe::SAMPLE_BITS));
}


RI_INLINE static IntegerColor intPattern(const PixelPipe::SignatureState &state, const PixelPipe::PPUniforms& u, const PixelPipe::PPVariants& v)
{
    // \todo The following code is between low- and high-level representation of sampling.
    // It should probably be modified to appear fully as low-level, since we want as many
    // optimizations as possible.

    // "External" variables
    const PixelPipe::TilingMode tilingMode = state.paintTilingMode;
    const IntegerColor fillColor = u.tileFillColor;
    const int w = u.paint_width;
    const int h = u.paint_height;

    IntegerColor retColor;

    RIint32 sx0 = v.sx;
    RIint32 sy0 = v.sy;

    IntegerColor ic00;

    applyPatternRepeat(sx0, sy0, tilingMode);
    sx0 = gradientToFixedCoords(sx0, w);
    sy0 = gradientToFixedCoords(sy0, h);
    //sx0 = (RIint32)(((RIint64)w * sx0) >> (PixelPipe::GRADIENT_BITS - PixelPipe::SAMPLE_BITS));
    //sy0 = (RIint32)(((RIint64)h * sy0) >> (PixelPipe::GRADIENT_BITS - PixelPipe::SAMPLE_BITS));

    const void* ptr = u.patternPtr;
    const int stride = u.patternStride;
    const Color::Descriptor& desc = state.patternDesc;

    return intSampleImage(ptr, stride, w, h, desc, sx0, sy0, state.paintSampler, tilingMode, &fillColor);
}

RI_INLINE static bool formatPremultipliedAfterSampling(const Color::Descriptor& desc, PixelPipe::SamplerType samplerType, PixelPipe::ImageGradientType gradientType)
{
    // Sampled at pixel centers -> no processing of colors -> does not get premultiplied
    if (gradientType == PixelPipe::GRADIENT_TYPE_INTEGER)
        return desc.isPremultiplied();

    if (samplerType != PixelPipe::SAMPLER_TYPE_NEAREST)
        return true;

    return desc.isPremultiplied();
}

RI_INLINE static bool imagePremultipliedAfterSampling(const PixelPipe::SignatureState& state)
{
    RI_ASSERT(state.hasImage);

    return formatPremultipliedAfterSampling(state.imageDesc, state.imageSampler, state.imageGradientType);
}

RI_INLINE static bool gradientPremultipliedAfterSampling(const PixelPipe::SignatureState& state)
{
    if (state.paintSampler != PixelPipe::SAMPLER_TYPE_NEAREST)
        return true;

    return true;

    // Otherwise, the gradient value is a single sample, and should be in the destination
    // color-space:
    //return state.dstDesc.isPremultiplied();
}

RI_INLINE static bool patternPremultipliedAfterSampling(const PixelPipe::SignatureState& state)
{
    RI_ASSERT(state.paintType == VG_PAINT_TYPE_PATTERN);

    return formatPremultipliedAfterSampling(state.patternDesc, state.paintSampler, PixelPipe::GRADIENT_TYPE_FIXED);
}

/**
 * \brief   Returns true if generated paint will be in RGB, false if luminance.
 */
RI_INLINE static bool paintInRGB(const PixelPipe::SignatureState& state)
{
    if (state.paintType != VG_PAINT_TYPE_PATTERN)
        return true;

    return !state.patternDesc.isLuminance();
}


/**
 * \brief   Applies color transform to input color
 * \param   isNonlinear "true" if input is nonlinear. This only affects luminance -> RGB conversion,
 *          other conversions happen in the input color-space.
 * \note    Leaves the color unpremultiplied, in source color-space and converts luminance to RGB
 * \todo    isNonlinear is not needed. It can be deduced from the state information!
 */
RI_INLINE static IntegerColor maybeColorTransform(const PixelPipe::SignatureState& state, const IntegerColor& c, const RIint32* colorTransformValues, bool isNonlinear)
{
    if (!state.hasColorTransform)
        return c;

    RI_ASSERT(state.hasImage || state.paintType == VG_PAINT_TYPE_PATTERN);
    
    IntegerColor r = c;

    if (state.imageMode == VG_DRAW_IMAGE_MULTIPLY)
    {
        r.unpremultiply();
    }
    else if (state.imageMode == VG_DRAW_IMAGE_STENCIL || state.paintType == VG_PAINT_TYPE_PATTERN)
    {
        // -> Check pattern
        if (patternPremultipliedAfterSampling(state))
            r.unpremultiply(); 
    }
    else
    {
        // -> Check image
        if (imagePremultipliedAfterSampling(state))
            r.unpremultiply();
    }

    // Check if it is necessary to convert to RGB:
    if (state.imageMode == VG_DRAW_IMAGE_MULTIPLY)
    {
        if (state.imageDesc.isLuminance() && !paintInRGB(state))
        {
            r.fullLuminanceToRGB(false, isNonlinear, false, isNonlinear);
        }
    }
    else if (state.imageMode == VG_DRAW_IMAGE_STENCIL)
    {
        if (state.patternDesc.isLuminance())
            r.fullLuminanceToRGB(false, isNonlinear, false, isNonlinear);
    }
    
    // \todo Use lookup-tables in some cases?
    r.r = (((RIint32)r.r * colorTransformValues[0]) >> PixelPipe::COLOR_TRANSFORM_BITS) + colorTransformValues[4];
    r.g = (((RIint32)r.g * colorTransformValues[1]) >> PixelPipe::COLOR_TRANSFORM_BITS) + colorTransformValues[5];
    r.b = (((RIint32)r.b * colorTransformValues[2]) >> PixelPipe::COLOR_TRANSFORM_BITS) + colorTransformValues[6];
    r.a = (((RIint32)r.a * colorTransformValues[3]) >> PixelPipe::COLOR_TRANSFORM_BITS) + colorTransformValues[7];

    // Clamp (integerColor?)
    r.r = (RIuint32)RI_INT_CLAMP((int)r.r, 0, 255);
    r.g = (RIuint32)RI_INT_CLAMP((int)r.g, 0, 255);
    r.b = (RIuint32)RI_INT_CLAMP((int)r.b, 0, 255);
    r.a = (RIuint32)RI_INT_CLAMP((int)r.a, 0, 255);


    return r;
}

/// Some rounding multiplications for blends:

/**
 * \brief   Multiply with rounding.
 */
RI_INLINE static RIuint32 rMul2(RIuint32 c0, RIuint32 c1, RIuint32 k0, RIuint32 k1)
{
    RIuint32 t = c0 * k0 + c1 * k1; 
    //RIuint32 r = (t + (t>>9)) >> 8;
    RIuint32 r = (t + (1>>7))>>8;
    RI_ASSERT(r <= 255);
    return r;
}

/**
 * \brief   Returns rounding color-multiplication: c0 + c1 * k
 */
RI_INLINE static RIuint32 rMul1(RIuint32 c0, RIuint32 c1, RIuint32 k)
{
    RIuint32 t = c1 * k;
    RIuint32 r = c0 + ((t + (t >> 7)) >> 8);
    RI_ASSERT(r <= 255);
    return r;
}

/**
 * \brief   Fixed-point multiplication
 */
RI_INLINE static RIuint32 rMul(RIuint32 c0, RIuint32 f)
{
    RIuint32 t = c0 * f;
    return (t + (1<<7))>>8;
}

/**
 * \brief   Multiply two colors [0, 255]
 */
RI_INLINE static RIuint32 cMul(RIuint32 c0, RIuint32 c1)
{
    RIuint32 t = c0 * c1;
    RIuint32 r = (t + (t >> 9)) >> 8;
    //RIuint32 t = c0 * c1;
    //RIuint32 r = (t + (t >> 7))>>8;
    RI_ASSERT(r <= 255);
    return r;
}

// \todo Are signed versions required?
RI_INLINE static RIuint32 cMin(RIuint32 c0, RIuint32 c1)
{
    return c0 <= c1 ? c0 : c1;
}

RI_INLINE static RIuint32 cMax(RIuint32 c0, RIuint32 c1)
{
    return c0 >= c1 ? c0 : c1;
}

/**
 * \brief   Blends two integer colors. Only considers the alpha-channels within
 *          the colors themselves. There should be a separate function to do
 *          blending with individual channel-alphas.
 * \note    It is also possible that LLVM is able to detect, whether individual alpha-
 *          channels contain a single/multi alpha
 * \todo    Overall, check how much and how fast LLVM is able to optimize out unused
 *          expressions.
 */
RI_INLINE static IntegerColor blendIntegerColors(const IntegerColor& s, const IntegerColor& d, VGBlendMode blendMode)
{
    IntegerColor r;

    switch(blendMode)
    {
    case VG_BLEND_SRC:
        r = s;
        break;

    case VG_BLEND_SRC_OVER:
    {
        RIuint32 ia = 255 - s.a;
        r.r = rMul1(s.r, d.r, ia);
        r.g = rMul1(s.g, d.g, ia);
        r.b = rMul1(s.b, d.b, ia);
        r.a = rMul1(s.a, d.a, ia);
        break;
    }
    case VG_BLEND_DST_OVER:
    {
        RIuint32 ia = 255 - d.a;
        r.r = rMul1(d.r, s.r, ia);
        r.g = rMul1(d.g, s.g, ia);
        r.b = rMul1(d.b, s.b, ia);
        r.a = rMul1(d.a, s.a, ia);
        break;
    }
    case VG_BLEND_SRC_IN:
    {
        r.r = cMul(s.r, d.a);
        r.g = cMul(s.g, d.a);
        r.b = cMul(s.b, d.a);
        r.a = cMul(s.a, d.a);
        break;
    }
    case VG_BLEND_DST_IN:
    {
        r.r = cMul(d.r, s.a);
        r.g = cMul(d.g, s.a);
        r.b = cMul(d.b, s.a);
        r.a = cMul(d.a, s.a);
        break;
    }
    case VG_BLEND_MULTIPLY:
    {
        RIuint32 iasrc, iadst;
        iasrc = 255 - s.a;
        iadst = 255 - d.a;
        r.r = rMul2(s.r, d.r, iadst + d.r, iasrc);
        r.g = rMul2(s.g, d.g, iadst + d.g, iasrc);
        r.b = rMul2(s.b, d.b, iadst + d.b, iasrc);
        r.a = rMul1(s.a, d.a, iasrc);
        break;
    }
    case VG_BLEND_SCREEN:
    {
        r.r = rMul1(s.r, d.r, 255 - s.r);
        r.g = rMul1(s.g, d.g, 255 - s.g);
        r.b = rMul1(s.b, d.b, 255 - s.b);
        r.a = rMul1(s.a, d.a, 255 - s.a);
        break;
    }
    case VG_BLEND_DARKEN:
    {
        RIuint32 iasrc = 255 - s.a;
        RIuint32 iadst = 255 - d.a;
        r.r = cMin(rMul1(s.r, d.r, iasrc), rMul1(d.r, s.r, iadst));
        r.g = cMin(rMul1(s.g, d.g, iasrc), rMul1(d.g, s.g, iadst));
        r.b = cMin(rMul1(s.b, d.b, iasrc), rMul1(d.b, s.b, iadst));
        r.a = rMul1(s.a, d.a, iasrc);
        break;
    }
    case VG_BLEND_LIGHTEN:
    {
        // \todo Compact darken w/r lighten?
        RIuint32 iasrc = 255 - s.a;
        RIuint32 iadst = 255 - d.a;
        r.r = cMax(rMul1(s.r, d.r, iasrc), rMul1(d.r, s.r, iadst));
        r.g = cMax(rMul1(s.g, d.g, iasrc), rMul1(d.g, s.g, iadst));
        r.b = cMax(rMul1(s.b, d.b, iasrc), rMul1(d.b, s.b, iadst));
        //although the statement below is equivalent to r.a = s.a + d.a * (1.0f - s.a)
        //in practice there can be a very slight difference because
        //of the max operation in the blending formula that may cause color to exceed alpha.
        //Because of this, we compute the result both ways and return the maximum.
        r.a = cMax(rMul1(s.a, d.a, iasrc), rMul1(d.a, s.a, iadst));
        break;
    }
    default:
    {
        RI_ASSERT(blendMode == VG_BLEND_ADDITIVE);
        r.r = cMin(s.r + d.r, 255);
        r.g = cMin(s.g + d.g, 255);
        r.b = cMin(s.b + d.b, 255);
        r.a = cMin(s.a + d.a, 255);
        break;
    }
    }
    return r;

}

RI_INLINE static IntegerColor blendIntegerStencil(const IntegerColor& s, const IntegerColor& im, const IntegerColor& d, VGBlendMode blendMode)
{
    IntegerColor r;

    switch(blendMode)
    {
    case VG_BLEND_SRC:
        r = s;
        break;

    case VG_BLEND_SRC_OVER:
    {
        r.r = rMul1(s.r, d.r, 255 - im.r);
        r.g = rMul1(s.g, d.g, 255 - im.g);
        r.b = rMul1(s.b, d.b, 255 - im.b);
        r.a = rMul1(s.a, d.a, 255 - s.a);
        break;
    }
    case VG_BLEND_DST_OVER:
    {
        r = blendIntegerColors(s, d, blendMode);
        break;
    }
    case VG_BLEND_SRC_IN:
    {
        r = blendIntegerColors(s, d, blendMode);
        break;
    }
    case VG_BLEND_DST_IN:
    {
        r.r = cMul(d.r, im.r);
        r.g = cMul(d.g, im.g);
        r.b = cMul(d.b, im.b);
        r.a = cMul(d.a, s.a);
        break;
    }
    case VG_BLEND_MULTIPLY:
    {
        RIuint32 iadst;
        iadst = 255 - d.a;
        r.r = rMul2(s.r, d.r, iadst + d.r, 255 - im.r);
        r.g = rMul2(s.g, d.g, iadst + d.g, 255 - im.g);
        r.b = rMul2(s.b, d.b, iadst + d.b, 255 - im.b);
        r.a = rMul1(s.a, d.a, 255 - s.a);
        break;
    }
    case VG_BLEND_SCREEN:
    {
        r = blendIntegerColors(s, d, blendMode);
        break;
    }
    case VG_BLEND_DARKEN:
    {
        RIuint32 iadst = 255 - d.a;
        r.r = cMin(rMul1(s.r, d.r, 255 - im.r), rMul1(d.r, s.r, iadst));
        r.g = cMin(rMul1(s.g, d.g, 255 - im.g), rMul1(d.g, s.g, iadst));
        r.b = cMin(rMul1(s.b, d.b, 255 - im.b), rMul1(d.b, s.b, iadst));
        r.a = rMul1(s.a, d.a, 255 - s.a);
        break;
    }
    case VG_BLEND_LIGHTEN:
    {
        // \todo Compact darken w/r lighten?
        RIuint32 iadst = 255 - d.a;
        r.r = cMax(rMul1(s.r, d.r, 255 - im.r), rMul1(d.r, s.r, iadst));
        r.g = cMax(rMul1(s.g, d.g, 255 - im.g), rMul1(d.g, s.g, iadst));
        r.b = cMax(rMul1(s.b, d.b, 255 - im.b), rMul1(d.b, s.b, iadst));
        //although the statement below is equivalent to r.a = s.a + d.a * (1.0f - s.a)
        //in practice there can be a very slight difference because
        //of the max operation in the blending formula that may cause color to exceed alpha.
        //Because of this, we compute the result both ways and return the maximum.
        r.a = cMax(rMul1(s.a, d.a, 255 - s.a), rMul1(d.a, s.a, iadst));
        break;
    }
    default:
    {
        RI_ASSERT(blendMode == VG_BLEND_ADDITIVE);
        return blendIntegerColors(s, d, blendMode);
        break;
    }
    }
    return r;

}

/**
 * \brief   Perform SRC_OVER and apply coverage in a single operation.
 * \note    It is possible to do optimizations like this for other blending operations,
 *          but they are not as widely used -> optimize if there is a requirement.
 * \note    Prints are included because GDB is confused about the value of r.
 */
static RI_INLINE IntegerColor srcOverCoverage(const IntegerColor& s, const IntegerColor& d, RIuint32 cov)
{
    IntegerColor r;
    RIuint32 ac = ((s.a + (s.a>>7)) * cov);
    ac = (ac + (1<<7))>>8;
    RIuint32 ia = 256 - ac;

    r.r = rMul2(s.r, d.r, cov, ia);
    r.g = rMul2(s.g, d.g, cov, ia);
    r.b = rMul2(s.b, d.b, cov, ia);
    r.a = rMul2(s.a, d.a, cov, ia);
    //r.r = (s.r * cov + d.r * ia) >> 8;
    //r.g = (s.g * cov + d.g * ia) >> 8;
    //r.b = (s.b * cov + d.b * ia) >> 8;
    //r.a = (s.a * cov + d.a * ia) >> 8;

#if defined(RI_DEBUG)
    if (!(r.r <= r.a && r.g <= r.a && r.b <= r.a && r.a <= 255))
    {
        printf("r: %d, g: %d, b: %d, a: %d\n",r.r,r.g,r.b,r.a);
        RI_ASSERT(false);
    }
    //RI_ASSERT(r.r <= 255 && r.g <= 255 && r.b <= 255 && r.a <= 255);
#endif

    return r;
}

/**
 * \brief   Check if converting between two color formats requires a gamma-conversion.
 * \todo    Move this to descriptor class.
 */
static RI_INLINE bool needGammaConvert(const Color::Descriptor& srcDesc, const Color::Descriptor& dstDesc)
{
    //if ((!srcDesc.isAlphaOnly()) && (srcDesc.isNonlinear() != dstDesc.isNonlinear()))
        //return true;
    if ((srcDesc.isNonlinear() != dstDesc.isNonlinear()))
        return true;

    return false;
}


RI_INLINE static bool preBlendPremultiplication(const PixelPipe::SignatureState& state)
{
    // \todo Simplify the rules (see the corresponding places in the pixelpipe
    const bool colorTransform = state.hasColorTransform;

    if (PixelPipe::isImageOnly(state))
    {
        if (colorTransform)
            return true;

        // Gamma conversion will leave the result premultiplied
        if (needGammaConvert(state.imageDesc, state.dstDesc))
            return true;
        //if (state.imageDesc.isAlphaOnly())
            //return false;

        return !imagePremultipliedAfterSampling(state);
    }

    if (state.hasImage)
    {
        if (state.imageMode == VG_DRAW_IMAGE_NORMAL)
            return !imagePremultipliedAfterSampling(state);
        // Image color has been combined with the paint color and that requires premultiplication
        if (state.imageMode == VG_DRAW_IMAGE_MULTIPLY)
            return false; // Always results in a premultiplied output color

        return false; // ?
    }

    if (state.paintType == VG_PAINT_TYPE_COLOR)
        return false;

    if (state.paintType != VG_PAINT_TYPE_PATTERN)
        return !gradientPremultipliedAfterSampling(state);

    // Must be pattern
    RI_ASSERT(state.paintType == VG_PAINT_TYPE_PATTERN);

    if (state.hasColorTransform)
        return true;

    if (needGammaConvert(state.patternDesc, state.dstDesc))
        return true;
    
    return !patternPremultipliedAfterSampling(state);
}

/**
 * \brief   Apply coverage [0 .. 256] on color
 * \note    This is actually "just coverage".
 */
RI_INLINE static IntegerColor srcCoverage(const IntegerColor& s, const IntegerColor& d, RIuint32 cov)
{
    IntegerColor r;
    RIuint32 icov = 256-cov;
    // Make function for multiplication between fixed point values (coverage is
    // a proper [0 .. 1] value.
    r.r = (s.r * cov + d.r * icov) >> 8;
    r.g = (s.g * cov + d.g * icov) >> 8;
    r.b = (s.b * cov + d.b * icov) >> 8;
    r.a = (s.a * cov + d.a * icov) >> 8;

    RI_ASSERT(r.r <= 255 && r.g <= 255 && r.b <= 255 && r.a <= 255);

    return r;
}

/**
 * \brief   Converts color gamma only. Care must be taken concerning luminance color formats.
 * \return  Converted color in "color". This will always be unpremultiplied if gamma conversion
 *          takes place, i.e, tries to minimize the amount of further conversions.
 */
RI_INLINE static void maybeGammaConvert(const Color::Descriptor& srcDesc, const Color::Descriptor& dstDesc, IntegerColor& color, bool inputPremultiplied)
{
    if (needGammaConvert(srcDesc, dstDesc))
    {
        if (inputPremultiplied)
            color.unpremultiply();
            //color.unpremultiply(srcDesc.isLuminance());

        if (dstDesc.isNonlinear())
            color.linearToGamma();
        else
            color.gammaToLinear();
    }
    // Output always unpremultiplied if gamma conversion takes place
}

/**
 * \brief   Integer pixel-pipeline.
 * \note    See internal_formats.txt for info on how the data is passed within the pipeline
 */
RI_INLINE static void intPixelPipe(const PixelPipe::SignatureState& signatureState, const PixelPipe::PPUniforms &uniforms, PixelPipe::PPVariants& variants)
{
    const RIuint32 ppMaxCoverage = Rasterizer::MAX_COVERAGE << (8 - Rasterizer::SAMPLE_BITS);
    RIuint32 coverage = variants.coverage << (8 - Rasterizer::SAMPLE_BITS);
    IntegerColor out;
    IntegerColor imageColor; // imagemode != normal
    const Color::Descriptor& dstDesc = signatureState.dstDesc;
    const Color::Descriptor& patternDesc = signatureState.patternDesc;
    const Color::Descriptor& imageDesc = signatureState.imageDesc;

    if (!PixelPipe::isImageOnly(signatureState))
    {
        switch(signatureState.paintType)
        {
        case VG_PAINT_TYPE_COLOR:
            out = uniforms.solidColor;
            break;
        case VG_PAINT_TYPE_LINEAR_GRADIENT:
            out = intLinearGradient(signatureState, uniforms, variants);
            variants.sx += uniforms.dgdx;
            // \todo Optimize this so that the lookup is in premultiplied dst format!
            // How about image-operations?
            if ((signatureState.imageMode != VG_DRAW_IMAGE_MULTIPLY) && dstDesc.isLuminance())
            {
                out.fullRGBToLuminance(true, dstDesc.isNonlinear(), true, dstDesc.isNonlinear());
            }
            break;
        case VG_PAINT_TYPE_RADIAL_GRADIENT:
            out = intRadialGradient(signatureState, uniforms, variants);
            variants.rx += uniforms.rdxdx;
            variants.ry += uniforms.rdydx;

            // \todo Optimize this so that the lookup is in premultiplied dst format!
            if ((signatureState.imageMode != VG_DRAW_IMAGE_MULTIPLY) && dstDesc.isLuminance())
            {
                out.fullRGBToLuminance(true, dstDesc.isNonlinear(), true, dstDesc.isNonlinear());
            }
            break;
        default:
            RI_ASSERT(signatureState.paintType == VG_PAINT_TYPE_PATTERN);
            out = intPattern(signatureState, uniforms, variants);
            // color-space == pattern color-space, not always premultiplied, expanded
            //
            // \todo Only increment the proper pixel-counters. This requires detecting the
            // transform type before generating the pixel-pipeline.
            // \note Implement fastpaths for at least identity transform with image edges coinciding
            // with the pixel edges. <- This has been done for images.
            variants.sx += uniforms.paint_dxdx;
            variants.sy += uniforms.paint_dydx;

            if (!patternDesc.hasAlpha())
                out.a = 255;

            if (!signatureState.hasImage)
            {
                out = maybeColorTransform(signatureState, out, uniforms.colorTransformValues, patternDesc.isNonlinear());
                const bool tmpPre = patternPremultipliedAfterSampling(signatureState) && !signatureState.hasColorTransform;
                const bool outLuminance = !signatureState.hasColorTransform && imageDesc.isLuminance();

                if (outLuminance != dstDesc.isLuminance())
                {
                    if (outLuminance)
                        out.fullLuminanceToRGB(tmpPre, patternDesc.isNonlinear(), tmpPre, patternDesc.isNonlinear());
                    else
                        out.fullRGBToLuminance(tmpPre, patternDesc.isNonlinear(), tmpPre, patternDesc.isNonlinear());
                }
                maybeGammaConvert(patternDesc, dstDesc, out, tmpPre);
            }

            break;
        }
    }
    
    if (signatureState.hasImage)
    {
        switch (signatureState.imageGradientType)
        {
        case PixelPipe::GRADIENT_TYPE_INTEGER:
        {
            void* addr = Image::calculateAddress(uniforms.imagePtr, imageDesc.bitsPerPixel, variants.iImageX, variants.iImageY, uniforms.imageStride);
            RIuint32 packedImageColor = Image::readPackedPixelFromAddress(addr, imageDesc.bitsPerPixel, variants.iImageX);
            imageColor.fromPackedColor(packedImageColor, imageDesc);
            imageColor.expandColor(imageDesc);
            // color-space == image color-space, not always premultiplied, expanded

            // Only integer image-gradient can have unsafe image data as an input at the moment.
            if (signatureState.unsafeImageInput)
            {
                if (imageDesc.hasAlpha() && imageDesc.isPremultiplied())
                    imageColor.clampToAlpha();
            }

            variants.iImageX += uniforms.image_idxdx;
            variants.iImageY += uniforms.image_idydx;
            break;
        }
        case PixelPipe::GRADIENT_TYPE_FIXED:
        {
            RI_ASSERT(!signatureState.unsafeImageInput);

            RIint32 sx, sy;
            sx = variants.iImageX;
            sy = variants.iImageY;
            applyPatternRepeat(sx, sy, PixelPipe::TILING_MODE_PAD);
            sx = gradientToFixedCoords(sx, uniforms.image_iWidth);
            sy = gradientToFixedCoords(sy, uniforms.image_iHeight);
            imageColor = intSampleImage(
                uniforms.imagePtr,
                uniforms.imageStride,
                uniforms.image_iWidth,
                uniforms.image_iHeight,
                imageDesc,
                sx, sy, signatureState.imageSampler, PixelPipe::TILING_MODE_PAD, NULL);

            variants.iImageX += uniforms.image_idxdx;
            variants.iImageY += uniforms.image_idydx;
            break;
        }
        default:
        {
            RI_ASSERT(signatureState.imageGradientType == PixelPipe::GRADIENT_TYPE_FLOAT);
            RI_ASSERT(!signatureState.unsafeImageInput);

            RIfloat fx, fy, fw, rw;
            fx = variants.fImageX;
            fy = variants.fImageY;
            fw = variants.fImageW;
            rw = 1.0f / fw;
            RIint32 sx0, sy0;
            fx = RI_CLAMP(fx * rw, 0.0f, uniforms.image_fWidth - 1.0f); // \todo fImageMaxX
            fy = RI_CLAMP(fy * rw, 0.0f, uniforms.image_fHeight - 1.0f);
            sx0 = RI_ROUND_TO_INT(fx * (1<<PixelPipe::SAMPLE_BITS));
            sy0 = RI_ROUND_TO_INT(fy * (1<<PixelPipe::SAMPLE_BITS)); 

            imageColor = intSampleImage(
                uniforms.imagePtr,
                uniforms.imageStride,
                uniforms.image_iWidth,
                uniforms.image_iHeight,
                imageDesc,
                sx0, sy0, signatureState.imageSampler, PixelPipe::TILING_MODE_PAD, NULL);

            variants.fImageX += uniforms.image_fdxdx;
            variants.fImageY += uniforms.image_fdydx;
            variants.fImageW += uniforms.image_fdwdx;
            break;
        }
        }

        if (!imageDesc.hasAlpha())
            imageColor.a = 255;
        
        if (PixelPipe::isImageOnly(signatureState))
        {
            RI_ASSERT(signatureState.imageMode == VG_DRAW_IMAGE_NORMAL);
            out = maybeColorTransform(signatureState, imageColor, uniforms.colorTransformValues, imageDesc.isNonlinear());

            const bool tmpPre = imagePremultipliedAfterSampling(signatureState) && !signatureState.hasColorTransform;
            const bool outLuminance = !signatureState.hasColorTransform && imageDesc.isLuminance();

            // Color-format conversion to dst before blending.
            if (outLuminance != dstDesc.isLuminance())
            {
                if (outLuminance)
                    out.fullLuminanceToRGB(tmpPre, imageDesc.isNonlinear(), tmpPre, imageDesc.isNonlinear());
                else
                    out.fullRGBToLuminance(tmpPre, imageDesc.isNonlinear(), tmpPre, imageDesc.isNonlinear());
            }
            maybeGammaConvert(imageDesc, dstDesc, out, tmpPre);

            //if (!signatureState.hasColorTransform)
                //out.premultiply();
        }
        else
        {
            RI_ASSERT(signatureState.imageMode != VG_DRAW_IMAGE_NORMAL);

            if (!imagePremultipliedAfterSampling(signatureState))
                imageColor.premultiply();

            if (signatureState.imageMode == VG_DRAW_IMAGE_MULTIPLY)
            {
                if (signatureState.paintType == VG_PAINT_TYPE_PATTERN && 
                    !patternPremultipliedAfterSampling(signatureState))
                {
                    out.premultiply();
                }

                out.r = cMul(out.r, imageColor.r);
                out.g = cMul(out.g, imageColor.g);
                out.b = cMul(out.b, imageColor.b);
                out.a = cMul(out.a, imageColor.a);

                out = maybeColorTransform(signatureState, out, uniforms.colorTransformValues, imageDesc.isNonlinear());
                //const bool outLuminance = !signatureState.hasColorTransform && imageDesc.isLuminance();
                // Color transform will always result in RGB, regardless of input.
                const bool outLuminance = (imageDesc.isLuminance() && !paintInRGB(signatureState)) && !signatureState.hasColorTransform;
                if (!outLuminance && dstDesc.isLuminance())
                {
                    // Convert to destination (luminance)
                    out.fullRGBToLuminance(!signatureState.hasColorTransform, imageDesc.isNonlinear(), true, dstDesc.isNonlinear());
                }
                else if (imageDesc.isNonlinear() != dstDesc.isNonlinear())
                {
                    // Non-luminance gamma
                    if (!signatureState.hasColorTransform)
                        out.unpremultiply();

                    if (dstDesc.isNonlinear())
                        out.linearToGamma();
                    else
                        out.gammaToLinear();

                    out.premultiply();
                }
                else if (signatureState.hasColorTransform)
                    out.premultiply();

                // Output dst and premultiplied.
            } 
            else
            {
                RI_ASSERT(signatureState.imageMode == VG_DRAW_IMAGE_STENCIL);
                IntegerColor alphas, pr;
                
                if (signatureState.paintType == VG_PAINT_TYPE_PATTERN)
                {
                    out = maybeColorTransform(signatureState, out, uniforms.colorTransformValues, patternDesc.isNonlinear());
                    const bool isLuminance = patternDesc.isLuminance() && !signatureState.hasColorTransform;
                    // If using pattern, convert to destination color-space
                    // \todo If not, handle this when the lookups are generated.
                    if (isLuminance != dstDesc.isLuminance())
                    {
                        out.fullRGBToLuminance(patternPremultipliedAfterSampling(signatureState) && !signatureState.hasColorTransform, patternDesc.isNonlinear(), true, dstDesc.isNonlinear());
                    }
                    else if (patternDesc.isNonlinear() != dstDesc.isNonlinear())
                    {
                        if (patternPremultipliedAfterSampling(signatureState) && !signatureState.hasColorTransform)
                            out.unpremultiply();

                        if (dstDesc.isNonlinear())
                            out.linearToGamma();
                        else
                            out.gammaToLinear();

                        out.premultiply();
                    } else if (signatureState.hasColorTransform || !patternPremultipliedAfterSampling(signatureState))
                        out.premultiply();
                }

                if (dstDesc.isLuminance() && !imageDesc.isLuminance())
                {
                    // Convert image to luminance
                    imageColor.rgbToLuminance();
                    imageColor.r = imageColor.b = imageColor.b = RI_INT_MIN(imageColor.r, imageColor.a);
                }

#if defined(RI_DEBUG) && 0
                printf("stencil r: %d, g: %d, b: %d, a: %d\n",imageColor.r,imageColor.g,imageColor.b,imageColor.a);
                printf("input r: %d, g: %d, b: %d, a: %d\n",out.r,out.g,out.b,out.a);
#endif
                if (signatureState.paintType == VG_PAINT_TYPE_COLOR)
                {
                    // Better precision for solid color input.
                    // Compute alpha channels
                    alphas.r = rMul(out.a, imageColor.r);
                    alphas.g = rMul(out.a, imageColor.g);
                    alphas.b = rMul(out.a, imageColor.b);
                    // Premultiply
                    pr.r = rMul(out.r, imageColor.r);
                    pr.g = rMul(out.g, imageColor.g);
                    pr.b = rMul(out.b, imageColor.b);
                    pr.a = rMul(out.a, imageColor.a);
                }
                else
                {
                    // Compute alpha channels
                    alphas.r = cMul(out.a, imageColor.r);
                    alphas.g = cMul(out.a, imageColor.g);
                    alphas.b = cMul(out.a, imageColor.b);
                    // Premultiply
                    pr.r = cMul(out.r, imageColor.r);
                    pr.g = cMul(out.g, imageColor.g);
                    pr.b = cMul(out.b, imageColor.b);
                    pr.a = cMul(out.a, imageColor.a);
                }
#if defined(RI_DEBUG) && 0
                printf("alphas r: %d, g: %d, b: %d, a: %d\n",alphas.r,alphas.g,alphas.b,alphas.a);
                printf("pr r: %d, g: %d, b: %d, a: %d\n",pr.r,pr.g,pr.b,pr.a);
#endif
                out = pr;
                imageColor = alphas;
            }
        }
    }

    if (signatureState.hasMasking)
    {
        // \todo Read and process only the proper component of the mask pixel.
        const int maskBpp = signatureState.maskDesc.bitsPerPixel;

        RIuint32 packedMaskColor = Image::readPackedPixelFromAddress(variants.maskPtr, maskBpp, variants.dstX);
        IntegerColor maskColor;
        maskColor.fromPackedMask(packedMaskColor, signatureState.maskDesc);
        maskColor.expandMask(signatureState.maskDesc);

        RIuint32 maskCoverage = maskColor.a + (maskColor.a >> 7);
        coverage = (coverage * maskCoverage) >> 8;

        variants.maskPtr = (void*)Image::incrementPointer(variants.maskPtr, maskBpp, variants.dstX);  
    }
    
#if defined(RI_DEBUG)
    IntegerColor preblend = out;
#endif
    // \todo Coverage check for pixelpipes != solid color with solid output colors?

    IntegerColor d(0,0,0,0);

    // All operations that depend on DST are done next. Keep it organized like that.
    if ((coverage < ppMaxCoverage) || (out.a < 255) || alwaysLoadDst(signatureState))
    {
        d = IntegerColor(Image::readPackedPixelFromAddress(
            variants.dst, dstDesc.bitsPerPixel, variants.dstX), dstDesc);
        d.expandColor(dstDesc);

        if (!dstDesc.isPremultiplied())
        {
            d.premultiply();
        }

        // Premultiply output
#if 0
        if (!PixelPipe::isImageOnly(signatureState))
        {
            if (signatureState.paintType == VG_PAINT_TYPE_PATTERN && !patternPremultipliedAfterSampling(signatureState))
                out.premultiply();
            else if (signatureState.hasImage && !imagePremultipliedAfterSampling(signatureState))
                out.premultiply();
        }
#endif

        if (!signatureState.isRenderToMask)
        {
            VGBlendMode bm = signatureState.blendMode;

            // Currently SRC requires premultiplication even when only applying coverage.
            //if (bm != VG_BLEND_SRC)
            {
                // If the src color has not been premultiplied before, now's the time.
                // \todo Fast path for src alpha == 255 and SRC_OVER? Others?
                if (preBlendPremultiplication(signatureState))
                    out.premultiply();
            }

            if (signatureState.hasImage && signatureState.imageMode == VG_DRAW_IMAGE_STENCIL)
            {
                out = blendIntegerStencil(out, imageColor, d, bm);
            } 
            else
            {
                switch(bm)
                {
                case VG_BLEND_SRC_OVER:
                    out = srcOverCoverage(out, d, coverage);
                    break;
                case VG_BLEND_SRC:
                    out = srcCoverage(out, d, coverage);
                    break;
                default:
                    out = blendIntegerColors(out, d, bm);
                    out = srcCoverage(out, d, coverage);
                    break;
                }
            }

#if defined(RI_DEBUG)
            if (dstDesc.isPremultiplied())
            {
                RI_ASSERT(out.r <= out.a);
                RI_ASSERT(out.g <= out.a);
                RI_ASSERT(out.b <= out.a);
            }
#endif

        }
        else
        {
            // Mask operation
            out = intMaskOperation(coverage, d, signatureState.maskOperation);
        }

        // out is always premultiplied at this point. Must be in destination color-space
        if (!dstDesc.isPremultiplied())
        { 
            // Unpremultiply if output is not premultiplied
            out.unpremultiply();
        }
    }
    else
    {
        // Unpremultiply, ...
        if (!dstDesc.isPremultiplied())
            out.unpremultiply();
    }

    // VG_SET_MASK does not require dst load:
    if (signatureState.isRenderToMask && signatureState.maskOperation == VG_SET_MASK)
        out = intMaskOperation(coverage, d, VG_SET_MASK);

    out.truncateColor(dstDesc);
    Image::writePackedPixelToAddress(
        variants.dst, dstDesc.bitsPerPixel, variants.dstX, out.getPackedColor(dstDesc));

    // \todo X for bpp < 8
    variants.dst = (void*)Image::incrementPointer(variants.dst, dstDesc.bitsPerPixel, variants.dstX);
    //variants.dst = colorBuffer->advancePointer(variants.dst);
    variants.dstX++;
}

RI_INLINE static void fillSolidSpan(const PixelPipe::SignatureState& state, const PixelPipe::PPUniforms& uniforms, int startX, int y, int nPixels, RIuint32 packedColor) 
{
    Image::fillPackedPixels((void*)uniforms.dstPtr, state.dstDesc.bitsPerPixel, startX, y, uniforms.dstStride, nPixels, packedColor);
}

/**
 * \brief   This will calculate all the pixel-pipeline variants that need to be updated per-pixel.
 * \note    There may be a need for a different, faster function for image rendering, where
 *          there are faster methods of updating the variants.
 */
RI_INLINE static void prepareSpanVariants(const PixelPipe::SignatureState& state, const PixelPipe::PPUniforms& uniforms, const Span& span, PixelPipe::PPVariants& variants)
{
    //variants.dst = uniforms.dst->calculateAddress(span.x0, span.y);
    variants.dst = Image::calculateAddress(uniforms.dstPtr, state.dstDesc.bitsPerPixel, span.x0, span.y, uniforms.dstStride);
    variants.dstX = span.x0;
    variants.coverage = span.coverage;

    if (state.paintType != VG_PAINT_TYPE_COLOR)
    {
        if (state.paintType == VG_PAINT_TYPE_LINEAR_GRADIENT)
        {
            // \todo Adjust pixel-center.
            int x = uniforms.dgdx * span.x0 + uniforms.dgdy * span.y + uniforms.lgc;
            variants.sx = x;
        } 
        else if (state.paintType == VG_PAINT_TYPE_RADIAL_GRADIENT)
        {
            RGScalar x = uniforms.rdxdx * (RGScalar)span.x0 + uniforms.rdxdy * (RGScalar)span.y;
            RGScalar y = uniforms.rdydy * (RGScalar)span.y + uniforms.rdydx * (RGScalar)span.x0;

            variants.rx = x + uniforms.rx0;
            variants.ry = y + uniforms.ry0;
        }
        else
        {
            RI_ASSERT(state.paintType == VG_PAINT_TYPE_PATTERN);
            variants.sx = uniforms.paint_dxdx * span.x0 + uniforms.paint_dxdy * span.y + uniforms.paint_x0;
            variants.sy = uniforms.paint_dydy * span.y + uniforms.paint_dydx * span.x0 + uniforms.paint_y0;
        }
    }

    if (state.hasMasking)
    {
        variants.maskPtr = Image::calculateAddress(uniforms.maskPtr, state.maskDesc.bitsPerPixel, span.x0, span.y, uniforms.maskStride);
    }

    if (state.hasImage)
    {
        switch (state.imageGradientType)
        {
        case PixelPipe::GRADIENT_TYPE_INTEGER:
        case PixelPipe::GRADIENT_TYPE_FIXED:
            variants.iImageX = uniforms.image_ix0 + span.x0 * uniforms.image_idxdx + span.y * uniforms.image_idxdy;
            variants.iImageY = uniforms.image_iy0 + span.y * uniforms.image_idydy + span.x0 * uniforms.image_idydx;
            break;
        default:
            RI_ASSERT(state.imageGradientType == PixelPipe::GRADIENT_TYPE_FLOAT);
            variants.fImageX = uniforms.image_fx0 + span.x0 * uniforms.image_fdxdx + span.y * uniforms.image_fdxdy;
            variants.fImageY = uniforms.image_fy0 + span.y * uniforms.image_fdydy + span.x0 * uniforms.image_fdydx;
            variants.fImageW = uniforms.image_fw0 + span.x0 * uniforms.image_fdwdx + span.y * uniforms.image_fdwdy;
            break;
        }
    }
}

void executePixelPipeline(const PixelPipe::SignatureState& state, const PixelPipe::PPUniforms& uniforms, PixelPipe::PPVariants& variants, const Span* spans, int nSpans)
{
    RI_ASSERT(nSpans > 0);
    for (int i = 0; i < nSpans; i++)
    {
        const Span& s = spans[i]; 

        if (s.coverage != Rasterizer::MAX_COVERAGE || !canSolidFill(state))
        {
            int n = s.len;
            RI_ASSERT(n);
            prepareSpanVariants(state, uniforms, s, variants);

            do {
                intPixelPipe(state, uniforms, variants);
            } while (--n);
        } else
        {
            fillSolidSpan(state, uniforms, s.x0, s.y, s.len, uniforms.packedSolidColor);
        }
    }
    
}

void calculatePPHash(PixelPipeHash& hash, const PixelPipe::SignatureState& derivedState)
{
    const RIuint32 blendModeBits = 4;
    const RIuint32 imageModeBits = 2;
    const RIuint32 paintTypeBits = 2;
    const RIuint32 tilingModeBits = 2;
    const RIuint32 samplerBits = 1;
    const RIuint32 imageGradientTypeBits = 2;
    const RIuint32 boolBits = 1;
    const RIuint32 descBits = 10;
    const RIuint32 maskOperationBits = 3;

    RIuint32 blendMode = ((RIuint32)derivedState.blendMode) - ((RIuint32)VG_BLEND_SRC);
    RIuint32 imageMode = ((RIuint32)derivedState.imageMode) - ((RIuint32)VG_DRAW_IMAGE_NORMAL);
    RIuint32 paintType = ((RIuint32)derivedState.paintType) - ((RIuint32)VG_PAINT_TYPE_COLOR);
    RIuint32 maskOperation = ((RIuint32)derivedState.maskOperation) - ((RIuint32)VG_CLEAR_MASK);
    RIuint32 paintTilingMode = ((RIuint32)derivedState.paintTilingMode);
    RIuint32 paintSampler = ((RIuint32)derivedState.paintSampler);
    RIuint32 imageSampler = ((RIuint32)derivedState.imageSampler);

    RIuint32 imageGradientType = ((RIuint32)derivedState.imageGradientType);

    RIuint32 dstFormat = (RIuint32)(derivedState.dstDesc.toIndex());
    RIuint32 maskFormat = (RIuint32)(derivedState.maskDesc.toIndex());
    RIuint32 imageFormat = (RIuint32)(derivedState.imageDesc.toIndex());
    RIuint32 patternFormat = (RIuint32)(derivedState.patternDesc.toIndex());

    RIuint32 hasMasking = derivedState.hasMasking ? 1 : 0;
    RIuint32 hasImage = derivedState.hasImage ? 1 : 0;
    RIuint32 hasColorTransform = derivedState.hasColorTransform ? 1 : 0;
    RIuint32 isMaskOperation = derivedState.isRenderToMask ? 1 : 0;
    RIuint32 fillColorTransparent = derivedState.fillColorTransparent ? 1 : 0;
    RIuint32 unsafeImageInput = derivedState.unsafeImageInput ? 1 : 0;

    // Modify hashes according to relevant state:
    int b = 0;
    b = riInsertBits32(hash.value, sizeof(hash.value), blendMode, blendModeBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), imageMode, imageModeBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), paintType, paintTypeBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), maskOperation, maskOperationBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), paintTilingMode, tilingModeBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), paintSampler, samplerBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), imageSampler, samplerBits, b);

    b = riInsertBits32(hash.value, sizeof(hash.value), imageGradientType, imageGradientTypeBits, b);

    b = riInsertBits32(hash.value, sizeof(hash.value), dstFormat, descBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), maskFormat, descBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), imageFormat, descBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), patternFormat, descBits, b);

    b = riInsertBits32(hash.value, sizeof(hash.value), hasMasking, boolBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), hasImage, boolBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), hasColorTransform, boolBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), isMaskOperation, boolBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), fillColorTransparent, boolBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), unsafeImageInput, boolBits, b);
}

}

