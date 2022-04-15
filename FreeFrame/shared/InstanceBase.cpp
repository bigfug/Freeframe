
#include "InstanceBase.h"

#include <cassert>

#ifdef WIN32
#include <crtdbg.h>
#endif

#if !defined( FF_VERSION_10 )

const GLfloat		InstanceBase::mVertexData[] =
{
	// x,	y,			u,		v
	-1.0f,	-1.0f,		0.0f,	0.0f,
	 1.0f,	-1.0f,		1.0f,	0.0f,
	 1.0f,	 1.0f,		1.0f,	1.0f,
	-1.0f,	 1.0f,		0.0f,	1.0f,
};

#endif

InstanceBase::InstanceBase( PluginBase &pPluginBase, FreeFrameApiVersion pApiVersion, plugMainUnion &pResult )
	: mPluginBase( pPluginBase ), mApiVersion( pApiVersion )
#if FF_VERSION >= 15
	, mTime( 0.0 ), mVertexBufferId( 0 )
#endif
{
#if defined( FF_VERSION_10 )
	assert( pApiVersion == VERSION_10 );

	memcpy( &mVideoInfo, pResult.VISvalue, sizeof( VideoInfoStruct ) );
#endif

#if FF_VERSION >= 15
	assert( pApiVersion == VERSION_15 );

#if FF_VERSION >= 16
	memcpy( &mViewport, pResult.PointerValue, sizeof( FFGLViewportStruct ) );
#else
	memcpy( &mViewport, reinterpret_cast<FFGLViewportStruct *>( pResult.ivalue ), sizeof( FFGLViewportStruct ) );
#endif

//	FILE		*F;

//	if( ( F = fopen( "E:\\dev\\InstanceBase.txt", "w" ) ) != 0 )
//	{
//		fprintf( F, "VP: x=%d y=%d w=%d h=%d\n\n", mViewport.x, mViewport.y, mViewport.width, mViewport.height );

//		fclose( F );
//	}

	glGenBuffers( 1, &mVertexBufferId );

	if( mVertexBufferId )
	{
		glBindBuffer( GL_ARRAY_BUFFER, mVertexBufferId );

		glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * 4 * 4, &mVertexData[ 0 ], GL_STATIC_DRAW );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}

#endif

	const Param		*P = mPluginBase.getParameters();
	const size_t	 C = mPluginBase.getParameterCount();

	for( size_t i = 0 ; i < C ; i++ )
	{
		// Make sure the numbering is the same

		assert( P[ i ].mIndex == i );

		mParams.push_back( ParamInstance( P[ i ] ) );
	}
}

InstanceBase::~InstanceBase( void )
{
}

#if defined( FF_VERSION_10 )

void InstanceBase::processFrame( plugMainUnion &pResult )
{
	pResult.ivalue = FF_FAIL;
}

void InstanceBase::processFrameCopy( plugMainUnion &pResult )
{
	pResult.ivalue = FF_FAIL;
}

#endif

void InstanceBase::getInputStatus( plugMainUnion &pResult )
{
	pResult.ivalue = FF_INPUT_INUSE;
}

void InstanceBase::getParameterDisplay( plugMainUnion &pResult )
{
	if( pResult.ivalue >= mParams.size() )
	{
		pResult.ivalue = FF_FAIL;

		return;
	}

	ParamInstance		&PI = mParams[ pResult.ivalue ];

	pResult.svalue = PI.mValueString;

	const char *Fmt = mPluginBase.getParameterTypeFormat( PI.mParam.mType );

	if( !Fmt )
	{
		return;
	}

	switch( PI.mParam.mType )
	{
		case FF_TYPE_BOOLEAN:
			sprintf( PI.mValueString, Fmt, PI.getFloat() );
			break;

		case FF_TYPE_ONOFF:
			strcpy( PI.mValueString, PI.getBool() ? "On" : "Off" );
			break;

		case FF_TYPE_CHOICE:
			strcpy( PI.mValueString, PI.mParam.mStrings[ PI.getChoice() ] );
			break;

		case FF_TYPE_EVENT:
			sprintf( PI.mValueString, Fmt, PI.getFloat() );
			break;

		case FF_TYPE_RED:
		case FF_TYPE_GREEN:
		case FF_TYPE_BLUE:
			sprintf( PI.mValueString, Fmt, PI.getFloat() );
			break;

		case FF_TYPE_XPOS:
#if defined( FF_VERSION_10 )
			sprintf( PI.mValueString, Fmt, PI.mValueFloat * (float)mVideoInfo.FrameWidth );
#else
			sprintf( PI.mValueString, Fmt, PI.mValueFloat * (float)mViewport.width );
#endif
			break;

		case FF_TYPE_YPOS:
#if defined( FF_VERSION_10 )
			sprintf( PI.mValueString, Fmt, PI.mValueFloat * (float)mVideoInfo.FrameHeight );
#else
			sprintf( PI.mValueString, Fmt, PI.mValueFloat * (float)mViewport.height );
#endif
			break;

		case FF_TYPE_STANDARD:
			sprintf( PI.mValueString, Fmt, PI.getFloat() );
			break;

		case FF_TYPE_INTEGER:
			sprintf( PI.mValueString, Fmt, PI.getInt() );
			break;

		case FF_TYPE_TEXT:
			break;

		default:
			pResult.ivalue = FF_FAIL;
			break;
	}
}

