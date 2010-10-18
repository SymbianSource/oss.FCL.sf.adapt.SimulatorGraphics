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

#ifndef _EGLUTILS_H_
#define _EGLUTILS_H_

#include <vector>
#include <EGL/egl.h>
#include "eglDefinitions.h"

class RefCountingObject
    {
public:
    RefCountingObject() : m_refCount( 0 ), m_terminated( false ) {}
    virtual ~RefCountingObject() { EGLI_ASSERT( m_refCount == 0); }

public:
    virtual void AddRef() { m_refCount++; }
    virtual int RefCount() const { return m_refCount; }
    //returns true if object should be destroyed
    virtual bool RemoveRef() { m_refCount--; EGLI_ASSERT(m_refCount >= 0); return (m_refCount == 0); }
    virtual void Terminate() { m_terminated = true; }
    virtual bool IsTerminated() const { return m_terminated; }

protected:
    int     m_refCount;
    bool    m_terminated;
    };

template<class T> 
extern void AddObject( std::vector<T*>& vector, T*& object );

template<class T>
extern T* FindObjectByPointer( const std::vector<T*>& vector, void* id, int* index );

template<class T>
extern  T* FindObjectById( const std::vector<T*>& vector, EGLint id, int* index );

template<class T>
extern  void DeleteObjectByPointer( std::vector<T*>& vector, void* item );

template<class T>
extern  void DeleteObjectById( std::vector<T*>& vector, EGLint id );

template<class T>
extern  void DestroyPointerVector( std::vector<T*>& vector );


#endif //_EGLUTILS_H_
