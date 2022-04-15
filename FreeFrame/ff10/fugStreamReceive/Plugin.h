
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
#if PLUGIN_TYPE == FF_EFFECT
	{ "Receive Data",	FF_TYPE_BOOLEAN,	0.0f,    0.0f,     1.0f },
#endif
	{ "Port",			FF_TYPE_STANDARD,	( 10000.0f / 63336.0f ), 1.0f, 65535.0f },
	{ "Protocol",		FF_TYPE_STANDARD,	0.0, 0.0f, 2.0f },
	{ "Help!",			FF_TYPE_BOOLEAN,	0.0f,    0.0f,     1.0f },
};

enum
{
#if PLUGIN_TYPE == FF_EFFECT
	FF_PARAM_RECEIVE,
#endif
	FF_PARAM_PORT,
	FF_PARAM_PROTOCOL,
	FF_PARAM_HELP,
	NUM_PARAMS
};

class ffPlugin : public FreeFramePlugin
{
private:
public:
	ffPlugin( ParamConstantsStruct *Parameters, DWORD Count );
	~ffPlugin();

	DWORD					 getPluginCaps( DWORD index );
	FreeFrameInstance		*getInstance( VideoInfoStruct *VideoInfo );
};

