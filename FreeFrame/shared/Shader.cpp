#include "Shader.h"

#include <vector>
#include <iostream>
#include <fstream>

Shader::Shader( void )
	: mVertexShaderId( 0 ), mFragmentShaderId( 0 ), mProgramId( 0 )
{
}

Shader::~Shader()
{
	if( mProgramId != 0 )
	{
		glDeleteProgram( mProgramId );

		mProgramId = 0;
	}

	if( mFragmentShaderId != 0 )
	{
		glDeleteShader( mFragmentShaderId );

		mFragmentShaderId = 0;
	}

	if( mVertexShaderId != 0 )
	{
		glDeleteShader( mVertexShaderId );

		mVertexShaderId = 0;
	}
}

void Shader::load( const char *pV, const char *pF , const GLint *pLV, const GLint *pLF )
{
#if 0 //defined( _DEBUG )
	std::ifstream	vfile( pV, std::ios::in | std::ios::ate );

	if( !vfile.is_open() )
	{
		std::cerr << "Couldn't open " << pV << std::endl;

		return;
	}

	std::ifstream::pos_type		vsize = vfile.tellg();

	std::vector<char>			vsrc( int( vsize ) + 1, 0 );

	vfile.seekg( 0, std::ios::beg );

	vfile.read( vsrc.data(), vsize );

	//-------------------------------------------------------------------------

	std::ifstream	ffile( pF, std::ios::in | std::ios::ate );

	if( !ffile.is_open() )
	{
		std::cerr << "Couldn't open " << pF << std::endl;

		return;
	}

	std::ifstream::pos_type		fsize = ffile.tellg();

	std::vector<char>			fsrc( int( fsize ) + 1, 0 );

	ffile.seekg( 0, std::ios::beg );

	ffile.read( fsrc.data(), fsize );

	pV = vsrc.data();
	pF = fsrc.data();
#endif

	if( ( mVertexShaderId = glCreateShader( GL_VERTEX_SHADER ) ) != 0 )
	{
		glShaderSource( mVertexShaderId, 1, &pV, pLV );

		glCompileShader( mVertexShaderId );

		GLint compiled;

		glGetShaderiv( mVertexShaderId, GL_COMPILE_STATUS, &compiled );

		if( compiled == GL_FALSE )
		{
			GLint		LogLen;

			glGetShaderiv( mVertexShaderId, GL_INFO_LOG_LENGTH, &LogLen );

			std::vector<GLchar>	LogDat( LogLen );

			glGetShaderInfoLog( mVertexShaderId, LogDat.size(), nullptr, LogDat.data() );

			std::cerr << LogDat.data() << std::endl;

			return;
		}
	}

	if( ( mFragmentShaderId = glCreateShader( GL_FRAGMENT_SHADER ) ) != 0 )
	{
		glShaderSource( mFragmentShaderId, 1, &pF, pLF );

		glCompileShader( mFragmentShaderId );

		GLint compiled;

		glGetShaderiv( mFragmentShaderId, GL_COMPILE_STATUS, &compiled );

		if( compiled == GL_FALSE )
		{
			GLint		LogLen;

			glGetShaderiv( mFragmentShaderId, GL_INFO_LOG_LENGTH, &LogLen );

			std::vector<GLchar>	LogDat( LogLen );

			glGetShaderInfoLog( mFragmentShaderId, LogDat.size(), nullptr, LogDat.data() );

			std::cerr << LogDat.data() << std::endl;

			return;
		}
	}

	if( ( mProgramId = glCreateProgram() ) != 0 )
	{
		glAttachShader( mProgramId, mVertexShaderId );
		glAttachShader( mProgramId, mFragmentShaderId );

		glLinkProgram( mProgramId );

		GLint linked;

		glGetProgramiv( mProgramId, GL_LINK_STATUS, &linked );

		if( linked == GL_FALSE )
		{
			GLint		LogLen;

			glGetProgramiv( mProgramId, GL_INFO_LOG_LENGTH, &LogLen );

			std::vector<GLchar>	LogDat( LogLen );

			glGetProgramInfoLog( mProgramId, LogDat.size(), nullptr, LogDat.data() );

			std::cerr << LogDat.data() << std::endl;

			return;
		}
	}
}
