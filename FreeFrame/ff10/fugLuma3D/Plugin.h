
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
	{ "Draw Lines",		FF_TYPE_BOOLEAN,	0.0f, 0.0f,  1.0f },
	{ "Pixel Skip",		FF_TYPE_STANDARD,	0.0f, 1.0f, 30.0f },
	{ "Line Distance",	FF_TYPE_STANDARD,	0.25f, 0.0f,  1.0f },
	{ "Center X",		FF_TYPE_STANDARD,	0.5f, 0.0f,  1.0f },
	{ "Center Y",		FF_TYPE_STANDARD,	0.5f, 0.0f,  1.0f },
	{ "Invert",			FF_TYPE_BOOLEAN,	0.0f, 0.0f,  1.0f },
	{ "Luma Scale",		FF_TYPE_STANDARD,	1.0f, 0.0f,  1.0f },
//	{ "Edge",			FF_TYPE_BOOLEAN,	0.0f, 0.0f,  1.0f },
};

enum
{
	FF_PARAM_LINES,
	FF_PARAM_SKIP,
	FF_PARAM_DISTANCE,
	FF_PARAM_CENTERX,
	FF_PARAM_CENTERY,
	FF_PARAM_INVERT,
	FF_PARAM_LUMA_EFFECT,
//	FF_PARAM_EDGE,
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

