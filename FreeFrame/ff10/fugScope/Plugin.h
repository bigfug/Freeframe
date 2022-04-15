
#ifndef __FF_FFPLUGIN_H__
#define __FF_FFPLUGIN_H__

#include "FreeFrame.h"
#include "FreeFramePlugin.h"
#include "AudioCapture.h"

enum
{
	FF_PARAM_RED,
	FF_PARAM_GREEN,
	FF_PARAM_BLUE,
	FF_PARAM_POSITION,
	FF_PARAM_SCALE,
	FF_PARAM_LINE,
	FF_PARAM_FILLMODE,
	FF_PARAM_VERTICAL,
	FF_PARAM_SIZE,
	NUM_PARAMS
};

#define NUM_INPUTS 1

#define SKIP_WORDS	(2)

extern	ParamConstantsStruct		paramConstants[];

class ffPlugin : public FreeFramePlugin
{
public:
	AudioCapture			*Audio;
public:
	ffPlugin( ParamConstantsStruct *Parameters, DWORD Count );
	~ffPlugin();

	DWORD					 initialise();
	DWORD					 deInitialise();
	DWORD					 getPluginCaps( DWORD index );
	FreeFrameInstance		*getInstance( VideoInfoStruct *VideoInfo );

	AudioCapture			*getAudioCapture();
};

#endif