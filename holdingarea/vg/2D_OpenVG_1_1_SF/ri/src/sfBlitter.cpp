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

#include "sfBlitter.h"

#include "sfCompiler.h"

namespace OpenVGRI {

DynamicBlitter::DynamicBlitter() :
    m_maskOperation(VG_SET_MASK),
    m_isMaskOperation(false),
    m_fillColor(),
    m_isFill(false),

    m_signatureState(),
    m_uniforms()
{
}

DynamicBlitter::~DynamicBlitter()
{
}

/*static*/ void DynamicBlitter::calculateHash(BlitterHash& hash, const BlitSignatureState& state)
{
    const RIuint32 descBits = 10;
    const RIuint32 maskOperationBits = 3;
    const RIuint32 boolBits = 1;

    RIuint32 srcFormat = (RIuint32)(state.srcDesc.toIndex());
    RIuint32 dstFormat = (RIuint32)(state.dstDesc.toIndex());
    RIuint32 maskOperation = ((RIuint32)(state.maskOperation - VG_CLEAR_MASK));
    RIuint32 incompatibleStride = ((RIuint32)state.incompatibleStrides);
    RIuint32 isMaskOperation = ((RIuint32)state.isMaskOperation);
    RIuint32 unsafeInput = (RIuint32)state.unsafeInput;
    
    int b = 0;

    b = riInsertBits32(hash.value, sizeof(hash.value), srcFormat, descBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), dstFormat, descBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), maskOperation, maskOperationBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), incompatibleStride, boolBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), isMaskOperation, boolBits, b);
    b = riInsertBits32(hash.value, sizeof(hash.value), unsafeInput, boolBits, b);
}

/**
 * \brief   Blit a region. The input coordinates and dimensions must be validated outside
 *          the blitter currently.
 * \note    The user must also apply the storage offset to the image(s).
 */
void DynamicBlitter::prepareBlit(Image* dst, const Image* src, int sx, int sy, int dx, int dy, int w, int h)
{
    //const Image *srcImage = src->getImage();
    //Image* dstImage = dst->m_image;
    
    // \todo Move these to derivation of the state?
    m_signatureState.srcDesc = src->getDescriptor();
    m_signatureState.dstDesc = dst->getDescriptor();
    m_signatureState.isMaskOperation = m_isMaskOperation;
    m_signatureState.maskOperation = m_isMaskOperation ? m_maskOperation : VG_CLEAR_MASK;
    m_signatureState.incompatibleStrides = false;
    m_signatureState.unsafeInput = false;

    m_uniforms.src = src->getData();
    m_uniforms.dst = dst->getData();
    m_uniforms.srcX = sx;
    m_uniforms.srcY = sy;
    m_uniforms.dstX = dx;
    m_uniforms.dstY = dy;
    m_uniforms.width = w;
    m_uniforms.height = h;
    m_uniforms.srcStride = src->getStride();
    m_uniforms.dstStride = dst->getStride();

    if (m_signatureState.srcDesc.isZeroConversion(m_signatureState.dstDesc))
    {
        const int fullCopyStride = Image::descriptorToStride(m_signatureState.srcDesc, m_uniforms.width);

        if ((m_uniforms.dstStride != m_uniforms.srcStride) || (fullCopyStride != m_uniforms.srcStride)) 
            m_signatureState.incompatibleStrides = true;
    }

    if (src->isUnsafe())
        m_signatureState.unsafeInput = true;

}

void DynamicBlitter::blit()
{
#if 1
    bool compiledBlitter = false;
    {
        PPCompiler& compiler = PPCompiler::getCompiler();
        PPCompiler::BlitterHandle blitterHandle = compiler.compileBlitter(getSignatureState());
        if (blitterHandle)
        {
            compiledBlitter = true;
            BlitterFunction func = compiler.getBlitterPtr(blitterHandle);
            func(getUniforms());
            compiler.releaseBlitter(blitterHandle);
        }
    }

    if (!compiledBlitter)
#endif
    {
        executeBlitter(getSignatureState(), getUniforms());
    }
}

}
