
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
	{ "Bypass",			FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Sample",			FF_TYPE_BOOLEAN,	1.0f, 0.0f, 1.0f },
	{ "Offset",			FF_TYPE_STANDARD,	0.5f, 0.0f, 1.0f },
	{ "Fill Gaps",		FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
};

enum
{
	FF_PARAM_BYPASS,
	FF_PARAM_SAMPLE,
	FF_PARAM_OFFSET,
	FF_PARAM_FILL_GAPS,
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

