#ifndef PLUGIN_H
#define PLUGIN_H

#include <PluginBase.h>

#include <iostream>

#define	AUDIO_BUFFER_SIZE		(512)
#define AUDIO_SAMPLE_RATE		(11025*2)

enum
{
	PARAM_CONFIG,
	PARAM_ARRANGE,
	PARAM_POSITION,
	PARAM_SCALE,
	PARAM_WIDTH,
	PARAM_BANDS,
	PARAM_BAND_WIDTH,
	PARAM_LINES,
	PARAM_ATTACK,
	PARAM_DECAY,
	NUM_PARAMS
};

enum
{
	EFFECT_LINE_1,
	EFFECT_POINTS_1,
	EFFECT_FILL_1,
	EFFECT_POINTS2_1,
	EFFECT_LINE2_1,
	EFFECT_BANDS,
	EFFECT_COUNT
};

enum
{
	ARRANGE_LEFT,
	ARRANGE_MIRROR_LEFT,
	ARRANGE_MIRROR_REVERSE_LEFT,
	ARRANGE_COUNT
};

enum
{
	BANDS_8,
	BANDS_16,
	BANDS_32,
	BANDS_64,
	BANDS_128,
	BANDS_256,
	BANDS_COUNT
};

enum
{
	POSITION_BOTTOM,
	POSITION_TOP,
	POSITION_CENTER1,
	POSITION_CENTER2,
	POSITION_COUNT
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
