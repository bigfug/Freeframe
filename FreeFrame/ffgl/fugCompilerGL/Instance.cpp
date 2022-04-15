
#include <gl/glew.h>

#include "Instance.h"

Instance::Instance( Plugin &pPlugin, plugMainUnion &pResult )
: InstanceBase( pPlugin, pResult ), mPlugin( pPlugin ), mProgram( 0 )
{
	int				Linked;

	Plugin::checkError();

	if( !loadShaders( mPlugin.mCfgGeometryShaderFileNames, GL_GEOMETRY_SHADER ) )
	{
		pResult.ivalue = FF_FAIL;

		return;
	}

	if( !loadShaders( mPlugin.mCfgVertexShaderFileNames, GL_VERTEX_SHADER ) )
	{
		pResult.ivalue = FF_FAIL;

		return;
	}

	if( !loadShaders( mPlugin.mCfgFragmentShaderFileNames, GL_FRAGMENT_SHADER ) )
	{
		pResult.ivalue = FF_FAIL;

		return;
	}

	if( mShaders.size() == 0 )
	{
		pResult.ivalue = FF_FAIL;

		return;
	}

	if( ( mProgram = glCreateProgram() ) == 0 )
	{
		pResult.ivalue = FF_FAIL;

		return;
	}

	for( vector<GLuint>::iterator it = mShaders.begin() ; it != mShaders.end() ; it++ )
	{
		glAttachShader( mProgram, *it );
	}

	glLinkProgram( mProgram );

	glGetProgramiv( mProgram, GL_LINK_STATUS, &Linked );

	if( Linked != GL_TRUE )
	{
		char		Log[ 1024 ];
		int			Len;

		glGetProgramInfoLog( mProgram, sizeof( Log ), &Len, Log );

		MessageBox( 0, Log, "Linker Error", MB_OK );

		pResult.ivalue = FF_FAIL;

		return;
	}

	Plugin::checkError();
}

Instance::~Instance( void )
{
	if( mProgram != 0 )
	{
		for( vector<GLuint>::iterator it = mShaders.begin() ; it != mShaders.end() ; it++ )
		{
			glDetachShader( mProgram, *it );
		}

		glDeleteProgram( mProgram );

		mProgram = 0;
	}

	for( vector<GLuint>::iterator it = mShaders.begin() ; it != mShaders.end() ; it++ )
	{
		glDeleteShader( *it );
	}

	mShaders.clear();
}

bool Instance::loadShaders( const vector<string> &pFileNames, const GLuint pType )
{
	int		Compiled;

	for( vector<string>::const_iterator it = pFileNames.begin() ; it != pFileNames.end() ; it++ )
	{
		ifstream	Input( (*it).c_str() );

		if( !Input.is_open() )
		{
			MessageBox( 0, (*it).c_str(), "Can't open shader file", MB_OK );

			return( false );
		}

		GLuint		Shader = glCreateShader( pType );

		if( Shader == 0 )
		{
			return( false );
		}

		string		InputData( ( istreambuf_iterator<char>( Input ) ), istreambuf_iterator<char>() );

		const char	*InputPtrs[ 1 ] = { InputData.c_str() };

		glShaderSource( Shader, 1, InputPtrs, 0 );

		glCompileShader( Shader );

		glGetShaderiv( Shader, GL_COMPILE_STATUS, &Compiled );

		if( Compiled != GL_TRUE )
		{
			char		Log[ 1024 ];
			int			Len;

			glGetShaderInfoLog( Shader, sizeof( Log ), &Len, Log );

			MessageBox( 0, Log, (*it).c_str(), MB_OK );

			return( false );
		}

		mShaders.push_back( Shader );
	}

	return( true );
}

void Instance::getParameterDisplay( plugMainUnion &pResult )
{
#if 0
	if( pResult.ivalue >= mParams.size() )
	{
		pResult.ivalue = FF_FAIL;

		return;
	}

	ParamInstance		&PI = mParams[ pResult.ivalue ];

	switch( pResult.ivalue )
	{
		case PARAM_PROTOCOL:
			{
				const fugStreamProtocol Protocol = (fugStreamProtocol)mParams[ PARAM_PROTOCOL ].getIndex();

				switch( Protocol )
				{
					case FUG_STREAM_MEM:
						strcpy( PI.mValueString, "MEM" );
						break;

					case FUG_STREAM_UDP:
						strcpy( PI.mValueString, "UDP" );
						break;

					case FUG_STREAM_TCP:
						strcpy( PI.mValueString, "TCP" );
						break;
				}

				pResult.svalue = PI.mValueString;
			}
			break;

		case PARAM_PORT:
		case PARAM_IP1:
		case PARAM_IP2:
		case PARAM_IP3:
		case PARAM_IP4:
			{
				sprintf( PI.mValueString, "%d", PI.getInt() );

				pResult.svalue = PI.mValueString;
			}
			break;

		case PARAM_SEND:
		case PARAM_COMPRESS:
			{
				if( PI.getBool() )
				{
					strcpy( PI.mValueString, "On" );
				}
				else
				{
					strcpy( PI.mValueString, "Off" );
				}

				pResult.svalue = PI.mValueString;
			}
			break;

		default:
			InstanceBase::getParameterDisplay( pResult );
			break;
	}
#endif
}

void Instance::processOpengl( plugMainUnion &pResult )
{
	ProcessOpenGLStruct		*GL = reinterpret_cast<ProcessOpenGLStruct *>( pResult.ivalue );

	Plugin::checkError();

	pResult.ivalue = FF_FAIL;

	if( GL == 0 )
	{
		return;
	}

	if( GL->numInputTextures != 1 )
	{
		return;
	}

	if( GL->inputTextures == 0 )
	{
		return;
	}

	FFGLTextureStruct		*TS = GL->inputTextures[ 0 ];

	if( TS == 0 )
	{
		return;
	}

	glActiveTexture( GL_TEXTURE0 );

	glBindTexture( GL_TEXTURE_2D, TS->Handle );

	Plugin::checkError();

	glUseProgram( mProgram );

	Plugin::checkError();

	const double	mx = (double)TS->Width  / (double)TS->HardwareWidth;
	const double	my = (double)TS->Height / (double)TS->HardwareHeight;
	
	glBegin( GL_QUADS );
		glTexCoord2d( 0.0, 0.0 );
		glVertex2d( -1.0, -1.0 );
		glTexCoord2d( mx, 0.0 );
		glVertex2d( 1.0, -1.0 );
		glTexCoord2d( mx, my );
		glVertex2d( 1.0, 1.0 );
		glTexCoord2d( 0.0, my );
		glVertex2d( -1.0, 1.0 );
	glEnd();

	Plugin::checkError();

	glUseProgram( 0 );

	glBindTexture( GL_TEXTURE_2D, 0 );

	Plugin::checkError();

	pResult.ivalue = FF_SUCCESS;
}
