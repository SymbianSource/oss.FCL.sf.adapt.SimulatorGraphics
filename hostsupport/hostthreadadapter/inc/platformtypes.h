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
// Cross platform types

#ifndef PSU_PLATFORMTYPES_H
#define PSU_PLATFORMTYPES_H

#include <limits.h>


#ifdef WIN32
#define DllExport   __declspec( dllexport )
#else  // LINUX
#define DllExport
#endif


#ifdef WIN32

#if !defined(__STDC_CONSTANT_MACROS) && !defined(HAVE_STDINT_H)
#define __STDC_CONSTANT_MACROS
    /*  MSVC++ does not have the standard header <stdint.h>, so we need to
        write here all the declarations that <stdint.h> shall contain */
#ifndef HAVE_INT8_T
#define HAVE_INT8_T 1
    typedef signed __int8         int8_t;
#endif
#ifndef HAVE_INT16_T
#define HAVE_INT16_T 1
    typedef signed __int16        int16_t;
#endif
#ifndef HAVE_INT32_T
#define HAVE_INT32_T 1
    typedef signed __int32        int32_t;
#endif
#ifndef HAVE_INT64_T
#define HAVE_INT64_T 1
    typedef signed __int64        int64_t;
#endif
#ifndef HAVE_U_INT8_T
#define HAVE_U_INT8_T 1
    typedef unsigned __int8     uint8_t;
    typedef unsigned __int8     u_int8_t;
#endif
#ifndef HAVE_U_INT16_T
#define HAVE_U_INT16_T 1
    typedef unsigned __int16    uint16_t;
    typedef unsigned __int16    u_int16_t;
#endif
#ifndef HAVE_U_INT32_T
#define HAVE_U_INT32_T 1
    typedef unsigned __int32    uint32_t;
    typedef unsigned __int32    u_int32_t;
#endif
#ifndef HAVE_U_INT64_T
#define HAVE_U_INT64_T 1
    typedef unsigned __int64    uint64_t;
    typedef unsigned __int64    u_int64_t;
#endif



// on both Linux and Windows platforms, size_t is a 4-bytes unsigned integer
// on Linux, ssize_t is a 4 bytes signed integer
// Windows does not seem to have a ssize_t
typedef int32_t ssize_t;

#endif  // __STDC_CONSTANT_MACROS
#else
#include <stdint.h>
#include <sys/types.h>
#endif


// a byte will be an unsigned char, i.e. an unsigned int8_t
// this typedef works fine on both Linux and Windows
typedef unsigned char byte_t;

#endif
