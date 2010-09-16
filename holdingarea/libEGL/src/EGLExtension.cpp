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

#include "EGLExtension.h"
#include "EGLState.h"
#include "EGLImage.h"
#include "EGLContext.h"
#include "EGLDisplay.h"
#include "eglInternal.h"
#include "EGLProcess.h"
#include "EGLThread.h"
#include "EGLOs.h"
#include "ImageTarget.h"

#include <string.h>

typedef struct
	{
	const char*      extnName;		// procedure or extension name
	ProcPointer      procAddr;      // address
	} TExtnInfo;

static const TExtnInfo eglProcedures[] =
    { // list of EGL extension name strings, followed by any EGLI_NEW function names
        {"eglCreateImageKHR", (ProcPointer)eglCreateImageKHR},
        {"eglDestroyImageKHR", (ProcPointer)eglDestroyImageKHR}
    };

const EGLint KEglProcCount = sizeof(eglProcedures) / sizeof(TExtnInfo); 

CEGLExtension::CEGLExtension()
    {
    }

CEGLExtension::~CEGLExtension(void)
    {
    }

ProcPointer CEGLExtension::eglGetProcAddress(const char* aProcname)
    {
    EGLint nameLength =  strlen(aProcname);
	if ((nameLength > 3) && !strncmp(aProcname, "egl", 3))
		{ // EGL extensions - exhaustive search 
		for (EGLint idx = 0; idx < KEglProcCount; idx++)
			{
			if ( strcmp( aProcname, eglProcedures[idx].extnName ) == 0 )
				return eglProcedures[idx].procAddr;
			}
		}	
	return NULL;
    }

static EImageTarget mapImageTarget( EGLenum target )
	{
	switch(target)
		{
		case EGL_GL_TEXTURE_2D_KHR:
			return IMAGE_TARGET_TEXTURE_2D;
		case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X_KHR:
			return IMAGE_TARGET_TEXTURE_CUBE_MAP_POSITIVE_X;
        case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X_KHR:
			return IMAGE_TARGET_TEXTURE_CUBE_MAP_NEGATIVE_X;
        case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y_KHR:
			return IMAGE_TARGET_TEXTURE_CUBE_MAP_POSITIVE_Y;
        case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_KHR:
			return IMAGE_TARGET_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z_KHR:
			return IMAGE_TARGET_TEXTURE_CUBE_MAP_POSITIVE_Z;
        case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_KHR:
			return IMAGE_TARGET_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		case EGL_GL_RENDERBUFFER_KHR:
			return IMAGE_TARGET_RENDERBUFFER;
		default:
			EGLI_ASSERT(false);
		}

	// Not reached.
	return (EImageTarget)-1;
	}