void InstanceBase::setParameter( plugMainUnion &pResult )
{
	const SetParameterStruct		*SPS       = reinterpret_cast<SetParameterStruct *>( pResult.PointerValue );
#if FF_VERSION >= 16
	const float						 ValFloat  = SPS->NewParameterValue.FloatValue;
	const char						*ValString = (const char *)SPS->NewParameterValue.PointerValue;
#else
	const float						 ValFloat  = SPS->NewParameterValueFloat;
	const char						*ValString = SPS->NewParameterValueString;
#endif

	if( SPS->ParameterNumber >= mParams.size() )
	{
		pResult.ivalue = FF_FAIL;

		return;
	}

	ParamInstance		&PI = mParams[ SPS->ParameterNumber ];

	if( PI.mParam.mType == FF_TYPE_TEXT )
	{
		strcpy( PI.mValueString, ValString );
	}
	else
	{
		PI.mValueFloat = ValFloat;
	}

	pResult.ivalue = FF_SUCCESS;
}

void InstanceBase::getParameter( plugMainUnion &pResult )
{
	if( pResult.ivalue >= mParams.size() )
	{
		pResult.ivalue = FF_FAIL;

		return;
	}

	ParamInstance		&PI = mParams[ pResult.ivalue ];

	if( PI.mParam.mType == FF_TYPE_TEXT )
	{
		strcpy( PI.mValueString, (const char *)pResult.svalue );
	}
	else
	{
		pResult.fvalue = PI.mValueFloat;
	}
}

#if FF_VERSION >= 15

void InstanceBase::processOpengl( plugMainUnion &pResult )
{
	pResult.ivalue = FF_FAIL;
}

void InstanceBase::setTime( plugMainUnion &pResult )
{
	mTime = static_cast<double>( pResult.ivalue );

	pResult.ivalue = FF_SUCCESS;
}

#endif

bool InstanceBase::getParamBool( const size_t pIndex ) const
{
	assert( pIndex < mParams.size() );

	const ParamInstance		&PI = mParams[ pIndex ];

	return( PI.getBool() );
}

float InstanceBase::getParamFloat( const size_t pIndex ) const
{
	assert( pIndex < mParams.size() );

	const ParamInstance		&PI = mParams[ pIndex ];

	return( PI.getFloat() );
}

int InstanceBase::getParamInt( const size_t pIndex ) const
{
	assert( pIndex < mParams.size() );

	const ParamInstance		&PI = mParams[ pIndex ];

	return( PI.getInt() );
}

#if FF_VERSION >= 15

void InstanceBase::updateTexture( const FFGLTextureStruct *pInput, FFGLTextureStruct &pOutput, GLint pInternalFormat, GLint pFormat )
{
	if( pOutput.Handle == 0 || pOutput.HardwareWidth != pInput->HardwareWidth || pOutput.HardwareHeight != pInput->HardwareHeight )
	{
		if( pOutput.Handle != 0 )
		{
			glDeleteTextures( 1, &pOutput.Handle );

			pOutput.Handle = 0;
		}

		glGenTextures( 1, &pOutput.Handle );

		if( pOutput.Handle )
		{
			glBindTexture( GL_TEXTURE_2D, pOutput.Handle );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			pOutput.Width  = pInput->Width;
			pOutput.Height = pInput->Height;

#if 0
			GLint		MaxSize = 0;

			glGetIntegerv( GL_MAX_TEXTURE_SIZE, &MaxSize );

			pOutput.HardwareWidth  = min( GLint( pOutput.Width ),  MaxSize );
			pOutput.HardwareHeight = min( GLint( pOutput.Height ), MaxSize );

#if !defined( TARGET_OS_MAC )
//			if( !GLEW_ARB_texture_non_power_of_two )
#endif
			{
				GLint		MaxSizeW = MaxSize;

				while( MaxSizeW >= (GLint)pOutput.Width )
				{
					MaxSizeW >>= 1;
				}

				MaxSizeW <<= 1;

				pOutput.HardwareWidth = MaxSizeW;

				GLint		MaxSizeH = MaxSize;

				while( MaxSizeH >= (GLint)pOutput.Height )
				{
					MaxSizeH >>= 1;
				}

				MaxSizeH <<= 1;

				pOutput.HardwareHeight = MaxSizeH;
			}
#else
			pOutput.HardwareWidth  = pInput->HardwareWidth;
			pOutput.HardwareHeight = pInput->HardwareHeight;
#endif

			glTexImage2D( GL_TEXTURE_2D, 0, pInternalFormat, pOutput.HardwareWidth, pOutput.HardwareHeight, 0, pFormat, GL_UNSIGNED_BYTE, nullptr );

			glBindTexture( GL_TEXTURE_2D, 0 );
		}
	}
}

void InstanceBase::drawFrame()
{
	glBindBuffer( GL_ARRAY_BUFFER, mVertexBufferId );

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

#endif
