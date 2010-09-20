/* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *
 */

#include "degl.h"
#include "context.h"
#include "ColorDescriptor.h"
#include "ImageTarget.h"

void* deglGetHostProcAddress(char const* proc)
{
	return EGLtoGLES2Interface::GetEGLInterface()->GetHostProcAddress(proc);
}

DGLContext* deglGetCurrentContext(void)
{
	DGLContext* ctx = static_cast<DGLContext*>(EGLtoGLES2Interface::GetEGLInterface()->GetGLESContext());
	return ctx;
}

int deglLockSurface(int read, int draw)
{
	return EGLtoGLES2Interface::GetEGLInterface()->LockGLESSurface(!!read, !!draw);
}

int deglUnlockSurface(void)
{
	return EGLtoGLES2Interface::GetEGLInterface()->UnlockGLESSurface();
}

static EImageTarget dglGLenumToImageTarget(GLenum target)
{
	switch(target)
	{
		case GL_TEXTURE_2D:
			return IMAGE_TARGET_TEXTURE_2D;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
			return IMAGE_TARGET_TEXTURE_CUBE_MAP_POSITIVE_X;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
			return IMAGE_TARGET_TEXTURE_CUBE_MAP_NEGATIVE_X;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
			return IMAGE_TARGET_TEXTURE_CUBE_MAP_POSITIVE_Y;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
			return IMAGE_TARGET_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
			return IMAGE_TARGET_TEXTURE_CUBE_MAP_POSITIVE_Z;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			return IMAGE_TARGET_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		case GL_RENDERBUFFER:
			return IMAGE_TARGET_RENDERBUFFER;
		default:
			DGLES2_ASSERT(false);
	}

	// Not reached.
	return (EImageTarget)-1;
}

void deglRegisterImageTarget(GLeglImageOES image, GLenum target, GLuint name)
{
	EGLtoGLES2Interface::GetEGLInterface()->RegisterImageTarget(image, dglGLenumToImageTarget(target), (void*)name);
}

void deglUnregisterImageTarget(GLeglImageOES image, GLenum target, GLuint name)
{
	EGLtoGLES2Interface::GetEGLInterface()->UnregisterImageTarget(image, dglGLenumToImageTarget(target), (void*)name);
}

void deglUpdateImageSiblings(GLeglImageOES image, GLenum target, GLuint name)
{
	EGLtoGLES2Interface::GetEGLInterface()->UpdateImageSiblings(image, dglGLenumToImageTarget(target), (void*)name);
}

void deglReleaseTexImage(void* surface, int name, int level)
{
	EGLtoGLES2Interface::GetEGLInterface()->ReleaseTexImage(surface, name, level);
}

static SurfaceDescriptor dglCreateSurfaceDescriptor(int redBits, int redShift, int greenBits, int greenShift, int blueBits, int blueShift, int alphaBits, int alphaShift, int luminanceBits, int luminanceShift, CColorDescriptor::ColorFormat format, int bpp)
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

static bool dglIsDescEqualToMapping(const SurfaceDescriptor& desc, const DescToEnumMapping& mapping)
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

