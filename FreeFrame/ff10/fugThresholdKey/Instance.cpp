
#include <stdio.h>
#include "Plugin.h"
#include "Instance.h"

ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	FrameBuffer = ffFrameBufferFactory::getInstance( VideoInfo );
}

DWORD ffInstance::processFrameCopy( ProcessFrameCopyStruct* pFrameData )
{
	VideoPixel32bit		 Pixel;
	VideoPixel32bit		 KeyPixel;
	DWORD				 ThreshStart = (DWORD)( this->paramDynamicData[ FF_PARAM_START ].value.fValue * 256.0 );
	DWORD				 ThreshEnd   = (DWORD)( this->paramDynamicData[ FF_PARAM_END ].value.fValue * 256.0f );
	DWORD				 Luma;

	for( register DWORD x = 0 ; x < this->videoInfo.frameWidth ; x++ )
	{
		for( register DWORD y = 0 ; y < this->videoInfo.frameHeight ; y++ )
		{
			FrameBuffer->getPixel( pFrameData->InputFrames[ 0 ], x, y, &Pixel );
			FrameBuffer->getPixel( pFrameData->InputFrames[ 1 ], x, y, &KeyPixel );

			if( this->paramDynamicData[ FF_PARAM_MASK ].value.fValue < 0.5f )
			{
				Luma = ( KeyPixel.red + KeyPixel.green + KeyPixel.blue ) / 3;
			}
			else
			{
				Luma = ( Pixel.red + Pixel.green + Pixel.blue ) / 3;
			}

			if( ( this->paramDynamicData[ FF_PARAM_INVERT ].value.fValue < 0.5f &&
					( Luma >= ThreshStart && Luma <= ThreshEnd ) ) ||
				  ( this->paramDynamicData[ FF_PARAM_INVERT ].value.fValue >= 0.5f &&
					( Luma < ThreshStart || Luma > ThreshEnd ) ) )
			{
				Pixel = KeyPixel;
			}

			FrameBuffer->setPixel( pFrameData->OutputFrame, x, y, &Pixel );
		}
	}

	return( FF_SUCCESS );
}
