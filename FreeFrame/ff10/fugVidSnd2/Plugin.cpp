
#include "Plugin.h"
#include "Instance.h"

const unsigned			 PluginBase::mParamCount = NUM_PARAMS;

const char				*PluginBase::mPluginUniqueId	= "BF14";
const char				*PluginBase::mPluginName		= "fugVidSnd2";
const char				*PluginBase::mPluginDescription	= "A video streaming sender";
const char				*PluginBase::mPluginAbout		= "(c)2015 Alex May - www.bigfug.com";

const unsigned			 PluginBase::mPluginMajorVersion = 2;
const unsigned			 PluginBase::mPluginMinorVersion = 100;

const unsigned			 PluginBase::mMinimumInputFrames = 1;
const unsigned			 PluginBase::mMaximumInputFrames = 1;

const unsigned			 PluginBase::m16bit = FF_UNSUPPORTED;
const unsigned			 PluginBase::m24bit = FF_SUPPORTED;
const unsigned			 PluginBase::m32bit = FF_SUPPORTED;

const unsigned			 PluginBase::mProcessFrameCopy = FF_UNSUPPORTED;
const unsigned			 PluginBase::mCopyOrInPlace = FF_CAP_PREFER_INPLACE;
const unsigned			 PluginBase::mPluginType = FF_EFFECT;

const Param				 PluginBase::mParams[] =
{
	Param( PARAM_CONFIG, "Config", 0, 0, 10 ),
	Param::onoff( PARAM_SEND, "Send", false ),
	Param( PARAM_FPS, "FPS", 25, 0, 120 )
};

Plugin::Plugin( void )
: PluginBase()
{
}

Plugin::~Plugin( void )
{
}

void Plugin::instantiate( plugMainUnion &pResult )
{
	Instance		*I = new Instance( *this, VERSION_10, pResult );

	pResult.ivalue = ( I == 0 ? FF_FAIL : reinterpret_cast<DWORD>( I ) );
}

void Plugin::deInstantiate( plugMainUnion &pResult )
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
