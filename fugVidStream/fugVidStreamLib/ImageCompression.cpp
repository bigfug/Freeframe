
#include "ImageCompression.h"
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <algorithm>

using namespace std;

ImageCompression::ImageCompression( const fugVideoInfo &pVidInf )
	: mVidInf( pVidInf ), mSrcSze( 0 )
#if defined( USING_COMPRESSION )
	, mHandle( 0 ), mPixFmt( -1 ), mCmpBuf( 0 ), mCmpSze( 0 )
#endif
{
	const size_t		PixelCount = mVidInf.frameWidth * mVidInf.frameHeight;

	mPitchSze = mVidInf.frameWidth;

	switch( mVidInf.bitDepth )
	{
		case FUG_VID_8BITVIDEO:
			mSrcSze = PixelCount;
#if defined( USING_COMPRESSION )
			mPixFmt = TJPF_GRAY;
#endif
			break;

		case FUG_VID_16BITVIDEO:
			mSrcSze = PixelCount * 2;
			mPitchSze *= 2;
#if defined( USING_COMPRESSION )
			mPixFmt = -1;
#endif
			break;

		case FUG_VID_24BITVIDEO:
			mSrcSze = PixelCount * 3;
			mPitchSze *= 3;
#if defined( USING_COMPRESSION )
			mPixFmt = TJPF_RGB;
#endif
			break;

		case FUG_VID_32BITVIDEO:
			mSrcSze = PixelCount * 4;
			mPitchSze *= 4;
#if defined( USING_COMPRESSION )
			mPixFmt = TJPF_RGBX;
#endif
			break;
	}
}

ImageCompression::~ImageCompression( void )
{
#if defined( USING_COMPRESSION )
	if( mHandle != 0 )
	{

	}
#endif
}

ImageCompression *ImageCompression::getCompressor( const fugVideoInfo &pVidInf, uint16_t pLevel )
{
	ImageCompression		*IC = new ImageCompression( pVidInf );

	if( IC == 0 )
	{
		return( 0 );
	}

	IC->mCompressor = true;
	IC->mLevel      = pLevel;

#if defined( USING_COMPRESSION )
	if( IC->mPixFmt != -1 )
	{
		if( ( IC->mHandle = tjInitCompress() ) == 0 )
		{
			delete IC;

			return( 0 );
		}
	}
#endif

	return( IC );
}

ImageCompression *ImageCompression::getDecompressor( const fugVideoInfo &pVidInf )
{
	ImageCompression		*IC = new ImageCompression( pVidInf );

	if( IC == 0 )
	{
		return( 0 );
	}

	IC->mCompressor = false;

#if defined( USING_COMPRESSION )
	if( IC->mPixFmt != -1 )
	{
		if( ( IC->mHandle = tjInitDecompress() ) == 0 )
		{
			delete IC;

			return( 0 );
		}
	}
#endif

	return( IC );
}

size_t ImageCompression::compress( const void *pSrcDat, void *pDstDat )
{
#if defined( USING_COMPRESSION )
	if( mHandle != 0 )
	{
		// TJFLAG_BOTTOMUP

		if( tjCompress2( mHandle, (unsigned char *)pSrcDat, mVidInf.frameWidth, mPitchSze, mVidInf.frameHeight, mPixFmt, &mCmpBuf, &mCmpSze, TJSAMP_422, mLevel, 0 ) == 0 )
		{
#if defined( _DEBUG )
			//printf( "compressed %d -> %d\n", mSrcSze, mCmpSze );
#endif

			if( mCmpBuf != 0 && mCmpSze < mSrcSze )
			{
				memcpy( pDstDat, mCmpBuf, mCmpSze );

				return( mCmpSze );
			}
		}
	}
#endif

	memcpy( pDstDat, pSrcDat, mSrcSze );

	return( mSrcSze );
}

void ImageCompression::decompress( const void *pSrcDat, const size_t pSrcSze, void *pDstDat )
{
#if defined( USING_COMPRESSION )
	if( mHandle != 0 )
	{
		if( tjDecompress2( mHandle, (unsigned char *)pSrcDat, pSrcSze, (unsigned char *)pDstDat, mVidInf.frameWidth, mPitchSze, mVidInf.frameHeight, mPixFmt, TJFLAG_NOREALLOC ) == 0 )
		{
			return;
		}

#if defined( _DEBUG )
		printf( "decompress: %s\n", tjGetErrorStr() );
#endif
	}
#endif

	//size_t		CpySze = min<size_t>( max<size_t>( mSrcSze, 0 ), max<size_t>( pSrcSze, 0 ) );

	//memcpy( pDstDat, pSrcDat, CpySze );
}
