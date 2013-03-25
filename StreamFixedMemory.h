/*
Copyright (C) 2009-2010 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/////////////////////////////////////////////////////////////////////////////
// EAStreamFixedMemory.h
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Written by Talin
// Based on EAStreamMemory.h by PaulPedriana.
//
// Implements a IO stream that reads from a fixed-length block of memory.
/////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EASTREAMFIXEDMEMORY_H) && !defined(FOUNDATION_EASTREAMFIXEDMEMORY_H)
#define EAIO_EASTREAMFIXEDMEMORY_H
#define FOUNDATION_EASTREAMFIXEDMEMORY_H    // For backward compatability. Eventually, we'll want to get rid of this.


#include "EAIO/internal/Config.h"
#ifndef EAIO_EASTREAM_H
    #include "EAIO/Stream.h"
#endif


namespace eaio
{
     /// Implements an memory-based stream that supports the IStream interface.
     ///
     /// This class is not inherently thread-safe. As a result, thread-safe usage
     /// between multiple threads requires higher level coordination, such as a mutex.
     ///
     class EAIO_API FixedMemoryStream : public IStream
     {
     public:
         enum { kTypeFixedMemoryStream = 0x02f2f470 };

         FixedMemoryStream( void *pBuffer = NULL, size_type nSize = 0 );
         FixedMemoryStream( FixedMemoryStream& memoryStream );
         virtual ~FixedMemoryStream();

         int         addRef();
         int         release();

         void*       getData() const;
         bool        setData( void* pData, size_type nSize );

         size_type   getCapacity() const;

         // IStream
         uint32_t    getType() const;
         int         getAccessFlags() const;
         int         getState() const;
         bool        close();

         size_type   getSize() const;
         bool        setSize(size_type size);

         off_type    getPosition( PositionType positionType = kPositionTypeBegin ) const;
         bool        setPosition( off_type position, PositionType positionType = kPositionTypeBegin );

         size_type   getAvailable() const;
         size_type   read(void* pData, size_type nSize);

         bool        flush();
         bool        write(const void* pData, size_type nSize);

     protected:
         void         * mpData;
         int            mnRefCount;          /// Reference count. May or may not be in use.
         size_type      mnSize;              /// The size of the stream, in bytes.
         size_type      mnCapacity;          /// The size of the memory buffer, in bytes.
         size_type      mnPosition;          /// Current position within memory block.
     };

} // namespace eaio



///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////


inline eaio::FixedMemoryStream::~FixedMemoryStream() {}

inline int eaio::FixedMemoryStream::addRef()
{
    return ++mnRefCount;
}

inline int eaio::FixedMemoryStream::release()
{
    if(mnRefCount > 1)
        return --mnRefCount;
    delete this;
    return 0;
}


inline void* eaio::FixedMemoryStream::getData() const
{
    return mpData;
}


inline eaio::size_type eaio::FixedMemoryStream::getCapacity() const
{
    return mnCapacity;
}


inline uint32_t eaio::FixedMemoryStream::getType() const
{
    return kTypeFixedMemoryStream;
}


inline int eaio::FixedMemoryStream::getAccessFlags() const
{
    return kAccessFlagReadWrite;
}


inline int eaio::FixedMemoryStream::getState() const
{
    return kStateSuccess;
}


inline bool eaio::FixedMemoryStream::close()
{
    return true;
}


inline eaio::size_type eaio::FixedMemoryStream::getSize() const
{
    return mnSize;
}


inline eaio::size_type eaio::FixedMemoryStream::getAvailable() const
{
    // assert(mnPosition <= mnSize);
    return (mnSize - mnPosition);
}


inline bool eaio::FixedMemoryStream::flush()
{
    // Nothing to do.
    return true;
}

#endif // Header include guard
