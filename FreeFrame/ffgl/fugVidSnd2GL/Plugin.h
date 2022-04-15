
#pragma once

#include <PluginBase.h>

enum
{
	PARAM_CONFIG,
	PARAM_SEND,
	PARAM_FPS,
	PARAM_FLIPY,
	NUM_PARAMS
};

class Plugin : public PluginBase
{
public:
	Plugin( void );

	virtual ~Plugin( void );

	virtual void initialise( plugMainUnion &pResult );
	virtual void deinitialise( plugMainUnion &pResult );

	virtual void instantiateGL( plugMainUnion &pResult );
	virtual void deInstantiateGL( plugMainUnion &pResult );

private:
	bool		mGlewInitialised;
};
