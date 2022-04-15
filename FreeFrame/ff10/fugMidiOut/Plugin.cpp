
#include "stdafx.h"
#include <string.h>
#include <stdio.h>
#include "FreeFramePlugin.h"
#include "Plugin.h"
#include "Instance.h"
#include <direct.h>

StreamConfig			 gConfig;

///////////////////////////////////////////////////////////////////////////////////////
// initialise
//
// do nothing for now - plugin instantiate is where the init happens now

ffPlugin::ffPlugin( ParamConstantsStruct *Parameters, DWORD Count ) : FreeFramePlugin( Parameters, Count )
{
	plugInfo.APIMajorVersion = 1;		// number before decimal point in version nums
	plugInfo.APIMinorVersion = 000;		// this is the number after the decimal point
										// so version 0.511 has major num 0, minor num 501
	char ID[5] = "BF05";		 // this *must* be unique to your plugin 
								 // see www.freeframe.org for a list of ID's already taken
	char name[17] = "fugMidiOut";
	
	memcpy(plugInfo.uniqueID, ID, 4);
	memcpy(plugInfo.pluginName, name, 16);
	plugInfo.pluginType = FF_EFFECT;

	plugExtInfo.PluginMajorVersion = 1;
	plugExtInfo.PluginMinorVersion = 1;

	plugExtInfo.Description = "MidiOut";
	plugExtInfo.About = "(c)2006 Alex May - www.bigfug.com";

	// FF extended data block is not in use by the API yet
	// we will define this later if we want to
	plugExtInfo.FreeFrameExtendedDataSize = 0;
	plugExtInfo.FreeFrameExtendedDataBlock = NULL;

/*
	gConfig.mNext = 0;

	gConfig.mNumber = 0;

	gConfig.mAddress[ 0 ] = 127;
	gConfig.mAddress[ 1 ] = 0;
	gConfig.mAddress[ 2 ] = 0;
	gConfig.mAddress[ 3 ] = 1;

	gConfig.mPort = 10000;

	char				 buff[ 1024 ];
	StreamConfig		*Config = &gConfig;
	bool				 DoAlloc = false;
	FILE				*ConfigFH = NULL;
	int					 ConfCount = 0;
	char				 currdir[ 256 ];

	if( _getcwd( currdir, 256 ) != NULL )
	{
		char	*LastSlashPos = strrchr( currdir, '\\' );

		while( ConfigFH == NULL && currdir[ 0 ] != 0x00 )
		{
			sprintf_s( buff, 1024, "%s\\fugStreamSend.cfg", currdir );

			//MessageBox( NULL, buff, "StreamSend", MB_OK );

			fopen_s( &ConfigFH, buff, "r" );

			*LastSlashPos = 0x00;

			LastSlashPos = strrchr( currdir, '\\' );

			if( LastSlashPos == NULL )
			{
				LastSlashPos = currdir;
			}
		}
	}

	if( ConfigFH != NULL )
	{
		int		Num, IP1, IP2, IP3, IP4, Port;

		while( fgets( buff, 256, ConfigFH ) != NULL )
		{
			if( buff[ 0 ] == ';' || buff[ 0 ] == 0x0a || buff[ 0 ] == 0x0d )
			{
			}
			else if( sscanf_s( buff, "%d=%d.%d.%d.%d:%d", &Num, &IP1, &IP2, &IP3, &IP4, &Port ) == 6 )
			{
				if( !DoAlloc )
				{
					DoAlloc = true;
				}
				else
				{
					StreamConfig *NewConfig = (StreamConfig *)malloc( sizeof(StreamConfig) );

					if( NewConfig )
					{
						Config->mNext = NewConfig;

						ConfCount = max( ConfCount, Num );
					}

					Config = NewConfig;
				}

				if( Config )
				{
					Config->mNext = 0;
					Config->mNumber = Num;
					Config->mAddress[ 0 ] = (unsigned char)IP1;
					Config->mAddress[ 1 ] = (unsigned char)IP2;
					Config->mAddress[ 2 ] = (unsigned char)IP3;
					Config->mAddress[ 3 ] = (unsigned char)IP4;
					Config->mPort = Port;
				}
			}
		}

		fclose( ConfigFH );

		paramConstants[ 1 ].maxValue = (float)ConfCount;
	}
	else
	{
		MessageBox( NULL, "Couldn't open fugStreamSend.cfg", "StreamSend", MB_OK );
	}
*/
}

ffPlugin::~ffPlugin()
{
/*
	StreamConfig		*Config = gConfig.mNext;

	while( Config )
	{
		StreamConfig	*Next = Config->mNext;

		free( Config );

		Config = Next;
	}

	gConfig.mNext = 0;
*/
}

DWORD ffPlugin::getPluginCaps( DWORD index )
{
	switch( index )
	{
		case FF_CAP_16BITVIDEO:
			return( FF_SUPPORTED );

		case FF_CAP_24BITVIDEO:
			return( FF_SUPPORTED );

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
