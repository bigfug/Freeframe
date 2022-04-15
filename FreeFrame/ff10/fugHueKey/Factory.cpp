
#include "FreeFramePlugin.h"
#include "FreeFrameInstance.h"
#include "Plugin.h"

static const ParamConstantsStruct paramConstants[] =
{
	{ "Hue",				FF_TYPE_STANDARD,	  0.0f, 0.0f, 360.0f },
	{ "Width",				FF_TYPE_STANDARD,   0.166f, 0.0f, 360.0f },
	{ "Soft Width",			FF_TYPE_STANDARD,	  0.3f, 0.0f, 360.0f },
	{ "Mask Mode",			FF_TYPE_BOOLEAN,	  0.0f, 0.0f,   1.0f }
};

FreeFramePlugin *FreeFramePluginFactory::getInstance()
{
	return( new ffPlugin( paramConstants, NUM_PARAMS ) );
}
