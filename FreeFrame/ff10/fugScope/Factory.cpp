
#include "FreeFramePlugin.h"
#include "FreeFrameInstance.h"
#include "Plugin.h"

static ParamConstantsStruct paramConstants[] =
{
	{ "Red",			FF_TYPE_RED,		1.0f, 0.0f, 255.0f },
	{ "Green",			FF_TYPE_GREEN,		1.0f, 0.0f, 255.0f },
	{ "Blue",			FF_TYPE_BLUE,		1.0f, 0.0f, 255.0f },
	{ "Position",		FF_TYPE_STANDARD,	0.5f, 0.0f, 1.0f },
	{ "Scale",			FF_TYPE_STANDARD,	0.5f, 0.0f, 1.0f },
	{ "Draw Lines",		FF_TYPE_BOOLEAN,	1.0f, 0.0f, 1.0f },
	{ "Fill Mode",		FF_TYPE_STANDARD,	0.0f, 0.0f, 3.0f },
	{ "Vertical",		FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Pixel Height",	FF_TYPE_STANDARD,	0.0f, 0.0f, 1.0f }
};

FreeFramePlugin *FreeFramePluginFactory::getInstance()
{
	return( new ffPlugin( paramConstants, NUM_PARAMS ) );
}
