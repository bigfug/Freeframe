
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
	{ "Advanced",		FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Blend Mode",		FF_TYPE_STANDARD,	0.0f, 0.0f, 8.0f },
};

enum
{
	FF_PARAM_ADVANCED,
	FF_PARAM_BLEND_MODE,
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

