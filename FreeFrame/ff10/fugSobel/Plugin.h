
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
	{ "X",				FF_TYPE_BOOLEAN,	1.0f, 0.0f, 1.0f },
	{ "Y",				FF_TYPE_BOOLEAN,	1.0f, 0.0f, 1.0f },
	{ "Invert",			FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
};

enum
{
	FF_PARAM_COLOUR,
	FF_PARAM_X,
	FF_PARAM_Y,
	FF_PARAM_INVERT,
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

