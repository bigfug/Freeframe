
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
	return( FF_SUCCESS );
}
