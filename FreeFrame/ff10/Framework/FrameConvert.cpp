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

#include "FrameConvert.h"

FrameConvert::FrameConvert( VideoInfoStruct &pSrcInfo, VideoInfoStruct &pDstInfo )
{
	VideoInfoStruct		VidInfo;

	mFrameBufferSource  = ffFrameBufferFactory::getInstance( &pSrcInfo );

	mFrameDataConverted = new VideoPixel32bit[ pSrcInfo.frameWidth * pSrcInfo.frameHeight ];

	memcpy( &VidInfo, &pDstInfo, sizeof( VideoInfoStruct ) ) ;

	VidInfo.bitDepth    = FF_CAP_32BITVIDEO;

	mFrameBufferResized = ffFrameBufferFactory::getInstance( &VidInfo );

	mFrameDataResized   = new VideoPixel32bit[ mFrameBufferResized->getPixelCount() ];

	VidInfo.bitDepth    = pDstInfo.bitDepth;

	mFrameBufferOutput  = ffFrameBufferFactory::getInstance( &VidInfo );
}

FrameConvert::FrameConvert( UINT pSrcWidth, UINT pSrcHeight, UINT pSrcDepth, UINT pDstWidth, UINT pDstHeight, UINT pDstDepth )
{
	VideoInfoStruct		VidInfo;

	VidInfo.frameWidth  = pSrcWidth;
	VidInfo.frameHeight = pSrcHeight;
	VidInfo.bitDepth    = pSrcDepth;

	mFrameBufferSource  = ffFrameBufferFactory::getInstance( &VidInfo );

	mFrameDataConverted = new VideoPixel32bit[ pSrcWidth * pSrcHeight ];

	VidInfo.frameWidth  = pDstWidth;
	VidInfo.frameHeight = pDstHeight;
	VidInfo.bitDepth    = FF_CAP_32BITVIDEO;

	mFrameBufferResized = ffFrameBufferFactory::getInstance( &VidInfo );

	mFrameDataResized   = new VideoPixel32bit[ mFrameBufferResized->getPixelCount() ];

	VidInfo.bitDepth    = pDstDepth;

	mFrameBufferOutput  = ffFrameBufferFactory::getInstance( &VidInfo );
}

FrameConvert::~FrameConvert( void )
{
	SAFE_DELETE( mFrameBufferOutput );
	SAFE_DELETE( mFrameDataResized );
	SAFE_DELETE( mFrameBufferResized );
	SAFE_DELETE( mFrameDataConverted );
	SAFE_DELETE( mFrameBufferSource );
}

void FrameConvert::convert( BYTE *pSrcData, BYTE *pDstData )
{
	BYTE		*DstPtr = pDstData;

	convert( pSrcData, &DstPtr );

	if( DstPtr != pDstData )
	{
		memcpy( pDstData, DstPtr, mFrameBufferOutput->getFrameByteSize() );
	}
}

void FrameConvert::convert( BYTE *pSrcData, BYTE **pDstPtr )
{
	ffFrameBuffer		*SrcBuf = this->mFrameBufferSource;
	BYTE				*SrcPtr = pSrcData;
			
	switch( mFrameBufferSource->getBitDepth() )
	{
		case 16:
			SrcBuf->convert16to32( (VideoPixel16bit *)SrcPtr, this->mFrameDataConverted );

			SrcPtr = (BYTE *)this->mFrameDataConverted;
			break;

		case 24:
			SrcBuf->convert24to32( (VideoPixel24bit *)SrcPtr, this->mFrameDataConverted );

			SrcPtr = (BYTE *)this->mFrameDataConverted;
			break;
	}

	if( mFrameBufferSource->getWidth() != mFrameBufferOutput->getWidth() || mFrameBufferSource->getHeight() != mFrameBufferOutput->getHeight() )
	{
		//SrcBuf->resizeNearestNeighbour( (VideoPixel32bit *)SrcPtr, this->mFrameDataResized, mFrameBufferOutput->getWidth(), mFrameBufferOutput->getHeight() );
		SrcBuf->resizeBilinear( (VideoPixel32bit *)SrcPtr, this->mFrameDataResized, mFrameBufferOutput->getWidth(), mFrameBufferOutput->getHeight() );

		SrcBuf = this->mFrameBufferResized;
		SrcPtr = (BYTE *)this->mFrameDataResized;
	}

	switch( mFrameBufferOutput->getBitDepth() )
	{
		case 8:
			{
				int					 Remaining = mFrameBufferOutput->getPixelCount();
				VideoPixel32bit		*PixPtr    = (VideoPixel32bit *)SrcPtr;
				BYTE				*DstPtr    = *pDstPtr;

				while( Remaining-- > 0 )
				{
					const int		Min = min( PixPtr->red, min( PixPtr->green, PixPtr->blue ) );
					const int		Max = max( PixPtr->red, max( PixPtr->green, PixPtr->blue ) );
					const int		MaxPlusMin = Max + Min;

					*DstPtr++ = MaxPlusMin / 2;

					PixPtr++;
				}
			}
			break;

		case 16:
			SrcBuf->convert32to16( (VideoPixel32bit *)SrcPtr, reinterpret_cast<VideoPixel16bit *>( *pDstPtr ) );
			break;

		case 24:
			SrcBuf->convert32to24( (VideoPixel32bit *)SrcPtr, reinterpret_cast<VideoPixel24bit *>( *pDstPtr ) );
			break;

		case 32:
			*pDstPtr = SrcPtr;
			break;
	}
}
