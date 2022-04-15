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

#include "Stream.h"
#include "fugVidStream.h"
#include "ImageConvert.h"
#include "ImageCompression.h"
#include <bitset>

#include <boost/array.hpp>

//using namespace boost;

enum StreamEncoderFlags
{
	ENCODER_VALID,
	ENCODER_HAVE_DEST,
	ENCODER_HAVE_FORMAT,
	ENCODER_CHANGED_DEST,
	ENCODER_CHANGED_FORMAT,
	ENCODER_HAVE_DATA,
	ENCODER_FLAG_COUNT
};

class StreamEncoder
{
protected:
	std::string						 mDstAddr;
	unsigned short					 mDstPort;
	const fugVideoInfo				 mSrcVidInfo;
	fugVideoInfo					 mDstVidInfo;
	size_t							 mSrcVidSize;
	size_t							 mDstVidSize;
	ImageConvert					*mFrameConvert;
	ImageCompression				*mFrameCompress;
	std::vector<uint8_t>			 mConvertBuffer;
	std::vector<uint8_t>			 mCompressBuffer;
	boost::mutex					 mEncoderMutex;
	fugStreamCompression			 mCompression;
	uint16_t						 mLevel;
	size_t							 mCompressedSize;
	std::bitset<ENCODER_FLAG_COUNT>	 mFlags;
	uint8_t							 mFrameNum;

public:
	StreamEncoder( const fugVideoInfo &pSrcInfo );
	virtual ~StreamEncoder( void );

	void setDstAddr( const char *pAddr, const unsigned short pPort );
	void setDstFormat( const fugVideoInfo &pDstInfo, const fugStreamCompression pCompression, boost::uint16_t pLevel );
	void encode( const unsigned char *pSrcData );

protected:
	virtual void encode( const void *pData, const size_t pSize ) = 0;
};
