
#ifndef _FF_FFINSTANCE_H_
#define _FF_FFINSTANCE_H_

#include <malloc.h>
#include <time.h>
#include <sys/types.h>
#include <stdio.h>
#include <winsock2.h>

#include "FreeFrame.h"
#include "FrameBuffer.h"

#include "portmidi.h"

// Russell - PluginInstance Object - these calls relate to instances of plugObj
// created by FF_INSTANTIATE

class ffInstance : public FreeFrameInstance
{
private:
	PmDeviceID			 mDeviceId;
	PortMidiStream		*mStream;
	unsigned char		 mControllers[ 8 ];
	bool				 mTriggers[ 8 ];
public:
	ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *videoInfo );
	~ffInstance();

	DWORD	processFrame( VideoFrame pFrame );
};

#endif