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

#include "GLESDesktopGL.h"
#include "glesInternal.h"
#include "EGLInterface.h"
#include <iostream>

// Load function via EGL interface
#define LOAD(func) do \
	{ \
		*reinterpret_cast<void**>(&func) = iFace->GetHostProcAddress(#func); \
		if(func == NULL) \
		{ \
			std::cerr << "Symbol " << #func << " not found" << std::endl; \
			success = false; \
		} \
	} while(0)

GLESDesktopGL::GLESDesktopGL() :
	m_loaded(false)
{
}

bool GLESDesktopGL::Load()
{
	if( m_loaded )
	{
		return true;
	}

	bool success = true;

	IGLEStoEGLInterface* iFace = EGLtoGLESInterface::GetEGLInterface();

	LOAD(glActiveTexture);
	LOAD(glAlphaFunc);
	LOAD(glBegin);
	LOAD(glBindBuffer);
	LOAD(glBindTexture);
	LOAD(glBlendFunc);
	LOAD(glBufferData);
	LOAD(glBufferSubData);
	LOAD(glClear);
	LOAD(glClearColor);
	LOAD(glClearDepth);
	LOAD(glClearStencil);
	LOAD(glClientActiveTexture);
	LOAD(glClipPlane);
	LOAD(glColor4d);
	LOAD(glColor4f);
	LOAD(glColor4fv);
	LOAD(glColor4ub);
	LOAD(glColor4ubv);
	LOAD(glColorMask);
	LOAD(glColorPointer);
	LOAD(glCompressedTexImage2D);
	LOAD(glCompressedTexSubImage2D);
	LOAD(glCopyTexImage2D);
	LOAD(glCopyTexSubImage2D);
	LOAD(glCullFace);
	LOAD(glDeleteBuffers);
	LOAD(glDeleteTextures);
	LOAD(glDepthFunc);
	LOAD(glDepthMask);
	LOAD(glDepthRange);
	LOAD(glDisable);
	LOAD(glDisableClientState);
	LOAD(glDrawArrays);
	LOAD(glDrawElements);
	LOAD(glEnable);
	LOAD(glEnableClientState);
	LOAD(glEnd);
	LOAD(glFinish);
	LOAD(glFlush);
	LOAD(glFogf);
	LOAD(glFogfv);
	LOAD(glFrontFace);
	LOAD(glFrustum);
	LOAD(glGenBuffers);
	LOAD(glGenTextures);
	LOAD(glGetBooleanv);
	LOAD(glGetBufferParameteriv);
	LOAD(glGetClipPlane);
	LOAD(glGetDoublev);
	LOAD(glGetError);
	LOAD(glGetFloatv);
	LOAD(glGetIntegerv);
	LOAD(glGetLightfv);
	LOAD(glGetMaterialfv);
	LOAD(glGetPointerv);
	LOAD(glGetString);
	LOAD(glGetTexEnvfv);
	LOAD(glGetTexEnviv);
	LOAD(glGetTexParameterfv);
	LOAD(glGetTexParameteriv);
	LOAD(glHint);
	LOAD(glIsBuffer);
	LOAD(glIsEnabled);
	LOAD(glIsTexture);
	LOAD(glLightf);
	LOAD(glLightfv);
	LOAD(glLightModelf);
	LOAD(glLightModelfv);
	LOAD(glLineWidth);
	LOAD(glLoadIdentity);
	LOAD(glLoadMatrixf);
	LOAD(glLogicOp);
	LOAD(glMaterialf);
	LOAD(glMaterialfv);
	LOAD(glMultiTexCoord2fv);
	LOAD(glMultiTexCoord2sv);
	LOAD(glMultiTexCoord3fv);
	LOAD(glMultiTexCoord3sv);
	LOAD(glMultiTexCoord4fv);
	LOAD(glMultiTexCoord4sv);
	LOAD(glMultiTexCoord4f);
	LOAD(glMultMatrixf);
	LOAD(glNormal3f);
	LOAD(glNormal3fv);
	LOAD(glNormal3sv);
	LOAD(glOrtho);
	LOAD(glPointParameterf);
	LOAD(glPointParameterfv);
	LOAD(glPointSize);
	LOAD(glPolygonOffset);
	LOAD(glRotatef);
	LOAD(glScalef);
	LOAD(glTexEnvf);
	LOAD(glTexEnvfv);
	LOAD(glTexParameterf);
	LOAD(glTexParameterfv);
	LOAD(glMatrixMode);
	LOAD(glNormalPointer);
	LOAD(glPixelStorei);
	LOAD(glPopMatrix);
	LOAD(glPushMatrix);
	LOAD(glReadPixels);
	LOAD(glSampleCoverage);
	LOAD(glScissor);
	LOAD(glShadeModel);
	LOAD(glStencilFunc);
	LOAD(glStencilMask);
	LOAD(glStencilOp);
	LOAD(glTexCoordPointer);
	LOAD(glTexEnvi);
	LOAD(glTexEnviv);
	LOAD(glTexImage2D);
	LOAD(glTexParameteri);
	LOAD(glTexParameteriv);
	LOAD(glTexSubImage2D);
	LOAD(glTranslatef);
	LOAD(glVertex2fv);
	LOAD(glVertex2sv);
	LOAD(glVertex3fv);
	LOAD(glVertex3sv);
	LOAD(glVertex4fv);
	LOAD(glVertex4sv);
	LOAD(glVertexPointer);
	LOAD(glViewport);
    LOAD(glDrawPixels);

	m_loaded = success;

	return success;
}

#undef LOAD