static void dglSurfaceDescriptorToGLEnums(const SurfaceDescriptor& desc, GLenum& internal_format, GLenum& data_format, GLenum& data_type)
{
	static const DescToEnumMapping map[] = {
		/* RGB{A,X} channel ordering */
		 // sRGBX_8888
		{dglCreateSurfaceDescriptor(8, 24, 8, 16, 8, 8, 0, 0, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8, GL_RGBA, GL_UNSIGNED_BYTE},
		 // sRGBA_8888
		{dglCreateSurfaceDescriptor(8, 24, 8, 16, 8, 8, 8, 0, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE},
         // sRGBA_8888_PRE
        {dglCreateSurfaceDescriptor(8, 24, 8, 16, 8, 8, 8, 0, 0, 0, CColorDescriptor::sRGBA_PRE, 32), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE},
         // sRGB_565
        {dglCreateSurfaceDescriptor(5, 11, 6, 5, 5, 0, 0, 0, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8, GL_RGB, GL_UNSIGNED_SHORT_5_6_5},
         // sRGBA_5551
        {dglCreateSurfaceDescriptor(5, 11, 5, 6, 5, 1, 1, 0, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1},
         // sRGBA_4444
        {dglCreateSurfaceDescriptor(4, 12, 4, 8, 4, 4, 4, 0, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4},
         // sL_8
        {dglCreateSurfaceDescriptor(0, 0, 0, 0, 0, 0, 0, 0, 8, 0, CColorDescriptor::sLA, 8), GL_SLUMINANCE8, GL_LUMINANCE, GL_UNSIGNED_BYTE},
         // lRGBX_8888
        {dglCreateSurfaceDescriptor(8, 24, 8, 16, 8, 8, 0, 0, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGB8, GL_RGBA, GL_UNSIGNED_BYTE},
         // lRGBA_8888
        {dglCreateSurfaceDescriptor(8, 24, 8, 16, 8, 8, 8, 0, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE},
         // lRGBA_8888_PRE
        {dglCreateSurfaceDescriptor(8, 24, 8, 16, 8, 8, 8, 0, 0, 0, CColorDescriptor::lRGBA_PRE, 32), GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE},
         // lL_8
        {dglCreateSurfaceDescriptor(0, 0, 0, 0, 0, 0, 0, 0, 8, 0, CColorDescriptor::lLA, 8), GL_LUMINANCE8, GL_LUMINANCE, GL_UNSIGNED_BYTE},
         // A_8
        {dglCreateSurfaceDescriptor(0, 0, 0, 0, 0, 0, 8, 0, 0, 0, CColorDescriptor::lRGBA, 8), GL_ALPHA8, GL_ALPHA, GL_UNSIGNED_BYTE},
		// These should be converted to a compatible format by VG.
		/*
         // BW_1
        {dglCreateSurfaceDescriptor(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, CColorDescriptor::lLA, 1), GL_NONE, GL_NONE, GL_NONE},
         // A_1
        {dglCreateSurfaceDescriptor(0, 0, 0, 0, 0, 0, 1, 0, 0, 0, CColorDescriptor::lRGBA, 1), GL_NONE, GL_NONE, GL_NONE},
         // A_4
        {dglCreateSurfaceDescriptor(0, 0, 0, 0, 0, 0, 4, 0, 0, 0, CColorDescriptor::lRGBA, 4), GL_NONE, GL_NONE, GL_NONE},
		*/

		/* {A,X}RGB channel ordering */
         // sXRGB_8888
        {dglCreateSurfaceDescriptor(8, 16, 8, 8, 8, 0, 0, 0, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // sARGB_8888
        {dglCreateSurfaceDescriptor(8, 16, 8, 8, 8, 0, 8, 24, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // sARGB_8888_PRE
        {dglCreateSurfaceDescriptor(8, 16, 8, 8, 8, 0, 8, 24, 0, 0, CColorDescriptor::sRGBA_PRE, 32), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // sARGB_1555
        {dglCreateSurfaceDescriptor(5, 10, 5, 5, 5, 0, 1, 15, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV},
         // sARGB_4444
        {dglCreateSurfaceDescriptor(4, 8, 4, 4, 4, 0, 4, 12, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV},
         // lXRGB_8888
        {dglCreateSurfaceDescriptor(8, 16, 8, 8, 8, 0, 0, 0, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGB8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // lARGB_8888
        {dglCreateSurfaceDescriptor(8, 16, 8, 8, 8, 0, 8, 24, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // lARGB_8888_PRE
        {dglCreateSurfaceDescriptor(8, 16, 8, 8, 8, 0, 8, 24, 0, 0, CColorDescriptor::lRGBA_PRE, 32), GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},

		/* BGR{A,X} channel ordering */
         // sBGRX_8888
		{dglCreateSurfaceDescriptor(8, 8, 8, 16, 8, 24, 0, 0, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8, GL_BGRA, GL_UNSIGNED_BYTE},
         // sBGRA_8888
        {dglCreateSurfaceDescriptor(8, 8, 8, 16, 8, 24, 8, 0, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_BYTE},
         // sBGRA_8888_PRE
        {dglCreateSurfaceDescriptor(8, 8, 8, 16, 8, 24, 8, 0, 0, 0, CColorDescriptor::sRGBA_PRE, 32), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_BYTE},
         // sBGR_565
        {dglCreateSurfaceDescriptor(5, 0, 6, 5, 5, 11, 0, 0, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8, GL_BGR, GL_UNSIGNED_SHORT_5_6_5},
         // sBGRA_5551
        {dglCreateSurfaceDescriptor(5, 1, 5, 6, 5, 11, 1, 0, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_SHORT_5_5_5_1},
         // sBGRA_4444
        {dglCreateSurfaceDescriptor(4, 4, 4, 8, 4, 12, 4, 0, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4},
         // lBGRX_8888
        {dglCreateSurfaceDescriptor(8, 8, 8, 16, 8, 24, 0, 0, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGB8, GL_BGRA, GL_UNSIGNED_BYTE},
         // lBGRA_8888
        {dglCreateSurfaceDescriptor(8, 8, 8, 16, 8, 24, 8, 0, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE},
         // lBGRA_8888_PRE
        {dglCreateSurfaceDescriptor(8, 8, 8, 16, 8, 24, 8, 0, 0, 0, CColorDescriptor::lRGBA_PRE, 32), GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE},

		/* {A,X}BGR channel ordering */
         // sXBGR_8888
        {dglCreateSurfaceDescriptor(8, 0, 8, 8, 8, 16, 0, 0, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // sABGR_8888
        {dglCreateSurfaceDescriptor(8, 0, 8, 8, 8, 16, 8, 24, 0, 0, CColorDescriptor::sRGBA, 32), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // sABGR_8888_PRE
        {dglCreateSurfaceDescriptor(8, 0, 8, 8, 8, 16, 8, 24, 0, 0, CColorDescriptor::sRGBA_PRE, 32), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // sABGR_1555
        {dglCreateSurfaceDescriptor(5, 0, 5, 5, 5, 10, 1, 15, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV},
         // sABGR_4444
		{dglCreateSurfaceDescriptor(4, 0, 4, 4, 4, 8, 4, 12, 0, 0, CColorDescriptor::sRGBA, 16), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4_REV},
         // lXBGR_8888
        {dglCreateSurfaceDescriptor(8, 0, 8, 8, 8, 16, 0, 0, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGB8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV},
         // lABGR_8888
        {dglCreateSurfaceDescriptor(8, 0, 8, 8, 8, 16, 8, 24, 0, 0, CColorDescriptor::lRGBA, 32), GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV},
		 // lABGR_8888_PRE:
		{dglCreateSurfaceDescriptor(8, 0, 8, 8, 8, 16, 8, 24, 0, 0, CColorDescriptor::lRGBA_PRE, 32), GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV}};

    for (size_t i = 0; i < sizeof(map)/sizeof(map[0]); i++)
    {
        if (dglIsDescEqualToMapping(desc, map[i]))
		{
			internal_format = map[i].internal_format;
			data_format = map[i].data_format;
			data_type = map[i].data_type;
			DGLES2_ASSERT(internal_format != GL_NONE && data_format != GL_NONE && data_type != GL_NONE);
			return;
		}
    }
    DGLES2_ASSERT(GL_FALSE);
    return;
}

void deglGetImageInfo(GLeglImageOES image, GLenum* internal_format, GLsizei* width, GLsizei* height, GLsizei* stride, GLenum* data_format, GLenum* data_type)
{
	DGLES2_ASSERT(image != NULL);
	DGLES2_ASSERT(internal_format != NULL);
	DGLES2_ASSERT(width != NULL);
	DGLES2_ASSERT(height != NULL);
	DGLES2_ASSERT(stride != NULL);
	DGLES2_ASSERT(data_format != NULL);
	DGLES2_ASSERT(data_type != NULL);

	SurfaceDescriptor desc;
	EGLtoGLES2Interface::GetEGLInterface()->GetDescForImage(image, desc);

	*width = desc.m_width;
	*height = desc.m_height;
	*stride = desc.m_stride;
	dglSurfaceDescriptorToGLEnums(desc, *internal_format, *data_format, *data_type);
}

void* deglGetImageData(GLeglImageOES image)
{
	DGLES2_ASSERT(image != NULL);
	return EGLtoGLES2Interface::GetEGLInterface()->GetDataForImage(image);
}

namespace
{
EGLtoGLES2Interface g_EGLtoGLES2Interface;
}

IEGLtoGLES2Interface* getGLES2Interface(void)
{
	return &g_EGLtoGLES2Interface;
}

#include <new>

EGLtoGLES2Interface::EGLtoGLES2Interface() :
    m_egl(NULL)
{
}

void EGLtoGLES2Interface::SetEGLInterface( IGLEStoEGLInterface* egl )
{
    DGLES2_ASSERT(!m_egl);
    m_egl = egl;
}

void* EGLtoGLES2Interface::CreateContext(void* nativeContext)
{
	DGLContext* newContext = NULL;

	newContext = DGLContext_create(nativeContext);
	if(newContext == NULL)
	{
		return NULL;
    }
	m_contexts.insert(newContext);
    return newContext;
}

bool EGLtoGLES2Interface::ReleaseContext(void* context)
{
	DGLES2_ASSERT(context != NULL);
	DGLContext* ctx = static_cast<DGLContext*>(context);
	if(m_contexts.find(ctx) == m_contexts.end())
	{
		return false;
	}

	DGLContext_destroy(ctx);
	m_contexts.erase(ctx);
	
	return true;
}

void* EGLtoGLES2Interface::GetNativeContext(void* context)
{
	DGLES2_ASSERT(context != NULL);
	DGLContext* ctx = static_cast<DGLContext*>(context);
	if(m_contexts.find(ctx) == m_contexts.end())
	{
		return false;
	}
	
	return ctx->native_context;
}

extern "C"
{
GL_APICALL void GL_APIENTRY glEGLImageTargetTexture2DOES (GLenum target, GLeglImageOES image);
GL_APICALL void GL_APIENTRY glEGLImageTargetRenderbufferStorageOES (GLenum target, GLeglImageOES image);
}

fpGLProc EGLtoGLES2Interface::GetGLProcAddress(const char *procname)
{
	if(strcmp(procname, "glEGLImageTargetTexture2DOES") == 0)
	{
		return (fpGLProc)glEGLImageTargetTexture2DOES;
	}
	else if(strcmp(procname, "glEGLImageTargetRenderbufferStorageOES") == 0)
	{
		return (fpGLProc)glEGLImageTargetRenderbufferStorageOES;
	}
	else
	{
		return NULL;
	}
}

int EGLtoGLES2Interface::BindTexImage( void* surface, int level, bool generate_mipmap, const SurfaceDescriptor* desc, void* buffer )
{
	DGLES2_ENTER_RET(NULL);

	GLuint ret = 0;

	// Store the current error and clear the error flag.
	DGLContext_getHostError(ctx);

	if(level < 0)
	{
		level = 0;
	}
	else if(level > ctx->max_texture_level)
	{
		level = ctx->max_texture_level;
	}

	// Clear all mipmap levels.
	{
		int i;
		for(i = 0; i < ctx->max_texture_level; i++)
		{
			ctx->hgl.TexImage2D(GL_TEXTURE_2D, level, GL_RGBA, 0, 0, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}
	}

	{
		GLenum internal_format, data_format, data_type;
		dglSurfaceDescriptorToGLEnums(*desc, internal_format, data_format, data_type);
		ctx->hgl.TexImage2D(GL_TEXTURE_2D, level, internal_format, desc->m_width, desc->m_height, 0,
			data_format, data_type, buffer);
	}

	if(generate_mipmap && level == 0)
	{
		ctx->hgl.GenerateMipmapEXT(GL_TEXTURE_2D);
	}

	// Clear any possible error flag.
	if(ctx->hgl.GetError() == GL_NO_ERROR)
	{
		DGLTexture* texture;
		GLeglImageOES image;
		
		texture = DGLContext_findTexture(ctx, ctx->texture_binding_2d);
		DGLES2_ASSERT(texture != NULL);
		DGLTexture_setLevel(texture, GL_TEXTURE_2D, level, GL_RGBA, desc->m_width, desc->m_height);

		if(generate_mipmap && level == 0)
		{
			DGLTexture_generateMipmap(texture);
		}

		image = DGLTexture_getEGLImage(texture, GL_TEXTURE_2D);
		if(image != NULL)
		{
			// Texture is respecified. It is no longer an EGLImage sibling.
			deglUnregisterImageTarget(image, GL_TEXTURE_2D, texture->obj.name);
			DGLTexture_setEGLImage(texture, GL_TEXTURE_2D, NULL);
		}

		{
			DGLTextureLevel* level_obj = DGLTexture_getLevel(texture, GL_TEXTURE_2D, level);
			if(level_obj->bound_surface != NULL)
			{
				// Texture is respecified. Release the bound EGLSurface.
				deglReleaseTexImage(level_obj->bound_surface, texture->obj.name, level);
			}
			level_obj->bound_surface = surface;
		}

		ret = texture->obj.name;
	}

	DGLES2_LEAVE_NO_ERROR_CHECK_RET(ret);
}

bool EGLtoGLES2Interface::ReleaseTexImage( int name, int level )
{
	DGLES2_ENTER_RET(false);

	DGLContext_getHostError(ctx);
		
	GLuint binding;
	ctx->hgl.GetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&binding);
	ctx->hgl.BindTexture(GL_TEXTURE_2D, (GLuint)name);
	ctx->hgl.TexImage2D(GL_TEXTURE_2D, level, GL_RGBA, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	ctx->hgl.BindTexture(GL_TEXTURE_2D, binding);

	bool ret = false;

	if(ctx->hgl.GetError() == GL_NO_ERROR)
	{
		DGLTexture* texture;
		DGLTextureLevel* level_obj;

		texture = DGLContext_findTexture(ctx, name);
		level_obj = DGLTexture_getLevel(texture, GL_TEXTURE_2D, level);
		level_obj->bound_surface = NULL;

		ret = true;
	}

	DGLES2_LEAVE_NO_ERROR_CHECK_RET(ret);
}

static GLenum dglImageTargetToGLenum(EImageTarget target)
{
	switch(target)
	{
		case IMAGE_TARGET_TEXTURE_2D:
			return GL_TEXTURE_2D;
		case IMAGE_TARGET_TEXTURE_CUBE_MAP_POSITIVE_X:
			return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		case IMAGE_TARGET_TEXTURE_CUBE_MAP_NEGATIVE_X:
			return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
		case IMAGE_TARGET_TEXTURE_CUBE_MAP_POSITIVE_Y:
			return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
		case IMAGE_TARGET_TEXTURE_CUBE_MAP_NEGATIVE_Y:
			return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		case IMAGE_TARGET_TEXTURE_CUBE_MAP_POSITIVE_Z:
			return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		case IMAGE_TARGET_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		default:
			DGLES2_ASSERT(false);
	}

	// Not reached.
	return -1;
}

static bool dglPrepareState(DGLContext& ctx, GLuint name, EImageTarget target, GLint level, GLenum& query_target, GLenum& bind_target, GLint& binding)
{
	if(name == 0)
	{
		return false;
	}
	
	// Store the current error and clear the error flag.
	DGLContext_getHostError(&ctx);

	query_target = dglImageTargetToGLenum(target);
	switch(query_target)
	{
		case GL_TEXTURE_2D:
			bind_target = GL_TEXTURE_2D;
			ctx.hgl.GetIntegerv(GL_TEXTURE_BINDING_2D, &binding);
			break;

		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			bind_target = GL_TEXTURE_CUBE_MAP;
			ctx.hgl.GetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &binding);
			break;
	}

	const DGLTexture* texture = DGLContext_findTexture(&ctx, name);
	if(texture == NULL)
	{
		return false;
	}

	if(!DGLTexture_isComplete(texture))
	{
		if(level != 0)
		{
			return false;
		}
		else
		{
			if(!DGLTexture_hasLevelZero(texture))
			{
				return false;
			}

			if(DGLTexture_hasLevelsOtherThanZero(texture))
			{
				return false;
			}
		}
	}

	ctx.hgl.BindTexture(bind_target, name);
	// Clear any possible error flag.
	if(ctx.hgl.GetError() == GL_INVALID_OPERATION)
	{
		// Texture was not created with the requested target.
		return false;
	}

	return true;
}

static void dglResetState(DGLContext& ctx, GLenum target, GLint binding)
{
	// Reset state.
	ctx.hgl.BindTexture(target, binding);

	DGLES2_ASSERT(ctx.hgl.GetError() == GL_NO_ERROR);
}

EStatus EGLtoGLES2Interface::GetTextureInfo(void* context, EImageTarget target, void* texture, GLint texture_level, SurfaceDescriptor& surfDesc)
{
	DGLES2_ASSERT(context != NULL);

	DGLContext* ctx = (DGLContext*)context;
	GLuint name = (GLuint)texture;

	dglGetLock();

	GLenum query_target;
	GLenum bind_target;
	GLint binding;
	if(!dglPrepareState(*ctx, name, target, texture_level, query_target, bind_target, binding))
	{
		dglReleaseLock();
		return DGLES2_BAD_PARAMETER;
	}

	CColorDescriptor& colDesc = surfDesc.m_colorDescriptor;
	GLenum format;
	ctx->hgl.GetTexLevelParameteriv(query_target, texture_level, GL_TEXTURE_WIDTH, &surfDesc.m_width);
	ctx->hgl.GetTexLevelParameteriv(query_target, texture_level, GL_TEXTURE_HEIGHT, &surfDesc.m_height);
	ctx->hgl.GetTexLevelParameteriv(query_target, texture_level, GL_TEXTURE_RED_SIZE, &colDesc.m_redSize);
	ctx->hgl.GetTexLevelParameteriv(query_target, texture_level, GL_TEXTURE_GREEN_SIZE, &colDesc.m_greenSize);
	ctx->hgl.GetTexLevelParameteriv(query_target, texture_level, GL_TEXTURE_BLUE_SIZE, &colDesc.m_blueSize);
	ctx->hgl.GetTexLevelParameteriv(query_target, texture_level, GL_TEXTURE_ALPHA_SIZE, &colDesc.m_alphaSize);
	ctx->hgl.GetTexLevelParameteriv(query_target, texture_level, GL_TEXTURE_LUMINANCE_SIZE, &colDesc.m_luminanceSize);
	ctx->hgl.GetTexLevelParameteriv(query_target, texture_level, GL_TEXTURE_INTERNAL_FORMAT, (GLint*)&format);
	if(format == GL_RGB || format == GL_RGB8)
	{
		// Convert RGB888 to RGBA8888 to make VG interop easier.
		format = GL_RGBA;
	}
	if(ctx->hgl.GetError() == GL_INVALID_VALUE)
	{
		// Invalid level.
		dglReleaseLock();
		return DGLES2_BAD_MATCH;
	}

	surfDesc.m_redShift = 0;
	surfDesc.m_greenShift = 0;
	surfDesc.m_blueShift = 0;
	surfDesc.m_luminanceShift = 0;
	surfDesc.m_alphaShift = 0;
	int bpp = 0;
	switch(format)
	{
#ifndef DGLES2_NO_SRGB
		case GL_SRGB8:
			surfDesc.m_redShift = 16;
			surfDesc.m_greenShift = 8;
			surfDesc.m_blueShift = 0;
			colDesc.m_format = CColorDescriptor::sRGBA;
			colDesc.m_bpp = 24;
			break;

		case GL_SRGB8_ALPHA8:
			surfDesc.m_redShift = 24;
			surfDesc.m_greenShift = 16;
			surfDesc.m_blueShift = 8;
			surfDesc.m_alphaShift = 0;
			colDesc.m_format = CColorDescriptor::sRGBA;
			colDesc.m_bpp = 32;
			break;

		case GL_SLUMINANCE8:
			surfDesc.m_luminanceShift = 0;
			colDesc.m_format = CColorDescriptor::sLA;
			colDesc.m_bpp = 8;
			break;
#endif

		case GL_RGB:
		case GL_RGB8:
			surfDesc.m_redShift = 16;
			surfDesc.m_greenShift = 8;
			surfDesc.m_blueShift = 0;
			colDesc.m_format = CColorDescriptor::lRGBA;
			colDesc.m_bpp = 24;
			break;

		case GL_RGBA:
		case GL_RGBA8:
			surfDesc.m_redShift = 24;
			surfDesc.m_greenShift = 16;
			surfDesc.m_blueShift = 8;
			surfDesc.m_alphaShift = 0;
			colDesc.m_format = CColorDescriptor::lRGBA;
			colDesc.m_bpp = 32;
			break;

		case GL_LUMINANCE_ALPHA:
			surfDesc.m_luminanceShift = 8;
			surfDesc.m_alphaShift = 0;
			colDesc.m_format = CColorDescriptor::lLA;
			colDesc.m_bpp = 16;
			break;

		case GL_ALPHA:
		case GL_ALPHA8:
			surfDesc.m_alphaShift = 0;
			colDesc.m_format = CColorDescriptor::lRGBA;
			colDesc.m_bpp = 8;
			break;

		case GL_LUMINANCE:
		case GL_LUMINANCE8:
			surfDesc.m_luminanceShift = 0;
			colDesc.m_format = CColorDescriptor::lLA;
			colDesc.m_bpp = 8;
			break;
			
		default:
			DGLES2_ASSERT(GL_FALSE);
	}

	surfDesc.m_stride = colDesc.m_bpp / 8 * surfDesc.m_width;

	dglResetState(*ctx, bind_target, binding);
	dglReleaseLock();
	return DGLES2_SUCCESS;
}

EStatus EGLtoGLES2Interface::GetTextureData( void* context, EImageTarget target, void* texture, EGLint texture_level, void* data )
{
	DGLES2_ASSERT(context != NULL);

	DGLContext* ctx = (DGLContext*)context;
	GLuint name = (GLuint)texture;

	dglGetLock();

	GLenum query_target;
	GLenum bind_target;
	GLint binding;
	if(!dglPrepareState(*ctx, name, target, texture_level, query_target, bind_target, binding))
	{
		dglReleaseLock();
		return DGLES2_BAD_PARAMETER;
	}

	GLenum format;
	GLint pack_alignment;
	ctx->hgl.GetTexLevelParameteriv(query_target, texture_level, GL_TEXTURE_INTERNAL_FORMAT, (GLint*)&format);
	if(format == GL_RGB || format == GL_RGB8)
	{
		// Convert RGB888 to RGBA8888 to make VG interop easier.
		format = GL_RGBA;
	}
	ctx->hgl.GetIntegerv(GL_PACK_ALIGNMENT, &pack_alignment);
	ctx->hgl.PixelStorei(GL_PACK_ALIGNMENT, 1);
	ctx->hgl.GetTexImage(query_target, texture_level, format, GL_UNSIGNED_BYTE, data);
	ctx->hgl.PixelStorei(GL_PACK_ALIGNMENT, pack_alignment);

	if(ctx->hgl.GetError() == GL_INVALID_VALUE)
	{
		// Invalid level.
		dglReleaseLock();
		return DGLES2_BAD_MATCH;
	}

	dglResetState(*ctx, bind_target, binding);
	dglReleaseLock();
	return DGLES2_SUCCESS;
}

bool EGLtoGLES2Interface::CopyBuffers( void* buf, const SurfaceDescriptor* desc )
{
	DGLES2_ENTER_RET(false);

	// Store the current error and clear the error flag.
	DGLContext_getHostError(ctx);

	{
		GLint framebuffer_binding;
		GLint pack_alignment;
		GLenum internal_format, data_format, data_type;

		ctx->hgl.GetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer_binding);
		ctx->hgl.GetIntegerv(GL_PACK_ALIGNMENT, &pack_alignment);

		dglSurfaceDescriptorToGLEnums(*desc, internal_format, data_format, data_type);

		ctx->hgl.BindFramebufferEXT(GL_FRAMEBUFFER, 0);
		ctx->hgl.PixelStorei(GL_PACK_ALIGNMENT, 1);
		ctx->hgl.ReadPixels(0, 0, desc->m_width, desc->m_height, data_format, data_type, buf);

		// Restore state.
		ctx->hgl.PixelStorei(GL_PACK_ALIGNMENT, pack_alignment);
		ctx->hgl.BindFramebufferEXT(GL_FRAMEBUFFER, framebuffer_binding);
	}

	DGLES2_LEAVE_NO_ERROR_CHECK_RET(ctx->hgl.GetError() == GL_NO_ERROR);
}

bool EGLtoGLES2Interface::UpdateBuffers( void* buf, const SurfaceDescriptor* desc )
{
	DGLES2_ENTER_RET(false);

	// Store the current error and clear the error flag.
	DGLContext_getHostError(ctx);

	{
		GLint framebuffer_binding;
		GLint unpack_alignment;
		GLenum internal_format, data_format, data_type;

		ctx->hgl.GetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer_binding);
		ctx->hgl.GetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_alignment);

		dglSurfaceDescriptorToGLEnums(*desc, internal_format, data_format, data_type);

		ctx->hgl.BindFramebufferEXT(GL_FRAMEBUFFER, 0);
		ctx->hgl.PixelStorei(GL_UNPACK_ALIGNMENT, 1);
		ctx->hgl.DrawPixels( desc->m_width, desc->m_height, data_format, data_type, buf );
		
		// Restore state.
		ctx->hgl.PixelStorei(GL_UNPACK_ALIGNMENT, unpack_alignment);
		ctx->hgl.BindFramebufferEXT(GL_FRAMEBUFFER, framebuffer_binding);
	}

	DGLES2_LEAVE_NO_ERROR_CHECK_RET(ctx->hgl.GetError() == GL_NO_ERROR);
}

bool EGLtoGLES2Interface::UpdateImageSibling( EImageTarget imageTarget, void* name )
{
	DGLES2_ENTER_RET(false);
	DGLContext_getHostError(ctx);
	{
		bool success = true;
		bool cubeMap = false;

		switch(imageTarget)
		{
		case IMAGE_TARGET_TEXTURE_CUBE_MAP_POSITIVE_X:
		case IMAGE_TARGET_TEXTURE_CUBE_MAP_NEGATIVE_X:
		case IMAGE_TARGET_TEXTURE_CUBE_MAP_POSITIVE_Y:
		case IMAGE_TARGET_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		case IMAGE_TARGET_TEXTURE_CUBE_MAP_POSITIVE_Z:
		case IMAGE_TARGET_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			cubeMap = true;
			// fall through

		case IMAGE_TARGET_TEXTURE_2D:
			{
				DGLTexture* texture;
				GLenum textureTarget;
				GLeglImageOES image;
				GLint binding;

				texture = DGLContext_findTexture(ctx, (GLuint)name);
				if(texture == NULL)
				{
					success = false;
					break;
				}

				textureTarget = dglImageTargetToGLenum(imageTarget);
				image = DGLTexture_getEGLImage(texture, textureTarget);
				if(image == NULL)
				{
					success = false;
					break;
				}

				ctx->hgl.GetIntegerv(cubeMap ? GL_TEXTURE_BINDING_CUBE_MAP : GL_TEXTURE_BINDING_2D, &binding);
				ctx->hgl.BindTexture(cubeMap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, texture->obj.name);
				success = !!DGLContext_specifyTextureFromEGLImage(ctx, image, textureTarget);

				// Reset state.
				ctx->hgl.BindTexture(cubeMap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, binding);

				if(!success)
				{
					success = false;
					break;
				}

				break;
			}

		case IMAGE_TARGET_RENDERBUFFER:
			{
				DGLRenderbuffer* buffer;
				GLint binding;

				buffer = DGLContext_findRenderbuffer(ctx, (GLuint)name);
				if(buffer == NULL)
				{
					success = false;
					break;
				}

				if(buffer->egl_image == NULL)
				{
					success = false;
					break;
				}
				
				ctx->hgl.GetIntegerv(GL_RENDERBUFFER_BINDING, &binding);
				ctx->hgl.BindRenderbufferEXT(GL_RENDERBUFFER, buffer->obj.name);
				success = !!DGLContext_specifyRenderbufferFromEGLImage(ctx, buffer->egl_image);

				// Reset state.
				ctx->hgl.BindRenderbufferEXT(GL_RENDERBUFFER, binding);

				if(!success)
				{
					success = false;
					break;
				}

				break;
			}

		default:
			DGLES2_ASSERT(false);
			success = false;
			break;
		}
	
		if(ctx->hgl.GetError() != GL_NO_ERROR)
		{
			success = false;
		}
		
		DGLES2_LEAVE_NO_ERROR_CHECK_RET(success);
	}
}

void EGLtoGLES2Interface::Flush()
{
	DGLES2_ENTER();
	ctx->hgl.Flush();
	DGLES2_LEAVE_NO_ERROR_CHECK();
}

void EGLtoGLES2Interface::Finish()
{
	DGLES2_ENTER();
	ctx->hgl.Finish();
	DGLES2_LEAVE_NO_ERROR_CHECK();
}

/*static*/ IGLEStoEGLInterface* EGLtoGLES2Interface::GetEGLInterface()
{
    return g_EGLtoGLES2Interface.m_egl;
}
