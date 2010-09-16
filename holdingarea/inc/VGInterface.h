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

#ifndef _VGINTERFACE_H_
#define _VGINTERFACE_H_

#include "ImageTarget.h"

struct SurfaceDescriptor;
struct BufferContainer;
class CColorDescriptor;

typedef void (*fpVGProc)(void);

// EGL services for VG
class IVGtoEGLInterface
    {
public:
    // \todo Descriptions
    virtual void* GetVGContext() = 0;
    virtual bool IsImageInUse( void* image ) = 0;
    virtual bool LockVGSurface( bool read, bool write ) = 0;
    virtual bool UnlockVGSurface() = 0;
    virtual void GetDescForImage( void* image, SurfaceDescriptor& ImageDesc ) = 0;
    virtual void* GetDataForImage( void* image ) = 0;
	virtual void RegisterImageTarget( void* image, EImageTarget target, void* buffer ) = 0;
	virtual void UnregisterImageTarget( void* image, EImageTarget target, void* buffer ) = 0;
    };

// VG services for EGL
class IEGLtoVGInterface
    {
public:
    /* \brief Sets interface for EGL services to VG
     *
     * \param egl Pointer to IVGtoEGLInterface implementation.
     */
    virtual void SetEGLInterface( IVGtoEGLInterface* egl ) = 0;

    /* \brief Used to create internal VG context
     * 
     * \param shareContext  Context to be shared with created context
     *
     * \return  Handle to created context
     */
    virtual void* CreateContext( void* shareContext ) = 0;

    /* \brief Releases internal VG context
     * 
     * \param context   Context to be released
     *
     * \return  true if release succeeded false otherwise
     */
    virtual bool ReleaseContext( void* context ) = 0;

    /* \brief Creates internal VG surface
     * 
     * \param desc      Descriptor describing surface pixel format, size, stride etc.
     * \param buffers   Container for buffers. VG should fill in handle for surface, color buffer and mask buffer.
     * \param image     VG image handle used in surface creation (eglCreatePbufferFromClientBuffer()).
     *
     * \return true if surface creation succeeded false otherwise
     */
    virtual bool CreateSurface( const SurfaceDescriptor* desc, BufferContainer* buffers, void* image ) = 0;

    /* \brief Releases internal VG surface
     * 
     * \param surface   Surface to be released
     *
     * \return  true if surface release succeeded false otherwise
     */
    virtual bool ReleaseSurface( void* surface ) = 0; 

    /* \brief Sets current surface for internal VG context
     * 
     * \param context   Handle to internal VG context
     * \param surface   Handle to internal VG surface to be set current to context.
     *
     * \return  true if succeeded false otherwise
     */
    virtual bool SetCurrentSurface( void* context, void* surface ) = 0;

     /* \brief Resizes internal VG surface
     * 
     * \param context   Handle to internal VG context
     * \param surface   Handle to internal VG surface
     * \param width     New width
     * \param height    New height
     * \param buffers   Container for buffers. VG should fill in handle for surface, color buffer and mask buffer.
     *
     * \return  true if resize succeeded false otherwise
     */
    virtual bool ResizeSurface( void* context, void* surface, int width, int height, BufferContainer* buffers ) =0;

    /* \brief Checks if image handle is valid VGImage
     * 
     * \param image     Handle to VG image
     * \param colorDesc Color descriptor. VG should fill in descriptor describing image.
     * \param width     On return should contain widht of the VG image
     * \param height    On return should contain height of the VG image
     *
     * \return  true if image is valid false otherwise
     */
    virtual bool IsValidImage( void* image, CColorDescriptor* colorDesc, int* width, int* height ) = 0;

    /* \brief Checks if image handle is in use by VG
     *
     * param image      Handle to VG image

     * return true if image is in use false otherwise
     */
    virtual bool IsImageInUse( void* image ) = 0;

    /* \brief Flushes current vg context. Same operation as vgFlush().
     */
    virtual void Flush() = 0;

    /* \brief Finishes VG drawing. Same operation as vgFinish().
     */
    virtual void Finish() = 0;

    /* \brief Used to get address of vg extension function. Called by eglGetProcAddress().
     *
     * \param procname Name of the vg extension function
     *
     * \return  Address of the vg extension function or NULL if extension is not found.
     */
    virtual fpVGProc GetVGProcAddress( const char *procname ) = 0;

    /* \brief Copy VG color buffer into buffer.
     * 
     * \param buffer Buffer to copy data from VG color buffer
     * \param stride Buffer stride
     * \param surface Handle to VG internal surface to copy color buffer from.
     * 
     * \return void
     */
    virtual void CopyBuffers( void* buffer, int stride, void* surface ) = 0;

    /* \brief Updates VG color buffer
     *
     * \param buffer Buffer to copy data from
     * \param stride Stride of buffer
     * \param desc   Surface descriptor describing buffer.
     *
     * \return void
     */
    virtual void UpdateBuffers( void* buffer, int stride, const SurfaceDescriptor* desc ) = 0;

    /* \brief Checks if VG image is root image
     *
     * \param image Handle to VG image
     *
     * \return True if image is root, false otherwise
     */
    virtual bool IsRootImage( void* image ) = 0;

    /* \brief Copies data from VG image
     * 
     * \param image Handle to VG image
     * \param desc  Descriptor describing VG image. Function should fill descriptor.
     * \param data  Pointer to copy data to.
     * 
     * \return void
     */
    virtual void GetImageData( void* image, SurfaceDescriptor& desc, void* data ) = 0;

    /* \brief Add(+1) VG image reference count when EGLimage is created from VGImage
     * 
     * \param image Handle to VG image
     * 
     * \return void
     */
    virtual void AddRef( void* image ) = 0;

    /* \brief Remove(-1) VG image reference count when EGLimage deleted
     * 
     * \param image Handle to VG image
     * 
     * \return void
     */
    virtual void RemoveRef( void* image ) = 0;
    };

typedef IEGLtoVGInterface* (*fpGetVGInterface)(void);
IEGLtoVGInterface* LoadVGInterface( void*& libraryHandle );

#endif // _VGINTERFACE_H_

