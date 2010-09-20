// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implementation of Guest OpenGL ES 1.1 serialization stubs

#include "remotefunctioncall.h"
#include "opengles11rfc.h"
#include "vghwutils.h"
#include "opengl.h"

// Writeable Static Data, is initialised to 0s by the compiler
vertexArrayInfo vertexArrays[NUM_ARRAYS];
bool vertexArraysEnabled[NUM_ARRAYS];


// Writeable Static Data - causes constructor to be called at DLL load time.
TGles11ApiForEgl GlesApiExporter;



// Informative Panic messages
_LIT(KGlesPanicCategory, "Guest GLES");

void GlesPanic(TGlPanic aPanicCode, char* aCondition, char* aFile, TInt aLine)
	{
	if (aCondition && aFile)
		{
		RDebug::Printf("Guest Open GLES DLL Panic %d for failed Assert (%s), at %s:%d", aPanicCode, aCondition, aFile, aLine);
		}
	else
		{  
		RDebug::Printf("Guest Open GLES DLL Panic %d for failed Assert (line %d)", aPanicCode, aLine);
		}
		
	User::Panic(KGlesPanicCategory, aPanicCode);
	}


inline void SetError(GLint aError)
	{
	MGlesContext* glesContext = CVghwUtils::GlesContext();
	if (glesContext)
		{ // there is a GL ES context  
		glesContext->SetGlesError(aError);
		}
	}

inline void ExecuteCommand(RemoteFunctionCall& aRequestData)
	{
	MGlesContext* glesContext = CVghwUtils::GlesContext();
	if (glesContext)
		{ // there is a GL ES context  
		glesContext->ExecuteGlesCommand(aRequestData);
		}
    }

inline TUint32 ExecuteFunction(RemoteFunctionCall& aRequestData)
	{
	GLPANIC_ASSERT_DEBUG(aRequestData.Data().Header().iOpType == RemoteFunctionCallData::EOpRequestWithReply, EGlPanicNotReplyOpcode);
	MGlesContext* glesContext = CVghwUtils::GlesContext();
	if (glesContext)
		{ // there is GL ES context  
		glesContext->ExecuteGlesCommand(aRequestData);
		return aRequestData.ReturnValue();
		}
	return 0;
    }


extern "C" {

EXPORT_C void GL_APIENTRY glActiveTexture (GLenum texture)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glActiveTexture ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglActiveTexture, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( texture );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glActiveTexture <-\n");
	}

EXPORT_C void GL_APIENTRY glAlphaFunc (GLenum func, GLclampf ref) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glAlphaFunc ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglAlphaFunc, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( func );
    call.AppendParam( ref );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glAlphaFunc <-\n");
	}

EXPORT_C void GL_APIENTRY glAlphaFuncx (GLenum func, GLclampx ref)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glAlphaFuncx ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglAlphaFuncx, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( func );
    call.AppendParam( ref );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glAlphaFuncx <-\n");
	}

EXPORT_C void GL_APIENTRY glBindBuffer (GLenum target, GLuint buffer)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glBindBuffer ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglBindBuffer, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( buffer );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glBindBuffer <-\n");
	}

EXPORT_C void GL_APIENTRY glBindTexture (GLenum target, GLuint texture)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glBindTexture ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglBindTexture, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( texture );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glBindTexture <-\n");
	}

EXPORT_C void GL_APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glBlendFunc ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglBlendFunc, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( sfactor );
    call.AppendParam( dfactor );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glBlendFunc <-\n");
	}

EXPORT_C void GL_APIENTRY glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glBufferData ->\n");
    if(size < 0) // cannot serialize vector with size < 0
    	{
        SetError( GL_INVALID_VALUE );
        return;
    	}
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglBufferData, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( size );
    call.AppendVector( data, size );
    call.AppendParam( usage );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glBufferData <-\n");
	}

EXPORT_C void GL_APIENTRY glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glBufferSubData ->\n");
    if(offset < 0 || size < 0) // cannot serialize vector with size < 0
    	{
        SetError( GL_INVALID_VALUE );
        return;
    	}
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglBufferSubData, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( offset );
    call.AppendParam( size );
    call.AppendVector( data, offset+size );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glBufferSubData <-\n");
	}

EXPORT_C void GL_APIENTRY glClear (GLbitfield mask)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClear ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglClear, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( mask );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClear <-\n");
	}

EXPORT_C void GL_APIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClearColor ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglClearColor, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( red );
    call.AppendParam( green );
    call.AppendParam( blue );
    call.AppendParam( alpha );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClearColor <-\n");
	}

EXPORT_C void GL_APIENTRY glClearColorx (GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClearColorx ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglClearColorx, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( red );
    call.AppendParam( green );
    call.AppendParam( blue );
    call.AppendParam( alpha );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClearColorx <-\n");
	}

EXPORT_C void GL_APIENTRY glClearDepthf (GLclampf depth) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClearDepthf ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglClearDepthf, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( depth );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClearDepthf <-\n");
	}

EXPORT_C void GL_APIENTRY glClearDepthx (GLclampx depth)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClearDepthx ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglClearDepthx, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( depth );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClearDepthx <-\n");
	}

EXPORT_C void GL_APIENTRY glClearStencil (GLint s)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClearStencil ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglClearStencil, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( s );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClearStencil <-\n");
	}

EXPORT_C void GL_APIENTRY glClientActiveTexture (GLenum texture)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClientActiveTexture ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglClientActiveTexture, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( texture );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClientActiveTexture <-\n");
	}

EXPORT_C void GL_APIENTRY glClipPlanef (GLenum plane, const GLfloat *equation)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClipPlanef ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglClipPlanef, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( plane );
    call.AppendVector( equation, 4 ); // 4 values according to OpenGL_ES_1.1.12.pdf, section 2.11
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClipPlanef <-\n");
	}

EXPORT_C void GL_APIENTRY glClipPlanex (GLenum plane, const GLfixed *equation)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClipPlanex ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglClipPlanex, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( plane );
    call.AppendVector( equation, 4 ); // 4 values according to OpenGL_ES_1.1.12.pdf, section 2.11
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glClipPlanex <-\n");
	}

EXPORT_C void GL_APIENTRY glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glColor4f ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglColor4f, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( red );
    call.AppendParam( green );
    call.AppendParam( blue );
    call.AppendParam( alpha );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glColor4f <-\n");
	}

EXPORT_C void GL_APIENTRY glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glColor4ub ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglColor4ub, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( red );
    call.AppendParam( green );
    call.AppendParam( blue );
    call.AppendParam( alpha );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glColor4ub <-\n");
	}

EXPORT_C void GL_APIENTRY glColor4x (GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glColor4x ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglColor4x, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( red );
    call.AppendParam( green );
    call.AppendParam( blue );
    call.AppendParam( alpha );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glColor4x <-\n");
	}

EXPORT_C void GL_APIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glColorMask ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglColorMask, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( red );
    call.AppendParam( green );
    call.AppendParam( blue );
    call.AppendParam( alpha );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glColorMask <-\n");
	}

