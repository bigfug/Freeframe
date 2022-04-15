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

#include "StreamEncoder.h"
#include "StreamThread.h"

#include <boost/format.hpp>

typedef std::vector< std::vector<char> >	BuffArray;

typedef struct
{
	BuffArray		mBuffer;
	size_t			mDataSize;
	bool			mDataAvailable;
} BuffArrayHolder;

class StreamEncoderUDP : public StreamEncoder
{
protected:
	boost::thread				 mThread;
	boost::condition_variable	 mCondition;
	struct addrinfo				*mDstAddrInfo;
	unsigned int				 mMaxPktSze;
	BuffArrayHolder				 mSendBuff1;
	BuffArrayHolder				 mSendBuff2;
	BuffArrayHolder				*mSendBuffRead;
	BuffArrayHolder				*mSendBuffWrite;

#ifdef WIN32
	int							 mWSAresult;
#endif

public:
	StreamEncoderUDP( const fugVideoInfo &pSrcInfo );

	virtual ~StreamEncoderUDP( void );

	void operator()( void );

protected:
	bool openPort( const size_t pSize );
	void closePort( void );

	virtual void encode( const void *pData, const size_t pSize );

	void onRead( const boost::system::error_code &pError, std::size_t bytes_transferred );

	void startRead( void );
};
