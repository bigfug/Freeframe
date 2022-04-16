#include "PluginBase.h"

#include <gl/glew.h>

#include <fstream>

#ifdef WIN32
#include <crtdbg.h>
#endif

#include "Instance.h"

Instance::Instance( Plugin &pPlugin, plugMainUnion &pResult )
: InstanceBase( pPlugin, pResult ), mPlugin( pPlugin ), mInstance( 0 )
{
	plugMainUnion		Result;

	mVidInf.FrameWidth  = mViewport.width - mViewport.x;
	mVidInf.FrameHeight = mViewport.height - mViewport.y;
	mVidInf.Orientation = FF_ORIENTATION_BL;

	if( mPlugin.mCap32bit )
	{
		mVidInf.BitDepth = FF_DEPTH_32;
	}
	else if( mPlugin.mCap24bit )
	{
		mVidInf.BitDepth = FF_DEPTH_24;
	}
	else if( mPlugin.mCap16bit )
	{
		mVidInf.BitDepth = FF_DEPTH_16;
	}

	Result = mPlugin.mPlugMain( FF_INSTANTIATE, (DWORD)&mVidInf, 0 );

	if( Result.ivalue == 0 || Result.ivalue == FF_FAIL || Result.ivalue == FF_UNSUPPORTED )
	{
		ofstream	LogFile( "fugWrapperGL.log", ios::app );

		LogFile << "Error creating instance of " << mPlugin.getLibName() << std::endl;

		pResult.ivalue = FF_FAIL;
	}
	else
	{
		mInstance = Result.svalue;

		pResult.ivalue = FF_SUCCESS;
	}
}

Instance::~Instance( void )
{
	if( mInstance != 0 )
	{
		plugMainUnion		Result;

		Result = mPlugin.mPlugMain( FF_DEINSTANTIATE, 0, (DWORD)mInstance );

		mInstance = 0;
	}

	if( !mOutput.checkBuf() )
	{
		ofstream	LogFile( "fugWrapperGL.log", ios::app );

		LogFile << "Output buffer has been damaged in " << mPlugin.getLibName() << std::endl;
	}

	for( vector<TexToBuf>::iterator it = mTexToBuf.begin() ; it != mTexToBuf.end() ; it++ )
	{
		if( !(*it).checkBuf() )
		{
			ofstream	LogFile( "fugWrapperGL.log", ios::app );

			LogFile << "Input buffer has been damaged in " << mPlugin.getLibName() << std::endl;
		}
	}
}

void Instance::getInputStatus( plugMainUnion &pResult )
{
	if( mPlugin.mPlugMain != 0 && mInstance != 0 )
	{
		pResult = mPlugin.mPlugMain( FF_DEINSTANTIATE, pResult.ivalue, (DWORD)mInstance );
	}
}

void Instance::getParameterDisplay( plugMainUnion &pResult )
{
	if( mPlugin.mPlugMain != 0 && mInstance != 0 )
	{
		pResult = mPlugin.mPlugMain( FF_GETPARAMETERDISPLAY, pResult.ivalue, (DWORD)mInstance );
	}
}

void Instance::setParameter( plugMainUnion &pResult )
{
	SetParameterStruct	*SIS = (SetParameterStruct *)pResult.svalue;

	if( mPlugin.mPlugMain != 0 && mInstance != 0 )
	{
		pResult = mPlugin.mPlugMain( FF_SETPARAMETER, pResult.ivalue, (DWORD)mInstance );
	}
}

void Instance::getParameter( plugMainUnion &pResult )
{
	if( mPlugin.mPlugMain != 0 && mInstance != 0 )
	{
		pResult = mPlugin.mPlugMain( FF_GETPARAMETER, pResult.ivalue, (DWORD)mInstance );
	}
}

void Instance::setTime( plugMainUnion &pResult )
{
	mTime = static_cast<double>( pResult.ivalue );
}

