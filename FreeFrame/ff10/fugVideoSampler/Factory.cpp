
#include "FreeFramePlugin.h"
#include "FreeFrameInstance.h"
#include "Plugin.h"

static ParamConstantsStruct paramConstants[] =
{
	{ "Sample",			FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Play",			FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f  },
	{ "Shuttle",		FF_TYPE_STANDARD,	0.5f + ( 0.5f / MAX_SHUTTLE ), -MAX_SHUTTLE, MAX_SHUTTLE },
	{ "Max Frames",		FF_TYPE_STANDARD,	0.1f, 0.0f, MAX_FRAMES },
	{ "Positioning",	FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Position",		FF_TYPE_STANDARD,	0.0f, 0.0f, 1.0f }
};

FreeFramePlugin *FreeFramePluginFactory::getInstance()
{
	return( new ffPlugin( paramConstants, NUM_PARAMS ) );
}
