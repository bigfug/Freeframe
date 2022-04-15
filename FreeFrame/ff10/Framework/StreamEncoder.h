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

class StreamEncoder;

typedef struct
{
	StreamEncoder		*mInstance;
} StreamEncoderThreadArgs;

typedef struct sockaddr_in SockAddr;

class StreamEncoder
{
protected:
	StreamEncoderThreadArgs	 mThreadArgs;
	DWORD					 mThreadId;
    HANDLE					 mThreadHandle;
	SockAddr				 mDstAddr;
	VideoInfoStruct			 mSrcVidInfo;
	VideoInfoStruct			 mDstVidInfo;
	FrameConvert			*mFrameConvert;
	volatile bool			 mRunThread;
    HANDLE					 mWriteEvent;
	HANDLE					 mConfigEvent;
	BYTE					*mFrameBuffer1, *mFrameBuffer2;
	BYTE					*mLastFrame;
	BYTE					*mWritePtr;
	FrameData				*mPktBuffer1, *mPktBuffer2;
	const bool				 mThreaded;
	int						 mFirstLine, mLastLine;
	UINT					 mWriteFreq;
public:
	StreamEncoder( const VideoInfoStruct &pSrcInfo, bool pThreaded );
	virtual ~StreamEncoder( void );

	void setDstAddr( struct sockaddr_in *pAddr );
	void setDstFormat( VideoInfoStruct &pDstInfo );
	void encode( VideoFrame pSrcData );
protected:
	void startThread( void );
	void stopThread( void );

	static DWORD WINAPI ThreadFunc( StreamEncoderThreadArgs *pArgs );

	virtual void allocBuffers( void ) = 0;
	virtual void deallocBuffers( void ) = 0;

	virtual void portOpen( void ) = 0;
	virtual void portClose( void ) = 0;
	virtual bool portConnect( void ) = 0;
	virtual bool portWrite( const BYTE *pBuffer ) = 0;
};
