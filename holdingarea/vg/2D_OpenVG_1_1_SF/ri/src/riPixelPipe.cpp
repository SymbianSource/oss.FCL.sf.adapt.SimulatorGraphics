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
 * \brief   Implementation of Paint and pixel pipe functionality.
 * \note
 *//*-------------------------------------------------------------------*/

#ifndef __RIPIXELPIPE_H
#   include "riPixelPipe.h"
#endif
#ifndef __RIRASTERIZER_H
#   include "riRasterizer.h"
#endif
#ifndef __SFDYNAMICPIXELPIPE_H
#   include "sfDynamicPixelPipe.h"
#endif
#ifndef __SFCOMPILER_H
#   include "sfCompiler.h"
#endif

//==============================================================================================

namespace OpenVGRI
{

/*-------------------------------------------------------------------*//*!
* \brief    Paint constructor.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

Paint::Paint() :
    m_paintType(VG_PAINT_TYPE_COLOR),
    m_paintColor(0,0,0,1,Color::sRGBA_PRE),
    m_inputPaintColor(0,0,0,1,Color::sRGBA),
    m_colorRampSpreadMode(VG_COLOR_RAMP_SPREAD_PAD),
    m_colorRampStops(),
    m_inputColorRampStops(),
    m_colorRampPremultiplied(VG_TRUE),
    m_inputLinearGradientPoint0(0,0),
    m_inputLinearGradientPoint1(1,0),
    m_inputRadialGradientCenter(0,0),
    m_inputRadialGradientFocalPoint(0,0),
    m_inputRadialGradientRadius(1.0f),
    m_linearGradientPoint0(0,0),
    m_linearGradientPoint1(1,0),
    m_radialGradientCenter(0,0),
    m_radialGradientFocalPoint(0,0),
    m_radialGradientRadius(1.0f),
    m_patternTilingMode(VG_TILE_FILL),
    m_pattern(NULL),
    m_referenceCount(0),
    m_lutFormat((VGImageFormat)-1),
    m_gradientStopsChanged(true)
{
    Paint::GradientStop gs;
    gs.offset = 0.0f;
    gs.color.set(0,0,0,1,Color::sRGBA);
    m_colorRampStops.push_back(gs); //throws bad_alloc
    gs.offset = 1.0f;
    gs.color.set(1,1,1,1,Color::sRGBA);
    m_colorRampStops.push_back(gs); //throws bad_alloc
}

/*-------------------------------------------------------------------*//*!
* \brief    Paint destructor.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

Paint::~Paint()
{
    RI_ASSERT(m_referenceCount == 0);
    if(m_pattern)
    {
        m_pattern->removeInUse();
        if(!m_pattern->removeReference())
            RI_DELETE(m_pattern);
    }
}

static Color readStopColor(const Array<Paint::GradientStop>& colorRampStops, int i, VGboolean colorRampPremultiplied)
{
    RI_ASSERT(i >= 0 && i < colorRampStops.size());
    Color c = colorRampStops[i].color;
    RI_ASSERT(c.getInternalFormat() == Color::sRGBA);
    if(colorRampPremultiplied)
        c.premultiply();
    return c;
}

void Paint::setGradientStops(Array<GradientStop>& inputStops, Array<GradientStop>& stops)
{
    m_colorRampStops.swap(stops);
    m_inputColorRampStops.swap(inputStops);
    m_gradientStopsChanged = true;
}

void Paint::setLinearGradient(const Vector2& p0, const Vector2& p1)
{
    m_linearGradientPoint0 = p0;
    m_linearGradientPoint1 = p1;
}

void Paint::setRadialGradient(const Vector2& c, const Vector2& f, VGfloat r)
{
    m_radialGradientCenter = c;
    m_radialGradientFocalPoint = f;
    m_radialGradientRadius = r;
}

bool Paint::linearDegenerate() const
{
    return m_linearGradientPoint0 == m_linearGradientPoint1 ? true : false;
}

bool Paint::radialDegenerate() const
{
    return m_radialGradientRadius == 0.0f ? true : false;
}

/**
 * \brief   Returns either the VG_PAINT_COLOR, or evaluated gradient value when the
 *          gradient is degenerate.
 */
Color Paint::getSolidColor() const
{
    if (m_paintType == VG_PAINT_TYPE_PATTERN)
    {
        RI_ASSERT(m_pattern == NULL);
        return m_paintColor;
    }

    if (m_paintType == VG_PAINT_TYPE_COLOR)
        return m_paintColor;

    RI_ASSERT(linearDegenerate() || radialDegenerate());

    // Determine the color at the end of the gradient
    RIfloat gs, ge;
    if (m_colorRampSpreadMode == VG_COLOR_RAMP_SPREAD_PAD)
    {
        gs = 1.0f - 1/256.0f;
        ge = 1.0f;
    } else if (m_colorRampSpreadMode == VG_COLOR_RAMP_SPREAD_REPEAT)
    {
        gs = 0.0f;
        ge = 1/256.0f;
    } else
    {
        gs = 1.0f - 1/256.0f;
        ge = 1.0f;
    }
    Color c = integrateColorRamp(gs, ge);
    return c * 256.0f;
}

/*-------------------------------------------------------------------*//*!
* \brief    Returns the average color within an offset range in the color ramp.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

Color Paint::integrateColorRamp(RIfloat gmin, RIfloat gmax) const
{
    RI_ASSERT(gmin <= gmax);
    RI_ASSERT(gmin >= 0.0f && gmin <= 1.0f);
    RI_ASSERT(gmax >= 0.0f && gmax <= 1.0f);
    RI_ASSERT(m_colorRampStops.size() >= 2);   //there are at least two stops

    Color currC(0,0,0,0,m_colorRampPremultiplied ? Color::sRGBA_PRE : Color::sRGBA);

    if(gmin == 1.0f || gmax == 0.0f)
        return currC;

    int i = 0;

    for(; i < m_colorRampStops.size()-1; i++)
    {
        if(gmin >= m_colorRampStops[i].offset && gmin < m_colorRampStops[i+1].offset)
        {
            RIfloat s = m_colorRampStops[i].offset;
            RIfloat e = m_colorRampStops[i+1].offset;
            RI_ASSERT(s < e);
            RIfloat g = (gmin - s) / (e - s);

            Color sc = readStopColor(m_colorRampStops, i, m_colorRampPremultiplied);
            Color ec = readStopColor(m_colorRampStops, i+1, m_colorRampPremultiplied);
            Color rc = (1.0f-g) * sc + g * ec;

            //subtract the average color from the start of the stop to gmin
            Color dc = 0.5f*(gmin - s)*(sc + rc);
            currC -= dc; 
            break;
        }
    }

    for(;i < m_colorRampStops.size()-1; i++)
    {
        RIfloat s = m_colorRampStops[i].offset;
        RIfloat e = m_colorRampStops[i+1].offset;
        RI_ASSERT(s <= e);

        Color sc = readStopColor(m_colorRampStops, i, m_colorRampPremultiplied);
        Color ec = readStopColor(m_colorRampStops, i+1, m_colorRampPremultiplied);

        //average of the stop
        Color dc = 0.5f*(e-s)*(sc + ec);
        currC += dc; 

        if(gmax >= m_colorRampStops[i].offset && gmax < m_colorRampStops[i+1].offset)
        {
            RIfloat g = (gmax - s) / (e - s);
            Color rc = (1.0f-g) * sc + g * ec;

            //subtract the average color from gmax to the end of the stop
            dc = 0.5f*(e - gmax)*(rc + ec);
            currC -= dc;
            break;
        }
    }

    return currC;
}

/**
 * \brief   Generates gradient color-ramp lookup values.
 *
 * \param   targetFormat    Destination or image format to associate LUT with.
 * \patam   drawImage       true if paint is evaluated along drawImage.
 *
 * \note    Must be called prior to rendering, and after the destination
 *          format is known. The destination format is either destination 
 *          surface format, or the image format in case of image rendering
 *          operation.
 */
void Paint::generateLUT(PixelPipe& pipe, VGImageFormat preferredFormat)
{
    const RIfloat gstep = 1.0f / (GRADIENT_LUT_COUNT);
    const RIfloat rcp = (RIfloat)(GRADIENT_LUT_COUNT);
    RIfloat gsx;
    gsx = 0.0f;
    
    if (!pipe.colorTransformChanged() && !m_gradientStopsChanged && (preferredFormat == m_lutFormat))
        return; // Already in correct format

    const bool inputPremultiplied = m_colorRampPremultiplied == VG_TRUE ? true : false;

    // Colortransform premultiplies color.
    const Color::Descriptor srcDesc = Color::formatToDescriptorConst(
        inputPremultiplied || pipe.hasColorTransform() ? VG_sRGBA_8888_PRE : VG_sRGBA_8888);

    const Color::Descriptor dstDesc = Color::formatToDescriptorConst(preferredFormat);

    // Create a pre-calculated LUT.
    for (int i = 0; i < GRADIENT_LUT_COUNT; i++)
    {
        // \todo Open up the integrator and/or use also integers.
        Color c = integrateColorRamp(gsx, gsx + gstep);
        c *= rcp;

        // \todo Changing the mode must be tracked somehow!
        if (pipe.getImageMode() != VG_DRAW_IMAGE_MULTIPLY)
            pipe.colorTransform(c);

        IntegerColor ic = IntegerColor(c);
        ic.convertToFrom(dstDesc, srcDesc, false);
        m_gradientLUT[i] = ic;

        gsx += gstep;
    }

    m_gradientStopsChanged = false;
    m_lutFormat = Color::descriptorToVGImageFormat(dstDesc);
    pipe.setColorTransformChanged(false);

    RI_ASSERT(m_lutFormat == preferredFormat);
}

/*-------------------------------------------------------------------*//*!
* \brief    PixelPipe constructor.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

PixelPipe::PixelPipe() :
    m_drawable(NULL),
    m_image(NULL),
    m_paint(NULL),
    m_defaultPaint(),
    m_blendMode(VG_BLEND_SRC_OVER),
    m_imageMode(VG_DRAW_IMAGE_NORMAL),
    m_imageQuality(VG_IMAGE_QUALITY_FASTER),
    m_tileFillColor(0,0,0,0,Color::sRGBA),
    m_colorTransform(false),
    m_colorTransformValues(),
    m_iColorTransformValues(),
    m_surfaceToPaintMatrix(),
    m_surfaceToImageMatrix(),
    m_paintToSurfaceMatrix(),
    m_maskOperation(VG_SET_MASK),
    m_renderToMask(false),
    m_colorTransformChanged(true)
{
    for(int i=0;i<8;i++)
    {
        m_colorTransformValues[i] = (i < 4) ? 1.0f : 0.0f;
        m_iColorTransformValues[i] = (i < 4) ? (COLOR_TRANSFORM_ONE) : 0;
    }
}


/*-------------------------------------------------------------------*//*!
* \brief    PixelPipe destructor.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

PixelPipe::~PixelPipe()
{
}

/*-------------------------------------------------------------------*//*!
* \brief    Sets the rendering surface.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void PixelPipe::setDrawable(Drawable* drawable)
{
    RI_ASSERT(drawable);
    m_drawable = drawable;
}

/*-------------------------------------------------------------------*//*!
* \brief    Sets the blend mode.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void PixelPipe::setBlendMode(VGBlendMode blendMode)
{
    RI_ASSERT(blendMode >= VG_BLEND_SRC && blendMode <= VG_BLEND_ADDITIVE);
    m_blendMode = blendMode;
}

/*-------------------------------------------------------------------*//*!
* \brief    Sets the mask image. NULL disables masking.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void PixelPipe::setMask(bool masking)
{
    m_masking = masking;
}

/*-------------------------------------------------------------------*//*!
* \brief    Sets the image to be drawn. NULL disables image drawing.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void PixelPipe::setImage(Image* image, VGImageMode imageMode)
{
    RI_ASSERT(imageMode == VG_DRAW_IMAGE_NORMAL || imageMode == VG_DRAW_IMAGE_MULTIPLY || imageMode == VG_DRAW_IMAGE_STENCIL);
    m_image = image;
    m_imageMode = imageMode;
}

/*-------------------------------------------------------------------*//*!
* \brief    Sets the surface-to-paint matrix.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void PixelPipe::setSurfaceToPaintMatrix(const Matrix3x3& surfaceToPaintMatrix)
{
    m_surfaceToPaintMatrix = surfaceToPaintMatrix;
}

/*-------------------------------------------------------------------*//*!
* \brief    Sets the surface-to-image matrix.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void PixelPipe::setSurfaceToImageMatrix(const Matrix3x3& surfaceToImageMatrix)
{
    m_surfaceToImageMatrix = surfaceToImageMatrix;
}

/*-------------------------------------------------------------------*//*!
* \brief    Sets image quality.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void PixelPipe::setImageQuality(VGImageQuality imageQuality)
{
    RI_ASSERT(imageQuality == VG_IMAGE_QUALITY_NONANTIALIASED || imageQuality == VG_IMAGE_QUALITY_FASTER || imageQuality == VG_IMAGE_QUALITY_BETTER);
    m_imageQuality = imageQuality;
}

/*-------------------------------------------------------------------*//*!
* \brief    Sets fill color for VG_TILE_FILL tiling mode (pattern only).
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void PixelPipe::setTileFillColor(const Color& c)
{
    m_tileFillColor = c;
    m_tileFillColor.clamp();
}

/*-------------------------------------------------------------------*//*!
* \brief    Sets paint.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void PixelPipe::setPaint(Paint* paint)
{
    // \temp Only call this right before filling a polygon.
    m_paint = paint;

    if(!m_paint)
        m_paint = &m_defaultPaint;

    if(m_paint->m_pattern)
        m_tileFillColor.convert(m_paint->m_pattern->getDescriptor().internalFormat);

}

/*-------------------------------------------------------------------*//*!
* \brief    Color transform.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void PixelPipe::setColorTransform(bool enable, RIfloat values[8])
{
    m_colorTransform = enable;
    for(int i=0;i<4;i++)
    {
        m_colorTransformValues[i] = RI_CLAMP(values[i], -127.0f, 127.0f);
        m_colorTransformValues[i+4] = RI_CLAMP(values[i+4], -1.0f, 1.0f);
        m_iColorTransformValues[i] = RI_ROUND_TO_INT(m_colorTransformValues[i]*(RIfloat)COLOR_TRANSFORM_ONE);
        m_iColorTransformValues[i+4] = RI_ROUND_TO_INT(m_colorTransformValues[i+4]*255.0f);
    }
    m_colorTransformChanged = true;
}

const Image* PixelPipe::getRenderTargetImage() const
{
    if (m_renderToMask)
        return m_drawable->getMaskBuffer()->getImage();

    return m_drawable->getColorBuffer()->getImage();
}

/**
 * \brief   Determine an appropriate VGImageFormat to use for lookup tables.
 * \todo    Should return descriptor instead?
 */
VGImageFormat PixelPipe::getPreferredLUTFormat() const
{
    const Image* target = getRenderTargetImage();
    const Color::Descriptor& targetDesc = target->getDescriptor();

    if (m_renderToMask)
    {
        RI_ASSERT(!m_image);
        if (targetDesc.isNonlinear())
            return VG_sRGBA_8888_PRE;
        else
            return VG_lRGBA_8888_PRE;
    }

    if (m_image && m_imageMode == VG_DRAW_IMAGE_MULTIPLY)
        return VG_sRGBA_8888_PRE; // ?

    // Prefer premultiplied formats
    // \note Can also generate non-premultiplied if no sampling/other operation and destination
    // is in linear format.
    // \note Do not use VGImageFormat, because using (s/l)LA88 is possible with
    // luminance destination formats.
    if (targetDesc.isNonlinear())
        return VG_sRGBA_8888_PRE;
    else
        return VG_lRGBA_8888_PRE;
}

void PixelPipe::prepareSolidFill()
{
    if (!(m_drawable && m_paint))
        return;
        
    Color c = m_paint->getSolidColor();
    //Color c = m_paint->m_paintColor;
    
    if (!m_image || m_imageMode != VG_DRAW_IMAGE_MULTIPLY)
        colorTransform(c); // Output will be premultiplied
    // Generate internal color
    Color::Descriptor blendDesc = getRenderTargetImage()->getDescriptor();

    // MULTIPLY uses the color as-is.
    if (m_imageMode != VG_DRAW_IMAGE_MULTIPLY) c.convert(blendDesc.internalFormat);

    IntegerColor ic = IntegerColor(c);
    blendDesc.internalFormat = (Color::InternalFormat)(blendDesc.internalFormat | (Color::PREMULTIPLIED));

    if (m_imageMode != VG_DRAW_IMAGE_MULTIPLY) c.convert(blendDesc.internalFormat);
    
    IntegerColor blendColor = IntegerColor(c);

    if (m_imageMode == VG_DRAW_IMAGE_STENCIL)
        blendColor.asFixedPoint(c); // Enhance the precision a bit
    
    // \todo No need to pack the color if solid fill is not possible
    if (!m_renderToMask)
        ic.truncateColor(getRenderTargetImage()->getDescriptor());
    else
        ic.truncateMask(getRenderTargetImage()->getDescriptor());

    RIuint32 p = ic.getPackedColor(getRenderTargetImage()->getDescriptor());

    m_spanUniforms.solidColor = blendColor; // This must be premultiplied
    m_spanUniforms.packedSolidColor = p; // This must be exactly the dst color
}

void PixelPipe::prepareCoverageFill()
{
    IntegerColor ic = IntegerColor(255, 255, 255, 255);
    RIuint32 p = ic.getPackedColor(m_drawable->getMaskBuffer()->getDescriptor());

    m_spanUniforms.solidColor = ic;
    m_spanUniforms.packedSolidColor = p;
}

void PixelPipe::prepareLinearGradient()
{
    const Matrix3x3& s2p = m_surfaceToPaintMatrix;

    Vector2 zero(0,0);
    Vector2 p0 = m_paint->m_linearGradientPoint0;
    Vector2 p1 = m_paint->m_linearGradientPoint1;
    Vector2 delta = p1 - p0;

    zero = affineTransform(s2p, zero);

    RIfloat d = (delta.x * delta.x) + (delta.y * delta.y);
    RIfloat gdx = (s2p[0][0] * delta.x + s2p[1][0] * delta.y) / d;
    RIfloat gdy = (s2p[0][1] * delta.x + s2p[1][1] * delta.y) / d;
    RIfloat cx = (zero.x-p0.x) * (delta.x);
    RIfloat cy = (zero.y-p0.y) * (delta.y);
    RIfloat c = (cx + cy) / d;

    m_spanUniforms.dgdx = RI_FLOAT_TO_FX(gdx, PixelPipe::GRADIENT_BITS);
    m_spanUniforms.dgdy = RI_FLOAT_TO_FX(gdy, PixelPipe::GRADIENT_BITS);
    m_spanUniforms.lgc = RI_FLOAT_TO_FX(c + 0.5*(gdx + gdy), PixelPipe::GRADIENT_BITS);

    m_spanUniforms.gradientLookup = m_paint->getGradientLUT();
}

void PixelPipe::prepareRadialGradient()
{
    const Matrix3x3& s2p = m_surfaceToPaintMatrix;

    Vector2 c = m_paint->m_radialGradientCenter;
    Vector2 f = m_paint->m_radialGradientFocalPoint;
    RGScalar r = m_paint->m_radialGradientRadius;

    Vector2 zero(0,0);
    Vector2 pzero = affineTransform(s2p, zero);

    Vector2 fp = f - c;

    RGScalar q = fp.length();

    if (q > r)
    {
        const RIfloat scale = 0.99f;
        fp.normalize();
        fp *= r * scale;
        f = fp + c;
    } 

    RGScalar r1sqr = RI_SQR(r);
    RGScalar d = r1sqr - dot(fp, fp);

    m_spanUniforms.rdxdx = s2p[0][0];
    m_spanUniforms.rdxdy = s2p[0][1];
    m_spanUniforms.rdydx = s2p[1][0];
    m_spanUniforms.rdydy = s2p[1][1];

    m_spanUniforms.rsqrp = r1sqr / RI_SQR(d);
    m_spanUniforms.rfxp = fp.x / d;
    m_spanUniforms.rfyp = fp.y / d;
    m_spanUniforms.rx0 = pzero.x - f.x + 0.5f*(m_spanUniforms.rdxdx + m_spanUniforms.rdxdy);
    m_spanUniforms.ry0 = pzero.y - f.y + 0.5f*(m_spanUniforms.rdydy + m_spanUniforms.rdydx);

    m_spanUniforms.gradientLookup = m_paint->getGradientLUT();
}

void PixelPipe::preparePattern()
{
    // Patterns only support affine transforms
    const Matrix3x3& s2p = m_surfaceToPaintMatrix;
    const RIfloat patternWidth = (RIfloat)m_paint->m_pattern->getWidth();
    const RIfloat patternHeight = (RIfloat)m_paint->m_pattern->getHeight();
    const Vector2 zero(0, 0);
    Vector2 pzero = affineTransform(s2p, zero); 

    m_spanUniforms.paint_x0 = RI_ROUND_TO_INT((pzero.x/patternWidth)*(1<<GRADIENT_BITS));
    m_spanUniforms.paint_y0 = RI_ROUND_TO_INT((pzero.y/patternHeight)*(1<<GRADIENT_BITS));
    m_spanUniforms.paint_dxdx = RI_ROUND_TO_INT((s2p[0][0]/patternWidth)*(1<<GRADIENT_BITS));
    m_spanUniforms.paint_dxdy = RI_ROUND_TO_INT((s2p[0][1]/patternHeight)*(1<<GRADIENT_BITS));
    m_spanUniforms.paint_dydx = RI_ROUND_TO_INT((s2p[1][0]/patternWidth)*(1<<GRADIENT_BITS));
    m_spanUniforms.paint_dydy = RI_ROUND_TO_INT((s2p[1][1]/patternHeight)*(1<<GRADIENT_BITS));

    m_spanUniforms.paint_x0 += (m_spanUniforms.paint_dxdx + m_spanUniforms.paint_dxdy) / 2;
    m_spanUniforms.paint_y0 += (m_spanUniforms.paint_dydy + m_spanUniforms.paint_dydx) / 2;

    m_spanUniforms.patternPtr = m_paint->m_pattern->getData(); 
    m_spanUniforms.patternStride = m_paint->m_pattern->getStride();
    m_spanUniforms.paint_width = m_paint->m_pattern->getWidth();
    m_spanUniforms.paint_height = m_paint->m_pattern->getHeight();

    m_signatureState.patternDesc = m_paint->m_pattern->getDescriptor();

    m_spanUniforms.tileFillColor = IntegerColor(m_tileFillColor);
    // The tile fill-color must be shifted down to same bit-depth (see integer samplers)
    m_spanUniforms.tileFillColor.truncateColor(m_signatureState.patternDesc);

}

RI_INLINE static RIfloat floatEqu(RIfloat a, RIfloat b, RIfloat e)
{
    // \note This should be sufficient for our use-cases;
    return (RI_ABS(a - b) < e);
}

RI_INLINE static RIfloat distToInt(RIfloat f)
{
    const RIfloat intF = RI_ROUND_TO_INT(f);
    return RI_ABS(intF - f);
}

/**
 * \brief   Check if transform is 90 degree rotation, or flip and nothing else.
 */
RI_INLINE static bool orthoNormalCoAxialTransform(const Matrix3x3& t, bool aa)
{
    const RIfloat epsilonCoord = 1/255.0f; // 1/127.0f;
    const RIfloat epsilonGradient = epsilonCoord * epsilonCoord; // \todo Too strict?
    const RIfloat absPatterns[2][4] = {
        {1.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 1.0f, 0.0f} };

    if (!t.isAffine())
        return false;

    // \todo This rule only applies if filtering is in use?
    if (aa)
        if (!floatEqu(distToInt(t[0][2]), 0.0f, epsilonCoord) || !floatEqu(distToInt(t[1][2]), 0.0f, epsilonCoord))
            return false;

    Matrix3x3 u = t;
    
    for (int j = 0; j < 2; j++)
        for (int i = 0; i < 2; i++)
            u[j][i] = RI_ABS(u[j][i]);

    bool found;

    for (int m = 0; m < 2; m++)
    {
        found = true;
        for (int j = 0; j < 2; j++)
        {
            for (int i = 0; i < 2; i++)
            {
                //if (u[j][i] != absPatterns[m][i+j*2])
                if (!floatEqu(u[j][i], absPatterns[m][i+j*2], epsilonGradient))
                {
                    found = false;
                    break;
                }
            }
            if (!found) break;
        }
        if (found) break;
    }

    return found;
}

void PixelPipe::prepareImage(bool aa)
{
    if (!m_image)
    {
        m_signatureState.imageGradientType = GRADIENT_TYPE_INTEGER;
        return;
    }

    RI_ASSERT(m_image);
    m_spanUniforms.imagePtr = m_image->getData();
    m_spanUniforms.imageStride = m_image->getStride();
    
    if (m_image->getParent() != NULL)
    {
        // Adjust the pointer.
        int x, y;
        m_image->getStorageOffset(x, y);
        m_spanUniforms.imagePtr = Image::calculateAddress(
            m_spanUniforms.imagePtr, m_image->getDescriptor().bitsPerPixel, x, y, m_spanUniforms.imageStride);
    }

    // \todo This function writes to derived state also.
    // \todo Plenty of fast-paths possible!
    const Matrix3x3& s2i = m_surfaceToImageMatrix;
    
    Vector3 zero(0,0,1);
    Vector3 pzero;

    bool fastImage = orthoNormalCoAxialTransform(s2i, aa);
    
    pzero = s2i * zero;

    if (fastImage)
    {
        RI_ASSERT(pzero.z == 1.0f);
        m_spanUniforms.image_idxdx = RI_ROUND_TO_INT(s2i[0][0]);
        m_spanUniforms.image_idxdy = RI_ROUND_TO_INT(s2i[0][1]);
        m_spanUniforms.image_idydx = RI_ROUND_TO_INT(s2i[1][0]);
        m_spanUniforms.image_idydy = RI_ROUND_TO_INT(s2i[1][1]);
        m_spanUniforms.image_ix0 = RI_FLOOR(pzero.x + 0.5f*(s2i[0][0]+s2i[0][1]));
        m_spanUniforms.image_iy0 = RI_FLOOR(pzero.y + 0.5f*(s2i[1][1]+s2i[1][0]));

        // Adjust sample-center when using (exactly) integer coordinates.
        
#if 0
        if (m_spanUniforms.image_idxdx < 0 || m_spanUniforms.image_idxdy < 0)
            m_spanUniforms.image_ix0--;
    
        if (m_spanUniforms.image_idydy < 0 || m_spanUniforms.image_idydx < 0)
            m_spanUniforms.image_iy0--;
#endif

        m_signatureState.imageGradientType = GRADIENT_TYPE_INTEGER;
    } 
    else if (s2i.isAffine())
    {
        RI_ASSERT(pzero.z == 1.0f); 
        const RIfloat imageWidth = m_image->getWidth();
        const RIfloat imageHeight = m_image->getHeight();
        
        m_spanUniforms.image_idxdx = RI_ROUND_TO_INT((s2i[0][0]/imageWidth)*(1<<GRADIENT_BITS));
        m_spanUniforms.image_idxdy = RI_ROUND_TO_INT((s2i[0][1]/imageHeight)*(1<<GRADIENT_BITS));
        m_spanUniforms.image_idydx = RI_ROUND_TO_INT((s2i[1][0]/imageWidth)*(1<<GRADIENT_BITS));
        m_spanUniforms.image_idydy = RI_ROUND_TO_INT((s2i[1][1]/imageHeight)*(1<<GRADIENT_BITS));
        m_spanUniforms.image_ix0 = RI_ROUND_TO_INT((pzero.x/imageWidth)*(1<<GRADIENT_BITS));
        m_spanUniforms.image_iy0 = RI_ROUND_TO_INT((pzero.y/imageHeight)*(1<<GRADIENT_BITS));

        m_spanUniforms.image_ix0 += (m_spanUniforms.image_idxdx + m_spanUniforms.image_idxdy)/2;
        m_spanUniforms.image_iy0 += (m_spanUniforms.image_idydy + m_spanUniforms.image_idydx)/2;

        m_spanUniforms.image_iWidth = (RIint32)imageWidth;
        m_spanUniforms.image_iHeight = (RIint32)imageHeight;

        m_signatureState.imageGradientType = GRADIENT_TYPE_FIXED;
    } 
    else
    {
        // Use floats.
        m_spanUniforms.image_fx0 = pzero.x;
        m_spanUniforms.image_fy0 = pzero.y;
        m_spanUniforms.image_fw0 = pzero.z;
        m_spanUniforms.image_fdxdx = s2i[0][0];
        m_spanUniforms.image_fdxdy = s2i[0][1];
        m_spanUniforms.image_fdydx = s2i[1][0];
        m_spanUniforms.image_fdydy = s2i[1][1];
        m_spanUniforms.image_fdwdx = s2i[2][0];
        m_spanUniforms.image_fdwdy = s2i[2][1];

        m_spanUniforms.image_fx0 += 0.5f * (m_spanUniforms.image_fdxdx + m_spanUniforms.image_fdxdy);
        m_spanUniforms.image_fy0 += 0.5f * (m_spanUniforms.image_fdydy + m_spanUniforms.image_fdydx);
        m_spanUniforms.image_fw0 += 0.5f * (m_spanUniforms.image_fdwdx + m_spanUniforms.image_fdwdy);

        m_spanUniforms.image_fWidth = (RIfloat)m_image->getWidth();
        m_spanUniforms.image_fHeight = (RIfloat)m_image->getHeight();

        m_signatureState.imageGradientType = GRADIENT_TYPE_FLOAT;
    }

    m_signatureState.imageDesc = m_image->getDescriptor();
}

static PixelPipe::TilingMode tilingModeOfImageTilingMode(VGTilingMode it)
{
    switch(it)
    {
    case VG_TILE_PAD:
        return PixelPipe::TILING_MODE_PAD;
    case VG_TILE_REPEAT:
        return PixelPipe::TILING_MODE_REPEAT;
    case VG_TILE_REFLECT:
        return PixelPipe::TILING_MODE_REFLECT;
    default:
        RI_ASSERT(it == VG_TILE_FILL);
        return PixelPipe::TILING_MODE_FILL;
    }
}

static PixelPipe::TilingMode tilingModeOfSpreadMode(VGColorRampSpreadMode sm)
{
    switch(sm)
    {
    case VG_COLOR_RAMP_SPREAD_PAD:
        return PixelPipe::TILING_MODE_PAD;
    case VG_COLOR_RAMP_SPREAD_REPEAT:
        return PixelPipe::TILING_MODE_REPEAT;
    default:
        RI_ASSERT(sm == VG_COLOR_RAMP_SPREAD_REFLECT);
        return PixelPipe::TILING_MODE_REFLECT;
    }
}

static PixelPipe::TilingMode tilingModeOfPaint(const Paint* paint)
{
    switch(paint->m_paintType)
    {
    case VG_PAINT_TYPE_COLOR:
        return PixelPipe::TILING_MODE_PAD;
    case VG_PAINT_TYPE_LINEAR_GRADIENT:
    case VG_PAINT_TYPE_RADIAL_GRADIENT:
        return tilingModeOfSpreadMode(paint->m_colorRampSpreadMode);
    default:
        RI_ASSERT(paint->m_paintType == VG_PAINT_TYPE_PATTERN);
        return tilingModeOfImageTilingMode(paint->m_patternTilingMode);
    }
}

void PixelPipe::prepareRenderToMask()
{
    RI_ASSERT(m_drawable->getMaskBuffer());

    m_signatureState.dstDesc = m_drawable->getMaskBuffer()->getDescriptor();
    //RI_ASSERT(m_signatureState.dstFormat >= 0 && m_signatureState.dstFormat <= VG_lABGR_8888_PRE);

    m_signatureState.maskOperation = m_maskOperation;
}

void PixelPipe::prepareSignatureState()
{
    m_signatureState.isRenderToMask = m_renderToMask;

    if (m_signatureState.isRenderToMask)
    {
        prepareRenderToMask();
        return;
    }

    m_signatureState.blendMode = getBlendMode();

    m_signatureState.hasColorTransform = this->m_colorTransform;

    m_signatureState.paintType = getPaintType();
    
    m_signatureState.paintTilingMode = tilingModeOfPaint(m_paint);
    // \todo Derive these from the quality settings somehow.
    // Linear and nearest should work atm.
    m_signatureState.paintSampler = SAMPLER_TYPE_NEAREST;
    m_signatureState.imageSampler = SAMPLER_TYPE_NEAREST;

    m_signatureState.hasMasking = isMasking() && (m_drawable->getMaskBuffer() != NULL);
    
    m_signatureState.hasImage = m_image ? true : false;
    m_signatureState.unsafeImageInput = !m_image ? false : m_image->isUnsafe();
    m_signatureState.imageMode = m_imageMode;

    // Formats. Note that fields that are not filled in / used get set to a derived state in a 
    // separate function!

    if (m_signatureState.paintType == (RIuint32)VG_PAINT_TYPE_COLOR)
    {
        RI_ASSERT(m_paint);
        if (m_paint->getSolidColor().a == 1.0)
            m_signatureState.fillColorTransparent = false;
        else
            m_signatureState.fillColorTransparent = true;
    }

    m_signatureState.dstDesc = m_drawable->getColorBuffer()->getDescriptor();

    // \todo Why isn't the imagedescriptor set here?
    if (m_signatureState.hasMasking)
    {
        m_signatureState.maskDesc = m_drawable->getMaskBuffer()->getDescriptor();
    }

}

/**
 * \brief   Remove redundancy from the pixel-pipeline state so that less
 *          pipelines are generated.
 */
static void determineDerivedState(PixelPipe::SignatureState& derivedState, const PixelPipe::SignatureState& originalState)
{
    derivedState = originalState;

    if (derivedState.isRenderToMask)
    {
        // Set a lot of defaults:
        derivedState.blendMode = VG_BLEND_SRC;
        derivedState.imageMode = VG_DRAW_IMAGE_NORMAL;
        derivedState.paintType = VG_PAINT_TYPE_COLOR;

        derivedState.hasImage = false;
        derivedState.hasMasking = false;
        derivedState.hasColorTransform = false;
    }

    if (derivedState.paintType == VG_PAINT_TYPE_COLOR)
    {
        derivedState.paintTilingMode = PixelPipe::TILING_MODE_PAD;
        derivedState.paintSampler = PixelPipe::SAMPLER_TYPE_NEAREST;
        // \todo Opaque solid colors can benefit from simpler coverage-blending
        // becase SRC_OVER == SRC. This information has to be present in
        // the derivedState (and not just uniform).
    }

    if (!derivedState.hasImage)
    {
        derivedState.imageMode = VG_DRAW_IMAGE_NORMAL;
        derivedState.imageSampler = PixelPipe::SAMPLER_TYPE_NEAREST;
        derivedState.imageGradientType = PixelPipe::GRADIENT_TYPE_INTEGER;
        derivedState.imageDesc = Color::Descriptor::getDummyDescriptor();
    } else if (derivedState.imageMode == VG_DRAW_IMAGE_NORMAL)
    {
        // If paint is not generated, use a common enum
        derivedState.paintType = VG_PAINT_TYPE_COLOR;
    }

    if (derivedState.paintType != VG_PAINT_TYPE_PATTERN)
    {
        derivedState.patternDesc = Color::Descriptor::getDummyDescriptor();
    }

    if (!derivedState.isRenderToMask)
        derivedState.maskOperation = VG_CLEAR_MASK;

    if (!derivedState.hasMasking)
    {
        derivedState.maskDesc = Color::Descriptor::getDummyDescriptor();
    }
}


/**
 * \brief   Determine per-scanconversion constant state.
 * \todo    NOTE! This also prepares the derived state at the moment.
 */
void PixelPipe::prepareSpanUniforms(bool aa)
{
    prepareSignatureState();

    if (m_signatureState.hasColorTransform)
        m_spanUniforms.colorTransformValues = m_iColorTransformValues;

    RI_ASSERT(m_drawable->getColorBuffer());

    const Image* dst;

    if (!m_signatureState.isRenderToMask)
        dst = m_drawable->getColorBuffer()->getImage();
    else
        dst = m_drawable->getMaskBuffer()->getImage();
    
    m_spanUniforms.dstPtr = dst->getData();
    m_spanUniforms.dstStride = dst->getStride();

    if (m_drawable->getMaskBuffer())
    {
        m_spanUniforms.maskPtr = m_drawable->getMaskBuffer()->m_image->getData();
        m_spanUniforms.maskStride = m_drawable->getMaskBuffer()->m_image->getStride();
    }
    else
    {
        m_spanUniforms.maskPtr = NULL;
        m_spanUniforms.maskStride = 0;
    }

    if (!m_renderToMask)
    {
        VGImageFormat prefPaintFormat = getPreferredLUTFormat();

        switch (getPaintType())
        {
            case VG_PAINT_TYPE_COLOR:
                prepareSolidFill();
                break;
            case VG_PAINT_TYPE_LINEAR_GRADIENT:
                m_paint->generateLUT(*this, prefPaintFormat);
                prepareLinearGradient(); 
                break;
            case VG_PAINT_TYPE_RADIAL_GRADIENT:
                m_paint->generateLUT(*this, prefPaintFormat);
                prepareRadialGradient();
                break;
            default:
                RI_ASSERT(getPaintType() == VG_PAINT_TYPE_PATTERN);
                preparePattern();
                break;
        }
    }
    else
    {
        prepareCoverageFill();
    }

    prepareImage(aa);

    // Must be done last:
    determineDerivedState(m_derivedState, m_signatureState);
}


/*-------------------------------------------------------------------*//*!
* \brief    Computes the linear gradient function at (x,y).
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/
void PixelPipe::linearGradient(RIfloat& g, RIfloat& rho, RIfloat x, RIfloat y) const
{
    RI_ASSERT(m_paint);
    Vector2 u = m_paint->m_linearGradientPoint1 - m_paint->m_linearGradientPoint0;
    RIfloat usq = dot(u,u);
    if( usq <= 0.0f )
    {   //points are equal, gradient is always 1.0f
        g = 1.0f;
        rho = 0.0f;
        return;
    }
    RIfloat oou = 1.0f / usq;

    Vector2 p(x, y);
    p = affineTransform(m_surfaceToPaintMatrix, p);
    p -= m_paint->m_linearGradientPoint0;
    RI_ASSERT(usq >= 0.0f);
    g = dot(p, u) * oou;
    RIfloat dgdx = oou * u.x * m_surfaceToPaintMatrix[0][0] + oou * u.y * m_surfaceToPaintMatrix[1][0];
    RIfloat dgdy = oou * u.x * m_surfaceToPaintMatrix[0][1] + oou * u.y * m_surfaceToPaintMatrix[1][1];
    rho = (RIfloat)sqrt(dgdx*dgdx + dgdy*dgdy);
    RI_ASSERT(rho >= 0.0f);
}

/*-------------------------------------------------------------------*//*!
* \brief    Computes the radial gradient function at (x,y).
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void PixelPipe::radialGradient(RIfloat &g, RIfloat &rho, RIfloat x, RIfloat y) const
{
    RI_ASSERT(m_paint);
    if( m_paint->m_radialGradientRadius <= 0.0f )
    {
        g = 1.0f;
        rho = 0.0f;
        return;
    }

    RIfloat r = m_paint->m_radialGradientRadius;
    Vector2 c = m_paint->m_radialGradientCenter;
    Vector2 f = m_paint->m_radialGradientFocalPoint;
    Vector2 gx(m_surfaceToPaintMatrix[0][0], m_surfaceToPaintMatrix[1][0]);
    Vector2 gy(m_surfaceToPaintMatrix[0][1], m_surfaceToPaintMatrix[1][1]);

    Vector2 fp = f - c;

    //clamp the focal point inside the gradient circle
    RIfloat fpLen = fp.length();
    if( fpLen > 0.999f * r )
        fp *= 0.999f * r / fpLen;

    RIfloat D = -1.0f / (dot(fp,fp) - r*r);
    Vector2 p(x, y);
    p = affineTransform(m_surfaceToPaintMatrix, p) - c;
    Vector2 d = p - fp;
    RIfloat s = (RIfloat)sqrt(r*r*dot(d,d) - RI_SQR(p.x*fp.y - p.y*fp.x));
    g = (dot(fp,d) + s) * D;
    if(RI_ISNAN(g))
        g = 0.0f;
    RIfloat dgdx = D*dot(fp,gx) + (r*r*dot(d,gx) - (gx.x*fp.y - gx.y*fp.x)*(p.x*fp.y - p.y*fp.x)) * (D / s);
    RIfloat dgdy = D*dot(fp,gy) + (r*r*dot(d,gy) - (gy.x*fp.y - gy.y*fp.x)*(p.x*fp.y - p.y*fp.x)) * (D / s);
    rho = (RIfloat)sqrt(dgdx*dgdx + dgdy*dgdy);
    if(RI_ISNAN(rho))
        rho = 0.0f;
    RI_ASSERT(rho >= 0.0f);
}

/*-------------------------------------------------------------------*//*!
* \brief    Maps a gradient function value to a color.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

Color PixelPipe::colorRamp(RIfloat gradient, RIfloat rho) const
{
    RI_ASSERT(m_paint);
    RI_ASSERT(rho >= 0.0f);

    Color c(0,0,0,0,m_paint->m_colorRampPremultiplied ? Color::sRGBA_PRE : Color::sRGBA);
    Color avg;

    if(rho == 0.0f)
    {   //filter size is zero or gradient is degenerate
        switch(m_paint->m_colorRampSpreadMode)
        {
        case VG_COLOR_RAMP_SPREAD_PAD:
            gradient = RI_CLAMP(gradient, 0.0f, 1.0f);
            break;
        case VG_COLOR_RAMP_SPREAD_REFLECT:
        {
            RIfloat g = RI_MOD(gradient, 2.0f);
            gradient = (g < 1.0f) ? g : 2.0f - g;
            break;
        }
        default:
            RI_ASSERT(m_paint->m_colorRampSpreadMode == VG_COLOR_RAMP_SPREAD_REPEAT);
            gradient = gradient - (RIfloat)floor(gradient);
            break;
        }
        RI_ASSERT(gradient >= 0.0f && gradient <= 1.0f);

        for(int i=0;i<m_paint->m_colorRampStops.size()-1;i++)
        {
            if(gradient >= m_paint->m_colorRampStops[i].offset && gradient < m_paint->m_colorRampStops[i+1].offset)
            {
                RIfloat s = m_paint->m_colorRampStops[i].offset;
                RIfloat e = m_paint->m_colorRampStops[i+1].offset;
                RI_ASSERT(s < e);
                RIfloat g = RI_CLAMP((gradient - s) / (e - s), 0.0f, 1.0f); //clamp needed due to numerical inaccuracies

                Color sc = readStopColor(m_paint->m_colorRampStops, i, m_paint->m_colorRampPremultiplied);
                Color ec = readStopColor(m_paint->m_colorRampStops, i+1, m_paint->m_colorRampPremultiplied);
                return (1.0f-g) * sc + g * ec;  //return interpolated value
            }
        }
        return readStopColor(m_paint->m_colorRampStops, m_paint->m_colorRampStops.size()-1, m_paint->m_colorRampPremultiplied);
    }

    RIfloat gmin = gradient - rho*0.5f;         //filter starting from the gradient point (if starts earlier, radial gradient center will be an average of the first and the last stop, which doesn't look good)
    RIfloat gmax = gradient + rho*0.5f;

    switch(m_paint->m_colorRampSpreadMode)
    {
    case VG_COLOR_RAMP_SPREAD_PAD:
    {
        if(gmin < 0.0f)
            c += (RI_MIN(gmax, 0.0f) - gmin) * readStopColor(m_paint->m_colorRampStops, 0, m_paint->m_colorRampPremultiplied);
        if(gmax > 1.0f)
            c += (gmax - RI_MAX(gmin, 1.0f)) * readStopColor(m_paint->m_colorRampStops, m_paint->m_colorRampStops.size()-1, m_paint->m_colorRampPremultiplied);
        gmin = RI_CLAMP(gmin, 0.0f, 1.0f);
        gmax = RI_CLAMP(gmax, 0.0f, 1.0f);
        c += m_paint->integrateColorRamp(gmin, gmax);
        c *= 1.0f/rho;
        c.clamp();  //clamp needed due to numerical inaccuracies
        return c;
    }

    case VG_COLOR_RAMP_SPREAD_REFLECT:
    {
        avg = m_paint->integrateColorRamp(0.0f, 1.0f);
        RIfloat gmini = (RIfloat)floor(gmin);
        RIfloat gmaxi = (RIfloat)floor(gmax);
        c = (gmaxi + 1.0f - gmini) * avg;       //full ramps

        //subtract beginning
        if(((int)gmini) & 1)
            c -= m_paint->integrateColorRamp(RI_CLAMP(1.0f - (gmin - gmini), 0.0f, 1.0f), 1.0f);
        else
            c -= m_paint->integrateColorRamp(0.0f, RI_CLAMP(gmin - gmini, 0.0f, 1.0f));

        //subtract end
        if(((int)gmaxi) & 1)
            c -= m_paint->integrateColorRamp(0.0f, RI_CLAMP(1.0f - (gmax - gmaxi), 0.0f, 1.0f));
        else
            c -= m_paint->integrateColorRamp(RI_CLAMP(gmax - gmaxi, 0.0f, 1.0f), 1.0f);
        break;
    }

    default:
    {
        RI_ASSERT(m_paint->m_colorRampSpreadMode == VG_COLOR_RAMP_SPREAD_REPEAT);
        avg = m_paint->integrateColorRamp(0.0f, 1.0f);
        RIfloat gmini = (RIfloat)floor(gmin);
        RIfloat gmaxi = (RIfloat)floor(gmax);
        c = (gmaxi + 1.0f - gmini) * avg;       //full ramps
        c -= m_paint->integrateColorRamp(0.0f, RI_CLAMP(gmin - gmini, 0.0f, 1.0f));  //subtract beginning
        c -= m_paint->integrateColorRamp(RI_CLAMP(gmax - gmaxi, 0.0f, 1.0f), 1.0f);  //subtract end
        break;
    }
    }

    //divide color by the length of the range
    c *= 1.0f / rho;
    c.clamp();  //clamp needed due to numerical inaccuracies

    //hide aliasing by fading to the average color
    const RIfloat fadeStart = 0.5f;
    const RIfloat fadeMultiplier = 2.0f;    //the larger, the earlier fade to average is done

    if(rho < fadeStart)
        return c;

    RIfloat ratio = RI_MIN((rho - fadeStart) * fadeMultiplier, 1.0f);
    return ratio * avg + (1.0f - ratio) * c;
}

/*-------------------------------------------------------------------*//*!
* \brief    Computes blend.
* \param
* \return
* \note     premultiplied blending formulas
            //src
            a = asrc
            r = rsrc
            //src over
            a = asrc + adst * (1-asrc)
            r = rsrc + rdst * (1-asrc)
            //dst over
            a = asrc * (1-adst) + adst
            r = rsrc * (1-adst) + adst
            //src in
            a = asrc * adst
            r = rsrc * adst
            //dst in
            a = adst * asrc
            r = rdst * asrc
            //multiply
            a = asrc + adst * (1-asrc)
            r = rsrc * (1-adst) + rdst * (1-asrc) + rsrc * rdst
            //screen
            a = asrc + adst * (1-asrc)
            r = rsrc + rdst - rsrc * rdst
            //darken
            a = asrc + adst * (1-asrc)
            r = MIN(rsrc + rdst * (1-asrc), rdst + rsrc * (1-adst))
            //lighten
            a = asrc + adst * (1-asrc)
            r = MAX(rsrc + rdst * (1-asrc), rdst + rsrc * (1-adst))
            //additive
            a = MIN(asrc+adst,1)
            r = rsrc + rdst
*//*-------------------------------------------------------------------*/



Color PixelPipe::blend(const Color& s, RIfloat ar, RIfloat ag, RIfloat ab, const Color& d, VGBlendMode blendMode) const
{
    //apply blending in the premultiplied format
    Color r(0,0,0,0,d.getInternalFormat());
    RI_ASSERT(s.a >= 0.0f && s.a <= 1.0f);
    RI_ASSERT(s.r >= 0.0f && s.r <= s.a && s.r <= ar);
    RI_ASSERT(s.g >= 0.0f && s.g <= s.a && s.g <= ag);
    RI_ASSERT(s.b >= 0.0f && s.b <= s.a && s.b <= ab);
    RI_ASSERT(d.a >= 0.0f && d.a <= 1.0f);
    RI_ASSERT(d.r >= 0.0f && d.r <= d.a);
    RI_ASSERT(d.g >= 0.0f && d.g <= d.a);
    RI_ASSERT(d.b >= 0.0f && d.b <= d.a);
    switch(blendMode)
    {
    case VG_BLEND_SRC:
        r = s;
        break;

    case VG_BLEND_SRC_OVER:
        r.r = s.r + d.r * (1.0f - ar);
        r.g = s.g + d.g * (1.0f - ag);
        r.b = s.b + d.b * (1.0f - ab);
        r.a = s.a + d.a * (1.0f - s.a);
        break;

    case VG_BLEND_DST_OVER:
        r.r = s.r * (1.0f - d.a) + d.r;
        r.g = s.g * (1.0f - d.a) + d.g;
        r.b = s.b * (1.0f - d.a) + d.b;
        r.a = s.a * (1.0f - d.a) + d.a;
        break;

    case VG_BLEND_SRC_IN:
        r.r = s.r * d.a;
        r.g = s.g * d.a;
        r.b = s.b * d.a;
        r.a = s.a * d.a;
        break;

    case VG_BLEND_DST_IN:
        r.r = d.r * ar;
        r.g = d.g * ag;
        r.b = d.b * ab;
        r.a = d.a * s.a;
        break;

    case VG_BLEND_MULTIPLY:
        r.r = s.r * (1.0f - d.a + d.r) + d.r * (1.0f - ar);
        r.g = s.g * (1.0f - d.a + d.g) + d.g * (1.0f - ag);
        r.b = s.b * (1.0f - d.a + d.b) + d.b * (1.0f - ab);
        r.a = s.a + d.a * (1.0f - s.a);
        break;

    case VG_BLEND_SCREEN:
        r.r = s.r + d.r * (1.0f - s.r);
        r.g = s.g + d.g * (1.0f - s.g);
        r.b = s.b + d.b * (1.0f - s.b);
        r.a = s.a + d.a * (1.0f - s.a);
        break;

    case VG_BLEND_DARKEN:
        r.r = RI_MIN(s.r + d.r * (1.0f - ar), d.r + s.r * (1.0f - d.a));
        r.g = RI_MIN(s.g + d.g * (1.0f - ag), d.g + s.g * (1.0f - d.a));
        r.b = RI_MIN(s.b + d.b * (1.0f - ab), d.b + s.b * (1.0f - d.a));
        r.a = s.a + d.a * (1.0f - s.a);
        break;

    case VG_BLEND_LIGHTEN:
        r.r = RI_MAX(s.r + d.r * (1.0f - ar), d.r + s.r * (1.0f - d.a));
        r.g = RI_MAX(s.g + d.g * (1.0f - ag), d.g + s.g * (1.0f - d.a));
        r.b = RI_MAX(s.b + d.b * (1.0f - ab), d.b + s.b * (1.0f - d.a));
        //although the statement below is equivalent to r.a = s.a + d.a * (1.0f - s.a)
        //in practice there can be a very slight difference because
        //of the max operation in the blending formula that may cause color to exceed alpha.
        //Because of this, we compute the result both ways and return the maximum.
        r.a = RI_MAX(s.a + d.a * (1.0f - s.a), d.a + s.a * (1.0f - d.a));
        break;

    default:
        RI_ASSERT(blendMode == VG_BLEND_ADDITIVE);
        r.r = RI_MIN(s.r + d.r, 1.0f);
        r.g = RI_MIN(s.g + d.g, 1.0f);
        r.b = RI_MIN(s.b + d.b, 1.0f);
        r.a = RI_MIN(s.a + d.a, 1.0f);
        break;
    }
    return r;
}

/*-------------------------------------------------------------------*//*!
* \brief    Applies color transform.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void PixelPipe::colorTransform(Color& c) const
{
    if(m_colorTransform)
    {
        c.unpremultiply();
        c.luminanceToRGB();
        c.r = c.r * m_colorTransformValues[0] + m_colorTransformValues[4];
        c.g = c.g * m_colorTransformValues[1] + m_colorTransformValues[5];
        c.b = c.b * m_colorTransformValues[2] + m_colorTransformValues[6];
        c.a = c.a * m_colorTransformValues[3] + m_colorTransformValues[7];
        c.clamp();
        c.premultiply();
    }
}

void PixelPipe::fillSpans(PPVariants& variants, const Span* spans, int nSpans) const
{
#if 1
    PPCompiler& compiler = PPCompiler::getCompiler();

    PPCompiler::PixelPipeHandle handle = compiler.compilePixelPipeline(m_derivedState);
    if (handle)
    {
        PixelPipeFunction func = compiler.getPixelPipePtr(handle);
        RI_ASSERT(func);
        func(m_spanUniforms, variants, spans, nSpans);
        compiler.releasePixelPipeline(handle);
    } else
#endif
    {
        executePixelPipeline(m_derivedState, m_spanUniforms, variants, spans, nSpans);
    }
}

//=======================================================================

}   //namespace OpenVGRI

