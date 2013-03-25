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
// EAStreamAdapter.cpp
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
//
// Implements adapters for reading and writing basic types in an 
// endian-proper way with the EA::Stream interfaces.
//
// Written by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#include "EAIO/internal/Config.h"
#include "EAIO/Stream.h"
#include "EAIO/StreamAdapter.h"
#include <limits.h>

namespace {

    // Swizzling functions, copied from EAEndianConst.h. We don't use
    // the optimized, processor specific versions of these functions
    // defined in EndianValue.h, although perhaps we should consider
    // doing that.

    // The implementations are defined in a simplistic way so that 
    // the compiler can optimize away the logic in each function.
    // These functions are intentionally not optimized with tricks such 
    // as assembly language, compiler intrinsics, or memory tricks, as
    // such things would interfere with the compiler's ability to optimize
    // away these operations.
    inline uint16_t SwizzleUint16(uint16_t x)
    {
        return (uint16_t) ((x >> 8) | (x << 8));
    }

    inline uint32_t SwizzleUint32(uint32_t x)
    {
        // An alternative to the mechanism of using shifts and ors below
        // is to use byte addressing.
        return (uint32_t)
            ((x >> 24)               |
            ((x << 24) & 0xff000000) |
            ((x <<  8) & 0x00ff0000) |
            ((x >>  8) & 0x0000ff00)); 
    }

    inline uint64_t SwizzleUint64(uint64_t x)
    {
        return (uint64_t)
            ((x        & 0x000000ff) << 56) |
            ((x >> 56) & 0x000000ff)        |
            ((x        & 0x0000ff00) << 40) |
            ((x >> 40) & 0x0000ff00)        |
            ((x        & 0x00ff0000) << 24) |
            ((x >> 24) & 0x00ff0000)        |
            ((x        & 0xff000000) <<  8) |
            ((x >>  8) & 0xff000000);

        // Alternative implementation:
        //const uint32_t high32Bits = EASwizzleUint32((uint32_t)(x));
        //const uint32_t low32Bits  = EASwizzleUint32((uint32_t)(x >> 32));
        //return ((uint64_t)high32Bits << 32) | low32Bits;
    }
}


eaio::size_type eaio::CopyStream(IStream* pSource, IStream* pDestination, size_type nSize)
{
    char            buffer[2048];
    size_type       nCurrentCount, nRemaining;
    const size_type nSourceSize = pSource->getSize();

    if(nSourceSize == kSizeTypeError) // If the source size is of undetermined size...
        nSize = kLengthNull;          // read all of the source.
    else if(nSize > nSourceSize)      // If the user size is too high...
        nSize = nSourceSize;          // Reduce the user size to be the source size.

    for(nRemaining = nSize; nRemaining != 0; nRemaining -= nCurrentCount)
    {
        nCurrentCount = ((nRemaining >= sizeof(buffer)) ? sizeof(buffer) : nRemaining);
        nCurrentCount = pSource->read(buffer, nCurrentCount);

        if((nCurrentCount == kSizeTypeError) || !pDestination->write(buffer, nCurrentCount))
            return kSizeTypeError;

        if(nCurrentCount == 0) // If we have read the entire source...
            break;
    }

    return (nSize - nRemaining); // Return the number of bytes copied. Note that nRemaining might be non-zero.
}



bool eaio::readBool8(IStream* pIS, bool& value)
{
    bool8_t n;

    if(pIS->read(&n, sizeof(n)) == sizeof(n))
    {
        value = (n != 0);
        return true;
    }
    return false;
}


bool eaio::readInt8(IStream* pIS, int8_t& value)
{
    return pIS->read(&value, sizeof(value)) == sizeof(value);
}


bool eaio::readInt8(IStream* pIS, int8_t* value, size_type count )
{
    return pIS->read(value, count * sizeof(*value)) == count * sizeof(*value);
}


bool eaio::readUint8(IStream* pIS, uint8_t& value)
{
    return pIS->read(&value, sizeof(value)) == sizeof(value);
}


bool eaio::readUint8(IStream* pIS, uint8_t* value, size_type count )
{
    return pIS->read(value, count * sizeof(*value)) == count * sizeof(*value);
}


