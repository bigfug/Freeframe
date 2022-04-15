
#include <GL/glew.h>

#include "Plugin.h"
#include "Instance.h"

#ifdef WIN32
extern HANDLE			 DLLmodule;
extern char				 DLLname[];
#endif

const unsigned			 PluginBase::mParamCount = NUM_PARAMS;

const char				*PluginBase::mPluginUniqueId	= "BF26";
const char				*PluginBase::mPluginName		= "fugGlitchGL";
const char				*PluginBase::mPluginDescription	= "FFGL Glitcher";
const char				*PluginBase::mPluginAbout		= "(c)2015 Alex May - www.bigfug.com";

const unsigned			 PluginBase::mPluginMajorVersion = 1;
const unsigned			 PluginBase::mPluginMinorVersion = 100;

const unsigned			 PluginBase::mMinimumInputFrames = 1;
const unsigned			 PluginBase::mMaximumInputFrames = 1;

const unsigned			 PluginBase::mPluginType = FF_EFFECT;


const Param				 PluginBase::mParams[] =
{
	Param::onoff( PARAM_GLITCH, "Glitch", true ),
	Param( PARAM_QUALITY, "Quality", 100, 0, 100 ),
	Param( PARAM_SEED, "Seed", 0.0f ),
	Param( PARAM_AMOUNT1, "Amount1", 1, 0, 20 ),
	Param( PARAM_SEARCH, "Search", 0, 0, 255 ),
	Param( PARAM_REPLACE, "Replace", 0, 0, 255 ),
	Param( PARAM_AMOUNT2, "Amount2", 1.0f ),
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
		//glewExperimental = GL_TRUE;

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

	if( I )
	{
		delete I;

		pResult.ivalue = FF_SUCCESS;
	}
	else
	{
		pResult.ivalue = FF_FAIL;
	}
}