EXPORT_C void GL_APIENTRY glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glColorPointer ->\n");
    vertexArrays[COLOR_ARRAY].size = size;
    vertexArrays[COLOR_ARRAY].type = type;
    vertexArrays[COLOR_ARRAY].stride = stride;
    vertexArrays[COLOR_ARRAY].pointer = pointer;
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glColorPointer <-\n");
	}

EXPORT_C void GL_APIENTRY glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glCompressedTexImage2D ->\n");
    if(imageSize < 0) // cannot serialize vector with size < 0
    	{
        SetError( GL_INVALID_VALUE );
        return;
    	}
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglCompressedTexImage2D, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( level );
    call.AppendParam( internalformat );
    call.AppendParam( width );
    call.AppendParam( height );
    call.AppendParam( border );
    call.AppendParam( imageSize );
    call.AppendVector( data, imageSize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glCompressedTexImage2D <-\n");
	}

EXPORT_C void GL_APIENTRY glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glCompressedTexSubImage2D ->\n");
    if(imageSize < 0) // cannot serialize vector with size < 0
    	{
        SetError( GL_INVALID_VALUE );
        return;
    	}
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglCompressedTexSubImage2D, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( level );
    call.AppendParam( xoffset );
    call.AppendParam( yoffset );
    call.AppendParam( width );
    call.AppendParam( height );
    call.AppendParam( format );
    call.AppendParam( imageSize );
    call.AppendVector( data, imageSize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glCompressedTexSubImage2D <-\n");
	}

EXPORT_C void GL_APIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glCopyTexImage2D ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglCopyTexImage2D, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( level );
    call.AppendParam( internalformat );
    call.AppendParam( x );
    call.AppendParam( y );
    call.AppendParam( width );
    call.AppendParam( height );
    call.AppendParam( border );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glCopyTexImage2D <-\n");
	}

EXPORT_C void GL_APIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glCopyTexSubImage2D ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglCopyTexSubImage2D, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( level );
    call.AppendParam( xoffset );
    call.AppendParam( yoffset );
    call.AppendParam( x );
    call.AppendParam( y );
    call.AppendParam( width );
    call.AppendParam( height );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glCopyTexSubImage2D <-\n");
	}

EXPORT_C void GL_APIENTRY glCullFace (GLenum mode)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glCullFace ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglCullFace, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( mode );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glCullFace <-\n");
	}

EXPORT_C void GL_APIENTRY glDeleteBuffers (GLsizei n, const GLuint *buffers)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDeleteBuffers ->\n");
    if(n < 0) // cannot serialize vector with size < 0
    	{
        SetError( GL_INVALID_VALUE );
        return;
		}
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDeleteBuffers, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( n );
    call.AppendVector( buffers, n );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDeleteBuffers <-\n");
	}

EXPORT_C void GL_APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDeleteTextures ->\n");
    if(n < 0) // cannot serialize vector with size < 0
    	{
        SetError( GL_INVALID_VALUE );
        return;
		}
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDeleteTextures, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( n );
    call.AppendVector( textures, n );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDeleteTextures <-\n");
	}

EXPORT_C void GL_APIENTRY glDepthFunc (GLenum func)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDepthFunc ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDepthFunc, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( func );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDepthFunc <-\n");
	}

EXPORT_C void GL_APIENTRY glDepthMask (GLboolean flag)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDepthMask ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDepthMask, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( flag );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDepthMask <-\n");
	}

EXPORT_C void GL_APIENTRY glDepthRangef (GLclampf zNear, GLclampf zFar) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDepthRangef ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDepthRangef, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( zNear );
    call.AppendParam( zFar );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDepthRangef <-\n");
	}

EXPORT_C void GL_APIENTRY glDepthRangex (GLclampx zNear, GLclampx zFar)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDepthRangex ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDepthRangex, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( zNear );
    call.AppendParam( zFar );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDepthRangex <-\n");
	}

EXPORT_C void GL_APIENTRY glDisable (GLenum cap)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDisable ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDisable, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( cap );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDisable <-\n");
	}

EXPORT_C void GL_APIENTRY glDisableClientState (GLenum array)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDisableClientState ->\n");
    // store state on here as well, used in DrawArrays and DrawElements
    bool enumError = false;
    switch( array )
    	{
        case GL_COLOR_ARRAY:
            vertexArraysEnabled[COLOR_ARRAY] = false;
            break;
        case GL_NORMAL_ARRAY:
            vertexArraysEnabled[NORMAL_ARRAY] = false;
            break;
        case GL_POINT_SIZE_ARRAY_OES:
            vertexArraysEnabled[POINT_SIZE_ARRAY] = false;
            break;
        case GL_TEXTURE_COORD_ARRAY:
            vertexArraysEnabled[TEX_COORD_ARRAY] = false;
            break;
        case GL_VERTEX_ARRAY:
            vertexArraysEnabled[VERTEX_ARRAY] = false;
            break;
        case GL_MATRIX_INDEX_ARRAY_OES:
            vertexArraysEnabled[MATRIX_INDEX_ARRAY] = false;
            break;
        case GL_WEIGHT_ARRAY_OES:
            vertexArraysEnabled[WEIGHT_ARRAY] = false;
            break;
        default:
            SetError( GL_INVALID_ENUM );
            enumError = true;
            break;
    	}
    if( !enumError )
    	{
        RemoteFunctionCallData rfcData;
        OpenGlES11RFC call( rfcData );
        call.Init( OpenGlES11RFC::EglDisableClientState, RemoteFunctionCallData::EOpRequest );
        call.AppendParam( array );
        ExecuteCommand(call);
    	}
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDisableClientState <-\n");
	}

EXPORT_C void GL_APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawArrays ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDrawArrays, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( mode );
    call.AppendParam( first );
    call.AppendParam( count );

    /* serialize vertex arrays
       order of the data:
       - number of arrays (int)
       - bitfield of enabled arrays, order specified by opengl.h vertex array pointer enumeration (int)
       - for each enabled vertex array:
         - size (int)
         - type (int)
         - stride (int)
         - data vector
    */
    int arrayCount = 0;
    int arrayEnabled = 0;
    for(int i=0; i<NUM_ARRAYS; i++)
    {
        arrayEnabled = (arrayEnabled << 1) + vertexArraysEnabled[i];
        if( vertexArraysEnabled[i] )
            arrayCount++;
    }
    call.AppendParam( arrayCount );
    call.AppendParam( arrayEnabled );
    for(int i=0; i<NUM_ARRAYS; i++)
    	{
		if( vertexArraysEnabled[i] )
			{
            call.AppendParam( vertexArrays[i].size );
            call.AppendParam( vertexArrays[i].type );
            call.AppendParam( vertexArrays[i].stride );
            int vectorsize;
            if( vertexArrays[i].stride )
                vectorsize = vertexArrays[i].stride * (first + count - 1) + sizeof(vertexArrays[i].type) * vertexArrays[i].size;
            else 
                vectorsize = sizeof(vertexArrays[i].type) * vertexArrays[i].size * (first + count);
            call.AppendVector( vertexArrays[i].pointer, vectorsize ); 
			}
    	}

    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawArrays <-\n");
	}

