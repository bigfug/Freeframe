
#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "Plugin.h"
#include "Instance.h"

#define PIXEL_STEP	10

ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	FrameBuffer = ffFrameBufferFactory::getInstance( VideoInfo );

	mLumaBuff = new VideoPixel8bit[ FrameBuffer->getPixelCount() ];
	mTempBuff = new VideoPixel32bit[ FrameBuffer->getPixelCount() ];
}

///////////////////////////////////////////////////////////////////////////////////////
// processFrameCopy
//
// processes a frame of video from one buffer to another 
//
// parameters:
// 32-bit pointer to a structure containing a pointer to an array of input
// buffers, the number of input buffers, and a pointer to an output frame
// buffer
//
// return values (DWORD):
// FF_SUCCESS
// FF_FAIL on error
//

inline float getLuma( const VideoPixel32bit &pPixel )
{
	const int		Min = min( pPixel.red, min( pPixel.green, pPixel.blue ) );
	const int		Max = max( pPixel.red, max( pPixel.green, pPixel.blue ) );
	const int		MaxPlusMin = Max + Min;

	return( (float)( MaxPlusMin / 2 ) / 255.0f );
}

DWORD ffInstance::processFrameCopy( ProcessFrameCopyStruct *pFrameData )
{
	VideoPixel32bit		 Pixel;
	float				 Luma;
	const	int			 CenterX = (DWORD)( videoInfo.frameWidth * this->getParamFloat( FF_PARAM_CENTERX ) );
	const	int			 CenterY = (DWORD)( videoInfo.frameHeight * this->getParamFloat( FF_PARAM_CENTERY ) );
	const	DWORD		 PixelSkip = this->getParamInt( FF_PARAM_SKIP );
	const	float		 Distance  = this->getParamFloat( FF_PARAM_DISTANCE );
	const	bool		 Invert    = this->getParamBool( FF_PARAM_INVERT );
	const	bool		 DrawLines = this->getParamBool( FF_PARAM_LINES );
	const	float		 LumaEffect = 1.0f - this->getParamFloat( FF_PARAM_LUMA_EFFECT );
//	const	bool		 PARAM_EDGE = this->getParamBool( FF_PARAM_EDGE );
	const	bool		 PARAM_EDGE = false;
	const	int			 HALFW = videoInfo.frameWidth / 2;
	const	int			 HALFY = videoInfo.frameWidth / 2;
	register int		 x, y;

	this->FrameBuffer->toLuma( pFrameData->InputFrames[ 0 ].Vid32, mLumaBuff );

	FrameBuffer->clear( pFrameData->OutputFrame.Vid32 );

	for( x = 0 ; x <= CenterX ; x += PixelSkip )
	{
		for( y = 0 ; y <= CenterY ; y += PixelSkip )
		{
			const int SrcOff = ( y * FrameBuffer->getWidth() ) + x;

			Pixel = pFrameData->InputFrames[ 0 ].Vid32[ SrcOff ];
			Luma  = (float)mLumaBuff[ SrcOff ] / 255.0f;

			Pixel.alpha = 0xff;

			if( PARAM_EDGE )
			{
				float	xEdge = fabs( (float)x - (float)CenterX ) / (float)CenterX;
				float	yEdge = fabs( (float)y - (float)CenterY ) / (float)CenterY;

				Luma = max( Luma, max( xEdge, yEdge ) );
			}

			if( Invert )
			{
				Luma = 1.0f - Luma;
			}

			Luma = Luma + ( LumaEffect * ( 1.0f - Luma ) );

			const int x1 = CenterX - (DWORD)( (float)( CenterX - x ) * Luma );
			const int y1 = CenterY - (DWORD)( (float)( CenterY - y ) * Luma );

			if( DrawLines )
			{
				FrameBuffer->DrawLine32bit( pFrameData->OutputFrame.Vid32, x1, y1, CenterX, CenterY, Pixel, Distance );
			}
			else
			{
				pFrameData->OutputFrame.Vid32[ ( y1 * FrameBuffer->getWidth() ) + x1 ] = Pixel;
			}
		}
	}

	for( x = videoInfo.frameWidth - 1 ; x >= CenterX ; x -= PixelSkip )
	{
		for( y = 0 ; y <= CenterY ; y += PixelSkip )
		{
			const int SrcOff = ( y * FrameBuffer->getWidth() ) + x;

			Pixel = pFrameData->InputFrames[ 0 ].Vid32[ SrcOff ];
			Luma  = (float)mLumaBuff[ SrcOff ] / 255.0f;

			Pixel.alpha = 0xff;

			if( PARAM_EDGE )
			{
				float	xEdge = fabs( (float)x - (float)CenterX ) / (float)CenterX;
				float	yEdge = fabs( (float)y - (float)CenterY ) / (float)CenterY;

				Luma = max( Luma, max( xEdge, yEdge ) );
			}

			if( Invert )
			{
				Luma = 1.0f - Luma;
			}

			Luma = Luma + ( LumaEffect * ( 1.0f - Luma ) );

			const int x1 = CenterX + (DWORD)( (float)( x - CenterX ) * Luma );
			const int y1 = CenterY - (DWORD)( (float)( CenterY - y ) * Luma );

			if( DrawLines )
			{
				FrameBuffer->DrawLine32bit( pFrameData->OutputFrame.Vid32, x1, y1, CenterX, CenterY, Pixel, Distance );
			}
			else
			{
				pFrameData->OutputFrame.Vid32[ ( y1 * FrameBuffer->getWidth() ) + x1 ] = Pixel;
			}
		}
	}

	for( x = 0 ; x <= CenterX ; x += PixelSkip )
	{
		for( y = videoInfo.frameHeight - 1 ; y >= CenterY ; y -= PixelSkip )
		{
			const int SrcOff = ( y * FrameBuffer->getWidth() ) + x;

			Pixel = pFrameData->InputFrames[ 0 ].Vid32[ SrcOff ];
			Luma  = (float)mLumaBuff[ SrcOff ] / 255.0f;

			Pixel.alpha = 0xff;

			if( PARAM_EDGE )
			{
				float	xEdge = fabs( (float)x - (float)CenterX ) / (float)CenterX;
				float	yEdge = fabs( (float)y - (float)CenterY ) / (float)CenterY;

				Luma = max( Luma, max( xEdge, yEdge ) );
			}

			if( Invert )
			{
				Luma = 1.0f - Luma;
			}

			Luma = Luma + ( LumaEffect * ( 1.0f - Luma ) );

			const int x1 = CenterX - (DWORD)( (float)( CenterX - x ) * Luma );
			const int y1 = CenterY + (DWORD)( (float)( y - CenterY ) * Luma );

			if( DrawLines )
			{
				FrameBuffer->DrawLine32bit( pFrameData->OutputFrame.Vid32, x1, y1, CenterX, CenterY, Pixel, Distance );
			}
			else
			{
				pFrameData->OutputFrame.Vid32[ ( y1 * FrameBuffer->getWidth() ) + x1 ] = Pixel;
			}
		}
	}

	for( x = videoInfo.frameWidth - 1 ; x >= CenterX ; x -= PixelSkip )
	{
		for( y = videoInfo.frameHeight - 1 ; y >= CenterY ; y -= PixelSkip )
		{
			const int SrcOff = ( y * FrameBuffer->getWidth() ) + x;

			Pixel = pFrameData->InputFrames[ 0 ].Vid32[ SrcOff ];
			Luma  = (float)mLumaBuff[ SrcOff ] / 255.0f;

			Pixel.alpha = 0xff;

			if( PARAM_EDGE )
			{
				float	xEdge = fabs( (float)x - (float)CenterX ) / (float)CenterX;
				float	yEdge = fabs( (float)y - (float)CenterY ) / (float)CenterY;

				Luma = max( Luma, max( xEdge, yEdge ) );
			}

			if( Invert )
			{
				Luma = 1.0f - Luma;
			}

			Luma = Luma + ( LumaEffect * ( 1.0f - Luma ) );

			const int x1 = CenterX + (DWORD)( (float)( x - CenterX ) * Luma );
			const int y1 = CenterY + (DWORD)( (float)( y - CenterY  ) * Luma );

			if( DrawLines )
			{
				FrameBuffer->DrawLine32bit( pFrameData->OutputFrame.Vid32, x1, y1, CenterX, CenterY, Pixel, Distance );
			}
			else
			{
				pFrameData->OutputFrame.Vid32[ ( y1 * FrameBuffer->getWidth() ) + x1 ] = Pixel;
			}
		}
	}

	return( FF_SUCCESS );
}

