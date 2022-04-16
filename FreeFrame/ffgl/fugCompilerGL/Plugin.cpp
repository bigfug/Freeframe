
#include <GL/glew.h>

#include "Plugin.h"
#include "Instance.h"

#ifdef WIN32
extern HANDLE			 DLLmodule;
extern char				 DLLname[];
#endif

const unsigned			 PluginBase::mParamCount = NUM_PARAMS;

const char				*PluginBase::mPluginUniqueId	= "BF22";
const char				*PluginBase::mPluginName		= "fugCompilerGL";
const char				*PluginBase::mPluginDescription	= "An OpenGL Shader Compiler";
const char				*PluginBase::mPluginAbout		= "(c)2010 Alex May - www.bigfug.com";

const unsigned			 PluginBase::mPluginMajorVersion = 1;
const unsigned			 PluginBase::mPluginMinorVersion = 000;

const unsigned			 PluginBase::mMinimumInputFrames = 1;
const unsigned			 PluginBase::mMaximumInputFrames = 1;

const unsigned			 PluginBase::mPluginType = FF_EFFECT;

const Param				 PluginBase::mParams[ 1 ];

Plugin::Plugin( void )
: PluginBase()
{
	string		LibName;

#ifdef WIN32
	LibName = DLLname;
#endif

#if 1
	LibName = LibName.substr( LibName.find_last_of( '\\' ) + 1 );
	LibName = LibName.substr( 0, LibName.find_first_of( '.' ) );
#else
	LibName = "FFGLTest";
#endif

#ifdef WIN32
	LibName = ".\\shaders\\" + LibName + ".ini";

	fs::path		IniPath( LibName );

	po::options_description		Config( "Configuration" );

	Config.add_options()
		( "plugin.id",				po::value<string>( &mCfgPluginId ),	"unique plugin identifier" )
		( "plugin.name",			po::value<string>( &mCfgPluginName ),	"plugin name" )
		( "plugin.description",		po::value<string>( &mCfgPluginDescription ),	"plugin name" )
		( "plugin.about",			po::value<string>( &mCfgPluginAbout ),	"plugin name" )
		( "plugin.major_version",	po::value<int>( &mCfgPluginMajorVersion ),	"plugin name" )
		( "plugin.minor_version",	po::value<int>( &mCfgPluginMinorVersion ),	"plugin name" )
		( "plugin.min_frames",		po::value<int>( &mCfgPluginMinFrames ),	"plugin name" )
		( "plugin.max_frames",		po::value<int>( &mCfgPluginMaxFrames ),	"plugin name" )
		( "shader.geometry",		po::value< vector<string> >( &mCfgGeometryShaderFileNames ),		"geometry shaders" )
		( "shader.vertex",			po::value< vector<string> >( &mCfgVertexShaderFileNames ),		"vertex shaders" )
		( "shader.fragment",		po::value< vector<string> >( &mCfgFragmentShaderFileNames ),		"fragment shaders" )
	;

	po::variables_map			VarMap;

	ifstream	ConfigFile( LibName.c_str() );

	if( !ConfigFile.is_open() )
	{
		MessageBox( 0, LibName.c_str(), "Can't open ini file", MB_OK );

		return;
	}

	po::store( po::parse_config_file( ConfigFile, Config ), VarMap );

	po::notify( VarMap );

	memcpy( &mPluginInfo.PluginUniqueID, mCfgPluginId.c_str(), 4 );

	const size_t	CopyCount = min( sizeof( mPluginInfo.PluginName ), mCfgPluginName.size() );

	memset( &mPluginInfo.PluginName, 0, sizeof( mPluginInfo.PluginName ) );
	memcpy( &mPluginInfo.PluginName, mCfgPluginName.c_str(), CopyCount );

	mPluginExtendedInfo.PluginMajorVersion = mCfgPluginMajorVersion;
	mPluginExtendedInfo.PluginMinorVersion = mCfgPluginMinorVersion;

	mDescriptionBuffer.resize( mCfgPluginDescription.size() + 1, 0 );
	mAboutBuffer.resize( mCfgPluginAbout.size() + 1, 0 );

	strcpy( &mDescriptionBuffer[ 0 ], mCfgPluginDescription.c_str() );
	strcpy( &mAboutBuffer[ 0 ], mCfgPluginAbout.c_str() );

	mPluginExtendedInfo.Description        = &mDescriptionBuffer[ 0 ];
	mPluginExtendedInfo.About              = &mAboutBuffer[ 0 ];
#endif
}


Plugin::~Plugin( void )
{
}

void Plugin::checkError( void )
{
	GLenum		Error;

	while( ( Error = glGetError() ) != 0 )
	{
		MessageBox( 0, (LPCSTR)gluErrorString( Error ), "OpenGL Error", MB_OK );
	}
}

void Plugin::initialise( plugMainUnion &pResult )
{
	if( glewInit() != GLEW_OK )
	{
		pResult.ivalue = FF_FAIL;

		return;
	}

	pResult.ivalue = FF_SUCCESS;
}

void Plugin::deinitialise( plugMainUnion &pResult )
{
	pResult.ivalue = FF_SUCCESS;
}

void Plugin::instantiate( plugMainUnion &pResult )
{
	Instance		*I = new Instance( *this, pResult );

	if( I != 0 && pResult.ivalue == FF_FAIL )
	{
		delete I;

		return;
	}

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
