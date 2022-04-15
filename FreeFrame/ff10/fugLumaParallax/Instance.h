
#ifndef _FF_FFINSTANCE_H_
#define _FF_FFINSTANCE_H_

#include "FreeFrame.h"
#include "FrameBuffer.h"

// Russell - PluginInstance Object - these calls relate to instances of plugObj
// created by FF_INSTANTIATE

class ffInstance : public FreeFrameInstance
{
private:
	ffFrameBuffer				*FrameBuffer;
	VideoPixel32bit				*Buffer[ 4 ];
	VideoPixel8bit				*mLumaBuff;
public:
	ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *videoInfo );
	~ffInstance( void );

	DWORD	processFrame( VideoFrame pFrame );
};

#endif