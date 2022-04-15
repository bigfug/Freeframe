#ifndef PLUGIN_H
#define PLUGIN_H

#include <PluginBase.h>

#include <iostream>

enum
{
	PARAM_GLITCH,
	PARAM_QUALITY,
	PARAM_SEED,
	PARAM_AMOUNT1,
	PARAM_SEARCH,
	PARAM_REPLACE,
	PARAM_AMOUNT2,
	NUM_PARAMS
};

class Plugin : public PluginBase
{
public:
	static void checkError( void );

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

#endif // PLUGIN_H
