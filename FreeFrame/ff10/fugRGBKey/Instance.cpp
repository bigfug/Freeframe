
#include <stdio.h>
#include "Plugin.h"
#include "Instance.h"

ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	FrameBuffer = ffFrameBufferFactory::getInstance( VideoInfo );

	mHSL = new VideoPixelHSL[ VideoInfo->frameWidth * VideoInfo->frameHeight ];
}

DWORD ffInstance::processFrameCopy( ProcessFrameCopyStruct* pFrameData )
{
	const	BYTE		 ThreshStart = (BYTE)( this->getParamFloat( FF_PARAM_START ) * 255.0f );
	const	BYTE		 ThreshEnd   = (BYTE)( this->getParamFloat( FF_PARAM_END   ) * 255.0f );
	const	bool		 MaskMode    = this->getParamBool( FF_PARAM_MASK );
	const	bool		 pRed        = this->getParamBool( FF_PARAM_RED );
	const	bool		 pGrn        = this->getParamBool( FF_PARAM_GREEN );
	const	bool		 pBlu        = this->getParamBool( FF_PARAM_BLUE );
	const	bool		 pInvert     = this->getParamBool( FF_PARAM_INVERT );
	const	float		 pWidth      = this->getParamFloat( FF_PARAM_WIDTH );
	const	float		 pRotation   = this->getParamFloat( FF_PARAM_ROTATION );
	float				 RedPos[]    = { 360.0f, 360.0f,   0.0f,   0.0f };
	float				 GrnPos[]    = { 120.0f, 120.0f, 120.0f, 120.0f };
	float				 BluPos[]    = { 240.0f, 240.0f, 240.0f, 240.0f };
	float				*WheelPos[]  = { RedPos, GrnPos, BluPos };
	VideoPixel32bit		*SrcPixel;
	VideoPixel32bit		*KeyPixel;
	VideoPixel32bit		*DstPixel;
	VideoPixelHSL		*LmaPixel = this->mHSL;
	int					 Remaining = this->FrameBuffer->getWidth() * this->FrameBuffer->getHeight();

	SrcPixel = pFrameData->InputFrames[ 0 ].Vid32;
	KeyPixel = pFrameData->InputFrames[ 1 ].Vid32;
	DstPixel = pFrameData->OutputFrame.Vid32;

	this->FrameBuffer->RGB2HSL( pFrameData->InputFrames[ 1 ].Vid32, this->mHSL );

	for( int i = 0 ; i < 3 ; i++ )
	{
		WheelPos[ i ][ 0 ] -= pWidth;
		WheelPos[ i ][ 3 ] += pWidth;

		for( int j = 0 ; j < 4 ; j++ )
		{
			WheelPos[ i ][ j ] += pRotation;

			while( WheelPos[ i ][ j ] < 0.0f   ) WheelPos[ i ][ j ] += 360.0f;
			while( WheelPos[ i ][ j ] > 360.0f ) WheelPos[ i ][ j ] -= 360.0f;
		}

		if( WheelPos[ i ][ 1 ] < WheelPos[ i ][ 0 ] || WheelPos[ i ][ 3 ] < WheelPos[ i ][ 2 ] )
		{
			WheelPos[ i ][ 1 ] = 360.0f;
			WheelPos[ i ][ 2 ] = 0.0f;
		}
	}

	while( Remaining-- > 0 )
	{
		if( LmaPixel->mHue == 0 && LmaPixel->mSat == 0 )
		{
			*DstPixel++ = *SrcPixel;
		}
		else if( pRed && ( ( LmaPixel->mHue >= RedPos[ 0 ] && LmaPixel->mHue <= RedPos[ 1 ] ) || ( LmaPixel->mHue >= RedPos[ 2 ] && LmaPixel->mHue <= RedPos[ 3 ] ) ) )
		{
			*DstPixel++ = *KeyPixel;
		}
		else if( pGrn && ( ( LmaPixel->mHue >= GrnPos[ 0 ] && LmaPixel->mHue <= GrnPos[ 1 ] ) || ( LmaPixel->mHue >= GrnPos[ 2 ] && LmaPixel->mHue <= GrnPos[ 3 ] ) ) )
		{
			*DstPixel++ = *KeyPixel;
		}
		else if( pBlu && ( ( LmaPixel->mHue >= BluPos[ 0 ] && LmaPixel->mHue <= BluPos[ 1 ] ) || ( LmaPixel->mHue >= BluPos[ 2 ] && LmaPixel->mHue <= BluPos[ 3 ] ) ) )
		{
			*DstPixel++ = *KeyPixel;
		}
		else
		{
			*DstPixel++ = *SrcPixel;
		}

		SrcPixel++;
		KeyPixel++;
		LmaPixel++;
	}

	return( FF_SUCCESS );
}

/*
DWORD ffInstance::processFrameCopy( ProcessFrameCopyStruct* pFrameData )
{
	VideoPixel32bit		*SrcPixel;
	VideoPixel32bit		*KeyPixel;
	VideoPixel32bit		*DstPixel;
	VideoPixel32bit		*LmaPixel;
	const	BYTE		 ThreshStart = (BYTE)( this->getParamFloat( FF_PARAM_START ) * 255.0f );
	const	BYTE		 ThreshEnd   = (BYTE)( this->getParamFloat( FF_PARAM_END   ) * 255.0f );
	const	bool		 MaskMode    = this->getParamBool( FF_PARAM_MASK );
	const	bool		 pRed        = this->getParamBool( FF_PARAM_RED );
	const	bool		 pGrn        = this->getParamBool( FF_PARAM_GREEN );
	const	bool		 pBlu        = this->getParamBool( FF_PARAM_BLUE );
	const	bool		 pInvert     = this->getParamBool( FF_PARAM_INVERT );

	SrcPixel = pFrameData->InputFrames[ 0 ].Vid32;
	KeyPixel = pFrameData->InputFrames[ 1 ].Vid32;
	DstPixel = pFrameData->OutputFrame.Vid32;

	for( register DWORD x = 0 ; x < this->videoInfo.frameWidth ; x++ )
	{
		for( register DWORD y = 0 ; y < this->videoInfo.frameHeight ; y++ )
		{
			if( MaskMode )
			{
				LmaPixel = SrcPixel;
			}
			else
			{
				LmaPixel = KeyPixel;
			}

			if( pRed && 
				( (  pInvert && ( LmaPixel->red >= ThreshStart && LmaPixel->red <= ThreshEnd ) ) ||
				  ( !pInvert && ( LmaPixel->red < ThreshStart || LmaPixel->red > ThreshEnd ) ) ) )
			{
				*DstPixel++ = *KeyPixel;
			}
			else if( pGrn &&
				( (  pInvert && ( LmaPixel->green >= ThreshStart && LmaPixel->green <= ThreshEnd ) ) ||
				  ( !pInvert && ( LmaPixel->green < ThreshStart || LmaPixel->green > ThreshEnd ) ) ) )
			{
				*DstPixel++ = *KeyPixel;
			}
			else if( pBlu &&
				( (  pInvert && ( LmaPixel->blue >= ThreshStart && LmaPixel->blue <= ThreshEnd ) ) ||
				  ( !pInvert && ( LmaPixel->blue < ThreshStart || LmaPixel->blue > ThreshEnd ) ) ) )
			{
				*DstPixel++ = *KeyPixel;
			}
			else
			{
				*DstPixel++ = *SrcPixel;
			}

			SrcPixel++;
			KeyPixel++;
		}
	}

	return( FF_SUCCESS );
}

*/