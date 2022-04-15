
#include <stdio.h>
#include <stdlib.h>
#include "Plugin.h"
#include "Instance.h"

ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	FrameBuffer = ffFrameBufferFactory::getInstance( VideoInfo );

	switch( this->videoInfo.bitDepth )
	{
		case FF_CAP_16BITVIDEO:
			this->mPixelSize = sizeof(VideoPixel16bit);
			break;

		case FF_CAP_24BITVIDEO:
			this->mPixelSize = sizeof(VideoPixel24bit);
			break;

		case FF_CAP_32BITVIDEO:
			this->mPixelSize = sizeof(VideoPixel32bit);
			break;
	}

	this->mFrameSize = this->mPixelSize * this->videoInfo.frameWidth * this->videoInfo.frameHeight;

	this->mLumaMap = (unsigned char *)malloc( VideoInfo->frameWidth * VideoInfo->frameHeight );
}

ffInstance::~ffInstance()
{
	free( this->mLumaMap );
}

inline BYTE mix( BYTE Dst, BYTE Src, BYTE Amt )
{
	int		Diff = Dst - Src;

	return( Src + ( ( Diff * Amt ) / 255 ) );
}

DWORD ffInstance::processFrameCopy( ProcessFrameCopyStruct* pFrameData )
{
	VideoPixel32bit		 SrcPixel;
	VideoPixel32bit		 MskPixel;
	VideoPixel32bit		 DstPixel;
	unsigned int		 Luma;
	unsigned short		 yStart;
	//unsigned short		 yMiddle;
	unsigned short		 yEnd;
	unsigned int		 yTotal;
	unsigned int		 yCentroid;
	const int			 LumaStart = this->getParamInt( FF_LUMA_START );
	const int			 LumaEnd   = this->getParamInt( FF_LUMA_END );
	const bool			 UseMaskAlpha = this->getParamBool( FF_USE_MASK_ALPHA );
	const bool			 UseWholeFrame = this->getParamBool( FF_USE_WHOLE_FRAME );
	const bool			 DrawZeroLuma  = this->getParamBool( FF_DRAW_ZERO_LUMA );

	memset( &MskPixel, 0x00, sizeof(MskPixel) );
	memset( &DstPixel, 0xff, sizeof(DstPixel) );

	memset( pFrameData->OutputFrame.Frame, 0x00, this->mFrameSize );
	memset( this->mLumaMap, 0x00, this->videoInfo.frameWidth * this->videoInfo.frameHeight );

	for( unsigned short x = 0 ; x < this->videoInfo.frameWidth ; x++ )
	{
		yStart = (unsigned short)( this->videoInfo.frameHeight - 1 );
		yEnd   = 0;
		yTotal = 0;
		yCentroid = 0;

		for( unsigned short y = 0 ; y < this->videoInfo.frameHeight ; y++ )
		{
			FrameBuffer->getPixel( pFrameData->InputFrames[ 1 ], x, y, &SrcPixel );

			Luma = ( SrcPixel.red * 30 + SrcPixel.green * 59 + SrcPixel.blue * 11 ) / 100;

			if( Luma >= LumaStart && Luma <= LumaEnd )
			{
				if( y < yStart ) yStart = y;
				if( y > yEnd   ) yEnd   = y;

				yCentroid += Luma * ( y + 1 );
				yTotal    += Luma;

				this->mLumaMap[ ( y * this->videoInfo.frameWidth ) + x ] = Luma;
			}
		}
/*
		for( unsigned short y = 0 ; y < this->videoInfo.frameHeight ; y++ )
		{
			if( y >= yStart && y <= yEnd )
			{
				FrameBuffer->setPixel( pFrameData->OutputFrame, x, y, &DstPixel );
			}
			else
			{
				FrameBuffer->setPixel( pFrameData->OutputFrame, x, y, &MskPixel );
			}
		}
*/
		if( yTotal > 0 )
		{
			yCentroid /= yTotal;
		}
		else if( DrawZeroLuma )
		{
			yCentroid = ( this->videoInfo.frameHeight - 1 ) / 2;
			yStart    = 0;
			yEnd      = (unsigned short)( this->videoInfo.frameHeight - 1 );
		}
		else
		{
			continue;
		}

		float yInc, yPos;

		yInc = ( (float)( this->videoInfo.frameHeight - 1 ) / 2.0f ) / (float)( yCentroid - ( UseWholeFrame ? 0 : yStart ) );
		yPos = 0.0f;

		//for( unsigned short y = ( UseWholeFrame ? 0 : yStart ) ; y <= yCentroid ; y++ )
		for( unsigned short y = yStart ; y <= yCentroid ; y++ )
		{
			FrameBuffer->getPixel( pFrameData->InputFrames[ 0 ], x, (DWORD)yPos, &SrcPixel );

			if( UseMaskAlpha )
			{
				SrcPixel.alpha = this->mLumaMap[ ( y * this->videoInfo.frameWidth ) + x ];
				SrcPixel.red   = ( SrcPixel.red   * SrcPixel.alpha ) / 255;
				SrcPixel.green = ( SrcPixel.green * SrcPixel.alpha ) / 255;
				SrcPixel.blue  = ( SrcPixel.blue  * SrcPixel.alpha ) / 255;
			}
			else
			{
				SrcPixel.alpha = 255;
			}

			FrameBuffer->setPixel( pFrameData->OutputFrame, x, y, &SrcPixel );

			yPos += yInc;
		}

		yInc = ( (float)( this->videoInfo.frameHeight - 1 ) / 2.0f ) / (float)( ( UseWholeFrame ? this->videoInfo.frameHeight - 1 : yEnd ) - yCentroid );
		yPos = (float)this->videoInfo.frameHeight / 2.0f;

		//for( unsigned short y = yCentroid ; y <= ( UseWholeFrame ? this->videoInfo.frameHeight - 1 : yEnd ) ; y++ )
		for( unsigned short y = yCentroid ; y <= yEnd ; y++ )
		{
			FrameBuffer->getPixel( pFrameData->InputFrames[ 0 ], x, (DWORD)yPos, &SrcPixel );

			if( UseMaskAlpha )
			{
				SrcPixel.alpha = this->mLumaMap[ ( y * this->videoInfo.frameWidth ) + x ];
				SrcPixel.red   = ( SrcPixel.red   * SrcPixel.alpha ) / 255;
				SrcPixel.green = ( SrcPixel.green * SrcPixel.alpha ) / 255;
				SrcPixel.blue  = ( SrcPixel.blue  * SrcPixel.alpha ) / 255;
			}
			else
			{
				SrcPixel.alpha = 255;
			}

			FrameBuffer->setPixel( pFrameData->OutputFrame, x, y, &SrcPixel );

			yPos += yInc;
		}

		//FrameBuffer->setPixel( pFrameData->OutputFrame, x, yCentroid, &DstPixel );
	}

	return( FF_SUCCESS );
}
