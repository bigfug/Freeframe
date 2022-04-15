
#pragma once

#include <PluginBase.h>
#include <fugVidStream.h>

enum
{
	PARAM_PROTOCOL,
	PARAM_PORT,
#if !defined( FF_IS_SOURCE )
	PARAM_RECEIVE,
#endif
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
