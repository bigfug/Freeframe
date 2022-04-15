
#ifndef __FF_FFINSTANCE_H__
#define __FF_FFINSTANCE_H__

#include <time.h>
#include "FreeFrame.h"
#include "FrameBuffer.h"

enum
{
	FF_FILL_NONE,
	FF_FILL_TOP,
	FF_FILL_BOTTOM,
	FF_FILL_SCOPE
};

class ffInstance : public FreeFrameInstance
{
private:
	ffFrameBuffer				*FrameBuffer;
public:
	ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *videoInfo );

	DWORD	processFrame( VideoFrame pFrame );
};

#endif