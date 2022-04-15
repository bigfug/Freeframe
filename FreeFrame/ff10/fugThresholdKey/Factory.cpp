
#include "FreeFramePlugin.h"
#include "FreeFrameInstance.h"
#include "Plugin.h"

static ParamConstantsStruct paramConstants[] =
{
	{ "Threshold Start",	FF_TYPE_STANDARD,	0.3f, 0.0f, 1.0f },
	{ "Threshold End",		FF_TYPE_STANDARD,	0.7f, 0.0f, 1.0f },
	{ "Invert",				FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Mask Mode",			FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f }
};

FreeFramePlugin *FreeFramePluginFactory::getInstance()
{
	return( new ffPlugin( paramConstants, NUM_PARAMS ) );
}