bool eaio::readUint16(IStream* pIS, uint16_t& value, Endian endianSource)
{
    if(pIS->read(&value, sizeof(value)) == sizeof(value))
    {
        if(endianSource != kEndianLocal)
            value = SwizzleUint16(value);
        return true;
    }
    return false;
}


bool eaio::readUint16(IStream* pIS, uint16_t* value, size_type count, Endian endianSource)
{
    if(pIS->read( value, count * sizeof(*value)) == count * sizeof(*value))
    {
        // Swizzle in place after the read
        if(endianSource != kEndianLocal)
        {
            while(count--)
            {
                const uint16_t c = *value;
                *value++ = SwizzleUint16(c); 
            }
        }
        return true;
    }
    return false;
}


bool eaio::readInt16(IStream* pIS, int16_t& value, Endian endianSource)
{
    return readUint16(pIS, (uint16_t&)value, endianSource);
}


bool eaio::readInt16(IStream* pIS, int16_t* value, size_type count, Endian endianSource)
{
    return readUint16(pIS, (uint16_t*)value, count, endianSource);
}


bool eaio::readUint32(IStream* pIS, uint32_t& value, Endian endianSource)
{
    if(pIS->read(&value, sizeof(value)) == sizeof(value))
    {
        if(endianSource != kEndianLocal)
            value = SwizzleUint32(value);
        return true;
    }
    return false;
}


bool eaio::readUint32(IStream* pIS, uint32_t* value, size_type count, Endian endianSource)
{
    if(pIS->read(value, count * sizeof(*value)) == count * sizeof(*value))
    {
        // Swizzle in place after the read
        if(endianSource != kEndianLocal)
        {
            while(count--)
            {
                const uint32_t c = *value;
                *value++ = SwizzleUint32(c);
            }
        }
        return true;
    }
    return false;
}


bool eaio::readInt32(IStream* pIS, int32_t& value, Endian endianSource)
{
    return readUint32(pIS, (uint32_t&)value, endianSource);
}


bool eaio::readInt32(IStream* pIS, int32_t* value, size_type count, Endian endianSource)
{
    return readUint32(pIS, (uint32_t*)value, count, endianSource);
}


bool eaio::readUint64(IStream* pIS, uint64_t& value, Endian endianSource)
{
    if(pIS->read(&value, sizeof(value)) == sizeof(value))
    {
        if(endianSource != kEndianLocal)
            value = SwizzleUint64(value);
        return true;
    }
    return false;
}


bool eaio::readUint64(IStream* pIS, uint64_t* value, size_type count, Endian endianSource)
{
    if(pIS->read(value, count * sizeof(*value)) == count * sizeof(*value))
    {
        // Swizzle in place after the read
        if(endianSource != kEndianLocal)
        {
            while(count--)
            {
                const uint64_t c = *value;
                *value++ = SwizzleUint64(c);
            }
        }
        return true;
    }
    return false;
}


bool eaio::readInt64(IStream* pIS, int64_t& value, Endian endianSource)
{
   return readUint64(pIS, (uint64_t&)value, endianSource);
}


bool eaio::readInt64(IStream* pIS, int64_t* value, size_type count, Endian endianSource)
{
    return readUint64(pIS, (uint64_t*)value, count, endianSource);
}


//bool eaio::readUint128(IStream* pIS, uint128_t& value, Endian endianSource)
//{
//    if(pIS->read(&value, sizeof(value)) == sizeof(value))
//    {
//       if(endianSource != kEndianLocal)
//          value = SwizzleUint128(value);
//       return true;
//    }
//    return false;
//}


//bool eaio::readInt128(IStream* pIS, int128_t& value, Endian endianSource)
//{
//   return readUint128(pIS, (uint128_t&)value, endianSource);
//}


bool eaio::readFloat(IStream* pIS, float& value, Endian endianSource)
{
    return readUint32(pIS, (uint32_t&)value, endianSource);
}


bool eaio::readFloat(IStream* pIS, float* value, size_type count, Endian endianSource)
{
    return readUint32(pIS, (uint32_t*)value, count, endianSource);
}


bool eaio::readDouble(IStream* pIS, double_t& value, Endian endianSource)
{
    return readUint64(pIS, (uint64_t&)value, endianSource);
}


