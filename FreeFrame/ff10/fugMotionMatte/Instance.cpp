
#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "Plugin.h"
#include "Instance.h"


ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	this->mFrameBuffer = ffFrameBufferFactory::getInstance( VideoInfo );
	this->mLumaBuf     = new VideoPixel8bit[ VideoInfo->frameWidth * VideoInfo->frameHeight ];
	this->mLumaCur     = new VideoPixel8bit[ VideoInfo->frameWidth * VideoInfo->frameHeight ];
	this->mColrBuf     = new VideoPixel32bit[ VideoInfo->frameWidth * VideoInfo->frameHeight ];

	memset( this->mLumaBuf, 0x00, VideoInfo->frameHeight * VideoInfo->frameWidth * sizeof(VideoPixel8bit) );
	memset( this->mColrBuf, 0x00, VideoInfo->frameHeight * VideoInfo->frameWidth * sizeof(VideoPixel32bit) );
}

DWORD ffInstance::processFrame( VideoFrame pFrame )
{
	const	UINT		 FRAME_WIDTH  = mFrameBuffer->getWidth();
	const	UINT		 FRAME_HEIGHT = mFrameBuffer->getHeight();
	const	int			 Threshold   = this->getParamInt( FF_PARAM_THRESHOLD );
	const	int			 FrameCount  = this->getParamInt( FF_PARAM_FRAMES );
	const	bool		 DrawBelow   = this->getParamBool( FF_PARAM_DRAW_BELOW );
	const	bool		 COLOUR_MODE = this->getParamBool( FF_PARAM_COLOUR );
	const	int			 KEY_FRAME   = this->getParamInt( FF_PARAM_KEYFRAME );
	VideoPixel32bit		*DstPtr;
	bool				 KeyFrame   = false;
	UINT				 DifCnt = 0;

	if( !COLOUR_MODE )
	{
		VideoPixel8bit		*SrcPtr, *BufPtr;
		VideoPixel8bit		 SrcPix, BufPix, DifPix;

		this->mFrameBuffer->toLuma( pFrame.Vid32, mLumaCur );

		if( KEY_FRAME < 255 )
		{
			SrcPtr = mLumaCur;
			BufPtr = mLumaBuf;

			for( UINT y = 0 ; y < FRAME_HEIGHT ; y++ )
			{
				for( UINT x = 0 ; x < FRAME_WIDTH ; x++ )
				{
					SrcPix = *SrcPtr++;
					BufPix = *BufPtr++;
					DifPix = max( SrcPix, BufPix ) - min( SrcPix, BufPix );

					if( DifPix >= KEY_FRAME )
					{
						DifCnt++;
					}
				}
			}

			if( DifCnt > ( FRAME_WIDTH * FRAME_HEIGHT ) / 4 )
			{
				memcpy( this->mLumaBuf, this->mLumaCur, mFrameBuffer->getPixelCount() * sizeof(VideoPixel8bit) );

				KeyFrame = true;
			}
		}

		SrcPtr = mLumaCur;
		BufPtr = mLumaBuf;
		DstPtr = pFrame.Vid32;

		for( UINT y = 0 ; y < FRAME_HEIGHT ; y++ )
		{
			for( UINT x = 0 ; x < FRAME_WIDTH ; x++ )
			{
				SrcPix = *SrcPtr++;
				BufPix = *BufPtr;
				DifPix = max( SrcPix, BufPix ) - min( SrcPix, BufPix );

				if( DifPix >= Threshold )
				{
					memset( DstPtr, 0xff, sizeof( VideoPixel32bit ) );
				}
				else if( DrawBelow )
				{
					memset( DstPtr, ( DifPix * 0xff ) / Threshold, sizeof( VideoPixel32bit ) );
				}
				else
				{
					memset( DstPtr, 0x00, sizeof( VideoPixel32bit ) );
				}

				*BufPtr++ = (BYTE)( ( ( BufPix * FrameCount ) + SrcPix ) / ( FrameCount + 1 ) );

				DstPtr++;
			}
		}
	}
	else
	{
		VideoPixel32bit		*SrcPtr, *BufPtr;
		VideoPixel32bit		 SrcPix, BufPix, DifPix;

		if( KEY_FRAME < 255 )
		{
			SrcPtr = pFrame.Vid32;
			BufPtr = mColrBuf;

			for( UINT y = 0 ; y < FRAME_HEIGHT ; y++ )
			{
				for( UINT x = 0 ; x < FRAME_WIDTH ; x++ )
				{
					SrcPix = *SrcPtr++;
					BufPix = *BufPtr++;
					DifPix.red   = max( SrcPix.red,   BufPix.red   ) - min( SrcPix.red,   BufPix.red   );
					DifPix.green = max( SrcPix.green, BufPix.green ) - min( SrcPix.green, BufPix.green );
					DifPix.blue  = max( SrcPix.blue,  BufPix.blue  ) - min( SrcPix.blue,  BufPix.blue  );

					if( DifPix.red >= KEY_FRAME || DifPix.green >= KEY_FRAME || DifPix.blue >= KEY_FRAME )
					{
						DifCnt++;
					}
				}
			}

			if( DifCnt > ( FRAME_WIDTH * FRAME_HEIGHT ) / 4 )
			{
				memcpy( this->mColrBuf, pFrame.Vid32, mFrameBuffer->getPixelCount() * sizeof(VideoPixel32bit) );

				KeyFrame = true;
			}
		}

		SrcPtr = pFrame.Vid32;
		BufPtr = mColrBuf;
		DstPtr = pFrame.Vid32;

		for( UINT y = 0 ; y < FRAME_HEIGHT ; y++ )
		{
			for( UINT x = 0 ; x < FRAME_WIDTH ; x++ )
			{
				SrcPix = *SrcPtr++;
				BufPix = *BufPtr;
				DifPix.red   = max( SrcPix.red,   BufPix.red   ) - min( SrcPix.red,   BufPix.red   );
				DifPix.green = max( SrcPix.green, BufPix.green ) - min( SrcPix.green, BufPix.green );
				DifPix.blue  = max( SrcPix.blue,  BufPix.blue  ) - min( SrcPix.blue,  BufPix.blue  );

				if( DifPix.red >= Threshold )
				{
					DstPtr->red = 0xff;
				}
				else if( DrawBelow )
				{
					DstPtr->red = ( DifPix.red * 0xff ) / Threshold;
				}
				else
				{
					DstPtr->red = 0x00;
				}

				BufPtr->red = (BYTE)( ( ( BufPix.red * FrameCount ) + SrcPix.red ) / ( FrameCount + 1 ) );

				if( DifPix.green >= Threshold )
				{
					DstPtr->green = 0xff;
				}
				else if( DrawBelow )
				{
					DstPtr->green = ( DifPix.green * 0xff ) / Threshold;
				}
				else
				{
					DstPtr->green = 0x00;
				}

				BufPtr->green = (BYTE)( ( ( BufPix.green * FrameCount ) + SrcPix.green ) / ( FrameCount + 1 ) );

				if( DifPix.blue >= Threshold )
				{
					DstPtr->blue = 0xff;
				}
				else if( DrawBelow )
				{
					DstPtr->blue = ( DifPix.blue * 0xff ) / Threshold;
				}
				else
				{
					DstPtr->blue = 0x00;
				}

				BufPtr->blue = (BYTE)( ( ( BufPix.blue * FrameCount ) + SrcPix.blue ) / ( FrameCount + 1 ) );

				BufPtr++;
				DstPtr++;
			}
		}
	}

#if 0
	for( DWORD y = 0 ; y < this->videoInfo.frameHeight ; y++ )
	{
		for( DWORD x = 0 ; x < this->videoInfo.frameWidth ; x++ )
		{
			SrcPix = *SrcPtr;

			if( Mode == 0 )
			{
				if( abs( SrcPix.red   - DatPtr->red   ) >= Threshold ||
					abs( SrcPix.green - DatPtr->green ) >= Threshold ||
					abs( SrcPix.blue  - DatPtr->blue  ) >= Threshold )
				{
					memset( SrcPtr, 0xff, sizeof(VideoPixel32bit) );
				}
				else if( DrawBelow )
				{
					memset( SrcPtr, ( max( max( abs( SrcPix.red - DatPtr->red ), abs( SrcPix.green - DatPtr->green ) ), abs( SrcPix.blue - DatPtr->blue ) ) * 0xff ) / Threshold, sizeof(VideoPixel32bit) );
				}
				else
				{
					memset( SrcPtr, 0x00, sizeof(VideoPixel32bit) );
				}

				DatPtr->red   = (BYTE)( ( ( DatPtr->red   * FrameCount ) + SrcPix.red   ) / ( FrameCount + 1 ) );
				DatPtr->green = (BYTE)( ( ( DatPtr->green * FrameCount ) + SrcPix.green ) / ( FrameCount + 1 ) );
				DatPtr->blue  = (BYTE)( ( ( DatPtr->blue  * FrameCount ) + SrcPix.blue  ) / ( FrameCount + 1 ) );
			}
			else if( Mode == 1 )
			{
				SrcGray = (BYTE)( ( SrcPtr->red * 0.3 ) + ( SrcPtr->green * 0.59 ) + ( SrcPtr->blue * 0.11 ) );

				if( abs( SrcGray - DatPtr->red ) >= Threshold )
				{
					memset( SrcPtr, 0xff, sizeof(VideoPixel32bit) );
				}
				else if( DrawBelow )
				{
					SrcPtr->red = abs( SrcGray - DatPtr->red );
				}
				else
				{
					memset( SrcPtr, 0x00, sizeof(VideoPixel32bit) );
				}

				DatPtr->red   = (BYTE)( ( ( DatPtr->red   * FrameCount ) + SrcPix.red   ) / ( FrameCount + 1 ) );
				DatPtr->green = (BYTE)( ( ( DatPtr->green * FrameCount ) + SrcPix.red   ) / ( FrameCount + 1 ) );
				DatPtr->blue  = (BYTE)( ( ( DatPtr->blue  * FrameCount ) + SrcPix.red   ) / ( FrameCount + 1 ) );
			}
			else if( Mode == 2 )
			{
				int		diff = ( abs( SrcPix.red   - DatPtr->red   ) +
					abs( SrcPix.green - DatPtr->green ) +
					abs( SrcPix.blue  - DatPtr->blue  ) ) / 3;

				if( diff >= Threshold )
				{
					memset( SrcPtr, 0xff, sizeof(VideoPixel32bit) );
				}
				else if( DrawBelow )
				{
					memset( SrcPtr, ( max( max( abs( SrcPix.red - DatPtr->red ), abs( SrcPix.green - DatPtr->green ) ), abs( SrcPix.blue - DatPtr->blue ) ) * 0xff ) / Threshold, sizeof(VideoPixel32bit) );
				}
				else
				{
					memset( SrcPtr, 0x00, sizeof(VideoPixel32bit) );
				}

				DatPtr->red   = (BYTE)( ( ( DatPtr->red   * FrameCount ) + SrcPix.red   ) / ( FrameCount + 1 ) );
				DatPtr->green = (BYTE)( ( ( DatPtr->green * FrameCount ) + SrcPix.green ) / ( FrameCount + 1 ) );
				DatPtr->blue  = (BYTE)( ( ( DatPtr->blue  * FrameCount ) + SrcPix.blue  ) / ( FrameCount + 1 ) );
			}

/*
			switch( GrayCode )
			{
				case 0:
					SrcGray = max( max( SrcPtr->red, SrcPtr->green ), SrcPtr->blue );
					break;

				case 1:
					SrcGray = ( max( max( SrcPtr->red, SrcPtr->green ), SrcPtr->blue ) + min( min( SrcPtr->red, SrcPtr->green ), SrcPtr->blue ) ) / 2;
					break;

				case 2:
					SrcGray = (BYTE)( ( SrcPtr->red * 0.3 ) + ( SrcPtr->green * 0.59 ) + ( SrcPtr->blue * 0.11 ) );
					break;
			}

			if( abs( SrcGray - DatGray ) >= Threshold )
			{
				//memset( SrcPtr, 0, sizeof(VideoPixel32bit) );
				*DatPtr = 0xff;
			}
			else
			{
				*DatPtr = ( ( *DatPtr * FrameCount ) + SrcGray ) / ( FrameCount + 1 );
			}

			SrcPtr->red = SrcPtr->green = SrcPtr->blue = abs( SrcGray - *DatPtr );

			//SrcPtr->red = SrcPtr->green = SrcPtr->blue = *DatPtr;
*/
			SrcPtr++;
			DatPtr++;
		}
	}

	//memcpy( pFrame.Vid32, this->Buffer, this->videoInfo.frameHeight * this->videoInfo.frameWidth * 4 );

#endif

	return( FF_SUCCESS );
}
