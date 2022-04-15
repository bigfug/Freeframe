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

#define NOSHLWAPI

#include "StreamDecoder.h"
#include <stdio.h>

typedef struct
{
	std::vector<uint8_t>		mBuffer;
	size_t				mDataReceived;;
	bool				mDataAvailable;
	uint8_t				mFrameNumber;
} FrameHolder;

class StreamDecoderUDP : public StreamDecoder
{
protected:
	boost::thread				 mThread;
	boost::condition_variable	 mCondition;
	SOCKET						 mSocket;
	std::vector<char>				 mPkt;
	FrameHolder					 mFrame1;
	FrameHolder					 mFrame2;
	FrameHolder					*mFrameRead;
	FrameHolder					*mFrameWrite;
	unsigned int				 mMaxPktSze;
	
#ifdef WIN32
	int							 mWSAresult;
#endif

public:
	StreamDecoderUDP( const fugVideoInfo &pDstInfo );

	virtual ~StreamDecoderUDP( void );

	virtual bool hasNewFrame( void );

	virtual bool waitForNewFrame( uint32_t pTimeout );

	void operator()( void );

protected:
	virtual uint8_t *lockFrame( uint32_t pTimeout, size_t &pSize );

	virtual void freeFrame( void );

	bool openPort( void );

	void closePort( void );

	void read( unsigned long pWait );
};
