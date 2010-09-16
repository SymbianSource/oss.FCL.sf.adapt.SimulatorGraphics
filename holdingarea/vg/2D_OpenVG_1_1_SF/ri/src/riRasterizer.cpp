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
 * \brief	Implementation of polygon rasterizer.
 * \note
 *//*-------------------------------------------------------------------*/

#include "riRasterizer.h"

// TEMP!
#ifndef __SFCOMPILER_H
#   include "sfCompiler.h"
#endif


namespace OpenVGRI
{

/*-------------------------------------------------------------------*//*!
* \brief	Rasterizer constructor.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

Rasterizer::Rasterizer() :
    m_covBuffer(NULL),
    m_covBufferSz(0),
    m_edges(),
    m_scissorEdges(),
    m_scissor(false),
    m_aa(true),
    m_vpx(0),
    m_vpy(0),
    m_vpwidth(0),
    m_vpheight(0),
    m_fillRule(VG_EVEN_ODD),
    m_pixelPipe(NULL),
    m_nSpans(0)
{}

/*-------------------------------------------------------------------*//*!
* \brief	Rasterizer destructor.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

Rasterizer::~Rasterizer()
{
    if(m_covBuffer)
        RI_DELETE_ARRAY(m_covBuffer);
}

/*-------------------------------------------------------------------*//*!
* \brief	Removes all appended edges.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

#define EDGE_TERMINATOR 0xFFFFFFFFu

void Rasterizer::clear()
{
    //m_edges.clear();
    for (int i = 0; i < m_edges.size(); i++)
        m_edges[i] = EDGE_TERMINATOR;

    m_edgePool.clear();

    m_edgeMin.set(0x7fffffffu, 0x7fffffffu);
    m_edgeMax.set(0x80000000, 0x80000000);
}

/*-------------------------------------------------------------------*//*!
* \brief	Appends an edge to the rasterizer.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void Rasterizer::addBBox(const IVector2& v)
{
    if(v.x < m_edgeMin.x) m_edgeMin.x = v.x;
    if(v.y < m_edgeMin.y) m_edgeMin.y = v.y;
    if(v.x > m_edgeMax.x) m_edgeMax.x = v.x;
    if(v.y > m_edgeMax.y) m_edgeMax.y = v.y;
}

void Rasterizer::pushEdge(const Edge& edge)
{
    addBBox(edge.v0);
    addBBox(edge.v1);

    // Only add processed edges.

    RI_ASSERT(edge.v0.y >= 0);
    RI_ASSERT(edge.v0.y < edge.v1.y);	//horizontal edges should have been dropped already

    ActiveEdge ae;
    ae.direction = edge.direction;

    // \todo Adjust for non-AA cases
    // \todo verySteep is temporary. Either clip to right edge also, or validate that a proper slope can be
    // calculated here.
    const int slope	= RI_SAT_SHL((edge.v1.x - edge.v0.x), RASTERIZER_BITS - X_BITS) / (edge.v1.y - edge.v0.y);
    //const bool verySteep = RI_INT_ABS(edge.v1.x - edge.v0.x) > (1 << (30-RASTERIZER_BITS)) ? true : false;
    //const int slope	= verySteep ? 1 << 30 : RI_SHL((edge.v1.x - edge.v0.x), RASTERIZER_BITS - X_BITS) / (edge.v1.y - edge.v0.y);
    // slope: SI.(RASTERIZER_BITS - Y_BITS)
    const int yF 	= edge.v0.y & Y_MASK;
    // \todo See verySteep note for this hack also. (Clip to right edge?)
    const int xRef 	= RI_SAT_SHL(edge.v0.x, RASTERIZER_BITS - X_BITS) - (yF * slope);
    //const int xRef 	= edge.v0.x > (1<<(30-RASTERIZER_BITS)) ? 1<<30 : RI_SHL(edge.v0.x, RASTERIZER_BITS - X_BITS) - (yF * slope);

    RI_ASSERT(RI_INT_ABS(edge.v0.y <= 32767));
    RI_ASSERT(RI_INT_ABS(edge.v1.y <= 32767));

    ae.yStart 		= (RIint16)edge.v0.y;
    ae.yEnd 	  	= (RIint16)edge.v1.y;
    ae.xRef 		= xRef;
    ae.slope 		= slope;
    // Scanline range.
    ae.minx         = xRef >> RASTERIZER_BITS;
    ae.maxx         = (xRef + slope * (1<<Y_BITS)) >> RASTERIZER_BITS;

    if (ae.minx > ae.maxx)
        RI_ANY_SWAP(ActiveEdge::XCoord, ae.minx, ae.maxx);

    if (ae.maxx < 0)
        ae.minx = ae.maxx = LEFT_DISCARD_SHORT;

    if (m_edges[ae.yStart>>Y_BITS] == EDGE_TERMINATOR)
        ae.next = EDGE_TERMINATOR;
    else
        ae.next = m_edges[ae.yStart>>Y_BITS];

    m_edgePool.push_back(ae);	//throws bad_alloc

    RI_ASSERT(m_edgePool.size() > 0);
    m_edges[ae.yStart>>Y_BITS] = m_edgePool.size()-1;
}

/**
 * \brief   Clips an edge and if something remains, adds it to the list of edges.
 * \todo    Enhance precision: Currently this just uses doubles and gets away with
 *          it in most cases.
 */
void Rasterizer::clipAndAddEdge(Edge& edge)
{
    //if (m_edges.size() > 48)
        //return;
    // Check y-clips
    // \todo Reduce amount of clips.
    bool outLeft[2] = {(edge.v0.x < m_vpMinx), (edge.v1.x < m_vpMinx)};
    bool outRight[2] = {(edge.v0.x > m_vpMaxx), (edge.v1.x > m_vpMaxx)};
    bool outTop[2] = {(edge.v0.y < m_vpMiny), (edge.v1.y < m_vpMiny)};
    bool outBottom[2] = {(edge.v0.y > m_vpMaxy), (edge.v1.y > m_vpMaxy)};

    if (!(outLeft[0] || outLeft[1] || outRight[0] || outRight[1] || outTop[0] || outTop[1] || outBottom[0] || outBottom[1]))
    {
        pushEdge(edge);
        return;
    }

    // \todo Make sure that checking out-of-right works with the scanconverter.
    if ((outBottom[0] && outBottom[1]) || (outTop[0] && outTop[1]))
        return; // Out of bounds

    // \todo Clip to right edge of screen.
    // \todo Make slope-calculation and signs consistent.
    //
    if (outTop[0] || outBottom[1])
    {
        // Clip to top/bottom.
        double slope = (double)(edge.v1.x - edge.v0.x)/(edge.v1.y - edge.v0.y);

        if (outTop[0])
        {
            RI_ASSERT(-(RIint64)edge.v0.y >= 0);
            RIint32 dx = RI_ROUND_TO_INT(-slope * edge.v0.y);
            edge.v0.y = 0;
            edge.v0.x += dx;
        }

        if (outBottom[1])
        {
            RIint32 dy = edge.v1.y - m_vpMaxy;
            RI_ASSERT(dy >= 0);
            RIint32 dx = -RI_ROUND_TO_INT(slope * dy);
            edge.v1.y = m_vpMaxy;
            edge.v1.x += dx;
        }

    }

    if (edge.v0.y >= edge.v1.y)
        return;

    // \todo Recheck left/right.
    outLeft[0] = (edge.v0.x < m_vpMinx); outLeft[1] = (edge.v1.x < m_vpMinx);
    outRight[1] = (edge.v0.x > m_vpMaxx); outRight[1] = (edge.v1.x > m_vpMaxx);

    if (outLeft[0] && outLeft[1])
    {
        edge.v0.x = m_vpMinx;
        edge.v1.x = m_vpMinx;
        pushEdge(edge);
        return;
    }
    if (outRight[0] && outRight[1])
    {
        edge.v0.x = m_vpMaxx;
        edge.v1.x = m_vpMaxx;
        pushEdge(edge);
        return;
    }

    // From outside -> screen
    if (outLeft[0] || outRight[1])
    {
        // infinite slope?
        double slope = (double)((RIint64)edge.v1.y - edge.v0.y)/((RIint64)edge.v1.x - edge.v0.x);

        if (outLeft[0])
        {
            RIint32 dx = edge.v0.x;
            //RI_ASSERT(dx >= 0);
            // Note the sign.
            RIint32 dy = RI_ROUND_TO_INT(-slope * dx);

            Edge vpart = edge;
            vpart.v1.y = edge.v0.y + dy;
            //vpart.v1.x = edge.v0.x; // = 0?
            // \note This should be flagged instead of setting the smallest possible
            // value because of extremely gentle slopes may cause bugs:
            vpart.v1.x = vpart.v0.x = -0x100000;

            if (vpart.v1.y > vpart.v0.y)
                pushEdge(vpart);

            edge.v0.y += dy;
            edge.v0.x = 0;
        }
    }
    // From screen -> outside
    if (outLeft[1] || outRight[0])
    {
        // infinite slope?
        double slope = (double)((RIint64)edge.v1.y - edge.v0.y)/((RIint64)edge.v1.x - edge.v0.x);

        if (outLeft[1])
        {
            RIint32 dx = edge.v0.x;
            RI_ASSERT(dx >= 0);
            RIint32 dy = RI_ROUND_TO_INT(-slope * dx);

            Edge vpart = edge;
            vpart.v0.y = edge.v0.y + dy;
            vpart.v1.x = vpart.v0.x = LEFT_DISCARD;

            if (vpart.v1.y > vpart.v0.y)
                pushEdge(vpart);

            edge.v1.y = edge.v0.y + dy;
            edge.v1.x = 0;
        }
    }

    if (edge.v0.y >= edge.v1.y)
        return;

    // Finally, add the edge:
    pushEdge(edge);
}

void Rasterizer::addEdge(const Vector2& v0, const Vector2& v1)
{
    if( m_edges.size() >= RI_MAX_EDGES )
        throw std::bad_alloc();	//throw an out of memory error if there are too many edges

    Edge e;

    {
        IVector2 i0(RI_ROUND_TO_INT(v0.x * (1<<X_BITS)), RI_ROUND_TO_INT(v0.y * (1<<Y_BITS)));
        IVector2 i1(RI_ROUND_TO_INT(v1.x * (1<<X_BITS)), RI_ROUND_TO_INT(v1.y * (1<<Y_BITS)));

        if(i0.y == i1.y)
            return;	//skip horizontal edges (they don't affect rasterization since we scan horizontally)

        if (i0.y < i1.y)
        {
            // Edge is going upward
            e.v0 = i0;
            e.v1 = i1;
            e.direction = 1;
        }
        else
        {
            // Edge is going downward
            e.v0 = i1;
            e.v1 = i0;
            e.direction = -1;
        }
    }

    // Clip and insert.

    clipAndAddEdge(e);
}

/*-------------------------------------------------------------------*//*!
* \brief	Set up rasterizer
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void Rasterizer::setup(int vpx, int vpy, int vpwidth, int vpheight, VGFillRule fillRule, const PixelPipe* pixelPipe)
{
    RI_ASSERT(vpwidth >= 0 && vpheight >= 0);
    RI_ASSERT(vpx + vpwidth >= vpx && vpy + vpheight >= vpy);
    RI_ASSERT(fillRule == VG_EVEN_ODD || fillRule == VG_NON_ZERO);
    RI_ASSERT(pixelPipe);

    clear();

    m_vpx = vpx;
    m_vpy = vpy;
    m_vpwidth = vpwidth;
    m_vpheight = vpheight;

    if (m_vpheight > m_edges.size())
    {
        int os = m_edges.size();
        m_edges.resize(m_vpheight);
        for (int i = os; i < m_edges.size(); i++)
            m_edges[i] = EDGE_TERMINATOR;
    }

    m_vpMinx = RI_SHL(vpx, X_BITS);
    m_vpMiny = RI_SHL(vpy, Y_BITS);
    m_vpMaxx = RI_SHL(vpx + vpwidth, X_BITS);
    m_vpMaxy = RI_SHL(vpy + vpheight, Y_BITS);

    m_fillRule = fillRule;

    RIuint32 fillRuleMask = fillRule == VG_NON_ZERO ? 0xffffffffu : 1;
    m_fillRuleMask = fillRuleMask;

    m_pixelPipe = pixelPipe;
    m_covMinx = vpx+vpwidth;
    m_covMiny = vpy+vpheight;
    m_covMaxx = vpx;
    m_covMaxy = vpy;
}

/*-------------------------------------------------------------------*//*!
* \brief	Sets scissor rectangles.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void Rasterizer::setScissor(const Array<Rectangle>& scissors)
{
    try
    {
        m_scissorEdges.clear();
        for(int i=0;i<scissors.size();i++)
        {
            if(scissors[i].width > 0 && scissors[i].height > 0)
            {
                ScissorEdge e;
                e.miny = scissors[i].y;
                e.maxy = RI_INT_ADDSATURATE(scissors[i].y, scissors[i].height);

                e.x = scissors[i].x;
                e.direction = 1;
                m_scissorEdges.push_back(e);	//throws bad_alloc
                e.x = RI_INT_ADDSATURATE(scissors[i].x, scissors[i].width);
                e.direction = -1;
                m_scissorEdges.push_back(e);	//throws bad_alloc
            }
        }
    }
    catch(std::bad_alloc)
    {
        m_scissorEdges.clear();
        throw;
    }
}

void Rasterizer::setScissoring(bool enabled)
{
    m_scissor = enabled;
}

static RI_INLINE void small_memcpy32(void* dst, const void* src, size_t n)
{
    RIuint32 *d = (RIuint32*)dst;
    const RIuint32 *s = (const RIuint32*)src;
    while(n)
    {
        *d++ = *s++;
        n-=4;
    }
}

// \todo Move this to some debug file or remove.
#if defined(USE_SSE2) && !defined(_WIN32)
RI_INLINE static void print128(__m128i ll)
{
#if defined(RI_DEBUG)
    unsigned long long v[2];
    _mm_storeu_pd((double*)v, (__m128d)ll);
    RI_PRINTF("0x%016llx %016llx\n", v[0], v[1]);
#else
    (void)ll;
#endif
}
#endif

#if defined(USE_SSE2)
RI_INLINE static __m128i mm_mul4x32(const __m128i a, const __m128i b) {
     __m128i res;
#if (_MSC_VER > 1400 )
     // \todo Simpler way to do this on intel?
     __m128i m0 = _mm_mul_epu32(a, _mm_shuffle_epi32(b, _MM_SHUFFLE(1, 1, 0, 0)));
     __m128i m1 = _mm_mul_epu32(a, _mm_shuffle_epi32(b, _MM_SHUFFLE(3, 3, 2, 2)));

     res = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(m0), _mm_castsi128_ps(m1), _MM_SHUFFLE(2, 0, 2, 0)));
#else
     __asm {
         movdqa xmm1, a;
         movdqa xmm2, b;
         pshufd xmm3, xmm2, 80;
         movdqa xmm0, xmm1;

         pshufd xmm2, xmm2, 250;
         pmuludq xmm0, xmm3;
         pmuludq xmm1, xmm2;

         shufps xmm0, xmm1, 136;
         movdqa res, xmm0;
     }
#endif
     return res;
}
#endif

#if defined(USE_SSE2)
RI_INLINE static void mm_get_xmasks(const __m128i& coords, const __m128i& sampleCoords, __m128i& slWindMask, __m128i& pxWindMask)
{
    const __m128i z = _mm_setzero_si128();
    const __m128i xMask = _mm_cmpeq_epi16(_mm_srai_epi16(coords, Rasterizer::RASTERIZER_BITS), z);
    const __m128i sCmp = _mm_or_si128(_mm_cmpgt_epi16(sampleCoords, coords), _mm_cmpeq_epi16(sampleCoords, coords));
    //const __m128i sCmp = _mm_cmplt_epi16(coords, sampleCoords);
    slWindMask = xMask;
    pxWindMask = _mm_and_si128(xMask, sCmp);
}
#endif

RI_INLINE static void getVerticalSubpixels(int iY, int yStart, int yEnd, int& py0, int& py1)
{
    const int cy = iY << Rasterizer::Y_BITS;
    py0 = cy > yStart ? 0 : yStart & Rasterizer::Y_MASK;
    py1 = (RI_INT_MIN(yEnd, cy + (1<<Rasterizer::Y_BITS)) - 1) & Rasterizer::Y_MASK;
}

RI_INLINE static void applyLeftEdge(const Rasterizer::ActiveEdge& currAe, Rasterizer::Windings& scanline, int intY)
{
    // Applies the whole edge at a time. Make sure xRight < x for all y.
    // \todo Remove duplicate code for determining the active samples
#if defined(USE_SSE2)
    int py0, py1;

    getVerticalSubpixels(intY, currAe.yStart, currAe.yEnd, py0, py1);

    const __m128i csteps = _mm_set_epi16(7,6,5,4,3,2,1,0);

    const __m128i ssePy0 = _mm_set1_epi16(py0-1);
    const __m128i ssePy1 = _mm_set1_epi16(py1+1);

    const __m128i yMask = _mm_and_si128(_mm_cmpgt_epi16(csteps, ssePy0), _mm_cmplt_epi16(csteps, ssePy1));
    const __m128i dir = _mm_set1_epi16(currAe.direction);

    scanline.sseWinding = _mm_add_epi16(scanline.sseWinding, _mm_and_si128(yMask, dir));

#else
    RI_ASSERT(false); // Not implemented yet.
#endif
}

RI_INLINE static void applyLeftEdgeNoAA(const Rasterizer::ActiveEdge& currAe, Rasterizer::Windings& scanline, int intY)
{
    // Applies the whole edge at a time. Make sure xRight < x for all y.
    // \todo Remove duplicate code for determining the active samples?
#if defined(USE_SSE2)
    int py0, py1;

    getVerticalSubpixels(intY, currAe.yStart, currAe.yEnd, py0, py1);

    //const __m128i csteps = _mm_set_epi16(4,4,4,4,4,4,4,4);

    __m128i yMask;

    if (py0 <= 4 && py1 >= 4)
        yMask = _mm_set1_epi8(-1);
    else
        yMask = _mm_set1_epi8(0);

    const __m128i dir = _mm_set1_epi16(currAe.direction);

    scanline.sseWinding = _mm_add_epi16(scanline.sseWinding, _mm_and_si128(yMask, dir));
    //scanline.sseWinding = _mm_add_epi32(scanline.sseWinding, dir);

#else
    RI_ASSERT(false); // Not implemented yet.
#endif
}

RI_INLINE void calculateAEWinding(const Rasterizer::ActiveEdge& currAe, Rasterizer::Windings& pixel, Rasterizer::Windings& scanline, int intY, int pixelX)
{
#define QUEEN_COORD(Y) ((Y<<(Rasterizer::RASTERIZER_BITS - Rasterizer::SAMPLE_BITS)) + (1<<(Rasterizer::RASTERIZER_BITS-Rasterizer::SAMPLE_BITS-1)))

#if !defined(USE_SSE2)
    static const int queenCoords[(1<<Rasterizer::SAMPLE_BITS)] = {
        QUEEN_COORD(3), QUEEN_COORD(7), QUEEN_COORD(0), QUEEN_COORD(2),
        QUEEN_COORD(5), QUEEN_COORD(1), QUEEN_COORD(6), QUEEN_COORD(4)
    };

    const int ix = pixelX >> Rasterizer::RASTERIZER_BITS;
    const int cy = intY << Rasterizer::Y_BITS;

    const int py0 = cy > currAe.yStart ? 0 : currAe.yStart & Rasterizer::Y_MASK;
    const int py1 = (RI_INT_MIN(currAe.yEnd, cy + (1<<Rasterizer::Y_BITS)) - 1) & Rasterizer::Y_MASK;

    int edgeX = currAe.xRef + (cy + py0 - (currAe.yStart & ~Rasterizer::Y_MASK)) * currAe.slope;

    RI_ASSERT(py1 >= py0);

    for (int s = py0; s <= py1; s++)
    {
        const int sampleX = pixelX + queenCoords[s];

        //compute winding number by evaluating the edge functions of edges to the left of the sampling point
        if(((edgeX >> Rasterizer::RASTERIZER_BITS) == ix))
        {
            if (sampleX >= edgeX)
            {
                pixel.winding[s] += currAe.direction;
            }
            scanline.winding[s] += currAe.direction;
        }

        edgeX += currAe.slope;
    }
#else

    __m128i qCoords = _mm_set_epi16(
        QUEEN_COORD(4), QUEEN_COORD(6), QUEEN_COORD(1), QUEEN_COORD(5),
        QUEEN_COORD(2), QUEEN_COORD(0), QUEEN_COORD(7), QUEEN_COORD(3));

    RI_ASSERT(Rasterizer::RASTERIZER_BITS <= 14);

    // TEROP: Optimize conditions.
    int py0, py1;
    getVerticalSubpixels(intY, currAe.yStart, currAe.yEnd, py0, py1);

    const int cy = intY << Rasterizer::Y_BITS;

    const __m128i csteps0 = _mm_set_epi32(3,2,1,0);
    const __m128i csteps1 = _mm_set_epi32(7,6,5,4);

    const __m128i ssePy0 = _mm_set1_epi32(py0-1);
    const __m128i ssePy1 = _mm_set1_epi32(py1+1);

    const __m128i yMask0 = _mm_and_si128(_mm_cmpgt_epi32(csteps0, ssePy0), _mm_cmplt_epi32(csteps0, ssePy1));
    const __m128i yMask1 = _mm_and_si128(_mm_cmpgt_epi32(csteps1, ssePy0), _mm_cmplt_epi32(csteps1, ssePy1));

    const int edgeX = currAe.xRef + (cy - (currAe.yStart & ~Rasterizer::Y_MASK)) * currAe.slope;
    const __m128i xStart = _mm_set1_epi32(edgeX - pixelX);

    const __m128i xs0 = _mm_set1_epi32(currAe.slope);

    __m128i xAdd0 = mm_mul4x32(xs0, csteps0);
    __m128i xAdd1 = mm_mul4x32(xs0, csteps1);
    __m128i coords0 = _mm_add_epi32(xStart, xAdd0);
    __m128i coords1 = _mm_add_epi32(xStart, xAdd1);
    __m128i coords = _mm_packs_epi32(coords0, coords1);

    __m128i dir = _mm_set1_epi16(currAe.direction);
    __m128i yMask = _mm_packs_epi32(yMask0, yMask1);
    __m128i mDir = _mm_and_si128(dir, yMask);

    __m128i sampleCoords = qCoords;

    __m128i sw, pw;
    mm_get_xmasks(coords, sampleCoords, sw, pw);

    pixel.sseWinding = _mm_add_epi16(pixel.sseWinding, _mm_and_si128(pw, mDir));
    scanline.sseWinding = _mm_add_epi16(scanline.sseWinding, _mm_and_si128(sw, mDir));
#endif

#undef QUEEN_COORD

}

/**
 * \brief   Calculate winding using one sample only.
 * \note    This uses most of the same code as the AA-case even though it is not
 *          necessary (one sample would be enough).
 */
RI_INLINE void calculateAEWindingNoAA(const Rasterizer::ActiveEdge& currAe, Rasterizer::Windings& pixel, Rasterizer::Windings& scanline, int intY, int pixelX)
{
#if defined(USE_SSE2)

#define QUEEN_COORD(Y) ((Y<<(Rasterizer::RASTERIZER_BITS - Rasterizer::SAMPLE_BITS)) + (1<<(Rasterizer::RASTERIZER_BITS-Rasterizer::SAMPLE_BITS-1)))
    const int half = 1<<(Rasterizer::RASTERIZER_BITS-1);

    __m128i sampleCoords = _mm_set1_epi16(half);

    RI_ASSERT(Rasterizer::RASTERIZER_BITS <= 14);

    const int cy = intY << Rasterizer::Y_BITS;

    int py0, py1;
    getVerticalSubpixels(intY, currAe.yStart, currAe.yEnd, py0, py1);

    __m128i yMask;

    if (py0 <= 4 && py1 >= 4)
        yMask = _mm_set1_epi8(-1);
    else
        yMask = _mm_set1_epi8(0);

    const __m128i csteps0 = _mm_set_epi32(4,4,4,4);
    const __m128i csteps1 = _mm_set_epi32(4,4,4,4);

    const int edgeX = currAe.xRef + (cy - (currAe.yStart & ~Rasterizer::Y_MASK)) * currAe.slope;
    const __m128i xStart = _mm_set1_epi32(edgeX - pixelX);

    const __m128i xs0 = _mm_set1_epi32(currAe.slope);

    __m128i xAdd0 = mm_mul4x32(xs0, csteps0);
    __m128i xAdd1 = mm_mul4x32(xs0, csteps1);
    __m128i coords0 = _mm_add_epi32(xStart, xAdd0);
    __m128i coords1 = _mm_add_epi32(xStart, xAdd1);
    __m128i coords = _mm_packs_epi32(coords0, coords1);

    __m128i dir = _mm_set1_epi16(currAe.direction);
    __m128i mDir = _mm_and_si128(dir, yMask);
    //__m128i mDir = dir;

    __m128i sw, pw;
    mm_get_xmasks(coords, sampleCoords, sw, pw);

    pixel.sseWinding = _mm_add_epi16(pixel.sseWinding, _mm_and_si128(pw, mDir));
    scanline.sseWinding = _mm_add_epi16(scanline.sseWinding, _mm_and_si128(sw, mDir));

#undef QUEEN_COORD

#else
    RI_ASSERT(false); // Not implemented.
#endif
}

#if defined(USE_SSE2)
RI_INLINE static int mm_winding_to_coverage(const Rasterizer::Windings& pixel, int fillRuleMask)
{
    // This version uses SSE2 counters.
    __m128i mask = _mm_set1_epi16(fillRuleMask);
    __m128i t = _mm_and_si128(mask, pixel.sseWinding);
    __m128i z = _mm_setzero_si128();
    __m128i isz = _mm_cmpeq_epi16(t, z);
    __m128i ones = _mm_set1_epi16(1);
    __m128i res = _mm_add_epi16(ones, isz);
    __m128i add0 = _mm_add_epi16(res, _mm_shuffle_epi32(res, _MM_SHUFFLE(2, 3, 2, 3)));
    __m128i add1 = _mm_add_epi16(add0, _mm_shuffle_epi32(add0, _MM_SHUFFLE(1, 1, 1, 1)));
    __m128i add2 = _mm_add_epi16(add1, _mm_shufflelo_epi16(add1, _MM_SHUFFLE(1, 1, 1, 1)));

    int nSamples = _mm_cvtsi128_si32(add2) & 0xff;
    return nSamples;
}
#endif

#define RI_DEBUG
#if defined(RI_DEBUG)
void maybeDumpEdges(Array<Rasterizer::ActiveEdge> &edgePool)
{
    return;
    // \note This gives an idea about the edges at the rasterization stage.
    // Input edges must be output at a different stage.
    RI_PRINTF("lines = []\n");
    for (int i = 0 ; i < edgePool.size(); i++)
    {
        const int slope = edgePool[i].slope;
        int x0, x1, y0, y1;
        y0 = edgePool[i].yStart;
        y1 = edgePool[i].yEnd;
        x0 = edgePool[i].xRef + (slope * (y0 & Rasterizer::Y_MASK));
        x1 = (edgePool[i].xRef + (slope * (y1 - (y0 & ~Rasterizer::Y_MASK))))>>(Rasterizer::RASTERIZER_BITS-Rasterizer::X_BITS);
        RI_PRINTF("lines += [[%d, %d], [%d, %d]]\n",x0>>(Rasterizer::RASTERIZER_BITS-Rasterizer::X_BITS),y0,x1,y1);
    }
}
#endif

/*-------------------------------------------------------------------*//*!
* \brief	Calls PixelPipe::pixelPipe for each pixel with coverage greater
*			than zero.
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/
void Rasterizer::fill()
{
    if(m_scissor && !m_scissorEdges.size())
        return;	//scissoring is on, but there are no scissor rectangles => nothing is visible

    int firstAe = 0;

    //proceed scanline by scanline
    //keep track of edges that can intersect the pixel filters of the current scanline (Active Edge Table)
    //until all pixels of the scanline have been processed
    //  for all sampling points of the current pixel
    //    determine the winding number using edge functions
    //    add filter weight to coverage
    //  divide coverage by the number of samples
    //  determine a run of pixels with constant coverage
    //  call fill callback for each pixel of the run

    const int fillRuleMask = m_fillRuleMask;

    int bbminx = (m_edgeMin.x >> X_BITS);
    int bbminy = (m_edgeMin.y >> Y_BITS);
    int bbmaxx = (m_edgeMax.x >> X_BITS)+1;
    int bbmaxy = (m_edgeMax.y >> Y_BITS)+1;
    int sx = RI_INT_MAX(m_vpx, bbminx);
    int ex = RI_INT_MIN(m_vpx+m_vpwidth, bbmaxx);
    int sy = RI_INT_MAX(m_vpy, bbminy);
    int ey = RI_INT_MIN(m_vpy+m_vpheight, bbmaxy);
    if(sx < m_covMinx) m_covMinx = sx;
    if(sy < m_covMiny) m_covMiny = sy;
    if(ex > m_covMaxx) m_covMaxx = ex;
    if(ey > m_covMaxy) m_covMaxy = ey;

#if 0
    // Dump edges:
    static bool dump = true;
    if (dump)
    {
        RI_PRINTF("lines = []\n");
        for (int ie = 0; dump && ie < m_edgePool.size(); ie++)
        {
            RI_PRINTF("lines += [[%d, %d], [%d, %d]]\n",m_edgePool[ie].v0.x, m_edgePool[ie].v0.y, m_edgePool[ie].v1.x, m_edgePool[ie].v1.y);
        }
        dump = false;
    }

#endif
    int debugMagic = 0;

    m_aet.clear();

#if defined(RI_DEBUG)
    maybeDumpEdges(m_edgePool);
#endif

    //fill the screen
    for(int j = sy; j < ey; j++)
    {
        Windings scanlineWinding;
        const int cminy = j << Y_BITS;

        if (m_scissor)
        {
            // Gather scissor edges intersecting this scanline
            // \todo Don't clear, remove unused instead!
            m_scissorAet.clear();

            for(int e = 0; e < m_scissorEdges.size(); e++)
            {
                const ScissorEdge& se = m_scissorEdges[e];

                if(j >= se.miny && j < se.maxy)
                    m_scissorAet.push_back(m_scissorEdges[e]);	//throws bad_alloc
            }

            //sort scissor AET by edge x
            if (m_scissor)
                m_scissorAet.sort();
        }

        // Drop unused edges, update remaining.
        // \todo Combine with full sweep. Use a sort-friendly edge-discard.
        for (int iae = firstAe; iae < m_aet.size(); iae++)
        {
            ActiveEdge& ae = m_aet[iae];

            if (cminy >= ae.yEnd)
            {
                m_aet[iae] = m_aet[firstAe];
                firstAe++;
                continue;
            }

            /* Update existing coordinates */
            // \todo AND instead of shift. See other places also.
            const int y0 = (ae.yStart & ~Y_MASK);
            const int x = ae.xRef + ((j << Y_BITS) - y0) * ae.slope;
            ae.minx = x >> RASTERIZER_BITS;
            ae.maxx = (x + ae.slope * (1<<Y_BITS)) >> RASTERIZER_BITS;

