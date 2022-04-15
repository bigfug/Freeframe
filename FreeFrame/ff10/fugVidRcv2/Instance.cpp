
#include "Instance.h"

Instance::Instance(Plugin &pPlugin, FreeFrameApiVersion pApiVersion, plugMainUnion &pResult )
: InstanceBase( pPlugin, pApiVersion, pResult ), mPlugin( pPlugin ), mVidBuf( 0 ), mDecoder( 0 )
{
	mLastProtocol = FUG_STREAM_UNKNOWN;
	mLastPort     = 0;
	mLastReceive  = false;

	mVidInf.frameHeight = (uint16_t)mVideoInfo.FrameHeight;
	mVidInf.frameWidth  = (uint16_t)mVideoInfo.FrameWidth;
	mVidInf.bitDepth    = (uint16_t)mVideoInfo.BitDepth;
	mVidInf.orientation = (uint16_t)mVideoInfo.Orientation;

	mVidSze = fugGetByteSize( &mVidInf );

	mVidBuf = new unsigned char[ mVidSze ];

	memset( mVidBuf, 0, mVidSze );

	pResult.ivalue = FF_SUCCESS;
}

Instance::~Instance( void )
{
	if( mDecoder != 0 )
	{
		fugDeleteDecoder( mDecoder );

		mDecoder = 0;
	}

	if( mVidBuf != 0 )
	{
		delete [] mVidBuf;

		mVidBuf = 0;
	}
}

void Instance::processFrame( plugMainUnion &pResult )
{
	const fugStreamProtocol	 ParamProtocol = (fugStreamProtocol)( mParams[ PARAM_PROTOCOL ].getChoice() + 1 );
	const unsigned short	 ParamPort     = mParams[ PARAM_PORT ].getInt();
#if !defined( FF_IS_SOURCE )
	const bool				 ParamReceive  = mParams[ PARAM_RECEIVE ].getBool();
#endif

#if !defined( FF_IS_SOURCE )
	if( ParamReceive && mVidBuf != 0)
#else
	if( mVidBuf != 0 )
#endif
	{
		if( ParamProtocol != mLastProtocol ||
#if !defined( FF_IS_SOURCE )
			ParamReceive  != mLastReceive ||
#endif
			ParamPort     != mLastPort )
		{
			if( mDecoder != 0 )
			{
				fugDeleteDecoder( mDecoder );

				mDecoder = 0;
			}

			if( ( mDecoder = fugGetDecoder( ParamProtocol, &mVidInf ) ) != 0 )
			{
				fugSetSourceAddress( mDecoder, FUG_BASE_PORT + ParamPort );
			}
		}

		if( mDecoder != 0 )
		{
			unsigned char		*SrcDat = fugLockReadBuffer( mDecoder, 0 );

			if( SrcDat != 0 )
			{
				memcpy( mVidBuf, SrcDat, mVidSze );

				fugUnlockReadBuffer( mDecoder );
			}
		}

		memcpy( pResult.svalue, mVidBuf, mVidSze );
	}
	else if( mDecoder != 0 )
	{
		fugDeleteDecoder( mDecoder );

		mDecoder = 0;
	}

#if !defined( FF_IS_SOURCE )
	mLastReceive  = ParamReceive;
#endif
	mLastProtocol = ParamProtocol;
	mLastPort     = ParamPort;

	pResult.ivalue = FF_SUCCESS;
}
