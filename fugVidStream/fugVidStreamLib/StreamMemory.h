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

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "boost/date_time/posix_time/posix_time.hpp"

using namespace boost::interprocess;
using namespace boost::date_time;

#if defined( WIN32 )
#define USE_WINDOWS_SHARED_MEMORY

#include <boost/interprocess/windows_shared_memory.hpp>
#endif

typedef struct MemoryHeader
{
	uint32_t						mHeader;
	uint16_t						mHeaderSize;
	uint16_t						mVersion;
	interprocess_mutex				mMutex;
	fugVideoInfo					mFrameInfo;
	uint32_t						mFrameSize;
	uint16_t						mCompression;
	uint16_t						mFrameCount;
	volatile uint32_t				mWriteIndex;
	volatile uint32_t				mReadIndex;
	boost::posix_time::ptime		mLastRead;
	boost::posix_time::ptime		mLastWrite;

	MemoryHeader( void )
	{
		mFrameSize  = 0;
		mFrameCount = 0;
		mWriteIndex = 0;
		mReadIndex  = 0;
	}

} MemoryHeader;

typedef struct MemoryFrame
{
	volatile bool		mDataAvailable;
	volatile bool		mDataReading;
	uint32_t			mDataSize;
	volatile uint32_t	mFrameNumber;

	uint32_t			mData;
} MemoryFrame;

#define	NUM_MEM_FRAMES		( 2 )
#define MEM_HDR_LEN			( sizeof( MemoryHeader ) )
#define MEM_FRAME_LEN		( sizeof( MemoryFrame ) - sizeof( uint32_t ) )
#define MEM_FRAME_ADDR(x,y)	(MemoryFrame *)( ((uint8_t *)x) + MEM_HDR_LEN + ( y * ( sizeof( MemoryFrame ) + (x)->mFrameSize ) ) )
#define MEM_BUF_ADDR(x,y)	( &MEM_FRAME_ADDR( x, y )->mData )
