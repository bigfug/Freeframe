
#include <gl/glew.h>

#include "Instance.h"

#include "INIReader.h"

#if defined( TARGET_OS_WIN )
#include <shlwapi.h>
#include <ShlObj.h>
#endif

Instance::Instance( Plugin &pPlugin, FreeFrameApiVersion pApiVersion, plugMainUnion &pResult )
: InstanceBase( pPlugin, pApiVersion, pResult ), mPlugin( pPlugin ), mEncoder( 0 )
{
	mCurrConfig = -1;

#if defined( TARGET_OS_WIN )
	TCHAR szPath[MAX_PATH];

	if( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath ) ) )
	{
		PathAppend( szPath, L"bigfug" );
		PathAppend( szPath, L"fugStream.ini" );

		std::wstring t = std::wstring( szPath );

		mSettingsFileName = std::string( t.begin(), t.end() );
	}
#elif defined( TARGET_OS_MAC )
	mSettingsFileName = "/Library/Preferences/fugStream.ini";
#else
	mSettingsFileName = "/etc/xdg/bigfug/fugStream.ini";
#endif

	mLastSend = boost::posix_time::microsec_clock::universal_time();

	pResult.ivalue = FF_SUCCESS;
}

Instance::~Instance( void )
{
	mTexToBuf.freeGL();

	if( mEncoder != 0 )
	{
		fugDeleteEncoder( mEncoder );

		mEncoder = 0;
	}
}

void Instance::processOpengl( plugMainUnion &pResult )
{
	const int				 ParamConfig = mParams[ PARAM_CONFIG ].getChoice();
	const int				 ParamFPS    = mParams[ PARAM_FPS ].getInt();
	const bool				 ParamSend   = mParams[ PARAM_SEND ].getBool();
	const int				 ParamFlipY  = mParams[ PARAM_FLIPY ].getBool();

	const boost::posix_time::ptime		CurrTime = boost::posix_time::microsec_clock::universal_time();

	ProcessOpenGLStruct		*GL = reinterpret_cast<ProcessOpenGLStruct *>( pResult.ivalue );

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

	if( ParamSend )
	{
		if( ParamFPS == 0 || CurrTime >= mLastSend )
		{
			if( ParamConfig != mCurrConfig )
			{
				if( mEncoder != 0 )
				{
					fugDeleteEncoder( mEncoder );

					mEncoder = 0;
				}

				mTexToBuf.freeGL();
			}

			if( mEncoder == 0 )
			{
				mVidInf.frameHeight = (uint16_t)TS->Height;
				mVidInf.frameWidth  = (uint16_t)TS->Width;
				mVidInf.bitDepth    = FUG_VID_24BITVIDEO;
				mVidInf.orientation = FUG_ORIEN_BOT_LEFT;

				std::stringstream		ConfigStream;
				std::string				Section;

				ConfigStream << ParamConfig;

				Section = ConfigStream.str();

				INIReader			ini( mSettingsFileName );

				fugStreamProtocol		ParamProtocol = (fugStreamProtocol)ini.GetInteger( Section, "protocol", FUG_STREAM_MEM );
				std::string				ParamAddr     = ini.Get( Section, "address", "127.0.0.1" );
				uint16_t				ParamChannel  = uint16_t( ini.GetInteger( Section, "channel", 0 ) );
				uint16_t				ParamWidth    = uint16_t( ini.GetInteger( Section, "width", 0 ) );
				uint16_t				ParamHeight   = uint16_t( ini.GetInteger( Section, "height", 0 ) );
				uint16_t				ParamDepth    = uint16_t( ini.GetInteger( Section, "depth", FUG_VID_24BITVIDEO ) );
				fugStreamCompression	ParamCompressionType = (fugStreamCompression)ini.GetInteger( Section, "compressiontype", FUG_COMPRESS_NONE );
				uint16_t				ParamCompressionLevel = uint16_t( ini.GetInteger( Section, "compressionlevel", 85 ) );

				if( ( mEncoder = fugGetEncoder( ParamProtocol, &mVidInf ) ) != 0 )
				{
					fugSetDestAddress( mEncoder, ParamAddr.c_str(), FUG_BASE_PORT + ParamChannel );

					fugVideoInfo	DstVidInf;

					DstVidInf.frameWidth  = ParamWidth > 0 ? ParamWidth : mVidInf.frameWidth;
					DstVidInf.frameHeight = ParamHeight > 0 ? ParamHeight : mVidInf.frameHeight;
					DstVidInf.bitDepth    = ParamDepth;
					DstVidInf.orientation = mVidInf.orientation;

					fugSetDestFormat( mEncoder, &DstVidInf, ParamCompressionType, ParamCompressionLevel );

					mCurrConfig = ParamConfig;
				}
			}

			if( mEncoder != 0 )
			{
				glActiveTexture( GL_TEXTURE0 );

				mTexToBuf.fromTex( *TS, FF_DEPTH_24 );

				if( ParamFlipY )
				{
					mVidBuf.resize( fugGetByteSize( &mVidInf ) );

					const size_t	DstStride = TS->Width * 3;

					for( size_t y = 0 ; y < TS->Height ; y++ )
					{
						const unsigned char		*SrcPtr = &mTexToBuf.getBuf()[ DstStride * y ];
						unsigned char			*DstPtr = &mVidBuf[ DstStride * ( TS->Height - 1 - y ) ];

						memcpy( DstPtr, SrcPtr, DstStride );
					}

					fugEncode( mEncoder, &mVidBuf[ 0 ] );
				}
				else
				{
					fugEncode( mEncoder, mTexToBuf.getBuf() );
				}

				if( ParamFPS > 0 )
				{
					mLastSend += boost::posix_time::milliseconds( 1000 / ParamFPS );
				}
			}
		}
	}
	else
	{
		if( mEncoder != 0 )
		{
			fugDeleteEncoder( mEncoder );

			mEncoder = 0;
		}

		mTexToBuf.freeGL();

		mVidBuf.clear();

		mLastSend = CurrTime;
	}

	glBindTexture( GL_TEXTURE_2D, TS->Handle );

	const double	mx = (double)TS->Width  / (double)TS->HardwareWidth;
	const double	my = (double)TS->Height / (double)TS->HardwareHeight;

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

	pResult.ivalue = FF_SUCCESS;
}
