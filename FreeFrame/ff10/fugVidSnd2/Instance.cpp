
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
	if( mEncoder != 0 )
	{
		fugDeleteEncoder( mEncoder );

		mEncoder = 0;
	}
}

void Instance::processFrame( plugMainUnion &pResult )
{
	const int				 ParamConfig = mParams[ PARAM_CONFIG ].getInt();
	const int				 ParamFPS    = mParams[ PARAM_FPS ].getInt();
	const bool				 ParamSend   = mParams[ PARAM_SEND ].getBool();

	const boost::posix_time::ptime		CurrTime = boost::posix_time::microsec_clock::universal_time();

	if( ParamSend )
	{
		if( ParamFPS > 0 )
		{
			if( CurrTime < mLastSend )
			{
				pResult.ivalue = FF_SUCCESS;

				return;
			}
		}

		if( ParamConfig != mCurrConfig )
		{
			if( mEncoder != 0 )
			{
				fugDeleteEncoder( mEncoder );

				mEncoder = 0;
			}
		}

		if( mEncoder == 0 )
		{
			fugVideoInfo	mVidInf;

			mVidInf.frameWidth  = (uint16_t)mVideoInfo.FrameWidth;
			mVidInf.frameHeight = (uint16_t)mVideoInfo.FrameHeight;
			mVidInf.bitDepth    = (uint16_t)mVideoInfo.BitDepth;
			mVidInf.orientation = (uint16_t)mVideoInfo.Orientation;

#if 1
			switch( mVidInf.bitDepth )
			{
				case FF_DEPTH_16:
					mConBuf.resize( mVidInf.frameWidth * mVidInf.frameHeight * 2 );
					break;

				case FF_DEPTH_24:
					mConBuf.resize( mVidInf.frameWidth * mVidInf.frameHeight * 3 );
					break;

				case FF_DEPTH_32:
					mConBuf.resize( mVidInf.frameWidth * mVidInf.frameHeight * 4 );
					break;
			}
#endif

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
#if 1

			const uint8_t	*SrcPtr = (const uint8_t *)pResult.svalue;
			uint8_t			*DstPtr = &mConBuf[ 0 ];

			switch( mVideoInfo.BitDepth )
			{
				case FF_DEPTH_16:
					for( uint16_t y = 0 ; y < mVideoInfo.FrameHeight ; y++ )
					{
						for( uint16_t x = 0 ; x < mVideoInfo.FrameWidth ; x++ )
						{
							DstPtr[ 0 ] = SrcPtr[ 0 ];
							DstPtr[ 1 ] = SrcPtr[ 1 ];

							SrcPtr += 2;
							DstPtr += 2;
						}
					}
					break;

				case FF_DEPTH_24:
					for( uint16_t y = 0 ; y < mVideoInfo.FrameHeight ; y++ )
					{
						for( uint16_t x = 0 ; x < mVideoInfo.FrameWidth ; x++ )
						{
							DstPtr[ 0 ] = SrcPtr[ 2 ];
							DstPtr[ 1 ] = SrcPtr[ 1 ];
							DstPtr[ 2 ] = SrcPtr[ 0 ];

							SrcPtr += 3;
							DstPtr += 3;
						}
					}
					break;

				case FF_DEPTH_32:
					for( uint16_t y = 0 ; y < mVideoInfo.FrameHeight ; y++ )
					{
						for( uint16_t x = 0 ; x < mVideoInfo.FrameWidth ; x++ )
						{
							DstPtr[ 0 ] = SrcPtr[ 2 ];
							DstPtr[ 1 ] = SrcPtr[ 1 ];
							DstPtr[ 2 ] = SrcPtr[ 0 ];
							DstPtr[ 3 ] = SrcPtr[ 3 ];

							SrcPtr += 4;
							DstPtr += 4;
						}
					}
					break;
			}

			fugEncode( mEncoder, &mConBuf[ 0 ] );
#else
			fugEncode( mEncoder, pResult.svalue );
#endif

			if( ParamFPS > 0 )
			{
				mLastSend += boost::posix_time::milliseconds( 1000 / ParamFPS );
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

		mLastSend = CurrTime;
	}

	pResult.ivalue = FF_SUCCESS;
}
