
#include <string.h>
#include <stdio.h>
#include "Plugin.h"
#include "Instance.h"

ffPlugin::ffPlugin( ParamConstantsStruct *Parameters, DWORD Count ) : FreeFramePlugin( Parameters, Count )
{
	plugInfo.APIMajorVersion = 1;		// number before decimal point in version nums
	plugInfo.APIMinorVersion = 000;		// this is the number after the decimal point
										// so version 0.511 has major num 0, minor num 501
	char ID[5] = "BF03";		 // this *must* be unique to your plugin 
								 // see www.freeframe.org for a list of ID's already taken
	char name[17] = "fugLumaMaskBend";
	
	memcpy(plugInfo.uniqueID, ID, 4);
	memcpy(plugInfo.pluginName, name, 16);
	plugInfo.pluginType = FF_EFFECT;

	plugExtInfo.PluginMajorVersion = 1;
	plugExtInfo.PluginMinorVersion = 000;

	// I'm just passing null for description etc for now
	// todo: send through description and about
	plugExtInfo.Description = "LumaMaskBend";
	plugExtInfo.About = "(c)2006 Alex May - www.bigfug.com";

	// FF extended data block is not in use by the API yet
	// we will define this later if we want to
	plugExtInfo.FreeFrameExtendedDataSize = 0;
	plugExtInfo.FreeFrameExtendedDataBlock = NULL;
}

DWORD ffPlugin::getPluginCaps(DWORD index)
{
	switch (index) {

	case FF_CAP_16BITVIDEO:
		return FF_TRUE;

	case FF_CAP_24BITVIDEO:
		return FF_TRUE;

	case FF_CAP_32BITVIDEO:
		return FF_TRUE;

	case FF_CAP_PROCESSFRAMECOPY:
		return FF_SUPPORTED;

	case FF_CAP_MINIMUMINPUTFRAMES:
		return NUM_INPUTS;

	case FF_CAP_MAXIMUMINPUTFRAMES:
		return NUM_INPUTS;

	case FF_CAP_COPYORINPLACE:
		return FF_CAP_PREFER_COPY;

	default:
		return FF_FALSE;
	}
}

FreeFrameInstance *ffPlugin::getInstance( VideoInfoStruct *VideoInfo )
{
	return( new ffInstance( this, VideoInfo ) );
}
