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
// 

#ifndef KHRONOSAPIWRAPPERDEFS_H
#define KHRONOSAPIWRAPPERDEFS_H

#include "guestvideodriverinterfaceconstants.h"

const int KMaxStackSize( VVI_PARAMETERS_INPUT_MEMORY_SIZE );//16Mb

//Checks for error after every request and logs it
#define LOG_ERROR

// tracing
//#define PRINT_TRACES
#ifdef PRINT_TRACES
    #define TRACE(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define TRACE(format, ...)
#endif

#endif