EXPORT_C void GL_APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawElements ->\n");
    if(count < 0) // cannot serialize vector with size < 0
    	{
        SetError( GL_INVALID_VALUE );
        return;
    	}
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDrawElements, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( mode );
    call.AppendParam( count );
    call.AppendParam( type );
    call.AppendVector( indices, count );
    
    /* serialize vertex arrays
       order of the data:
       - number of arrays (int)
       - bitfield of enabled arrays, order specified by opengl.h vertex array pointer enumeration (int)
       - for each enabled vertex array:
         - size (int)
         - type (int)
         - stride (int)
         - data vector
    */
    int arrayCount = 0;
    int arrayEnabled = 0;
    for(int i=0; i<NUM_ARRAYS; i++)
    	{
        arrayEnabled = (arrayEnabled << 1) + vertexArraysEnabled[i];
        if( vertexArraysEnabled[i] )
            arrayCount++;
		}
    call.AppendParam( arrayCount );
    call.AppendParam( arrayEnabled );
    // find max index
    int maxIndex = 0;
    switch( type )
    	{
        case GL_UNSIGNED_BYTE:
            for(int i=0; i<count; i++)
            	{
                if( ((GLubyte*)indices)[i] > maxIndex )
                	maxIndex = (int)((GLubyte*)indices)[i];
				}
            break;
        case GL_UNSIGNED_SHORT:
            for(int i=0; i<count; i++)
            	{
                if( ((GLushort*)indices)[i] > maxIndex )
                    maxIndex = (int)((GLushort*)indices)[i];
            	}
            break;
    	}
    for(int i=0; i<NUM_ARRAYS; i++)
    	{
        if( vertexArraysEnabled[i] )
        	{
            call.AppendParam( vertexArrays[i].size );
            call.AppendParam( vertexArrays[i].type );
            call.AppendParam( vertexArrays[i].stride );
            int vectorsize;
            int typesize = 0;
            switch(vertexArrays[i].type)
            	{
                case GL_BYTE:
                case GL_UNSIGNED_BYTE:
                    typesize = 1;
                    break;
                case GL_SHORT:
                case GL_UNSIGNED_SHORT:
                    typesize = 2;
                    break;
                case GL_FLOAT:
                case GL_FIXED:
                    typesize = 4;
                    break;
            	}
            if( vertexArrays[i].stride )
                vectorsize = vertexArrays[i].stride * maxIndex + typesize * vertexArrays[i].size;
            else 
                vectorsize = typesize * vertexArrays[i].size * (maxIndex+1);
            call.AppendVector( vertexArrays[i].pointer, vectorsize );
        	}
    	}
    
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawElements <-\n");
	}

EXPORT_C void GL_APIENTRY glEnable (GLenum cap)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glEnable ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglEnable, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( cap );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glEnable <-\n");
	}

EXPORT_C void GL_APIENTRY glEnableClientState (GLenum array)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glEnableClientState ->\n");
    // store state on here as well, used in DrawArrays and DrawElements
    bool enumError = false;
    switch( array )
    	{
        case GL_COLOR_ARRAY:
            vertexArraysEnabled[COLOR_ARRAY] = true;
            break;
        case GL_NORMAL_ARRAY:
            vertexArraysEnabled[NORMAL_ARRAY] = true;
            break;
        case GL_POINT_SIZE_ARRAY_OES:
            vertexArraysEnabled[POINT_SIZE_ARRAY] = true;
            break;
        case GL_TEXTURE_COORD_ARRAY:
            vertexArraysEnabled[TEX_COORD_ARRAY] = true;
            break;
        case GL_VERTEX_ARRAY:
            vertexArraysEnabled[VERTEX_ARRAY] = true;
            break;
        case GL_MATRIX_INDEX_ARRAY_OES:
            vertexArraysEnabled[MATRIX_INDEX_ARRAY] = true;
            break;
        case GL_WEIGHT_ARRAY_OES:
            vertexArraysEnabled[WEIGHT_ARRAY] = true;
            break;
        default:
            SetError( GL_INVALID_ENUM );
            enumError = true;
            break;
    	}
    if( !enumError )
    	{
        RemoteFunctionCallData rfcData;
        OpenGlES11RFC call( rfcData );
        call.Init( OpenGlES11RFC::EglEnableClientState, RemoteFunctionCallData::EOpRequest );
        call.AppendParam( array );
        ExecuteCommand(call);
    	}
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glEnableClientState <-\n");
	}

EXPORT_C void GL_APIENTRY glFinish (void)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFinish ->\n");
    MGlesContext* glesContext = CVghwUtils::GlesContext();
	if (glesContext)
		{ // there is GL ES context  
		glesContext->ExecuteGlesFinishCommand();
		}
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFinish <-\n");
	}

EXPORT_C void GL_APIENTRY glFlush (void)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFlush ->\n");
    MGlesContext* glesContext = CVghwUtils::GlesContext();
	if (glesContext)
		{ // there is GL ES context  
		glesContext->ExecuteGlesFlushCommand();
		}
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFlush <-\n");
	}

EXPORT_C void GL_APIENTRY glFogf (GLenum pname, GLfloat param) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFogf ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglFogf, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFogf <-\n");
	}

EXPORT_C void GL_APIENTRY glFogfv (GLenum pname, const GLfloat *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFogfv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_FOG_COLOR )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglFogfv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFogfv <-\n");
	}

EXPORT_C void GL_APIENTRY glFogx (GLenum pname, GLfixed param)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFogx ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglFogx, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFogx <-\n");
	}

EXPORT_C void GL_APIENTRY glFogxv (GLenum pname, const GLfixed *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFogxv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_FOG_COLOR )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglFogxv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFogxv <-\n");
	}

EXPORT_C void GL_APIENTRY glFrontFace (GLenum mode)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFrontFace ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglFrontFace, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( mode );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFrontFace <-\n");
	}

EXPORT_C void GL_APIENTRY glFrustumf (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFrustumf ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglFrustumf, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( left );
    call.AppendParam( right );
    call.AppendParam( bottom );
    call.AppendParam( top );
    call.AppendParam( zNear );
    call.AppendParam( zFar );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFrustumf <-\n");
	}

EXPORT_C void GL_APIENTRY glFrustumx (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFrustumx ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglFrustumx, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( left );
    call.AppendParam( right );
    call.AppendParam( bottom );
    call.AppendParam( top );
    call.AppendParam( zNear );
    call.AppendParam( zFar );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glFrustumx <-\n");
}

EXPORT_C void GL_APIENTRY glGenBuffers (GLsizei n, GLuint *buffers)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGenBuffers ->\n");
    if(n < 0)
    	{
        SetError( GL_INVALID_VALUE );
        return;
    	}
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGenBuffers );
    call.AppendParam( n );
    call.AppendVector( buffers, n, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGenBuffers <-\n");
	}

EXPORT_C void GL_APIENTRY glGenTextures (GLsizei n, GLuint *textures)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGenTextures ->\n");
    if(n < 0) // cannot serialize vector with size < 0
    	{
        SetError( GL_INVALID_VALUE );
        return;
    	}
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGenTextures );
    call.AppendParam( n );
    call.AppendVector( textures, n, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGenTextures <-\n");
	}