void Instance::processOpengl( plugMainUnion &pResult )
{
	ProcessOpenGLStruct		*GL = reinterpret_cast<ProcessOpenGLStruct *>( pResult.ivalue );

	if( GL == 0 )
	{
		return;
	}

	plugMainUnion		Result;

	pResult.ivalue = FF_FAIL;

	if( glewInit() != GLEW_OK )
	{
		return;
	}

	if( GL->numInputTextures < mPlugin.mCapMinInputFrames )
	{
		return;
	}

	if( GL->inputTextures == 0 )
	{
		return;
	}

	const size_t	TexCnt = min( mPlugin.mCapMaxInputFrames, GL->numInputTextures );
	size_t			MaxW   = ( mPlugin.mCapMaxInputFrames == 0 ? mVidInf.FrameWidth : 0 );
	size_t			MaxH   = ( mPlugin.mCapMaxInputFrames == 0 ? mVidInf.FrameHeight : 0 );
	double			mx     = 1.0;
	double			my     = 1.0;

	for( size_t i = 0 ; i < TexCnt ; i++ )
	{
		if( GL->inputTextures[ i ]->Width > MaxW )
		{
			MaxW = GL->inputTextures[ i ]->Width;
		}

		if( GL->inputTextures[ i ]->Height > MaxH )
		{
			MaxH = GL->inputTextures[ i ]->Height;
		}
	}

	if( MaxW != mVidInf.FrameWidth || MaxH != mVidInf.FrameHeight )
	{
		vector<plugMainUnion>		Params;

		Result = mPlugin.mPlugMain( FF_GETNUMPARAMETERS, pResult.ivalue, 0 );

		Params.resize( Result.ivalue );

		if( mInstance != 0 )
		{
			for( size_t i = 0 ; i < Params.size() ; i++ )
			{
				Params[ i ] = mPlugin.mPlugMain( FF_GETPARAMETER, i, (DWORD)mInstance );
			}

			Result = mPlugin.mPlugMain( FF_DEINSTANTIATE, 0, (DWORD)mInstance );

			mInstance = 0;
		}
		else
		{
			for( size_t i = 0 ; i < Params.size() ; i++ )
			{
				Params[ i ] = mPlugin.mPlugMain( FF_GETPARAMETERDEFAULT, i, 0 );
			}
		}

		mVidInf.FrameWidth  = MaxW;
		mVidInf.FrameHeight = MaxH;

		Result = mPlugin.mPlugMain( FF_INSTANTIATE, (DWORD)&mVidInf, 0 );

		if( ( mInstance = Result.svalue ) != 0 )
		{
			SetParameterStruct		SPS;

			for( size_t i = 0 ; i < Params.size() ; i++ )
			{
				SPS.ParameterNumber = i;
				SPS.NewParameterValueFloat = Params[ i ].fvalue;

				mPlugin.mPlugMain( FF_SETPARAMETER, (DWORD)&SPS, (DWORD)mInstance );
			}
		}
	}

	if( mInstance != 0 )
	{
		mTexToBuf.resize( TexCnt );

		for( size_t i = 0 ; i < TexCnt ; i++ )
		{
			mTexToBuf[ i ].fromTexSized( *GL->inputTextures[ i ], mVidInf.BitDepth, MaxW, MaxH );
		}

		if( mTexToBuf.size() == 0 )
		{
			mOutput.makeTex( mVidInf );

			pResult = mPlugin.mPlugMain( FF_PROCESSFRAME, (DWORD)mOutput.getBuf(), (DWORD)mInstance );

			mOutput.genTex( mVidInf );
		}
		else if( mTexToBuf.size() <= 1 && ( !mPlugin.mCapProcessFrameCopy || mPlugin.mCapCopyOrInPlace != FF_CAP_PREFER_COPY ) )
		{
			pResult = mPlugin.mPlugMain( FF_PROCESSFRAME, (DWORD)mTexToBuf[ 0 ].getBuf(), (DWORD)mInstance );

			if( mVidInf.BitDepth == FF_DEPTH_32 && mPlugin.mForceAlpha )
			{
				BYTE		*P = mTexToBuf[ 0 ].getBuf();

				for( unsigned y = 0 ; y < mVidInf.FrameHeight ; y++ )
				{
					for( unsigned x = 0 ; x < mVidInf.FrameWidth ; x++, P += 4 )
					{
						P[ 3 ] = 0xff;
					}
				}
			}

			mOutput.genTex( mVidInf, mTexToBuf[ 0 ].getBuf() );
		}
		else
		{
			ProcessFrameCopyStruct		PFC;
			vector<void *>				Input;

			for( size_t i = 0 ; i < TexCnt ; i++ )
			{
				Input.push_back( mTexToBuf[ i ].getBuf() );
			}

			mOutput.makeTex( mVidInf );

			PFC.numInputFrames = mTexToBuf.size();
			PFC.ppInputFrames  = &Input[ 0 ];
			PFC.pOutputFrame   = mOutput.getBuf();

			pResult = mPlugin.mPlugMain( FF_PROCESSFRAMECOPY, (DWORD)&PFC, (DWORD)mInstance );

			mOutput.genTex( mVidInf );
		}

		glBindTexture( GL_TEXTURE_2D, mOutput.getTex() );

		mx = mOutput.getTexMaxCoordX();
		my = mOutput.getTexMaxCoordY();
	}
	else
	{
		FFGLTextureStruct		*TS = GL->inputTextures[ 0 ];

		if( TS == 0 )
		{
			return;
		}

		glBindTexture( GL_TEXTURE_2D, TS->Handle );

		mx = (double)TS->Width  / (double)TS->HardwareWidth;
		my = (double)TS->Height / (double)TS->HardwareHeight;
	}

	glEnable( GL_TEXTURE_2D );

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

	glDisable( GL_TEXTURE_2D );

	glBindTexture( GL_TEXTURE_2D, 0 );

	for( vector<TexToBuf>::iterator it = mTexToBuf.begin() ; it != mTexToBuf.end() ; it++ )
	{
		(*it).freeGL();
	}

	mOutput.freeGL();
}
