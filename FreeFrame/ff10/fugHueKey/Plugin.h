
#ifndef __FF_FFPLUGIN_H__
#define __FF_FFPLUGIN_H__

#include "FreeFrame.h"
#include "FreeFramePlugin.h"

enum
{
	FF_PARAM_HUE,
	FF_PARAM_WIDTH,
	FF_PARAM_SOFTWIDTH,
	FF_PARAM_MASK,
	NUM_PARAMS
};

#define NUM_INPUTS 2

extern	const ParamConstantsStruct		paramConstants[];

class ffPlugin : public FreeFramePlugin
{
private:
public:
	ffPlugin( const ParamConstantsStruct *Parameters, DWORD Count );

	DWORD					 getPluginCaps( DWORD index );
	FreeFrameInstance		*getInstance( VideoInfoStruct *VideoInfo );
};

#endif