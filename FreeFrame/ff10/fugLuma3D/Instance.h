
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
	VideoPixel8bit				*mLumaBuff;
	VideoPixel32bit				*mTempBuff;
public:
	ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *videoInfo );

	DWORD	processFrame( VideoFrame pFrame );
	DWORD ffInstance::processFrameCopy( ProcessFrameCopyStruct *pFrameData );
};

#endif