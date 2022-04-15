
#include <fstream>

#ifdef WIN32
#include <crtdbg.h>
#endif

#include "Plugin.h"
#include "Instance.h"

#ifdef WIN32
extern HANDLE			 DLLmodule;
extern char				 DLLname[];
#endif

const unsigned			 PluginBase::mParamCount = NUM_PARAMS;

const char				*PluginBase::mPluginUniqueId	= "BF21";
const char				*PluginBase::mPluginName		= "fugWrapperGL";
const char				*PluginBase::mPluginDescription	= "FreeFrame 1.0 wrapper for FFGL";
const char				*PluginBase::mPluginAbout		= "(c)2010 Alex May - www.bigfug.com";

const unsigned			 PluginBase::mPluginMajorVersion = 1;
const unsigned			 PluginBase::mPluginMinorVersion = 000;

const unsigned			 PluginBase::mMinimumInputFrames = 1;
const unsigned			 PluginBase::mMaximumInputFrames = 1;

const unsigned			 PluginBase::mPluginType = FF_EFFECT;

const Param				 PluginBase::mParams[ 1 ];

const char				*Plugin::m24bitPlugins[] =
{
	"PeteColourReduce",
	"PeteColourWarp",
	"PeteHalfTone",
	"PeteLumaCycle",
	"PeteMetaImage",
	"PeteTVPixel",
	"PeteVectorize",
	"PeteWave",
	"resAsciiArt",
	0
};

const char				*Plugin::mForceAlphaPlugins[] =
{
	"resBiHue",
	"resBiHueGradient",
	"resHeat",
	"resHueScale",
	"resHueDuoScale",
	0
};

