
#include "stdafx.h"
#include "FreeFramePlugin.h"
#include "FreeFrameInstance.h"
#include "Plugin.h"

FreeFramePlugin *FreeFramePluginFactory::getInstance()
{
	return( new ffPlugin( paramConstants, NUM_PARAMS ) );
}
