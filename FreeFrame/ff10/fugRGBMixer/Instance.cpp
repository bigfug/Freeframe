
#include <stdio.h>
#include "Plugin.h"
#include "Instance.h"

enum
{
	FF_BUFF_SRC = 0,
	FF_BUFF_RED,
	FF_BUFF_GREEN,
	FF_BUFF_BLUE,
	FF_BUFF_MASK
};

ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	FrameBuffer = ffFrameBufferFactory::getInstance( VideoInfo );
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

	for( register DWORD x = 0 ; x < this->videoInfo.frameWidth ; x++ )
	{
		for( register DWORD y = 0 ; y < this->videoInfo.frameHeight ; y++ )
		{
			FrameBuffer->getPixel( pFrameData->InputFrames[ FF_BUFF_MASK ], x, y, &MskPixel );

			if( MskPixel.blue == 255 )
			{
				FrameBuffer->getPixel( pFrameData->InputFrames[ FF_BUFF_BLUE ], x, y, &DstPixel );
			}
			else
			{
				if( MskPixel.green == 255 )
				{
					FrameBuffer->getPixel( pFrameData->InputFrames[ FF_BUFF_GREEN ], x, y, &DstPixel );
				}
				else
				{
					if( MskPixel.red == 255 )
					{
						FrameBuffer->getPixel( pFrameData->InputFrames[ FF_BUFF_RED ], x, y, &DstPixel );
					}
					else
					{
						FrameBuffer->getPixel( pFrameData->InputFrames[ FF_BUFF_SRC ], x, y, &DstPixel );

						if( MskPixel.red > 0 )
						{
							FrameBuffer->getPixel( pFrameData->InputFrames[ FF_BUFF_RED ], x, y, &SrcPixel );

							DstPixel.red   = mix( SrcPixel.red,   DstPixel.red,   MskPixel.red );
							DstPixel.green = mix( SrcPixel.green, DstPixel.green, MskPixel.red );
							DstPixel.blue  = mix( SrcPixel.blue,  DstPixel.blue,  MskPixel.red );
						}
					}

					if( MskPixel.green > 0 )
					{
						FrameBuffer->getPixel( pFrameData->InputFrames[ FF_BUFF_GREEN ], x, y, &SrcPixel );

						DstPixel.red   = mix( SrcPixel.red,   DstPixel.red,   MskPixel.green );
						DstPixel.green = mix( SrcPixel.green, DstPixel.green, MskPixel.green );
						DstPixel.blue  = mix( SrcPixel.blue,  DstPixel.blue,  MskPixel.green );
					}
				}

				if( MskPixel.blue > 0 )
				{
					FrameBuffer->getPixel( pFrameData->InputFrames[ FF_BUFF_BLUE ], x, y, &SrcPixel );

					DstPixel.red   = mix( SrcPixel.red,   DstPixel.red,   MskPixel.blue );
					DstPixel.green = mix( SrcPixel.green, DstPixel.green, MskPixel.blue );
					DstPixel.blue  = mix( SrcPixel.blue,  DstPixel.blue,  MskPixel.blue );
				}
			}

			FrameBuffer->setPixel( pFrameData->OutputFrame, x, y, &DstPixel );
		}
	}

	return( FF_SUCCESS );
}