bool eaio::readDouble(IStream* pIS, double_t* value, size_type count, Endian endianSource)
{
    return readUint64(pIS, (uint64_t*)value, count, endianSource);
}


eaio::size_type eaio::readString(IStream* pIS, char8_t* pBuffer, size_type nMaxCount, eaio::Endian endianSource)
{
    const off_type ninitialPosition(pIS->getPosition());

    char8_t   cCurrent;
    uint32_t  nLength(0);
    size_type nCount(0); // Number of chars returned to user.
    size_type nResult;

    if(!readUint32(pIS, nLength, endianSource))
        return kSizeTypeError;

    // If no buffer has been provided, just reset the stream and return the length.
    if(!pBuffer)
    {
        pIS->setPosition(ninitialPosition);
        return (size_type)nLength;
    }

    // Determine how many characters we'll actually read into the buffer.
    // 'nMaxCount - 1' because we want to leave room for terminating NUL.
    size_type nreadLength = (nLength < nMaxCount - 1) ? nLength : nMaxCount - 1;

    while(pBuffer && (nCount < nreadLength)) 
    {
        nResult = pIS->read(&cCurrent, sizeof(cCurrent));

        if(nResult != sizeof(cCurrent))
            break;

        *pBuffer++ = cCurrent;
        ++nCount;
    }

    // We may not have been able to read the entire string out of the stream
    // due to the nMaxCount limit, but we still want to advance the stream's
    // position to the end of the string.
    pIS->setPosition(ninitialPosition + (off_type)sizeof(uint32_t) + (off_type)nLength);

    if(pBuffer)
        *pBuffer = '\0';

    return nLength; // Note that we return nLength and not nCount.
}


eaio::size_type eaio::readString(IStream* pIS, char16_t* pBuffer, size_type nMaxCount, eaio::Endian endianSource)
{
    const off_type ninitialPosition(pIS->getPosition());

    char16_t  cCurrent;
    uint32_t  nLength(0);
    size_type nCount(0); // Number of chars returned to user.

    if(!readUint32(pIS, nLength, endianSource))
        return kSizeTypeError;

    // If no buffer has been provided, just reset the stream and return the length.
    if(!pBuffer)
    {
        pIS->setPosition(ninitialPosition);
        return (size_type)nLength;
    }

    // Determine how many characters we'll actually read into the buffer.
    // 'nMaxCount - 1' because we want to leave room for terminating NUL.
    size_type nreadLength = (nLength < nMaxCount - 1) ? nLength : nMaxCount - 1;

    while(pBuffer && (nCount < nreadLength)) 
    {
        if(!eaio::readUint16(pIS, (uint16_t&)cCurrent, endianSource))
            break;

        *pBuffer++ = cCurrent;
        ++nCount;
    }

    // We may not have been able to read the entire string out of the stream
    // due to the nMaxCount limit, but we still want to advance the stream's
    // position to the end of the string.
    pIS->setPosition(ninitialPosition + (off_type)sizeof(uint32_t) + (off_type)(nLength * sizeof(char16_t)));

    if(pBuffer)
        *pBuffer = '\0';

    return nLength; // Note that we return nLength and not nCount.
}


