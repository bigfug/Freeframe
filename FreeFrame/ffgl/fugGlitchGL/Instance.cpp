
#include "Instance.h"

#include <algorithm>

using namespace std;

#if !defined(Q_UNUSED)
#define Q_UNUSED(arg) (void)arg;
#endif

Instance::Instance( Plugin &pPlugin, FreeFrameApiVersion pApiVersion, plugMainUnion &pResult )
	: InstanceBase( pPlugin, pApiVersion, pResult ), mPlugin( pPlugin )
{
	mCompressionHandle = tjInitCompress();

	mDecompressionHandle = tjInitDecompress();

	mJpegBuffer = 0;
	mJpegBufferSize = 0;
}

Instance::~Instance( void )
{
}

void Instance::processOpengl( plugMainUnion &pResult )
{
	const bool				 ParamGlitch  = mParams[ PARAM_GLITCH ].getBool();
	const int				 ParamQuality = mParams[ PARAM_QUALITY ].getInt();
	const float				 ParamSeed    = mParams[ PARAM_SEED ].getFloat();
	const int				 ParamAmount1 = mParams[ PARAM_AMOUNT1 ].getInt();
	const unsigned char		 ParamSearch  = mParams[ PARAM_SEARCH ].getInt();
	const unsigned char		 ParamReplace = mParams[ PARAM_REPLACE ].getInt();
	const float				 ParamAmount2 = mParams[ PARAM_AMOUNT2 ].getFloat();

#if defined( FF_VERSION_16 )
	ProcessOpenGLStruct		*GL = reinterpret_cast<ProcessOpenGLStruct *>( pResult.PointerValue );
#else
	ProcessOpenGLStruct		*GL = reinterpret_cast<ProcessOpenGLStruct *>( pResult.ivalue );
#endif

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

	GLuint			TexId = TS->Handle;

	glActiveTexture( GL_TEXTURE0 );

	mTexToBuf.fromTex( *TS, FF_DEPTH_32 );

	if( tjCompress2( mCompressionHandle, mTexToBuf.getBuf(), TS->Width, TS->Width * 4, TS->Height, TJPF_RGBA, &mJpegBuffer, &mJpegBufferSize, TJSAMP_422, ParamQuality, 0 ) != 0 )
	{
		return;
	}

	if( ParamSeed != 0.0f )
	{
		srand( (unsigned int)( ParamSeed * float( RAND_MAX ) ) );
	}

	if( ParamGlitch )
	{
		for( int i = 0 ; i < ParamAmount1 ; i++ )
		{
			long	p = long( ( double( rand() ) * double( mJpegBufferSize ) ) / double( RAND_MAX ) );

			mJpegBuffer[ p ] = rand();
		}

		if( ParamSearch != ParamReplace )
		{
			for( unsigned long i = 0 ; i < mJpegBufferSize ; i++ )
			{
				if( mJpegBuffer[ i ] != ParamSearch )
				{
					continue;
				}

				if( float( rand() ) / float( RAND_MAX ) > ParamAmount2 )
				{
					continue;
				}

				mJpegBuffer[ i ] = ParamReplace;
			}
		}
	}

	VideoInfoStruct		VIS;

	VIS.FrameWidth  = TS->Width;
	VIS.FrameHeight = TS->Height;
	VIS.BitDepth    = FF_DEPTH_32;
	VIS.Orientation = FF_ORIENTATION_TL;

	mBufToTex.makeTex( VIS );

	if( mBufToTex.getBuf() != 0 )
	{
		if( tjDecompress2( mDecompressionHandle, mJpegBuffer, mJpegBufferSize, mBufToTex.getBuf(), TS->Width, TS->Width * 4, TS->Height, TJPF_RGBA, 0 ) == 0 )
		{
		}

		mBufToTex.genTex( VIS );

		TexId = mBufToTex.getTex();
	}

	glEnable( GL_TEXTURE_2D );

	glBindTexture( GL_TEXTURE_2D, mBufToTex.getTex() );

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	glBegin( GL_QUADS );
		glTexCoord2d( 0.0, 0.0 );
		glVertex2f( -1.0f, -1.0f );
		glTexCoord2d( mBufToTex.getTexMaxCoordX(), 0.0 );
		glVertex2f( +1.0f, -1.0f );
		glTexCoord2d( mBufToTex.getTexMaxCoordX(), mBufToTex.getTexMaxCoordY() );
		glVertex2f( +1.0f, +1.0f );
		glTexCoord2d( 0.0, mBufToTex.getTexMaxCoordY() );
		glVertex2f( -1.0f, +1.0f );
	glEnd();

	glBindTexture( GL_TEXTURE_2D, 0 );

	glDisable( GL_TEXTURE_2D );

	pResult.ivalue = FF_SUCCESS;
}