DWORD ffInstance::processFrame( VideoFrame pFrame )
{
	VideoPixel32bit		 Pixel;
	float				 Luma;
	const	int					 CenterX = (DWORD)( videoInfo.frameWidth * this->getParamFloat( FF_PARAM_CENTERX ) );
	const	int					 CenterY = (DWORD)( videoInfo.frameHeight * this->getParamFloat( FF_PARAM_CENTERY ) );
	const	DWORD				 PixelSkip = this->getParamInt( FF_PARAM_SKIP );
	const	float				 Distance  = this->getParamFloat( FF_PARAM_DISTANCE );
	const	bool				 Invert    = this->getParamBool( FF_PARAM_INVERT );
	const	bool				 DrawLines = this->getParamBool( FF_PARAM_LINES );
	const	float				 LumaEffect = 1.0f - this->getParamFloat( FF_PARAM_LUMA_EFFECT );
//	const	bool				 PARAM_EDGE = this->getParamBool( FF_PARAM_EDGE );
	const	bool				 PARAM_EDGE = false;
	const	int					 HALFW = videoInfo.frameWidth / 2;
	const	int					 HALFY = videoInfo.frameWidth / 2;
	register int				 x, y;
/*
	VideoPixel32bit		*DstPtr = pFrame.Vid32;

	for( y = 0 ; y < videoInfo.frameHeight ; y++ )
	{
		for( x = 0 ; x < videoInfo.frameWidth ; x++ )
		{
			float	xEdge = fabs( (float)x - (float)CenterX ) / (float)CenterX;
			float	yEdge = fabs( (float)y - (float)CenterY ) / (float)CenterY;
			float	iMax  = max( xEdge, yEdge );
			float	iMin  = min( xEdge, yEdge );
			float	iRan  = iMax - iMin;
			float	eDist = iMin + ( iRan * iMin );

			DstPtr->red = (BYTE)min( 255.0f, eDist * 255.0f );
			DstPtr->green = DstPtr->blue = DstPtr->red;
			DstPtr->alpha = 0xff;

			DstPtr++;
		}
	}

	if( true ) return( FF_SUCCESS );
*/
	this->FrameBuffer->toLuma( pFrame.Vid32, mLumaBuff );

	this->FrameBuffer->clear( mTempBuff );

	for( x = 0 ; x <= CenterX ; x += PixelSkip )
	{
		for( y = 0 ; y <= CenterY ; y += PixelSkip )
		{
			const int SrcOff = ( y * FrameBuffer->getWidth() ) + x;

			Pixel = pFrame.Vid32[ SrcOff ];
			Luma  = (float)mLumaBuff[ SrcOff ] / 255.0f;

			Pixel.alpha = 0xff;

			if( PARAM_EDGE )
			{
				float	xEdge = fabs( (float)x - (float)CenterX ) / (float)CenterX;
				float	yEdge = fabs( (float)y - (float)CenterY ) / (float)CenterY;

				Luma = max( Luma, max( xEdge, yEdge ) );
			}

			if( Invert )
			{
				Luma = 1.0f - Luma;
			}

			Luma = Luma + ( LumaEffect * ( 1.0f - Luma ) );

			const int x1 = CenterX - (DWORD)( (float)( CenterX - x ) * Luma );
			const int y1 = CenterY - (DWORD)( (float)( CenterY - y ) * Luma );

			if( DrawLines )
			{
				FrameBuffer->DrawLine32bit( mTempBuff, x1, y1, CenterX, CenterY, Pixel, Distance );
			}
			else
			{
				mTempBuff[ ( y1 * FrameBuffer->getWidth() ) + x1 ] = Pixel;
			}
		}
	}

	for( x = videoInfo.frameWidth - 1 ; x >= CenterX ; x -= PixelSkip )
	{
		for( y = 0 ; y <= CenterY ; y += PixelSkip )
		{
			const int SrcOff = ( y * FrameBuffer->getWidth() ) + x;

			Pixel = pFrame.Vid32[ SrcOff ];
			Luma  = (float)mLumaBuff[ SrcOff ] / 255.0f;

			Pixel.alpha = 0xff;

			if( PARAM_EDGE )
			{
				float	xEdge = fabs( (float)x - (float)CenterX ) / (float)CenterX;
				float	yEdge = fabs( (float)y - (float)CenterY ) / (float)CenterY;

				Luma = max( Luma, max( xEdge, yEdge ) );
			}

			if( Invert )
			{
				Luma = 1.0f - Luma;
			}

			Luma = Luma + ( LumaEffect * ( 1.0f - Luma ) );

			const int x1 = CenterX + (DWORD)( (float)( x - CenterX ) * Luma );
			const int y1 = CenterY - (DWORD)( (float)( CenterY - y ) * Luma );

			if( DrawLines )
			{
				FrameBuffer->DrawLine32bit( mTempBuff, x1, y1, CenterX, CenterY, Pixel, Distance );
			}
			else
			{
				mTempBuff[ ( y1 * FrameBuffer->getWidth() ) + x1 ] = Pixel;
			}
		}
	}

	for( x = 0 ; x <= CenterX ; x += PixelSkip )
	{
		for( y = videoInfo.frameHeight - 1 ; y >= CenterY ; y -= PixelSkip )
		{
			const int SrcOff = ( y * FrameBuffer->getWidth() ) + x;

			Pixel = pFrame.Vid32[ SrcOff ];
			Luma  = (float)mLumaBuff[ SrcOff ] / 255.0f;

			Pixel.alpha = 0xff;

			if( PARAM_EDGE )
			{
				float	xEdge = fabs( (float)x - (float)CenterX ) / (float)CenterX;
				float	yEdge = fabs( (float)y - (float)CenterY ) / (float)CenterY;

				Luma = max( Luma, max( xEdge, yEdge ) );
			}

			if( Invert )
			{
				Luma = 1.0f - Luma;
			}

			Luma = Luma + ( LumaEffect * ( 1.0f - Luma ) );

			const int x1 = CenterX - (DWORD)( (float)( CenterX - x ) * Luma );
			const int y1 = CenterY + (DWORD)( (float)( y - CenterY ) * Luma );

			if( DrawLines )
			{
				FrameBuffer->DrawLine32bit( mTempBuff, x1, y1, CenterX, CenterY, Pixel, Distance );
			}
			else
			{
				mTempBuff[ ( y1 * FrameBuffer->getWidth() ) + x1 ] = Pixel;
			}
		}
	}

	for( x = videoInfo.frameWidth - 1 ; x >= CenterX ; x -= PixelSkip )
	{
		for( y = videoInfo.frameHeight - 1 ; y >= CenterY ; y -= PixelSkip )
		{
			const int SrcOff = ( y * FrameBuffer->getWidth() ) + x;

			Pixel = pFrame.Vid32[ SrcOff ];
			Luma  = (float)mLumaBuff[ SrcOff ] / 255.0f;

			Pixel.alpha = 0xff;

			if( PARAM_EDGE )
			{
				float	xEdge = fabs( (float)x - (float)CenterX ) / (float)CenterX;
				float	yEdge = fabs( (float)y - (float)CenterY ) / (float)CenterY;

				Luma = max( Luma, max( xEdge, yEdge ) );
			}

			if( Invert )
			{
				Luma = 1.0f - Luma;
			}

			Luma = Luma + ( LumaEffect * ( 1.0f - Luma ) );

			const int x1 = CenterX + (DWORD)( (float)( x - CenterX ) * Luma );
			const int y1 = CenterY + (DWORD)( (float)( y - CenterY  ) * Luma );

			if( DrawLines )
			{
				FrameBuffer->DrawLine32bit( mTempBuff, x1, y1, CenterX, CenterY, Pixel, Distance );
			}
			else
			{
				mTempBuff[ ( y1 * FrameBuffer->getWidth() ) + x1 ] = Pixel;
			}
		}
	}

	memcpy( pFrame.Vid32, mTempBuff, FrameBuffer->getFrameByteSize() );

	return( FF_SUCCESS );
}
