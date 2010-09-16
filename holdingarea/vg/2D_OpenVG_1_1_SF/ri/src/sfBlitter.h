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

#ifndef __SFBLITTER_H
#define __SFBLITTER_H

#ifndef __RIDEFS_H
#   include "riDefs.h"
#endif

#ifndef __RIIMAGE_H
#   include "riImage.h"
#endif

#include "VG/openvg.h"

namespace OpenVGRI {

class PPCompiler;

struct BlitterHash {
    BlitterHash() {value[0] = 0; }
    bool operator==(const BlitterHash& rhs) const { return value[0] == rhs.value[0]; }
    RIuint32 value[1];
}; 

// \todo Rename to just "Blitter" and move out of the same compilation unit.
class DynamicBlitter {
public:
    struct BlitSignatureState {
        VGMaskOperation maskOperation;

        bool incompatibleStrides;
        bool isMaskOperation;
        bool unsafeInput;
        
        // Derived:
        Color::Descriptor srcDesc;
        Color::Descriptor dstDesc;
    };

    struct BlitUniforms {
        void*       dst;
        const void* src;
        RIuint32    srcX;
        RIuint32    srcY;
        RIuint32    dstX;
        RIuint32    dstY;
        RIuint32    width;
        RIuint32    height;
        RIint32     srcStride;
        RIint32     dstStride;
    };

public:
    DynamicBlitter();
    ~DynamicBlitter();

    static void calculateHash(BlitterHash& hash, const BlitSignatureState& state);

    void setMaskOperation(VGMaskOperation maskOperation)    { m_maskOperation = maskOperation; }
    void enableMaskOperation(bool isMaskOperation)          { m_isMaskOperation = isMaskOperation; }
    void setFillColor(const Color& fillColor)               { m_fillColor = fillColor; }
    void enableFill(bool isFill)                            { m_isFill = isFill; }

    void prepareBlit(Image* dst, const Image* src, int sx, int sy, int dx, int dy, int w, int h);
    void blit();

    const BlitSignatureState&   getSignatureState() { return m_signatureState; }
    const BlitUniforms&         getUniforms() { return m_uniforms; }

private:

    VGMaskOperation     m_maskOperation;
    bool                m_isMaskOperation;
    Color               m_fillColor;
    bool                m_isFill;
    
    BlitSignatureState  m_signatureState;
    BlitUniforms        m_uniforms;
};
}

#endif