Plugin::Plugin( void )
: PluginBase(), mLibrary( 0 ), mPlugMain( 0 ), mCap16bit( false ), mCap24bit( false ), mCap32bit( false ), mForceAlpha( false ),
  mCapProcessFrameCopy( false ), mCapMinInputFrames( 1 ), mCapMaxInputFrames( 1 ), mCapCopyOrInPlace( FF_CAP_PREFER_NONE )
{
#ifdef WIN32
	mLibName = DLLname;
#endif

#ifdef NDEBUG
	size_t		Start = 0;

	for( size_t i = 0 ; i < mLibName.size() ; i++ )
	{
		if( mLibName[ i ] == '\\' || mLibName[ i ] == '/' )
		{
			Start = i + 1;
		}
	}

	mLibName = mLibName.substr( Start );
	mLibName = mLibName.substr( 0, mLibName.find_first_of( '.' ) );
#else
	mLibName = "resHsbSines";
#endif

#ifdef WIN32
	const string	LibPath = "ff10\\" + mLibName + ".dll";

	if( ( mLibrary = LoadLibrary( LibPath.c_str() ) ) == 0 )
	{
		ofstream	LogFile( "fugWrapperGL.log", ios::app );

		LogFile << "Couldn't open plugin " << LibPath << std::endl;

		return;
	}

	if( ( mPlugMain = (FF_Main_FuncPtr)GetProcAddress( mLibrary, "plugMain" ) ) == 0L )
	{
		ofstream	LogFile( "fugWrapperGL.log", ios::app );

		LogFile << "Couldn't locate plugMain in " << LibPath << std::endl;

		return;
	}
#endif

	plugMainUnion		Result;

	Result = mPlugMain( FF_GETPLUGINCAPS, FF_CAP_16BITVIDEO, 0 );

	if( Result.ivalue == FF_SUPPORTED )
	{
		mCap16bit = true;
	}

	Result = mPlugMain( FF_GETPLUGINCAPS, FF_CAP_24BITVIDEO, 0 );

	if( Result.ivalue == FF_SUPPORTED )
	{
		mCap24bit = true;
	}

	Result = mPlugMain( FF_GETPLUGINCAPS, FF_CAP_32BITVIDEO, 0 );

	if( Result.ivalue == FF_SUPPORTED )
	{
		mCap32bit = true;
	}

	Result = mPlugMain( FF_GETPLUGINCAPS, FF_CAP_PROCESSFRAMECOPY, 0 );

	if( Result.ivalue == FF_SUPPORTED )
	{
		mCapProcessFrameCopy = true;
	}

	Result = mPlugMain( FF_GETPLUGINCAPS, FF_CAP_COPYORINPLACE, 0 );

	if( Result.ivalue >= FF_CAP_PREFER_NONE && Result.ivalue <= FF_CAP_PREFER_BOTH )
	{
		mCapCopyOrInPlace = Result.ivalue;
	}

	// Check if this plugin is in the 24bit only list
	// Note: we're comparing on the filename, not the plugin name from FF_GETINFO

	if( mCap32bit && mCap24bit )
	{
		for( unsigned i = 0 ; m24bitPlugins[ i ] != 0 ; i++ )
		{
			if( strcmp( mLibName.c_str(), m24bitPlugins[ i ] ) == 0 )
			{
				mCap32bit = false;

				break;
			}
		}
	}

	if( mCap32bit )
	{
		for( unsigned i = 0 ; mForceAlphaPlugins[ i ] != 0 ; i++ )
		{
			if( strcmp( mLibName.c_str(), mForceAlphaPlugins[ i ] ) == 0 )
			{
				mForceAlpha = true;

				break;
			}
		}
	}

	// Get the plugin's info

	Result = mPlugMain( FF_GETINFO, 0, 0 );

	if( Result.PISvalue == 0 || Result.ivalue == FF_FAIL || Result.ivalue == FF_UNSUPPORTED )
	{
		ofstream	LogFile( "fugWrapperGL.log", ios::app );

		LogFile << "Error on FF_GETINFO in " << LibPath << std::endl;
	}
	else
	{
		memcpy( &mPluginInfo, Result.PISvalue, sizeof( PluginInfoStruct ) );

		mPluginInfo.APIMajorVersion = 1;
		mPluginInfo.APIMinorVersion = 500;
	}

	// Get the plugin's extended info

	Result = mPlugMain( FF_GETEXTENDEDINFO, 0, 0 );

	if( Result.ivalue != 0 && Result.ivalue != FF_FAIL && Result.ivalue != FF_UNSUPPORTED )
	{
		memcpy( &mPluginExtendedInfo, Result.svalue, sizeof( PluginExtendedInfoStruct ) );
	}

	if( mPluginInfo.PluginType == FF_SOURCE )
	{
		mCapMinInputFrames = 0;
		mCapMaxInputFrames = 0;
	}
	else
	{
		Result = mPlugMain( FF_GETPLUGINCAPS, FF_CAP_MINIMUMINPUTFRAMES, 0 );

		if( Result.ivalue != FF_UNSUPPORTED && Result.ivalue != FF_FAIL )
		{
			mCapMinInputFrames = max( 1, Result.ivalue );
		}

		Result = mPlugMain( FF_GETPLUGINCAPS, FF_CAP_MAXIMUMINPUTFRAMES, 0 );

		if( Result.ivalue != FF_UNSUPPORTED && Result.ivalue != FF_FAIL )
		{
			mCapMaxInputFrames = max( 1, Result.ivalue );
		}
	}
}

Plugin::~Plugin( void )
{
	if( !mInstances.empty() )
	{
		ofstream	LogFile( "fugWrapperGL.log", ios::app );

		for( list<Instance *>::iterator it = mInstances.begin() ; it != mInstances.end() ; it++ )
		{
			LogFile << mLibName << " didn't free an instance" << std::endl;

			delete *it;
		}
	}

#ifdef WIN32
	if( mLibrary != 0 )
	{
		FreeLibrary( mLibrary );

		mLibrary = 0;

		mPlugMain = 0;
	}

#ifndef NDEBUG
	_CrtDumpMemoryLeaks();
#endif

#endif
}

