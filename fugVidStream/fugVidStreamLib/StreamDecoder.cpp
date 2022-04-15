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

#include "StreamDecoder.h"
#include <assert.h>

StreamDecoder::StreamDecoder( const fugVideoInfo &pDstInfo )
    : mDstVidInfo( pDstInfo ), mFrameConvert( 0 ), mFrameCompress( 0 ), mCurrPort( 0 ), mCurrFrameNum( 0 ), mCompression( FUG_COMPRESS_NONE )
{
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

	mCompressedFrameSize = mDstVidSize;

	mConvertBuffer.resize( mDstVidSize );
}

StreamDecoder::~StreamDecoder( void )
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

void StreamDecoder::setSrcAddr( const uint16_t pPort )
{
	if( pPort == mCurrPort )
	{
		return;
	}

	boost::lock_guard<boost::mutex>		Lock( mDecoderMutex );

	mCurrPort = pPort;

	mFlags.set( DECODER_HAVE_SOURCE );
	mFlags.set( DECODER_CHANGED_SOURCE );

	mFlags.reset( DECODER_VALID );
}

void StreamDecoder::setSrcFormat( const fugVideoInfo &pSrcInfo, const fugStreamCompression pCompression )
{
	boost::lock_guard<boost::mutex>		Lock( mDecoderMutex );

	memcpy( &mSrcVidInfo, &pSrcInfo, sizeof( fugVideoInfo ) );

	size_t	SrcPixCnt = mSrcVidInfo.frameWidth * mSrcVidInfo.frameHeight;

	mSrcVidSize = 0;

	switch( mSrcVidInfo.bitDepth )
	{
		case FUG_VID_16BITVIDEO:
			mSrcVidSize = SrcPixCnt * 2;
			break;

		case FUG_VID_24BITVIDEO:
			mSrcVidSize = SrcPixCnt * 3;
			break;

		case FUG_VID_32BITVIDEO:
			mSrcVidSize = SrcPixCnt * 4;
			break;
	}

	if( mFrameCompress != 0 )
	{
		delete mFrameCompress;

		mFrameCompress = 0;
	}

	if( pCompression != FUG_COMPRESS_NONE )
	{
		mCompression = pCompression;

		if( ( mFrameCompress = ImageCompression::getDecompressor( pSrcInfo ) ) != 0 )
		{
			if( mDecompressBuffer.size() != mSrcVidSize )
			{
				mDecompressBuffer.resize( mSrcVidSize );
			}
		}
	}
	else
	{
		mCompression = FUG_COMPRESS_NONE;

		mDecompressBuffer.clear();
	}

	if( mFrameConvert != 0 )
	{
		delete mFrameConvert;

		mFrameConvert = 0;
	}

	if( mFlags[ DECODER_ADAPTIVE_BUFFER ] )
	{
		memcpy( &mDstVidInfo, &mSrcVidInfo, sizeof( fugVideoInfo ) );
	}
	else if( memcmp( &mSrcVidInfo, &mDstVidInfo, sizeof( fugVideoInfo ) ) != 0 )
	{
		if( ( mFrameConvert = new ImageConvert( mSrcVidInfo, mDstVidInfo ) ) == 0 )
		{
			return;
		}
	}

	mFlags.set( DECODER_VALID );
}

uint8_t *StreamDecoder::lockReadBuffer( uint32_t pTimeout )
{
	assert( !mFlags[ DECODER_INSTANCE_LOCKED ] );

	uint8_t		*SrcDat;
	size_t		 SrcSze = 0;

	if( ( SrcDat = lockFrame( pTimeout, SrcSze ) ) == 0 )
	{
		return( 0 );
	}

	mCompressedFrameSize = SrcSze;

	mFlags.set( DECODER_INSTANCE_LOCKED );

	if( mFrameCompress != 0 )
	{
		mFlags.reset( DECODER_INSTANCE_LOCKED );

		mCompressedFrameSize = SrcSze;

		mFrameCompress->decompress( SrcDat, SrcSze, &mDecompressBuffer[ 0 ] );

		SrcDat = &mDecompressBuffer[ 0 ];
		SrcSze = mSrcVidSize;
	}

	if( mFrameConvert != 0 )
	{
		mFlags.reset( DECODER_INSTANCE_LOCKED );

		mFrameConvert->convert( SrcDat, &mConvertBuffer[ 0 ] );

		SrcDat = &mConvertBuffer[ 0 ];
		SrcSze = mDstVidSize;
	}

	if( !mFlags[ DECODER_INSTANCE_LOCKED ] )
	{
		freeFrame();
	}

	return( SrcDat );
}

void StreamDecoder::unlockReadBuffer( void )
{
	if( mFlags[ DECODER_INSTANCE_LOCKED ] )
	{
		freeFrame();

		mFlags.reset( DECODER_INSTANCE_LOCKED );
	}
}

uint16_t StreamDecoder::getRecvWidth( void )
{
	return( !mFlags[ DECODER_VALID ] ? 0 : mSrcVidInfo.frameWidth );
}

uint16_t StreamDecoder::getRecvHeight( void )
{
	return( !mFlags[ DECODER_VALID ] ? 0 : mSrcVidInfo.frameHeight );
}

uint8_t StreamDecoder::getRecvDepth( void )
{
	return( !mFlags[ DECODER_VALID ] ? 0 : mSrcVidInfo.bitDepth );
}

uint8_t StreamDecoder::getRecvOrientation( void )
{
	return( !mFlags[ DECODER_VALID ] ? 0 : mSrcVidInfo.orientation );
}

uint8_t StreamDecoder::getFrameNumber( void )
{
	return( mCurrFrameNum );
}

size_t StreamDecoder::getCompressedFrameSize()
{
	return( mCompressedFrameSize );
}

void StreamDecoder::setAdaptiveBuffer( const bool pIsAdaptive )
{
	mFlags[ DECODER_ADAPTIVE_BUFFER ] = pIsAdaptive;
}
