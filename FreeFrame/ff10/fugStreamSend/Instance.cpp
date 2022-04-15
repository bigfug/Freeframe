
#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "Plugin.h"
#include "Instance.h"

ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	mLastConfig = ~0;
	mFrameSkip  = 0;

	mFrameBuffer     = ffFrameBufferFactory::getInstance( VideoInfo );
	mFrameData.Frame = new BYTE[ mFrameBuffer->getFrameByteSize() ];

	memcpy( &mFrameInfo, VideoInfo, sizeof( VideoInfoStruct ) );
}

ffInstance::~ffInstance()
{
	for( vector<StreamEncoder *>::iterator it = mEncoders.begin() ; it != mEncoders.end() ; it++ )
	{
		SAFE_DELETE( *it );
	}

	mEncoders.clear();

	SAFE_DELETE( mFrameData.Frame );
	SAFE_DELETE( mFrameBuffer );
}

DWORD ffInstance::processFrame( VideoFrame pFrame )
{
	UINT				 CurrConfig = getParamInt( FF_PARAM_CONFIG );
	bool				 FlipFrame  = getParamBool( FF_PARAM_VFLIP );
	VideoFrame			 Frame  = pFrame;
	StreamEncoder		*Encoder;

	if( getParamBool( FF_PARAM_SEND ) )
	{
		if( this->mFrameSkip++ < getParamFloat( FF_PARAM_SKIP ) )
		{
			return( FF_SUCCESS );
		}

		this->mFrameSkip = 0;

		if( FlipFrame )
		{
			mFrameBuffer->flipV( pFrame, mFrameData );

			Frame = mFrameData;
		}

		if( CurrConfig != mLastConfig )
		{
			for( vector<StreamEncoder *>::iterator it = mEncoders.begin() ; it != mEncoders.end() ; it++ )
			{
				SAFE_DELETE( *it );
			}

			mEncoders.clear();

			for( StreamConfig *Config = &gConfig ; Config != NULL ; Config = Config->mNext )
			{
				if( Config->mNumber == CurrConfig )
				{
					if( ( Encoder = StreamEncoderFactory::getInstance( Config->mProtocol, mFrameInfo ) ) != NULL )
					{
						struct sockaddr_in		DstAddr;
						VideoInfoStruct			DstFormat;

						memset( &DstAddr, 0, sizeof( DstAddr ) );

						DstAddr.sin_addr.S_un.S_un_b.s_b1 = Config->mAddress[ 0 ];
						DstAddr.sin_addr.S_un.S_un_b.s_b2 = Config->mAddress[ 1 ];
						DstAddr.sin_addr.S_un.S_un_b.s_b3 = Config->mAddress[ 2 ];
						DstAddr.sin_addr.S_un.S_un_b.s_b4 = Config->mAddress[ 3 ];

						DstAddr.sin_family = AF_INET;
						DstAddr.sin_port   = htons( Config->mPort );

						Encoder->setDstAddr( &DstAddr );

						memset( &DstFormat, 0, sizeof( DstFormat ) );

						DstFormat.frameWidth  = gWidth;
						DstFormat.frameHeight = gHeight;
						DstFormat.bitDepth    = ffFrameBuffer::getFreeFrameDepthFromBits( gDepth );

						Encoder->setDstFormat( DstFormat );

						mEncoders.push_back( Encoder );
					}
				}
			}

			if( mEncoders.empty() )
			{
				MessageBox( NULL, "Can't find config for encoding", "StreamSend", MB_OK );
			}

			mLastConfig = CurrConfig;
		}

		for( vector<StreamEncoder *>::iterator it = mEncoders.begin() ; it != mEncoders.end() ; it++ )
		{
			Encoder = *it;

			Encoder->encode( Frame );
		}
	}

	return( FF_SUCCESS );
}
