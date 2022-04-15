
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
#include "StreamEncoderFactory.h"

#define		SEND_BUFF_SIZE		(4*1024)

class ffInstance : public FreeFrameInstance
{
private:
	ffFrameBuffer				*mFrameBuffer;
	VideoFrame					 mFrameData;
	VideoInfoStruct				 mFrameInfo;
	float						 mFrameSkip;
	UINT						 mLastConfig;
	vector<StreamEncoder *>		 mEncoders;

public:
	ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *videoInfo );
	~ffInstance();

	DWORD	processFrame( VideoFrame pFrame );
};

#endif