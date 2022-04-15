
#pragma once

#include <PluginBase.h>
#include <fugVidStream.h>

enum
{
	PARAM_CONFIG,
	PARAM_SEND,
	PARAM_FPS,
	NUM_PARAMS
};

class Plugin : public PluginBase
{
public:
	Plugin( void );

	virtual ~Plugin( void );

	virtual void instantiate( plugMainUnion &pResult );
	virtual void deInstantiate( plugMainUnion &pResult );
};