EXPORT_C void GL_APIENTRY glGetBooleanv (GLenum pname, GLboolean *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetBooleanv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_COLOR_WRITEMASK )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetBooleanv );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetBooleanv <-\n");
	}

EXPORT_C void GL_APIENTRY glGetBufferParameteriv (GLenum target, GLenum pname, GLint *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetBufferParameteriv ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetBufferParameteriv );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendVector( params, 1, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetBufferParameteriv <-\n");
	}

EXPORT_C void GL_APIENTRY glGetClipPlanef (GLenum pname, GLfloat eqn[4])
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetClipPlanef ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetClipPlanef );
    call.AppendParam( pname );
    call.AppendVector( eqn, 4, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetClipPlanef <-\n");
	}

EXPORT_C void GL_APIENTRY glGetClipPlanex (GLenum pname, GLfixed eqn[4])
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetClipPlanex ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetClipPlanex );
    call.AppendParam( pname );
    call.AppendVector( eqn, 4, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetClipPlanex <-\n");
	}

EXPORT_C GLenum GL_APIENTRY glGetError (void)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetError ->\n");
    GLenum gles11Error = GL_NO_ERROR;
    MGlesContext* glesContext = CVghwUtils::GlesContext();
    if (glesContext)
    	{
		gles11Error = glesContext->GlesError();
    	}
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetError <- %x\n", gles11Error);
    return gles11Error;
	}

inline TInt32 getGetterVectorSize(GLenum pname)
	{
    // oglGetterData.inl, s_getterInfos struct
    TInt32 vectorsize;
    switch( pname )
    	{
        case GL_SMOOTH_POINT_SIZE_RANGE:
        case GL_SMOOTH_LINE_WIDTH_RANGE:
        case GL_DEPTH_RANGE:
        case GL_MAX_VIEWPORT_DIMS:
        case GL_ALIASED_POINT_SIZE_RANGE:
        case GL_ALIASED_LINE_WIDTH_RANGE:
            vectorsize = 2;
            break;
        case GL_CURRENT_NORMAL:
        case GL_POINT_DISTANCE_ATTENUATION:
            vectorsize = 3;
            break;
        case GL_CURRENT_COLOR:
        case GL_CURRENT_TEXTURE_COORDS:
        case GL_LIGHT_MODEL_AMBIENT:
        case GL_FOG_COLOR:
        case GL_VIEWPORT:
        case GL_SCISSOR_BOX:
        case GL_COLOR_CLEAR_VALUE:
        case GL_COLOR_WRITEMASK:
            vectorsize = 4;
            break;
        case GL_COMPRESSED_TEXTURE_FORMATS:
            vectorsize = 10;
            break;
        case GL_MODELVIEW_MATRIX:
        case GL_PROJECTION_MATRIX:
        case GL_TEXTURE_MATRIX:
            vectorsize = 16;
            break;
        default:
            vectorsize = 1;
            break;
    	}
    return vectorsize;
	}

EXPORT_C void GL_APIENTRY glGetFixedv (GLenum pname, GLfixed *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetFixedv ->\n");
    TInt32 vectorsize = getGetterVectorSize(pname);
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetFixedv );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetFixedv <-\n");
	}

EXPORT_C void GL_APIENTRY glGetFloatv (GLenum pname, GLfloat *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetFloatv ->\n");
    TInt32 vectorsize = getGetterVectorSize(pname);
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetFloatv );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetFloatv <-\n");
	}

EXPORT_C void GL_APIENTRY glGetIntegerv (GLenum pname, GLint *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetIntegerv ->\n");
    TInt32 vectorsize = getGetterVectorSize(pname);
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetIntegerv );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetIntegerv <-\n");
	}

inline TInt32 getLightVectorSize(GLenum pname)
	{
    // oglApi.inl, oglGetLightInternal()
    TInt32 vectorsize;
    switch( pname )
    	{
        case GL_SPOT_DIRECTION:
            vectorsize = 3;
            break;
        case GL_AMBIENT:
        case GL_DIFFUSE:
        case GL_SPECULAR:
        case GL_POSITION:
            vectorsize = 4;
            break;
        default:
            vectorsize = 1;
            break;
		}
    return vectorsize;
	}

EXPORT_C void GL_APIENTRY glGetLightfv (GLenum light, GLenum pname, GLfloat *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetLightfv ->\n");
    TInt32 vectorsize = getLightVectorSize(pname);
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetLightfv );
    call.AppendParam( light );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetLightfv <-\n");
	}

EXPORT_C void GL_APIENTRY glGetLightxv (GLenum light, GLenum pname, GLfixed *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetLightxv ->\n");
    TInt32 vectorsize = getLightVectorSize(pname);
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetLightxv );
    call.AppendParam( light );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetLightxv <-\n");
	}

inline TInt32 getMaterialVectorSize(GLenum pname)
	{
    // oglApi.inl, oglGetMaterialInternal()
    TInt32 vectorsize;
    switch( pname )
    	{
        case GL_AMBIENT:
        case GL_DIFFUSE:
        case GL_SPECULAR:
        case GL_EMISSION:
            vectorsize = 4;
            break;
        default:
            vectorsize = 1;
            break;
    	}
    return vectorsize;
	}

EXPORT_C void GL_APIENTRY glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetMaterialfv ->\n");
    TInt32 vectorsize = getMaterialVectorSize(pname);
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetMaterialfv );
    call.AppendParam( face );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetMaterialfv <-\n");
	}

EXPORT_C void GL_APIENTRY glGetMaterialxv (GLenum face, GLenum pname, GLfixed *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetMaterialxv ->\n");
    TInt32 vectorsize = getMaterialVectorSize(pname);
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetMaterialxv );
    call.AppendParam( face );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetMaterialxv <-\n");
	}

EXPORT_C void GL_APIENTRY glGetPointerv (GLenum pname, void **params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetPointerv ->\n");
    const GLvoid *pointer;
    switch( pname )
    	{
        case GL_COLOR_ARRAY_POINTER:
            pointer = vertexArrays[COLOR_ARRAY].pointer;
            break;
        case GL_NORMAL_ARRAY_POINTER:
            pointer = vertexArrays[NORMAL_ARRAY].pointer;
            break;
        case GL_POINT_SIZE_ARRAY_POINTER_OES:
            pointer = vertexArrays[POINT_SIZE_ARRAY].pointer;
            break;
        case GL_TEXTURE_COORD_ARRAY_POINTER:
            pointer = vertexArrays[TEX_COORD_ARRAY].pointer;
            break;
        case GL_VERTEX_ARRAY_POINTER:
            pointer = vertexArrays[VERTEX_ARRAY].pointer;
            break;
        case GL_MATRIX_INDEX_ARRAY_POINTER_OES:
            pointer = vertexArrays[MATRIX_INDEX_ARRAY].pointer;
            break;
        case GL_WEIGHT_ARRAY_POINTER_OES:
            pointer = vertexArrays[WEIGHT_ARRAY].pointer;
            break;
        default:
            pointer = NULL;
            break;
    	}
    *params = (void*)pointer;
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetPointerv <-\n");
	}

