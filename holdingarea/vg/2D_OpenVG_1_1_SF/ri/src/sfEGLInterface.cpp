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

#include "SurfaceDescriptor.h"
#include "BufferContainer.h"

#include "sfEGLInterface.h"
#include "riContext.h"
#include "riPath.h"
#include "vgext.h"
#include "riImage.h"

namespace 
    {
    EGLtoVGInterface g_EGLtoVGInterface;
    }

IEGLtoVGInterface* getVGInterface(void)
{    
    return &g_EGLtoVGInterface;
}

EGLtoVGInterface::EGLtoVGInterface() :
    m_egl(NULL)
{
    m_contexts.reserve(4);
}

EGLtoVGInterface::~EGLtoVGInterface()
{
    for(int i = 0; i < m_contexts.size(); i++)
    {
        RI_ASSERT(m_contexts[i]);
        RI_DELETE(m_contexts[i]);
    }
}

void EGLtoVGInterface::SetEGLInterface( IVGtoEGLInterface* egl )
{
    RI_ASSERT(!m_egl);
    m_egl = egl;
}

int EGLtoVGInterface::findContext(OpenVGRI::VGContext* contextPtr)
{
    return m_contexts.findIndex(contextPtr);
}

bool EGLtoVGInterface::isValidImage(void* image)
    {
    bool ret = false;
    for(int i = 0; i < m_contexts.size() && !ret; i++)
        {
        ret = m_contexts[i]->isValidImage((VGImage)image);
        }
    return ret;
    }

void* EGLtoVGInterface::CreateContext( void* shareContext )
{
    if (shareContext)
    {
        if (findContext((OpenVGRI::VGContext*)shareContext) < 0)
            return NULL;
    }

    OpenVGRI::VGContext* newContext = NULL;

    try 
    {
        newContext = RI_NEW(OpenVGRI::VGContext, ((OpenVGRI::VGContext*)shareContext));
        m_contexts.push_back(newContext);
    } 
    catch (std::bad_alloc)
    {
        if (newContext) 
            delete newContext;

        newContext = NULL;
    }

    return newContext;
}

bool EGLtoVGInterface::ReleaseContext( void* context )
{
    int contextIndex = findContext((OpenVGRI::VGContext*)context);

    if (contextIndex < 0)
        return false;

    OpenVGRI::VGContext* ctx = (OpenVGRI::VGContext*)context;
    if( !m_contexts.remove(ctx) )
        return false;
    
    RI_DELETE(ctx);

    return true;
}

OpenVGRI::Color::Descriptor EGLtoVGInterface::vgDescriptorFromSurfaceDescriptor(const SurfaceDescriptor* sdesc)
{
    const CColorDescriptor& scdesc = sdesc->m_colorDescriptor;
    OpenVGRI::Color::Descriptor vdesc;
    unsigned int formatBits = 0;

    // VG formats are built favoring the first ones in the enum (RGBA, RGBX, etc.)

    // Padded alpha (RGBX, etc.) must be handled. For example:
    // if (vdesc.bitsPerPixel < sdesc.bitsPerPixel)
    //      alphabits = 0, alphashift = 8
    vdesc.bitsPerPixel = scdesc.m_bpp;
    vdesc.bytesPerPixel = vdesc.bitsPerPixel >> 3;

    vdesc.alphaBits = scdesc.m_alphaSize;
    vdesc.alphaShift = sdesc->m_alphaShift;
    vdesc.redBits = scdesc.m_redSize;
    vdesc.redShift = sdesc->m_redShift;
    vdesc.greenBits = scdesc.m_greenSize;
    vdesc.greenShift = sdesc->m_greenShift;
    vdesc.blueBits = scdesc.m_blueSize;
    vdesc.blueShift = sdesc->m_blueShift;
    vdesc.luminanceBits = scdesc.m_luminanceSize;
    vdesc.luminanceShift = sdesc->m_luminanceShift;
    
    if(scdesc.isLuminance())
        formatBits |= OpenVGRI::Color::LUMINANCE;    
    
    // \note Could be copied if LUMINANCE == LUMINANCE, etc.
    if (scdesc.isPremultiplied())
        formatBits |= OpenVGRI::Color::PREMULTIPLIED;

    if (scdesc.isNonlinear())
        formatBits |= OpenVGRI::Color::NONLINEAR;

    vdesc.internalFormat = (OpenVGRI::Color::InternalFormat)formatBits;
    // \todo format
    vdesc.vgFormat = (VGImageFormat)-1; // Not necessarily any VG image format
    vdesc.shape = vdesc.getShape();

    return vdesc;
}