EGLAPI EGLImageKHR EGLAPIENTRY eglCreateImageKHR( EGLDisplay dpy,
												  EGLContext ctx,
												  EGLenum target,
												  EGLClientBuffer buffer,
												  const EGLint *attrib_list )
    {
    EGLI_ENTER_RET( EGL_NO_IMAGE_KHR );

    CEGLImage *image = NULL;
    CEGLDisplay *display = NULL;
    CEGLContext *context = NULL;

    display = state->GetDisplay( dpy );
    // Check display
    //<dpy> must be a valid EGLDisplay
    if( !display )
       {                   
       EGLI_LEAVE_RET( EGL_NO_IMAGE_KHR, EGL_BAD_DISPLAY );
       }

    context = display->GetContext( ctx ); 
    // Check context
    //<ctx> must be a valid OpenVG API context on that display
    if( !context )
       {                    
       EGLI_LEAVE_RET( EGL_NO_IMAGE_KHR, EGL_BAD_CONTEXT );
       }
    
    // Display and context ok, continue...
    switch( target )
        {
        case EGL_NATIVE_PIXMAP_KHR:
            {
            // Not supported
            // 1) set error code to EGL_BAD_PARAMETER
            // 2) release lock
            // 3) return no image
            EGLI_LEAVE_RET( EGL_NO_IMAGE_KHR, EGL_BAD_PARAMETER );
            break;
            }
        case EGL_VG_PARENT_IMAGE_KHR:
            {            
            // Context must be client context and api must be openVG
            if( context->ClientContext() && EGL_OPENVG_API == context->ClientApi() )                    
                {                        
                EGLI_LEAVE_RET( EGL_NO_IMAGE_KHR, EGL_BAD_CONTEXT );
                }

            //<buffer>
            //must be a handle of a VGImage object valid in the specified context, cast
            //into the type EGLClientBuffer
            //Furthermore, the specified VGImage
            //<buffer> must not be a child image
            if ( !state->VGInterface()->IsRootImage( buffer ) )
                {
                EGLI_LEAVE_RET( EGL_NO_IMAGE_KHR, EGL_BAD_ACCESS );
                }
            
            // TODO:
            /*
            1) We also need to check that VGImage is not bound 
            to a Pbuffer surface
            
            2) VGimage has not been already used to create EGLImage. 
            
            3) VGImage itself is not created from EGLImage.            
            */
            //state->VGInterface()-
            // 1)
            if( EGL_PBUFFER_BIT == context->Config()->GetAttribute( EGL_SURFACE_TYPE ) )
                {
                // If VGImage is bound to a Pbuffer no EGLImage
                EGLI_LEAVE_RET( EGL_NO_IMAGE_KHR, EGL_BAD_ACCESS );
                }

            // Descriptor for pixeldata data values
            SurfaceDescriptor desc; 
            // data is actual pixeldata
            void* data = NULL;

            //Any values
            //specified in <attr_list> are ignored.
     
            // GetImageData will allocate memory for data
            // data must be deleted in CEGLImage destructor
            state->VGInterface()->GetImageData( buffer, desc, data );
            if ( !data )
                {
                EGLI_LEAVE_RET( EGL_NO_IMAGE_KHR, EGL_BAD_ACCESS );
                }

            // Create CEGLImage.
            image = EGLI_NEW CEGLImage( target, buffer, desc, data );
            
            // If EGLImage creation fail then return error code.
            if( !image )
                {
                EGLI_LEAVE_RET( EGL_NO_IMAGE_KHR, EGL_BAD_ALLOC );
                }
            // Add refcount to VGImage
            state->VGInterface()->AddRef( buffer );
            // Add image in array
            image = display->AddEGLImage( image );
            break;
            }   

        case EGL_GL_TEXTURE_2D_KHR:
		case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X_KHR:
        case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X_KHR:
        case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y_KHR:
        case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_KHR:
        case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z_KHR:
        case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_KHR:
            {
            // From RFP document
            // EGL_KHR_gl_image (not required for OpenGL ES 1.1) [R12]
            /*<ctx> must be a valid OpenGL ES API context on that display.*/
			if( context->ClientContext() && (EGL_OPENGL_ES_API != context->ClientApi() || 2 != context->ClientVersion()) )
                {                        
                EGLI_LEAVE_RET( EGL_NO_IMAGE_KHR, EGL_BAD_CONTEXT );
                }

            
            // From spec.
            /*KHR_gl_texture_2D_image requires the presence of an OpenGL ES
            implementation (1.0 or later and/or 2.0 or later).*/
            
            /*Accepted as an attribute in the <attr_list> parameter of
            eglCreateImageKHR: EGL_GL_TEXTURE_LEVEL_KHR 0x30BC*/

            //EGL_GL_TEXTURE_LEVEL_KHR  

            /*If <target> is EGL_GL_TEXTURE_2D_KHR, 
            <buffer> must be the name of a nonzero, EGL_GL_TEXTURE_2D target texture object, 
            cast into the type EGLClientBuffer.  

            <attr_list> should specify the mipmap level
            which will be used as the EGLImage source (EGL_GL_TEXTURE_LEVEL_KHR); the
            specified mipmap level must be part of <buffer>.  If not specified, the
            default value listed in Table bbb will be used, instead.  Additional
            values specified in <attr_list> are ignored.  The texture must be complete
            unless the mipmap level to be used is 0, the texture has mipmap level 0
            specified, and no other mipmap levels are specified. */
            
            SurfaceDescriptor surfDesc;
            EGLint textureLevel = 0;
            EGLint dataSize;

            if( attrib_list && attrib_list[0] != EGL_NONE )
	            {
                  for(int i=0; attrib_list[i] != EGL_NONE ; i+=2)
                    {
                    if( attrib_list[i] == EGL_GL_TEXTURE_LEVEL_KHR )
                        {
                        textureLevel = attrib_list[i+1];
                        }
                    }
	            }

			EImageTarget textureTarget = mapImageTarget(target);

			IEGLtoGLES2Interface* iFace = (IEGLtoGLES2Interface*)state->GLESInterface(2);
			EStatus status = iFace->GetTextureInfo( context->ClientContext(), textureTarget, buffer, textureLevel, surfDesc );
			if ( status != DGLES2_SUCCESS )
				{
				switch( status )
					{
					case DGLES2_BAD_PARAMETER:
					case DGLES2_BAD_MATCH:
						EGLI_LEAVE_RET( EGL_NO_IMAGE_KHR, EGL_BAD_PARAMETER );
						break;

					default:
						EGLI_ASSERT(false);
					}
                EGLI_LEAVE_RET( EGL_NO_IMAGE_KHR, EGL_BAD_ALLOC );
                }
			dataSize = surfDesc.m_stride * surfDesc.m_height;
			unsigned char* data = new unsigned char [ dataSize ];

            iFace->GetTextureData( context->ClientContext(), textureTarget, buffer, textureLevel, data );
            
            // Create CEGLImage.
            image = EGLI_NEW CEGLImage( target, buffer, surfDesc, data );
            
            // If EGLImage creation fail then return error code.
            if( !image )
                {
                EGLI_LEAVE_RET( EGL_NO_IMAGE_KHR, EGL_BAD_ALLOC );
                }
            // Add image in array
            // That array is in display side.
            image = display->AddEGLImage( image );
            break;
            }
        case EGL_GL_TEXTURE_3D_KHR:
            {
            /*<ctx> must be a valid OpenGL ES API context on that display.*/
			//EGL_BAD_PARAMETER 
            // NOT SUPPORTED 
            EGLI_LEAVE_RET( EGL_NO_IMAGE_KHR, EGL_BAD_PARAMETER );
            }
        case EGL_GL_RENDERBUFFER_KHR:
            {
            //EGL_BAD_PARAMETER 
            // NOT SUPPORTED 
            EGLI_LEAVE_RET( EGL_NO_IMAGE_KHR, EGL_BAD_PARAMETER );
            }

        default:
            {
            //EGL_BAD_PARAMETER 
            EGLI_LEAVE_RET( EGL_NO_IMAGE_KHR, EGL_BAD_PARAMETER );
            }
        }
    EGLI_LEAVE_RET( image, EGL_SUCCESS );
    }

EGLAPI EGLBoolean EGLAPIENTRY eglDestroyImageKHR( EGLDisplay dpy,
												  EGLImageKHR image)
    {
    // TODO:
    /* Check display
       Delete image from display array*/ 
    EGLI_ENTER_RET( EGL_FALSE );

    CEGLDisplay *display = NULL;
    CEGLImage *tempImage = NULL;

    display = state->GetDisplay( dpy );
    // Check display
    //<dpy> must be a valid EGLDisplay
    if( !display )
       {                   
       EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_DISPLAY );
       }

    tempImage = display->GetImage( image );
    if( !tempImage )
        {
        EGLI_LEAVE_RET( EGL_FALSE, EGL_BAD_PARAMETER );
        }

    // Display and image are both ok...
    // Let siblings know that image is destroyed...
    // Delete mother of image...
    // Remove image...
    //tempImage
    display->RemoveEGLImage( image );

    return EGL_TRUE;
    }