void Plugin::getPluginCaps( plugMainUnion &pResult )
{
	if( mPlugMain == 0 )
	{
		pResult.ivalue = FF_UNSUPPORTED;

		return;
	}

	switch( pResult.ivalue )
	{
		case FF_CAP_16BITVIDEO:
		case FF_CAP_24BITVIDEO:
		case FF_CAP_32BITVIDEO:
		case FF_CAP_PROCESSFRAMECOPY:
		case FF_CAP_COPYORINPLACE:
			pResult.ivalue = FF_UNSUPPORTED;
			break;

		case FF_CAP_PROCESSOPENGL:
			pResult.ivalue = FF_SUPPORTED;
			break;

		case FF_CAP_SETTIME:
			pResult.ivalue = FF_SUPPORTED;
			break;

		default:
			pResult = mPlugMain( FF_GETPLUGINCAPS, pResult.ivalue, 0 );
			break;
	}
}

void Plugin::initialise( plugMainUnion &pResult )
{
	if( mPlugMain != 0 )
	{
		pResult = mPlugMain( FF_INITIALISE, pResult.ivalue, 0 );
	}
	else
	{
		pResult.ivalue = FF_FAIL;
	}
}

void Plugin::deinitialise( plugMainUnion &pResult )
{
	if( mPlugMain != 0 )
	{
		pResult = mPlugMain( FF_DEINITIALISE, pResult.ivalue, 0 );
	}
	else
	{
		pResult.ivalue = FF_FAIL;
	}
}

void Plugin::instantiate( plugMainUnion &pResult )
{
	Instance		*I = new Instance( *this, pResult );

	if( I != 0 && pResult.ivalue == FF_SUCCESS )
	{
		mInstances.push_back( I );

		pResult.ivalue = reinterpret_cast<DWORD>( I );
	}
	else
	{
		if( I != 0 )
		{
			delete I;
		}

		pResult.ivalue = FF_FAIL;
	}
}

void Plugin::deInstantiate( plugMainUnion &pResult )
{
	Instance		*I = reinterpret_cast<Instance *>( pResult.ivalue );

	if( I != 0 )
	{
		const size_t	OldSze = mInstances.size();

		mInstances.remove( I );

		if( mInstances.size() == OldSze )
		{
			ofstream	LogFile( "fugWrapperGL.log", ios::app );

			LogFile << mLibName << " tried to delete an invalid instance" << std::endl;

			pResult.ivalue = FF_FAIL;
		}
		else
		{
			delete I;

			pResult.ivalue = FF_SUCCESS;
		}
	}
	else
	{
		pResult.ivalue = FF_FAIL;
	}
}

void Plugin::getNumParameters( plugMainUnion &pResult )
{
	if( mPlugMain != 0 )
	{
		pResult = mPlugMain( FF_GETNUMPARAMETERS, pResult.ivalue, 0 );
	}
}

void Plugin::getParameterName( plugMainUnion &pResult )
{
	if( mPlugMain != 0 )
	{
		pResult = mPlugMain( FF_GETPARAMETERNAME, pResult.ivalue, 0 );
	}
}

void Plugin::getParameterDefault( plugMainUnion &pResult )
{
	if( mPlugMain != 0 )
	{
		pResult = mPlugMain( FF_GETPARAMETERDEFAULT, pResult.ivalue, 0 );
	}
}

void Plugin::getParameterType( plugMainUnion &pResult )
{
	if( mPlugMain != 0 )
	{
		pResult = mPlugMain( FF_GETPARAMETERTYPE, pResult.ivalue, 0 );

		// Some plugins return FF_FAIL for parameter type: make sure we're
		// returning a valid value

		switch( pResult.ivalue )
		{
			case FF_TYPE_BOOLEAN:
			case FF_TYPE_EVENT:
			case FF_TYPE_RED:
			case FF_TYPE_GREEN:
			case FF_TYPE_BLUE:
			case FF_TYPE_XPOS:
			case FF_TYPE_YPOS:
			case FF_TYPE_STANDARD:
			case FF_TYPE_ALPHA:
			case FF_TYPE_TEXT:
				break;

			default:
				pResult.ivalue = FF_TYPE_STANDARD;
				break;
		}
	}
}