eaio::size_type eaio::readLine(IStream* pIS, char8_t* pLine, size_type nMaxCount)
{
    char8_t   cCurrent;
    size_type nCount(0); // Number of chars in the line, not including the line end characters(s).
    size_type nread(0);  // Number of chars successfully read from stream. Will be >= nCount (due to presence of newlines).
    size_type nResult;
    off_type  ninitialPosition(0);

    if(!pLine)
        ninitialPosition = pIS->getPosition();

    for(;;)
    {
        // We are reading one character at a time, which can be slow if the stream is 
        // not buffered. We read one character at a time because we don't want to read
        // past the end of the line and thus trigger seeks, which may not even be possible
        // for some streams.
        nResult = pIS->read(&cCurrent, sizeof(cCurrent));

        if(nResult == sizeof(cCurrent))
        {
            ++nread;

            if((cCurrent == '\r') || (cCurrent == '\n'))
            {
                // It's possible that we have a "\n" or "\r\n" sequence, and we want 
                // to read past the sequence, but not past anything else. This code here takes
                // care not to read past the first "\n" in a "\n\n" sequence, but is smart 
                // enough to read past the just first "\r\n" in a "\r\n\r\n" sequence.
                char8_t cNext = cCurrent;

                if(cCurrent == '\r') // If we have a "\r", then we read again, expecting a "\n".
                    nResult = (size_type)pIS->read(&cNext, sizeof(cNext));

                if(nResult == sizeof(cNext) && cNext != '\n')
                {
                    // We have encountered an unexpected sequence: We have a "\rx" instead of "\n" or "\r\n".
                    // This call requires a stream that can back up.
                    pIS->setPosition(-(off_type)sizeof(cNext), kPositionTypeCurrent);
                }

                break;
            }
            else
            {
                if(pLine && (nCount < (nMaxCount - 1))) // '- 1' because we want to leave room for terminating null.
                    *pLine++ = cCurrent;

                ++nCount;
            }
        }
        else
        {
            // In this case, there was nothing left to read in the file.
            // We need to differentiate between an empty line vs. nothing 
            // left to read in the file. To deal with that we return kSizeTypeDone.
            if(nread == 0)
                nCount = kSizeTypeDone;

            break;
        }
    }

    if(pLine)
        *pLine = 0;
    else
        pIS->setPosition(ninitialPosition);

    return nCount;
}


eaio::size_type eaio::readLine(IStream* pIS, char16_t* pLine, size_type nMaxCount, Endian endianSource)
{
    char16_t  cCurrent;
    size_type nCount(0); // Number of chars in the line, not including the line end characters(s).
    size_type nread(0);  // Number of chars successfully read from stream. Will be >= nCount (due to presence of newlines).
    size_type nResult;
    off_type  ninitialPosition(0);
    char16_t  cr, lf;

    if(!pLine)
        ninitialPosition = pIS->getPosition();

    if(endianSource == kEndianLocal)
    {
        cr = '\r';
        lf = '\n';
    }
    else
    {
        cr = SwizzleUint16('\r');
        lf = SwizzleUint16('\n');
    }

    for(;;)
    {
        // We are reading one character at a time, which can be slow if the stream is 
        // not buffered. We read one character at a time because we don't want to read
        // past the end of the line and thus trigger seeks, which may not even be possible
        // for some streams.
        nResult = pIS->read(&cCurrent, sizeof(cCurrent));

        if(nResult == sizeof(cCurrent))
        {
            ++nread;

            if((cCurrent == cr) || (cCurrent == lf))
            {
                // It's possible that we have a "\n" or "\r\n" sequence, and we want 
                // to read past the sequence, but not past anything else. This code here takes
                // care not to read past the first "\n" in a "\n\n" sequence, but is smart 
                // enough to read past the just first "\r\n" in a "\r\n\r\n" sequence.
                char16_t cNext = cCurrent;

                if(cCurrent == cr) // If we have a "\r", then we read again, expecting a "\n".
                    nResult = (size_type)pIS->read(&cNext, sizeof(cNext));

                if(cNext != lf)
                {
                    // We have encountered an unexpected sequence: We have a "\rx" instead of "\n" or "\r\n".
                    // This call requires a stream that can back up.
                    pIS->setPosition(-(off_type)sizeof(cNext), kPositionTypeCurrent);
                }

                break;
            }
            else
            {
                if(pLine && (nCount < (nMaxCount - 1))) // '- 1' because we want to leave room for terminating null.
                {
                    if(endianSource != kEndianLocal)
                        cCurrent = SwizzleUint16(cCurrent);

                    *pLine++ = cCurrent;
                }

                ++nCount;
            }
        }
        else
        {
            // In this case, there was nothing left to read in the file.
            // We need to differentiate between an empty line vs. nothing 
            // left to read in the file. To deal with that we return kSizeTypeDone.
            if(nread == 0)
                nCount = kSizeTypeDone;

            break;
        }
    }

    if(pLine)
        *pLine = 0;
    else
        pIS->setPosition(ninitialPosition);

    return nCount;
}


bool eaio::writeBool8(IStream* pOS, bool value)
{
    const uint8_t n(value ? 1u : 0u);
    return pOS->write(&n, sizeof(n)) == sizeof(n);
}


bool eaio::writeInt8(IStream* pOS, int8_t value)
{
    return pOS->write(&value, sizeof(value)) == sizeof(value);
}


