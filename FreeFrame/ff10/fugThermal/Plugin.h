
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
	{ "Colour",			FF_TYPE_BOOLEAN,	1.0f, 0.0f, 1.0f },
	{ "Hue Rotate",		FF_TYPE_STANDARD,	0.0f, 0.0f, 360.0f },
	{ "Blur Radius",	FF_TYPE_STANDARD,	0.0f, 0.0f, 10.0f },
	{ "Luma Overlay",	FF_TYPE_STANDARD,	0.0f, 0.0f, 255.0f },
};

enum
{
	FF_PARAM_COLOUR,
	FF_PARAM_HUE_ROTATE,
	FF_PARAM_BLUR_RADIUS,
	FF_PARAM_LUMA_OVERLAY,
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

