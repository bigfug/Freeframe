
#pragma once

#if defined( FF_VERSION_10 )
#include <FreeFrame.h>
#else
#include <FFGL.h>
#endif

#include <PluginBase.h>
#include <cassert>
#include <cmath>

typedef struct ParamInstance
{
	const Param		&mParam;
	float			 mValueFloat;
	char			 mValueString[ 256 ];

	ParamInstance( const Param &pParam ) : mParam( pParam )
	{
		if( mParam.mDefaultString != 0 )
		{
			strcpy( mValueString, mParam.mDefaultString );

			mValueFloat = 0.0;
		}
		else
		{
			mValueFloat = mParam.getDefaultFloat();

			memset( mValueString, 0, sizeof( mValueString ) );
		}
	}

	ParamInstance operator = ( const ParamInstance &pSrc )
	{
		ParamInstance	P( pSrc.mParam );

		P.mValueFloat = pSrc.mValueFloat;

		strcpy( P.mValueString, pSrc.mValueString );

		return( P );
	}

	float getFloat( void ) const
	{
		assert( mParam.mDefaultString == 0 );

		return( mParam.mMin + ( ( mParam.mMax - mParam.mMin ) * mValueFloat ) );
	}

	int getInt( void ) const
	{
		return( int( getFloat() ) );
	}

	bool getBool( void ) const
	{
		assert( mParam.mDefaultString == 0 );

		return( mValueFloat > 0.0 );
	}

	int getIndex( void ) const
	{
		const int		Values = (const int)fabs( mParam.mMax - mParam.mMin ) + 1;
		const float		Width  = 1.0f / (float)Values;

		return( (int)( mParam.mMin + ( mValueFloat / Width ) ) );
	}

	int getChoice( void ) const
	{
		const int		MaxVal = int( mParam.mMax );
		const int		CurVal = int( ( mParam.mMax + 1.0f ) * mValueFloat );

		return( min( CurVal, MaxVal ) );
	}

} ParamInstance;

class InstanceBase
{
protected:
	PluginBase						&mPluginBase;
	vector<ParamInstance>			 mParams;
	const FreeFrameApiVersion		 mApiVersion;

#if defined( FF_VERSION_10 )
	VideoInfoStruct				 mVideoInfo;
#else
	FFGLViewportStruct			 mViewport;
	double						 mTime;

	GLuint						 mVertexBufferId;

	const static GLfloat		 mVertexData[];
#endif

	bool getParamBool( const size_t pIndex ) const;
	float getParamFloat( const size_t pIndex ) const;
	int getParamInt( const size_t pIndex ) const;

public:
	InstanceBase( PluginBase &pPluginBase, FreeFrameApiVersion pApiVersion, plugMainUnion &pResult );

	virtual ~InstanceBase( void );

	virtual void getParameterDisplay( plugMainUnion &pResult );
	virtual void setParameter( plugMainUnion &pResult );
	virtual void getParameter( plugMainUnion &pResult );

	virtual void getInputStatus( plugMainUnion &pResult );

#if defined( FF_VERSION_10 )
	virtual void processFrame( plugMainUnion &pResult );
	virtual void processFrameCopy( plugMainUnion &pResult );
#else
	virtual void processOpengl( plugMainUnion &pResult );

	virtual void setTime( plugMainUnion &pResult );

	static void updateTexture( const FFGLTextureStruct *pInput, FFGLTextureStruct &pOutput, GLint pInternalFormat, GLint pFormat );

	void drawFrame( void );
#endif
};
