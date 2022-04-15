
#ifndef _FF_FFINSTANCE_H_
#define _FF_FFINSTANCE_H_

#include <malloc.h>
#include <time.h>
#include <sys/types.h>
#include <stdio.h>
#include <winsock2.h>

#include "FreeFrame.h"
#include "FrameBuffer.h"
#include "MemCopy.h"
#include "StreamDecoderFactory.h"

// Russell - PluginInstance Object - these calls relate to instances of plugObj
// created by FF_INSTANTIATE

class ffInstance : public FreeFrameInstance
{
private:
	VideoInfoStruct		 mDstVideoInfo;
	ffFrameBuffer		*mBuffer;
	StreamDecoder		*mDecoder;
	StreamProtocol		 mCurrProtocol;
	StreamProtocol		 mLastProtocol;
	UINT				 mCurrPort;
public:
	ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *videoInfo );
	~ffInstance();

	DWORD	processFrame( VideoFrame pFrame );
};

#endif