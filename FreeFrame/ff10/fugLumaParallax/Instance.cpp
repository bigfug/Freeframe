
#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "Plugin.h"
#include "Instance.h"


ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	const UINT FRAME_SIZE = VideoInfo->frameWidth * VideoInfo->frameHeight;

	this->FrameBuffer = ffFrameBufferFactory::getInstance( VideoInfo );

	for( int i = 0 ; i < 4 ; i++ )
	{
		this->Buffer[ i ]= new VideoPixel32bit[ FRAME_SIZE ];

		memset( this->Buffer[ i ], 0x00, FRAME_SIZE * sizeof(VideoPixel32bit) );
	}

	mLumaBuff = new VideoPixel8bit[ FRAME_SIZE ];
}

ffInstance::~ffInstance( void )
{
	for( int i = 0 ; i < 4 ; i++ )
	{
		delete this->Buffer[ i ];
	}

	delete mLumaBuff;
}

DWORD ffInstance::processFrame( VideoFrame pFrame )
{
	const	UINT		 FRAME_HEIGHT = FrameBuffer->getHeight();
	const	UINT		 FRAME_WIDTH  = FrameBuffer->getWidth();
	const	float		 Offset  = ( this->getParamFloat( FF_PARAM_OFFSET ) * 2.0f ) - 1.0f;
	const	BOOL		 Sample  = this->getParamBool( FF_PARAM_SAMPLE );
	const	BOOL		 Bypass  = this->getParamBool( FF_PARAM_BYPASS );
	const	BOOL		 FILL_GAPS = this->getParamBool( FF_PARAM_FILL_GAPS );

	if( Bypass )
	{
		return( FF_SUCCESS );
	}

	if( Sample )
	{
		VideoPixel8bit		*LumPix = mLumaBuff;
		UINT				 Pos;
		UINT				 Col;

		FrameBuffer->toLuma( pFrame.Vid32, mLumaBuff );

		for( int i = 0 ; i < 4 ; i++ )
		{
			memset( this->Buffer[ i ], 0x00, FrameBuffer->getFrameByteSize() );
		}

		VideoPixel32bit		*SrcPix = pFrame.Vid32;

		for( UINT y = 0 ; y < FRAME_HEIGHT ; y++ )
		{
			for( UINT x = 0 ; x < FRAME_WIDTH ; x++ )
			{
				Col = *LumPix++ / 64;
				Pos = ( y * FRAME_WIDTH ) + x;

				VideoPixel32bit		*DstPix = &this->Buffer[ Col ][ Pos ];
				
				*DstPix = *SrcPix++;

				DstPix->alpha = 255;
			}
		}
	}

	VideoPixel32bit		*DstPix = pFrame.Vid32;
	INT					 Offsets[ 4 ];

	Offsets[ 0 ] = 0 - (INT)( Offset * (FLOAT)( FRAME_WIDTH / 8 ) );
	Offsets[ 1 ] = 0 - (INT)( Offset * (FLOAT)( FRAME_WIDTH / 4 ) );
	Offsets[ 2 ] = 0 - (INT)( Offset * (FLOAT)( FRAME_WIDTH / 2 ) );
	Offsets[ 3 ] = 0 - (INT)( Offset * (FLOAT)( FRAME_WIDTH / 1 ) );

	for( UINT y = 0 ; y < FRAME_HEIGHT ; y++ )
	{
		for( UINT x = 0 ; x < FRAME_WIDTH ; x++ )
		{
			BOOL		DonePix = false;

			for( INT i = 3 ; i >= 0 && !DonePix ; i-- )
			{
				INT		 SrcPos = (INT)x + Offsets[ i ];

				if( SrcPos >= 0 && SrcPos < (INT)FRAME_WIDTH )
				{
					VideoPixel32bit		*SrcBuf = Buffer[ i ];

					assert( SrcBuf != 0 );

					SrcPos += ( y * FRAME_WIDTH );

					if( SrcBuf[ SrcPos ].alpha > 0x00 )
					{
						*DstPix++ = SrcBuf[ SrcPos ];

						DonePix = true;
					}
				}
			}

			if( !DonePix )
			{
				DstPix->value = 0;
				DstPix++;
			}
		}
	}

	if( FILL_GAPS && Offset != 0 )
	{
		VideoPixel32bit		*PixPtr = pFrame.Vid32;

		for( UINT y = 0 ; y < FRAME_HEIGHT ; y++ )
		{
			PixPtr = &pFrame.Vid32[ ( y * FRAME_WIDTH ) ];

			for( UINT x = 0 ; x < FRAME_WIDTH - 1 ; x++ )
			{
				if( PixPtr[ 1 ].alpha == 0 )
				{
					PixPtr[ 1 ] = PixPtr[ 0 ];
				}

				PixPtr++;
			}

			PixPtr = &pFrame.Vid32[ ( y * FRAME_WIDTH ) + FRAME_WIDTH - 2 ];

			for( INT x = FRAME_WIDTH - 2 ; x >= 0 ; x-- )
			{
				if( PixPtr[ 0 ].alpha == 0 )
				{
					PixPtr[ 0 ] = PixPtr[ 1 ];
				}

				PixPtr--;
			}
		}
	}

	return( FF_SUCCESS );
}
