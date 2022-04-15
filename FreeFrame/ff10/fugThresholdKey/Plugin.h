
#ifndef __FF_FFPLUGIN_H__
#define __FF_FFPLUGIN_H__

#include "FreeFrame.h"
#include "FreeFramePlugin.h"

enum
{
	FF_PARAM_START,
	FF_PARAM_END,
	FF_PARAM_INVERT,
	FF_PARAM_MASK,
	NUM_PARAMS
};

#define NUM_INPUTS 2

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