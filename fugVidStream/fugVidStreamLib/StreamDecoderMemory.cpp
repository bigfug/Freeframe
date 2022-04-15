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

#if defined( WIN32 )
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ObjBase.h>
#endif

StreamDecoderMemory::StreamDecoderMemory( const fugVideoInfo &pDstInfo )
	: StreamDecoder( pDstInfo ), mMemHdr( 0 )
{
#ifdef USE_WINDOWS_SHARED_MEMORY
	mSharedMemory = 0;
#endif
}

StreamDecoderMemory::~StreamDecoderMemory(void)
{
	freeMemory();
}

void StreamDecoderMemory::freeMemory( void )
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

//		shared_memory_object::remove( mSharedName );
#endif
	}
	catch( ... )
	{
	}
}

bool StreamDecoderMemory::openMemory( void )
{
	if( !mFlags[ DECODER_HAVE_SOURCE ] )
	{
		return( false );
	}

	if( mFlags[ DECODER_CHANGED_SOURCE ] )
	{
		freeMemory();

		mFlags.reset( DECODER_CHANGED_SOURCE );
	}

	if( mMemHdr != 0 )
	{
		return( true );
	}

	sprintf( mSharedName, "FUG_VID_MEM_DATA_%d", mCurrPort );

	try
	{
#ifdef USE_WINDOWS_SHARED_MEMORY
		mSharedMemory = new windows_shared_memory( open_only, mSharedName, read_write );

		mMappedRegion = mapped_region( *mSharedMemory, read_write );
#else
		mSharedMemory = shared_memory_object( open_only, mSharedName, read_write );

		mMappedRegion = mapped_region( mSharedMemory, read_write );
#endif

		if( ( mMemHdr = static_cast<MemoryHeader *>( mMappedRegion.get_address() ) ) == 0 )
		{
			freeMemory();

			return( false );
		}

		if( mMemHdr->mHeader != HEADER_VALUE || mMemHdr->mHeaderSize != sizeof( MemoryHeader ) || mMemHdr->mVersion != HEADER_VERSION )
		{
			freeMemory();

			return( false );
		}

		if( mMemHdr->mFrameSize != mDstVidSize && !mFlags[ DECODER_ADAPTIVE_BUFFER ] )
		{
			freeMemory();

			return( false );
		}
	}
	catch( ... )
	{
		freeMemory();

		return( false );
	}

	return( true );
}

bool StreamDecoderMemory::hasNewFrame( void )
{
	if( mMemHdr != 0 && mMemHdr->mLastRead - mMemHdr->mLastWrite >= boost::posix_time::seconds( 2 ) )
	{
		freeMemory();

		return( false );
	}

	if( mMemHdr == 0 && !openMemory() )
	{
		return( false );
	}

	MemoryFrame *Frame = MEM_FRAME_ADDR( mMemHdr, mMemHdr->mReadIndex );

	return( Frame->mDataAvailable );
}

bool StreamDecoderMemory::waitForNewFrame( uint32_t pTimeout )
{
	(void)pTimeout;

	return( hasNewFrame() );
}

uint8_t *StreamDecoderMemory::lockFrame( uint32_t pTimeout, size_t &pSize )
{
	(void)pTimeout;

	pSize = 0;

	if( mMemHdr != 0 && mMemHdr->mLastRead - mMemHdr->mLastWrite >= boost::posix_time::seconds( 2 ) )
	{
		freeMemory();

		return( false );
	}

	if( mMemHdr == 0 && !openMemory() )
	{
		return( 0 );
	}

	try
	{
		scoped_lock<interprocess_mutex>		Lock( mMemHdr->mMutex );

		mMemHdr->mLastRead = boost::posix_time::second_clock::universal_time();

		MemoryFrame *Frame = MEM_FRAME_ADDR( mMemHdr, mMemHdr->mReadIndex );

		if( !Frame->mDataAvailable )
		{
			return( 0 );
		}

		if( memcmp( &mSrcVidInfo, &mMemHdr->mFrameInfo, sizeof( fugVideoInfo ) ) != 0 || mCompression != (fugStreamCompression)mMemHdr->mCompression )
		{
			setSrcFormat( mMemHdr->mFrameInfo, (fugStreamCompression)mMemHdr->mCompression );
		}

		if( !mFlags[ DECODER_VALID ] )
		{
			return( 0 );
		}

		Frame->mDataReading = true;

		mCurrFrameNum = Frame->mFrameNumber;

		pSize = Frame->mDataSize;

		return( (uint8_t *)&Frame->mData );
	}
	catch( ... )
	{
	}

	return( 0 );
}

void StreamDecoderMemory::freeFrame( void )
{
	if( mMemHdr == 0 )
	{
		return;
	}

	try
	{
		scoped_lock<interprocess_mutex>		Lock( mMemHdr->mMutex );

		MemoryFrame *Frame = MEM_FRAME_ADDR( mMemHdr, mMemHdr->mReadIndex );

		Frame->mDataAvailable = false;
		Frame->mDataReading   = false;

		if ( ++mMemHdr->mReadIndex == mMemHdr->mFrameCount )
		{
			mMemHdr->mReadIndex = 0;
		}
	}
	catch( ... )
	{
	}
}
