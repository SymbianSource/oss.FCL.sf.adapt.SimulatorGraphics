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

#include "common.h"
#include "hgl.h"
#include "context.h"
#include "half.h"
#include "util.h"
#include "degl.h"

#ifdef __cplusplus
extern "C"
{
GL_APICALL void GL_APIENTRY glEGLImageTargetTexture2DOES (GLenum target, GLeglImageOES image);
}
#endif

DGLTexture* DGLTexture_create(GLuint name, DGLTextureType type, GLint num_levels)
{
	DGLTexture* texture = (DGLTexture*)malloc(sizeof(DGLTexture));
	if(texture == NULL)
	{
		return NULL;
	}

	texture->obj.name = name;
	texture->obj.next = NULL;

	texture->type = type;

	{
		int face;
		for(face = 0; face < 6; face++)
		{
			texture->num_levels[face] = 0;
			texture->levels[face] = (DGLTextureLevel*)malloc(num_levels * sizeof(DGLTextureLevel));
			if(texture->levels[face] == NULL)
			{
				while(face--)
				{
					free(texture->levels[face]);
				}
				free(texture);
				return NULL;
			}
			{
				int level;
				for(level = 0; level < num_levels; level++)
				{
					texture->levels[face][level].specified = GL_FALSE;
					texture->levels[face][level].format = 0;
					texture->levels[face][level].width = 0;
					texture->levels[face][level].height = 0;
					texture->levels[face][level].bound_surface = NULL;
				}
			}
			texture->egl_image[face] = NULL;
		}
	}

	return texture;
}

static GLenum dglFaceToTarget(DGLTexture* texture, int face)
{
	DGLES2_ASSERT(texture != NULL);
	{
		switch(face)
		{
			case 0:
				if(texture->type == DGLES2_TEXTURE_2D)
				{
					return GL_TEXTURE_2D;
				}
				else
				{
					DGLES2_ASSERT(texture->type == DGLES2_TEXTURE_CUBE_MAP);
					return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
				}

			case 1:
				DGLES2_ASSERT(texture->type == DGLES2_TEXTURE_CUBE_MAP);
				return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;

			case 2:
				DGLES2_ASSERT(texture->type == DGLES2_TEXTURE_CUBE_MAP);
				return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;

			case 3:
				DGLES2_ASSERT(texture->type == DGLES2_TEXTURE_CUBE_MAP);
				return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;

			case 4:
				DGLES2_ASSERT(texture->type == DGLES2_TEXTURE_CUBE_MAP);
				return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;

			case 5:
				DGLES2_ASSERT(texture->type == DGLES2_TEXTURE_CUBE_MAP);
				return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;

			default:
				DGLES2_ASSERT(GL_FALSE);
				return (GLenum)-1;
		}
	}
}

void DGLTexture_destroy(DGLTexture *texture)
{
	DGLES2_ASSERT(texture != NULL);
	{
		int face;
		for(face = 0; face < 6; face++)
		{
			DGLES2_ASSERT(texture->levels[face] != NULL);
			free(texture->levels[face]);
			texture->levels[face] = NULL;

			if(texture->egl_image[face] != NULL)
			{
				deglUnregisterImageTarget(texture->egl_image[face], dglFaceToTarget(texture, face), texture->obj.name);
				texture->egl_image[face] = NULL;
			}
		}
	}
	free(texture);
}

GLboolean DGLTexture_isComplete(const DGLTexture* texture)
{
	DGLES2_ASSERT(texture != NULL);
	{
		int num_faces = 6 ? texture->type == DGLES2_TEXTURE_CUBE_MAP : 1;
		int face;
		for(face = 0; face < num_faces; face++)
		{
			if(texture->num_levels[face] < 1)
			{
				return GL_FALSE;
			}
			else
			{
				int i;
				const DGLTextureLevel* level_zero;
				int width;
				int height;

				level_zero = &texture->levels[face][0];
				width = level_zero->width;
				height = level_zero->height;

				if(width <= 0 || height <= 0)
				{
					return GL_FALSE;
				}

				for(i = 1; i < texture->num_levels[face]; i++)
				{
					const DGLTextureLevel* level = &texture->levels[face][i];

					if(width > 1) width /= 2;
					if(height > 1) height /= 2;

					if(level->format != level_zero->format ||
					   level->width != width ||
					   level->height != height ||
					   level->width == 0 ||
					   level->height == 0)
					{
						return GL_FALSE;
					}
				}
			}
		}

		return GL_TRUE;
	}
}

GLboolean DGLTexture_hasLevelZero(const DGLTexture* texture)
{
	DGLES2_ASSERT(texture != NULL);
	{
		int num_faces = 6 ? texture->type == DGLES2_TEXTURE_CUBE_MAP : 1;
		int face;
		for(face = 0; face < num_faces; face++)
		{
			if(texture->num_levels[face] <= 0 || !texture->levels[face][0].specified)
			{
				return GL_FALSE;
			}
		}

		return GL_TRUE;
	}
}

