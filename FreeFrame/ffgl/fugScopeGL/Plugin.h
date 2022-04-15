
#pragma once

#include <PluginBase.h>

#include <iostream>

enum
{
	PARAM_CONFIG,
	PARAM_ARRANGE,
	PARAM_SCALE,
	PARAM_WIDTH,
	NUM_PARAMS
};

enum
{
	EFFECT_LINE_1,
	EFFECT_POINTS_1,
	EFFECT_FILL_1,
	EFFECT_POINTS2_1,
	EFFECT_LINE2_1,
	EFFECT_FILL_BOTTOM_1,
	EFFECT_FILL_TOP_1,
	EFFECT_COUNT
};

enum
{
	ARRANGE_LEFT,
	ARRANGE_MIRROR_LEFT,
	ARRANGE_MIRROR_REVERSE_LEFT,
	ARRANGE_COUNT
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
