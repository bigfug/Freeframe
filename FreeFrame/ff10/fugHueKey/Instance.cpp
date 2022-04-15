
#include <stdio.h>
#include "Plugin.h"
#include "Instance.h"

static inline float NormaliseHue( const float pHue )
{
	return( pHue > 180.0f ? pHue - 360.0f : pHue );
}

static float AngleDiff( const float pAngle1, const float pAngle2 )
{
	float		Diff = pAngle1 - pAngle2;

	while( Diff < -180.0f ) Diff += 360.0f;
	while( Diff >  180.0f ) Diff -= 360.0f;

	return( Diff );
}

ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	FrameBuffer = ffFrameBufferFactory::getInstance( VideoInfo );

	mHSL = new VideoPixelHSL[ VideoInfo->frameWidth * VideoInfo->frameHeight ];
}

DWORD ffInstance::processFrameCopy( ProcessFrameCopyStruct* pFrameData )
{
	const	float		 pHue        = this->getParamFloat( FF_PARAM_HUE );
	const	float		 pWidth      = this->getParamFloat( FF_PARAM_WIDTH ) / 2.0f;
	const	float		 pSoftWidth  = this->getParamFloat( FF_PARAM_SOFTWIDTH ) / 2.0f;
	const	bool		 pMaskMode   = this->getParamBool( FF_PARAM_MASK );
	float				 HardPos[]    = { 360.0f, 360.0f,   0.0f,   0.0f };
	float				 SoftPos[]    = { 120.0f, 120.0f, 120.0f, 120.0f };
	float				*WheelPos[]  = { HardPos, SoftPos };
	VideoPixel32bit		*SrcPixel;
	VideoPixel32bit		*KeyPixel;
	VideoPixel32bit		*DstPixel;
	VideoPixelHSL		*LmaPixel = this->mHSL;
	int					 Remaining = this->FrameBuffer->getWidth() * this->FrameBuffer->getHeight();

	SrcPixel = pFrameData->InputFrames[ 0 ].Vid32;
	KeyPixel = pFrameData->InputFrames[ 1 ].Vid32;
	DstPixel = pFrameData->OutputFrame.Vid32;

	if( pMaskMode )
	{
		this->FrameBuffer->RGB2HSL( pFrameData->InputFrames[ 0 ].Vid32, this->mHSL );
	}
	else
	{
		this->FrameBuffer->RGB2HSL( pFrameData->InputFrames[ 1 ].Vid32, this->mHSL );
	}

	HardPos[ 0 ] = pHue - pWidth;
	HardPos[ 1 ] = pHue;
	HardPos[ 2 ] = pHue;
	HardPos[ 3 ] = pHue + pWidth;

	SoftPos[ 0 ] = pHue - pSoftWidth;
	SoftPos[ 1 ] = pHue;
	SoftPos[ 2 ] = pHue;
	SoftPos[ 3 ] = pHue + pSoftWidth;

	for( int i = 0 ; i < 2 ; i++ )
	{
		for( int j = 0 ; j < 4 ; j++ )
		{
			while( WheelPos[ i ][ j ] < 0.0f   ) WheelPos[ i ][ j ] += 360.0f;
			while( WheelPos[ i ][ j ] > 360.0f ) WheelPos[ i ][ j ] -= 360.0f;
		}

		if( WheelPos[ i ][ 1 ] < WheelPos[ i ][ 0 ] || WheelPos[ i ][ 3 ] < WheelPos[ i ][ 2 ] )
		{
			WheelPos[ i ][ 1 ] = 360.0f;
			WheelPos[ i ][ 2 ] = 0.0f;
		}
	}
/*
	FILE		*fh;

	if( fopen_s( &fh, "O:\\fugHueKey.log", "a+" ) == 0 )
	{
		fprintf( fh, "( %03.2f - %03.2f ) ( %03.2f - %03.2f )\n", SoftPos[ 0 ], SoftPos[ 1 ], SoftPos[ 2 ], SoftPos[ 3 ] );

		fclose( fh );
	}
*/

	while( Remaining-- > 0 )
	{
		if( LmaPixel->mHue == 0 && LmaPixel->mSat == 0 )
		{
			*DstPixel = *SrcPixel;
			
			DstPixel->alpha = 0x00;
		}
		else if( ( LmaPixel->mHue >= HardPos[ 0 ] && LmaPixel->mHue <= HardPos[ 1 ] ) || ( LmaPixel->mHue >= HardPos[ 2 ] && LmaPixel->mHue <= HardPos[ 3 ] ) )
		{
			*DstPixel = *KeyPixel;

			DstPixel->alpha = 0xff;
		}
		else if( ( LmaPixel->mHue >= SoftPos[ 0 ] && LmaPixel->mHue <= SoftPos[ 1 ] ) || ( LmaPixel->mHue >= SoftPos[ 2 ] && LmaPixel->mHue <= SoftPos[ 3 ] ) )
		{
			const	float	SoftRange = pSoftWidth - pWidth;
			const	float	HueDiff   = abs( AngleDiff( pHue, LmaPixel->mHue ) );
			const	float	SrcMix    = ( HueDiff - pWidth ) / SoftRange;
			const	float	KeyMix    = 1.0f - SrcMix;
			const	float	r = (float)SrcPixel->red   + ( ( (float)KeyPixel->red   - (float)SrcPixel->red   ) * KeyMix );
			const	float	g = (float)SrcPixel->green + ( ( (float)KeyPixel->green - (float)SrcPixel->green ) * KeyMix );
			const	float	b = (float)SrcPixel->blue  + ( ( (float)KeyPixel->blue  - (float)SrcPixel->blue  ) * KeyMix );

			DstPixel->red   = (BYTE)r;
			DstPixel->green = (BYTE)g;
			DstPixel->blue  = (BYTE)b;
			DstPixel->alpha = (BYTE)( KeyMix * 255.0f );
		}
		else
		{
			*DstPixel = *SrcPixel;

			DstPixel->alpha = 0x00;
		}

		SrcPixel++;
		KeyPixel++;
		LmaPixel++;
		DstPixel++;
	}

	return( FF_SUCCESS );
}