GLboolean DGLTexture_hasLevelsOtherThanZero(const DGLTexture* texture)
{
	DGLES2_ASSERT(texture != NULL);
	{
		int num_faces = 6 ? texture->type == DGLES2_TEXTURE_CUBE_MAP : 1;
		int face;
		for(face = 0; face < num_faces; face++)
		{
			int level;
			for(level = 1; level < texture->num_levels[face]; level++)
			{
				if(texture->levels[face][level].specified)
				{
					return GL_TRUE;
				}
			}
		}

		return GL_FALSE;
	}
}

static int dglTargetToFace(DGLTexture* texture, GLenum target)
{
	DGLES2_ASSERT(texture != NULL);
	{
		switch(target)
		{
			case GL_TEXTURE_2D:
				DGLES2_ASSERT(texture->type == DGLES2_TEXTURE_2D);
				return 0;

			case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
				DGLES2_ASSERT(texture->type == DGLES2_TEXTURE_CUBE_MAP);
				return 0;

			case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
				DGLES2_ASSERT(texture->type == DGLES2_TEXTURE_CUBE_MAP);
				return 1;

			case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
				DGLES2_ASSERT(texture->type == DGLES2_TEXTURE_CUBE_MAP);
				return 2;

			case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
				DGLES2_ASSERT(texture->type == DGLES2_TEXTURE_CUBE_MAP);
				return 3;

			case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
				DGLES2_ASSERT(texture->type == DGLES2_TEXTURE_CUBE_MAP);
				return 4;

			case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
				DGLES2_ASSERT(texture->type == DGLES2_TEXTURE_CUBE_MAP);
				return 5;

			default:
				DGLES2_ASSERT(GL_FALSE);
				return -1;
		}
	}
}

DGLTextureLevel* DGLTexture_getLevel(DGLTexture* texture, GLenum target, GLint level)
{	
	DGLES2_ASSERT(texture != NULL);
	return &texture->levels[dglTargetToFace(texture, target)][level];
}

void DGLTexture_setLevel(DGLTexture* texture, GLenum target, GLint level, GLenum format, GLsizei width, GLsizei height)
{
	DGLES2_ASSERT(texture != NULL);
	{
		DGLTextureLevel* level_obj = DGLTexture_getLevel(texture, target, level);
		level_obj->format = format;
		level_obj->width = width;
		level_obj->height = height;
		level_obj->specified = GL_TRUE;
	}
}

GLeglImageOES DGLTexture_getEGLImage(DGLTexture* texture, GLenum target)
{
	return texture->egl_image[dglTargetToFace(texture, target)];
}

void DGLTexture_setEGLImage(DGLTexture* texture, GLenum target, GLeglImageOES image)
{
	texture->egl_image[dglTargetToFace(texture, target)] = image;
}

void DGLTexture_generateMipmap(DGLTexture* texture)
{
	DGLES2_ASSERT(texture != NULL);
	{
		int face;
		int num_faces;
		const DGLTextureLevel* level_zero;
		int level;
		int num_levels;
		int width, height;

		num_faces = 6 ? texture->type == DGLES2_TEXTURE_CUBE_MAP : 1;
		for(face = 0; face < num_faces; face++)
		{
			level_zero = &texture->levels[face][0];

			num_levels = dglLog2(dglMax(level_zero->width, level_zero->height)) + 1;
			texture->num_levels[face] = num_levels;

			width = level_zero->width;
			height = level_zero->height;

			for(level = 0; level < num_levels; level++)
			{
				if(width > 1) width /= 2;
				if(height > 1) height /= 2;

				DGLES2_ASSERT(level < num_levels - 1 || (width > 1 || height > 1));

				if(texture->levels[face][level].bound_surface != NULL)
				{
					// Texture image is respecified. Release the bound EGLSurface.
					deglReleaseTexImage(texture->levels[face][level].bound_surface, texture->obj.name, level);
				}

				texture->levels[face][level].format = level_zero->format;
				texture->levels[face][level].width = width;
				texture->levels[face][level].height = height;
				texture->levels[face][level].bound_surface = NULL;
				texture->levels[face][level].specified = GL_TRUE;
			}
		}
	}
}

// Add a 3-bit two's complement integer to an integer.
static int dglAddTwosComplement(int a, char b)
{
	if(b & 0x4)
	{
		// Negative.
		return a - ((~b + 1) & 0x7);
	}
	else
	{
		// Positive.
		return a + b;
	}
}

static int dglClamp(int x, int min, int max)
{
	if(x < min)
	{
		return min;
	}
	else if(x > max)
	{
		return max;
	}
	else
	{
		return x;
	}
}

