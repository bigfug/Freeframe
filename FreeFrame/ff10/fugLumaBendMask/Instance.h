
#ifndef __FF_FFINSTANCE_H__
#define __FF_FFINSTANCE_H__

#include "FreeFrame.h"
#include "FrameBuffer.h"

// Russell - PluginInstance Object - these calls relate to instances of plugObj
// created by FF_INSTANTIATE

class ffInstance : public FreeFrameInstance
{
private:
	ffFrameBuffer				*FrameBuffer;
	DWORD						 mPixelSize;
	DWORD						 mFrameSize;
	unsigned char				*mLumaMap;
public:
	ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *videoInfo );

	~ffInstance();

	DWORD	processFrameCopy( ProcessFrameCopyStruct* pFrameData );
};

#endif