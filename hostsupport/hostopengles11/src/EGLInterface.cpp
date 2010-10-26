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

#include "EGLInterface.h"
#include <new>
#include "glesInternal.h"
#include "GLESContext.h"
#include "SurfaceDescriptor.h"
#include "GLESTexture.h"

#include <stdlib.h>
#include <string.h>

namespace
{
EGLtoGLESInterface g_EGLtoGLESInterface;
}

GLES_API_CALL IEGLtoGLESInterface* getGLESInterface(void)
{
    return &g_EGLtoGLESInterface;
}

void glesReleaseTexImage(void* surface, int name, int level)
{
	EGLtoGLESInterface::GetEGLInterface()->ReleaseTexImage(surface, name, level);
}

EGLtoGLESInterface::EGLtoGLESInterface() :
    m_egl(NULL)
{
}

void EGLtoGLESInterface::SetEGLInterface( IGLEStoEGLInterface* egl )
{
    m_egl = egl;
}

void* EGLtoGLESInterface::CreateContext(void* nativeContext)
{
    GLESContext* newContext = NULL;

    newContext = GLES_NEW GLESContext(nativeContext);
    if(newContext == NULL)
    {
        return NULL;
    }
    m_contexts.insert(newContext);
    return newContext;
}

bool EGLtoGLESInterface::ReleaseContext(void* context)
{
    GLES_ASSERT(context != NULL);

    GLESContext* ctx = static_cast<GLESContext*>(context);
    if(m_contexts.find(ctx) == m_contexts.end())
    {
        return false;
    }

    delete ctx;
    m_contexts.erase(ctx);
	
    return true;
}

void* EGLtoGLESInterface::GetNativeContext(void* context)
{
    GLES_ASSERT(context != NULL);

    GLESContext* ctx = static_cast<GLESContext*>(context);
    if(m_contexts.find(ctx) == m_contexts.end())
    {
        return false;
    }
	
    return ctx->NativeContext();
}

fpGLProc EGLtoGLESInterface::GetGLProcAddress( const char *procname )
{
	if(strcmp(procname, "glPointSizePointerOES") == 0)
	{
		return (fpGLProc)glPointSizePointerOES;
	}
	else
	{
		return NULL;
	}
}

static SurfaceDescriptor createSurfaceDescriptor(int redBits, int redShift, int greenBits, int greenShift, int blueBits, int blueShift, int alphaBits, int alphaShift, int luminanceBits, int luminanceShift, CColorDescriptor::ColorFormat format, int bpp)
{
	SurfaceDescriptor desc;
	desc.m_colorDescriptor.m_redSize = redBits;
	desc.m_colorDescriptor.m_greenSize = greenBits;
	desc.m_colorDescriptor.m_blueSize = blueBits;
	desc.m_colorDescriptor.m_alphaSize = alphaBits;
	desc.m_colorDescriptor.m_luminanceSize = luminanceBits;
	desc.m_redShift = redShift;
	desc.m_greenShift = greenShift;
	desc.m_blueShift = blueShift;
	desc.m_alphaShift = alphaShift;
	desc.m_luminanceShift = luminanceShift;
	desc.m_colorDescriptor.m_format = format;
	desc.m_colorDescriptor.m_bpp = bpp;
	return desc;
}

typedef struct
{
	SurfaceDescriptor desc;
	GLenum internal_format;
	GLenum data_format;
	GLenum data_type;
} DescToEnumMapping;

static bool isDescEqualToMapping(const SurfaceDescriptor& desc, const DescToEnumMapping& mapping)
{
    if ((desc.m_colorDescriptor.m_redSize == mapping.desc.m_colorDescriptor.m_redSize) &&
		(desc.m_colorDescriptor.m_greenSize == mapping.desc.m_colorDescriptor.m_greenSize) &&
        (desc.m_colorDescriptor.m_blueSize == mapping.desc.m_colorDescriptor.m_blueSize) &&
		(desc.m_colorDescriptor.m_alphaSize == mapping.desc.m_colorDescriptor.m_alphaSize) &&
		(desc.m_colorDescriptor.m_luminanceSize == mapping.desc.m_colorDescriptor.m_luminanceSize) &&
        (desc.m_redShift == mapping.desc.m_redShift) &&
        (desc.m_greenShift == mapping.desc.m_greenShift) &&
        (desc.m_blueShift == mapping.desc.m_blueShift) &&
        (desc.m_alphaShift == mapping.desc.m_alphaShift) &&
        (desc.m_luminanceShift == mapping.desc.m_luminanceShift) &&
        (desc.m_colorDescriptor.m_format == mapping.desc.m_colorDescriptor.m_format) &&
		(desc.m_colorDescriptor.m_bpp == mapping.desc.m_colorDescriptor.m_bpp))
        return true;

    return false;
}

