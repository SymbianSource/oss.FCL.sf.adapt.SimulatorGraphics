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

#ifndef __SFDYNAMICPIXELPIPE_H
#define __SFDYNAMICPIXELPIPE_H

#ifndef __RIPIXELPIPE_H
#   include "riPixelPipe.h"
#endif

namespace OpenVGRI {


struct PixelPipeHash 
{
    PixelPipeHash() {value[0] = 0; value[1] = 0;}
    bool operator==(const PixelPipeHash& rhs) const { return value[0] == rhs.value[0] && value[1] == rhs.value[1]; }

    RIuint32 value[2];
}; 
// Interface to compiler?
void pixelPipelineCall(const PixelPipe::SignatureState& state, const PixelPipe::PPUniforms& uniforms, PixelPipe::PPVariants& variants, const Span* spans, int nSpans);
// Interface to rasterizer
void executePixelPipeline(const PixelPipe::SignatureState& state, const PixelPipe::PPUniforms& uniforms, PixelPipe::PPVariants& variants, const Span* spans, int nSpans);

void calculatePPHash(PixelPipeHash& hash, const PixelPipe::SignatureState& state);

}

#endif