EXPORT_C const GLubyte * GL_APIENTRY glGetString (GLenum name)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetString ->\n");
    if(name < GL_VENDOR || name > GL_EXTENSIONS)
    {
        SetError( GL_INVALID_ENUM );
        return NULL;
    }
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetString <-\n");
    return (const GLubyte*) g_strings[name - GL_VENDOR];
	}

EXPORT_C void GL_APIENTRY glGetTexEnvfv (GLenum env, GLenum pname, GLfloat *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetTexEnvfv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_TEXTURE_ENV_COLOR )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetTexEnvfv );
    call.AppendParam( env );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetTexEnvfv <-\n");
	}

EXPORT_C void GL_APIENTRY glGetTexEnviv (GLenum env, GLenum pname, GLint *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetTexEnviv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_TEXTURE_ENV_COLOR )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetTexEnviv );
    call.AppendParam( env );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetTexEnviv <-\n");
	}

EXPORT_C void GL_APIENTRY glGetTexEnvxv (GLenum env, GLenum pname, GLfixed *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetTexEnvxv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_TEXTURE_ENV_COLOR )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetTexEnvxv );
    call.AppendParam( env );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetTexEnvxv <-\n");
	}

EXPORT_C void GL_APIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetTexParameterfv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_TEXTURE_CROP_RECT_OES )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetTexParameterfv );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetTexParameterfv <-\n");
	}

EXPORT_C void GL_APIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetTexParameteriv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_TEXTURE_CROP_RECT_OES )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetTexParameteriv );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetTexParameteriv <-\n");
	}

EXPORT_C void GL_APIENTRY glGetTexParameterxv (GLenum target, GLenum pname, GLfixed *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetTexParameterxv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_TEXTURE_CROP_RECT_OES )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglGetTexParameterxv );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glGetTexParameterxv <-\n");
	}

EXPORT_C void GL_APIENTRY glHint (GLenum target, GLenum mode)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glHint ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglHint, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( mode );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glHint <-\n");
	}

EXPORT_C GLboolean GL_APIENTRY glIsBuffer (GLuint buffer)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glIsBuffer ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglIsBuffer );
    call.AppendParam( buffer );
    GLboolean result = (GLboolean)ExecuteFunction(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glIsBuffer <-\n");
    return result;
	}

EXPORT_C GLboolean GL_APIENTRY glIsEnabled (GLenum cap)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glIsEnabled ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglIsEnabled );
    call.AppendParam( cap );
    GLboolean result = (GLboolean)ExecuteFunction(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glIsEnabled <-\n");
    return result;
	}

EXPORT_C GLboolean GL_APIENTRY glIsTexture (GLuint texture)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glIsTexture ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglIsTexture );
    call.AppendParam( texture );
    GLboolean result = (GLboolean)ExecuteFunction(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glIsTexture <-\n");
    return result;
	}

EXPORT_C void GL_APIENTRY glLightf (GLenum light, GLenum pname, GLfloat param) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightf ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglLightf, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( light );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightf <-\n");
	}

EXPORT_C void GL_APIENTRY glLightfv (GLenum light, GLenum pname, const GLfloat *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightfv ->\n");
    TInt32 vectorsize = getLightVectorSize( pname );
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglLightfv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( light );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightfv <-\n");
	}

EXPORT_C void GL_APIENTRY glLightx (GLenum light, GLenum pname, GLfixed param)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightx ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglLightx, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( light );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightx <-\n");
	}

EXPORT_C void GL_APIENTRY glLightxv (GLenum light, GLenum pname, const GLfixed *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightxv ->\n");
    TInt32 vectorsize = getLightVectorSize( pname );
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglLightxv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( light );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightxv <-\n");
	}

EXPORT_C void GL_APIENTRY glLightModelf (GLenum pname, GLfloat param) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightModelf ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglLightModelf, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightModelf <-\n");
	}

EXPORT_C void GL_APIENTRY glLightModelfv (GLenum pname, const GLfloat *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightModelfv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_LIGHT_MODEL_AMBIENT )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglLightModelfv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightModelfv <-\n");
	}

EXPORT_C void GL_APIENTRY glLightModelx (GLenum pname, GLfixed param)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightModelx ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglLightModelx, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightModelx <-\n");
	}

EXPORT_C void GL_APIENTRY glLightModelxv (GLenum pname, const GLfixed *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightModelxv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_LIGHT_MODEL_AMBIENT )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglLightModelxv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLightModelxv <-\n");
	}

EXPORT_C void GL_APIENTRY glLineWidth (GLfloat width) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLineWidth ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglLineWidth, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( width );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLineWidth <-\n");
	}

EXPORT_C void GL_APIENTRY glLineWidthx (GLfixed width)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLineWidthx ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglLineWidthx, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( width );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLineWidthx <-\n");
	}

EXPORT_C void GL_APIENTRY glLoadIdentity (void)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLoadIdentity ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglLoadIdentity, RemoteFunctionCallData::EOpRequest );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLoadIdentity <-\n");
	}

EXPORT_C void GL_APIENTRY glLoadMatrixf (const GLfloat *m)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLoadMatrixf ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglLoadMatrixf, RemoteFunctionCallData::EOpRequest );
    call.AppendVector( m, 16 );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLoadMatrixf <-\n");
	}

EXPORT_C void GL_APIENTRY glLoadMatrixx (const GLfixed *m)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLoadMatrixx ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglLoadMatrixx, RemoteFunctionCallData::EOpRequest );
    call.AppendVector( m, 16 );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLoadMatrixx <-\n");
	}

EXPORT_C void GL_APIENTRY glLogicOp (GLenum opcode)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLogicOp ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglLogicOp, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( opcode );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLogicOp <-\n");
	}

EXPORT_C void GL_APIENTRY glMaterialf (GLenum face, GLenum pname, GLfloat param) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMaterialf ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglMaterialf, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( face );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMaterialf <-\n");
	}

EXPORT_C void GL_APIENTRY glMaterialfv (GLenum face, GLenum pname, const GLfloat *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMaterialfv ->\n");
    TInt32 vectorsize = getMaterialVectorSize( pname );
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglMaterialfv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( face );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMaterialfv <-\n");
	}

EXPORT_C void GL_APIENTRY glMaterialx (GLenum face, GLenum pname, GLfixed param)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMaterialx ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglMaterialx, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( face );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMaterialx <-\n");
	}

EXPORT_C void GL_APIENTRY glMaterialxv (GLenum face, GLenum pname, const GLfixed *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMaterialxv ->\n");
    TInt32 vectorsize = getMaterialVectorSize( pname );
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglMaterialxv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( face );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMaterialxv <-\n");
	}

EXPORT_C void GL_APIENTRY glMatrixMode (GLenum mode)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMatrixMode ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglMatrixMode, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( mode );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMatrixMode <-\n");
	}

EXPORT_C void GL_APIENTRY glMultMatrixf (const GLfloat *m)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMultMatrixf ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglMultMatrixf, RemoteFunctionCallData::EOpRequest );
    call.AppendVector( m, 16 );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMultMatrixf <-\n");
	}

