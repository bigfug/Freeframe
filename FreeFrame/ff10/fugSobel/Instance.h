
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
	VideoPixel32bit				*Buffer1;
	VideoPixel32bit				*Buffer2;
public:
	ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *videoInfo );

	DWORD	processFrame( VideoFrame pFrame );
};

#endif