bool eaio::writeInt8(IStream* pOS, const int8_t* value, size_type count)
{
    return pOS->write(value, count * sizeof(*value));
}


bool eaio::writeUint8(IStream* pOS, uint8_t value)
{
    return pOS->write(&value, sizeof(value)) == sizeof(value);
}


bool eaio::writeUint8(IStream* pOS, const uint8_t* value, size_type count)
{
    return pOS->write(value, count * sizeof(*value));
}


bool eaio::writeUint16(IStream* pOS, uint16_t value, Endian endianDestination)
{
    if(endianDestination != kEndianLocal)
        value = SwizzleUint16(value);

    return pOS->write(&value, sizeof(value));
}


bool eaio::writeUint16(IStream* pOS, const uint16_t* value, size_type count, Endian endianDestination)
{
    if(endianDestination != kEndianLocal)
    {
        while(count--)
        {
            const uint16_t n = SwizzleUint16(*value++);

            if(!pOS->write( &n, sizeof( n )))
                return false;
        }
    }
    else
    {
        if(!pOS->write(value, count * sizeof(*value)))
            return false;
    }

    return true;
}


bool eaio::writeInt16(IStream* pOS, int16_t value, Endian endianDestination)
{
   return writeUint16(pOS, (uint16_t)value, endianDestination);
}


bool eaio::writeInt16(IStream* pOS, const int16_t* value, size_type count, Endian endianDestination)
{
    return writeUint16(pOS, (uint16_t*)value, count, endianDestination);
}


bool eaio::writeUint32(IStream* pOS, uint32_t value, Endian endianDestination)
{
    if(endianDestination != kEndianLocal)
        value = SwizzleUint32(value);

    return pOS->write(&value, sizeof(value));
}


bool eaio::writeUint32(IStream* pOS, const uint32_t* value, size_type count, Endian endianDestination)
{
    if(endianDestination != kEndianLocal)
    {
        while(count--)
        {
            const uint32_t n = SwizzleUint32(*value++);

            if(!pOS->write(&n, sizeof(n)))
                return false;
        }
    }
    else
    {
        if(!pOS->write(value, count * sizeof(*value)))
            return false;
    }

    return true;
}


bool eaio::writeInt32(IStream* pOS, int32_t value, Endian endianDestination)
{
   return writeUint32(pOS, (uint32_t)value, endianDestination);
}


bool eaio::writeInt32(IStream* pOS, const int32_t* value, size_type count, Endian endianDestination)
{
    return writeUint32(pOS, (uint32_t*)value, count, endianDestination);
}


bool eaio::writeUint64(IStream* pOS, uint64_t value, Endian endianDestination)
{
    if(endianDestination != kEndianLocal)
        value = SwizzleUint64(value);

    return pOS->write(&value, sizeof(value));
}


bool eaio::writeUint64(IStream* pOS, const uint64_t* value, size_type count, Endian endianDestination)
{
    if(endianDestination != kEndianLocal)
    {
        while(count--)
        {
            const uint64_t n = SwizzleUint64(*value++);

            if(!pOS->write(&n, sizeof(n)))
                return false;
        }
    }
    else
    {
        if(!pOS->write(value, count * sizeof(*value)))
            return false;
    }

    return true;
}


bool eaio::writeInt64(IStream* pOS, int64_t value, Endian endianDestination)
{
   return writeUint64(pOS, (uint64_t)value, endianDestination);
}


bool eaio::writeInt64(IStream* pOS, const int64_t* value, size_type count, Endian endianDestination)
{
    return writeUint64(pOS, (uint64_t*)value, count, endianDestination);
}


//bool eaio::WriteUint128(IStream* pOS, uint128_t value, Endian endianDestination)
//{
//    if(endianDestination != kEndianLocal)
//       value = SwizzleUint128(value);
//    return pOS->Write(&value, sizeof(value));
//


//bool eaio::WriteInt128(IStream* pOS, int128_t value, Endian endianDestination)
//{
//    return WriteUint128(pOS, (uint128_t)value, endianDestination);
//}


bool eaio::writeFloat(IStream* pOS, float value, Endian endianDestination)
{
    return writeFloat(pOS, &value, 1, endianDestination);
}


