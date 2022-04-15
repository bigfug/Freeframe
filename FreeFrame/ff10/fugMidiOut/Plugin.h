
//////////////////////////////////////////////////////////////////////////////////
//
// includes
//

#include "FreeFrame.h"
#include "FreeFramePlugin.h"
#include "Instance.h"

#define		NUM_INPUTS		1

static	ParamConstantsStruct	  paramConstants[] = 
{
	{ "Midi Channel",	FF_TYPE_STANDARD,	0.0f, 1.0f, 16.0f },
	{ "Control 01",		FF_TYPE_STANDARD,	0.0f, 0.0f, 255.0f },
	{ "Control 02",		FF_TYPE_STANDARD,	0.0f, 0.0f, 255.0f },
	{ "Control 03",		FF_TYPE_STANDARD,	0.0f, 0.0f, 255.0f },
	{ "Control 04",		FF_TYPE_STANDARD,	0.0f, 0.0f, 255.0f },
	{ "Control 05",		FF_TYPE_STANDARD,	0.0f, 0.0f, 255.0f },
	{ "Control 06",		FF_TYPE_STANDARD,	0.0f, 0.0f, 255.0f },
	{ "Control 07",		FF_TYPE_STANDARD,	0.0f, 0.0f, 255.0f },
	{ "Control 08",		FF_TYPE_STANDARD,	0.0f, 0.0f, 255.0f },
	{ "Trigger 01",		FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Trigger 02",		FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Trigger 03",		FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Trigger 04",		FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Trigger 05",		FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Trigger 06",		FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Trigger 07",		FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Trigger 08",		FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
};

enum
{
	FF_PARAM_CHANNEL,
	FF_PARAM_CTRL_01,
	FF_PARAM_CTRL_02,
	FF_PARAM_CTRL_03,
	FF_PARAM_CTRL_04,
	FF_PARAM_CTRL_05,
	FF_PARAM_CTRL_06,
	FF_PARAM_CTRL_07,
	FF_PARAM_CTRL_08,
	FF_PARAM_TRIG_01,
	FF_PARAM_TRIG_02,
	FF_PARAM_TRIG_03,
	FF_PARAM_TRIG_04,
	FF_PARAM_TRIG_05,
	FF_PARAM_TRIG_06,
	FF_PARAM_TRIG_07,
	FF_PARAM_TRIG_08,
	NUM_PARAMS
};

typedef struct StreamConfigStruct
{
	struct StreamConfigStruct		*mNext;
	unsigned int  mNumber;
	unsigned char mAddress[ 4 ];
	unsigned short mPort;
} StreamConfig;

extern		StreamConfig			 gConfig;

class ffPlugin : public FreeFramePlugin
{
private:
public:
	ffPlugin( ParamConstantsStruct *Parameters, DWORD Count );
	~ffPlugin();

	DWORD					 getPluginCaps( DWORD index );
	FreeFrameInstance		*getInstance( VideoInfoStruct *VideoInfo );
};