EXPORT_C void GL_APIENTRY glMultMatrixx (const GLfixed *m)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMultMatrixx ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglMultMatrixx, RemoteFunctionCallData::EOpRequest );
    call.AppendVector( m, 16 );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMultMatrixx <-\n");
	}

EXPORT_C void GL_APIENTRY glMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMultiTexCoord4f ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglMultiTexCoord4f, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( s );
    call.AppendParam( t );
    call.AppendParam( r );
    call.AppendParam( q );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMultiTexCoord4f <-\n");
	}

EXPORT_C void GL_APIENTRY glMultiTexCoord4x (GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMultiTexCoord4x ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglMultiTexCoord4x, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( s );
    call.AppendParam( t );
    call.AppendParam( r );
    call.AppendParam( q );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMultiTexCoord4x <-\n");
	}

EXPORT_C void GL_APIENTRY glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glNormal3f ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglNormal3f, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( nx );
    call.AppendParam( ny );
    call.AppendParam( nz );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glNormal3f <-\n");
	}

EXPORT_C void GL_APIENTRY glNormal3x (GLfixed nx, GLfixed ny, GLfixed nz)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glNormal3x ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglNormal3x, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( nx );
    call.AppendParam( ny );
    call.AppendParam( nz );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glNormal3x <-\n");
	}

EXPORT_C void GL_APIENTRY glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glNormalPointer ->\n");
    vertexArrays[NORMAL_ARRAY].size = 3;
    vertexArrays[NORMAL_ARRAY].type = type;
    vertexArrays[NORMAL_ARRAY].stride = stride;
    vertexArrays[NORMAL_ARRAY].pointer = pointer;
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glNormalPointer <-\n");
	}

EXPORT_C void GL_APIENTRY glOrthof (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glOrthof ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglOrthof, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( left );
    call.AppendParam( right );
    call.AppendParam( bottom );
    call.AppendParam( top );
    call.AppendParam( zNear );
    call.AppendParam( zFar );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glOrthof <-\n");
	}

EXPORT_C void GL_APIENTRY glOrthox (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glOrthox ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglOrthox, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( left );
    call.AppendParam( right );
    call.AppendParam( bottom );
    call.AppendParam( top );
    call.AppendParam( zNear );
    call.AppendParam( zFar );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glOrthox <-\n");
	}

EXPORT_C void GL_APIENTRY glPixelStorei (GLenum pname, GLint param)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPixelStorei ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglPixelStorei, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPixelStorei <-\n");
	}

EXPORT_C void GL_APIENTRY glPointParameterf (GLenum pname, GLfloat param) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPointParameterf ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglPointParameterf, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPointParameterf <-\n");
	}

EXPORT_C void GL_APIENTRY glPointParameterfv (GLenum pname, const GLfloat *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPointParameterfv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_POINT_DISTANCE_ATTENUATION )
        vectorsize = 3;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglPointParameterfv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPointParameterfv <-\n");
	}

EXPORT_C void GL_APIENTRY glPointParameterx (GLenum pname, GLfixed param)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPointParameterx ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglPointParameterx, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPointParameterx <-\n");
	}

EXPORT_C void GL_APIENTRY glPointParameterxv (GLenum pname, const GLfixed *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPointParameterxv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_POINT_DISTANCE_ATTENUATION )
        vectorsize = 3;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglPointParameterxv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPointParameterxv <-\n");
	}

EXPORT_C void GL_APIENTRY glPointSize (GLfloat size) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPointSize ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglPointSize, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( size );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPointSize <-\n");
	}

EXPORT_C void GL_APIENTRY glPointSizePointerOES (GLenum type, GLsizei stride, const GLvoid *pointer)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPointSizePointerOES ->\n");
    vertexArrays[POINT_SIZE_ARRAY].size = 1;
    vertexArrays[POINT_SIZE_ARRAY].type = type;
    vertexArrays[POINT_SIZE_ARRAY].stride = stride;
    vertexArrays[POINT_SIZE_ARRAY].pointer = pointer;
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPointSizePointerOES <-\n");
	}

EXPORT_C void GL_APIENTRY glPointSizex (GLfixed size)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPointSizex ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglPointSizex, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( size );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPointSizex <-\n");
	}

EXPORT_C void GL_APIENTRY glPolygonOffset (GLfloat factor, GLfloat units) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPolygonOffset ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglPolygonOffset, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( factor );
    call.AppendParam( units );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPolygonOffset <-\n");
	}

EXPORT_C void GL_APIENTRY glPolygonOffsetx (GLfixed factor, GLfixed units)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPolygonOffsetx ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglPolygonOffsetx, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( factor );
    call.AppendParam( units );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPolygonOffsetx <-\n");
	}

EXPORT_C void GL_APIENTRY glPopMatrix (void)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPopMatrix ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglPopMatrix, RemoteFunctionCallData::EOpRequest );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPopMatrix <-\n");
	}

EXPORT_C void GL_APIENTRY glPushMatrix (void)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPushMatrix ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglPushMatrix, RemoteFunctionCallData::EOpRequest );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glPushMatrix <-\n");
	}

inline TInt32 getPixelSizeInBytes(GLenum format, GLenum type)
	{
    TInt32 size = 0;
    if( format == GL_RGBA && type == GL_UNSIGNED_BYTE )
		{
        size = 4;
		}
    else if( format == GL_RGB && type == GL_UNSIGNED_BYTE )
		{
        size = 3;
		}
    else if( format == GL_RGBA && type == GL_UNSIGNED_SHORT_4_4_4_4
	        ||  format == GL_RGBA && type == GL_UNSIGNED_SHORT_5_5_5_1
		    ||  format == GL_RGB && type == GL_UNSIGNED_SHORT_5_6_5
			||  format == GL_LUMINANCE_ALPHA && type == GL_UNSIGNED_BYTE )
		{
        size = 2;
		}
    return size;
	}

EXPORT_C void GL_APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glReadPixels ->\n");
    TInt32 vectorsize = width * height * getPixelSizeInBytes( format, type );
    if(vectorsize < 0) // cannot serialize vector with size < 0
		{
        SetError( GL_INVALID_VALUE );
        return;
		}
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglReadPixels );
    call.AppendParam( x );
    call.AppendParam( y );
    call.AppendParam( width );
    call.AppendParam( height );
    call.AppendParam( format );
    call.AppendParam( type );
    call.AppendVector( pixels, vectorsize, RemoteFunctionCallData::EOut );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glReadPixels <-\n");
	}

EXPORT_C void GL_APIENTRY glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glRotatef ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglRotatef, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( angle );
    call.AppendParam( x );
    call.AppendParam( y );
    call.AppendParam( z );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glRotatef <-\n");
	}

EXPORT_C void GL_APIENTRY glRotatex (GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glRotatex ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglRotatex, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( angle );
    call.AppendParam( x );
    call.AppendParam( y );
    call.AppendParam( z );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glRotatex <-\n");
	}

EXPORT_C void GL_APIENTRY glSampleCoverage (GLclampf value, GLboolean invert) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glSampleCoverage ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglSampleCoverage, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( value );
    call.AppendParam( invert );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glSampleCoverage <-\n");
	}

