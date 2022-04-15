
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <math.h>
#include "Plugin.h"
#include "Instance.h"

ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	PmError		Error;

	for( int i = 0 ; i < 8 ; i++ )
	{
		this->mControllers[ i ] = 0;
		this->mTriggers[ i ] = false;
	}

	this->mDeviceId = pmNoDevice;
	this->mStream   = 0;

	if( ( Error = Pm_Initialize() ) != pmNoError )
	{
		MessageBox( NULL, "Pm_Initialise() failed", "fugMidiOut", MB_OK );

		return;
	}

	const int	 DeviceCount = Pm_CountDevices();
	FILE		*ConfigHandle = NULL;
	char		 MidiDeviceName[ 256 ];
	char		 CurrentDir[ 512 ];
	char		 ConfigPath[ 512 ];

	if( _getcwd( CurrentDir, 512 ) != NULL )
	{
		char	*LastSlashPos = strrchr( CurrentDir, '\\' );

		while( ConfigHandle == NULL && CurrentDir[ 0 ] != 0x00 )
		{
			sprintf_s( ConfigPath, 512, "%s\\fugMidiOut.cfg", CurrentDir );

			//MessageBox( NULL, ConfigPath, "StreamSend", MB_OK );

			fopen_s( &ConfigHandle, ConfigPath, "r" );

			*LastSlashPos = 0x00;

			LastSlashPos = strrchr( CurrentDir, '\\' );

			if( LastSlashPos == NULL )
			{
				LastSlashPos = CurrentDir;
			}
		}

		if( ConfigHandle != NULL )
		{
			while( fgets( ConfigPath, 512, ConfigHandle ) != NULL )
			{
				if( ConfigPath[ 0 ] == ';' || ConfigPath[ 0 ] == 0x0a || ConfigPath[ 0 ] == 0x0d )
				{
				}
				else if( strncmp( ConfigPath, "DEVICE=", 7 ) == 0 )
				{
					strcpy_s( MidiDeviceName, 256, &ConfigPath[ 7 ] );

					char *tmp = MidiDeviceName;

					while( *tmp && *tmp != 0x00 && *tmp != 0x0a && *tmp != 0x0d ) tmp++;

					*tmp = 0x00;

					//MessageBox( NULL, MidiDeviceName, "fugMidiOut", MB_OK );

					for( int i = 0 ; i < DeviceCount ; i++ )
					{
						const PmDeviceInfo	*DeviceInfo = Pm_GetDeviceInfo( i );

						if( strcmp( DeviceInfo->name, MidiDeviceName ) == 0 && DeviceInfo->output > 0 )
						{
							//MessageBox( NULL, "Found Device", "fugMidiOut", MB_OK );

							this->mDeviceId = i;
						}
					}
				}
			}

			fclose( ConfigHandle );
		}
	}

	if( this->mDeviceId == pmNoDevice )
	{
		if( ( this->mDeviceId = Pm_GetDefaultOutputDeviceID() ) == pmNoDevice )
		{
			MessageBox( NULL, "No output MIDI device found", "fugMidiOut", MB_OK );

			return;
		}

		if( _getcwd( CurrentDir, 512 ) != NULL )
		{
			sprintf_s( ConfigPath, 512, "%s\\fugMidiOut.cfg", CurrentDir );

			if( fopen_s( &ConfigHandle, ConfigPath, "w" ) == 0 )
			{
				for( int i = 0 ; i < DeviceCount ; i++ )
				{
					const PmDeviceInfo	*DeviceInfo = Pm_GetDeviceInfo( i );

					if( this->mDeviceId == i )
					{
						fprintf( ConfigHandle, "DEVICE=%s\n", DeviceInfo->name );
					}
					else if( DeviceInfo->output > 0 )
					{
						fprintf( ConfigHandle, ";DEVICE=%s\n", DeviceInfo->name );
					}
				}

				fclose( ConfigHandle );

				sprintf_s( CurrentDir, 512, "Wrote config to %s", ConfigPath );

				MessageBox( NULL, CurrentDir, "fugMidiOut", MB_OK );
			}
			else
			{
				sprintf_s( CurrentDir, 512, "Couldn't open config file %s", ConfigPath );

				MessageBox( NULL, CurrentDir, "fugMidiOut", MB_OK );
			}
		}
	}

	if( ( Error = Pm_OpenOutput( &this->mStream, this->mDeviceId, NULL, NULL, NULL, NULL, 0 ) ) != pmNoError )
	{
		MessageBox( NULL, "Error opening MIDI output device", "fugMidiOut", MB_OK );

		return;
	}

	const PmDeviceInfo	*DeviceInfo = Pm_GetDeviceInfo( this->mDeviceId );

	if( DeviceInfo != NULL )
	{
		//MessageBox( NULL, DeviceInfo->name, "fugMidiOut", MB_OK );
	}
}

ffInstance::~ffInstance()
{
	PmError		Error;

	if( this->mDeviceId != pmNoDevice && this->mStream != 0 )
	{
		if( ( Error = Pm_Close( this->mStream ) ) != pmNoError )
		{
			MessageBox( NULL, "Pm_Terminate() failed", "fugMidiOut", MB_OK );
		}

		this->mStream   = 0;
		this->mDeviceId = pmNoDevice;
	}

	if( ( Error = Pm_Terminate() ) != pmNoError )
	{
		MessageBox( NULL, "Pm_Terminate() failed", "fugMidiOut", MB_OK );

		return;
	}
}

inline unsigned char MIDI_NOTE_OFF( const int pChannel )
{
	return( 0x80 + pChannel - 1 );
}

inline unsigned char MIDI_NOTE_ON( const int pChannel )
{
	return( 0x90 + pChannel - 1 );
}

inline unsigned char MIDI_PARAMETER( const int pChannel )
{
	return( 0xb0 + pChannel - 1 );
}

DWORD ffInstance::processFrame( VideoFrame pFrame )
{
	PmError		Error;

	const int Channel = this->getParamInt( FF_PARAM_CHANNEL );

	for( int i = 0 ; i < 8 ; i++ )
	{
		const	int		IntData = this->getParamInt( FF_PARAM_CTRL_01 + i );

		if( IntData != this->mControllers[ i ] )
		{
			if( ( Error = Pm_WriteShort( this->mStream, 0, Pm_Message( MIDI_PARAMETER( Channel ), i + 32, IntData ) ) ) != pmNoError )
			{
				//MessageBox( NULL, "Pm_WriteShort() failed", "fugMidiOut", MB_OK );
			}

			this->mControllers[ i ] = IntData;
		}

		const	bool	BoolData = this->getParamBool( FF_PARAM_TRIG_01 + i );

		if( BoolData != this->mTriggers[ i ] )
		{
			if( BoolData )
			{
				Pm_WriteShort( this->mStream, 0, Pm_Message( MIDI_NOTE_ON( Channel ), 60 + i, 127 ) );
			}
			else
			{
				Pm_WriteShort( this->mStream, 0, Pm_Message( MIDI_NOTE_OFF( Channel ), 60 + i, 0 ) );
			}

			this->mTriggers[ i ] = BoolData;
		}
	}

	return( FF_SUCCESS );
}