bool EGLtoVGInterface::CreateSurface(const SurfaceDescriptor* desc, BufferContainer* buffers, void* image)
{
    RI_ASSERT( buffers );
    OpenVGRI::Color::Descriptor vgColorDescriptor; 
    OpenVGRI::Image* newImage = NULL;
    if(image)
        {
        if(!isValidImage(image))
            return false;
        newImage = (OpenVGRI::Image*)image;
        vgColorDescriptor = newImage->getDescriptor();
        }
    else
        {
        vgColorDescriptor = vgDescriptorFromSurfaceDescriptor(desc);
        }
    OpenVGRI::Drawable* newDrawable = NULL;
    
    //VGImageQuality quality = VG_IMAGE_QUALITY_BETTER;

    int w = desc->m_width;
    int h = desc->m_height;
    int stride = OpenVGRI::Image::descriptorToStride(vgColorDescriptor, w);
    size_t bufSize = h * stride;

    OpenVGRI::RIuint8* dataPtr = NULL;

    try 
    {
        int maskBits = 0;
        if( !newImage )
            {
            newImage = RI_NEW(OpenVGRI::Image,(vgColorDescriptor, w, h, VG_IMAGE_QUALITY_BETTER));
            maskBits = desc->m_maskSize;
            }
        else
            {
            dataPtr = newImage->getData();
            maskBits = newImage->getDescriptor().maskBits;
            }
        newDrawable = RI_NEW(OpenVGRI::Drawable, (newImage, maskBits));
        newDrawable->addReference();
    } 
    catch (std::bad_alloc)
    {
        if (dataPtr)        RI_DELETE_ARRAY(dataPtr);
        if (newImage)       RI_DELETE(newImage);
        if (newDrawable)    RI_DELETE(newDrawable);

        return false;
    }
    buffers->m_clientSurface = newDrawable;
    buffers->m_colorBuffer = newDrawable->getColorBuffer()->getImage()->getData();
    buffers->m_maskBuffer = newDrawable->getMaskBuffer();
    return true;
}

bool EGLtoVGInterface::ReleaseSurface(void* surface)
{
    RI_ASSERT(surface);

    OpenVGRI::Drawable *drawable = (OpenVGRI::Drawable*)surface;

    if (!drawable->removeReference())
        RI_DELETE(drawable);
    
	return true;
}

bool EGLtoVGInterface::SetCurrentSurface( void* context, void* surface )
{
    OpenVGRI::Drawable* drawable = (OpenVGRI::Drawable*)surface;
    OpenVGRI::VGContext *ctx = (OpenVGRI::VGContext*)context;

    int i = findContext(ctx);

    if (i < 0)
        return false;

    ctx->setDefaultDrawable(drawable);

    return true;
}

bool EGLtoVGInterface::ResizeSurface( void* context, void* surface, int width, int height, BufferContainer* buffers )
    {
    OpenVGRI::Drawable* drawable = (OpenVGRI::Drawable*)surface;
    OpenVGRI::VGContext *ctx = (OpenVGRI::VGContext*)context;
    int i = findContext(ctx);
    if(i < 0)
        return false;
    
    try 
    {
        drawable->resize( ctx, width, height );
    } 
    catch (std::bad_alloc) 
    {
        return false;
    }
    buffers->m_clientSurface = drawable;
    buffers->m_colorBuffer = drawable->getColorBuffer()->getImage()->getData();
    buffers->m_maskBuffer = drawable->getMaskBuffer();
    return true;
    }

bool EGLtoVGInterface::IsValidImage( void* image, CColorDescriptor* colorDesc, int* width, int* height )
{
    bool ret = isValidImage(image);
    if(ret)
        {
        *width = ((OpenVGRI::Image*)image)->getWidth();
        *height = ((OpenVGRI::Image*)image)->getHeight();
        const OpenVGRI::Color::Descriptor& desc = ((OpenVGRI::Image*)image)->getDescriptor();
        colorDesc->m_bpp = desc.bitsPerPixel;
        colorDesc->m_redSize = desc.redBits;
        colorDesc->m_greenSize = desc.greenBits;
        colorDesc->m_blueSize = desc.blueBits;
        colorDesc->m_alphaSize = desc.alphaBits;
        colorDesc->m_luminanceSize = desc.luminanceBits;
        colorDesc->m_alphaMaskSize = desc.maskBits;
        colorDesc->m_format = (CColorDescriptor::ColorFormat)desc.internalFormat;
        }

    return ret;
}

bool EGLtoVGInterface::IsImageInUse( void* image )
{
    bool ret = false;
    if(image && isValidImage(image))
        {
        ret = ((OpenVGRI::Image*)image)->isInUse();
        }
    return ret;
}

void* EGLtoVGInterface::CreateImage()
{
    RI_ASSERT(false);
	return NULL;
}

bool EGLtoVGInterface::ReleaseImage()
{
    RI_ASSERT(false);
	return false;
}

void EGLtoVGInterface::Flush()
{
    vgFlush();
}

void EGLtoVGInterface::Finish()
{
    vgFinish();
}

fpVGProc EGLtoVGInterface::GetVGProcAddress( const char *procname )
{
    fpVGProc ret = NULL;
    if(strcmp(procname, "vgePathCoordsSizeInBytes") == 0)
        {
        ret = (fpVGProc)vgePathCoordsSizeInBytes;
        }
    return ret;
}

