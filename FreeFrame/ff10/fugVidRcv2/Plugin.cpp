
#include "Plugin.h"
#include "Instance.h"

const unsigned			 PluginBase::mParamCount = NUM_PARAMS;

#ifndef FF_IS_SOURCE
const char				*PluginBase::mPluginUniqueId	= "BF12";
const char				*PluginBase::mPluginName		= "fugVidRcv2";

const unsigned			 PluginBase::mMinimumInputFrames = 1;
const unsigned			 PluginBase::mMaximumInputFrames = 1;

const unsigned			 PluginBase::mCopyOrInPlace = FF_CAP_PREFER_INPLACE;
const unsigned			 PluginBase::mPluginType = FF_EFFECT;
#else
const char				*PluginBase::mPluginUniqueId	= "BF13";
const char				*PluginBase::mPluginName		= "fugVidRcv2";

const unsigned			 PluginBase::mMinimumInputFrames = 0;
const unsigned			 PluginBase::mMaximumInputFrames = 0;

const unsigned			 PluginBase::mCopyOrInPlace = FF_UNSUPPORTED;
const unsigned			 PluginBase::mPluginType = FF_SOURCE;
#endif

const char				*PluginBase::mPluginDescription	= "A video streaming receiver";
const char				*PluginBase::mPluginAbout		= "(c)2015 Alex May - www.bigfug.com";

const unsigned			 PluginBase::mPluginMajorVersion = 2;
const unsigned			 PluginBase::mPluginMinorVersion = 100;

const unsigned			 PluginBase::m16bit = FF_UNSUPPORTED;
const unsigned			 PluginBase::m24bit = FF_SUPPORTED;
const unsigned			 PluginBase::m32bit = FF_SUPPORTED;

const unsigned			 PluginBase::mProcessFrameCopy = FF_UNSUPPORTED;

const char				*ProtocolNames[] =
{
	"UDP",
	"TCP",
	"MEMORY"
};

const Param				 PluginBase::mParams[] =
{
	Param( PARAM_PROTOCOL, "Protocol", 2, ProtocolNames, 3 ),
	Param( PARAM_PORT, "Channel", 0, 0, 10 ),
	#if !defined( FF_IS_SOURCE )
		,
		Param::onoff( PARAM_RECEIVE, "Receive",	false )
	#endif
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