bool eaio::writeFloat(IStream* pOS, const float* value, size_type count, Endian endianDestination)
{
    return writeUint32(pOS, (const uint32_t*)(const char*)value, count, endianDestination);
}


bool eaio::writeDouble(IStream* pOS, double_t value, Endian endianDestination)
{
    return writeDouble(pOS, &value, 1, endianDestination);
}


bool eaio::writeDouble(IStream* pOS, const double_t* value, size_type count, Endian endianDestination)
{
    return writeUint64(pOS, (const uint64_t*)(const char*)value, count, endianDestination);
}


bool eaio::writeString(IStream* pOS, const char8_t* pBuffer, size_t nCount, Endian endianSource)
{
    bool bResult(true);

    if(nCount == kLengthNull)
    {
        nCount = 0;

        // For maximal portability and modularity, we invent our own strlen function here.
        const char8_t* pCurrent = pBuffer;
        while(*pCurrent++)
            ++nCount;
    }

    // Embed the string's length at the nCount of the buffer.
    bResult = writeUint32(pOS, (uint32_t)nCount, endianSource);

    if(bResult && nCount)
        bResult = pOS->write(pBuffer, nCount * sizeof(char8_t));

    return bResult;
}


bool eaio::writeString(IStream* pOS, const char16_t* pBuffer, size_t nCount, Endian endianSource)
{
    bool bResult(true);

    if(nCount == kLengthNull)
    {
        nCount = 0;

        // For maximal portability and modularity, we invent our own strlen function here.
        const char16_t* pCurrent = pBuffer;
        while(*pCurrent++)
            ++nCount;
    }

    // Embed the string's length at the beginning of the buffer.
    bResult = writeUint32(pOS, (uint32_t)nCount, endianSource);

    if(bResult && nCount)
        bResult = writeUint16(pOS, (const uint16_t*)pBuffer, nCount, endianSource);

    return bResult;
}


bool eaio::writeLine(IStream* pOS, const char8_t* pLine, size_type nCount, LineEnd lineEndToUse)
{
    bool bResult(true);

    if(nCount == kLengthNull)
    {  
        nCount = 0;

        // For maximal portability and modularity, we invent our own strlen function here.
        const char8_t* pCurrent = pLine;
        while(*pCurrent++)
            ++nCount;
    }

    if(nCount)
        bResult = pOS->write(pLine, nCount * sizeof(char8_t));

    if(bResult)
    {
        if(lineEndToUse == kLineEndAuto)
        {
            if(!nCount || ((pLine[nCount - 1] != '\n') && (pLine[nCount - 1] != '\r')))
                lineEndToUse = kLineEndNewline;
        }

        if(lineEndToUse == kLineEndWindows)
        {
            const char8_t terminator[2]= { '\r', '\n' };
            bResult = pOS->write(terminator, sizeof(terminator));

        }
        else if(lineEndToUse == kLineEndNewline)
        {
            const char8_t terminator[1]= { '\n' };
            bResult = pOS->write(terminator, sizeof(terminator));
        }
    }

    return bResult;
}


bool eaio::writeLine(IStream* pOS, const char16_t* pLine, size_type nCount, LineEnd lineEndToUse, Endian endianDestination)
{
    bool bResult(true);

    if(nCount == kLengthNull)
    {
        nCount = 0;

        // For maximial portability and modularity, we invent our own strlen function here for char16_t.
        const char16_t* pCurrent = pLine;
        while(*pCurrent++)
            ++nCount;
    }

    if(nCount)
        bResult = writeUint16(pOS, (const uint16_t*)pLine, nCount, endianDestination);

    if(bResult)
    {
        if(lineEndToUse == kLineEndAuto)
        {
            if(!nCount || ((pLine[nCount - 1] != '\n') && (pLine[nCount - 1] != '\r')))
                lineEndToUse = kLineEndNewline;
        }

        if(lineEndToUse == kLineEndWindows)
        {
            const uint16_t terminator[2]= { '\r', '\n' };
            bResult = writeUint16(pOS, terminator, 2, endianDestination);

        }
        else if(lineEndToUse == kLineEndNewline)
        {
            const uint16_t terminator[1]= { '\n' };
            bResult = writeUint16(pOS, terminator, endianDestination);
        }
    }

    return bResult;
}
