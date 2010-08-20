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

#ifndef SERIALIZER_PLATFORM_H_
#define SERIALIZER_PLATFORM_H_

#ifdef __SYMBIAN32__
#include <e32const.h>
#include <e32def.h>

#ifdef __KERNEL_MODE__
#include <e32cmn.h>
#undef ASSERT(x)
#define ASSERT(x)
#define SER_IMPORT_C
#define SER_EXPORT_C
#else
#include <e32std.h>
#define SER_IMPORT_C IMPORT_C
#define SER_EXPORT_C EXPORT_C
#endif
#else
#include <wchar.h>
#include <assert.h>

#define ASSERT assert
typedef void TAny;
typedef signed char TInt8;
typedef unsigned char TUint8;
typedef short int TInt16;
typedef unsigned short int TUint16;
typedef long int TInt32;
typedef unsigned long int TUint32;
typedef signed int TInt;
typedef long int TBool;
typedef unsigned long long TUint64;
enum 
{ 
	EFalse = 0,
	ETrue = 1
};
#define SER_IMPORT_C
#define SER_EXPORT_C
#define NONSHARABLE_CLASS( x ) class x
#endif

#endif
