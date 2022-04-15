
#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "Plugin.h"
#include "Instance.h"

ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	memcpy( &mDstVideoInfo, VideoInfo, sizeof( VideoInfoStruct ) );

	mBuffer  = ffFrameBufferFactory::getInstance( VideoInfo );

	mDecoder = NULL;

	mCurrProtocol = STREAM_UDP;
	mLastProtocol = STREAM_UNKNOWN;

	mCurrPort = 0;
}

ffInstance::~ffInstance()
{
	SAFE_DELETE( mDecoder );
	SAFE_DELETE( mBuffer );
}

DWORD ffInstance::processFrame( VideoFrame pFrame )
{
	const int			 DEF_PROTOCOL = getParamInt( FF_PARAM_PROTOCOL );
	StreamProtocol		 Protocol = STREAM_UNKNOWN;
	BYTE				*FrameData;

	switch( DEF_PROTOCOL )
	{
		case 0:
			Protocol = STREAM_UDP;
			break;

		case 1:
			Protocol = STREAM_TCP;
			break;

		case 2:
			Protocol = STREAM_MEM;
			break;
	}

#if PLUGIN_TYPE == FF_EFFECT
	if( getParamBool( FF_PARAM_RECEIVE ) )
#endif
	{
		if( mDecoder != NULL && Protocol != mCurrProtocol )
		{
			SAFE_DELETE( mDecoder );
		}

		if( mDecoder == NULL )
		{
			if( Protocol != STREAM_UNKNOWN )
			{
				mDecoder = StreamDecoderFactory::getInstance( Protocol, mDstVideoInfo );

				mCurrProtocol = Protocol;
			}
		}

		if( mDecoder == NULL )
		{
			return( FF_FAIL );
		}

		if( mDecoder != NULL )
		{
			struct sockaddr_in	Addr;

			memset( &Addr, 0, sizeof( Addr ) );

			Addr.sin_port = htons( getParamInt( FF_PARAM_PORT ) );

			mDecoder->setSrcAddr( &Addr );

			if( getParamBool( FF_PARAM_HELP ) && ( mCurrPort != Addr.sin_port || mLastProtocol != mCurrProtocol ) )
			{
				char		 Buff[ 256 ];
				char		*ProtName = "UNKNOWN";

				mCurrPort = Addr.sin_port;
				mLastProtocol = mCurrProtocol;

				switch( mCurrProtocol )
				{
					case STREAM_UDP:
						ProtName = "UDP";
						break;

					case STREAM_TCP:
						ProtName = "TCP";
						break;

					case STREAM_MEM:
						ProtName = "MEM";
						break;
				}

				sprintf_s( Buff, sizeof( Buff ), "Current Settings:\n\nProtocol: %s\nPort: %d", ProtName, getParamInt( FF_PARAM_PORT ) );

				MessageBox( NULL, Buff, "fugStreamReceive Help", MB_OK );
			}
		}

		if( FrameData = mDecoder->lockReadBuffer() )
		{
			memcpy( pFrame.Frame, FrameData, mBuffer->getFrameByteSize() );

			mDecoder->unlockReadBuffer();
		}
	}

	return( FF_SUCCESS );
}
