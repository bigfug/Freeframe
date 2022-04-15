#ifndef PLUGIN_H
#define PLUGIN_H

#include <PluginBase.h>

#include <iostream>

#define	AUDIO_BUFFER_SIZE		(512)
#define AUDIO_SAMPLE_RATE		(11025*2)

enum
{
	PARAM_MODE,
	PARAM_LUMA_MIN,
	PARAM_LUMA_MAX,
	PARAM_ROTATION,
	PARAM_ZOOM,
	PARAM_DIRECTION,
	PARAM_AMOUNT,
	PARAM_FADE,
	PARAM_HUE_ROTATE,
	NUM_PARAMS
};

enum
{
	MODE_ALPHA,
	MODE_LUMA,
	MODE_LIGHTNESS,
	MODE_COUNT
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
	bool		mInitialisedGL;
};

#endif // PLUGIN_H
