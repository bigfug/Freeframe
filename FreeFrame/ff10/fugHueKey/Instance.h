
#ifndef __FF_FFINSTANCE_H__
#define __FF_FFINSTANCE_H__

#include "FreeFrame.h"
#include "FrameBuffer.h"

#include <math.h>

// Russell - PluginInstance Object - these calls relate to instances of plugObj
// created by FF_INSTANTIATE

class ffInstance : public FreeFrameInstance
{
private:
	ffFrameBuffer				*FrameBuffer;
	VideoPixelHSL				*mHSL;
public:
	ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *videoInfo );

	DWORD	processFrameCopy( ProcessFrameCopyStruct* pFrameData );
};

#endif