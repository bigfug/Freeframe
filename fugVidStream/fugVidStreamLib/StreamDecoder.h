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

#include <stdio.h>
#include <bitset>
#include "Stream.h"
#include "fugVidStream.h"
#include "ImageConvert.h"
#include "ImageCompression.h"

//#include <boost/interprocess/managed_shared_memory.hpp>

enum StreamDecoderFlags
{
	DECODER_VALID,
	DECODER_HAVE_SOURCE,
	DECODER_CHANGED_SOURCE,
	DECODER_ADAPTIVE_BUFFER,
	DECODER_INSTANCE_LOCKED,
	DECODER_FLAG_COUNT
};

class StreamDecoder
{
protected:
	boost::mutex				 mDecoderMutex;
	fugVideoInfo				 mSrcVidInfo;
	fugVideoInfo				 mDstVidInfo;
	ImageConvert				*mFrameConvert;
	ImageCompression			*mFrameCompress;
	uint16_t					 mCurrPort;
	uint8_t						 mCurrFrameNum;
	size_t						 mSrcVidSize;
	size_t						 mDstVidSize;
	std::vector<uint8_t>				 mConvertBuffer;
	std::vector<uint8_t>				 mDecompressBuffer;
	fugStreamCompression		 mCompression;
	std::bitset<DECODER_FLAG_COUNT>		 mFlags;
	size_t								 mCompressedFrameSize;

public:
	StreamDecoder( const fugVideoInfo &pDstInfo );
	virtual ~StreamDecoder( void );

	void setSrcAddr( const unsigned short pPort );

	uint8_t *lockReadBuffer( uint32_t pTimeout = INFINITE );
	virtual void unlockReadBuffer( void );

	virtual bool hasNewFrame( void ) = 0;
	virtual bool waitForNewFrame( uint32_t pTimeout ) = 0;

	uint16_t getRecvWidth( void );
	uint16_t getRecvHeight( void );
	uint8_t getRecvDepth( void );
	uint8_t getRecvOrientation( void );

	uint8_t getFrameNumber( void );

	size_t getCompressedFrameSize( void );

	void setAdaptiveBuffer( const bool pIsAdaptive );

protected:
	void setSrcFormat( const fugVideoInfo &pSrcInfo, const fugStreamCompression pCompression );

	virtual uint8_t *lockFrame( uint32_t pTimeout, size_t &pSize ) = 0;

	virtual void freeFrame( void ) = 0;
};
