
#include <GL/glew.h>
#include <portaudio.h>

#include "Plugin.h"
#include "Instance.h"

#ifdef WIN32
extern HANDLE			 DLLmodule;
extern char				 DLLname[];
#endif

const unsigned			 PluginBase::mParamCount = NUM_PARAMS;

const char				*PluginBase::mPluginUniqueId	= "BF24";
const char				*PluginBase::mPluginName		= "fugScopeGL";
const char				*PluginBase::mPluginDescription	= "FFGL Scope";
const char				*PluginBase::mPluginAbout		= "(c)2015 Alex May - www.bigfug.com";

const unsigned			 PluginBase::mPluginMajorVersion = 1;
const unsigned			 PluginBase::mPluginMinorVersion = 400;

const unsigned			 PluginBase::mMinimumInputFrames = 0;
const unsigned			 PluginBase::mMaximumInputFrames = 0;

const unsigned			 PluginBase::mPluginType = FF_SOURCE;

const char				*ConfigChoices[] =
{
	"Line",
	"Points",
	"Fill",
	"Points Mirror",
	"Line Mirror",
	"Fill Bottom",
	"Fill Top",
};

const char				*ArrangeChoices[] =
{
	"Normal",
	"Mirror1",
	"Mirror2"
};

const Param				 PluginBase::mParams[] =
{
	Param( PARAM_CONFIG,	"Config",	0, ConfigChoices, EFFECT_COUNT - 1 ),
	Param( PARAM_ARRANGE,	"Arrange",	0, ArrangeChoices, ARRANGE_COUNT - 1 ),
	Param( PARAM_SCALE,		"Scale",	1.0f, 0.0f, 10.0f ),
	Param( PARAM_WIDTH,		"Width",	1.0f, 1.0f, 50.0f ),
};

Plugin::Plugin( void )
	: PluginBase(), mGlewInitialised( false )
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
	pResult.ivalue = FF_FAIL;

	if( Pa_Initialize() != paNoError )
	{
		return;
	}

	pResult.ivalue = FF_SUCCESS;
}

void Plugin::deinitialise( plugMainUnion &pResult )
{
	Pa_Terminate();

	pResult.ivalue = FF_SUCCESS;
}

void Plugin::instantiateGL( plugMainUnion &pResult )
{
	if( !mGlewInitialised )
	{
		if( glewInit() != GLEW_NO_ERROR )
		{
			pResult.ivalue = FF_FAIL;

			return;
		}

		mGlewInitialised = true;
	}

	Instance		*I = new Instance( *this, VERSION_15, pResult );

	if( I != 0 && pResult.ivalue == FF_FAIL )
	{
		delete I;

		return;
	}

#if defined( FF_VERSION_16 )
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
#if defined( FF_VERSION_16 )
	Instance		*I = reinterpret_cast<Instance *>( pResult.PointerValue );
#else
	Instance		*I = reinterpret_cast<Instance *>( pResult.ivalue );
#endif

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
