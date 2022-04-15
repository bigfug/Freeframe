
#include <stdio.h>
#include "Plugin.h"
#include "Instance.h"

ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	FrameBuffer = ffFrameBufferFactory::getInstance( VideoInfo );
}

DWORD ffInstance::processFrameCopy( ProcessFrameCopyStruct* pFrameData )
{
	VideoPixel32bit		 SrcPixel;
	VideoPixel32bit		 KeyPixel;
	VideoPixel32bit		 DstPixel;
	VideoPixel32bit		*LmaPixel;
	BYTE				 ThreshStart = (BYTE)( this->paramDynamicData[ FF_PARAM_START ].value.fValue * 255.0f );
	BYTE				 ThreshEnd   = (BYTE)( this->paramDynamicData[ FF_PARAM_END ].value.fValue * 255.0f );

	for( register DWORD x = 0 ; x < this->videoInfo.frameWidth ; x++ )
	{
		for( register DWORD y = 0 ; y < this->videoInfo.frameHeight ; y++ )
		{
			FrameBuffer->getPixel( pFrameData->InputFrames[ 0 ], x, y, &SrcPixel );
			FrameBuffer->getPixel( pFrameData->InputFrames[ 1 ], x, y, &KeyPixel );

			if( this->paramDynamicData[ FF_PARAM_MASK ].value.fValue < 0.5f )
			{
				LmaPixel = &KeyPixel;
			}
			else
			{
				LmaPixel = &SrcPixel;
			}

			if( this->paramDynamicData[ FF_PARAM_RED ].value.fValue >= 0.5f &&
				( ( this->paramDynamicData[ FF_PARAM_INVERT ].value.fValue < 0.5f &&
					( LmaPixel->red >= ThreshStart && LmaPixel->red <= ThreshEnd ) ) ||
				  ( this->paramDynamicData[ FF_PARAM_INVERT ].value.fValue >= 0.5f &&
					( LmaPixel->red < ThreshStart || LmaPixel->red > ThreshEnd ) ) ) )
			{
				DstPixel.red = KeyPixel.red;
			}
			else
			{
				DstPixel.red = SrcPixel.red;
			}

			if( this->paramDynamicData[ FF_PARAM_GREEN ].value.fValue >= 0.5f &&
				( ( this->paramDynamicData[ FF_PARAM_INVERT ].value.fValue < 0.5f &&
					( LmaPixel->green >= ThreshStart && LmaPixel->green <= ThreshEnd ) ) ||
				  ( this->paramDynamicData[ FF_PARAM_INVERT ].value.fValue >= 0.5f &&
					( LmaPixel->green < ThreshStart || LmaPixel->green > ThreshEnd ) ) ) )
			{
				DstPixel.green = KeyPixel.green;
			}
			else
			{
				DstPixel.green = SrcPixel.green;
			}

			if( this->paramDynamicData[ FF_PARAM_BLUE ].value.fValue >= 0.5f &&
				( ( this->paramDynamicData[ FF_PARAM_INVERT ].value.fValue < 0.5f &&
					( LmaPixel->blue >= ThreshStart && LmaPixel->blue <= ThreshEnd ) ) ||
				  ( this->paramDynamicData[ FF_PARAM_INVERT ].value.fValue >= 0.5f &&
					( LmaPixel->blue < ThreshStart || LmaPixel->blue > ThreshEnd ) ) ) )
			{
				DstPixel.blue = KeyPixel.blue;
			}
			else
			{
				DstPixel.blue = SrcPixel.blue;
			}

			FrameBuffer->setPixel( pFrameData->OutputFrame, x, y, &DstPixel );
		}
	}

	return( FF_SUCCESS );
}
