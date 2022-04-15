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

#include "StreamEncoder.h"

StreamEncoder::StreamEncoder( const fugVideoInfo &pSrcInfo )
	: mSrcVidInfo( pSrcInfo ), mSrcVidSize( 0 ), mFrameConvert( 0 ), mFrameCompress( 0 ), mFrameNum( 0 )
{
	const size_t		PixelCount = pSrcInfo.frameWidth * pSrcInfo.frameHeight;

	switch( pSrcInfo.bitDepth )
	{
		case FUG_VID_16BITVIDEO:
			mSrcVidSize = PixelCount * 2;
			break;

		case FUG_VID_24BITVIDEO:
			mSrcVidSize = PixelCount * 3;
			break;

		case FUG_VID_32BITVIDEO:
			mSrcVidSize = PixelCount * 4;
			break;
	}

	setDstFormat( mSrcVidInfo, FUG_COMPRESS_NONE, 100 );
}

StreamEncoder::~StreamEncoder( void )
{
	if( mFrameConvert != 0 )
	{
		delete mFrameConvert;

		mFrameConvert = 0;
	}

	if( mFrameCompress != 0 )
	{
		delete mFrameCompress;

		mFrameCompress = 0;
	}
}

void StreamEncoder::setDstAddr( const char *pAddr, const unsigned short pPort )
{
	std::string					NewAddr( pAddr );

	if( NewAddr == mDstAddr && pPort == mDstPort )
	{
		return;
	}

	boost::lock_guard<boost::mutex>		Lock( mEncoderMutex );

	mDstAddr = NewAddr;
	mDstPort = pPort;

	mFlags.set( ENCODER_HAVE_DEST );
	mFlags.set( ENCODER_CHANGED_DEST );
}

void StreamEncoder::setDstFormat( const fugVideoInfo &pDstInfo, const fugStreamCompression pCompression, boost::uint16_t pLevel )
{
	if( memcmp( &pDstInfo, &mDstVidInfo, sizeof( fugVideoInfo ) ) == 0 && pCompression == mCompression && pLevel == mLevel )
	{
		return;
	}

	boost::lock_guard<boost::mutex>		Lock( mEncoderMutex );

	// Clear out the current settings

	if( mFrameConvert != 0 )
	{
		delete mFrameConvert;

		mFrameConvert = 0;
	}

	mFlags.reset( ENCODER_VALID );

	mConvertBuffer.clear();
	mCompressBuffer.clear();

	// Store the new dest format and compression

	memcpy( &mDstVidInfo, &pDstInfo, sizeof( fugVideoInfo ) );

	const size_t		PixelCount = mDstVidInfo.frameWidth * mDstVidInfo.frameHeight;

	switch( mDstVidInfo.bitDepth )
	{
		case FUG_VID_16BITVIDEO:
			mDstVidSize = PixelCount * 2;
			break;

		case FUG_VID_24BITVIDEO:
			mDstVidSize = PixelCount * 3;
			break;

		case FUG_VID_32BITVIDEO:
			mDstVidSize = PixelCount * 4;
			break;
	}

	mCompression = pCompression;
	mLevel = pLevel;

	// If the Src and Dst are the same then we don't need to convert

	if( memcmp( &mSrcVidInfo, &mDstVidInfo, sizeof( fugVideoInfo ) ) == 0 )
	{
		mFlags.set( ENCODER_VALID );
	}
	else if( ( mFrameConvert = new ImageConvert( mSrcVidInfo, mDstVidInfo ) ) != NULL )
	{
		mFlags.set( ENCODER_VALID );
	}

	if( mCompression != FUG_COMPRESS_NONE )
	{
		if( ( mFrameCompress = ImageCompression::getCompressor( mDstVidInfo, mLevel ) ) != 0 )
		{
			if( mCompressBuffer.size() != mDstVidSize )
			{
				mCompressBuffer.resize( mDstVidSize );
			}
		}
	}
	else if( mFrameCompress != 0 )
	{
		delete mFrameCompress;

		mFrameCompress = 0;
	}

	mFlags.set( ENCODER_HAVE_FORMAT );
	mFlags.set( ENCODER_CHANGED_FORMAT );
}

void StreamEncoder::encode( const unsigned char *pSrcData )
{
	boost::lock_guard<boost::mutex>		 Lock( mEncoderMutex );
	const unsigned char					*SrcDat = pSrcData;
	size_t								 SrcSze = mSrcVidSize;

	if( !mFlags[ ENCODER_VALID ] )
	{
		return;
	}

	// Do we need to convert this frame to the Dst format?

	if( mFrameConvert != 0 )
	{
		SrcSze = mFrameConvert->getOutputByteSize();

		if( mConvertBuffer.size() != SrcSze )
		{
			mConvertBuffer.resize( SrcSze );
		}

		mFrameConvert->convert( SrcDat, &mConvertBuffer[ 0 ] );

		SrcDat = &mConvertBuffer[ 0 ];
	}

	// Do we need to apply compression?

	if( mFrameCompress != 0 )
	{
		SrcSze = mFrameCompress->compress( SrcDat, &mCompressBuffer[ 0 ] );

		SrcDat = &mCompressBuffer[ 0 ];
	}

	// Pass the (converted/compressed) buffer to the actual instance

	encode( SrcDat, SrcSze );
}
