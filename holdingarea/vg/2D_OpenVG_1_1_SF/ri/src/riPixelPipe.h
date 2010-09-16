#ifndef __RIPIXELPIPE_H
#define __RIPIXELPIPE_H

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
 * \brief	Paint and PixelPipe classes.
 * \note
 *//*-------------------------------------------------------------------*/

#ifndef __RIMATH_H
#include "riMath.h"
#endif

#ifndef __RIIMAGE_H
#include "riImage.h"
#endif

//=======================================================================

namespace OpenVGRI
{

struct Span;
class PPCompiler;
class PixelPipe;

/*-------------------------------------------------------------------*//*!
* \brief	Storage and operations for VGPaint.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

class Paint
{
public:
    enum { GRADIENT_LUT_BITS = 8 };
    enum { GRADIENT_LUT_COUNT = 1 << GRADIENT_LUT_BITS };
    enum { GRADIENT_LUT_MASK = (1<<GRADIENT_LUT_BITS)-1 };

    struct GradientStop
    {
        GradientStop() : offset(0.0f), color(0.0f, 0.0f, 0.0f, 0.0f, Color::sRGBA) {}
        RIfloat		offset;
        Color		color;
    };

public:
    Paint();
    ~Paint();
    void					addReference()							{ m_referenceCount++; }
    int						removeReference()						{ m_referenceCount--; RI_ASSERT(m_referenceCount >= 0); return m_referenceCount; }
    void                    setColor(const Color& color) {m_paintColor = color; m_paintColor.clamp(); m_paintColor.premultiply(); }
    void                    setGradientStops(Array<GradientStop>& inputStops, Array<GradientStop>& stops);
    void                    generateLUT(PixelPipe& pipe, VGImageFormat targetFormat);
    const IntegerColor*     getGradientLUT() const { return m_gradientLUT; }
    void                    setLinearGradient(const Vector2& p0, const Vector2& p1);
    void                    setRadialGradient(const Vector2& c, const Vector2& f, VGfloat r);
    bool                    linearDegenerate() const;
    bool                    radialDegenerate() const;
    Color                   getSolidColor() const;
    
    Color integrateColorRamp(RIfloat gmin, RIfloat gmax) const; // \todo Private after modifications.
    
public:
    VGPaintType				m_paintType;
    Color					m_paintColor;
    Color					m_inputPaintColor;
    VGColorRampSpreadMode	m_colorRampSpreadMode;
    Array<GradientStop>		m_colorRampStops;
    Array<GradientStop>		m_inputColorRampStops;
    VGboolean				m_colorRampPremultiplied;
    Vector2					m_inputLinearGradientPoint0;
    Vector2					m_inputLinearGradientPoint1;
    Vector2					m_inputRadialGradientCenter;
    Vector2					m_inputRadialGradientFocalPoint;
    RIfloat					m_inputRadialGradientRadius;
    Vector2					m_linearGradientPoint0;
    Vector2					m_linearGradientPoint1;
    Vector2					m_radialGradientCenter;
    Vector2					m_radialGradientFocalPoint;
    RIfloat					m_radialGradientRadius;
    VGTilingMode			m_patternTilingMode;
    Image*					m_pattern;
private:
    Paint(const Paint&);						//!< Not allowed.
    const Paint& operator=(const Paint&);		//!< Not allowed.

    int						m_referenceCount;
    IntegerColor            m_gradientLUT[GRADIENT_LUT_COUNT];
    VGImageFormat           m_lutFormat;
    bool                    m_gradientStopsChanged;
    bool                    m_gradientDegenerate;
};

/*-------------------------------------------------------------------*//*!
* \brief	Encapsulates all information needed for painting a pixel.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

#define RGRAD_FLOATS
#if defined(RGRAD_FLOATS)
typedef RIfloat RGScalar;
#else
typedef double RGScalar;
#endif

class PixelPipe
{
public:
    enum SamplerType 
    {
        SAMPLER_TYPE_NEAREST    = 0,
        SAMPLER_TYPE_LINEAR     = 1,
        SAMPLER_TYPE_SIZE
    };
    
    enum TilingMode 
    {
        TILING_MODE_PAD         = 0,
        TILING_MODE_REPEAT      = 1,
        TILING_MODE_REFLECT     = 2,
        TILING_MODE_FILL        = 3,
        TILING_MODE_SIZE
    };
    
    // Span per-pixel variants:
    struct PPVariants
    {
        void*       dst;
        void*       src;
        void*       maskPtr;
        int         coverage;

        RIuint32    dstX;

        RIint32     sx;
        RIint32     sy;

        RGScalar    rx;
        RGScalar    ry;

        // \todo Image sampling coordinates will be in fixed point if transform is affine,
        // in floating point if not.
        RGScalar    ix;
        RGScalar    iy;

        RIint32     iImageX;  
        RIint32     iImageY;
        RIfloat     fImageX;
        RIfloat     fImageY;
        RIfloat     fImageW;
    };
    
    // Uniform state per-pixel
    // \todo Organize into sub-structures?
    struct PPUniforms
    {
        // \todo Do not store pointers to classes, only atoms! It should make the
        // dynamic compilation a lot easier.
        void*           srcPtr;
        RIint32         srcStride;
        void*           dstPtr;
        RIint32         dstStride;
        void*           maskPtr;
        int             maskStride;
        void*           imagePtr;
        int             imageStride;
        void*           patternPtr;
        int             patternStride;
        const IntegerColor*   gradientLookup;
        const RIint32*        colorTransformValues;

        // Linear gradient
        RIint32         dgdx;
        RIint32         dgdy;
        RIint32         lgc;

        // Radial gradient
        RGScalar        rsqrp;
        RGScalar        rfxp;
        RGScalar        rfyp;
        RGScalar        rx0;
        RGScalar        ry0;
        RGScalar        rdxdx;
        RGScalar        rdxdy;
        RGScalar        rdydx;
        RGScalar        rdydy;

        // Pattern. Note that pattern and image may be used at the same time.
        RIint32         paint_width;
        RIint32         paint_height;
        RIint32         paint_x0;
        RIint32         paint_y0;
        RIint32         paint_dxdx;
        RIint32         paint_dxdy;
        RIint32         paint_dydx;
        RIint32         paint_dydy;

        // Image
        RIint32         image_iWidth;
        RIint32         image_iHeight;
        RIint32         image_ix0;
        RIint32         image_iy0;
        RIint32         image_idxdx;
        RIint32         image_idxdy;
        RIint32         image_idydx;
        RIint32         image_idydy;


        RIfloat         image_fWidth;
        RIfloat         image_fHeight;
        RIfloat         image_fx0;
        RIfloat         image_fy0;
        RIfloat         image_fw0;
        RIfloat         image_fdxdx;
        RIfloat         image_fdxdy;
        RIfloat         image_fdydx;
        RIfloat         image_fdydy;
        RIfloat         image_fdwdx;
        RIfloat         image_fdwdy;

        IntegerColor    tileFillColor;
        IntegerColor    solidColor;
        RIuint32        packedSolidColor;
    };

    enum ImageGradientType {
        GRADIENT_TYPE_INTEGER   = 0,
        GRADIENT_TYPE_FIXED     = 1,
        GRADIENT_TYPE_FLOAT     = 2,
        GRADIENT_TYPE_SIZE
    };

    // Signature state contains all the information necessary to compile
    // a pixel-pipeline. Note that some of these are actually derived.
    // \note REMEMBER TO UPDATE THE COMPILER. For now, there is now
    // automatic mechanism to propagate changes to that component!
    struct SignatureState
    {
        VGBlendMode         blendMode;
        VGImageMode         imageMode;
        VGPaintType         paintType;
        VGMaskOperation     maskOperation;
        TilingMode          paintTilingMode;
        SamplerType         paintSampler;
        SamplerType         imageSampler;

        ImageGradientType   imageGradientType;

        Color::Descriptor   dstDesc;
        Color::Descriptor   maskDesc;
        Color::Descriptor   imageDesc;
        Color::Descriptor   patternDesc;

        bool                hasMasking;
        bool                hasImage;
        bool                hasColorTransform;
        bool                isRenderToMask;
        bool                fillColorTransparent;
        // When using external data for rendering an image: This is the only case
        // where the data can be invalid in the pixel-pipe.
        bool                unsafeImageInput; 

    };

public:
    PixelPipe();	//throws bad_alloc
    ~PixelPipe();

    void	pixelPipe(int x, int y, RIuint32 coverage) const;	//rasterizer calls this function for each pixel
    void 	fillSolidSpan(int startX, int y, int nPixels) const;
    void	setDrawable(Drawable* drawable);
    void	setBlendMode(VGBlendMode blendMode);
    RI_INLINE VGBlendMode getBlendMode() const { return m_blendMode; }
    void    setRenderToMask(bool renderToMask) { m_renderToMask = renderToMask; }
    void    setMaskOperation(VGMaskOperation maskOperation) { m_maskOperation = maskOperation; }
    void	setMask(bool masking);
    void	setImage(Image* image, VGImageMode imageMode);	//image = NULL disables drawImage functionality
    void	setSurfaceToPaintMatrix(const Matrix3x3& surfaceToPaintMatrix);
    void	setSurfaceToImageMatrix(const Matrix3x3& surfaceToImageMatrix);
    void	setImageQuality(VGImageQuality imageQuality);
    void	setTileFillColor(const Color& c);
    void	setPaint(Paint* paint);
    void    setColorTransform(bool enable, RIfloat values[8]);
    bool    hasColorTransform() const { return m_colorTransform; }
    RI_INLINE const SignatureState& getSignatureState() const { return m_signatureState; }

    // Functions that determine parts of derived state.
    void    prepareSpanUniforms(bool aa);

    RI_INLINE VGPaintType getPaintType() const;
    RI_INLINE bool isMasking() const;
    void fillSpans(PPVariants& variants, const Span* spans, int nSpans) const;

    void    colorTransform(Color& c) const;
    void    setColorTransformChanged(bool changed) { m_colorTransformChanged = changed; } // make paint friend and this private!
    bool    colorTransformChanged() const { return m_colorTransformChanged; }
    RI_INLINE VGImageMode getImageMode() const { return m_imageMode; }

    RI_INLINE static bool isImageOnly(const SignatureState& state);

private:

    const Image*    getRenderTargetImage() const;
    VGImageFormat   getPreferredLUTFormat() const;

    void	prepareSolidFill();
    void    prepareCoverageFill();
    void    prepareLinearGradient();
    void    prepareRadialGradient();
    void    preparePattern();
    void    prepareImage(bool aa);
    void    prepareSignatureState();
    void    prepareRenderToMask();
    void	linearGradient(RIfloat& g, RIfloat& rho, RIfloat x, RIfloat y) const;
    void	radialGradient(RIfloat& g, RIfloat& rho, RIfloat x, RIfloat y) const;
    Color	colorRamp(RIfloat gradient, RIfloat rho) const;
    Color	blend(const Color& s, RIfloat ar, RIfloat ag, RIfloat ab, const Color& d, VGBlendMode blendMode) const;

    PixelPipe(const PixelPipe&);						//!< Not allowed.
    const PixelPipe& operator=(const PixelPipe&);		//!< Not allowed.

    Drawable*               m_drawable;
    bool					m_masking;
    Image*					m_image;
    // \todo LUT within the paint class broke constness of paint.
    Paint*			        m_paint;
    Paint					m_defaultPaint;
    VGBlendMode				m_blendMode;
    VGImageMode				m_imageMode;
    VGImageQuality			m_imageQuality;
    Color					m_tileFillColor;
    bool                    m_colorTransform;
    RIfloat                 m_colorTransformValues[8];
    RIint32                 m_iColorTransformValues[8];
    Matrix3x3				m_surfaceToPaintMatrix;
    Matrix3x3				m_surfaceToImageMatrix;
    Matrix3x3               m_paintToSurfaceMatrix;
    VGMaskOperation         m_maskOperation;
    bool                    m_renderToMask;
    bool                    m_colorTransformChanged;

public:

    enum { COLOR_TRANSFORM_BITS = 8 };
    enum { COLOR_TRANSFORM_ONE = (1<<COLOR_TRANSFORM_BITS) };
    enum { COLOR_TRANSFORM_MASK = (COLOR_TRANSFORM_ONE - 1) };
    enum { GRADIENT_BITS = 16 };
    enum { GRADIENT_MASK = (1<<GRADIENT_BITS)-1 };
    enum { SAMPLE_BITS = 8 };
    enum { SAMPLE_MASK = (1<<SAMPLE_BITS)-1 };

private:


    SignatureState          m_signatureState;
    SignatureState          m_derivedState;

    PPUniforms  m_spanUniforms;
};

RI_INLINE VGPaintType PixelPipe::getPaintType() const
{
    if (m_paint->m_paintType == VG_PAINT_TYPE_COLOR)
        return VG_PAINT_TYPE_COLOR;

    if (m_paint->m_paintType == VG_PAINT_TYPE_PATTERN && !m_paint->m_pattern)
        return VG_PAINT_TYPE_COLOR;

    if (m_paint->m_paintType == VG_PAINT_TYPE_LINEAR_GRADIENT && m_paint->linearDegenerate())
        return VG_PAINT_TYPE_COLOR;

    if (m_paint->m_paintType == VG_PAINT_TYPE_RADIAL_GRADIENT && m_paint->radialDegenerate())
        return VG_PAINT_TYPE_COLOR;

    return m_paint->m_paintType;
}

RI_INLINE bool PixelPipe::isMasking() const
{
    return m_masking;
}

RI_INLINE /*static*/ bool PixelPipe::isImageOnly(const SignatureState& state)
{
    if (state.hasImage)
        return (state.imageMode == VG_DRAW_IMAGE_NORMAL) ? true : false;
    else
        return false;
}

//=======================================================================

}	//namespace OpenVGRI

//=======================================================================

#endif /* __RIPIXELPIPE_H */
