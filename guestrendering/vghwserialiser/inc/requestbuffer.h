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

#ifndef REQUEST_BUFFER_H_
#define REQUEST_BUFFER_H_

#include "serializerplatform.h"

/*
 * Base for ring buffer users
 */
class MRequestBufferBookKeepingBase
    {
public:
    virtual TUint32 GetWriteCount() = 0;
    virtual TUint32 GetReadCount() = 0;
    virtual TUint32 BufferTail() = 0;
    };

/*
 * Ring buffer writer. Data source for ring buffer.
 */
class MRequestBufferBookKeepingWriter: public MRequestBufferBookKeepingBase
    {
public:
    virtual TUint32 BufferHead() = 0;
    virtual void SetBufferHead( TUint32 aIndex ) = 0;
    virtual void IncrementWriteCount( TUint32 aWriteCount ) = 0;
    virtual void SetMaxTailIndex( TUint32 aIndex ) = 0;
    };

/*
 * Ring buffer reader. Data consumer for ring buffer.
 */
class MRequestBufferBookKeepingReader: public MRequestBufferBookKeepingBase
    {
public:
    virtual void SetBufferTail( TUint32 aIndex ) = 0;
    virtual void IncrementReadCount( TUint32 aReadCount ) = 0;
    virtual TUint32 MaxTailIndex() = 0;
    };

class RequestBufferBase
    {
public:

    /*
     * 
     */
    static TUint32 AdjustAlignment( TUint32 aIndex, const TUint32 aAlignment )
    {
    const TUint32 remainder = aIndex % aAlignment;
    if ( remainder )
        {
        aIndex += aAlignment - remainder;
        }
    return aIndex;
    }
        
protected:

    /*
     * 
     */
    RequestBufferBase( TUint32 aSize ):
        iSize( aSize )
        {}
    
protected:
    const TUint32 iSize;
    };



class RequestBufferWriter: RequestBufferBase
    {
public:

    /*
     * 
     */
    RequestBufferWriter( MRequestBufferBookKeepingWriter& aBookKeeper, TUint32 aSize ): 
        RequestBufferBase( aSize ),
        iBookKeeper( aBookKeeper )
        {}
    
    /*
     * 
     */
    void InitBuffer()
        {
        iBookKeeper.SetBufferHead( 0 );    
        iBookKeeper.SetMaxTailIndex( iSize );
        }
    
    /*
     * Does not check for free space, assure free space by using CheckForSpace
     */
    TUint32 AllocateBytes( const TUint32 aBytes )
        {
        TUint32 base = iBookKeeper.BufferHead();
        if ( base + aBytes > iSize )
            {
            iBookKeeper.SetMaxTailIndex( base );
            base = 0;
            }
        else if ( iBookKeeper.BufferTail() <= base )
            {
            iBookKeeper.SetMaxTailIndex( iSize );
            }
        return base;
        }

    /*
     * Does not check for free space, assure free space by using CheckForSpace
     */
    void CommitBytes( const TUint32 aBase, const TUint32 aBytes )
        {
        //TUint32 base = CheckIndexForWrapAround( AdjustAlignment( aBase + aBytes, 4 ) );
        TUint32 base = AdjustAlignment( aBase + aBytes, 4 );
		const TUint32 inc( base - aBase );
        iBookKeeper.SetBufferHead( base );
        iBookKeeper.IncrementWriteCount( inc );
        }

    /*
     * 
     */
    TBool CheckForSpace( const TUint32 aSpaceNeeded )
        {
        const TUint32 inputBufferHead( iBookKeeper.BufferHead() );
        const TUint32 inputBufferTail( iBookKeeper.BufferTail() );
        //Notice that tail might grow during the execution of this function
        // but it would only cause false negative as a result

        //Buffer can be empty or full
        if ( inputBufferHead == inputBufferTail )
            {
            //Check if the buffer is full or empty
            if ( iBookKeeper.GetWriteCount() - iBookKeeper.GetReadCount() )
                {
                //Buffer is full
                return EFalse;
                }
            }

        //Let's check if the SFC fits to the buffer
        const TUint32 newHeadIndex = inputBufferHead + aSpaceNeeded;// + alignmentAdjust;

        if ( inputBufferHead < inputBufferTail && newHeadIndex > inputBufferTail )
            {
            //Buffer does not have enough space
            return EFalse; 
            }
        else if ( inputBufferHead >= inputBufferTail )
            {
            if ( newHeadIndex > iSize && aSpaceNeeded > inputBufferTail )
                {
                //Buffer does not have enough space
                return EFalse;
                }
            }
        return ETrue;        
        }
    
private:
    MRequestBufferBookKeepingWriter& iBookKeeper;
    };

class RequestBufferReader: public RequestBufferBase
    {
public:
    /*
     * 
     */
    RequestBufferReader( MRequestBufferBookKeepingReader& aBookKeeper, TUint32 aSize ): 
        RequestBufferBase( aSize ),
        iBookKeeper( aBookKeeper )
        {}
    
    /*
     * 
     */
    void InitBuffer()
        {
        iBookKeeper.SetBufferTail( 0 );
        }
    
    /*
     * 
     */
    TUint32 GetReadIndex()
        {
		const TUint32 bufTail( iBookKeeper.BufferTail() );
		const TUint32 bufTail2( CheckIndexForWrapAround( bufTail ) );
        if ( bufTail != bufTail2 )
			{
			iBookKeeper.SetBufferTail( bufTail2 );
			}
		return bufTail2;
        }
    
    /*
     * 
     */
    void FreeBytes( TUint32 aBytes )
        {
		const TUint32 oldTail(iBookKeeper.BufferTail());
        TUint32 newBufferTail = AdjustAlignment( aBytes + oldTail, 4 );
		const TUint32 inc( newBufferTail - oldTail );
        newBufferTail = CheckIndexForWrapAround( newBufferTail );
        iBookKeeper.IncrementReadCount( inc );
        iBookKeeper.SetBufferTail( newBufferTail );
        }

    /*
     * 
     */
    TBool IsDataAvailable()
        {
		const TUint32 readc( iBookKeeper.GetReadCount() );
		const TUint32 writec( iBookKeeper.GetWriteCount() );
        return readc != writec;
        }   
    
    /*
     * 
     */
    TUint32 CheckIndexForWrapAround( TUint32 aIndex )
        {
        //Head is behind of tail when MaxTailIndex is applied so
        // this routine works for head, too
        if ( aIndex >= iBookKeeper.MaxTailIndex() )
            {
            return 0;
            }
        return aIndex;
        }

private:
    MRequestBufferBookKeepingReader& iBookKeeper;
    };

#endif