            if (ae.minx > ae.maxx)
                RI_ANY_SWAP(ActiveEdge::XCoord, ae.minx, ae.maxx);

            // If the edge is not visible, "mark" it as immediately applicable
            // \todo Verify that this is the correct procedure.

           if (ae.maxx < 0)
               ae.minx = ae.maxx = LEFT_DISCARD_SHORT;
       }

        /* Add new edges */

        RIuint32 aeIndex = m_edges[j];
        while (aeIndex != EDGE_TERMINATOR)
        {
            const ActiveEdge& ae = m_edgePool[aeIndex];
            m_aet.push_back(ae); // \todo Just copy pointers?
            aeIndex = ae.next;
        }

        if (firstAe >= m_aet.size())
        {
            RI_ASSERT(firstAe == m_aet.size());
            continue;	//no edges on the whole scanline, skip it
        }

        //sort AET by edge minx
        m_aet.sort(firstAe, m_aet.size() - 1);

        // \todo Optimize adding and updating the edges?
        if (m_scissor && !m_scissorAet.size())
            continue;	// Scissoring is on, but there are no scissor rectangles on this scanline.

        //fill the scanline
        int scissorWinding = m_scissor ? 0 : 1;	//if scissoring is off, winding is always 1
        int scissorIndex = 0;
        int aes = firstAe;
        int aen = firstAe;

        RI_ASSERT(sx >= 0);

