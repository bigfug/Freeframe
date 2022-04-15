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

#if defined( WIN32 )
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ObjBase.h>
#endif

StreamEncoderMemory::StreamEncoderMemory( const fugVideoInfo &pSrcInfo )
: StreamEncoder( pSrcInfo )
{
	mMemHdr = NULL;
	mCurrPort = 0;

#ifdef USE_WINDOWS_SHARED_MEMORY
	mSharedMemory = 0;
#endif
}

StreamEncoderMemory::~StreamEncoderMemory( void )
{
	freeMemory();
}

void StreamEncoderMemory::freeMemory( void )
{
	try
	{
		mMemHdr = 0;

		mMappedRegion = mapped_region();

#ifdef USE_WINDOWS_SHARED_MEMORY
		if( mSharedMemory != 0 )
		{
			delete mSharedMemory;

			mSharedMemory = 0;
		}
#else
		mSharedMemory = shared_memory_object();

		shared_memory_object::remove( mSharedName );
#endif
	}
	catch( ... )
	{
	}
}

bool StreamEncoderMemory::openMemory( const size_t pSize )
{
	(void)pSize;

	MemoryFrame		*Frame;

	if( mMemHdr != 0 && mCurrPort == mDstPort )
	{
		return( true );
	}

	if( mMemHdr != 0 )
	{
		freeMemory();
	}

	const uint32_t		BufferSize = MEM_HDR_LEN + ( ( MEM_FRAME_LEN + mDstVidSize ) * NUM_MEM_FRAMES );

	mCurrPort = mDstPort;

	sprintf( mSharedName, "FUG_VID_MEM_DATA_%d", mDstPort );

	try
	{
#ifdef USE_WINDOWS_SHARED_MEMORY
		mSharedMemory = new windows_shared_memory( open_only, mSharedName, read_write );

		mMappedRegion = mapped_region( *mSharedMemory, read_write );
#else
		mSharedMemory = shared_memory_object( open_only, mSharedName, read_write );

		mSharedMemory.truncate( BufferSize );

		mMappedRegion = mapped_region( mSharedMemory, read_write );
#endif

		if( ( mMemHdr = static_cast<MemoryHeader *>( mMappedRegion.get_address() ) ) == 0 )
		{
			freeMemory();

			return( false );
		}

		if( mMemHdr->mHeader != HEADER_VALUE || mMemHdr->mHeaderSize != sizeof( MemoryHeader ) || mMemHdr->mVersion != HEADER_VERSION || mMemHdr->mFrameSize != mDstVidSize )
		{
			freeMemory();

			return( false );
		}

		if( memcmp( &mMemHdr->mFrameInfo, &mDstVidInfo, sizeof( fugVideoInfo ) ) != 0 || mCompression != mMemHdr->mCompression )
		{
			freeMemory();

			return( false );
		}

		mMemHdr->mLastWrite = boost::posix_time::second_clock::universal_time();
	}
	catch( ... )
	{

	}

	if( mMemHdr != NULL )
	{
		return( true );
	}

	try
	{
#ifdef USE_WINDOWS_SHARED_MEMORY
		mSharedMemory = new windows_shared_memory( create_only, mSharedName, read_write, BufferSize );

		mMappedRegion = mapped_region( *mSharedMemory, read_write );
#else
		mSharedMemory = shared_memory_object( create_only, mSharedName, read_write );

		mSharedMemory.truncate( BufferSize );

		mMappedRegion = mapped_region( mSharedMemory, read_write );
#endif

		mMemHdr = new( mMappedRegion.get_address() ) MemoryHeader;

		mMemHdr->mHeader      = HEADER_VALUE;
		mMemHdr->mHeaderSize  = sizeof( MemoryHeader );
		mMemHdr->mVersion     = HEADER_VERSION;
		mMemHdr->mFrameCount  = NUM_MEM_FRAMES;
		mMemHdr->mFrameSize   = mDstVidSize;
		mMemHdr->mCompression = mCompression;

		mMemHdr->mLastWrite   = boost::posix_time::second_clock::universal_time();

		for( int i = 0 ; i < NUM_MEM_FRAMES ; i++ )
		{
			Frame = MEM_FRAME_ADDR( mMemHdr, i );

			Frame->mDataAvailable = false;
			Frame->mDataReading   = false;
			Frame->mFrameNumber   = 0;
			Frame->mDataSize      = mMemHdr->mFrameSize;
		}

		memcpy( &mMemHdr->mFrameInfo, &mDstVidInfo, sizeof( fugVideoInfo ) );
	}
	catch( ... ) //std::exception &e )
	{
		freeMemory();

		//std::cerr << "DEC: couldn't open shared memory: " << e.what();

		return( false );
	}

	if( mMemHdr == NULL )
	{
		return( false );
	}

	return( true );
}

void StreamEncoderMemory::encode( const void *pData, const size_t pSize )
{
	if( mFlags[ ENCODER_CHANGED_DEST ] || mFlags[ ENCODER_CHANGED_FORMAT ] )
	{
		freeMemory();

		mFlags.reset( ENCODER_CHANGED_DEST );
		mFlags.reset( ENCODER_CHANGED_FORMAT );
	}

	if( !openMemory( pSize ) )
	{
		mFrameNum++;

		return;
	}

	try
	{
		scoped_lock<interprocess_mutex>		Lock( mMemHdr->mMutex );

		MemoryFrame *Frame1 = MEM_FRAME_ADDR( mMemHdr, mMemHdr->mWriteIndex );
		MemoryFrame *Frame2 = MEM_FRAME_ADDR( mMemHdr, mMemHdr->mReadIndex );

		memcpy( &Frame1->mData, pData, pSize );

		Frame1->mDataSize = pSize;
		Frame1->mFrameNumber = mFrameNum;

		Frame1->mDataAvailable = true;

		if( !Frame2->mDataReading )
		{
			mMemHdr->mReadIndex = mMemHdr->mWriteIndex;

			if( ++mMemHdr->mWriteIndex == NUM_MEM_FRAMES )
			{
				mMemHdr->mWriteIndex = 0;
			}
		}
#if defined( _DEBUG )
		else
		{
			std::cout << "!Frame2->mDataReading" << std::endl;
		}
#endif

		mMemHdr->mLastWrite = boost::posix_time::second_clock::universal_time();
	}
	catch( ... )
	{
	}

	mFrameNum++;
}
