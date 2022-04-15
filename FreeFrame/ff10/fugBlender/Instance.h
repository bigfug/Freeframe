
#ifndef _FF_FFINSTANCE_H_
#define _FF_FFINSTANCE_H_

#include "FreeFrame.h"
#include "FrameBuffer.h"
#include "FrameStore.h"

class ffInstance : public FreeFrameInstance
{
private:
	ffFrameBuffer				*mFrameBuffer;
	FrameStore					*mFrameStore;
	BYTE						*mTempFrame1;
	BYTE						*mTempFrame2;
	BYTE						*mTempPtr;
	UINT						 mFrameCount;
	UINT						 mChangeLength;
	double						 mBlendAmount;
public:
	ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *videoInfo );

	DWORD	processFrame( VideoFrame pFrame );
	//DWORD	processFrameCopy( ProcessFrameCopyStruct *pFrameData );
};

#endif