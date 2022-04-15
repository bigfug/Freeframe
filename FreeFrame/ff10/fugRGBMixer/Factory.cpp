
#include "FreeFramePlugin.h"
#include "FreeFrameInstance.h"
#include "Plugin.h"

static ParamConstantsStruct paramConstants[] =
{
	{ "Mix Channels",				FF_TYPE_BOOLEAN,	1.0f, 0.0f, 1.0f },
};

FreeFramePlugin *FreeFramePluginFactory::getInstance()
{
	return( new ffPlugin( paramConstants, NUM_PARAMS ) );
}
