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

///////////////////////////////////////////////////////////////////////////////
// EAStreamCpp.h
//
// Copyright (c) 2006 Electronic Arts Inc.
// Created by Paul Pedriana
//
// Implements a stream which wraps around a C++ std::istream and std::ostream.
///////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EASTREAMCPP_H
#define EAIO_EASTREAMCPP_H


#include "EAIO/internal/Config.h"
#include "EAIO/Stream.h"
#include <iostream>


namespace eaio
{
     /// StreamCpp
     ///
     /// Implements an IStream that uses a C++ std::istream, std::ostream,
     /// and/or std::iostream as an underlying interface.
     ///
     /// In order to deal with std C++ istream, ostream, and iostream,
     /// this class works with istream and ostream independently.
     ///
     /// Example usage:
     ///    std::fstream fileStream;
     ///    StreamCpp    streamCpp(&fileStream, &fileStream);
     ///
     class EAIO_API StreamCpp : public IStream
     {
     public:
         static const uint32_t kStreamType = 0x040311cf; // Random guid.

         // Note that std::iostream inherits from istream and ostream and thus
         // you can pass an iostream as both arguments here.
         StreamCpp();
         StreamCpp(std::istream* pStdIstream, std::ostream* pStdOstream);
         StreamCpp(const StreamCpp& x);
         StreamCpp& operator=(const StreamCpp& x);

         int addRef();
         int release();

         void setStream(std::istream* pStdIstream, std::ostream* pStdOstream)
         {
             if (pStdIstream != mpStdIstream)
                 mpStdIstream = pStdIstream;

             if (pStdOstream != mpStdOstream)
                 mpStdOstream = pStdOstream;
         }

         uint32_t getType() const;

         int  getAccessFlags() const;

         int  getState() const;

         // This is a no-op as std C++ streams don't have a close() method but the IStream interface
         // has a pure-virtual Close() method.
         bool close()
         {
             // There isn't any way to close a std C++ stream. You would
             // need a higher level fstream to do something like that.
             return true;
         }

         size_type getSize() const;
         bool      setSize(size_type size);

         off_type getPosition(PositionType positionType = kPositionTypeBegin) const;
         bool     setPosition(off_type position, PositionType positionType = kPositionTypeBegin);

         size_type getAvailable() const;
         size_type read(void* pData, size_type nSize);

         bool      flush();
         bool      write(const void* pData, size_type nSize);

     protected:
         std::istream* mpStdIstream;
         std::ostream* mpStdOstream;
         int           mnRefCount;
     };

} // namespace eaio


///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////

namespace eaio
{
     inline
     StreamCpp::StreamCpp()
       : mpStdIstream(NULL),
         mpStdOstream(NULL),
         mnRefCount(0)
     { }


     inline
     StreamCpp::StreamCpp(std::istream* pStdIstream, std::ostream* pStdOstream)
       : mnRefCount(0)
     {
         setStream(pStdIstream, pStdOstream);
     }

     inline
     StreamCpp::StreamCpp(const StreamCpp& x)
         : mnRefCount(0)
     {
         setStream(x.mpStdIstream, x.mpStdOstream);
     }

     inline
     StreamCpp& StreamCpp::operator=(const StreamCpp& x)
     {
         setStream(x.mpStdIstream, x.mpStdOstream);

         return *this;
     }

} // namespace eaio

#endif // Header include guard