EXPORT_C void GL_APIENTRY glSampleCoveragex (GLclampx value, GLboolean invert)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glSampleCoveragex ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglSampleCoveragex, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( value );
    call.AppendParam( invert );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glSampleCoveragex <-\n");
	}

EXPORT_C void GL_APIENTRY glScalef (GLfloat x, GLfloat y, GLfloat z) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glScalef ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglScalef, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( x );
    call.AppendParam( y );
    call.AppendParam( z );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glScalef <-\n");
	}

EXPORT_C void GL_APIENTRY glScalex (GLfixed x, GLfixed y, GLfixed z)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glScalex ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglScalex, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( x );
    call.AppendParam( y );
    call.AppendParam( z );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glScalex <-\n");
	}

EXPORT_C void GL_APIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glScissor ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglScissor, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( x );
    call.AppendParam( y );
    call.AppendParam( width );
    call.AppendParam( height );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glScissor <-\n");
	}

EXPORT_C void GL_APIENTRY glShadeModel (GLenum mode)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glShadeModel ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglShadeModel, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( mode );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glShadeModel <-\n");
	}

EXPORT_C void GL_APIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glStencilFunc ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglStencilFunc, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( func );
    call.AppendParam( ref );
    call.AppendParam( mask );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glStencilFunc <-\n");
	}

EXPORT_C void GL_APIENTRY glStencilMask (GLuint mask)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glStencilMask ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglStencilMask, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( mask );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glStencilMask <-\n");
	}

EXPORT_C void GL_APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glStencilOp ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglStencilOp, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( fail );
    call.AppendParam( zfail );
    call.AppendParam( zpass );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glStencilOp <-\n");
	}

EXPORT_C void GL_APIENTRY glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexCoordPointer ->\n");
    vertexArrays[TEX_COORD_ARRAY].size = size;
    vertexArrays[TEX_COORD_ARRAY].type = type;
    vertexArrays[TEX_COORD_ARRAY].stride = stride;
    vertexArrays[TEX_COORD_ARRAY].pointer = pointer;
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexCoordPointer <-\n");
	}

EXPORT_C void GL_APIENTRY glTexEnvf (GLenum target, GLenum pname, GLfloat param) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexEnvf ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTexEnvf, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexEnvf <-\n");
	}

EXPORT_C void GL_APIENTRY glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexEnvfv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_TEXTURE_ENV_COLOR )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTexEnvfv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexEnvfv <-\n");
	}

EXPORT_C void GL_APIENTRY glTexEnvi (GLenum target, GLenum pname, GLint param)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexEnvi ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTexEnvi, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexEnvi <-\n");
	}

EXPORT_C void GL_APIENTRY glTexEnviv (GLenum target, GLenum pname, const GLint *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexEnviv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_TEXTURE_ENV_COLOR )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTexEnviv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexEnviv <-\n");
	}

EXPORT_C void GL_APIENTRY glTexEnvx (GLenum target, GLenum pname, GLfixed param)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexEnvx ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTexEnvx, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexEnvx <-\n");
	}

EXPORT_C void GL_APIENTRY glTexEnvxv (GLenum target, GLenum pname, const GLfixed *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexEnvxv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_TEXTURE_ENV_COLOR )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTexEnvxv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexEnvxv <-\n");
	}

EXPORT_C void GL_APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexImage2D ->\n");
    TInt32 vectorsize = width * height * getPixelSizeInBytes( format, type );
    if(vectorsize < 0) // cannot serialize vector with size < 0
		{
        SetError( GL_INVALID_VALUE );
        return;
		}
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTexImage2D, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( level );
    call.AppendParam( internalformat );
    call.AppendParam( width );
    call.AppendParam( height );
    call.AppendParam( border );
    call.AppendParam( format );
    call.AppendParam( type );
    call.AppendVector( pixels, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexImage2D <-\n");
	}

EXPORT_C void GL_APIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexParameterf ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTexParameterf, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexParameterf <-\n");
	}

EXPORT_C void GL_APIENTRY glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexParameterfv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_TEXTURE_CROP_RECT_OES )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTexParameterfv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexParameterfv <-\n");
	}

EXPORT_C void GL_APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexParameteri ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTexParameteri, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexParameteri <-\n");
	}

EXPORT_C void GL_APIENTRY glTexParameteriv (GLenum target, GLenum pname, const GLint *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexParameteriv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_TEXTURE_CROP_RECT_OES )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTexParameteriv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexParameteriv <-\n");
	}

EXPORT_C void GL_APIENTRY glTexParameterx (GLenum target, GLenum pname, GLfixed param)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexParameterx ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTexParameterx, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendParam( param );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexParameterx <-\n");
	}

EXPORT_C void GL_APIENTRY glTexParameterxv (GLenum target, GLenum pname, const GLfixed *params)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexParameterxv ->\n");
    TInt32 vectorsize = 1;
    if( pname == GL_TEXTURE_CROP_RECT_OES )
        vectorsize = 4;
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTexParameterxv, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( pname );
    call.AppendVector( params, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexParameterxv <-\n");
	}

EXPORT_C void GL_APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexSubImage2D ->\n");
    TInt32 vectorsize = width * height * getPixelSizeInBytes( format, type );
    if(vectorsize < 0) // cannot serialize vector with size < 0
		{
        SetError( GL_INVALID_VALUE );
        return;
	    }
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTexSubImage2D, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( target );
    call.AppendParam( level );
    call.AppendParam( xoffset );
    call.AppendParam( yoffset );
    call.AppendParam( width );
    call.AppendParam( height );
    call.AppendParam( format );
    call.AppendParam( type );
    call.AppendVector( pixels, vectorsize );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTexSubImage2D <-\n");
	}

EXPORT_C void GL_APIENTRY glTranslatef (GLfloat x, GLfloat y, GLfloat z) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTranslatef ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTranslatef, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( x );
    call.AppendParam( y );
    call.AppendParam( z );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTranslatef <-\n");
	}

EXPORT_C void GL_APIENTRY glTranslatex (GLfixed x, GLfixed y, GLfixed z)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTranslatex ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglTranslatex, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( x );
    call.AppendParam( y );
    call.AppendParam( z );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glTranslatex <-\n");
	}

EXPORT_C void GL_APIENTRY glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glVertexPointer ->\n");
    vertexArrays[VERTEX_ARRAY].size = size;
    vertexArrays[VERTEX_ARRAY].type = type;
    vertexArrays[VERTEX_ARRAY].stride = stride;
    vertexArrays[VERTEX_ARRAY].pointer = pointer;
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glVertexPointer <-\n");
	}

EXPORT_C void GL_APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glViewport ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglViewport, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( x );
    call.AppendParam( y );
    call.AppendParam( width );
    call.AppendParam( height );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glViewport <-\n");
	}

// Guest Open GL ES extension export support for eglGetProcAddress  
typedef struct
	{
	const char*          extnName;		// procedure or extension name
	ExtensionProcPointer procAddr;
	} TGlExtnInfo;

