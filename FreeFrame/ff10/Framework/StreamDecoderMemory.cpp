/*

The MIT License

Copyright (c) 2007 Alex May - www.bigfug.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "StreamDecoderMemory.h"
#include <assert.h>

StreamDecoderMemory::StreamDecoderMemory( const VideoInfoStruct &pDstInfo )
        : StreamDecoder( pDstInfo, false )
{
    mMapFile = NULL;
    mMemHdr = NULL;
}

StreamDecoderMemory::~StreamDecoderMemory(void)
{
    deallocBuffers();
}

void StreamDecoderMemory::allocBuffers( void )
{
}

void StreamDecoderMemory::deallocBuffers( void )
{
    if ( mMemHdr != NULL )
    {
        assert( mMemHdr->mReaders > 0 );

        if ( mMemHdr->mReaders > 0 )
        {
            InterlockedDecrement( &mMemHdr->mReaders );
        }

        assert( mMemHdr->mReaders >= 0 );

        UnmapViewOfFile( mMemHdr );

        mMemHdr = NULL;
    }

    if ( mMapFile != NULL )
    {
        CloseHandle( mMapFile );

        mMapFile = NULL;
    }
}

void StreamDecoderMemory::portOpen( const UINT pPort )
{
    wchar_t		Name[ MAX_PATH ];

    deallocBuffers();

    mCurrPort = pPort;

    swprintf_s( Name, MAX_PATH, L"FUG_MEM_DATA_%04x", pPort );

    LOG( L"DEC: Opening %s", Name );

    mMapFile = OpenFileMapping( FILE_MAP_ALL_ACCESS, FALSE, Name );

    if ( mMapFile == NULL || mMapFile == INVALID_HANDLE_VALUE )
    {
        mMapFile = NULL;

        return;
    }

    if ( ( mMemHdr = (MemoryHeader *)MapViewOfFile( mMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0 ) ) == NULL )
    {
        LOG( L"DEC: Couldn't create map of file: %s\n", Name );

        return;
    }

    InterlockedIncrement( &mMemHdr->mReaders );

    LOG( L"DEC: Opened Port %d\n", mCurrPort );
}

void StreamDecoderMemory::portClose( void )
{
    deallocBuffers();
}

bool StreamDecoderMemory::portConnect( DWORD pTimeout )
{
    if ( mMemHdr == NULL )
    {
        portOpen( mCurrPort );
    }

    if ( mMemHdr == NULL )
    {
        return( false );
    }

    if ( mMemHdr->mReaders == 0 || mMemHdr->mWriters == 0 )
    {
        portClose();

        return( false );
    }

    MemoryFrame *Frame = MEM_FRAME_ADDR( mMemHdr, mMemHdr->mReadIndex );

    return( Frame->mDataAvailable );
}

void StreamDecoderMemory::portRead( BYTE *pBuffer )
{
    MemoryFrame *Frame = MEM_FRAME_ADDR( mMemHdr, mMemHdr->mReadIndex );

    if ( !Frame->mDataAvailable )
    {
        return;
    }

    if ( memcmp( &mSrcVidInfo, &mMemHdr->mFrameInfo, sizeof( VideoInfoStruct ) ) != 0 )
    {
        SAFE_DELETE( mFrameConvert );

        memcpy( &mSrcVidInfo, &mMemHdr->mFrameInfo, sizeof( VideoInfoStruct ) );

        if ( !mAdaptiveBuffer )
        {
            mFrameConvert = new FrameConvert( mSrcVidInfo, mDstVidInfo );
        }
        else
        {
            mFrameConvert = new FrameConvert( mSrcVidInfo, mSrcVidInfo );
        }
    }

    if ( mFrameConvert == NULL )
    {
        LOG( L"DEC: mFrameConvert == NULL\n" );

        return;
    }

    if ( !mAdaptiveBuffer )
    {
        mBuildBuffer = (BYTE *)&Frame->mData;

        swapBuffers();

        Frame->mDataAvailable = false;

        mCurrFrameNum++;

        if ( ++mMemHdr->mReadIndex == mMemHdr->mFrameCount )
        {
            mMemHdr->mReadIndex = 0;
        }
    }
    else
    {
        mReadPtr = (BYTE *)&Frame->mData;

        SetEvent( mNewFrameEvent );
    }
}

void StreamDecoderMemory::unlockReadBuffer( void )
{
    MemoryFrame *Frame = MEM_FRAME_ADDR( mMemHdr, mMemHdr->mReadIndex );

    if ( mAdaptiveBuffer && Frame->mDataAvailable )
    {
        Frame->mDataAvailable = false;

        mCurrFrameNum++;

        if ( ++mMemHdr->mReadIndex == mMemHdr->mFrameCount )
        {
            mMemHdr->mReadIndex = 0;
        }
    }

    StreamDecoder::unlockReadBuffer();
}