static void* dglDecompressETCTexture(int width, int height, const unsigned char* data)
{
	int bytes_per_pixel = 3; // RGB888

	unsigned char* decompressed = (unsigned char*)malloc(width * height * bytes_per_pixel);
	if(decompressed == NULL)
	{
		return NULL;
	}
	
	{
		int xblock, yblock;

		char dr, dg, db;

		// Number of 4x4 blocks horizontally and vertically.
		int num_xblocks = (width + 3) / 4;
		int num_yblocks = (height + 3) / 4;

		for(yblock = 0; yblock < num_yblocks; yblock++)
		{
			for(xblock = 0; xblock < num_xblocks; xblock++)
			{
				int i;
				char pixel;

				khronos_int64_t blockbits;
				int diffbit, flipbit;

				unsigned char r[2], g[2], b[2];

				int table[2];

				// Construct 64 bits from 8 bytes.
				blockbits = data[0];
				for(i = 1; i < 8; i++)
				{
					blockbits <<= 8;
					blockbits |= data[i];
				}

				diffbit = (blockbits >> 33) & 1;
				flipbit = (blockbits >> 32) & 1;

				// Base color.

				if(!diffbit)
				{
					// Individual mode.
					
					// Subblock 1.
					r[0] = (blockbits >> 60) & 0xf;
					g[0] = (blockbits >> 52) & 0xf;
					b[0] = (blockbits >> 44) & 0xf;

					r[0] |= r[0] << 4;
					g[0] |= g[0] << 4;
					b[0] |= b[0] << 4;

					// Subblock 2.
					r[1] = (blockbits >> 56) & 0xf;
					g[1] = (blockbits >> 48) & 0xf;
					b[1] = (blockbits >> 40) & 0xf;

					r[1] |= r[1] << 4;
					g[1] |= g[1] << 4;
					b[1] |= b[1] << 4;
				}
				else
				{
					// Differential mode.

					r[0] = (blockbits >> 59) & 0x1f;
					g[0] = (blockbits >> 51) & 0x1f;
					b[0] = (blockbits >> 43) & 0x1f;

					dr = (blockbits >> 56) & 0x7;
					dg = (blockbits >> 48) & 0x7;
					db = (blockbits >> 40) & 0x7;

					// Subblock 2.
					r[1] = dglAddTwosComplement(r[0], dr);
					g[1] = dglAddTwosComplement(g[0], dg);
					b[1] = dglAddTwosComplement(b[0], db);

					r[1] = (r[1] << 3) | ((r[1] >> 2) & 0x7);
					g[1] = (g[1] << 3) | ((g[1] >> 2) & 0x7);
					b[1] = (b[1] << 3) | ((b[1] >> 2) & 0x7);

					// Subblock 1.
					r[0] = (r[0] << 3) | ((r[0] >> 2) & 0x7);
					g[0] = (g[0] << 3) | ((g[0] >> 2) & 0x7);
					b[0] = (b[0] << 3) | ((b[0] >> 2) & 0x7);
				}

				// Modifier tables.

				table[0] = (blockbits >> 37) & 0x7;
				table[1] = (blockbits >> 34) & 0x7;

				// Write final pixel colors in a top-down left-right order per block.
				for(pixel = 0; pixel < 4 * 4; pixel++)
				{
					static const int tables[][8] = {{2, 8}, {15, 17}, {9, 29}, {13, 42},
										 	    {18, 60}, {24, 80}, {33, 106}, {47, 183}};
					
					int x, y;
					int loc;
					int subblock;
					int modifier;

					x = 4 * xblock + pixel / 4;
					y = 4 * yblock + pixel % 4;

					if(x >= width || y >= height)
					{
						continue;
					}

					// Memory location of destination pixel.
					loc = y * width + x;
					loc *= bytes_per_pixel;

					if(flipbit)
					{
						subblock = (pixel / 2) & 1;
					}
					else
					{
						subblock = pixel / 8;
					}

					DGLES2_ASSERT(subblock == 0 || subblock == 1);

					modifier = tables[table[subblock]][(blockbits >> pixel) & 1];
					if((blockbits >> (16 + pixel)) & 1)
					{
						modifier *= -1;
					}

					decompressed[loc + 0] = dglClamp(r[subblock] + modifier, 0, 255);
					decompressed[loc + 1] = dglClamp(g[subblock] + modifier, 0, 255);
					decompressed[loc + 2] = dglClamp(b[subblock] + modifier, 0, 255);
				}

				// Move to next block.
				data += 8;
			}
		}
	}

	return decompressed;
}

static GLboolean dglIsPalettedFormat(GLenum format)
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
		return GL_TRUE;
	default:
		return GL_FALSE;
	}
}

static GLenum dglMapPalettedToBaseFormat(GLenum format)
{
	switch(format)
	{
	case GL_PALETTE4_RGB8_OES:
	case GL_PALETTE4_R5_G6_B5_OES:
	case GL_PALETTE8_RGB8_OES:
	case GL_PALETTE8_R5_G6_B5_OES:
		return GL_RGB;

	case GL_PALETTE4_RGBA8_OES:
	case GL_PALETTE4_RGBA4_OES:
	case GL_PALETTE4_RGB5_A1_OES:
	case GL_PALETTE8_RGBA8_OES:
	case GL_PALETTE8_RGBA4_OES:
	case GL_PALETTE8_RGB5_A1_OES:
		return GL_RGBA;
	
	default:
		DGLES2_ASSERT(GL_FALSE);
	}

	// not reached
	return 0;
}

