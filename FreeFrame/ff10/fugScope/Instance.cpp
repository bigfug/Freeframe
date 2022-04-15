
#include <stdio.h>
#include "Plugin.h"
#include "Instance.h"

ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	FrameBuffer = ffFrameBufferFactory::getInstance( VideoInfo );
}

///////////////////////////////////////////////////////////////////////////////////////
// processFrame
//
// process a frame of video
//
// parameters:
// 32-bit pointer to byte array containing frame of video
//
// return values (DWORD):
// FF_SUCCESS
// FF_FAIL on error
//

DWORD ffInstance::processFrame( VideoFrame pFrame )
{
	ffPlugin			*RealPlugin = static_cast<ffPlugin *>( this->Plugin );
	VideoPixel32bit		 Pixel;
	DWORD				 BufferSize;
	short				 LineCenter;
	float				 Scale;

	BufferSize = ( this->getParamBool( FF_PARAM_VERTICAL ) ? videoInfo.frameHeight : videoInfo.frameWidth ) * SKIP_WORDS;

	if( FAILED( RealPlugin->Audio->bufferLock( BufferSize ) ) )
	{
		return( FF_FAIL );
	}

	const float		LineHeight = getParamFloat( FF_PARAM_SIZE );

	Pixel.red   = getParamInt( FF_PARAM_RED );
	Pixel.green = getParamInt( FF_PARAM_GREEN );
	Pixel.blue  = getParamInt( FF_PARAM_BLUE );
	Pixel.alpha = 255;

	if( !this->getParamBool( FF_PARAM_VERTICAL ) )
	{
		short				 y;

		LineCenter = (short)( videoInfo.frameHeight * getParamFloat( FF_PARAM_POSITION ) );
		Scale      = getParamFloat( FF_PARAM_SCALE ) * videoInfo.frameHeight;

		if( getParamFloat( FF_PARAM_FILLMODE ) >= FF_FILL_SCOPE )
		{
			for( register DWORD x = 0 ; x < videoInfo.frameWidth ; x++ )
			{
				y = (short)( RealPlugin->Audio->getChannelMix( x * SKIP_WORDS ) * Scale );

				FrameBuffer->DrawLine( pFrame, x, LineCenter, x, LineCenter + y, &Pixel );
			}
		}
		else if( getParamFloat( FF_PARAM_FILLMODE ) >= FF_FILL_BOTTOM )
		{
			for( register DWORD x = 0 ; x < videoInfo.frameWidth ; x++ )
			{
				y = (short)( RealPlugin->Audio->getChannelMix( x * SKIP_WORDS ) * Scale );

				FrameBuffer->DrawLine( pFrame, x, 0, x, LineCenter + y, &Pixel );
			}
		}
		else if( getParamFloat( FF_PARAM_FILLMODE ) >= FF_FILL_TOP )
		{
			for( register DWORD x = 0 ; x < videoInfo.frameWidth ; x++ )
			{
				y = (short)( RealPlugin->Audio->getChannelMix( x * SKIP_WORDS ) * Scale );

				FrameBuffer->DrawLine( pFrame, x, this->videoInfo.frameHeight - 1, x, LineCenter + y, &Pixel );
			}
		}
		else if( LineHeight > 0.0 )
		{
			const int	H = LineHeight * ( videoInfo.frameHeight / 2 );

			for( register DWORD x = 0 ; x < videoInfo.frameWidth ; x++ )
			{
				y = (short)( RealPlugin->Audio->getChannelMix( x * SKIP_WORDS ) * Scale );

				FrameBuffer->DrawLine( pFrame, x, LineCenter + y - H, x, LineCenter + y + H, &Pixel );
			}
		}
		else if( getParamBool( FF_PARAM_LINE ) )
		{
			for( register DWORD x = 0 ; x < videoInfo.frameWidth ; x++ )
			{
				y = (short)( RealPlugin->Audio->getChannelMix( x * SKIP_WORDS ) * Scale );

				if( x < videoInfo.frameWidth - 1 )
				{
					short	y2 = (short)( RealPlugin->Audio->getChannelMix( ( x + 1 ) * SKIP_WORDS ) * Scale );

					FrameBuffer->DrawLine( pFrame, x, LineCenter + y, x + 1, LineCenter + y2, &Pixel );
				}
				else
				{
					//FrameBuffer->setPixel( pFrame, x, LineCenter + y, &Pixel );
				}
			}
		}
		else
		{
			for( register DWORD x = 0 ; x < videoInfo.frameWidth ; x++ )
			{
				y = (short)( RealPlugin->Audio->getChannelMix( x * SKIP_WORDS ) * Scale );

				FrameBuffer->setPixel( pFrame, x, LineCenter + y, &Pixel );
			}
		}
	}
	else
	{
		short		x;

		LineCenter = (short)( videoInfo.frameWidth * this->paramDynamicData[ FF_PARAM_POSITION ].value.fValue );
		Scale      = this->paramDynamicData[ FF_PARAM_SCALE ].value.fValue * videoInfo.frameWidth;

		if( getParamFloat( FF_PARAM_FILLMODE ) >= FF_FILL_SCOPE )
		{
			for( register DWORD y = 0 ; y < videoInfo.frameHeight ; y++ )
			{
				x = (short)( RealPlugin->Audio->getChannelMix( y * SKIP_WORDS ) * Scale );

				FrameBuffer->DrawLine( pFrame, LineCenter, y, LineCenter + x, y, &Pixel );
			}
		}
		else if( getParamFloat( FF_PARAM_FILLMODE ) >= FF_FILL_BOTTOM )
		{
			for( register DWORD y = 0 ; y < videoInfo.frameHeight ; y++ )
			{
				x = (short)( RealPlugin->Audio->getChannelMix( y * SKIP_WORDS ) * Scale );

				FrameBuffer->DrawLine( pFrame, 0, y, LineCenter + x, y, &Pixel );
			}
		}
		else if( getParamFloat( FF_PARAM_FILLMODE ) >= FF_FILL_TOP )
		{
			for( register DWORD y = 0 ; y < videoInfo.frameHeight ; y++ )
			{
				x = (short)( RealPlugin->Audio->getChannelMix( y * SKIP_WORDS ) * Scale );

				FrameBuffer->DrawLine( pFrame, this->videoInfo.frameWidth - 1, y, LineCenter + x, y, &Pixel );
			}
		}
		else if( LineHeight > 0.0 )
		{
			const int	W = LineHeight * ( videoInfo.frameWidth / 2 );

			for( register DWORD y = 0 ; y < videoInfo.frameHeight ; y++ )
			{
				x = (short)( RealPlugin->Audio->getChannelMix( y * SKIP_WORDS ) * Scale );

				FrameBuffer->DrawLine( pFrame, LineCenter + x - W, y, LineCenter + x + W, y, &Pixel );
			}
		}
		else if( getParamBool( FF_PARAM_LINE ) )
		{
			for( register DWORD y = 0 ; y < videoInfo.frameHeight ; y++ )
			{
				x = (short)( RealPlugin->Audio->getChannelMix( y * SKIP_WORDS ) * Scale );

				if( y < videoInfo.frameHeight - 1 )
				{
					short	x2 = (short)( RealPlugin->Audio->getChannelMix( ( y + 1 ) * SKIP_WORDS ) * Scale );

					FrameBuffer->DrawLine( pFrame, LineCenter + x, y, LineCenter + x2, y + 1, &Pixel );
				}
				else
				{
					//FrameBuffer->setPixel( pFrame, x, LineCenter + y, &Pixel );
				}
			}
		}
		else
		{
			for( register DWORD y = 0 ; y < videoInfo.frameHeight ; y++ )
			{
				x = (short)( RealPlugin->Audio->getChannelMix( y * SKIP_WORDS ) * Scale );

				FrameBuffer->setPixel( pFrame, LineCenter + x, y, &Pixel );
			}
		}
	}

	RealPlugin->Audio->bufferUnlock();

	return FF_SUCCESS;
}