void EGLtoVGInterface::CopyBuffers( void* buffer, int stride, void* surface )
    {
    OpenVGRI::Drawable *drawable = (OpenVGRI::Drawable*)surface;
    int width = drawable->getColorBuffer()->getWidth();
    int height = drawable->getColorBuffer()->getHeight();
	// \todo Pixel format.
    VGImageFormat format = VG_sARGB_8888_PRE;
    vgReadPixels( buffer, stride, format, 0, 0, width, height );
    }

void EGLtoVGInterface::UpdateBuffers( void* buffer, int stride, const SurfaceDescriptor* desc )
    {
    // \todo format, errors
    VGImageFormat format = VG_sARGB_8888_PRE;
    vgWritePixels( buffer, stride, format, 0, 0, desc->m_width, desc->m_height );
    }

bool EGLtoVGInterface::IsRootImage( void* image )
    {
    if( !image ) return false;    
    if ( vgGetParent( (VGImage)image ) )
        {
        // if vgGetParent returns not NULL image it is not parent image
        // , only child image has parent image, and this should return false
        return false;
        }
    // vgGetParent is NULL and image is parent image
    return true;
    }                                                    

void EGLtoVGInterface::GetImageData( void* image, SurfaceDescriptor& desc, void* data )
    {
    OpenVGRI::Image* vgimage = (OpenVGRI::Image*)image;
    if( !image )
        {
        return;
        }
    desc.m_height = vgimage->getHeight();
    desc.m_width  = vgimage->getWidth();
    int bufSize;
    
    OpenVGRI::Color::Descriptor colorDesc   = vgimage->getDescriptor();
	VGImageFormat vgFormat;
	// Convert some formats into more GL-friendly formats.
	if( colorDesc.vgFormat == VG_BW_1 )
		{
		vgFormat = VG_lL_8;
		}
	else if( colorDesc.vgFormat == VG_A_1 || colorDesc.vgFormat == VG_A_4 )
		{
		vgFormat = VG_A_8;
		}
	else
		{
		vgFormat = colorDesc.vgFormat;
		}
    desc.m_colorDescriptor.m_format = (CColorDescriptor::ColorFormat)colorDesc.internalFormat;
    desc.m_alphaShift     = colorDesc.alphaShift;
    desc.m_blueShift      = colorDesc.blueShift;
    desc.m_greenShift     = colorDesc.greenShift;
    desc.m_redShift       = colorDesc.redShift;
    desc.m_luminanceShift = colorDesc.luminanceShift;
    desc.m_stride         = vgimage->getStride();

    bufSize = (desc.m_stride * desc.m_height);
    // Allocate data from memory.
    data = RI_NEW_ARRAY(OpenVGRI::RIuint8, bufSize);
    // Get data from VG
    vgGetImageSubData( (VGImage)vgimage, data, vgimage->getStride(), vgFormat, 0, 0, vgimage->getWidth(), vgimage->getWidth() );

    }

void EGLtoVGInterface::AddRef( void* image )
    {
    OpenVGRI::Image* vgimage = (OpenVGRI::Image*)image;
    if( !image )
        {
        return;
        }
    vgimage->addReference();
    }

void EGLtoVGInterface::RemoveRef( void* image )
    {
    OpenVGRI::Image* vgimage = (OpenVGRI::Image*)image;
    if( !image )
        {
        return;
        }
    vgimage->removeReference();
    }

/*static*/ IVGtoEGLInterface* EGLtoVGInterface::GetEGLInterface()
{
    return g_EGLtoVGInterface.m_egl;
}

void* OpenVGRI::eglvgGetCurrentVGContext(void)
{
    return EGLtoVGInterface::GetEGLInterface()->GetVGContext();
}

bool OpenVGRI::eglvgIsInUse(void* image)
{
    return EGLtoVGInterface::GetEGLInterface()->IsImageInUse(image);
}

bool OpenVGRI::eglvgLockSurface(bool read, bool write)
{
	return EGLtoVGInterface::GetEGLInterface()->LockVGSurface(read, write);
}

bool OpenVGRI::eglvgUnlockSurface()
{
	return EGLtoVGInterface::GetEGLInterface()->UnlockVGSurface();
}

void OpenVGRI::OSAcquireMutex(void)
{
}

void OpenVGRI::OSReleaseMutex(void) 
{
}

void OpenVGRI::eglvgGetImageDescriptor( void* image, Color::Descriptor &desc, int &width, int &height, int &stride )
    {
    SurfaceDescriptor surfDesc;
    EGLtoVGInterface::GetEGLInterface()->GetDescForImage( image, surfDesc );
    desc = EGLtoVGInterface::vgDescriptorFromSurfaceDescriptor( &surfDesc );
    width = surfDesc.m_width;
    height = surfDesc.m_height;
    stride = surfDesc.m_stride;
    }

void* OpenVGRI::eglvgGetImageData( void* image )
    {
    return EGLtoVGInterface::GetEGLInterface()->GetDataForImage( image );
    }
