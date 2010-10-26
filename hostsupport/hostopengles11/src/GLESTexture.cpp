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

#include "GLESTexture.h"
#include "glesInternal.h"

GLESTexture::GLESTexture(unsigned int name) :
	m_name(name),
	m_numLevels(0),
	m_levels(NULL)
{
}

GLESTexture::~GLESTexture()
{
	delete[] m_levels;
}

bool GLESTexture::AllocateLevels(int numLevels)
{
	GLES_ASSERT(m_levels == NULL);

	m_numLevels = numLevels;
	m_levels = GLES_NEW GLESTextureLevel[numLevels];
	if(m_levels == NULL)
	{
		return false;
	}

	for(int i = 0; i < numLevels; i++)
	{
		// From desktop GL spec.
		m_levels[i].format = 1;
		m_levels[i].width = 0;
		m_levels[i].height = 0;
		
		m_levels[i].boundSurface = NULL;
	}

	return true;
}

GLESTextureLevel* GLESTexture::Level(int level)
{
	GLES_ASSERT(m_levels != NULL);
	GLES_ASSERT(level >= 0 && level < m_numLevels);
	return &m_levels[level];
}

void GLESTexture::SetLevel(int level, GLenum format, GLsizei width, GLsizei height)
{
	GLES_ASSERT(m_levels != NULL);
	GLES_ASSERT(level >= 0 && level < m_numLevels);
	m_levels[level].format = format;
	m_levels[level].width = width;
	m_levels[level].height = height;
}

void GLESTexture::GenerateMipmap()
{
	GLES_ASSERT(m_levels != NULL);

	const GLESTextureLevel& level_zero = m_levels[0];

	m_numLevels = glesLog2(GLES_MAX(level_zero.width, level_zero.height)) + 1;

	int width = level_zero.width;
	int height = level_zero.height;

	for(int level = 1; level < m_numLevels; level++)
	{
		if(width > 1) width /= 2;
		if(height > 1) height /= 2;

		GLES_ASSERT((width > 1 || height > 1) || level == m_numLevels - 1);

		m_levels[level].format = level_zero.format;
		m_levels[level].width = width;
		m_levels[level].height = height;
	}
}

bool glesIsValidCompressedFormat(GLenum format)
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

GLenum glesMapCompressedToBaseFormat(GLenum format)
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
		GLES_ASSERT(false);
	}

	// not reached
	return 0;
}

void* glesUncompressImage(int level, GLenum format, int width, int height, int imageSize, const void* data)
{
	const unsigned char* palette = static_cast<const unsigned char*>(data);
	int bitsPerPixel;
	int paletteEntrySize;
	
	switch(format)
	{
	case GL_PALETTE4_RGB8_OES:
		bitsPerPixel = 4;
		paletteEntrySize = 3;
		break;

	case GL_PALETTE4_RGBA8_OES:
		bitsPerPixel = 4;
		paletteEntrySize = 4;
		break;

	case GL_PALETTE4_R5_G6_B5_OES:
	case GL_PALETTE4_RGB5_A1_OES:
	case GL_PALETTE4_RGBA4_OES:
		bitsPerPixel = 4;
		paletteEntrySize = 2;
		break;

	case GL_PALETTE8_RGB8_OES:
		bitsPerPixel = 8;
		paletteEntrySize = 3;
		break;

	case GL_PALETTE8_RGBA8_OES:
		bitsPerPixel = 8;
		paletteEntrySize = 4;
		break;

	case GL_PALETTE8_R5_G6_B5_OES:
	case GL_PALETTE8_RGBA4_OES:
	case GL_PALETTE8_RGB5_A1_OES:
		bitsPerPixel = 8;
		paletteEntrySize = 2;
		break;

	default:
		GLES_ASSERT(false);
	}

	int numPaletteEntries = 2 << (bitsPerPixel - 1);
	const unsigned char* imageData = palette + numPaletteEntries * paletteEntrySize;

	// Skip to the correct mip level
	for(int i = 0; i < level; i++)
	{
		if(bitsPerPixel == 8)
		{
			imageData += width * height * bitsPerPixel / 8;
		}
		else
		{
			GLES_ASSERT(bitsPerPixel == 4);
			imageData += width * height * bitsPerPixel / 8 / 2;
		}
		width /= 2;
		height /= 2;
	}

	int bytesPerPixel;
	GLenum baseFormat = glesMapCompressedToBaseFormat(format);
	if(baseFormat == GL_RGB)
	{
		bytesPerPixel = 3;
	}
	else
	{
		GLES_ASSERT(baseFormat == GL_RGBA);
		bytesPerPixel = 4;
	}

	char* uncompressedData = GLES_NEW char[width * height * bytesPerPixel];
	if(uncompressedData == NULL)
	{
		return NULL;
	}
	
	// Don't go past the end of the data
	int pixelsPerByte = 8 / bitsPerPixel;
	int maxPixels = (static_cast<const unsigned char*>(data) + imageSize - imageData) * pixelsPerByte;
	int end = GLES_MIN(width * height, maxPixels);

	for(int i = 0; i < end; i++)
	{
		int index;
		if(bitsPerPixel == 4)
		{
			if(i & 1)
			{
				index = imageData[i / 2] & 15;
			}
			else
			{
				index = imageData[i / 2] >> 4;
			}
		}
		else
		{
			GLES_ASSERT(bitsPerPixel == 8);
			index = imageData[i];
		}

		int r, g, b, a;

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
			GLES_ASSERT(false);
		}

		if(baseFormat == GL_RGB)
		{
			uncompressedData[i*3+0] = r;
			uncompressedData[i*3+1] = g;
			uncompressedData[i*3+2] = b;
		}
		else
		{
			GLES_ASSERT(baseFormat == GL_RGBA);
			uncompressedData[i*4+0] = r;
			uncompressedData[i*4+1] = g;
			uncompressedData[i*4+2] = b;
			uncompressedData[i*4+3] = a;
		}
	}

	return uncompressedData;
}
