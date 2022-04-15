
//////////////////////////////////////////////////////////////////////////////////
//
// includes
//

#include "FreeFrame.h"
#include "FreeFramePlugin.h"
#include "Instance.h"

#define		NUM_INPUTS		1

static	ParamConstantsStruct	  paramConstants[] = 
{
	{ "Threshold",		FF_TYPE_STANDARD,	0.255f, 0.0f, 255.0f },
	{ "Frame Sample",	FF_TYPE_STANDARD,	0.0f, 0.0f, 100.0f },
	{ "Draw Below",		FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Colour",			FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Key Frame",		FF_TYPE_STANDARD,	0.18f, 0.0f, 255.0f },
};

enum
{
	FF_PARAM_THRESHOLD,
	FF_PARAM_FRAMES,
	FF_PARAM_DRAW_BELOW,
	FF_PARAM_COLOUR,
	FF_PARAM_KEYFRAME,
	NUM_PARAMS
};

class ffPlugin : public FreeFramePlugin
{
private:
public:
	ffPlugin( ParamConstantsStruct *Parameters, DWORD Count );

	DWORD					 getPluginCaps( DWORD index );
	FreeFrameInstance		*getInstance( VideoInfoStruct *VideoInfo );
};

