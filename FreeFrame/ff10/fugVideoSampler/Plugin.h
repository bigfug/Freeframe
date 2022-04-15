
#ifndef __FF_FFPLUGIN_H__
#define __FF_FFPLUGIN_H__

#include "FreeFrame.h"
#include "FreeFramePlugin.h"

#define MAX_SHUTTLE		(8.0f)
#define	MAX_FRAMES		(5000)

enum
{
	FF_PARAM_SAMPLE,
	FF_PARAM_PLAY,
	FF_PARAM_SHUTTLE,
	FF_PARAM_FRAMES,
	FF_PARAM_POSITIONING,
	FF_PARAM_POSITION,
	FF_PARAM_CUE_IN,
	FF_PARAM_CUE_OUT,
	NUM_PARAMS
};

#define NUM_INPUTS 1

extern	ParamConstantsStruct		paramConstants[];

class ffPlugin : public FreeFramePlugin
{
private:
public:
	ffPlugin( ParamConstantsStruct *Parameters, DWORD Count );
	DWORD					 getPluginCaps( DWORD index );
	FreeFrameInstance		*getInstance( VideoInfoStruct *VideoInfo );
};

#endif