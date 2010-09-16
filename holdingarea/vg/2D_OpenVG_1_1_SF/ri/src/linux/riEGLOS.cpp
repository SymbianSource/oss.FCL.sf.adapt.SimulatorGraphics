/*------------------------------------------------------------------------
 *
 * EGL 1.3
 * -------
 *
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *//**
 * \file
 * \brief	Linux specific EGL functionality, modified from macosx source file
 * \note
  *//*-------------------------------------------------------------------*/

#include "riEGLOS.h"

#include <pthread.h>
#include <sys/errno.h>
#include <GL/gl.h>
#include <GL/glut.h>

#if defined(None)
#   undef None
#endif

#include "sfCompiler.h"

namespace OpenVGRI
{

/*-------------------------------------------------------------------*//*!
* \brief
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

void* OSGetCurrentThreadID(void)
{
    return (void*)pthread_self();   //TODO this is not safe
}

/*-------------------------------------------------------------------*//*!
* \brief
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

static pthread_mutex_t mutex;
static int mutexRefCount = 0;
static bool mutexInitialized = false;
//acquired mutex cannot be deinited
void OSDeinitMutex(void)
{
    RI_ASSERT(mutexInitialized);
    RI_ASSERT(mutexRefCount == 0);
    int ret = pthread_mutex_destroy(&mutex);
    RI_ASSERT(ret != EINVAL);	//assert that the mutex has been initialized
    RI_ASSERT(ret != EAGAIN);	//assert that the maximum number of recursive locks hasn't been exceeded
    RI_ASSERT(!ret);	//check that there aren't other errors
    RI_UNREF(ret);
}
void OSAcquireMutex(void)
{
    if(!mutexInitialized)
    {
        int ret;
        pthread_mutexattr_t attr;
        ret = pthread_mutexattr_init(&attr);	//initially not locked
        RI_ASSERT(!ret);	//check that there aren't any errors
        ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);	//count the number of recursive locks
        RI_ASSERT(!ret);	//check that there aren't any errors
        ret = pthread_mutex_init(&mutex, &attr);
        pthread_mutexattr_destroy(&attr);
        RI_ASSERT(!ret);	//check that there aren't more errors
        RI_UNREF(ret);
        mutexInitialized = true;
    }
    int ret = pthread_mutex_lock(&mutex);
    RI_ASSERT(ret != EINVAL);	//assert that the mutex has been initialized
    RI_ASSERT(ret != EAGAIN);	//assert that the maximum number of recursive locks hasn't been exceeded
    RI_ASSERT(ret != EDEADLK);	//recursive mutexes shouldn't return this
    RI_ASSERT(!ret);	//check that there aren't other errors
    RI_UNREF(ret);
    mutexRefCount++;
}
void OSReleaseMutex(void)
{
    RI_ASSERT(mutexInitialized);
    mutexRefCount--;
    RI_ASSERT(mutexRefCount >= 0);
    int ret = pthread_mutex_unlock(&mutex);
    RI_ASSERT(ret != EPERM);	//assert that the current thread owns the mutex
    RI_ASSERT(!ret);	//check that there aren't more errors
    RI_UNREF(ret);
}

/*-------------------------------------------------------------------*//*!
* \brief
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

static bool isBigEndian()
{
    static const RIuint32 v = 0x12345678u;
    const RIuint8* p = (const RIuint8*)&v;
    RI_ASSERT (*p == (RIuint8)0x12u || *p == (RIuint8)0x78u);
    return (*p == (RIuint8)(0x12)) ? true : false;
}


/*-------------------------------------------------------------------*//*!
* \brief
* \param
* \return
* \note
*//*-------------------------------------------------------------------*/

OSWindowContext* OSCreateWindowContext(EGLNativeWindowType window)
{
    try
    {
        OSWindowContext* ctx = RI_NEW(OSWindowContext, ());
        ctx->window = (int)window;
        ctx->tmp = NULL;
        ctx->tmpWidth = 0;
        ctx->tmpHeight = 0;
        return ctx;
    }
    catch(std::bad_alloc)
    {
        return NULL;
    }
}

void OSDestroyWindowContext(OSWindowContext* ctx)
{
    if(ctx)
    {
        RI_DELETE_ARRAY(ctx->tmp);
        RI_DELETE(ctx);
    }
}

bool OSIsWindow(const OSWindowContext* context)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
        //TODO implement
        return true;
    }
    return false;
}

void OSGetWindowSize(const OSWindowContext* ctx, int& width, int& height)
{
    if(ctx)
    {
        int currWin = glutGetWindow();
        glutSetWindow((int)ctx->window);
        width = glutGet(GLUT_WINDOW_WIDTH);
        height = glutGet(GLUT_WINDOW_HEIGHT);
        glutSetWindow(currWin);
    }
    else
    {
        width = 0;
        height = 0;
    }
}

void OSBlitToWindow(OSWindowContext* ctx, const Drawable* drawable)
{
    if(ctx)
    {
        int w = drawable->getWidth();
        int h = drawable->getHeight();

        int currWin = glutGetWindow();
        glutSetWindow((int)ctx->window);

        if(!ctx->tmp || ctx->tmpWidth != w || ctx->tmpHeight != h)
        {
            RI_DELETE_ARRAY(ctx->tmp);
            ctx->tmp = NULL;
            try
            {
                ctx->tmp = RI_NEW_ARRAY(unsigned int, w*h);	//throws bad_alloc
                ctx->tmpWidth = w;
                ctx->tmpHeight = h;
            }
            catch(std::bad_alloc)
            {
                //do nothing
            }
        }

        if(ctx->tmp)
        {
            glViewport(0, 0, w, h);
            glDisable(GL_DEPTH_TEST);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            //NOTE: we assume here that the display is always in sRGB color space
            VGImageFormat f = VG_sABGR_8888_PRE;
            if(isBigEndian())
                f = VG_sRGBA_8888_PRE;
            static bool flip = false;
            if (!flip)
            {
                vgReadPixels(ctx->tmp, w*sizeof(unsigned int), f, 0, 0, w, h);
                //flip = true;
            }
            else
            {
                void* ptr = (void*)((RIuint8*)ctx->tmp + (w*sizeof(unsigned int)*(h-1)));
                vgReadPixels(ptr, -w*sizeof(unsigned int), f, 0, 0, w, h);
                flip = false;
            }
            glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, ctx->tmp);
        }

        glutSwapBuffers();	//shows the OpenGL frame buffer
        glutSetWindow(currWin);		//restore the current window
    }
}

EGLDisplay OSGetDisplay(EGLNativeDisplayType display_id)
{
    RI_UNREF(display_id);
    return (EGLDisplay)1;    //support only a single display
}

}   //namespace OpenVGRI
