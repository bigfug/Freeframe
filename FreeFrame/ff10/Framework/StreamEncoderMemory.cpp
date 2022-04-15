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

#include "StreamEncoderMemory.h"
#include <assert.h>

StreamEncoderMemory::StreamEncoderMemory( const VideoInfoStruct &pSrcInfo )
: StreamEncoder( pSrcInfo, false )
{
	mMapFile = NULL;
	mMemHdr = NULL;
	mCurrPort = 0;
	mFrameNum = 0;
	mDeadWriteCount = 0;
}

StreamEncoderMemory::~StreamEncoderMemory( void )
{
	deallocBuffers();
}

void StreamEncoderMemory::allocBuffers( void )
{
}

void StreamEncoderMemory::deallocBuffers( void )
{
	if( mMemHdr != NULL )
	{
		assert( mMemHdr->mWriters > 0 );

		InterlockedDecrement( &mMemHdr->mWriters );

		assert( mMemHdr->mWriters >= 0 );
		
		if( !UnmapViewOfFile( mMemHdr ) )
		{
		}

		mMemHdr = NULL;

		mFrameBuffer1 = NULL;
		mFrameBuffer2 = NULL;

		mWritePtr = NULL;
	}

	if( mMapFile != NULL )
	{
		CloseHandle( mMapFile );

		mMapFile = NULL;
	}
}

void StreamEncoderMemory::portOpen( void )
{
}

void StreamEncoderMemory::portClose( void )
{
}

bool StreamEncoderMemory::portConnect( void )
{
	MemoryFrame		*Frame;

	if( mCurrPort != mDstAddr.sin_port || mWritePtr == NULL )
	{
		const DWORD		FrameSize  = mFrameConvert->getOutputFrame()->getFrameByteSize();
		const DWORD		BufferSize = MEM_HDR_LEN + ( ( MEM_FRAME_LEN + FrameSize ) * NUM_MEM_FRAMES );
		wchar_t			Name[ MAX_PATH ];

		deallocBuffers();

		mCurrPort = mDstAddr.sin_port;

		swprintf_s( Name, MAX_PATH, L"FUG_MEM_DATA_%04x", mDstAddr.sin_port );

		LOG( L"ENC: Opening %s", Name );

		mMapFile = OpenFileMapping( FILE_MAP_ALL_ACCESS, FALSE, Name );

		if( mMapFile != NULL && mMapFile == INVALID_HANDLE_VALUE )
		{
			if( ( mMemHdr = (MemoryHeader *)MapViewOfFile( mMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0 ) ) == NULL )
			{
				LOG( L"ENC: Couldn't create map of file: %s\n", Name );

				CloseHandle( mMapFile );

				mMapFile = NULL;

				return( false );
			}

			if( mMemHdr->mWriters != 0 )
			{
				LOG( L"ENC: mMapFile found but mWriters > 0" );

				UnmapViewOfFile( mMemHdr );

				mMemHdr = NULL;

				CloseHandle( mMapFile );

				mMapFile = NULL;

				return( false );
			}

			if( memcmp( &mMemHdr->mFrameInfo, &mDstVidInfo, sizeof( VideoInfoStruct ) ) != 0 )
			{
				LOG( L"ENC: mMapFile found but VideoInfoStruct not the same" );

				UnmapViewOfFile( mMemHdr );

				mMemHdr = NULL;

				CloseHandle( mMapFile );

				mMapFile = NULL;

				return( false );
			}
		}
		else
		{
			mMapFile = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BufferSize, Name );

			if( mMapFile == NULL || mMapFile == INVALID_HANDLE_VALUE )
			{
				LOG( L"ENC: Couldn't create map of file: %s\n", Name );

				mMapFile = NULL;

				return( false );
			}

			//MessageBox( NULL, Name, "fugStreamSend", MB_OK );

			if( ( mMemHdr = (MemoryHeader *)MapViewOfFile( mMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BufferSize ) ) == NULL )
			{
				LOG( L"ENC: Can't map view of file" );

				CloseHandle( mMapFile );

				mMapFile = NULL;

				return( false );
			}

			mMemHdr->mWriteIndex = 0;
			mMemHdr->mReadIndex  = 0;
			mMemHdr->mWriters    = 1;
			mMemHdr->mReaders    = 0;
			mMemHdr->mFrameCount = NUM_MEM_FRAMES;
			mMemHdr->mFrameSize  = FrameSize;

			for( int i = 0 ; i < NUM_MEM_FRAMES ; i++ )
			{
				Frame = MEM_FRAME_ADDR( mMemHdr, i );

				Frame->mDataAvailable = false;
			}

			memcpy( &mMemHdr->mFrameInfo, &mDstVidInfo, sizeof( VideoInfoStruct ) );
		}

		Frame = MEM_FRAME_ADDR( mMemHdr, mMemHdr->mWriteIndex );

		mWritePtr = (BYTE *)&Frame->mData;
	}

	if( mMemHdr == NULL )
	{
		return( false );
	}

	if( mMemHdr->mReaders == 0 )
	{
		return( false );
	}

	Frame = MEM_FRAME_ADDR( mMemHdr, mMemHdr->mWriteIndex );

	return( true );
}

bool StreamEncoderMemory::portWrite( const BYTE *pBuffer )
{
	MemoryFrame *Frame = MEM_FRAME_ADDR( mMemHdr, mMemHdr->mWriteIndex );

	if( Frame->mDataAvailable )
	{
		if( ++mDeadWriteCount == 50 )
		{
			LOG( L"ENC: DeadWriteCount exceeded" );

			if( mMemHdr->mReaders > 0 )
			{
				InterlockedDecrement( &mMemHdr->mReaders );
			}

			if( mMemHdr->mReaders == 0 )
			{
				deallocBuffers();
			}

			mDeadWriteCount = 0;
		}

		return( false );
	}

	mDeadWriteCount = 0;

	Frame->mDataAvailable = true;

	if( ++mMemHdr->mWriteIndex == NUM_MEM_FRAMES )
	{
		mMemHdr->mWriteIndex = 0;
	}

	Frame = MEM_FRAME_ADDR( mMemHdr, mMemHdr->mWriteIndex );

	mWritePtr = (BYTE *)&Frame->mData;

	return( true );
}
