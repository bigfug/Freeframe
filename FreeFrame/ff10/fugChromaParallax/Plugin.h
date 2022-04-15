
#ifndef __FF_FFPLUGIN_H__
#define __FF_FFPLUGIN_H__

#include "FreeFrame.h"
#include "FreeFramePlugin.h"

#define		NUM_INPUTS		2

static	ParamConstantsStruct	  paramConstants[] = 
{
	{ "Use Mask Alpha",			FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Use Whole Frame",		FF_TYPE_BOOLEAN,	1.0f, 0.0f, 1.0f },
	{ "Draw Zero Luma",			FF_TYPE_BOOLEAN,	1.0f, 0.0f, 1.0f },
	{ "Luma Start",				FF_TYPE_STANDARD,	0.0f, 0.0f, 255.0f },
	{ "Luma End",				FF_TYPE_STANDARD,	255.0f, 0.0f, 255.0f },
};

enum
{
	FF_USE_MASK_ALPHA,
	FF_USE_WHOLE_FRAME,
	FF_DRAW_ZERO_LUMA,
	FF_LUMA_START,
	FF_LUMA_END,
	NUM_PARAMS
};

extern	ParamConstantsStruct		paramConstants[];

class ffPlugin : public FreeFramePlugin
{
private:
public:
	ffPlugin( ParamConstantsStruct *Parameters, DWORD Count );

	DWORD					 getPluginCaps( DWORD index );
	FreeFrameInstance		*getInstance( VideoInfoStruct *VideoInfo );
};

#endif