static void* dglDecompressPalettedTexture(int level, GLenum format, int width, int height, int imageSize, const void* data)
{
	const unsigned char* palette = (const unsigned char*)data;
	int bits_per_pixel;
	int palette_entry_size;
	int num_palette_entries;
	const unsigned char* image_data;
	int i;
	int bytes_per_pixel;
	GLenum base_format;
	char* decompressed_data;
	int pixels_per_byte;
	int max_pixels;
	int end;
	int r, g, b, a;
	
	switch(format)
	{
	case GL_PALETTE4_RGB8_OES:
		bits_per_pixel = 4;
		palette_entry_size = 3;
		break;

	case GL_PALETTE4_RGBA8_OES:
		bits_per_pixel = 4;
		palette_entry_size = 4;
		break;

	case GL_PALETTE4_R5_G6_B5_OES:
	case GL_PALETTE4_RGB5_A1_OES:
	case GL_PALETTE4_RGBA4_OES:
		bits_per_pixel = 4;
		palette_entry_size = 2;
		break;

	case GL_PALETTE8_RGB8_OES:
		bits_per_pixel = 8;
		palette_entry_size = 3;
		break;

	case GL_PALETTE8_RGBA8_OES:
		bits_per_pixel = 8;
		palette_entry_size = 4;
		break;

	case GL_PALETTE8_R5_G6_B5_OES:
	case GL_PALETTE8_RGBA4_OES:
	case GL_PALETTE8_RGB5_A1_OES:
		bits_per_pixel = 8;
		palette_entry_size = 2;
		break;

	default:
		DGLES2_ASSERT(GL_FALSE);
	}

	num_palette_entries = 2 << (bits_per_pixel - 1);
	image_data = palette + num_palette_entries * palette_entry_size;

	// Skip to the correct mip level
	for(i = 0; i < level; i++)
	{
		if(bits_per_pixel == 8)
		{
			image_data += width * height * bits_per_pixel / 8;
		}
		else
		{
			DGLES2_ASSERT(bits_per_pixel == 4);
			image_data += width * height * bits_per_pixel / 8 / 2;
		}
		width /= 2;
		height /= 2;
	}

	base_format = dglMapPalettedToBaseFormat(format);
	if(base_format == GL_RGB)
	{
		bytes_per_pixel = 3;
	}
	else
	{
		DGLES2_ASSERT(base_format == GL_RGBA);
		bytes_per_pixel = 4;
	}

	decompressed_data = (char*)malloc(width * height * bytes_per_pixel);
	if(decompressed_data == NULL)
	{
		return NULL;
	}
	
	// Don't go past the end of the data
	pixels_per_byte = 8 / bits_per_pixel;
	max_pixels = ((const unsigned char*)data + imageSize - image_data) * pixels_per_byte;
	end = dglMin(width * height, max_pixels);

	for(i = 0; i < end; i++)
	{
		int index;
		if(bits_per_pixel == 4)
		{
			if(i & 1)
			{
				index = image_data[i / 2] & 15;
			}
			else
			{
				index = image_data[i / 2] >> 4;
			}
		}
		else
		{
			DGLES2_ASSERT(bits_per_pixel == 8);
			index = image_data[i];
		}

		switch(format)
		{
		case GL_PALETTE4_RGB8_OES:
		case GL_PALETTE8_RGB8_OES:
			r = palette[index*3];
			g = palette[index*3+1];
			b = palette[index*3+2];
			break;

		case GL_PALETTE4_RGBA8_OES:
		case GL_PALETTE8_RGBA8_OES:
			r = palette[index*4];
			g = palette[index*4+1];
			b = palette[index*4+2];
			a = palette[index*4+3];
			break;

		case GL_PALETTE4_R5_G6_B5_OES:
		case GL_PALETTE8_R5_G6_B5_OES:
			r = palette[index*2+1] >> 3;
			r = (r << 3) | (r >> 2);
			g = ((palette[index*2+1] & 7) << 3) | (palette[index*2] >> 5);
			g = (g << 2) | (g >> 4);
			b = palette[index*2] & 0x1f;
			b = (b << 3) | (b >> 2);
			break;

		case GL_PALETTE4_RGBA4_OES:
		case GL_PALETTE8_RGBA4_OES:
			r = palette[index*2+1] >> 4;
			r |= (r << 4) | r;
			g = palette[index*2+1] & 0xf;
			g |= (g << 4) | g;
			b = palette[index*2] >> 4;
			b |= (b << 4) | b;
			a = palette[index*2] & 0xf;
			a |= (a << 4) | a;
			break;

		case GL_PALETTE4_RGB5_A1_OES:
		case GL_PALETTE8_RGB5_A1_OES:
			r = palette[index*2+1] >> 3;
			r = (r << 3) | (r >> 2);
			g = ((palette[index*2+1] & 7) << 2) | (palette[index*2] >> 6);
			g = (g << 3) | (g >> 2);
			b = (palette[index*2] >> 1) & 0x1f;
			b = (b << 3) | (b >> 2);
			a = (palette[index*2] & 1) ? 255 : 0;
			break;
			
		default:
			DGLES2_ASSERT(GL_FALSE);
		}

		if(base_format == GL_RGB)
		{
			decompressed_data[i*3+0] = r;
			decompressed_data[i*3+1] = g;
			decompressed_data[i*3+2] = b;
		}
		else
		{
			DGLES2_ASSERT(base_format == GL_RGBA);
			decompressed_data[i*4+0] = r;
			decompressed_data[i*4+1] = g;
			decompressed_data[i*4+2] = b;
			decompressed_data[i*4+3] = a;
		}
	}

	return decompressed_data;
}

