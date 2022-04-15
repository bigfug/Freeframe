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

#include "FreeFrame.h"

typedef struct
{
	VideoInfoStruct		mFrameInfo;
	DWORD				mFrameSize;
	DWORD				mFrameCount;
	volatile LONG		mWriteIndex;
	volatile LONG		mReadIndex;
	volatile LONG		mWriters;
	volatile LONG		mReaders;

} MemoryHeader;

typedef struct MemoryFrame
{
	volatile bool		mDataAvailable;
	DWORD				mData;
} MemoryFrame;

#define	NUM_MEM_FRAMES		( 2 )
#define MEM_HDR_LEN			( sizeof( MemoryHeader ) )
#define MEM_FRAME_LEN		( sizeof( MemoryFrame ) - sizeof( DWORD ) )
#define MEM_FRAME_ADDR(x,y)	(MemoryFrame *)( ((BYTE *)x) + MEM_HDR_LEN + ( y * ( sizeof( MemoryFrame ) + (x)->mFrameSize ) ) )
#define MEM_BUF_ADDR(x,y)	( &MEM_FRAME_ADDR( x, y )->mData )