#if 1
        if (m_aa)
        {
            while ((aen < m_aet.size()) && (m_aet[aen].maxx < 0))
            {
                applyLeftEdge(m_aet[aen], scanlineWinding, j);
                aen++;
            }
        }
        else
        {
            while ((aen < m_aet.size()) && (m_aet[aen].maxx < 0))
            {
                applyLeftEdgeNoAA(m_aet[aen], scanlineWinding, j);
                aen++;
            }
        }

#if defined(RI_DEBUG)
        for (int a = aen; a < m_aet.size(); a++)
        {
            RI_ASSERT(m_aet[a].maxx >= 0);
        }
#endif
#endif

        // \todo Combine this with the first check or reorganize the "clipping".
        if (aen >= m_aet.size())
            continue; // No edges within viewport. Can happen atm. when all edges are "left".

        for(int i = sx; i < ex;)
        {
            //find edges that intersect or are to the left of the pixel antialiasing filter
            while(aes < m_aet.size() && (i + 1) >= m_aet[aes].minx)
                aes++;
            //edges [0,aes[ may have an effect on winding, and need to be evaluated while sampling

            // RIint8 winding[SF_SAMPLES];
            Windings pixelWinding;

            pixelWinding = scanlineWinding;

            if (m_aa)
            {
                for(int e = aen; e < aes; e++)
                {
                    const ActiveEdge& currAe = m_aet[e];
                    calculateAEWinding(currAe, pixelWinding, scanlineWinding, j, i << RASTERIZER_BITS);
                }
            }
            else
            {
                for(int e = aen; e < aes; e++)
                {
                    const ActiveEdge& currAe = m_aet[e];
                    calculateAEWindingNoAA(currAe, pixelWinding, scanlineWinding, j, i << RASTERIZER_BITS);
                }
            }

            //compute coverage
            int coverageSamples = 0;
#if !defined(USE_SSE2)

            for (int s = 0; s < SF_SAMPLES; s++)
            {
                if(pixelWinding.winding[s])
                {
                    coverageSamples++;
                }
            }
#else
           coverageSamples = mm_winding_to_coverage(pixelWinding, fillRuleMask);
            _mm_empty();
#endif

            //constant coverage optimization:
            //scan AET from left to right and skip all the edges that are completely to the left of the pixel filter.
            //since AET is sorted by minx, the edge we stop at is the leftmost of the edges we haven't passed yet.
            //if that edge is to the right of this pixel, coverage is constant between this pixel and the start of the edge.
            while(aen < m_aet.size() && m_aet[aen].maxx < i)
                aen++;

            int endSpan = m_vpx + m_vpwidth;	// endSpan is the first pixel NOT part of the span

            if(aen < m_aet.size())
            {
                endSpan = RI_INT_MAX(i+1, RI_INT_MIN(endSpan, m_aet[aen].minx));
            }

            //fill a run of pixels with constant coverage
            if(coverageSamples)
            {

                if (!m_scissor)
                {
                    int fillStartX = i;	/* Inclusive */
                    pushSpan(fillStartX, j, (endSpan - fillStartX), coverageSamples);
                }
                else // (scissor)
                {
                    int fillStartX = i;
                    //update scissor winding number

                    /* \todo Sort the scissor edges and skip unnecessary checks when scissors are used */
                    while (scissorIndex < m_scissorAet.size() && m_scissorAet[scissorIndex].x <= fillStartX)
                    {
                        scissorWinding += m_scissorAet[scissorIndex++].direction;
                    }

                    while (!scissorWinding && scissorIndex < m_scissorAet.size() && m_scissorAet[scissorIndex].x < endSpan)
                    {
                        fillStartX = m_scissorAet[scissorIndex].x;
                        scissorWinding += m_scissorAet[scissorIndex++].direction;
                        RI_ASSERT(fillStartX >= i);
                    }

                    RI_ASSERT(scissorWinding >= 0);

                    int endScissorSpan = endSpan;

                    while (scissorWinding && fillStartX < endSpan && (scissorIndex < m_scissorAet.size()))
                    {

                        // Determine the end of renderable area:
                        while (scissorWinding && scissorIndex < m_scissorAet.size() && m_scissorAet[scissorIndex].x <= endSpan)
                        {
                            endScissorSpan = m_scissorAet[scissorIndex].x;
                            scissorWinding += m_scissorAet[scissorIndex++].direction;
                        }

                        RI_ASSERT(fillStartX >= i);
                        RI_ASSERT(endScissorSpan <= endSpan);

                        pushSpan(fillStartX, j, (endScissorSpan - fillStartX), coverageSamples);
                        fillStartX = endScissorSpan;
                        endScissorSpan = endSpan;

                        // Skip until within drawable area
                        while (!scissorWinding && scissorIndex < m_scissorAet.size() && m_scissorAet[scissorIndex].x < endSpan)
                        {
                            fillStartX = m_scissorAet[scissorIndex].x;
                            scissorWinding += m_scissorAet[scissorIndex++].direction;
                        }

                    }
                }
            }
            i = endSpan;
        }
    }
    commitSpans();
#if defined(USE_SSE2)
    _mm_empty();
#endif
    clear();
}

RI_INLINE void Rasterizer::commitSpans()
{
    if (!m_nSpans)
        return;

    m_pixelPipe->fillSpans(m_ppVariants, m_spanCache, m_nSpans);
    m_nSpans = 0;

}

RI_INLINE void Rasterizer::pushSpan(int x, int y, int len, int coverage)
{
    //printf("x: %d, y: %d, len: %d, coverage: %d\n", x, y, len, coverage);
    // \todo Check what causes this with scissors
    if (len <= 0) return;
    //RI_ASSERT(len > 0);

    Span& span = m_spanCache[m_nSpans];

    span.x0 = x;
    span.y = y;
    span.len = (RIuint16)len;
    span.coverage = coverage;

    m_nSpans++;

    if (m_nSpans == N_CACHED_SPANS)
    {
        commitSpans();
    }
}

//=======================================================================

}	//namespace OpenVGRI
