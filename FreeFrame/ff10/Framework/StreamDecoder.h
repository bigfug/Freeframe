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

#pragma once

#include <winsock2.h>
#include <stdio.h>
#include <crtdbg.h>

#include "FreeFrame.h"
#include "FrameBuffer.h"
#include "FrameConvert.h"
#include "Stream.h"

class StreamDecoder;

typedef struct
{
	StreamDecoder		*mInstance;
} StreamDecoderThreadArgs;

typedef struct sockaddr_in SockAddr;

class StreamDecoder
{
protected:
	StreamDecoderThreadArgs	 mThreadArgs;
	DWORD					 mThreadId;
    HANDLE					 mThreadHandle;
	VideoInfoStruct			 mSrcVidInfo;
	VideoInfoStruct			 mDstVidInfo;
	FrameConvert			*mFrameConvert;
	volatile bool			 mRunThread;
	HANDLE					 mConfigEvent;
	HANDLE					 mFrameEvent;
	HANDLE					 mNewFrameEvent;
	BYTE					*mBuildBuffer;		// Where we build the incoming frame
	BYTE					*mFrameBuffer1;		// Double buffered reader
	BYTE					*mFrameBuffer2;		// ..
	BYTE					*mReadPtr;			// The currently used mFrameBuffer
	BYTE					*mWritePtr;			// The currently unused mFrameBuffer
	FrameData				*mPktBuffer;		// Incoming packet buffer
	USHORT					 mCurrPort;
	BYTE					 mCurrFrameNum;
	const bool				 mThreaded;
	bool					 mAdaptiveBuffer;
public:
	StreamDecoder( const VideoInfoStruct &pDstInfo, bool pThreaded );
	virtual ~StreamDecoder( void );

	void setSrcAddr( struct sockaddr_in *pAddr );

	BYTE *lockReadBuffer( DWORD pTimeout = INFINITE );
	virtual void unlockReadBuffer( void );

	bool hasNewFrame( void );
	bool waitForNewFrame( DWORD pTimeout );

	UINT getRecvWidth( void );
	UINT getRecvHeight( void );
	UINT getRecvDepth( void );
	UINT getRecvOrientation( void );

	UINT getFrameNumber( void );

	void setAdaptiveBuffer( const bool pIsAdaptive )
	{
		mAdaptiveBuffer = pIsAdaptive;
	}

protected:
	void startThread( void );
	void stopThread( void );

	void swapBuffers( void );

	static DWORD WINAPI ThreadFunc( StreamDecoderThreadArgs *pArgs );

	virtual void allocBuffers( void ) = 0;
	virtual void deallocBuffers( void ) = 0;

	virtual void portOpen( const UINT pPort ) = 0;
	virtual void portClose( void ) = 0;
	virtual bool portConnect( DWORD pTimeout = 0 ) = 0;
	virtual void portRead( BYTE *pBuffer ) = 0;
};
