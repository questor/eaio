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
// EAStreamChild.h
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
//
// Implements a fixed-size read-only stream which is a 'child' of a parent
// stream. This is useful if you have a system whereby a single large file
// consists of smaller files or a single large database record consists of
// multiple sub-records and you want each sub-record to look like a standalone
// stream to the user.
//
// Written by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EASTREAMCHILD_H) && !defined(FOUNDATION_EASTREAM_CHILD_H)
#define EAIO_EASTREAMCHILD_H
#define FOUNDATION_EASTREAM_CHILD_H // For backward compatability. Eventually, we'll want to get rid of this.


#ifndef INCLUDED_base_H
    #include "eastl/base/base.h"
#endif
#include "EAIO/internal/Config.h"
#ifndef EAIO_EASTREAM_H
    #include "EAIO/Stream.h"
#endif



namespace eaio
{
     /// class StreamChild
     ///
     /// Implements a fixed-size read-only stream which is a 'child' of a parent
     /// stream. This is useful if you have a system whereby a single large file
     /// consists of smaller files or a single large database record consists of
     /// multiple sub-records and you want each sub-record to look like a standalone
     /// stream to the user.
     ///
     /// This class is not inherently thread-safe. As a result, thread-safe usage
     /// between multiple threads requires higher level coordination, such as a mutex.
     ///
     class EAIO_API StreamChild : public IStream
     {
     public:
         enum { kTypeStreamChild = 0x3472233a };

         StreamChild(IStream* pStreamParent = NULL, size_type nPosition = 0, size_type nSize = 0);
        ~StreamChild();

         IStream*  getStream() const;
         bool      setStream(IStream* pStream);

         int       addRef();
         int       release();
         bool      open(IStream* pStreamParent, size_type nPosition, size_type nSize);
         bool      close();
         uint32_t  getType() const { return kTypeStreamChild; }
         int       getAccessFlags() const;
         int       getState() const;
         size_type getSize() const;
         bool      setSize(size_type);
         off_type  getPosition(PositionType positionType = kPositionTypeBegin) const;
         bool      setPosition(off_type position, PositionType positionType = kPositionTypeBegin);

         size_type getAvailable() const;
         size_type read(void* pData, size_type nSize);

         bool      flush();
         bool      write(const void* pData, size_type nSize);

     protected:
         int         mnRefCount;
         int         mnAccessFlags;
         IStream*    mpStreamParent;
         size_type   mnPositionParent;
         size_type   mnPosition;
         size_type   mnSize;
     };

} // namespace eaio

#endif // Header include guard
