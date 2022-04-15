
#include <FFGL.h>

#include "Plugin.h"
#include "Instance.h"

#if defined( WIN32 )
extern HANDLE			 DLLmodule;
extern char				 DLLname[];
#endif

const unsigned			 PluginBase::mParamCount = NUM_PARAMS;

const char				*PluginBase::mPluginUniqueId	= "BF41";
const char				*PluginBase::mPluginName		= "fugFeedbackGL";
const char				*PluginBase::mPluginDescription	= "FFGL Video Feedback";
const char				*PluginBase::mPluginAbout		= "(c)2015 Alex May - www.bigfug.com";

const unsigned			 PluginBase::mPluginMajorVersion = 1;
const unsigned			 PluginBase::mPluginMinorVersion = 200;

const unsigned			 PluginBase::mMinimumInputFrames = 1;
const unsigned			 PluginBase::mMaximumInputFrames = 1;

const unsigned			 PluginBase::mPluginType = FF_EFFECT;

const char				*ModeNames[] =
{
	"Alpha",
	"Luma",
	"Lightness"
};

const Param				 PluginBase::mParams[] =
{
	Param( PARAM_MODE,				"Mode",				0, ModeNames, MODE_COUNT - 1 ),
	Param( PARAM_LUMA_MIN,			"Min Value",		0.0f, 0.0f, 1.0f ),
	Param( PARAM_LUMA_MAX,			"Max Value",		1.0f, 0.0f, 1.0f ),
	Param( PARAM_ROTATION,			"Rotation",			0.0f, -90.0f, +90.0f ),
	Param( PARAM_ZOOM,				"Zoom",				1.0f, 0.0f, +2.0f ),
	Param( PARAM_DIRECTION,			"Move Direction",	0.0f, 0.0f, 360.0f ),
	Param( PARAM_AMOUNT,			"Move Amount",		0.0f, 0.0f, 1.0f ),
	Param( PARAM_FADE,				"Fade",				0.0f, 0.0f, 1.0f ),
	Param( PARAM_HUE_ROTATE,		"Hue Rotate",		0.0f, -0.02f, 0.02f ),
};

Plugin::Plugin( void )
	: PluginBase(), mInitialisedGL( false )
{
}

Plugin::~Plugin( void )
{
}

void Plugin::checkError( void )
{
	GLenum		Error;

	while( ( Error = glGetError() ) != 0 )
	{
		//MessageBox( 0, (LPCSTR)gluErrorString( Error ), "OpenGL Error", MB_OK );
	}
}

void Plugin::initialise( plugMainUnion &pResult )
{
#if !defined( FF_VERSION_15 )
	pResult.UIntValue = FF_SUCCESS;
#else
	pResult.ivalue = FF_SUCCESS;
#endif
}

void Plugin::deinitialise( plugMainUnion &pResult )
{
#if !defined( FF_VERSION_15 )
	pResult.UIntValue = FF_SUCCESS;
#else
	pResult.ivalue = FF_SUCCESS;
#endif
}

void Plugin::instantiateGL( plugMainUnion &pResult )
{
	if( !mInitialisedGL )
	{
		if( !gladLoadGL() )
		{
#if !defined( FF_VERSION_15 )
			pResult.UIntValue = FF_FAIL;
#else
			pResult.ivalue = FF_FAIL;
#endif
			return;
		}

		mInitialisedGL = true;
	}

	Instance		*I = new Instance( *this, VERSION_15, pResult );

#if !defined( FF_VERSION_15 )
	if( I && pResult.UIntValue == FF_FAIL )
#else
	if( I && pResult.ivalue == FF_FAIL )
#endif
	{
		delete I;

		return;
	}

#if !defined( FF_VERSION_15 )
	if( !I )
	{
		pResult.UIntValue = FF_FAIL;
	}
	else
	{
		pResult.PointerValue = I;
	}
#else
	pResult.ivalue = ( !I ? FF_FAIL : reinterpret_cast<DWORD>( I ) );
#endif
}

void Plugin::deInstantiateGL( plugMainUnion &pResult )
{
#if !defined( FF_VERSION_15 )
	Instance		*I = reinterpret_cast<Instance *>( pResult.PointerValue );
#else
	Instance		*I = reinterpret_cast<Instance *>( pResult.ivalue );
#endif

	if( I )
	{
		delete I;

#if !defined( FF_VERSION_15 )
		pResult.UIntValue = FF_SUCCESS;
#else
		pResult.ivalue = FF_SUCCESS;
#endif
	}
	else
	{
#if !defined( FF_VERSION_15 )
		pResult.UIntValue = FF_FAIL;
#else
		pResult.ivalue = FF_FAIL;
#endif
	}
}
