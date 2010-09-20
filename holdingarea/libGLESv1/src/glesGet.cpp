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
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *
 */

#include "glesGet.h"
#include <GLES/glext.h>
#include "GLESContext.h"
#include "GLESDesktopGL.h"

// UINT_MAX
#include <limits.h>

bool glesGetParameter(const GLESContext& context, const GLESDesktopGL &dgl, GLenum pname, GLESGetType dstType, void* params)
{
    GLESGetType srcType;
    GLboolean b[4];
    GLint i[16];
    GLdouble d[16];
    int n = 1;

    switch(pname)
    {
    case GL_MAX_TEXTURE_UNITS:
        i[0] = context.MaxTextureUnits();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_MAX_CLIP_PLANES:
        i[0] = context.MaxClipPlanes();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_MAX_LIGHTS:
        i[0] = context.MaxLights();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_CLIENT_ACTIVE_TEXTURE:
        i[0] = GL_TEXTURE0 + context.ClientActiveTexture();
        srcType = GLES_TYPE_ENUM;
        break;

    case GL_VERTEX_ARRAY:
        i[0] = context.IsArrayEnabled(GLES_VERTEX_ARRAY);
        srcType = GLES_TYPE_BOOLEAN;
        break;

    case GL_VERTEX_ARRAY_SIZE:
        i[0] = context.VertexArray().Size();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_VERTEX_ARRAY_TYPE:
        i[0] = context.VertexArray().Type();
        srcType = GLES_TYPE_ENUM;
        break;

    case GL_VERTEX_ARRAY_STRIDE:
        i[0] = context.VertexArray().Stride();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_NORMAL_ARRAY:
        i[0] = context.IsArrayEnabled(GLES_NORMAL_ARRAY);
        srcType = GLES_TYPE_BOOLEAN;
        break;

    case GL_NORMAL_ARRAY_TYPE:
        i[0] = context.NormalArray().Type();
        srcType = GLES_TYPE_ENUM;
        break;

    case GL_NORMAL_ARRAY_STRIDE:
        i[0] = context.NormalArray().Stride();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_COLOR_ARRAY:
        i[0] = context.IsArrayEnabled(GLES_COLOR_ARRAY);
        srcType = GLES_TYPE_BOOLEAN;
        break;

    case GL_COLOR_ARRAY_SIZE:
        i[0] = context.ColorArray().Size();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_COLOR_ARRAY_TYPE:
        i[0] = context.ColorArray().Type();
        srcType = GLES_TYPE_ENUM;
        break;

    case GL_COLOR_ARRAY_STRIDE:
        i[0] = context.ColorArray().Stride();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_POINT_SIZE_ARRAY_OES:
        i[0] = context.IsArrayEnabled(GLES_POINT_SIZE_ARRAY);
        srcType = GLES_TYPE_BOOLEAN;
        break;

    case GL_POINT_SIZE_ARRAY_TYPE_OES:
        i[0] = context.PointSizeArray().Type();
        srcType = GLES_TYPE_ENUM;
        break;

    case GL_POINT_SIZE_ARRAY_STRIDE_OES:
        i[0] = context.PointSizeArray().Stride();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES:
        i[0] = context.PointSizeArray().BufferName();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_TEXTURE_COORD_ARRAY:
        i[0] = context.IsArrayEnabled(static_cast<GLESArrayFlag>(GLES_TEXTURE_COORD_ARRAY << context.ClientActiveTexture()));
        srcType = GLES_TYPE_BOOLEAN;
        break;

    case GL_TEXTURE_COORD_ARRAY_SIZE:
        i[0] = context.TexCoordArray().Size();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_TEXTURE_COORD_ARRAY_TYPE:
        i[0] = context.TexCoordArray().Type();
        srcType = GLES_TYPE_ENUM;
        break;

    case GL_TEXTURE_COORD_ARRAY_STRIDE:
        i[0] = context.TexCoordArray().Stride();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_VERTEX_ARRAY_BUFFER_BINDING:
        i[0] = context.VertexArray().BufferName();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_NORMAL_ARRAY_BUFFER_BINDING:
        i[0] = context.NormalArray().BufferName();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_COLOR_ARRAY_BUFFER_BINDING:
        i[0] = context.ColorArray().BufferName();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING:
        i[0] = context.TexCoordArray().BufferName();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_ARRAY_BUFFER_BINDING:
        i[0] = context.ArrayBufferBinding();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_ELEMENT_ARRAY_BUFFER_BINDING:
        i[0] = context.ElementArrayBufferBinding();
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES:
        if(dstType != GLES_TYPE_INTEGER)
        {
            return false;
        }
        dgl.glGetDoublev(GL_MODELVIEW_MATRIX, d);
        n = 16;
        for(int j = 0; j < n; j++)
        {
            static_cast<GLint*>(params)[j] = *reinterpret_cast<GLint*>(&d[j]);
        }
        break;

    case GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS_OES:
        if(dstType != GLES_TYPE_INTEGER)
        {
            return false;
        }
        dgl.glGetDoublev(GL_PROJECTION_MATRIX, d);
        n = 16;
        for(int j = 0; j < n; j++)
        {
            static_cast<GLint*>(params)[j] = *reinterpret_cast<GLint*>(&d[j]);
        }
        break;

    case GL_TEXTURE_MATRIX_FLOAT_AS_INT_BITS_OES:
        if(dstType != GLES_TYPE_INTEGER)
        {
            return false;
        }
        dgl.glGetDoublev(GL_TEXTURE_MATRIX, d);
        n = 16;
        for(int j = 0; j < n; j++)
        {
            static_cast<GLint*>(params)[j] = *reinterpret_cast<GLint*>(&d[j]);
        }
        break;

    case GL_POINT_SPRITE_OES:
        b[0] = dgl.glIsEnabled(GL_POINT_SPRITE);
        srcType = GLES_TYPE_BOOLEAN;
        break;

    case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
        i[0] = 10;
        srcType = GLES_TYPE_INTEGER;
        break;

    case GL_COMPRESSED_TEXTURE_FORMATS:
        i[0] = GL_PALETTE4_RGB8_OES;
        i[1] = GL_PALETTE4_RGBA8_OES;
        i[2] = GL_PALETTE4_R5_G6_B5_OES;
        i[3] = GL_PALETTE4_RGBA4_OES;
        i[4] = GL_PALETTE4_RGB5_A1_OES;
        i[5] = GL_PALETTE8_RGB8_OES;
        i[6] = GL_PALETTE8_RGBA8_OES;
        i[7] = GL_PALETTE8_R5_G6_B5_OES;
        i[8] = GL_PALETTE8_RGBA4_OES;
        i[9] = GL_PALETTE8_RGB5_A1_OES;
        srcType = GLES_TYPE_ENUM;
        n = 10;
        break;

    case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES:
        i[0] = GL_RGBA;
        srcType = GLES_TYPE_ENUM;
        break;

    case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES:
        i[0] = GL_UNSIGNED_BYTE;
        srcType = GLES_TYPE_ENUM;
        break;

    case GL_ALIASED_POINT_SIZE_RANGE:
    case GL_ALIASED_LINE_WIDTH_RANGE:
    case GL_DEPTH_RANGE:
    case GL_SMOOTH_LINE_WIDTH_RANGE:
    case GL_SMOOTH_POINT_SIZE_RANGE:
        dgl.glGetDoublev(pname, d);
        srcType = GLES_TYPE_DOUBLE;
        n = 2;
        break;
    case GL_ALPHA_TEST_FUNC:
    case GL_STENCIL_FAIL:
    case GL_STENCIL_FUNC:
    case GL_STENCIL_PASS_DEPTH_FAIL:
    case GL_STENCIL_PASS_DEPTH_PASS:
    case GL_BLEND_DST:
    case GL_BLEND_SRC:
    case GL_LOGIC_OP_MODE:
    case GL_CULL_FACE:
    case GL_DEPTH_FUNC:
    case GL_FOG_HINT:
    case GL_FOG_MODE:
    case GL_FRONT_FACE:
    case GL_LINE_SMOOTH_HINT:
    case GL_MATRIX_MODE:
    case GL_PERSPECTIVE_CORRECTION_HINT:
    case GL_POINT_SMOOTH_HINT:
    case GL_SHADE_MODEL:
    case GL_GENERATE_MIPMAP_HINT:
    case GL_CULL_FACE_MODE:
    case GL_ACTIVE_TEXTURE:
        dgl.glGetIntegerv(pname, i);
        srcType = GLES_TYPE_ENUM;
        break;
    case GL_ALPHA_BITS:
    case GL_RED_BITS:
    case GL_GREEN_BITS:
    case GL_BLUE_BITS:
    case GL_DEPTH_BITS:
    case GL_STENCIL_BITS:
    case GL_STENCIL_VALUE_MASK:
    case GL_STENCIL_WRITEMASK:
    case GL_MODELVIEW_STACK_DEPTH:
    case GL_PROJECTION_STACK_DEPTH:
    case GL_TEXTURE_STACK_DEPTH:
    case GL_PACK_ALIGNMENT:
    case GL_UNPACK_ALIGNMENT:
    case GL_TEXTURE_BINDING_2D:
    case GL_SUBPIXEL_BITS:
    case GL_SAMPLE_BUFFERS:
    case GL_SAMPLES:
    case GL_BUFFER_SIZE:
    case GL_STENCIL_CLEAR_VALUE:
    case GL_MAX_MODELVIEW_STACK_DEPTH:
    case GL_MAX_PROJECTION_STACK_DEPTH:
    case GL_MAX_TEXTURE_STACK_DEPTH:
    case GL_MAX_TEXTURE_SIZE:
        dgl.glGetIntegerv(pname, i);
        srcType = GLES_TYPE_INTEGER;
        break;
    case GL_ALPHA_TEST_REF:
    case GL_STENCIL_REF:
    case GL_DEPTH_CLEAR_VALUE:
    case GL_FOG_DENSITY:
    case GL_FOG_START:
    case GL_FOG_END:
    case GL_LINE_WIDTH:
    case GL_POINT_SIZE:
    case GL_POINT_SIZE_MIN:
    case GL_POINT_SIZE_MAX:
    case GL_POLYGON_OFFSET_FACTOR:
    case GL_POLYGON_OFFSET_UNITS:
    case GL_POINT_FADE_THRESHOLD_SIZE:
        dgl.glGetDoublev(pname, d);
        srcType = GLES_TYPE_DOUBLE;
        break;
    case GL_CURRENT_NORMAL:
    case GL_POINT_DISTANCE_ATTENUATION:
        dgl.glGetDoublev(pname, d);
        srcType = GLES_TYPE_DOUBLE;
        n = 3;
        break;
    case GL_COLOR_CLEAR_VALUE:
    case GL_CURRENT_TEXTURE_COORDS:
    case GL_CURRENT_COLOR:
    case GL_FOG_COLOR:
    case GL_LIGHT_MODEL_AMBIENT:
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_SPECULAR:
    case GL_EMISSION:
        dgl.glGetDoublev(pname, d);
        srcType = GLES_TYPE_DOUBLE;
        n = 4;
        break;
    case GL_COLOR_WRITEMASK:
        dgl.glGetBooleanv(pname, b);
        srcType = GLES_TYPE_BOOLEAN;
        n = 4;
        break;
    case GL_DEPTH_WRITEMASK:
    case GL_LIGHT_MODEL_TWO_SIDE:
        dgl.glGetBooleanv(pname, b);
        srcType = GLES_TYPE_BOOLEAN;
        break;
    case GL_PROJECTION_MATRIX:
    case GL_MODELVIEW_MATRIX:
    case GL_TEXTURE_MATRIX:
        dgl.glGetDoublev(pname, d);
        srcType = GLES_TYPE_DOUBLE;
        n = 16;
        break;
    case GL_VIEWPORT:
    case GL_SCISSOR_BOX:
        dgl.glGetIntegerv(pname, i);
        srcType = GLES_TYPE_INTEGER;
        n = 4;
        break;
    case GL_MAX_VIEWPORT_DIMS:
        dgl.glGetIntegerv(pname, i);
        srcType = GLES_TYPE_INTEGER;
        n = 2;
        break;

    default:
        return false;
    }

    // Type conversions

    // Special cases
    if(dstType == GLES_TYPE_INTEGER &&
       (pname == GL_CURRENT_COLOR || pname == GL_ALPHA_TEST_REF || pname == GL_DEPTH_CLEAR_VALUE))
    {
        for(int j = 0; j < n; j++)
        {
            static_cast<GLint*>(params)[j] = static_cast<GLint>((UINT_MAX * d[j] - 1) / 2);
        }
        return true;
    }

    for(int j = 0; j < n; j++)
    {
        switch(dstType)
        {
        case GLES_TYPE_BOOLEAN:
            switch(srcType)
            {
            case GLES_TYPE_BOOLEAN:
                static_cast<GLboolean*>(params)[j] = b[j];
                break;
            case GLES_TYPE_INTEGER:
            case GLES_TYPE_ENUM:
                static_cast<GLboolean*>(params)[j] = i[j] ? GL_TRUE : GL_FALSE;
                break;
            case GLES_TYPE_DOUBLE:
                static_cast<GLboolean*>(params)[j] = d[j] ? GL_TRUE : GL_FALSE;
                break;
            default:
                GLES_ASSERT(false);
            }
            break;

        case GLES_TYPE_INTEGER:
            switch(srcType)
            {
            case GLES_TYPE_BOOLEAN:
                static_cast<GLint*>(params)[j] = b[j];
                break;
            case GLES_TYPE_INTEGER:
            case GLES_TYPE_ENUM:
                static_cast<GLint*>(params)[j] = i[j];
                break;
            case GLES_TYPE_DOUBLE:
                static_cast<GLint*>(params)[j] = static_cast<GLint>(d[j]);
                break;
            default:
                GLES_ASSERT(false);
            }
            break;

        case GLES_TYPE_FLOAT:
            switch(srcType)
            {
            case GLES_TYPE_BOOLEAN:
                static_cast<GLfloat*>(params)[j] = b[j];
                break;
            case GLES_TYPE_INTEGER:
            case GLES_TYPE_ENUM:
                static_cast<GLfloat*>(params)[j] = static_cast<GLfloat>(i[j]);
                break;
            case GLES_TYPE_DOUBLE:
                static_cast<GLfloat*>(params)[j] = static_cast<GLfloat>(d[j]);
                break;
            default:
                GLES_ASSERT(false);
            }
            break;

        case GLES_TYPE_FIXED:
            switch(srcType)
            {
            case GLES_TYPE_BOOLEAN:
                static_cast<GLfixed*>(params)[j] = F_TO_X(b[j] ? 1.f : 0.f);
                break;
            case GLES_TYPE_INTEGER:
                static_cast<GLfixed*>(params)[j] = I_TO_X(i[j]);
                break;
            case GLES_TYPE_ENUM:
                static_cast<GLfixed*>(params)[j] = i[j];
                break;
            case GLES_TYPE_DOUBLE:
                static_cast<GLfixed*>(params)[j] = D_TO_X(d[j]);
                break;
            default:
                GLES_ASSERT(false);
            }
            break;

        default:
            GLES_ASSERT(false);
        }
    }

    return true;
}
