
//////////////////////////////////////////////////////////////////////////////////
//
// includes
//

#include "FreeFrame.h"
#include "FreeFramePlugin.h"
#include "Instance.h"
#include "StreamEncoder.h"
#include <map>

#define		NUM_INPUTS		1

static ParamConstantsStruct	  paramConstants[] = 
{
	{ "Send Data",		FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
	{ "Config",			FF_TYPE_STANDARD,	0.0f, 0.0f, 0.0f },
	{ "Frame Skip",		FF_TYPE_STANDARD,	0.0f, 0.0f, 30.0f },
	{ "Vert. Flip",		FF_TYPE_BOOLEAN,	0.0f, 0.0f, 1.0f },
};

enum
{
	FF_PARAM_SEND,
	FF_PARAM_CONFIG,
	FF_PARAM_SKIP,
	FF_PARAM_VFLIP,
	NUM_PARAMS
};

typedef struct StreamConfigStruct
{
	struct StreamConfigStruct		*mNext;
	unsigned int					 mNumber;
	unsigned char					 mAddress[ 4 ];
	unsigned short					 mPort;
	StreamProtocol					 mProtocol;
} StreamConfig;

extern		StreamConfig			 gConfig;
extern		DWORD					 gWidth, gHeight, gDepth;

class ffPlugin : public FreeFramePlugin
{
private:
public:
	ffPlugin( ParamConstantsStruct *Parameters, DWORD Count );
	~ffPlugin();

	DWORD					 getPluginCaps( DWORD index );
	FreeFrameInstance		*getInstance( VideoInfoStruct *VideoInfo );
};

