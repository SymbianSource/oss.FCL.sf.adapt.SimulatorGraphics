#ifndef __RIRASTERIZER_H
#define __RIRASTERIZER_H

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
 * \brief   Rasterizer class.
 * \note
 *//*-------------------------------------------------------------------*/

#ifndef __RIMATH_H
#include "riMath.h"
#endif

#ifndef __RIARRAY_H
#include "riArray.h"
#endif

#ifndef __RIPIXELPIPE_H
#include "riPixelPipe.h"
#endif

// TESTING ONLY!!
#include "sfDynamicPixelPipe.h"

#if defined(RI_DEBUG)
#    include <stdio.h>
#endif
// \todo SSE2 probably has everything necessary and matches the minimum platform requirements.
// SSE4 is probably too new. SSE3?
// \note This will cause the code to not compile on MSVC...

#if defined(USE_SSE2)
// \todo only the last include is needed: QT creator does not find types if others not included.
#   include <mmintrin.h>
#   include <xmmintrin.h>
#   include <emmintrin.h>
#endif

//=======================================================================

namespace OpenVGRI
{

/*-------------------------------------------------------------------*//*!
* \brief    Scalar and vector data types used by the rasterizer.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

typedef RIfloat RScalar;    //change this if you want to have different precision for rasterizer scalars and RIfloat

struct RVector2
{
    RI_INLINE RVector2()                            { }
    RI_INLINE RVector2(const Vector2& v)            { x = v.x; y = v.y; }
    RI_INLINE RVector2(RIfloat vx, RIfloat vy)      { x = vx; y = vy; }
    RI_INLINE void set(RIfloat vx, RIfloat vy)      { x = vx; y = vy; }
    RScalar     x;
    RScalar     y;
};

struct IVector2
{
    RI_INLINE IVector2() {}
    RI_INLINE IVector2(const IVector2& v) { x = v.x; y = v.y; }
    RI_INLINE IVector2(const Vector2& v) { x = (int)v.x; y = (int) v.y; }
    RI_INLINE IVector2(int vx, int vy) { x = vx; y = vy; }
    RI_INLINE void set(int vx, int vy) { x = vx; y = vy; }
    int x;
    int y;
};

struct Span
{
    RIuint16 x0;
    RIuint16 y;
    RIuint16 len;
    RIuint8  coverage;
    RIuint8  pad; // align to 8 bytes. \temp Use qualifiers instead?
};

/*-------------------------------------------------------------------*//*!
* \brief    Converts a set of edges to coverage values for each pixel and
*           calls PixelPipe::pixelPipe for painting a pixel.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

class Rasterizer
{
public:
    Rasterizer();   //throws bad_alloc
    ~Rasterizer();

    void        setup(int vpx, int vpy, int vpwidth, int vpheight, VGFillRule fillRule, const PixelPipe* pixelPipe);
    void        setScissor(const Array<Rectangle>& scissors);   //throws bad_alloc
    void        setScissoring(bool enabled);

    void        clear();
    void        addEdge(const Vector2& v0, const Vector2& v1);  //throws bad_alloc

    RI_INLINE void setAntiAliasing(bool enable) { m_aa = enable; }
    void        fill(); //throws bad_alloc

    void        getBBox(int& sx, int& sy, int& ex, int& ey) const       { sx = m_covMinx; sy = m_covMiny; ex = m_covMaxx; ey = m_covMaxy; }

public:
    enum { SAMPLE_BITS = 3 };
    enum { Y_BITS = SAMPLE_BITS };
    enum { MAX_COVERAGE = 1<<SAMPLE_BITS };

    // temp!:
    RIuint32    *m_covBuffer;
    size_t      m_covBufferSz;

private:
    Rasterizer(const Rasterizer&);                      //!< Not allowed.
    const Rasterizer& operator=(const Rasterizer&);     //!< Not allowed.

public:
    struct ScissorEdge
    {
        ScissorEdge() {}
        bool operator<(const ScissorEdge& e) const  { return x < e.x; }
        int         x;
        int         miny;
        int         maxy;
        int         direction;      //1 start, -1 end
    };

    struct Edge
    {
        Edge() {}
        bool operator<(const Edge& e) const { return v0.y < e.v0.y; }
#if defined(RI_DEBUG)
        bool operator<=(const Edge& e) const {return v0.y <= e.v0.y; }
#endif
        IVector2    v0;
        IVector2    v1;
        int         direction;
    };

    struct ActiveEdge
    {
        typedef RIint16 XCoord;
        typedef RIint16 YCoord;

        ActiveEdge() {}
        //bool operator<(const ActiveEdge& e) const   { return minx < e.minx; }
        bool operator<(const ActiveEdge& e) const
        {
            if (minx < e.minx)
                return true;
            if (minx == e.minx)
                return maxx < e.maxx;
            return false;
        }
#if defined(RI_DEBUG)
        bool operator<=(const ActiveEdge& e) const { return minx <= e.minx; }
#endif
        // Fixed-point edge coordinates.
        RIuint32 next; // \todo Pointer?
        int     direction;      //-1 down, 1 up
        int     xRef;
        int     slope;
        XCoord  minx;           // for the current scanline
        XCoord  maxx;           // for the current scanline
        YCoord  yStart;         // for the edge
        YCoord  yEnd;           // for the edge
    };

    enum { SF_SAMPLES = 8 };
    enum { RASTERIZER_BITS = 14 };
    enum { RASTERIZER_MASK = ((1<<RASTERIZER_BITS)-1) };
    enum { Y_MASK = ((1<<Y_BITS)-1) };
    enum { X_BITS = 7 };
    enum { XMASK  = ((1<<X_BITS)-1) };
    enum { LEFT_DISCARD = -0x100000 };
    enum { LEFT_DISCARD_SHORT = -32768 };

    struct Windings
    {
#if !defined(USE_SSE2)
        Windings() {wq = 0;}
        // \todo Use SSE counters or packed counters!
        // \todo 8 samples -> 64 bits == 2 32-bit uints
        union
        {
            RIint8              winding[SF_SAMPLES];
            RIuint32            wd[SF_SAMPLES/4];
            unsigned long long  wq;
        };
#else
        Windings() { sseWinding = _mm_setzero_si128(); }
        __m128i sseWinding;
#endif
     };

private:
    void                pushEdge(const Edge& edge);
    void                clipAndAddEdge(Edge& edge);
    void                addBBox(const IVector2& v);
    RI_INLINE void      pushSpan(int x, int y, int len, int coverage);
    RI_INLINE void      commitSpans();

    //RI_INLINE void      calculateAEWinding(const ActiveEdge& currAe, Windings& pixel, Windings& scanline, int intY, int pixelX);

    Array<ActiveEdge>   m_edgePool;
    Array<RIuint32>     m_edges;
    Array<ScissorEdge>  m_scissorEdges;
    Array<ActiveEdge>   m_aet;
    Array<ScissorEdge>  m_scissorAet;
    bool                m_scissor;

    bool                    m_aa;

    IVector2            m_edgeMin;
    IVector2            m_edgeMax;
    int                 m_covMinx;
    int                 m_covMiny;
    int                 m_covMaxx;
    int                 m_covMaxy;
    int                 m_vpx;
    int                 m_vpy;
    int                 m_vpwidth;
    int                 m_vpheight;
    int                 m_vpMinx;
    int                 m_vpMiny;
    int                 m_vpMaxx;
    int                 m_vpMaxy;
    VGFillRule          m_fillRule;
    RIuint32            m_fillRuleMask;

    const PixelPipe*        m_pixelPipe;
    PixelPipe::PPVariants   m_ppVariants;

    enum { N_CACHED_SPANS = 64 };
    Span                m_spanCache[N_CACHED_SPANS];
    int                 m_nSpans;

};

//=======================================================================

}   //namespace OpenVGRI

//=======================================================================

#endif /* __RIRASTERIZER_H */
