
#ifndef __FF_FFINSTANCE_H__
#define __FF_FFINSTANCE_H__

#include "FreeFrame.h"
#include "FrameBuffer.h"

#define	FRAME_INDEX		(100)

// Russell - PluginInstance Object - these calls relate to instances of plugObj
// created by FF_INSTANTIATE

typedef struct VideoFrameChainStruct
{
	struct	VideoFrameChainStruct	*Next;
	struct	VideoFrameChainStruct	*Prev;
	BYTE							 Frame;
}
VideoFrameChain;

class ffInstance : public FreeFrameInstance
{
private:
	ffFrameBuffer				*FrameBuffer;
	VideoFrameChain				*FrameHead;
	VideoFrameChain				*FrameTail;
	VideoFrameChain				*FrameSave;
	VideoFrameChain				*FramePlay;
	VideoFrameChain				*FrameRecord[ MAX_FRAMES / FRAME_INDEX ];
	bool						 Sample;
	bool						 Play;
	DWORD						 FrameSize;
	int							 FrameCount;
	float						 Shuttle;
	int						 FramePos;
public:
	ffInstance( ffPlugin *Plugin, VideoInfoStruct *videoInfo );
	~ffInstance();

	DWORD	processFrame( VideoFrame pFrame );
	void	removeFrames();
	DWORD	addFrame( VideoFrame pFrame );
};

#endif