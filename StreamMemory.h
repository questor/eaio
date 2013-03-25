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
// EAStreamMemory.h
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana
//
// Implements a IO stream that reads and writes to a block of memory.
/////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EASTREAMMEMORY_H) && !defined(FOUNDATION_EASTREAMMEMORY_H)
#define EAIO_EASTREAMMEMORY_H
#define FOUNDATION_EASTREAMMEMORY_H // For backward compatability. Eventually, we'll want to get rid of this.


#include "EAIO/internal/Config.h"
#ifndef EAIO_EASTREAM_H
    #include "EAIO/Stream.h"
#endif
#ifndef EAIO_ZONEOBJECT_H
    #include "EAIO/internal/EAIOZoneObject.h"
#endif
#include <string.h>
#include <stddef.h>



namespace eaio
{
     /// SharedPointer
     ///
     /// Implements a basic ref-counted pointer.
     ///
     /// This class is meant to be used like a COM object. When this object's
     /// reference count goes to zero, the memory it holds is deleted and then
     /// this object calls 'delete this'. This class is similar to but doesn't
     /// work exactly the same as Boost's shared_ptr template.
     /// A typical usage pattern is like so:
     ///     SharedPointer* pSP = new SharedPointer(1000);
     ///     pSP->AddRef();
     ///     pSP->Release();
     ///
     class EAIO_API SharedPointer : public eaio::Allocator::EAIOZoneObject
     {
     public:
         typedef eastl::allocator Allocator;

         SharedPointer(void* pData = NULL, bool bFreeData = true, Allocator* pAllocator = NULL);
         SharedPointer(size_type nSize, const char* pName = NULL);
         SharedPointer(size_type nSize, Allocator* pAllocator, const char* pName = NULL);

         void*      getPointer();
         int        addRef();
         int        release();
         Allocator* getAllocator() const;

     protected:
         Allocator*  mpAllocator;
         uint8_t*    mpData;
         int         mnRefCount;
         bool        mbFreeData; // If true, we free the data when done.
     };


     /// class MemoryIOStream
     ///
     /// Implements an memory-based stream that supports the IStream interface.
     ///
     /// This class is not inherently thread-safe. As a result, thread-safe usage
     /// between multiple threads requires higher level coordination, such as a mutex.
     ///
     class EAIO_API MemoryStream : public IStream
     {
     public:
         typedef eastl::allocator Allocator;

         enum { kTypeMemoryStream = 0x347223d2 };

         /// enum Options
         /// Specifies policies regarding the internal operation of this class.
         enum Options
         {
             kOptionNone            =  0,    /// No options
             kOptionResizeEnabled   =  1,    /// 0 or 1. Default is disabled.  If set, then the buffer is automatically resized on beyond-bounds position sets, beyond-bounds writes, and beyond-bounds SetSize calls.
             kOptionResizeFactor    =  4,    /// 1.0+    Default is 1.5.       Specifies how much a resize multiplies in size; is applied before kOptionResizeIncrement. Can be 1.0 if kOptionResizeIncrement > 0.
             kOptionResizeIncrement =  8,    /// 0.0+    Default is 0.0.       Specifies how much a resize increments; is applied after kOptionResizeFactor. Can be set to zero if kOptionResizeFactor is > 1.
             kOptionResizeMaximum   = 16     /// 0+      Default is 0.         Specifies the maximum size (0 = unlimited).
           //kOptionClearNewMemory  = 32     /// 0 or 1. Default is 0.         If set, then newly allocated space is cleared to zero. Otherwise the space is left as it comes from the memory allocator.
         };

         MemoryStream(SharedPointer* pSharedPointer = NULL, size_type nSize = 0, const char* pName = NULL);
         MemoryStream(void* pData, size_type nSize, bool bUsePointer, bool bFreePointer = true, Allocator* pAllocator = NULL, const char* pName = NULL); // If bUsePointer is true, then we take over ownership of pData instead of copying from it.
         MemoryStream(const MemoryStream& memoryStream);
         virtual ~MemoryStream();

         int     addRef();
         int     release();

         float   getOption(Options option) const;
         void    setOption(Options option, float fValue);

