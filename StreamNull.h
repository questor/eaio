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
// EAStreamNull.h
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EASTREAMNULL_H) && !defined(FOUNDATION_EASTREAMNULL_H)
#define EAIO_EASTREAMNULL_H
#define FOUNDATION_EASTREAMNULL_H // For backward compatability. Eventually, we'll want to get rid of this.


#include "EAIO/internal/Config.h"
#ifndef EAIO_EASTREAM_H
    #include "EAIO/Stream.h"
#endif


namespace eaio
{
     /// class StreamNull
     ///
     /// Implements a 'bit bucket' stream, whereby all writes to the stream
     /// succeed but do nothing and all reads from the stream succeed but write
     /// nothing to the user-supplied buffer.
     ///
     class EAIO_API StreamNull : public IStream
     {
     public:
         static const uint32_t kTypeStreamNull = 0x025c9bb3;

     public:
         StreamNull();
         StreamNull(const StreamNull& x);
         StreamNull& operator=(const StreamNull& x);

         // IStream functionality
         virtual int       addRef();
         virtual int       release();
         virtual uint32_t  getType() const;
         virtual int       getAccessFlags() const;
         virtual int       getState() const;
         virtual bool      close();
         virtual size_type getSize() const;
         virtual bool      setSize(size_type size);
         virtual off_type  getPosition(PositionType positionType) const;
         virtual bool      setPosition(off_type position, PositionType positionType);
         virtual size_type getAvailable() const;
         virtual size_type read(void* pData, size_type nSize);
         virtual bool      flush();
         virtual bool      write(const void* pData, size_type nSize);

     protected:
         int mnRefCount;

     }; // class StreamNull

} // namespace eaio

#endif // Header include guard
