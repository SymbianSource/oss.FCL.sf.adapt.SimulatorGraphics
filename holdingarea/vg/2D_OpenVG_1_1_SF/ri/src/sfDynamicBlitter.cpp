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

#include "sfDynamicBlitter.h"

#ifndef __SFBLITTER_H
#   include "sfBlitter.h"
#endif

#ifndef __SFMASK_H
#   include "sfMask.h"
#endif

namespace OpenVGRI {

RI_INLINE static bool maskOperationRequiresLoad(VGMaskOperation op)
{
    switch(op)
    {
    case VG_CLEAR_MASK:
    case VG_FILL_MASK:
    case VG_SET_MASK:
        return false;
    default:
        RI_ASSERT(op == VG_UNION_MASK || op == VG_INTERSECT_MASK || op == VG_SUBTRACT_MASK);
        return true;
    }
}

/**
 * \brief   Blit rectangular areas between similar or different color format buffers.
 * \note    Implementation is structured so that the per-pixel branches are minimized
 *          even if dynamic compilation is not in use.
 * \todo    For some reason the completely generic integer conversion does not optimize
 *          fully run-time. Check what causes this!
 */
void executeBlitter(const DynamicBlitter::BlitSignatureState& state, const DynamicBlitter::BlitUniforms& uniforms)
{
    const void* srcPtr = Image::calculateAddress(
        uniforms.src, state.srcDesc.bitsPerPixel, uniforms.srcX, uniforms.srcY, uniforms.srcStride); 
    void* dstPtr = Image::calculateAddress(
        uniforms.dst, state.dstDesc.bitsPerPixel, uniforms.dstX, uniforms.dstY, uniforms.dstStride);

    // The following flag is useful for debugging. Enabling it will hurt performance.
//#define ALWAYS_FORCE_FULL_CONVERSION

    // Currently the data must be byte-aligned for memcpy optimizations:
    const int minBpp = RI_INT_MIN(state.srcDesc.bitsPerPixel, state.dstDesc.bitsPerPixel);
    const bool byteCopy = minBpp >= 8 ? true : false;
    const bool forceFullConversion = state.isMaskOperation || state.unsafeInput || !byteCopy;
    
#if !defined(ALWAYS_FORCE_FULL_CONVERSION)
    if (state.srcDesc.isZeroConversion(state.dstDesc) && !forceFullConversion)
    {
        const int fullCopyStride = Image::descriptorToStride(state.srcDesc, uniforms.width);

        if ((uniforms.dstStride != uniforms.srcStride) || (fullCopyStride != uniforms.srcStride)) 
        {
            // memcpy individual scanlines.
            const size_t scanLength = (size_t)RI_INT_ABS(Image::descriptorToStride(state.srcDesc, uniforms.width));
            for (RIuint32 i = 0; i < uniforms.height; i++)
            {
                memcpy(dstPtr, srcPtr, scanLength);
                dstPtr = (void*)((RIuint8*)dstPtr + uniforms.dstStride);
                srcPtr = (void*)((RIuint8*)srcPtr + uniforms.srcStride);
            }
        } else
        {
            // memcpy the whole area
            memcpy(dstPtr, srcPtr, uniforms.srcStride * uniforms.height);
        }
    } else if (state.srcDesc.isShiftConversion(state.dstDesc) && !forceFullConversion)
    {
        // \todo Separate to function? Replace with pointer read/write & advance.
        if (state.srcDesc.isShiftConversionToLower(state.dstDesc))
        {
            // Components can be immediately shifted down to lower bit-depth.
            for (RIuint32 j = 0; j < uniforms.height; j++)
            {
                RIuint8* srcStart = (RIuint8*)srcPtr;
                RIuint8* dstStart = (RIuint8*)dstPtr;
                RIuint32 srcX = uniforms.srcX;
                RIuint32 dstX = uniforms.dstX;
                for (RIuint32 i = 0; i < uniforms.width; i++)
                {
                    RIuint32 c = Image::readPackedPixelFromAddress(srcPtr, state.srcDesc.bitsPerPixel, srcX); // \todo srcX!
                    RIuint32 dc = Color::Descriptor::crossConvertToLower(c, state.srcDesc, state.dstDesc);
                    Image::writePackedPixelToAddress(dstPtr, state.dstDesc.bitsPerPixel, dstX, dc);

                    srcPtr = Image::incrementPointer(srcPtr, state.srcDesc.bitsPerPixel, srcX);
                    dstPtr = (void*)Image::incrementPointer(dstPtr, state.dstDesc.bitsPerPixel, dstX); 

                    srcX++;
                    dstX++;
                }
                srcPtr = (void*)(srcStart + uniforms.srcStride);
                dstPtr = (void*)(dstStart + uniforms.dstStride);
            }
        }
        else
        {
            // Color components require expansion before shifting to destination color-format */
            for (RIuint32 j = 0; j < uniforms.height; j++)
            {
                RIuint8* srcStart = (RIuint8*)srcPtr;
                RIuint8* dstStart = (RIuint8*)dstPtr;
                RIuint32 srcX = uniforms.srcX;
                RIuint32 dstX = uniforms.dstX;
                for (RIuint32 i = 0; i < uniforms.width; i++)
                {
                    RIuint32 c = Image::readPackedPixelFromAddress(srcPtr, state.srcDesc.bitsPerPixel, srcX);
                    IntegerColor ic = IntegerColor(c, state.srcDesc);
                    ic.expandColor(state.srcDesc);
                    ic.truncateColor(state.dstDesc);
                    RIuint32 dc = ic.getPackedColor(state.dstDesc);
                    Image::writePackedPixelToAddress(dstPtr, state.dstDesc.bitsPerPixel, dstX, dc);

                    srcPtr = Image::incrementPointer(srcPtr, state.srcDesc.bitsPerPixel, srcX);
                    dstPtr = (void*)Image::incrementPointer(dstPtr, state.dstDesc.bitsPerPixel, dstX); 

                    srcX++;
                    dstX++;
                }
                srcPtr = (void*)(srcStart + uniforms.srcStride);
                dstPtr = (void*)(dstStart + uniforms.dstStride);
            }
        }
    } else
#endif
    {
        /* This block should handle the rest (and all) of the color-conversion cases. */
        for (RIuint32 j = 0; j < uniforms.height; j++)
        {
            RIuint8* srcStart = (RIuint8*)srcPtr;
            RIuint8* dstStart = (RIuint8*)dstPtr;
            RIuint32 srcX = uniforms.srcX;
            RIuint32 dstX = uniforms.dstX;
            for (RIuint32 i = 0; i < uniforms.width; i++)
            {
                RIuint32 c = Image::readPackedPixelFromAddress(srcPtr, state.srcDesc.bitsPerPixel, srcX);
                IntegerColor ic;
                RIuint32 dc;
                
                if (!state.isMaskOperation)
                {
                    ic = IntegerColor(c, state.srcDesc);
                    if (state.unsafeInput)
                        ic.clampToAlpha();

                    ic.convertToFrom(state.dstDesc, state.srcDesc, false);
                    dc = ic.getPackedColor(state.dstDesc);
                }
                else
                {
                    // Apply the given mask operation between two surfaces.
                    IntegerColor id;

                    if (maskOperationRequiresLoad(state.maskOperation))
                    {
                        ic.fromPackedMask(c, state.srcDesc);
                        ic.expandMask(state.srcDesc);

                        IntegerColor id;

                        RIuint32 d = Image::readPackedPixelFromAddress(dstPtr, state.dstDesc.bitsPerPixel, dstX);
                        id.fromPackedMask(d, state.dstDesc);
                        id.expandColor(state.dstDesc);

                        RIuint32 coverage = ic.a + (ic.a >> 7);
                        ic = intMaskOperation(coverage, id, state.maskOperation);
                    }
                    else
                    {
                        // Other ops handled with memset, not blitter
                        RI_ASSERT(state.maskOperation == VG_SET_MASK); 
                        ic.fromPackedMask(c, state.srcDesc);
                        //ic.expandMask(state.srcDesc);
                        ic.convertToFrom(state.dstDesc, state.srcDesc, true);
                    }
                    dc = ic.getPackedMaskColor(state.dstDesc);
                }

                Image::writePackedPixelToAddress(dstPtr, state.dstDesc.bitsPerPixel, dstX, dc);

                srcPtr = Image::incrementPointer(srcPtr, state.srcDesc.bitsPerPixel, srcX);
                dstPtr = (void*)Image::incrementPointer(dstPtr, state.dstDesc.bitsPerPixel, dstX); 

                srcX++;
                dstX++;
            }
            srcPtr = (void*)(srcStart + uniforms.srcStride);
            dstPtr = (void*)(dstStart + uniforms.dstStride);
        }
    }
}

}