         void       setAllocator(Allocator* pAllocator);
         Allocator* getAllocator() const;

         void*   getData() const;
         bool    setData(SharedPointer* pSharedPointer, size_type nSize);
         bool    setData(void* pData, size_type nSize, bool bUsePointer, bool bFreePointer = true, Allocator *pAllocator = NULL);

         SharedPointer* getSharedPointer();

         size_type   getCapacity() const;
         bool        setCapacity(size_type size);

         // IStream
         uint32_t    getType() const;
         int         getAccessFlags() const;
         int         getState() const;
         bool        close();

         size_type   getSize() const;
         bool        setSize(size_type size);

         off_type    getPosition(PositionType positionType = kPositionTypeBegin) const;
         bool        setPosition(off_type position, PositionType positionType = kPositionTypeBegin);

         size_type   getAvailable() const;
         size_type   read(void* pData, size_type nSize);

         bool        flush();
         bool        write(const void* pData, size_type nSize);

     protected:
         bool reallocBuffer(size_type nSize);

         SharedPointer* mpSharedPointer;     /// Pointer to memory block.
         Allocator*     mpAllocator;         /// Allocator.
         const char*    mpName;              /// Memory allocation name.
         int            mnRefCount;          /// Reference count. May or may not be in use.
         size_type      mnSize;              /// The size of the stream, in bytes.
         size_type      mnCapacity;          /// The size of the memory buffer, in bytes.
         size_type      mnPosition;          /// Current position within memory block.
       //bool           mbClearNewMemory;    /// True if clearing of newly allocated memory is enabled.
         bool           mbResizeEnabled;     /// True if resizing is enabled.
         float          mfResizeFactor;      /// Specifies how capacity is increased.
         int            mnResizeIncrement;   /// Specifies how capacity is increased.
         int            mnResizeMax;         /// Maximum resize amount
     };

} // namespace eaio





///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// SharedPointer
///////////////////////////////////////////////////////////////////////////////

inline void* eaio::SharedPointer::getPointer()
{
    return mpData;
}


inline eaio::SharedPointer* eaio::MemoryStream::getSharedPointer()
{
    return mpSharedPointer;
}


inline int eaio::SharedPointer::addRef()
{
    return ++mnRefCount;
}


inline eaio::SharedPointer::Allocator* eaio::SharedPointer::getAllocator() const
{
    return mpAllocator;
}



///////////////////////////////////////////////////////////////////////////////
// MemoryStream
///////////////////////////////////////////////////////////////////////////////

inline eaio::MemoryStream::~MemoryStream()
{
    if(mpSharedPointer)
        mpSharedPointer->release();
}


inline int eaio::MemoryStream::addRef()
{
    return ++mnRefCount;
}


inline int eaio::MemoryStream::release()
{
    if(mnRefCount > 1)
        return --mnRefCount;
    delete this;
    return 0;
}


inline void eaio::MemoryStream::setAllocator(Allocator* pAllocator)
{
    mpAllocator = pAllocator;
}


inline eaio::MemoryStream::Allocator* eaio::MemoryStream::getAllocator() const
{
    return mpAllocator;
}


inline void* eaio::MemoryStream::getData() const
{
    if(mpSharedPointer)
        return mpSharedPointer->getPointer();
    return 0;
}


inline eaio::size_type eaio::MemoryStream::getCapacity() const
{
    return mnCapacity;
}


inline uint32_t eaio::MemoryStream::getType() const
{
    return kTypeMemoryStream;
}


inline int eaio::MemoryStream::getAccessFlags() const
{
    return kAccessFlagReadWrite;
}


inline int eaio::MemoryStream::getState() const
{
    return kStateSuccess;
}


inline bool eaio::MemoryStream::close()
{
    return setData(NULL, 0, false);
}


inline eaio::size_type eaio::MemoryStream::getSize() const
{
    return mnSize;
}


inline eaio::size_type eaio::MemoryStream::getAvailable() const
{
    // assert(mnPosition <= mnSize);
    return (mnSize - mnPosition);
}


inline bool eaio::MemoryStream::flush()
{
    // Nothing to do.
    return true;
}

#endif // Header include guard
