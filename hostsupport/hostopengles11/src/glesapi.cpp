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

#include <GLES/gl.h>
#include <GLES/glplatform.h>
#include <iostream>
#include "GLESDesktopGL.h"
#include "glesInternal.h"
#include "EGLInterface.h"
#include "GLESContext.h"
#include "GLESArray.h"
#include "GLESTexture.h"
#include "glesGet.h"

// exit()
#include <stdlib.h>
// memcpy
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Available only in Common profile */
GL_API void GL_APIENTRY glAlphaFunc (GLenum func, GLclampf ref)
{
    GLES_ENTER();
    ctx->DGL().glAlphaFunc (func, ref);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    GLES_ENTER();
    ctx->DGL().glClearColor (red, green, blue, alpha);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glClearDepthf (GLclampf depth)
{
    GLES_ENTER();
    ctx->DGL().glClearDepth (depth);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glClipPlanef (GLenum plane, const GLfloat *equation)
{
    GLES_ENTER();
    GLES_ERROR_IF(plane < GL_CLIP_PLANE0 || plane >= GL_CLIP_PLANE0 + ctx->MaxClipPlanes(), GL_INVALID_ENUM);
    GLdouble dequation[4];
    for(int i = 0; i < 4; i++)
    {
        dequation[i] = static_cast<GLdouble>(equation[i]);
    }
    ctx->DGL().glClipPlane (plane, dequation);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    GLES_ENTER();
    ctx->DGL().glColor4f (red, green, blue, alpha);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glDepthRangef (GLclampf zNear, GLclampf zFar)
{
    GLES_ENTER();
    ctx->DGL().glDepthRange(zNear, zFar);
    GLES_LEAVE();
}
bool isValidSingleValuedFogParamEnum(GLenum pname)
{
    switch(pname)
    {
    case GL_FOG_MODE:
    case GL_FOG_DENSITY:
    case GL_FOG_START:
    case GL_FOG_END:
        return true;
    default:
        return false;
    }
}
bool isValidFogParamEnum(GLenum pname)
{
    return isValidSingleValuedFogParamEnum(pname) || pname == GL_FOG_COLOR;
}
GL_API void GL_APIENTRY glFogf (GLenum pname, GLfloat param)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidFogParamEnum(pname), GL_INVALID_ENUM);
    ctx->DGL().glFogf (pname, param);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glFogfv (GLenum pname, const GLfloat *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidFogParamEnum(pname), GL_INVALID_ENUM);
    ctx->DGL().glFogfv (pname, params);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glFrustumf (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
    GLES_ENTER();
    ctx->DGL().glFrustum (left, right, bottom, top, zNear, zFar);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glGetClipPlanef (GLenum pname, GLfloat eqn[4])
{
    GLES_ENTER();
    GLdouble deqn[4];
    ctx->DGL().glGetClipPlane (pname, deqn);
    for(int i = 0; i < 4; i++)
    {
        eqn[i] = static_cast<GLfloat>(deqn[i]);
    }
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glGetFloatv (GLenum pname, GLfloat *params)
{
    GLES_ENTER();
    if(!glesGetParameter(*ctx, ctx->DGL(), pname, GLES_TYPE_FLOAT, params))
    {
        GLES_ERROR(GL_INVALID_ENUM);
    }
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glGetLightfv (GLenum light, GLenum pname, GLfloat *params)
{
    GLES_ENTER();
    ctx->DGL().glGetLightfv (light, pname, params);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params)
{
    GLES_ENTER();
    ctx->DGL().glGetMaterialfv (face, pname, params);
    GLES_LEAVE();
}
bool isValidTexEnvTargetEnum(GLenum target)
{
    switch(target)
    {
    case GL_POINT_SPRITE_OES:
    case GL_TEXTURE_ENV:
        return true;
    default:
        return false;
    }
}
bool isValidSingleValuedTexEnvParamEnum(GLenum pname)
{
    switch(pname)
    {
    case GL_TEXTURE_ENV_MODE:
    case GL_COORD_REPLACE_OES:
    case GL_COMBINE_RGB:
    case GL_COMBINE_ALPHA:
    case GL_SRC0_RGB:
    case GL_SRC1_RGB:
    case GL_SRC2_RGB:
    case GL_SRC0_ALPHA:
    case GL_SRC1_ALPHA:
    case GL_SRC2_ALPHA:
    case GL_OPERAND0_RGB:
    case GL_OPERAND1_RGB:
    case GL_OPERAND2_RGB:
    case GL_OPERAND0_ALPHA:
    case GL_OPERAND1_ALPHA:
    case GL_OPERAND2_ALPHA:
    case GL_RGB_SCALE:
    case GL_ALPHA_SCALE:
        return true;
    default:
        return false;
    }
}
bool isValidTexEnvParamEnum(GLenum pname)
{
    return isValidSingleValuedTexEnvParamEnum(pname) || pname == GL_TEXTURE_ENV_COLOR;
}
GL_API void GL_APIENTRY glGetTexEnvfv (GLenum env, GLenum pname, GLfloat *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidTexEnvTargetEnum(env), GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidTexEnvParamEnum(pname), GL_INVALID_ENUM);
    env = env == GL_POINT_SPRITE_OES ? GL_POINT_SPRITE : env;
    pname = pname == GL_COORD_REPLACE_OES ? GL_COORD_REPLACE : pname;
    ctx->DGL().glGetTexEnvfv (env, pname, params);
    GLES_LEAVE();
}
bool isValidTexParamEnum(GLenum pname)
{
    switch(pname)
    {
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_GENERATE_MIPMAP:
        return true;
    default:
        return false;
    }
}
GL_API void GL_APIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidTexParamEnum(pname), GL_INVALID_ENUM);
    ctx->DGL().glGetTexParameterfv (target, pname, params);
    GLES_LEAVE();
}
bool isValidLightModelParamEnum(GLenum pname)
{
    switch(pname)
    {
    case GL_LIGHT_MODEL_AMBIENT:
    case GL_LIGHT_MODEL_TWO_SIDE:
        return true;
    default:
        return false;
    }
}
GL_API void GL_APIENTRY glLightModelf (GLenum pname, GLfloat param)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidLightModelParamEnum(pname), GL_INVALID_ENUM);
    ctx->DGL().glLightModelf (pname, param);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glLightModelfv (GLenum pname, const GLfloat *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidLightModelParamEnum(pname), GL_INVALID_ENUM);
    ctx->DGL().glLightModelfv (pname, params);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glLightf (GLenum light, GLenum pname, GLfloat param)
{
    GLES_ENTER();
    ctx->DGL().glLightf (light, pname, param);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glLightfv (GLenum light, GLenum pname, const GLfloat *params)
{
    GLES_ENTER();
    ctx->DGL().glLightfv (light, pname, params);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glLineWidth (GLfloat width)
{
    GLES_ENTER();
    ctx->DGL().glLineWidth (width);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glLoadMatrixf (const GLfloat *m)
{
    GLES_ENTER();
    ctx->DGL().glLoadMatrixf (m);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glMaterialf (GLenum face, GLenum pname, GLfloat param)
{
    GLES_ENTER();
    ctx->DGL().glMaterialf (face, pname, param);
    GLES_LEAVE();
}
bool isValidSingleValuedMaterialParamEnum(GLenum pname)
{
    return pname == GL_SHININESS;
}
bool isValidMaterialParamEnum(GLenum pname)
{
    if(isValidSingleValuedMaterialParamEnum(pname))
    {
        return true;
    }
    else
    {
        switch(pname)
        {
        case GL_AMBIENT:
        case GL_DIFFUSE:
        case GL_AMBIENT_AND_DIFFUSE:
        case GL_SPECULAR:
        case GL_EMISSION:
        case GL_SHININESS:
            return true;
        default:
            return false;
        }
    }
}
GL_API void GL_APIENTRY glMaterialfv (GLenum face, GLenum pname, const GLfloat *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidMaterialParamEnum(pname), GL_INVALID_ENUM);
    ctx->DGL().glMaterialfv (face, pname, params);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glMultMatrixf (const GLfloat *m)
{
    GLES_ENTER();
    ctx->DGL().glMultMatrixf(m);
    GLES_LEAVE();
}
bool isValidTextureTargetEnum(GLenum target, unsigned int maxTextureUnits)
{
    return target >= GL_TEXTURE0 && target < GL_TEXTURE0 + maxTextureUnits;
}
GL_API void GL_APIENTRY glMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidTextureTargetEnum(target, ctx->MaxTextureUnits()), GL_INVALID_ENUM);
    ctx->DGL().glMultiTexCoord4f(target, s, t, r, q);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
    GLES_ENTER();
    ctx->DGL().glNormal3f(nx, ny, nz);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glOrthof (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
    GLES_ENTER();
    ctx->DGL().glOrtho(left, right, bottom, top, zNear, zFar);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glPointParameterf (GLenum pname, GLfloat param)
{
    GLES_ENTER();
    ctx->DGL().glPointParameterf (pname, param);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glPointParameterfv (GLenum pname, const GLfloat *params)
{
    GLES_ENTER();
    ctx->DGL().glPointParameterfv (pname, params);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glPointSize (GLfloat size)
{
    GLES_ENTER();
    ctx->DGL().glPointSize (size);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glPolygonOffset (GLfloat factor, GLfloat units)
{
    GLES_ENTER();
    ctx->DGL().glPolygonOffset (factor, units);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    GLES_ENTER();
    ctx->DGL().glRotatef (angle, x, y, z);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glScalef (GLfloat x, GLfloat y, GLfloat z)
{
    GLES_ENTER();
    ctx->DGL().glScalef (x, y, z);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidTexEnvTargetEnum(target), GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidTexEnvParamEnum(pname), GL_INVALID_ENUM);
    target = target == GL_POINT_SPRITE_OES ? GL_POINT_SPRITE : target;
    pname = pname == GL_COORD_REPLACE_OES ? GL_COORD_REPLACE : pname;
    ctx->DGL().glTexEnvf (target, pname, param);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidTexEnvTargetEnum(target), GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidTexEnvParamEnum(pname), GL_INVALID_ENUM);
    target = target == GL_POINT_SPRITE_OES ? GL_POINT_SPRITE : target;
    pname = pname == GL_COORD_REPLACE_OES ? GL_COORD_REPLACE : pname;
    ctx->DGL().glTexEnvfv (target, pname, params);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
    GLES_ENTER();
    GLES_ERROR_IF(target != GL_TEXTURE_2D, GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidTexParamEnum(pname), GL_INVALID_ENUM);
    pname = pname == GL_COORD_REPLACE_OES ? GL_COORD_REPLACE : pname;
    ctx->DGL().glTexParameterf (target, pname, param);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(target != GL_TEXTURE_2D, GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidTexParamEnum(pname), GL_INVALID_ENUM);
    target = target == GL_POINT_SPRITE_OES ? GL_POINT_SPRITE : target;
    pname = pname == GL_COORD_REPLACE_OES ? GL_COORD_REPLACE : pname;
    ctx->DGL().glTexParameterfv (target, pname, params);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
    GLES_ENTER();
    ctx->DGL().glTranslatef (x, y, z);
    GLES_LEAVE();
}

/* Available in both Common and Common-Lite profiles */
GL_API void GL_APIENTRY glActiveTexture (GLenum texture)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidTextureTargetEnum(texture, ctx->MaxTextureUnits()), GL_INVALID_ENUM);
    ctx->DGL().glActiveTexture (texture);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glAlphaFuncx (GLenum func, GLclampx ref)
{
    GLES_ENTER();
    ctx->DGL().glAlphaFunc(func, X_TO_F(ref));
    GLES_LEAVE();
}
bool isValidBufferTarget(GLenum target)
{
    switch(target)
    {
    case GL_ARRAY_BUFFER:
    case GL_ELEMENT_ARRAY_BUFFER:
        return true;
    default:
        return false;
    }
}
GL_API void GL_APIENTRY glBindBuffer (GLenum target, GLuint buffer)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidBufferTarget(target), GL_INVALID_ENUM);
    switch(target)
    {
    case GL_ARRAY_BUFFER:
        if(!ctx->BindArrayBuffer(buffer))
        {
            GLES_ERROR(GL_OUT_OF_MEMORY);
        }
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        if(!ctx->BindElementArrayBuffer(buffer))
        {
            GLES_ERROR(GL_OUT_OF_MEMORY);
        }
        break;
    default:
        GLES_ERROR(GL_INVALID_ENUM);
    }
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glBindTexture (GLenum target, GLuint texture)
{
    GLES_ENTER();
    GLES_ERROR_IF(target != GL_TEXTURE_2D, GL_INVALID_ENUM);
    ctx->GetHostError();
    ctx->DGL().glBindTexture (target, texture);
    if(ctx->GetHostError() == GL_NO_ERROR)
    {
        ctx->BindTexture(texture);
    }
    GLES_LEAVE();
}
bool isValidSrcBlendFuncEnum(GLenum func)
{
    switch(func)
    {
    case GL_ZERO:
    case GL_ONE:
    case GL_DST_COLOR:
    case GL_ONE_MINUS_DST_COLOR:
    case GL_SRC_ALPHA:
    case GL_ONE_MINUS_SRC_ALPHA:
    case GL_DST_ALPHA:
    case GL_ONE_MINUS_DST_ALPHA:
    case GL_SRC_ALPHA_SATURATE:
        return true;
    default:
        return false;
    }
}
bool isValidDstBlendFuncEnum(GLenum func)
{
    switch(func)
    {
    case GL_ZERO:
    case GL_ONE:
    case GL_SRC_COLOR:
    case GL_ONE_MINUS_SRC_COLOR:
    case GL_SRC_ALPHA:
    case GL_ONE_MINUS_SRC_ALPHA:
    case GL_DST_ALPHA:
    case GL_ONE_MINUS_DST_ALPHA:
        return true;
    default:
        return false;
    }
}
GL_API void GL_APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidSrcBlendFuncEnum(sfactor), GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidDstBlendFuncEnum(dfactor), GL_INVALID_ENUM);
    ctx->DGL().glBlendFunc (sfactor, dfactor);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
{
    GLES_ENTER();
    GLES_ERROR_IF(size < 0, GL_INVALID_VALUE);
    GLES_ERROR_IF(usage != GL_STATIC_DRAW && usage != GL_DYNAMIC_DRAW, GL_INVALID_ENUM);

    GLESBuffer* buffer;
    switch(target)
    {
    case GL_ARRAY_BUFFER:
        GLES_ERROR_IF(ctx->ArrayBufferBinding() == 0, GL_INVALID_OPERATION);
        buffer = ctx->ArrayBuffer();
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        GLES_ERROR_IF(ctx->ElementArrayBufferBinding() == 0, GL_INVALID_OPERATION);
        buffer = ctx->ElementArrayBuffer();
        break;
    default:
        GLES_ERROR(GL_INVALID_ENUM);
    }
    GLES_ASSERT(buffer);

    delete[] buffer->data;
    buffer->data = GLES_NEW char[size];
    if(buffer->data == NULL)
    {
        GLES_ERROR(GL_OUT_OF_MEMORY);
    }

    if(data != NULL)
    {
        memcpy(buffer->data, data, size);
    }

    buffer->size = size;
    buffer->usage = usage;

    GLES_LEAVE();
}
GL_API void GL_APIENTRY glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
{
    GLES_ENTER();
    GLES_ERROR_IF(offset < 0, GL_INVALID_VALUE);
    GLES_ERROR_IF(size < 0, GL_INVALID_VALUE);

    GLESBuffer* buffer;
    switch(target)
    {
    case GL_ARRAY_BUFFER:
        GLES_ERROR_IF(ctx->ArrayBufferBinding() == 0, GL_INVALID_OPERATION);
        buffer = ctx->ArrayBuffer();
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        GLES_ERROR_IF(ctx->ElementArrayBufferBinding() == 0, GL_INVALID_OPERATION);
        buffer = ctx->ElementArrayBuffer();
        break;
    default:
        GLES_ERROR(GL_INVALID_ENUM);
    }
    GLES_ASSERT(buffer);

    GLES_ERROR_IF(size + offset > buffer->size, GL_INVALID_VALUE);
    memcpy(static_cast<char*>(buffer->data) + offset, data, size);

    GLES_LEAVE();
}
GL_API void GL_APIENTRY glClear (GLbitfield mask)
{
	GLES_ENTER();
	GLES_LOCK_DRAW_SURFACE();
	ctx->DGL().glClear (mask);
	GLES_UNLOCK_SURFACE();
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glClearColorx (GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
    GLES_ENTER();
    ctx->DGL().glClearColor (X_TO_F(red), X_TO_F(green), X_TO_F(blue), X_TO_F(alpha));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glClearDepthx (GLclampx depth)
{
    GLES_ENTER();
    ctx->DGL().glClearDepth (X_TO_D(depth));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glClearStencil (GLint s)
{
    GLES_ENTER();
    ctx->DGL().glClearStencil (s);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glClientActiveTexture (GLenum texture)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidTextureTargetEnum(texture, ctx->MaxTextureUnits()), GL_INVALID_ENUM);
    ctx->GetHostError();
    ctx->DGL().glClientActiveTexture (texture);
    if(ctx->GetHostError() == GL_NO_ERROR)
    {
        ctx->SetClientActiveTexture(texture - GL_TEXTURE0);
    }
    GLES_LEAVE_NO_ERROR_CHECK();
}
GL_API void GL_APIENTRY glClipPlanex (GLenum plane, const GLfixed *equation)
{
    GLES_ENTER();
    GLdouble dequation[4];
    for(int i = 0; i < 4; i++)
    {
        dequation[i] = X_TO_D(equation[i]);
    }
    ctx->DGL().glClipPlane (plane, dequation);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
    GLES_ENTER();
    ctx->DGL().glColor4ub (red, green, blue, alpha);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glColor4x (GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
    GLES_ENTER();
    ctx->DGL().glColor4d(X_TO_D(red), X_TO_D(green), X_TO_D(blue), X_TO_D(alpha));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    GLES_ENTER();
    ctx->DGL().glColorMask (red, green, blue, alpha);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    GLES_ENTER();
    GLES_ERROR_IF(size != 4, GL_INVALID_VALUE);
    GLES_ERROR_IF(type != GL_UNSIGNED_BYTE && type != GL_FIXED && type != GL_FLOAT, GL_INVALID_ENUM);
    GLES_ERROR_IF(stride < 0, GL_INVALID_VALUE);
    ctx->SetColorArray(size, type, stride, pointer);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
{
	GLES_ENTER();
	GLES_ERROR_IF(target != GL_TEXTURE_2D, GL_INVALID_ENUM);
	GLES_ERROR_IF(level > 0, GL_INVALID_VALUE);
	GLES_ERROR_IF(!glesIsValidCompressedFormat(internalformat), GL_INVALID_ENUM);
	GLES_ERROR_IF(height < 0, GL_INVALID_VALUE);
	GLES_ERROR_IF(width < 0, GL_INVALID_VALUE);
	GLES_ERROR_IF(height > 0 && !glesIsPowerOfTwo(height), GL_INVALID_VALUE);
	GLES_ERROR_IF(width > 0 && !glesIsPowerOfTwo(width), GL_INVALID_VALUE);
	GLES_ERROR_IF(imageSize < 0, GL_INVALID_VALUE);

	GLenum baseFormat = glesMapCompressedToBaseFormat(internalformat);

	int numLevels = -level + 1;
	for(int curLevel = 0; curLevel < numLevels; curLevel++)
	{
		if(data != NULL)
		{
			void* uncompressedData = glesUncompressImage(curLevel, internalformat, width, height, imageSize, data);
			if(uncompressedData == NULL)
			{
				GLES_ERROR(GL_OUT_OF_MEMORY);
			}
			ctx->DGL().glTexImage2D(target, curLevel, baseFormat, width, height, border, baseFormat, GL_UNSIGNED_BYTE, uncompressedData);
			delete uncompressedData;
		}
		else
		{
			ctx->DGL().glTexImage2D(target, curLevel, baseFormat, width, height, border, baseFormat, GL_UNSIGNED_BYTE, NULL);
		}

		if(ctx->GetHostError() == GL_NO_ERROR)
		{
			GLESTexture* texture = ctx->Texture(ctx->TextureBinding());
			GLES_ASSERT(texture != NULL);
			texture->SetLevel(level, internalformat, width, height);

			if(texture->Level(level)->boundSurface != NULL)
			{
				// Texture is respecified. Release the bound EGLSurface.
				glesReleaseTexImage(texture->Level(level)->boundSurface, texture->Name(), level);
				texture->Level(level)->boundSurface = NULL;
			}
		}

		width /= 2;
		height /= 2;
	}
	
	GLES_LEAVE();
}
static bool isPalettedFormat(GLenum format)
{
    switch(format)
    {
    case GL_PALETTE4_RGB8_OES:
    case GL_PALETTE4_RGBA8_OES:
    case GL_PALETTE4_R5_G6_B5_OES:
    case GL_PALETTE4_RGBA4_OES:
    case GL_PALETTE4_RGB5_A1_OES:
    case GL_PALETTE8_RGB8_OES:
    case GL_PALETTE8_RGBA8_OES:
    case GL_PALETTE8_R5_G6_B5_OES:
    case GL_PALETTE8_RGBA4_OES:
    case GL_PALETTE8_RGB5_A1_OES:
        return true;
    default:
        return false;
    }
}
GL_API void GL_APIENTRY glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isPalettedFormat(format), GL_INVALID_ENUM);
    // Not supported for compressed paletted textures.
    GLES_ERROR(GL_INVALID_OPERATION);
    GLES_LEAVE();
}
bool isValidPixelFormatEnum(GLenum format)
{
    switch(format)
    {
    case GL_ALPHA:
    case GL_LUMINANCE:
    case GL_LUMINANCE_ALPHA:
    case GL_RGB:
    case GL_RGBA:
        return true;
    default:
        return false;
    }
}
GL_API void GL_APIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
	GLES_ENTER();
	GLES_ERROR_IF(target != GL_TEXTURE_2D, GL_INVALID_ENUM);
	GLES_ERROR_IF(!isValidPixelFormatEnum(internalformat), GL_INVALID_ENUM);
	
	GLES_LOCK_READ_SURFACE();
	ctx->DGL().glCopyTexImage2D (target, level, internalformat, x, y, width, height, border);
	GLES_UNLOCK_SURFACE();

	if(ctx->GetHostError() == GL_NO_ERROR)
	{
		GLint genMipmap;
		ctx->DGL().glGetTexParameteriv(target, GL_GENERATE_MIPMAP, &genMipmap);
		GLESTexture* texture = ctx->Texture(ctx->TextureBinding());
		GLES_ASSERT(texture != NULL);
		if(level == 0 && genMipmap)
		{
			texture->GenerateMipmap();
		}

		if(texture->Level(level)->boundSurface != NULL)
		{
			// Texture is respecified. Release the bound EGLSurface.
			glesReleaseTexImage(texture->Level(level)->boundSurface, texture->Name(), level);
			texture->Level(level)->boundSurface = NULL;
		}
	}

	GLES_LEAVE();
}
GL_API void GL_APIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	GLES_ENTER();
	GLES_ERROR_IF(target != GL_TEXTURE_2D, GL_INVALID_ENUM);

	GLESTexture* texture = ctx->Texture(ctx->TextureBinding());
	GLES_ASSERT(texture != NULL);
	const GLESTextureLevel* level_obj = texture->Level(level);
	if(isPalettedFormat(level_obj->format))
	{
		GLES_ERROR(GL_INVALID_OPERATION);
	}

	GLES_LOCK_READ_SURFACE();
	ctx->DGL().glCopyTexSubImage2D (target, level, xoffset, yoffset, x, y, width, height);
	GLES_UNLOCK_SURFACE();

	if(ctx->GetHostError() == GL_NO_ERROR)
	{
		GLint genMipmap;
		ctx->DGL().glGetTexParameteriv(target, GL_GENERATE_MIPMAP, &genMipmap);
		if(level == 0 && genMipmap)
		{
			GLESTexture* texture = ctx->Texture(ctx->TextureBinding());
			GLES_ASSERT(texture != NULL);
			texture->GenerateMipmap();
		}
	}

	GLES_LEAVE();
}
GL_API void GL_APIENTRY glCullFace (GLenum mode)
{
    GLES_ENTER();
    ctx->DGL().glCullFace (mode);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glDeleteBuffers (GLsizei n, const GLuint *buffers)
{
    GLES_ENTER();
    GLES_ERROR_IF(n < 0, GL_INVALID_VALUE);
    for(int i = 0; i < n; i++)
    {
        ctx->DeleteBuffer(buffers[i]);
    }
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures)
{
    GLES_ENTER();
    GLES_ERROR_IF(n < 0, GL_INVALID_VALUE);
    ctx->GetHostError();
    ctx->DGL().glDeleteTextures (n, textures);
    if(ctx->GetHostError() == GL_NO_ERROR)
    {
        for(int i = 0; i < n; i++)
        {
            ctx->DeleteTexture(textures[i]);
        }
    }
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glDepthFunc (GLenum func)
{
    GLES_ENTER();
    ctx->DGL().glDepthFunc (func);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glDepthMask (GLboolean flag)
{
    GLES_ENTER();
    ctx->DGL().glDepthMask (flag);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glDepthRangex (GLclampx zNear, GLclampx zFar)
{
    GLES_ENTER();
    ctx->DGL().glDepthRange(X_TO_D(zNear), X_TO_D(zFar));
    GLES_LEAVE();
}
bool isValidCapabilityEnum(GLenum cap)
{
    switch(cap)
    {
    case GL_NORMALIZE:
    case GL_RESCALE_NORMAL:
    case GL_CLIP_PLANE0:
    case GL_CLIP_PLANE1:
    case GL_CLIP_PLANE2:
    case GL_CLIP_PLANE3:
    case GL_CLIP_PLANE4:
    case GL_CLIP_PLANE5:
    case GL_FOG:
    case GL_LIGHTING:
    case GL_COLOR_MATERIAL:
    case GL_LIGHT0:
    case GL_LIGHT1:
    case GL_LIGHT2:
    case GL_LIGHT3:
    case GL_LIGHT4:
    case GL_LIGHT5:
    case GL_LIGHT6:
    case GL_LIGHT7:
    case GL_POINT_SMOOTH:
    case GL_POINT_SPRITE_OES:
    case GL_LINE_SMOOTH:
    case GL_CULL_FACE:
    case GL_POLYGON_OFFSET_FILL:
    case GL_MULTISAMPLE:
    case GL_SAMPLE_ALPHA_TO_COVERAGE:
    case GL_SAMPLE_ALPHA_TO_ONE:
    case GL_SAMPLE_COVERAGE:
    case GL_TEXTURE_2D:
    case GL_SCISSOR_TEST:
    case GL_ALPHA_TEST:
    case GL_STENCIL_TEST:
    case GL_DEPTH_TEST:
    case GL_BLEND:
    case GL_DITHER:
    case GL_COLOR_LOGIC_OP:
        return true;
    default:
        return false;
    }
}
GL_API void GL_APIENTRY glDisable (GLenum cap)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidCapabilityEnum(cap), GL_INVALID_ENUM);
    cap = cap == GL_POINT_SPRITE_OES ? GL_POINT_SPRITE : cap;
    ctx->DGL().glDisable (cap);
    GLES_LEAVE();
}
static GLESArrayFlag mapArrayType(GLenum type)
{
    switch(type)
    {
    case GL_VERTEX_ARRAY:
        return GLES_VERTEX_ARRAY;
    case GL_NORMAL_ARRAY:
        return GLES_NORMAL_ARRAY;
    case GL_COLOR_ARRAY:
        return GLES_COLOR_ARRAY;
    case GL_POINT_SIZE_ARRAY_OES:
        return GLES_POINT_SIZE_ARRAY;
    case GL_TEXTURE_COORD_ARRAY:
        return GLES_TEXTURE_COORD_ARRAY;
    default:
        return GLES_INVALID_ARRAY;
    }
}
GL_API void GL_APIENTRY glDisableClientState (GLenum array)
{
    GLES_ENTER();
    int flag = static_cast<int>(mapArrayType(array));
    GLES_ERROR_IF(flag == GLES_INVALID_ARRAY, GL_INVALID_ENUM);
    if(flag == GLES_TEXTURE_COORD_ARRAY)
    {
        flag <<= ctx->ClientActiveTexture();
    }
    ctx->DisableArray(static_cast<GLESArrayFlag>(flag));
    if(array != GL_POINT_SIZE_ARRAY_OES)
    {
        ctx->DGL().glDisableClientState(array);
    }
    GLES_LEAVE();
}
static bool convertArrays(const GLESContext& ctx, GLsizei count, GLESArray** vertexArr, GLESArray** normalArr,
                          GLESArray** colorArr, GLESArray** texCoordArr)
{
    *vertexArr = ctx.VertexArray().Convert(count);
    if(*vertexArr == NULL)
    {
        return false;
    }
    ctx.DGL().glVertexPointer((*vertexArr)->Size(), (*vertexArr)->Type(), (*vertexArr)->Stride(), (*vertexArr)->Pointer());

    if(ctx.IsArrayEnabled(GLES_NORMAL_ARRAY))
    {
        *normalArr = ctx.NormalArray().Convert(count);
        if(*normalArr == NULL)
        {
            return false;
        }
        ctx.DGL().glNormalPointer((*normalArr)->Type(), (*normalArr)->Stride(), (*normalArr)->Pointer());
    }

    if(ctx.IsArrayEnabled(GLES_COLOR_ARRAY))
    {
        *colorArr = ctx.ColorArray().Convert(count);
        if(*colorArr == NULL)
        {
            return false;
        }
        ctx.DGL().glColorPointer((*colorArr)->Size(), (*colorArr)->Type(), (*colorArr)->Stride(), (*colorArr)->Pointer());
    }

    for(unsigned int i = 0; i < ctx.MaxTextureUnits(); i++)
    {
        if(ctx.IsArrayEnabled(static_cast<GLESArrayFlag>(GLES_TEXTURE_COORD_ARRAY << i)))
        {
            texCoordArr[i] = ctx.TexCoordArray(i).Convert(count);
            if(texCoordArr[i] == NULL)
            {
                return false;
            }
            ctx.DGL().glClientActiveTexture(GL_TEXTURE0 + i);
            ctx.DGL().glTexCoordPointer(texCoordArr[i]->Size(), texCoordArr[i]->Type(),
                                    texCoordArr[i]->Stride(), texCoordArr[i]->Pointer());
        }
    }

    // Reset state
    ctx.DGL().glClientActiveTexture(GL_TEXTURE0 + ctx.ClientActiveTexture());

    return true;
}
void drawPoint(const GLESContext& ctx, int i, GLESArrayPointer& vertexPtr, GLESArrayPointer& normalPtr,
               GLESArrayPointer& colorPtr, GLESArrayPointer& pointSizePtr,
               GLESArrayPointer* texCoordPtr)
{
    if(normalPtr != NULL)
    {
        switch(normalPtr.Type())
        {
        case GL_BYTE:
        case GL_SHORT:
            ctx.DGL().glNormal3sv(static_cast<const GLshort*>(normalPtr[i]));
            break;
        case GL_FIXED:
        case GL_FLOAT:
            ctx.DGL().glNormal3fv(static_cast<const GLfloat*>(normalPtr[i]));
            break;
        default:
            GLES_ASSERT(false);
        }
    }

    if(colorPtr != NULL)
    {
        switch(colorPtr.Type())
        {
        case GL_UNSIGNED_BYTE:
            ctx.DGL().glColor4ubv(static_cast<const GLubyte*>(colorPtr[i]));
            break;
        case GL_FIXED:
        case GL_FLOAT:
            ctx.DGL().glColor4fv(static_cast<const GLfloat*>(colorPtr[i]));
            break;
        default:
            GLES_ASSERT(false);
        }
    }

    GLES_ASSERT(pointSizePtr != NULL);
    ctx.DGL().glPointSize(*static_cast<const GLfloat*>(pointSizePtr[i]));

    for(unsigned int j = 0; j < ctx.MaxTextureUnits(); j++)
    {
        int texture = GL_TEXTURE0 + j;
		
        if(texCoordPtr[j] != NULL)
        {
            switch(texCoordPtr[j].Type())
            {
            case GL_BYTE:
            case GL_SHORT:
                switch(texCoordPtr[j].Size())
                {
                case 2:
                    ctx.DGL().glMultiTexCoord2sv(texture, static_cast<const GLshort*>(texCoordPtr[j][i]));
                    break;
                case 3:
                    ctx.DGL().glMultiTexCoord3sv(texture, static_cast<const GLshort*>(texCoordPtr[j][i]));
                    break;
                case 4:
                    ctx.DGL().glMultiTexCoord4sv(texture, static_cast<const GLshort*>(texCoordPtr[j][i]));
                    break;
                default:
                    GLES_ASSERT(false);
                }
                break;
            case GL_FIXED:
            case GL_FLOAT:
                switch(texCoordPtr[j].Size())
                {
                case 2:
                    ctx.DGL().glMultiTexCoord2fv(texture, static_cast<const GLfloat*>(texCoordPtr[j][i]));
                    break;
                case 3:
                    ctx.DGL().glMultiTexCoord3fv(texture, static_cast<const GLfloat*>(texCoordPtr[j][i]));
                    break;
                case 4:
                    ctx.DGL().glMultiTexCoord4fv(texture, static_cast<const GLfloat*>(texCoordPtr[j][i]));
                    break;
                default:
                    GLES_ASSERT(false);
                }
                break;
            default:
                GLES_ASSERT(false);
            }
        }
    }

    GLES_ASSERT(vertexPtr != NULL);
    ctx.DGL().glBegin(GL_POINTS);
    switch(vertexPtr.Type())
    {
    case GL_BYTE:
    case GL_SHORT:
        switch(vertexPtr.Size())
        {
        case 2:
            ctx.DGL().glVertex2sv(static_cast<const GLshort*>(vertexPtr[i]));
            break;
        case 3:
            ctx.DGL().glVertex3sv(static_cast<const GLshort*>(vertexPtr[i]));
            break;
        case 4:
            ctx.DGL().glVertex4sv(static_cast<const GLshort*>(vertexPtr[i]));
            break;
        default:
            GLES_ASSERT(false);
        }
        break;
    case GL_FIXED:
    case GL_FLOAT:
        switch(vertexPtr.Size())
        {
        case 2:
            ctx.DGL().glVertex2fv(static_cast<const GLfloat*>(vertexPtr[i]));
            break;
        case 3:
            ctx.DGL().glVertex3fv(static_cast<const GLfloat*>(vertexPtr[i]));
            break;
        case 4:
            ctx.DGL().glVertex4fv(static_cast<const GLfloat*>(vertexPtr[i]));
            break;
        default:
            GLES_ASSERT(false);
        }
        break;
    default:
        GLES_ASSERT(false);
    }
    ctx.DGL().glEnd();
}
bool drawPointArrays(const GLESContext& ctx, int first, int count,
                     GLESArray* vertexArr, GLESArray* normalArr, GLESArray* colorArr, GLESArray** texCoordArr)
{
    GLESArray* pointSizeArr = ctx.PointSizeArray().Convert(count);
    if(pointSizeArr == NULL)
    {
        return false;
    }

    GLES_ASSERT(vertexArr != NULL);
    GLESArrayPointer vertexPtr = vertexArr->ArrayPointer();
    GLESArrayPointer normalPtr = normalArr != NULL ? normalArr->ArrayPointer() : GLESArrayPointer();
    GLESArrayPointer colorPtr = colorArr != NULL ? colorArr->ArrayPointer() : GLESArrayPointer();
    GLESArrayPointer pointSizePtr = pointSizeArr != NULL ? pointSizeArr->ArrayPointer() : GLESArrayPointer();

    GLESArrayPointer* texCoordPtr = GLES_NEW GLESArrayPointer[ctx.MaxTextureUnits()];
    if(texCoordPtr == NULL)
    {
        delete pointSizeArr;
        return false;
    }
    for(unsigned int i = 0; i < ctx.MaxTextureUnits(); i++)
    {
        texCoordPtr[i] = texCoordArr[i] != NULL ? texCoordArr[i]->ArrayPointer() : GLESArrayPointer();
    }

    // Draw individual points

    for(int i = first; i < first + count; i++)
    {
        drawPoint(ctx, i, vertexPtr, normalPtr, colorPtr, pointSizePtr, texCoordPtr);
    }

    delete[] texCoordPtr;
    delete pointSizeArr;

    return true;
}
bool drawPointElements(const GLESContext& ctx, int count, GLenum type, const void* indices,
                       GLESArray* vertexArr, GLESArray* normalArr, GLESArray* colorArr, GLESArray** texCoordArr)
{
    GLESArray* pointSizeArr = ctx.PointSizeArray().Convert(count);
    if(pointSizeArr == NULL)
    {
        return false;
    }

    GLES_ASSERT(vertexArr != NULL);
    GLESArrayPointer vertexPtr = vertexArr->ArrayPointer();
    GLESArrayPointer normalPtr = normalArr != NULL ? normalArr->ArrayPointer() : GLESArrayPointer();
    GLESArrayPointer colorPtr = colorArr != NULL ? colorArr->ArrayPointer() : GLESArrayPointer();
    GLESArrayPointer pointSizePtr = pointSizeArr != NULL ? pointSizeArr->ArrayPointer() : GLESArrayPointer();

    GLESArrayPointer* texCoordPtr = GLES_NEW GLESArrayPointer[ctx.MaxTextureUnits()];
    if(texCoordPtr == NULL)
    {
        delete pointSizeArr;
        return false;
    }
    for(unsigned int i = 0; i < ctx.MaxTextureUnits(); i++)
    {
        texCoordPtr[i] = texCoordArr[i] != NULL ? texCoordArr[i]->ArrayPointer() : GLESArrayPointer();
    }

    // Draw individual points

    for(int i = 0; i < count; i++)
    {
        int index;
        switch(type)
        {
        case GL_UNSIGNED_BYTE:
            index = static_cast<const GLubyte*>(indices)[i];
            break;
        case GL_UNSIGNED_SHORT:
            index = static_cast<const GLushort*>(indices)[i];
            break;
        default:
            GLES_ASSERT(false);
        }
        drawPoint(ctx, index, vertexPtr, normalPtr, colorPtr, pointSizePtr, texCoordPtr);
    }

    delete[] texCoordPtr;
    delete pointSizeArr;

    return true;
}
GL_API void GL_APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count)
{
	GLES_ENTER();
	GLES_ERROR_IF(count < 0, GL_INVALID_VALUE);

    if(!ctx->IsArrayEnabled(GLES_VERTEX_ARRAY) || !count)
    {
        // Nothing to draw
        GLES_LEAVE();
    }

    GLESArray* vertexArr = NULL;
    GLESArray* normalArr = NULL;
    GLESArray* colorArr = NULL;
    GLESArray** texCoordArr = GLES_NEW GLESArray*[ctx->MaxTextureUnits()];
    for(unsigned int i = 0; i < ctx->MaxTextureUnits(); i++)
    {
        texCoordArr[i] = NULL;
    }

    bool oom = !convertArrays(*ctx, count, &vertexArr, &normalArr, &colorArr, texCoordArr);
    if(!oom)
    {
		GLES_LOCK_DRAW_SURFACE();
        if(mode == GL_POINTS && ctx->IsArrayEnabled(GLES_POINT_SIZE_ARRAY))
        {
            oom = !drawPointArrays(*ctx, first, count, vertexArr, normalArr, colorArr, texCoordArr);
        }
        else
        {
            ctx->DGL().glDrawArrays (mode, first, count);
        }
		GLES_UNLOCK_SURFACE();
    }

    for(unsigned int i = 0; i < ctx->MaxTextureUnits(); i++)
    {
        delete texCoordArr[i];
    }
    delete colorArr;
    delete normalArr;
    delete vertexArr;
    delete[] texCoordArr;

    if(oom)
    {
        GLES_ERROR(GL_OUT_OF_MEMORY);
    }

    GLES_LEAVE();
}
GL_API void GL_APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
	GLES_ENTER();
	GLES_ERROR_IF(count < 0, GL_INVALID_VALUE);
	GLES_ERROR_IF(type != GL_UNSIGNED_BYTE && type != GL_UNSIGNED_SHORT, GL_INVALID_ENUM);

    if(!ctx->IsArrayEnabled(GLES_VERTEX_ARRAY) || !count)
    {
        // Nothing to draw
        GLES_LEAVE();
    }

    GLESArray* vertexArr = NULL;
    GLESArray* normalArr = NULL;
    GLESArray* colorArr = NULL;
    GLESArray** texCoordArr = GLES_NEW GLESArray*[ctx->MaxTextureUnits()];
    for(unsigned int i = 0; i < ctx->MaxTextureUnits(); i++)
    {
        texCoordArr[i] = NULL;
    }

    bool oom = !convertArrays(*ctx, count, &vertexArr, &normalArr, &colorArr, texCoordArr);
    if(!oom)
    {
        if(indices == NULL && ctx->ElementArrayBufferBinding())
        {
            indices = ctx->ElementArrayBuffer()->data;
        }

		GLES_LOCK_DRAW_SURFACE();
        if(mode == GL_POINTS && ctx->IsArrayEnabled(GLES_POINT_SIZE_ARRAY))
        {
            oom = !drawPointElements(*ctx, count, type, indices,
                                     vertexArr, normalArr, colorArr, texCoordArr);
        }
        else
        {
            ctx->DGL().glDrawElements (mode, count, type, indices);
        }
		GLES_UNLOCK_SURFACE();
    }

    for(unsigned int i = 0; i < ctx->MaxTextureUnits(); i++)
    {
        delete texCoordArr[i];
    }
    delete colorArr;
    delete normalArr;
    delete vertexArr;
    delete[] texCoordArr;

    if(oom)
    {
        GLES_ERROR(GL_OUT_OF_MEMORY);
    }

    GLES_LEAVE();
}
GL_API void GL_APIENTRY glEnable (GLenum cap)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidCapabilityEnum(cap), GL_INVALID_ENUM);
    ctx->DGL().glEnable (cap);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glEnableClientState (GLenum array)
{
    GLES_ENTER();
    array = array == GL_POINT_SIZE_ARRAY_OES ? GL_POINT_SIZE_ARRAY : array;
    int flag = static_cast<int>(mapArrayType(array));
    GLES_ERROR_IF(flag == GLES_INVALID_ARRAY, GL_INVALID_ENUM);
    if(flag == GLES_TEXTURE_COORD_ARRAY)
    {
        flag <<= ctx->ClientActiveTexture();
    }
    ctx->EnableArray(static_cast<GLESArrayFlag>(flag));
    if(array != GL_POINT_SIZE_ARRAY_OES)
    {
        ctx->DGL().glEnableClientState(array);
    }
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glFinish (void)
{
	GLES_ENTER();
	ctx->DGL().glFinish();
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glFlush (void)
{
	GLES_ENTER();
	ctx->DGL().glFlush();
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glFogx (GLenum pname, GLfixed param)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidSingleValuedFogParamEnum(pname), GL_INVALID_ENUM);
    if(pname == GL_FOG_MODE)
    {
        ctx->DGL().glFogf(pname, static_cast<GLfloat>(param));
    }
    else
    {
        ctx->DGL().glFogf(pname, X_TO_F(param));
    }
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glFogxv (GLenum pname, const GLfixed *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidFogParamEnum(pname), GL_INVALID_ENUM);
    if(pname == GL_FOG_MODE)
    {
        GLfloat fparam = static_cast<GLfloat>(params[0]);
        ctx->DGL().glFogfv (pname, &fparam);
    }
    else
    {
        GLfloat fparams[4];
        fparams[0] = X_TO_F(params[0]);
        if (pname == GL_FOG_COLOR)
        {
            fparams[1] = X_TO_F(params[1]);
            fparams[2] = X_TO_F(params[2]);
            fparams[3] = X_TO_F(params[3]);
        }
        ctx->DGL().glFogfv (pname, fparams);
    }
	
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glFrontFace (GLenum mode)
{
    GLES_ENTER();
    ctx->DGL().glFrontFace (mode);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glFrustumx (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
    GLES_ENTER();
    ctx->DGL().glFrustum (X_TO_D(left), X_TO_D(right), X_TO_D(bottom), X_TO_D(top), X_TO_D(zNear), X_TO_D(zFar));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glGetBooleanv (GLenum pname, GLboolean *params)
{
    GLES_ENTER();
    if(!glesGetParameter(*ctx, ctx->DGL(), pname, GLES_TYPE_BOOLEAN, params))
    {
        GLES_ERROR(GL_INVALID_ENUM);
    }
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glGetBufferParameteriv (GLenum target, GLenum pname, GLint *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidBufferTarget(target), GL_INVALID_ENUM);
	
    const GLESBuffer* buffer;
    switch(target)
    {
    case GL_ARRAY_BUFFER:
        buffer = ctx->ArrayBuffer();
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        buffer = ctx->ElementArrayBuffer();
        break;
    default:
        GLES_ERROR(GL_INVALID_ENUM);
    }

    switch(pname)
    {
    case GL_BUFFER_SIZE:
        *params = buffer->size;
        break;
    case GL_BUFFER_USAGE:
        *params = buffer->usage;
        break;
    default:
        GLES_ERROR(GL_INVALID_ENUM);
    }

    GLES_LEAVE();
}
GL_API void GL_APIENTRY glGetClipPlanex (GLenum pname, GLfixed eqn[4])
{
    GLES_ENTER();
    GLES_ERROR_IF(pname < GL_CLIP_PLANE0 || pname >= GL_CLIP_PLANE0 + ctx->MaxClipPlanes(), GL_INVALID_ENUM);
    GLdouble deqn[4];
    ctx->DGL().glGetClipPlane(pname, deqn);
    for (int i = 0; i < 4; i++)
    {
        eqn[i] = D_TO_X(deqn[i]);
    }
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glGenBuffers (GLsizei n, GLuint *buffers)
{
    GLES_ENTER();
    GLES_ERROR_IF(n < 0, GL_INVALID_VALUE);
    ctx->ReserveBufferNames(n, buffers);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glGenTextures (GLsizei n, GLuint *textures)
{
    GLES_ENTER();
    ctx->DGL().glGenTextures (n, textures);
    GLES_LEAVE();
}
GL_API GLenum GL_APIENTRY glGetError (void)
{
    GLES_ENTER_RET(GL_INVALID_OPERATION);
    GLenum error = ctx->Error();
    ctx->SetError(GL_NO_ERROR);
    GLES_LEAVE_RET(error);
}
GL_API void GL_APIENTRY glGetFixedv (GLenum pname, GLfixed *params)
{
    GLES_ENTER();
    if(!glesGetParameter(*ctx, ctx->DGL(), pname, GLES_TYPE_FIXED, params))
    {
        GLES_ERROR(GL_INVALID_ENUM);
    }
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glGetIntegerv (GLenum pname, GLint *params)
{
    GLES_ENTER();
    if(!glesGetParameter(*ctx, ctx->DGL(), pname, GLES_TYPE_INTEGER, params))
    {
        GLES_ERROR(GL_INVALID_ENUM);
    }
    GLES_LEAVE();
}
bool isValidLightEnum(GLenum light, unsigned int maxLights)
{
    return light >= GL_LIGHT0 && light < GL_LIGHT0 + maxLights;
}
GL_API void GL_APIENTRY glGetLightxv (GLenum light, GLenum pname, GLfixed *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidLightEnum(light, ctx->MaxLights()), GL_INVALID_ENUM);
	
    float fparams[4];
    ctx->DGL().glGetLightfv(light, pname, fparams);

    switch (pname)
    {
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_SPECULAR:
    case GL_POSITION:
        params[3] = F_TO_X(fparams[3]);
    case GL_SPOT_DIRECTION:
        params[2] = F_TO_X(fparams[2]);
        params[1] = F_TO_X(fparams[1]);
    case GL_SPOT_EXPONENT:
    case GL_SPOT_CUTOFF:
    case GL_CONSTANT_ATTENUATION:
    case GL_LINEAR_ATTENUATION:
    case GL_QUADRATIC_ATTENUATION:
        params[0] = F_TO_X(fparams[0]);
        break;
    default:
        GLES_ERROR(GL_INVALID_ENUM);
    }
	
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glGetMaterialxv (GLenum face, GLenum pname, GLfixed *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidMaterialParamEnum(pname), GL_INVALID_ENUM);
    GLfloat fparams[4];
    ctx->DGL().glGetMaterialfv (face, pname, fparams);
    params[0] = F_TO_X(fparams[0]);
    if(pname != GL_SHININESS)
    {
        params[1] = F_TO_X(fparams[1]);
        params[2] = F_TO_X(fparams[2]);
        params[3] = F_TO_X(fparams[3]);
    }
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glGetPointerv (GLenum pname, GLvoid **params)
{
    GLES_ENTER();
    switch(pname)
    {
    case GL_VERTEX_ARRAY_POINTER:
        *params = const_cast<GLvoid*>(ctx->VertexArray().Pointer());
        break;
    case GL_NORMAL_ARRAY_POINTER:
        *params = const_cast<GLvoid*>(ctx->NormalArray().Pointer());
        break;
    case GL_COLOR_ARRAY_POINTER:
        *params = const_cast<GLvoid*>(ctx->ColorArray().Pointer());
        break;
    case GL_POINT_SIZE_ARRAY_POINTER_OES:
        *params = const_cast<GLvoid*>(ctx->PointSizeArray().Pointer());
        break;
    case GL_TEXTURE_COORD_ARRAY_POINTER:
        *params = const_cast<GLvoid*>(ctx->TexCoordArray().Pointer());
        break;
    default:
        GLES_ERROR(GL_INVALID_ENUM);
    }
    GLES_LEAVE();
}
GL_API const GLubyte * GL_APIENTRY glGetString (GLenum name)
{
    GLES_ENTER_RET(NULL);
    static const GLubyte vendor[] = "Nokia";
    static const GLubyte renderer[] = "OpenGL ES-CM 1.1";
    static const GLubyte version[] = "OpenGL ES-CM 1.1";
    static const GLubyte extensions[] = "GL_OES_read_format GL_OES_compressed_paletted_texture "
                                        "GL_OES_point_size_array GL_OES_point_sprite";
    const GLubyte* str = NULL;
    switch(name)
    {
    case GL_VENDOR:
        str = vendor;
        break;
    case GL_RENDERER:
        str = renderer;
        break;
    case GL_VERSION:
        str = version;
        break;
    case GL_EXTENSIONS:
        str = extensions;
        break;
    default:
        GLES_ERROR_RET(GL_INVALID_ENUM, NULL);
        break;
    }
    GLES_LEAVE_RET(str);
}
GL_API void GL_APIENTRY glGetTexEnviv (GLenum env, GLenum pname, GLint *params)
{
    GLES_ENTER();
    pname = pname == GL_COORD_REPLACE_OES ? GL_COORD_REPLACE : pname;
    ctx->DGL().glGetTexEnviv (env, pname, params);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glGetTexEnvxv (GLenum env, GLenum pname, GLfixed *params)
{
    GLES_ENTER();
    GLfloat fparams[4];

    pname = pname == GL_COORD_REPLACE_OES ? GL_COORD_REPLACE : pname;

    switch (pname)
    {
    case GL_TEXTURE_ENV_COLOR:
        ctx->DGL().glGetTexEnvfv(env, pname, fparams);
        params[0] = F_TO_X(fparams[0]);
        params[1] = F_TO_X(fparams[1]);
        params[2] = F_TO_X(fparams[2]);
        params[3] = F_TO_X(fparams[3]);
        break;
    case GL_RGB_SCALE:
    case GL_ALPHA_SCALE:
        ctx->DGL().glGetTexEnvfv(env, pname, fparams);
        params[0] = F_TO_X(fparams[0]);
        break;
    case GL_COMBINE_RGB:
    case GL_COMBINE_ALPHA:
    case GL_COORD_REPLACE:
    case GL_TEXTURE_ENV_MODE:
    case GL_SRC0_RGB:
    case GL_SRC0_ALPHA:
    case GL_SRC1_RGB:
    case GL_SRC1_ALPHA:
    case GL_SRC2_RGB:
    case GL_SRC2_ALPHA:
    case GL_OPERAND0_RGB:
    case GL_OPERAND0_ALPHA:
    case GL_OPERAND1_RGB:
    case GL_OPERAND1_ALPHA:
    case GL_OPERAND2_RGB:
    case GL_OPERAND2_ALPHA:
        {
            GLint i;
            ctx->DGL().glGetTexEnviv(env, pname, &i);
            params[0] = static_cast<GLfixed>(i);
        }
        break;
    default:
        GLES_ERROR(GL_INVALID_ENUM);
    }
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(target != GL_TEXTURE_2D, GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidTexParamEnum(pname), GL_INVALID_ENUM);
    ctx->DGL().glGetTexParameteriv (target, pname, params);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glGetTexParameterxv (GLenum target, GLenum pname, GLfixed *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(target != GL_TEXTURE_2D, GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidTexParamEnum(pname), GL_INVALID_ENUM);
    // All parameters are enum values, no conversion to be done.
    ctx->DGL().glGetTexParameteriv (target, pname, params);
    GLES_LEAVE();
}
bool isValidHintEnum(GLenum hint)
{
    switch(hint)
    {
    case GL_PERSPECTIVE_CORRECTION_HINT:
    case GL_POINT_SMOOTH_HINT:
    case GL_LINE_SMOOTH_HINT:
    case GL_FOG_HINT:
    case GL_GENERATE_MIPMAP_HINT:
        return true;
    default:
        return false;
    }
}
GL_API void GL_APIENTRY glHint (GLenum target, GLenum mode)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidHintEnum(target), GL_INVALID_ENUM);
    ctx->DGL().glHint (target, mode);
    GLES_LEAVE();
}
GL_API GLboolean GL_APIENTRY glIsBuffer (GLuint buffer)
{
    GLES_ENTER_RET(GL_FALSE);
    return ctx->Buffer(buffer) != NULL;
    GLES_LEAVE_RET(GL_FALSE);
}
GL_API GLboolean GL_APIENTRY glIsEnabled (GLenum cap)
{
    GLES_ENTER_RET(GL_FALSE);
    switch(cap)
    {
    case GL_VERTEX_ARRAY:
        return ctx->IsArrayEnabled(GLES_VERTEX_ARRAY);
    case GL_NORMAL_ARRAY:
        return ctx->IsArrayEnabled(GLES_NORMAL_ARRAY);
    case GL_COLOR_ARRAY:
        return ctx->IsArrayEnabled(GLES_COLOR_ARRAY);
    case GL_POINT_SIZE_ARRAY_OES:
        return ctx->IsArrayEnabled(GLES_POINT_SIZE_ARRAY);
    case GL_TEXTURE_COORD_ARRAY:
        {
        int flag = static_cast<int>(GLES_TEXTURE_COORD_ARRAY);
        flag <<= ctx->ClientActiveTexture();
        return ctx->IsArrayEnabled(static_cast<GLESArrayFlag>(flag));
        }
    case GL_POINT_SPRITE_OES:
        return ctx->DGL().glIsEnabled(GL_POINT_SPRITE);
    default:
        GLES_ERROR_IF_RET(!isValidCapabilityEnum(cap), GL_INVALID_ENUM, GL_FALSE);
        return ctx->DGL().glIsEnabled(cap);
    }
    GLES_LEAVE_RET(ctx->DGL().glIsEnabled (cap));
}
GL_API GLboolean GL_APIENTRY glIsTexture (GLuint texture)
{
    GLES_ENTER_RET(GL_FALSE);
    GLES_LEAVE_RET(ctx->DGL().glIsTexture (texture));
}
GL_API void GL_APIENTRY glLightModelx (GLenum pname, GLfixed param)
{
    GLES_ENTER();
    GLES_ERROR_IF(pname != GL_LIGHT_MODEL_TWO_SIDE, GL_INVALID_ENUM);
    ctx->DGL().glLightModelf (pname, static_cast<GLfloat>(param));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glLightModelxv (GLenum pname, const GLfixed *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidLightModelParamEnum(pname), GL_INVALID_ENUM);
    if(pname == GL_LIGHT_MODEL_TWO_SIDE)
    {
        GLfloat fparam = static_cast<GLfloat>(params[0]);
        ctx->DGL().glLightModelfv (pname, &fparam);
    }
    else
    {
        GLfloat fparams[4];
        fparams[0] = X_TO_F(params[0]);
        if(pname == GL_LIGHT_MODEL_AMBIENT)
        {
            fparams[1] = X_TO_F(params[1]);
            fparams[2] = X_TO_F(params[2]);
            fparams[3] = X_TO_F(params[3]);
        }
        ctx->DGL().glLightModelfv (pname, fparams);
    }
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glLightx (GLenum light, GLenum pname, GLfixed param)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidLightEnum(light, ctx->MaxLights()), GL_INVALID_ENUM);
    ctx->DGL().glLightf (light, pname, X_TO_F(param));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glLightxv (GLenum light, GLenum pname, const GLfixed *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidLightEnum(light, ctx->MaxLights()), GL_INVALID_ENUM);
    GLfloat fparams[4];
    switch(pname)
    {
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_SPECULAR:
    case GL_POSITION:
        fparams[3] = X_TO_F(params[3]);
    case GL_SPOT_DIRECTION:
        fparams[2] = X_TO_F(params[2]);
        fparams[1] = X_TO_F(params[1]);
        break;
    }
    fparams[0] = X_TO_F(params[0]);
    ctx->DGL().glLightfv (light, pname, fparams);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glLineWidthx (GLfixed width)
{
    GLES_ENTER();
    ctx->DGL().glLineWidth (X_TO_F(width));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glLoadIdentity (void)
{
    GLES_ENTER();
    ctx->DGL().glLoadIdentity ();
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glLoadMatrixx (const GLfixed *m)
{
    GLES_ENTER();
    GLfloat fm[16];
    for(int i = 0; i < 16; i++)
    {
        fm[i] = X_TO_F(m[i]);
    }
    ctx->DGL().glLoadMatrixf (fm);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glLogicOp (GLenum opcode)
{
    GLES_ENTER();
    ctx->DGL().glLogicOp (opcode);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glMaterialx (GLenum face, GLenum pname, GLfixed param)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidSingleValuedMaterialParamEnum(pname), GL_INVALID_ENUM);
    ctx->DGL().glMaterialf (face, pname, X_TO_F(param));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glMaterialxv (GLenum face, GLenum pname, const GLfixed *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidMaterialParamEnum(pname), GL_INVALID_ENUM);
    GLfloat fparams[4];
    for(int i = 0; i < 4; i++)
    {
        fparams[i] = X_TO_F(params[i]);
    }
    ctx->DGL().glMaterialfv (face, pname, fparams);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glMatrixMode (GLenum mode)
{
    GLES_ENTER();
    ctx->DGL().glMatrixMode (mode);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glMultMatrixx (const GLfixed *m)
{
    GLES_ENTER();
    GLfloat fm[16];
    for(int i = 0; i < 16; i++)
    {
        fm[i] = X_TO_F(m[i]);
    }
    ctx->DGL().glMultMatrixf (fm);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glMultiTexCoord4x (GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidTextureTargetEnum(target, ctx->MaxTextureUnits()), GL_INVALID_ENUM);
    ctx->DGL().glMultiTexCoord4f (target, X_TO_F(s), X_TO_F(t), X_TO_F(r), X_TO_F(q));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glNormal3x (GLfixed nx, GLfixed ny, GLfixed nz)
{
    GLES_ENTER();
    ctx->DGL().glNormal3f (X_TO_F(nx), X_TO_F(ny), X_TO_F(nz));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer)
{
    GLES_ENTER();
    GLES_ERROR_IF(type != GL_BYTE && type != GL_SHORT && type != GL_FIXED && type != GL_FLOAT, GL_INVALID_ENUM);
    GLES_ERROR_IF(stride < 0, GL_INVALID_VALUE);
    ctx->SetNormalArray(type, stride, pointer);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glOrthox (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
    GLES_ENTER();
    ctx->DGL().glOrtho (X_TO_D(left), X_TO_D(right), X_TO_D(bottom), X_TO_D(top), X_TO_D(zNear), X_TO_D(zFar));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glPixelStorei (GLenum pname, GLint param)
{
    GLES_ENTER();
    ctx->DGL().glPixelStorei (pname, param);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glPointParameterx (GLenum pname, GLfixed param)
{
    GLES_ENTER();
    ctx->DGL().glPointParameterf (pname, X_TO_F(param));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glPointParameterxv (GLenum pname, const GLfixed *params)
{
    GLES_ENTER();
    GLfloat fparams[3];
    fparams[0] = X_TO_F(params[0]);
    if(pname == GL_POINT_DISTANCE_ATTENUATION)
    {
        fparams[1] = X_TO_F(params[1]);
        fparams[2] = X_TO_F(params[2]);
    }
    ctx->DGL().glPointParameterfv (pname, fparams);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glPointSizex (GLfixed size)
{
    GLES_ENTER();
    ctx->DGL().glPointSize (X_TO_F(size));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glPolygonOffsetx (GLfixed factor, GLfixed units)
{
    GLES_ENTER();
    ctx->DGL().glPolygonOffset (X_TO_F(factor), X_TO_F(units));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glPopMatrix (void)
{
    GLES_ENTER();
    ctx->DGL().glPopMatrix();
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glPushMatrix (void)
{
    GLES_ENTER();
    ctx->DGL().glPushMatrix();
    GLES_LEAVE();
}
bool isValidPixelTypeEnum(GLenum type)
{
    switch(type)
    {
    case GL_UNSIGNED_BYTE:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_5_5_5_1:
        return true;
    default:
        return false;
    }
}
GL_API void GL_APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
{
	GLES_ENTER();
	GLES_ERROR_IF(!isValidPixelFormatEnum(format), GL_INVALID_ENUM);
	GLES_ERROR_IF(!isValidPixelTypeEnum(type), GL_INVALID_ENUM);
	GLES_LOCK_READ_SURFACE();
	ctx->DGL().glReadPixels (x, y, width, height, format, type, pixels);
	GLES_UNLOCK_SURFACE();
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glRotatex (GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
    GLES_ENTER();
    ctx->DGL().glRotatef (X_TO_F(angle), X_TO_F(x), X_TO_F(y), X_TO_F(z));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glSampleCoverage (GLclampf value, GLboolean invert)
{
    GLES_ENTER();
    ctx->DGL().glSampleCoverage (value, invert);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glSampleCoveragex (GLclampx value, GLboolean invert)
{
    GLES_ENTER();
    ctx->DGL().glSampleCoverage (X_TO_F(value), invert);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glScalex (GLfixed x, GLfixed y, GLfixed z)
{
    GLES_ENTER();
    ctx->DGL().glScalef (X_TO_F(x), X_TO_F(y), X_TO_F(z));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLES_ENTER();
    ctx->DGL().glScissor (x, y, width, height);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glShadeModel (GLenum mode)
{
    GLES_ENTER();
    ctx->DGL().glShadeModel (mode);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask)
{
    GLES_ENTER();
    ctx->DGL().glStencilFunc (func, ref, mask);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glStencilMask (GLuint mask)
{
    GLES_ENTER();
    ctx->DGL().glStencilMask (mask);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
    GLES_ENTER();
    ctx->DGL().glStencilOp (fail,  zfail, zpass);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    GLES_ENTER();
    GLES_ERROR_IF(size != 2 && size != 3 && size != 4, GL_INVALID_VALUE);
    GLES_ERROR_IF(type != GL_BYTE && type != GL_SHORT && type != GL_FIXED && type != GL_FLOAT, GL_INVALID_ENUM);
    GLES_ERROR_IF(stride < 0, GL_INVALID_VALUE);
    ctx->SetTexCoordArray(size, type, stride, pointer);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glTexEnvi (GLenum target, GLenum pname, GLint param)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidTexEnvTargetEnum(target), GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidSingleValuedTexEnvParamEnum(pname), GL_INVALID_ENUM);
    ctx->DGL().glTexEnvi (target, pname, param);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glTexEnvx (GLenum target, GLenum pname, GLfixed param)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidTexEnvTargetEnum(target), GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidSingleValuedTexEnvParamEnum(pname), GL_INVALID_ENUM);
    GLfloat fparam;
    if(target == GL_TEXTURE_ENV && (pname == GL_RGB_SCALE || pname == GL_ALPHA_SCALE))
    {
        fparam = X_TO_F(param);
    }
    else
    {
        fparam = static_cast<GLfloat>(param);
    }
    ctx->DGL().glTexEnvf (target, pname, fparam);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glTexEnviv (GLenum target, GLenum pname, const GLint *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidTexEnvTargetEnum(target), GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidTexEnvParamEnum(pname), GL_INVALID_ENUM);
    ctx->DGL().glTexEnviv (target, pname, params);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glTexEnvxv (GLenum target, GLenum pname, const GLfixed *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(!isValidTexEnvTargetEnum(target), GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidTexEnvParamEnum(pname), GL_INVALID_ENUM);
    GLfloat fparams[4];
    switch(pname)
    {
    case GL_TEXTURE_ENV_COLOR:
        fparams[3] = X_TO_F(params[3]);
        fparams[2] = X_TO_F(params[2]);
        fparams[1] = X_TO_F(params[1]);
        // fall-through
    case GL_RGB_SCALE:
    case GL_ALPHA_SCALE:
        fparams[0] = X_TO_F(params[0]);
        break;
    default:
        fparams[0] = static_cast<GLfloat>(params[0]);
    }
    ctx->DGL().glTexEnvfv (target, pname, fparams);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
	GLES_ENTER();
	GLES_ERROR_IF(!isValidPixelFormatEnum(internalformat), GL_INVALID_ENUM);
	GLES_ERROR_IF(!isValidPixelFormatEnum(format), GL_INVALID_ENUM);
	GLES_ERROR_IF(!isValidPixelTypeEnum(type), GL_INVALID_ENUM);
	GLES_ERROR_IF(internalformat != (GLint)format, GL_INVALID_ENUM);
	GLES_ERROR_IF(isPalettedFormat(internalformat), GL_INVALID_OPERATION);
	
	ctx->DGL().glTexImage2D (target, level, internalformat, width, height, border, format, type, pixels);
	if(ctx->GetHostError() == GL_NO_ERROR)
	{
		GLESTexture* texture = ctx->Texture(ctx->TextureBinding());
		GLES_ASSERT(texture != NULL);
		texture->SetLevel(level, internalformat, width, height);

		GLint genMipmap;
		ctx->DGL().glGetTexParameteriv(target, GL_GENERATE_MIPMAP, &genMipmap);
		if(level == 0 && genMipmap)
		{
			GLESTexture* texture = ctx->Texture(ctx->TextureBinding());
			GLES_ASSERT(texture != NULL);
			texture->GenerateMipmap();
		}

		if(texture->Level(level)->boundSurface != NULL)
		{
			// Texture is respecified. Release the bound EGLSurface.
			glesReleaseTexImage(texture->Level(level)->boundSurface, texture->Name(), level);
			texture->Level(level)->boundSurface = NULL;
		}
	}

	GLES_LEAVE();
}
GL_API void GL_APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param)
{
    GLES_ENTER();
    GLES_ERROR_IF(target != GL_TEXTURE_2D, GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidTexParamEnum(pname), GL_INVALID_ENUM);
    ctx->DGL().glTexParameteri (target, pname, param);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glTexParameterx (GLenum target, GLenum pname, GLfixed param)
{
    GLES_ENTER();
    GLES_ERROR_IF(target != GL_TEXTURE_2D, GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidTexParamEnum(pname), GL_INVALID_ENUM);
    ctx->DGL().glTexParameterf (target, pname, static_cast<GLfloat>(param));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glTexParameteriv (GLenum target, GLenum pname, const GLint *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(target != GL_TEXTURE_2D, GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidTexParamEnum(pname), GL_INVALID_ENUM);
    ctx->DGL().glTexParameteriv (target, pname, params);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glTexParameterxv (GLenum target, GLenum pname, const GLfixed *params)
{
    GLES_ENTER();
    GLES_ERROR_IF(target != GL_TEXTURE_2D, GL_INVALID_ENUM);
    GLES_ERROR_IF(!isValidTexParamEnum(pname), GL_INVALID_ENUM);
    GLfloat fparam = static_cast<GLfloat>(params[0]);
    ctx->DGL().glTexParameterfv (target, pname, &fparam);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
	GLES_ENTER();
	GLES_ERROR_IF(!isValidPixelFormatEnum(format), GL_INVALID_ENUM);
	GLES_ERROR_IF(!isValidPixelTypeEnum(type), GL_INVALID_ENUM);
	
	ctx->DGL().glTexSubImage2D (target, level, xoffset, yoffset, width, height, format, type, pixels);

	if(ctx->GetHostError() == GL_NO_ERROR)
	{
		GLint genMipmap;
		ctx->DGL().glGetTexParameteriv(target, GL_GENERATE_MIPMAP, &genMipmap);
		if(level == 0 && genMipmap)
		{
			GLESTexture* texture = ctx->Texture(ctx->TextureBinding());
			GLES_ASSERT(texture != NULL);
			texture->GenerateMipmap();
		}
	}

	GLES_LEAVE();
}
GL_API void GL_APIENTRY glTranslatex (GLfixed x, GLfixed y, GLfixed z)
{
    GLES_ENTER();
    ctx->DGL().glTranslatef (X_TO_F(x), X_TO_F(y), X_TO_F(z));
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    GLES_ENTER();
    GLES_ERROR_IF(size != 2 && size != 3 && size != 4, GL_INVALID_VALUE);
    GLES_ERROR_IF(type != GL_BYTE && type != GL_SHORT && type != GL_FIXED && type != GL_FLOAT, GL_INVALID_ENUM);
    GLES_ERROR_IF(stride < 0, GL_INVALID_VALUE);
    ctx->SetVertexArray(size, type, stride, pointer);
    GLES_LEAVE();
}
GL_API void GL_APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLES_ENTER();
    ctx->DGL().glViewport (x, y, width, height);
    GLES_LEAVE();
}

#ifdef GL_OES_point_size_array
GL_API void GL_APIENTRY glPointSizePointerOES (GLenum type, GLsizei stride, const GLvoid *pointer)
{
    GLES_ENTER();
    GLES_ERROR_IF(type != GL_FIXED && type != GL_FLOAT, GL_INVALID_ENUM);
    GLES_ERROR_IF(stride < 0, GL_INVALID_VALUE);
    ctx->SetPointerSizeArray(type, stride, pointer);
    GLES_LEAVE();
}
#endif

#ifdef __cplusplus
}
#endif
