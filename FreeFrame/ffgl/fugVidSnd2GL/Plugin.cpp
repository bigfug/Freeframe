
#include <gl/glew.h>

#include "Plugin.h"
#include "Instance.h"


const unsigned			 PluginBase::mParamCount = NUM_PARAMS;

const char				*PluginBase::mPluginUniqueId	= "BF19";
const char				*PluginBase::mPluginName		= "fugVidSnd2GL";
const char				*PluginBase::mPluginDescription	= "A video streaming sender";
const char				*PluginBase::mPluginAbout		= "(c)20015 Alex May - www.bigfug.com";

const unsigned			 PluginBase::mPluginMajorVersion = 2;
const unsigned			 PluginBase::mPluginMinorVersion = 100;

const unsigned			 PluginBase::mMinimumInputFrames = 1;
const unsigned			 PluginBase::mMaximumInputFrames = 1;

const unsigned			 PluginBase::mPluginType = FF_EFFECT;

const Param				 PluginBase::mParams[] =
{
	Param( PARAM_CONFIG, "Config", 0, 0, 10 ),
	Param::onoff( PARAM_SEND, "Send", false ),
	Param( PARAM_FPS, "FPS", 25, 0, 120 ),
	Param::onoff( PARAM_FLIPY, "FlipY", false )
};

Plugin::Plugin( void )
: PluginBase(), mGlewInitialised( false )
{
}

Plugin::~Plugin( void )
{
}

void Plugin::initialise( plugMainUnion &pResult )
{
	pResult.ivalue = FF_SUCCESS;
}

void Plugin::deinitialise( plugMainUnion &pResult )
{
	pResult.ivalue = FF_SUCCESS;
}

void Plugin::instantiateGL( plugMainUnion &pResult )
{
	if( !mGlewInitialised )
	{
		glewExperimental = GL_TRUE;

		if( glewInit() != GLEW_NO_ERROR )
		{
			pResult.ivalue = FF_FAIL;

			return;
		}

		mGlewInitialised = true;
	}

	Instance		*I = new Instance( *this, VERSION_15, pResult );

	pResult.ivalue = ( I == 0 ? FF_FAIL : reinterpret_cast<DWORD>( I ) );
}

void Plugin::deInstantiateGL( plugMainUnion &pResult )
{
	Instance		*I = reinterpret_cast<Instance *>( pResult.ivalue );

	if( I != 0 )
	{
		delete I;

		pResult.ivalue = FF_SUCCESS;
	}
	else
	{
		pResult.ivalue = FF_FAIL;
	}
}