static void surfaceDescriptorToGLEnums(const SurfaceDescriptor& desc, GLenum& internal_format, GLenum& data_format, GLenum& data_type)
{
	static const DescToEnumMapping map[] = {
		/* RGB{A,X} channel ordering */
		 // sRGBX_8888
		{createSurfaceDescriptor(8, 24, 8, 16, 8, 8, 0, 0, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8, GL_RGBA, GL_UNSIGNED_BYTE},
		 // sRGBA_8888
		{createSurfaceDescriptor(8, 24, 8, 16, 8, 8, 8, 0, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE},
         // sRGBA_8888_PRE
        {createSurfaceDescriptor(8, 24, 8, 16, 8, 8, 8, 0, 0, 0, CColorDescriptor::sRGBA_PRE, 32), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE},
         // sRGB_565
        {createSurfaceDescriptor(5, 11, 6, 5, 5, 0, 0, 0, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8, GL_RGB, GL_UNSIGNED_SHORT_5_6_5},
         // sRGBA_5551
        {createSurfaceDescriptor(5, 11, 5, 6, 5, 1, 1, 0, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1},
         // sRGBA_4444
        {createSurfaceDescriptor(4, 12, 4, 8, 4, 4, 4, 0, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4},
         // sL_8
        {createSurfaceDescriptor(0, 0, 0, 0, 0, 0, 0, 0, 8, 0, CColorDescriptor::sLA, 8), GL_SLUMINANCE8, GL_LUMINANCE, GL_UNSIGNED_BYTE},
         // lRGBX_8888
        {createSurfaceDescriptor(8, 24, 8, 16, 8, 8, 0, 0, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGB8, GL_RGBA, GL_UNSIGNED_BYTE},
         // lRGBA_8888
        {createSurfaceDescriptor(8, 24, 8, 16, 8, 8, 8, 0, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE},
         // lRGBA_8888_PRE
        {createSurfaceDescriptor(8, 24, 8, 16, 8, 8, 8, 0, 0, 0, CColorDescriptor::lRGBA_PRE, 32), GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE},
         // lL_8
        {createSurfaceDescriptor(0, 0, 0, 0, 0, 0, 0, 0, 8, 0, CColorDescriptor::lLA, 8), GL_LUMINANCE8, GL_LUMINANCE, GL_UNSIGNED_BYTE},
         // A_8
        {createSurfaceDescriptor(0, 0, 0, 0, 0, 0, 8, 0, 0, 0, CColorDescriptor::lRGBA, 8), GL_ALPHA8, GL_ALPHA, GL_UNSIGNED_BYTE},
		// These should be converted to a compatible format by VG.
		/*
         // BW_1
        {createSurfaceDescriptor(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, CColorDescriptor::lLA, 1), 0, 0, 0},
         // A_1
        {createSurfaceDescriptor(0, 0, 0, 0, 0, 0, 1, 0, 0, 0, CColorDescriptor::lRGBA, 1), 0, 0, 0},
         // A_4
        {createSurfaceDescriptor(0, 0, 0, 0, 0, 0, 4, 0, 0, 0, CColorDescriptor::lRGBA, 4), 0, 0, 0},
		*/

		/* {A,X}RGB channel ordering */
         // sXRGB_8888
        {createSurfaceDescriptor(8, 16, 8, 8, 8, 0, 0, 0, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // sARGB_8888
        {createSurfaceDescriptor(8, 16, 8, 8, 8, 0, 8, 24, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // sARGB_8888_PRE
        {createSurfaceDescriptor(8, 16, 8, 8, 8, 0, 8, 24, 0, 0, CColorDescriptor::sRGBA_PRE, 32), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // sARGB_1555
        {createSurfaceDescriptor(5, 10, 5, 5, 5, 0, 1, 15, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV},
         // sARGB_4444
        {createSurfaceDescriptor(4, 8, 4, 4, 4, 0, 4, 12, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV},
         // lXRGB_8888
        {createSurfaceDescriptor(8, 16, 8, 8, 8, 0, 0, 0, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGB8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // lARGB_8888
        {createSurfaceDescriptor(8, 16, 8, 8, 8, 0, 8, 24, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // lARGB_8888_PRE
        {createSurfaceDescriptor(8, 16, 8, 8, 8, 0, 8, 24, 0, 0, CColorDescriptor::lRGBA_PRE, 32), GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},

		/* BGR{A,X} channel ordering */
         // sBGRX_8888
		{createSurfaceDescriptor(8, 8, 8, 16, 8, 24, 0, 0, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8, GL_BGRA, GL_UNSIGNED_BYTE},
         // sBGRA_8888
        {createSurfaceDescriptor(8, 8, 8, 16, 8, 24, 8, 0, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_BYTE},
         // sBGRA_8888_PRE
        {createSurfaceDescriptor(8, 8, 8, 16, 8, 24, 8, 0, 0, 0, CColorDescriptor::sRGBA_PRE, 32), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_BYTE},
         // sBGR_565
        {createSurfaceDescriptor(5, 0, 6, 5, 5, 11, 0, 0, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8, GL_BGR, GL_UNSIGNED_SHORT_5_6_5},
         // sBGRA_5551
        {createSurfaceDescriptor(5, 1, 5, 6, 5, 11, 1, 0, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_SHORT_5_5_5_1},
         // sBGRA_4444
        {createSurfaceDescriptor(4, 4, 4, 8, 4, 12, 4, 0, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4},
         // lBGRX_8888
        {createSurfaceDescriptor(8, 8, 8, 16, 8, 24, 0, 0, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGB8, GL_BGRA, GL_UNSIGNED_BYTE},
         // lBGRA_8888
        {createSurfaceDescriptor(8, 8, 8, 16, 8, 24, 8, 0, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE},
         // lBGRA_8888_PRE
        {createSurfaceDescriptor(8, 8, 8, 16, 8, 24, 8, 0, 0, 0, CColorDescriptor::lRGBA_PRE, 32), GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE},

		/* {A,X}BGR channel ordering */
         // sXBGR_8888
        {createSurfaceDescriptor(8, 0, 8, 8, 8, 16, 0, 0, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // sABGR_8888
        {createSurfaceDescriptor(8, 0, 8, 8, 8, 16, 8, 24, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // sABGR_8888_PRE
        {createSurfaceDescriptor(8, 0, 8, 8, 8, 16, 8, 24, 0, 0, CColorDescriptor::sRGBA_PRE, 32), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // sABGR_1555
        {createSurfaceDescriptor(5, 0, 5, 5, 5, 10, 1, 15, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV},
         // sABGR_4444
		{createSurfaceDescriptor(4, 0, 4, 4, 4, 8, 4, 12, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4_REV},
         // lXBGR_8888
        {createSurfaceDescriptor(8, 0, 8, 8, 8, 16, 0, 0, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGB8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // lABGR_8888
        {createSurfaceDescriptor(8, 0, 8, 8, 8, 16, 8, 24, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV},
		 // lABGR_8888_PRE:
		{createSurfaceDescriptor(8, 0, 8, 8, 8, 16, 8, 24, 0, 0, CColorDescriptor::lRGBA_PRE, 32), GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV}};

    for (size_t i = 0; i < sizeof(map)/sizeof(map[0]); i++)
    {
        if (isDescEqualToMapping(desc, map[i]))
		{
			internal_format = map[i].internal_format;
			data_format = map[i].data_format;
			data_type = map[i].data_type;
			GLES_ASSERT(internal_format != 0 && data_format != 0 && data_type != 0);
			return;
		}
    }
    GLES_ASSERT(false);
    return;
}

int EGLtoGLESInterface::BindTexImage( void* surface, int level, bool generateMipmap, const SurfaceDescriptor* desc, void* buffer )
{
    GLES_ENTER_RET(0);

	GLuint ret = 0;

    // Store the current error and clear the error flag.
    ctx->GetHostError();

	if(level < 0)
	{
		level = 0;
	}
	else if(level > (int)ctx->MaxTextureLevel())
	{
		level = ctx->MaxTextureLevel();
	}

	GLint origGenMipmapParam;
	ctx->DGL().glGetTexParameteriv(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, &origGenMipmapParam);

	if(!generateMipmap)
	{
		// Disable automatic mipmap generation.
		ctx->DGL().glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	}

	// Clear all mipmap levels.
	for(unsigned int i = 0; i < ctx->MaxTextureLevel(); i++)
	{
		ctx->DGL().glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	GLenum internalFormat, dataFormat, dataType;
	surfaceDescriptorToGLEnums(*desc, internalFormat, dataFormat, dataType);
	ctx->DGL().glTexImage2D(GL_TEXTURE_2D, level, internalFormat, desc->m_width, desc->m_height, 0,
		dataFormat, dataType, buffer);

	if(!generateMipmap)
	{
		// Restore original state.
		ctx->DGL().glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, origGenMipmapParam);
	}

	// Clear any possible error flag.
	if(ctx->DGL().glGetError() == GL_NO_ERROR)
	{
		GLESTexture* texture = ctx->Texture(ctx->TextureBinding());
		GLES_ASSERT(texture != NULL);
		texture->SetLevel(level, GL_RGBA, desc->m_width, desc->m_height);

		if(generateMipmap && origGenMipmapParam && level == 0)
		{
			texture->GenerateMipmap();
		}

		if(texture->Level(level)->boundSurface != NULL)
		{
			glesReleaseTexImage(texture->Level(level)->boundSurface, texture->Name(), level);
		}
		texture->Level(level)->boundSurface = surface;

		ret = texture->Name();
	}

    GLES_LEAVE_NO_ERROR_CHECK_RET(ret);
}

bool EGLtoGLESInterface::ReleaseTexImage( int name, int level )
{
	GLES_ENTER_RET(false);

	ctx->GetHostError();
		
	GLuint binding;
	ctx->DGL().glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&binding);
	ctx->DGL().glBindTexture(GL_TEXTURE_2D, (GLuint)name);
	ctx->DGL().glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	ctx->DGL().glBindTexture(GL_TEXTURE_2D, binding);

	GLES_LEAVE_NO_ERROR_CHECK_RET(ctx->DGL().glGetError() == GL_NO_ERROR);
}

bool EGLtoGLESInterface::CopyBuffers( void* buf, const SurfaceDescriptor* desc )
{
    GLES_ENTER_RET(false);

    // Store the current error and clear the error flag.
    ctx->GetHostError();

	GLenum internal_format, data_format, data_type;
	surfaceDescriptorToGLEnums(*desc, internal_format, data_format, data_type);

	GLint pack_alignment;
	ctx->DGL().glGetIntegerv(GL_PACK_ALIGNMENT, &pack_alignment);
	ctx->DGL().glPixelStorei(GL_PACK_ALIGNMENT, 1);
    ctx->DGL().glReadPixels(0, 0, desc->m_width, desc->m_height, data_format, data_type, buf);
	ctx->DGL().glPixelStorei(GL_PACK_ALIGNMENT, pack_alignment);

    GLES_LEAVE_NO_ERROR_CHECK_RET(ctx->DGL().glGetError() == GL_NO_ERROR);
}

bool EGLtoGLESInterface::UpdateBuffers( void* buf, const SurfaceDescriptor* desc )
{
    GLES_ENTER_RET(false);

    // Store the current error and clear the error flag.
    ctx->GetHostError();

	GLenum internal_format, data_format, data_type;
	surfaceDescriptorToGLEnums(*desc, internal_format, data_format, data_type);

	GLint unpack_alignment;
	ctx->DGL().glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_alignment);
	ctx->DGL().glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    ctx->DGL().glDrawPixels( desc->m_width, desc->m_height, data_format, data_type, buf );
	ctx->DGL().glPixelStorei(GL_UNPACK_ALIGNMENT, unpack_alignment);

    GLES_LEAVE_NO_ERROR_CHECK_RET(ctx->DGL().glGetError() == GL_NO_ERROR);
}

void EGLtoGLESInterface::Flush()
{
    GLES_ENTER();

    // Store the current error and clear the error flag.
    ctx->GetHostError();

    ctx->DGL().glFlush();

    ctx->DGL().glGetError();

    GLES_LEAVE();
}

void EGLtoGLESInterface::Finish()
{
    GLES_ENTER();

    // Store the current error and clear the error flag.
    ctx->GetHostError();

    ctx->DGL().glFinish();

    ctx->DGL().glGetError();

    GLES_LEAVE();
}

/*static*/ IGLEStoEGLInterface* EGLtoGLESInterface::GetEGLInterface()
{
    return g_EGLtoGLESInterface.m_egl;
}
