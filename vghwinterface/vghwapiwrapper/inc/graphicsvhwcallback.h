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
// Graphics virtual hardware callback interface

#ifndef GRAPHICSVHWCALLBACK_H
#define GRAPHICSVHWCALLBACK_H

class MGraphicsVHWCallback
{
public:
    virtual ~MGraphicsVHWCallback() {}
    virtual void LockOutputBuffer() = 0;
    virtual void ReleaseOutputBuffer() = 0;
    virtual void ProcessingDone(int aTransactionId) = 0;
};

#endif

