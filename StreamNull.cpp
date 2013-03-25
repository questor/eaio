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
// EAStreamNull.cpp
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#include "EAIO/internal/Config.h"
#include "EAIO/StreamNull.h"


namespace eaio {

///////////////////////////////////////////////////////////////////////////////
// StreamNull
//
StreamNull::StreamNull()
  : mnRefCount(0)
{
}


///////////////////////////////////////////////////////////////////////////////
// StreamNull
//
StreamNull::StreamNull(const StreamNull&)
  : mnRefCount(0)
{
}


///////////////////////////////////////////////////////////////////////////////
// operator=
//
StreamNull& StreamNull::operator=(const StreamNull&)
{
    return *this;
}


///////////////////////////////////////////////////////////////////////////////
// GetType
//
uint32_t StreamNull::getType() const
{
    return kTypeStreamNull;
}


///////////////////////////////////////////////////////////////////////////////
// AddRef
//
int StreamNull::addRef()
{
    // There should be no need to manipulate the mpStream refcount here.
    return ++mnRefCount;
}


///////////////////////////////////////////////////////////////////////////////
// Release
//
int StreamNull::release()
{
    if(mnRefCount > 1)
        return --mnRefCount;
    delete this;
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// GetAccessFlags
//
int StreamNull::getAccessFlags() const
{
    return kAccessFlagReadWrite;
}


///////////////////////////////////////////////////////////////////////////////
// GetState
//
int StreamNull::getState() const
{
    return kStateSuccess;
}


///////////////////////////////////////////////////////////////////////////////
// Close
//
bool StreamNull::close()
{
    // To consider: Have the Close function cause future accesses to fail.
    return true;
}


///////////////////////////////////////////////////////////////////////////////
// GetSize
//
size_type StreamNull::getSize() const
{
    // To consider; Have the GetSize function return a count of the 
    // number of bytes that have been written.
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// SetSize
//
bool StreamNull::setSize(size_type)
{
    return true;
}


///////////////////////////////////////////////////////////////////////////////
// GetPosition
//
off_type StreamNull::getPosition(PositionType /*positionType*/) const
{
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// SetPosition
//
bool StreamNull::setPosition(off_type /*nPosition*/, PositionType /*positionType*/)
{
    return true;
}

          

///////////////////////////////////////////////////////////////////////////////
// GetAvailable
//
size_type StreamNull::getAvailable() const
{
    return 0x0fffffff;
}


///////////////////////////////////////////////////////////////////////////////
// read
//
size_type StreamNull::read(void* /*pData*/, size_type nSize)
{
    // We don't do this, as it merely burns CPU cycles that have no meaning.
    // memset(pData, 0, nSize);
    return nSize;
}


///////////////////////////////////////////////////////////////////////////////
// Flush
//
bool StreamNull::flush()
{
    return true;
}


///////////////////////////////////////////////////////////////////////////////
// Write
//
bool StreamNull::write(const void* /*pData*/, size_type /*nSize*/)
{
    return true;
}


} // namespace eaio
