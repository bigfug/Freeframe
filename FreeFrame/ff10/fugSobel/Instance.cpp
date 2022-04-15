
#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "Plugin.h"
#include "Instance.h"

ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	this->FrameBuffer = ffFrameBufferFactory::getInstance( VideoInfo );
	this->Buffer1     = new VideoPixel32bit[ VideoInfo->frameWidth * VideoInfo->frameHeight ];
//	this->Buffer2     = new VideoPixel32bit[ VideoInfo->frameWidth * VideoInfo->frameHeight ];
}

DWORD ffInstance::processFrame( VideoFrame pFrame )
{
	const	bool		 pColour  = this->getParamBool( FF_PARAM_COLOUR );
	const	bool		 pDoX     = this->getParamBool( FF_PARAM_X );
	const	bool		 pDoY     = this->getParamBool( FF_PARAM_Y );
	const	bool		 pInvert  = this->getParamBool( FF_PARAM_INVERT );

	VideoPixel32bit		*Row0, *Row1, *Row2;
	VideoPixel32bit		*DstPtr;
	float				 redR, grnR, bluR;

	if( !pColour )
	{
		this->FrameBuffer->toLuma( pFrame.Vid32, this->Buffer1 );
	}
	else
	{
		memcpy( this->Buffer1, pFrame.Vid32, sizeof( VideoPixel32bit ) * this->videoInfo.frameWidth * this->videoInfo.frameHeight );
	}

	for( DWORD y = 1 ; y < this->videoInfo.frameHeight - 1 ; y++ )
	{
		for( DWORD x = 1 ; x < this->videoInfo.frameWidth - 1 ; x++ )
		{
			const	int		Poff = ( y * this->videoInfo.frameWidth ) + x;

			DstPtr = &pFrame.Vid32[ Poff ];

			Row0 = &this->Buffer1[ Poff - this->videoInfo.frameWidth - 1 ];
			Row1 = &this->Buffer1[ Poff - 1 ];
			Row2 = &this->Buffer1[ Poff + this->videoInfo.frameWidth - 1 ];

			if( pColour )
			{
				int redH = 0;
				int redV = 0;
				int grnH = 0;
				int grnV = 0;
				int bluH = 0;
				int bluV = 0;
					
				if( pDoX )
				{
					redH  = ( -1 * (int)Row0[ 0 ].red ) + ( 1 * (int)Row0[ 2 ].red );
					redH += ( -2 * (int)Row1[ 0 ].red ) + ( 2 * (int)Row1[ 2 ].red );
					redH += ( -1 * (int)Row2[ 0 ].red ) + ( 1 * (int)Row2[ 2 ].red );

					grnH  = ( -1 * (int)Row0[ 0 ].green ) + ( 1 * (int)Row0[ 2 ].green );
					grnH += ( -2 * (int)Row1[ 0 ].green ) + ( 2 * (int)Row1[ 2 ].green );
					grnH += ( -1 * (int)Row2[ 0 ].green ) + ( 1 * (int)Row2[ 2 ].green );

					bluH  = ( -1 * (int)Row0[ 0 ].blue ) + ( 1 * (int)Row0[ 2 ].blue );
					bluH += ( -2 * (int)Row1[ 0 ].blue ) + ( 2 * (int)Row1[ 2 ].blue );
					bluH += ( -1 * (int)Row2[ 0 ].blue ) + ( 1 * (int)Row2[ 2 ].blue );
				}

					
				if( pDoY )
				{
					redV  = (  1 * (int)Row0[ 0 ].red ) + (  2 * (int)Row0[ 1 ].red ) + (  1 * (int)Row0[ 2 ].red );
					redV += ( -1 * (int)Row2[ 0 ].red ) + ( -2 * (int)Row2[ 1 ].red ) + ( -1 * (int)Row2[ 2 ].red );

					grnV  = (  1 * (int)Row0[ 0 ].green ) + (  2 * (int)Row0[ 1 ].green ) + (  1 * (int)Row0[ 2 ].green );
					grnV += ( -1 * (int)Row2[ 0 ].green ) + ( -2 * (int)Row2[ 1 ].green ) + ( -1 * (int)Row2[ 2 ].green );

					bluV  = (  1 * (int)Row0[ 0 ].blue ) + (  2 * (int)Row0[ 1 ].blue ) + (  1 * (int)Row0[ 2 ].blue );
					bluV += ( -1 * (int)Row2[ 0 ].blue ) + ( -2 * (int)Row2[ 1 ].blue ) + ( -1 * (int)Row2[ 2 ].blue );
				}

				redR = sqrt( (float)( redH * redH ) + (float)( redV * redV ) );
				grnR = sqrt( (float)( grnH * grnH ) + (float)( grnV * grnV ) );
				bluR = sqrt( (float)( bluH * bluH ) + (float)( bluV * bluV ) );

				redR = min( 255.0f, max( redR, 0.0f ) );
				grnR = min( 255.0f, max( grnR, 0.0f ) );
				bluR = min( 255.0f, max( bluR, 0.0f ) );
			}
			else
			{
				int redH = 0;
				int redV = 0;

				if( pDoX )
				{
					redH  = ( -1 * (int)Row0[ 0 ].red ) + ( 1 * (int)Row0[ 2 ].red );
					redH += ( -2 * (int)Row1[ 0 ].red ) + ( 2 * (int)Row1[ 2 ].red );
					redH += ( -1 * (int)Row2[ 0 ].red ) + ( 1 * (int)Row2[ 2 ].red );
				}

				if( pDoY )
				{
					redV  = (  1 * (int)Row0[ 0 ].red ) + (  2 * (int)Row0[ 1 ].red ) + (  1 * (int)Row0[ 2 ].red );
					redV += ( -1 * (int)Row2[ 0 ].red ) + ( -2 * (int)Row2[ 1 ].red ) + ( -1 * (int)Row2[ 2 ].red );
				}

				redR = sqrt( (float)( redH * redH ) + (float)( redV * redV ) );

				redR /= 1.4142135623730950488016887242097f;

				redR = min( 255.0f, max( redR, 0.0f ) );
				grnR = bluR = redR;
			}

			if( pInvert )
			{
				DstPtr->red   = 255 - (BYTE)redR;
				DstPtr->green = 255 - (BYTE)grnR;
				DstPtr->blue  = 255 - (BYTE)bluR;
			}
			else
			{
				DstPtr->red   = (BYTE)redR;
				DstPtr->green = (BYTE)grnR;
				DstPtr->blue  = (BYTE)bluR;
			}
		}
	}

	return( FF_SUCCESS );
}