GL_APICALL_BUILD void GL_APIENTRY glActiveTexture(GLenum texture)
{
	DGLES2_ENTER();
	ctx->hgl.ActiveTexture(texture);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glBindTexture (GLenum target, GLuint texture)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(target != GL_TEXTURE_2D && target != GL_TEXTURE_CUBE_MAP, GL_INVALID_ENUM);
	DGLContext_getHostError(ctx);
//	Dprintf("glBindTexture(%x, %d)\n", target, texture);
	ctx->hgl.BindTexture(target, texture);
	if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
	{
		if(!DGLContext_bindTexture(ctx, target, texture))
		{
			DGLES2_ERROR(GL_OUT_OF_MEMORY);
		}
	}
	DGLES2_LEAVE_NO_ERROR_CHECK();
}

static GLboolean dglIsValid2DTextureTarget(GLenum target)
{
	switch(target)
	{
		case GL_TEXTURE_2D:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			return GL_TRUE;

		default:
			return GL_FALSE;
	}
}

GL_APICALL_BUILD void GL_APIENTRY glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(!dglIsValid2DTextureTarget(target), GL_INVALID_ENUM);
	DGLES2_ERROR_IF(!dglIsPalettedFormat(internalformat) &&
					internalformat != GL_ETC1_RGB8_OES,
					GL_INVALID_ENUM);
	DGLES2_ERROR_IF(height < 0, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(width < 0, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(border != 0, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(imageSize < 0, GL_INVALID_VALUE);
	{
		if(dglIsPalettedFormat(internalformat))
		{
			int num_levels, cur_level;
			GLenum base_format;
			DGLTexture* texture;

			base_format = dglMapPalettedToBaseFormat(internalformat);
			texture = DGLContext_getTexture(ctx, target);
			DGLES2_ASSERT(texture != NULL);

			DGLES2_ERROR_IF(level > 0, GL_INVALID_VALUE);
			DGLES2_ERROR_IF(level < -ctx->max_texture_level, GL_INVALID_VALUE);

			num_levels = -level + 1;
			for(cur_level = 0; cur_level < num_levels; cur_level++)
			{
				if(data != NULL)
				{
					void* decompressed_data = dglDecompressPalettedTexture(cur_level, internalformat, width, height, imageSize, data);
					if(decompressed_data == NULL)
					{
						DGLES2_ERROR(GL_OUT_OF_MEMORY);
					}
					ctx->hgl.TexImage2D(target, cur_level, base_format, width, height, border, base_format, GL_UNSIGNED_BYTE, decompressed_data);
					free(decompressed_data);
				}
				else
				{
					ctx->hgl.TexImage2D(target, cur_level, base_format, width, height, border, base_format, GL_UNSIGNED_BYTE, NULL);
				}
				if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
				{
					DGLTexture_setLevel(texture, target, level, internalformat, width, height);
					DGLTexture_setEGLImage(texture, target, NULL);
				}
				width /= 2;
				height /= 2;
			}
		}
		else
		{
			void* decompressed_data;
			int numblocks;

			DGLES2_ASSERT(internalformat == GL_ETC1_RGB8_OES);

			DGLES2_ERROR_IF(level < 0, GL_INVALID_VALUE);
			DGLES2_ERROR_IF(level > ctx->max_texture_level, GL_INVALID_VALUE);

			numblocks = ((width + 3) / 4) * ((height + 3) / 4);

			if(imageSize != numblocks * 8)
			{
				DGLES2_ERROR(GL_INVALID_VALUE);
			}

			decompressed_data = dglDecompressETCTexture(width, height, (const unsigned char*)data);
			ctx->hgl.TexImage2D(target, level, GL_RGB, width, height, border, GL_RGB, GL_UNSIGNED_BYTE, decompressed_data);
			free(decompressed_data);
			if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
			{
				DGLTexture* texture;
				GLeglImageOES image;
				
				texture = DGLContext_getTexture(ctx, target);
				DGLES2_ASSERT(texture != NULL);
				DGLTexture_setLevel(texture, target, level, internalformat, width, height);
				
				image = DGLTexture_getEGLImage(texture, target);
				if(image != NULL)
				{
					// Texture is respecified. It is no longer an EGLImage sibling.
					deglUnregisterImageTarget(image, target, texture->obj.name);
					DGLTexture_setEGLImage(texture, target, NULL);
				}

				{
					DGLTextureLevel* level_obj = DGLTexture_getLevel(texture, target, level);
					if(level_obj->bound_surface != NULL)
					{
						// Texture is respecified. Release the bound EGLSurface.
						deglReleaseTexImage(level_obj->bound_surface, texture->obj.name, level);
						level_obj->bound_surface = NULL;
					}
				}
			}
		}
	}
	DGLES2_LEAVE_NO_ERROR_CHECK();
}

GL_APICALL_BUILD void GL_APIENTRY glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(!dglIsValid2DTextureTarget(target), GL_INVALID_ENUM);
	DGLES2_ERROR_IF(level < 0, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(level > ctx->max_texture_level, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(!dglIsPalettedFormat(format) && format != GL_ETC1_RGB8_OES, GL_INVALID_ENUM);
	// No supported formats.
	DGLES2_ERROR(GL_INVALID_OPERATION);
	DGLES2_LEAVE();
}

static GLboolean dglIsValidFormat(GLenum format)
{
	switch(format)
	{
		case GL_ALPHA:
		case GL_RGB:
		case GL_RGBA:
		case GL_LUMINANCE:
		case GL_LUMINANCE_ALPHA:
			return GL_TRUE;

		default:
			return GL_FALSE;
	}
}

GL_APICALL_BUILD void GL_APIENTRY glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(!dglIsValid2DTextureTarget(target), GL_INVALID_ENUM);
	DGLES2_ERROR_IF(level < 0, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(level > ctx->max_texture_level, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(!dglIsValidFormat(internalformat), GL_INVALID_ENUM);
	DGLES2_ERROR_IF(border != 0, GL_INVALID_VALUE);
	{
		DGLContext_getHostError(ctx);

		DGLES2_BEGIN_READING();
		ctx->hgl.CopyTexImage2D(target, level, internalformat, x, y, width, height, border);	
		DGLES2_END_READING();

		if(DGLContext_getHostError(ctx) == GL_NO_ERROR) {
			DGLTexture* texture;
			GLeglImageOES image;
			
			texture = DGLContext_getTexture(ctx, target);
			DGLES2_ASSERT(texture != NULL);
			DGLTexture_setLevel(texture, target, level, internalformat, width, height);

			image = DGLTexture_getEGLImage(texture, target);
			if(image != NULL)
			{
				// Texture is respecified. It is no longer an EGLImage sibling.
				deglUnregisterImageTarget(image, target, texture->obj.name);
				DGLTexture_setEGLImage(texture, target, NULL);
			}

			{
				DGLTextureLevel* level_obj = DGLTexture_getLevel(texture, target, level);
				if(level_obj->bound_surface != NULL)
				{
					// Texture is respecified. Release the bound EGLSurface.
					deglReleaseTexImage(level_obj->bound_surface, texture->obj.name, level);
					level_obj->bound_surface = NULL;
				}
			}
		}
	}
	DGLES2_LEAVE_NO_ERROR_CHECK();
}

GL_APICALL_BUILD void GL_APIENTRY glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(!dglIsValid2DTextureTarget(target), GL_INVALID_ENUM);
	{
		DGLTextureLevel* level_obj = DGLContext_getTextureLevel(ctx, target, level);
		DGLES2_ASSERT(level_obj != NULL);
		if(dglIsPalettedFormat(level_obj->format) || level_obj->format == GL_ETC1_RGB8_OES)
		{
			DGLES2_ERROR(GL_INVALID_OPERATION);
		}
	}
	DGLContext_getHostError(ctx);
	DGLES2_BEGIN_READING();
	ctx->hgl.CopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
	DGLES2_END_READING();
	if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
	{
		DGLTexture* texture;
		GLeglImageOES image;

		texture = DGLContext_getTexture(ctx, target);
		DGLES2_ASSERT(texture != NULL);
		image = DGLTexture_getEGLImage(texture, target);
		if(image != NULL)
		{
			deglUpdateImageSiblings(image, target, texture->obj.name);
		}
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glDeleteTextures(GLsizei n, const GLuint* textures)
{
	DGLES2_ENTER();
	DGLContext_getHostError(ctx);
	ctx->hgl.DeleteTextures(n, textures);
	if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
	{
		int i;
		for(i = 0; i < n; i++)
		{
			DGLContext_destroyTexture(ctx, textures[n]);
		}
	}
	DGLES2_LEAVE_NO_ERROR_CHECK();
}

GL_APICALL_BUILD void GL_APIENTRY glGenerateMipmap (GLenum target)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(target != GL_TEXTURE_2D && target != GL_TEXTURE_CUBE_MAP, GL_INVALID_ENUM);
	DGLContext_getHostError(ctx);
	ctx->hgl.GenerateMipmapEXT(target);
	if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
	{
		DGLTexture* texture;
		GLeglImageOES image;
		
		texture = DGLContext_getTexture(ctx, target);
		DGLTexture_generateMipmap(texture);
		image = DGLTexture_getEGLImage(texture, target);
		if(image != NULL)
		{
			// Texture is respecified. It is no longer an EGLImage sibling.
			deglUnregisterImageTarget(image, target, texture->obj.name);
			DGLTexture_setEGLImage(texture, target, NULL);
		}
	}
	DGLES2_LEAVE_NO_ERROR_CHECK();
}

GL_APICALL_BUILD void GL_APIENTRY glGenTextures(GLsizei n, GLuint* textures)
{
	DGLES2_ENTER();
	ctx->hgl.GenTextures(n, textures);
	DGLES2_LEAVE();
}

static GLboolean dglIsValidTextureParameter(GLenum pname)
{
	switch(pname)
	{
		case GL_TEXTURE_WRAP_S:
		case GL_TEXTURE_WRAP_T:
		case GL_TEXTURE_MIN_FILTER:
		case GL_TEXTURE_MAG_FILTER:
			return GL_TRUE;

		default:
			return GL_FALSE;
	}
}

GL_APICALL_BUILD void GL_APIENTRY glGetTexParameterfv(GLenum target, GLenum pname, GLfloat* params)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(target != GL_TEXTURE_2D && target != GL_TEXTURE_CUBE_MAP, GL_INVALID_ENUM);
	DGLES2_ERROR_IF(!dglIsValidTextureParameter(pname), GL_INVALID_ENUM);
	ctx->hgl.GetTexParameterfv(target, pname, params);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetTexParameteriv(GLenum target, GLenum pname, GLint* params)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(target != GL_TEXTURE_2D && target != GL_TEXTURE_CUBE_MAP, GL_INVALID_ENUM);
	DGLES2_ERROR_IF(!dglIsValidTextureParameter(pname), GL_INVALID_ENUM);
	ctx->hgl.GetTexParameteriv(target, pname, params);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD GLboolean GL_APIENTRY glIsTexture(GLuint texture)
{
	DGLES2_ENTER_RET(GL_FALSE);
	DGLES2_LEAVE_RET(ctx->hgl.IsTexture(texture));
}

static GLfloat* dglConvertHalfTextureToFloat(GLsizei width, GLsizei height, GLenum format, const void* pixels)
{
	int components;
	GLfloat* conv;
	int i;

	switch(format)
	{
		case GL_ALPHA:
		case GL_LUMINANCE:
			components = 1;
			break;

		case GL_LUMINANCE_ALPHA:
			components = 2;
			break;

		case GL_RGB:
			components = 3;
			break;

		case GL_RGBA:
			components = 4;
			break;

		default:
			DGLES2_ASSERT(GL_FALSE);
	}

	conv = (GLfloat*)malloc(width * height * components * sizeof(GLfloat));
	if(conv == NULL)
	{
		return NULL;
	}

	for(i = 0; i < width * height * components; i++)
	{
		conv[i] = dglConvertHalfToFloat(((GLfixed*)pixels)[i]);
	}

	return conv;
}

static GLboolean dglIsValidType(GLenum type)
{
	switch(type)
	{
		case GL_UNSIGNED_BYTE:
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_5_5_5_1:
			return GL_TRUE;

		default:
			return GL_FALSE;
	}
}

GL_APICALL_BUILD void GL_APIENTRY glTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(!dglIsValid2DTextureTarget(target), GL_INVALID_ENUM);
	DGLES2_ERROR_IF(level < 0, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(level > ctx->max_texture_level, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(dglIsPalettedFormat(internalformat), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!dglIsValidFormat(internalformat), GL_INVALID_VALUE);
	DGLES2_ERROR_IF(!dglIsValidFormat(format), GL_INVALID_ENUM);
	DGLES2_ERROR_IF(!dglIsValidType(type), GL_INVALID_ENUM);
	DGLES2_ERROR_IF(border != 0, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(format != internalformat, GL_INVALID_OPERATION);
	
	DGLContext_getHostError(ctx);

	if(pixels != NULL && type == GL_HALF_FLOAT_OES)
	{
		GLfloat* conv = dglConvertHalfTextureToFloat(width, height, format, pixels);
		if(conv == NULL)
		{
			DGLES2_ERROR(GL_OUT_OF_MEMORY);
		}
		ctx->hgl.TexImage2D(target, level, internalformat, width, height, border, format, GL_FLOAT, conv);
		free(conv);
	}
	else
	{
		ctx->hgl.TexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
	}

	if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
	{
		DGLTexture* texture;
		GLeglImageOES image;
		
		texture = DGLContext_getTexture(ctx, target);
		DGLES2_ASSERT(texture != NULL);
		DGLTexture_setLevel(texture, target, level, internalformat, width, height);

		image = DGLTexture_getEGLImage(texture, target);
		if(image != NULL)
		{
			// Texture is respecified. It is no longer an EGLImage sibling.
			deglUnregisterImageTarget(image, target, texture->obj.name);
			DGLTexture_setEGLImage(texture, target, NULL);
		}

		{
			DGLTextureLevel* level_obj = DGLTexture_getLevel(texture, target, level);
			if(level_obj->bound_surface != NULL)
			{
				// Texture is respecified. Release the bound EGLSurface.
				deglReleaseTexImage(level_obj->bound_surface, texture->obj.name, level);
				level_obj->bound_surface = NULL;
			}
		}
	}

	DGLES2_LEAVE_NO_ERROR_CHECK();
}

GL_APICALL_BUILD void GL_APIENTRY glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(target != GL_TEXTURE_2D && target != GL_TEXTURE_CUBE_MAP, GL_INVALID_ENUM);
	DGLES2_ERROR_IF(!dglIsValidTextureParameter(pname), GL_INVALID_ENUM);
	ctx->hgl.TexParameterf(target, pname, param);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(target != GL_TEXTURE_2D && target != GL_TEXTURE_CUBE_MAP, GL_INVALID_ENUM);
	DGLES2_ERROR_IF(!dglIsValidTextureParameter(pname), GL_INVALID_ENUM);
	ctx->hgl.TexParameterfv(target, pname, params);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glTexParameteri(GLenum target, GLenum pname, GLint param)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(target != GL_TEXTURE_2D && target != GL_TEXTURE_CUBE_MAP, GL_INVALID_ENUM);
	DGLES2_ERROR_IF(!dglIsValidTextureParameter(pname), GL_INVALID_ENUM);
	ctx->hgl.TexParameteri(target, pname, param);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glTexParameteriv(GLenum target, GLenum pname, const GLint* params)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(target != GL_TEXTURE_2D && target != GL_TEXTURE_CUBE_MAP, GL_INVALID_ENUM);
	DGLES2_ERROR_IF(!dglIsValidTextureParameter(pname), GL_INVALID_ENUM);
	ctx->hgl.TexParameteriv(target, pname, params);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(!dglIsValid2DTextureTarget(target), GL_INVALID_ENUM);
	DGLES2_ERROR_IF(level < 0, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(level > ctx->max_texture_level, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(!dglIsValidFormat(format), GL_INVALID_ENUM);
	DGLES2_ERROR_IF(!dglIsValidType(type), GL_INVALID_ENUM);
	{
		DGLTextureLevel* level_obj = DGLContext_getTextureLevel(ctx, target, level);
		DGLES2_ASSERT(level_obj != NULL);
		if(format != level_obj->format)
		{
			DGLES2_ERROR(GL_INVALID_OPERATION);
		}

		DGLContext_getHostError(ctx);

		if(pixels != NULL && type == GL_HALF_FLOAT_OES)
		{
			GLfloat* conv = dglConvertHalfTextureToFloat(width, height, format, pixels);
			if(conv == NULL)
			{
				DGLES2_ERROR(GL_OUT_OF_MEMORY);
			}
			ctx->hgl.TexSubImage2D(target, level, xoffset, yoffset, width, height, format, GL_FLOAT, conv);
			free(conv);
		}
		else
		{
			ctx->hgl.TexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
		}

		if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
		{
			DGLTexture* texture;
			GLeglImageOES image;
			
			texture = DGLContext_getTexture(ctx, target);
			DGLES2_ASSERT(texture != NULL);
			image = DGLTexture_getEGLImage(texture, target);
			if(image != NULL)
			{
				deglUpdateImageSiblings(image, target, texture->obj.name);
			}
		}
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glEGLImageTargetTexture2DOES(GLenum target, GLeglImageOES image)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(target != GL_TEXTURE_2D, GL_INVALID_ENUM);
	DGLES2_ERROR_IF(image == NULL, GL_INVALID_OPERATION);
	{
		// Clear all mipmap levels.
		int level;
		for(level = 0; level <= ctx->max_texture_level; level++)
		{
			ctx->hgl.TexImage2D(target, level, GL_RGBA, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}

		if(!DGLContext_specifyTextureFromEGLImage(ctx, image, target))
		{
			DGLES2_ERROR(GL_INVALID_OPERATION);
		}

		{
			DGLTexture* texture = DGLContext_getTexture(ctx, target);
			DGLTextureLevel* level_obj = DGLTexture_getLevel(texture, target, 0);
			if(level_obj->bound_surface != NULL)
			{
				// Texture is respecified. Release the bound EGLSurface.
				deglReleaseTexImage(level_obj->bound_surface, texture->obj.name, 0);
				level_obj->bound_surface = NULL;
			}
		}
	}
	DGLES2_LEAVE_NO_ERROR_CHECK();
}
