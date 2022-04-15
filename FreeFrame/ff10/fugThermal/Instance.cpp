
#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "Plugin.h"
#include "Instance.h"

ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	this->FrameBuffer = ffFrameBufferFactory::getInstance( VideoInfo );
	this->Buffer1     = new VideoPixel32bit[ VideoInfo->frameWidth * VideoInfo->frameHeight ];
	this->Buffer2     = new VideoPixel8bit[ VideoInfo->frameWidth * VideoInfo->frameHeight ];

	this->mHSL1       = new VideoPixelHSL[ VideoInfo->frameWidth * VideoInfo->frameHeight ];
	this->mHSL2       = new VideoPixelHSL[ VideoInfo->frameWidth * VideoInfo->frameHeight ];
}

DWORD ffInstance::processFrame( VideoFrame pFrame )
{
	const	bool	 COLOUR_MODE  = this->getParamBool( FF_PARAM_COLOUR );
	const	float	 HUE_ROTATE   = this->getParamFloat( FF_PARAM_HUE_ROTATE );
	const	INT		 BLUR_RADIUS  = this->getParamInt( FF_PARAM_BLUR_RADIUS );
	const	INT		 LUMA_OVERLAY = 255 - this->getParamInt( FF_PARAM_LUMA_OVERLAY );
	const	INT		 FRAME_WIDTH = this->FrameBuffer->getWidth();
	const	INT		 FRAME_HEIGHT = this->FrameBuffer->getHeight();

	this->FrameBuffer->RGB2HSL( pFrame.Vid32, this->mHSL1 );

	if( LUMA_OVERLAY < 255 )
	{
		this->FrameBuffer->toBrightness( pFrame.Vid32, this->Buffer2 );
	}

	if( COLOUR_MODE )
	{
		VideoPixelHSL *DstPtr = this->mHSL2;

		for( INT y = 0 ; y < FRAME_HEIGHT ; y++ )
		{
			for( INT x = 0 ; x < FRAME_WIDTH ; x++ )
			{
				VideoPixelHSL	SrcPix = this->mHSL1[ ( y * FRAME_WIDTH ) + x ];

				SrcPix.mHue += HUE_ROTATE;

				if( SrcPix.mHue > 360.0f )
				{
					SrcPix.mHue -= 360.0f;
				}

				if( SrcPix.mSat > 0.0f )
				{
					SrcPix.mLum = 0.5f;
					SrcPix.mSat = 1.0f;
				}
				else
				{
					SrcPix.mLum = 0.0f;
				}

				*DstPtr++ = SrcPix;
			}
		}

		this->FrameBuffer->HSL2RGB( this->mHSL2, pFrame.Vid32 );
	}
	else
	{
		VideoPixelHSL		*SrcPtr = this->mHSL1;
		VideoPixel32bit		*DstPtr = pFrame.Vid32;

		for( INT y = 0 ; y < FRAME_HEIGHT ; y++ )
		{
			for( INT x = 0 ; x < FRAME_WIDTH ; x++ )
			{
				VideoPixelHSL	SrcPix = *SrcPtr++;

				SrcPix.mHue += HUE_ROTATE;

				if( SrcPix.mHue > 360.0f )
				{
					SrcPix.mHue -= 360.0f;
				}

				if( SrcPix.mSat == 0.0f )
				{
					memset( DstPtr, (int)( SrcPix.mLum * 255.0f ), 4 );
				}
				else
				{
					memset( DstPtr, (int)( ( fabs( SrcPix.mHue - 180.0f ) / 180.0f ) * 255.0f ), 4 );
				}

				DstPtr++;
			}
		}
	}

	if( LUMA_OVERLAY < 255 )
	{
		VideoPixel8bit		*SrcPix = this->Buffer2;
		VideoPixel32bit		*DstPix = pFrame.Vid32;

		for( INT y = 0 ; y < FRAME_HEIGHT ; y++ )
		{
			for( INT x = 0 ; x < FRAME_WIDTH ; x++ )
			{
				const	UINT		Luma = max( *SrcPix, LUMA_OVERLAY );

				DstPix->red   = ( (UINT)DstPix->red   * Luma ) / 255;
				DstPix->green = ( (UINT)DstPix->green * Luma ) / 255;
				DstPix->blue  = ( (UINT)DstPix->blue  * Luma ) / 255;

				SrcPix++;
				DstPix++;
			}
		}
	}

	if( BLUR_RADIUS > 0 )
	{
		memcpy( this->Buffer1, pFrame.Vid32, FrameBuffer->getFrameByteSize() );

		VideoPixel32bit		*DstPix = pFrame.Vid32;

		for( INT y = 0 ; y < FRAME_HEIGHT ; y++ )
		{
			for( INT x = 0 ; x < FRAME_WIDTH ; x++ )
			{
				UINT		SumR = 0;
				UINT		SumG = 0;
				UINT		SumB = 0;
				UINT		SumA = 0;
				UINT		Cnt  = 0;

				for( INT j = 0 - BLUR_RADIUS ; j <= BLUR_RADIUS ; j++ )
				{
					if( y + j >= 0 && y + j < FRAME_HEIGHT )
					{
						VideoPixel32bit *SrcPtr = &this->Buffer1[ ( y + j ) * FRAME_WIDTH ];

						for( INT i = 0 - BLUR_RADIUS ; i <= BLUR_RADIUS ; i++ )
						{
							if( x + i >= 0 && x + i < FRAME_WIDTH )
							{
								SumR += SrcPtr[ x + i ].red;
								SumG += SrcPtr[ x + i ].green;
								SumB += SrcPtr[ x + i ].blue;
								SumA += SrcPtr[ x + i ].alpha;

								Cnt++;
							}
						}
					}
				}

				DstPix->red   = SumR / Cnt;
				DstPix->green = SumG / Cnt;
				DstPix->blue  = SumB / Cnt;
				DstPix->alpha = SumA / Cnt;

				DstPix++;
			}
		}
	}

	return( FF_SUCCESS );
}
