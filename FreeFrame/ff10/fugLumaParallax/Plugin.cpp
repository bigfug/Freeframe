
#include "stdafx.h"
#include <string.h>
#include <stdio.h>
#include "FreeFramePlugin.h"
#include "Plugin.h"
#include "Instance.h"

///////////////////////////////////////////////////////////////////////////////////////
// initialise
//
// do nothing for now - plugin instantiate is where the init happens now

ffPlugin::ffPlugin( ParamConstantsStruct *Parameters, DWORD Count ) : FreeFramePlugin( Parameters, Count )
{
	plugInfo.APIMajorVersion = 1;		// number before decimal point in version nums
	plugInfo.APIMinorVersion = 000;		// this is the number after the decimal point
										// so version 0.511 has major num 0, minor num 501
	char ID[5] = "BF04";		 // this *must* be unique to your plugin 
								 // see www.freeframe.org for a list of ID's already taken
	char name[17] = "fugLumaParallax";
	
	memcpy(plugInfo.uniqueID, ID, 4);
	memcpy(plugInfo.pluginName, name, 16);
	plugInfo.pluginType = FF_EFFECT;

	plugExtInfo.PluginMajorVersion = 1;
	plugExtInfo.PluginMinorVersion = 1;

	plugExtInfo.Description = "LumaParallax";
	plugExtInfo.About = "(c)2007 Alex May - www.bigfug.com";

	// FF extended data block is not in use by the API yet
	// we will define this later if we want to
	plugExtInfo.FreeFrameExtendedDataSize = 0;
	plugExtInfo.FreeFrameExtendedDataBlock = NULL;
}

DWORD ffPlugin::getPluginCaps( DWORD index )
{
	switch( index )
	{
		case FF_CAP_16BITVIDEO:
			return( FF_UNSUPPORTED );

		case FF_CAP_24BITVIDEO:
			return( FF_UNSUPPORTED );

		case FF_CAP_32BITVIDEO:
			return( FF_SUPPORTED );

		case FF_CAP_PROCESSFRAMECOPY:
			return( FF_SUPPORTED );

		case FF_CAP_MINIMUMINPUTFRAMES:
			return( NUM_INPUTS );

		case FF_CAP_MAXIMUMINPUTFRAMES:
			return( NUM_INPUTS );

		case FF_CAP_COPYORINPLACE:
			return( FF_CAP_PREFER_INPLACE );
	}

	return( FF_UNSUPPORTED );
}

FreeFrameInstance *ffPlugin::getInstance( VideoInfoStruct *VideoInfo )
{
	return( new ffInstance( this, VideoInfo ) );
}
