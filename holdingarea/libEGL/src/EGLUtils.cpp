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

#include "EGLUtils.h"
#include "EGLSurface.h"
#include "EGLContext.h"
#include "EGLImage.h"
#include "EGLThread.h"
#include "EGLProcess.h"
#include "EGLConfig.h"
#include "EGLDisplay.h"

template<class T>
void AddObject( std::vector<T*>& vector, T*& object )
    {
    if( !object ) return;
    try
        {
        vector.push_back(object);
        }
    catch( std::bad_alloc )
        {
        delete object;
        object = NULL;
        }
    }

template<class T>
T* FindObjectByPointer( const std::vector<T*>& vector, void* item, int* index )
    {
    T* ret = NULL;
    for( typename std::vector<T*>::size_type i = 0; i < vector.size(); i++ )
        {
        if( vector[i] == item )
            {
            ret = vector[i];
            if( index ) *index = i;
            break;
            }
        }
    return ret;
    }

template<class T>
T* FindObjectById( const std::vector<T*>& vector, EGLint id, int* index )
    {
    T* ret = NULL;
    for( typename std::vector<T*>::size_type i = 0; i < vector.size(); i++ )
        {
        if( vector[i]->Id() == id )
            {
            ret = vector[i];
            if( index ) *index = i;
            break;
            }
        }
    return ret;
    }

template<class T>
void DeleteObjectByPointer( std::vector<T*>& vector, void* item )
    {
    int index;
    T* object = FindObjectByPointer<T>( vector, item, &index );
    if( object )
        {
        delete object;
        vector.erase( vector.begin() + index );
        }
    }

template<class T>
void DeleteObjectById( std::vector<T*>& vector, EGLint id )
    {
    int index;
    T* object = FindObjectById<T>(vector, id, &index );
    if( object && object->Id() == id )
        {
        delete object;
        vector.erase( vector.begin() + index );
        }
    }

template<class T>
void DestroyPointerVector( std::vector<T*>& vector )
    {
    for(typename std::vector<T*>::iterator iter = vector.begin(); iter != vector.end(); iter++)
        {
        delete (*iter);
        }
    vector.clear();
    }

// CEGLSurface
template void AddObject( std::vector<class CEGLSurface*>& vector, class CEGLSurface*& object );
template class CEGLSurface* FindObjectByPointer( const std::vector<class CEGLSurface*>& vector, void* item, int* index );
template void DeleteObjectByPointer( std::vector<class CEGLSurface*>& vector, void* item );
template void DestroyPointerVector( std::vector<class CEGLSurface*>& vector );

// CEGLContext
template void AddObject( std::vector<class CEGLContext*>& vector, class CEGLContext*& object );
template void DeleteObjectByPointer( std::vector<class CEGLContext*>& vector, void* item );
template void DestroyPointerVector( std::vector<class CEGLContext*>& vector );

// CEGLImage
template void AddObject( std::vector<class CEGLImage*>& vector, class CEGLImage*& object );
template void DeleteObjectByPointer( std::vector<class CEGLImage*>& vector, void* item );
template void DestroyPointerVector( std::vector<class CEGLImage*>& vector );

// CEGLThread
template void AddObject( std::vector<class CEGLThread*>& vector, class CEGLThread*& object );
template void DeleteObjectById( std::vector<class CEGLThread*>& vector, EGLint id );
template void DestroyPointerVector( std::vector<class CEGLThread*>& vector );
template class CEGLThread* FindObjectById( const std::vector<class CEGLThread*>& vector, EGLint id, int* index );

// CEGLProcess
template void AddObject( std::vector<class CEGLProcess*>& vector, class CEGLProcess*& object );
template void DestroyPointerVector( std::vector<class CEGLProcess*>& vector );
template void DeleteObjectById( std::vector<class CEGLProcess*>& vector, EGLint id );

// CEGLDisplay
template void AddObject( std::vector<class CEGLDisplay*>& vector, class CEGLDisplay*& object );
template class CEGLDisplay* FindObjectByPointer( const std::vector<class CEGLDisplay*>& vector, void* item, int* index );
template void DeleteObjectByPointer( std::vector<class CEGLDisplay*>& vector, void* item );
template void DestroyPointerVector( std::vector<class CEGLDisplay*>& vector );

// CEGLConfig
template void AddObject( std::vector<class CEGLConfig*>& vector, class CEGLConfig*& object );
template class CEGLConfig* FindObjectByPointer( const std::vector<class CEGLConfig*>& vector, void* item, int* index );
template class CEGLConfig* FindObjectById( const std::vector<class CEGLConfig*>& vector, EGLint id, int* index );
template void DestroyPointerVector( std::vector<class CEGLConfig*>& vector );