// GL ES extension functions
static const TGlExtnInfo glesProcedures[] =
    { // all the functions below the "from glextplatform.h" comment
		{ "glCurrentPaletteMatrixOES", (ExtensionProcPointer)glCurrentPaletteMatrixOES },
		{ "glDrawTexfOES", (ExtensionProcPointer)glDrawTexfOES },
		{ "glDrawTexfvOES", (ExtensionProcPointer)glDrawTexfvOES },
		{ "glDrawTexiOES", (ExtensionProcPointer)glDrawTexiOES },
		{ "glDrawTexivOES", (ExtensionProcPointer)glDrawTexivOES },
		{ "glDrawTexsOES", (ExtensionProcPointer)glDrawTexsOES },
		{ "glDrawTexsvOES", (ExtensionProcPointer)glDrawTexsvOES },
		{ "glDrawTexxOES", (ExtensionProcPointer)glDrawTexxOES },
		{ "glDrawTexxvOES", (ExtensionProcPointer)glDrawTexxvOES },
		{ "glLoadPaletteFromModelViewMatrixOES", (ExtensionProcPointer)glLoadPaletteFromModelViewMatrixOES },
		{ "glMatrixIndexPointerOES", (ExtensionProcPointer)glMatrixIndexPointerOES },
		{ "glQueryMatrixxOES", (ExtensionProcPointer)glQueryMatrixxOES },
		{ "glWeightPointerOES", (ExtensionProcPointer)glWeightPointerOES },
    };

const TInt glesProcCount = sizeof(glesProcedures) / sizeof(TGlExtnInfo);


TGles11ApiForEgl::TGles11ApiForEgl()
	{
	// DLL has loaded - publish vtable for MGles11ApiForEgl
	CVghwUtils::SetGles11ApiForEgl(this);
	}


TGles11ApiForEgl::~TGles11ApiForEgl()
	{
	// DLL is unloading - unpublish vtable for MGles11ApiForEgl
	CVghwUtils::SetGles11ApiForEgl(NULL);
	}


ExtensionProcPointer TGles11ApiForEgl::guestGetGles11ProcAddress (const char *aProcName)
	{
	// exhaustive search, for now.  (We should consider a sorted list if the number of functions grows.) 
	for (TInt idx = 0; idx < glesProcCount; idx++)
	  {
	  if (!strcmp(aProcName, glesProcedures[idx].extnName))
		  return glesProcedures[idx].procAddr;
	  }
	return NULL;
	}


EXPORT_C void GL_APIENTRY glCurrentPaletteMatrixOES (GLuint matrixpaletteindex)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glCurrentPaletteMatrixOES ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglCurrentPaletteMatrixOES, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( matrixpaletteindex );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glCurrentPaletteMatrixOES <-\n");
	}

EXPORT_C void GL_APIENTRY glDrawTexfOES (GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height) __SOFTFP
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexfOES ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDrawTexfOES, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( x );
    call.AppendParam( y );
    call.AppendParam( width );
    call.AppendParam( height );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexfOES <-\n");
	}

EXPORT_C void GL_APIENTRY glDrawTexfvOES (const GLfloat *coords)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexfvOES ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDrawTexfvOES, RemoteFunctionCallData::EOpRequest );
    call.AppendVector( coords, 5 ); // oglFast.c
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexfvOES <-\n");
	}

EXPORT_C void GL_APIENTRY glDrawTexiOES (GLint x, GLint y, GLint z, GLint width, GLint height)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexiOES ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDrawTexiOES, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( x );
    call.AppendParam( y );
    call.AppendParam( width );
    call.AppendParam( height );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexiOES <-\n");
	}

EXPORT_C void GL_APIENTRY glDrawTexivOES (const GLint *coords)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexivOES ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDrawTexivOES, RemoteFunctionCallData::EOpRequest );
    call.AppendVector( coords, 5 ); // oglFast.c
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexivOES <-\n");
	}

EXPORT_C void GL_APIENTRY glDrawTexsOES (GLshort x, GLshort y, GLshort z, GLshort width, GLshort height)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexsOES ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDrawTexsOES, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( x );
    call.AppendParam( y );
    call.AppendParam( width );
    call.AppendParam( height );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexsOES <-\n");
	}

EXPORT_C void GL_APIENTRY glDrawTexsvOES (const GLshort *coords)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexsvOES ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDrawTexsvOES, RemoteFunctionCallData::EOpRequest );
    call.AppendVector( coords, 5 ); // oglFast.c
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexsvOES <-\n");
	}

EXPORT_C void GL_APIENTRY glDrawTexxOES (GLfixed x, GLfixed y, GLfixed z, GLfixed width, GLfixed height)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexxOES ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDrawTexxOES, RemoteFunctionCallData::EOpRequest );
    call.AppendParam( x );
    call.AppendParam( y );
    call.AppendParam( width );
    call.AppendParam( height );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexxOES <-\n");
	}

EXPORT_C void GL_APIENTRY glDrawTexxvOES (const GLfixed *coords)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexxvOES ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglDrawTexxvOES, RemoteFunctionCallData::EOpRequest );
    call.AppendVector( coords, 5 ); // oglFast.c
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glDrawTexxvOES <-\n");
	}

EXPORT_C void GL_APIENTRY glLoadPaletteFromModelViewMatrixOES (void)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLoadPaletteFromModelViewMatrixOES ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglLoadPaletteFromModelViewMatrixOES, RemoteFunctionCallData::EOpRequest );
    ExecuteCommand(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glLoadPaletteFromModelViewMatrixOES <-\n");
	}

EXPORT_C void GL_APIENTRY glMatrixIndexPointerOES (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMatrixIndexPointerOES ->\n");
    vertexArrays[MATRIX_INDEX_ARRAY].size = size;
    vertexArrays[MATRIX_INDEX_ARRAY].type = type;
    vertexArrays[MATRIX_INDEX_ARRAY].stride = stride;
    vertexArrays[MATRIX_INDEX_ARRAY].pointer = pointer;
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glMatrixIndexPointerOES <-\n");
	}

GLbitfield GL_APIENTRY glQueryMatrixxOES (GLfixed mantissa[16], GLint exponent[16])
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glQueryMatrixxOES ->\n");
    RemoteFunctionCallData rfcData;
    OpenGlES11RFC call( rfcData );
    call.Init( OpenGlES11RFC::EglQueryMatrixxOES );
    call.AppendVector( mantissa, 16, RemoteFunctionCallData::EOut );
    call.AppendVector( exponent, 16, RemoteFunctionCallData::EOut );
    GLbitfield result = (GLbitfield)ExecuteFunction(call);
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glQueryMatrixxOES <-\n");
    return result;
	}

EXPORT_C void GL_APIENTRY glWeightPointerOES (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
	{
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glWeightPointerOES ->\n");
    vertexArrays[WEIGHT_ARRAY].size = size;
    vertexArrays[WEIGHT_ARRAY].type = type;
    vertexArrays[WEIGHT_ARRAY].stride = stride;
    vertexArrays[WEIGHT_ARRAY].pointer = pointer;
    OPENGL_TRACE(_APICALL_, "OpenGL client side: glWeightPointerOES <-\n");
	}

} /* extern "C" */
