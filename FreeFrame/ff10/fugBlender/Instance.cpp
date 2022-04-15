
#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "Plugin.h"
#include "Instance.h"
#include <assert.h>

ffInstance::ffInstance( FreeFramePlugin *Plugin, VideoInfoStruct *VideoInfo )
: FreeFrameInstance( Plugin, VideoInfo ), mFrameCount( 1 ), mChangeLength( 50 ), mBlendAmount( 0.0 )
{
	this->mFrameBuffer = ffFrameBufferFactory::getInstance( VideoInfo );

	mTempFrame1 = new BYTE[ mFrameBuffer->getFrameByteSize() ];
	mTempFrame2 = new BYTE[ mFrameBuffer->getFrameByteSize() ];

	mTempPtr = 0;

	mFrameStore = new FrameStore( mFrameBuffer );
}

DWORD ffInstance::processFrame( VideoFrame pFrame )
{
	const	UINT		 FRAME_WIDTH  = mFrameBuffer->getWidth();
	const	UINT		 FRAME_HEIGHT = mFrameBuffer->getHeight();
	const	UINT		 FRAME_BYTE_SIZE = mFrameBuffer->getFrameByteSize();
	const	bool		 P_ADVANCED   = this->getParamBool( FF_PARAM_ADVANCED );
	const	int			 P_BLEND_MODE = this->getParamInt( FF_PARAM_BLEND_MODE );
	BYTE				*Src1Ptr, *Src2Ptr, *DstPtr;
	BYTE				 Div;
	int					 Cmp;

	if( P_ADVANCED )
	{
		if( mTempPtr == 0 )
		{
			memcpy( mTempFrame1, pFrame.Frame, FRAME_BYTE_SIZE );

			mTempPtr = mTempFrame2;
		}

		if( mTempPtr == mTempFrame1 )
		{
			Cmp = memcmp( pFrame.Frame, mTempFrame2, FRAME_BYTE_SIZE );
		}
		else
		{
			Cmp = memcmp( pFrame.Frame, mTempFrame1, FRAME_BYTE_SIZE );
		}

		if( Cmp == 0 )
		{
			mFrameCount++;
		}
		else
		{
			memcpy( mTempPtr, pFrame.Frame, FRAME_BYTE_SIZE );

			mTempPtr = ( mTempPtr == mTempFrame1 ? mTempFrame2 : mTempFrame1 );

			Src1Ptr = mTempFrame1;
			Src2Ptr = mTempFrame2;
			DstPtr  = (BYTE *)pFrame.Frame;

			for( int i = 0 ; i < FRAME_BYTE_SIZE ; i++ )
			{
				switch( P_BLEND_MODE )
				{
					case 0:
						// 50/50
						*DstPtr = ( *Src1Ptr + *Src2Ptr ) / 2;
						break;

					case 1:
						// Multiply
						// theRed = theColour1.red * theColour2.red / 255.0 
						*DstPtr = ( *Src1Ptr * *Src2Ptr ) / 255;
						break;

					case 2:
						// Screen
						// 255 - ((255 - theColour1.red) * (255 - theColour2.red) / theDivisor) 
						*DstPtr = 255 - ( ( 255 - *Src1Ptr ) * ( 255 - *Src2Ptr ) / 255 );
						break;

					case 3:
						// Darken
						// if theColour1.red < theColour2.red then theRed = theColour1.red
						*DstPtr = ( *Src1Ptr < *Src2Ptr ? *Src1Ptr : *Src2Ptr );
						break;

					case 4:
						// Lighten
						// if theColour1.red > theColour2.red then theRed = theColour1.red
						*DstPtr = ( *Src1Ptr > *Src2Ptr ? *Src1Ptr : *Src2Ptr );
						break;

					case 5:
						// Difference
						// theRed = abs(theColour1.red - theColour2.red) 
						*DstPtr = abs( *Src1Ptr - *Src2Ptr );
						break;

					case 6:
						// Negation
						// theRed = 255 - abs(255 - theColour1.red - theColour2.red)
						*DstPtr = 255 - abs( 255 - *Src1Ptr - *Src2Ptr );
						break;

					case 7:
						// Exclusion
						// theRed = theColour1.red + theColour2.red - ((theColour1.red * theColour2.red) / theDivisor)
						*DstPtr = *Src1Ptr + *Src2Ptr - ( ( *Src1Ptr * *Src2Ptr ) / 127 );
						break;

					case 8:
						// Overlay/Hardlight
						// if  theColour1.red < 128 then
						//   theRed = theColour1.red * theColour2.red / theDivisor
						// else
						//   theRed = 255 - ((255 - theColour1.red) * (255 - theColour2.red) / theDivisor)
						// end if 

						if( *Src1Ptr < 128 )
						{
							*DstPtr = *Src1Ptr * *Src2Ptr / 127;
						}
						else
						{
							*DstPtr = 255 - ( ( 255 - *Src1Ptr ) * ( 255 - *Src2Ptr ) / 127 );
						}
						break;
				}

				Src1Ptr++;
				Src2Ptr++;
				DstPtr++;
			}

			mChangeLength = ( ( mChangeLength + mFrameCount ) / 2 );
			//mChangeLength = mFrameCount;
			mFrameCount   = 1;

			if( mFrameStore->getFrameCount() < 25 )
			{
				mFrameStore->addFrame( (BYTE *)pFrame.Frame );
			}
		}
	}
	else
	{
		mTempPtr = 0;

		if( mFrameStore->getFrameCount() == 0 )
		{
			mFrameStore->addFrame( (BYTE *)pFrame.Frame );
		}

		if( memcmp( pFrame.Frame, mFrameStore->getLastFrame(), FRAME_BYTE_SIZE ) == 0 )
		{
			mFrameCount++;
		}
		else
		{
			mChangeLength = ( ( mChangeLength + mFrameCount ) / 2 );
			//mChangeLength = mFrameCount;
			mFrameCount   = 1;

			if( mFrameStore->getFrameCount() < 25 )
			{
				mFrameStore->addFrame( (BYTE *)pFrame.Frame );
			}
		}
	}

	mBlendAmount += ( 1.0 / (double)mChangeLength );
	//mBlendAmount = (double)mFrameCount / (double)mChangeLength;

	if( mBlendAmount >= 1.0 )
	{
		mBlendAmount -= 1.0;

		mFrameStore->delFrame( 0 );
	}

	if( mFrameStore->getFrameCount() == 1 )
	{
		memcpy( pFrame.Frame, mFrameStore->getLastFrame(), FRAME_BYTE_SIZE );

		mChangeLength++;
	}
	else if( mFrameStore->getFrameCount() >= 2 )
	{
		Src1Ptr = (BYTE *)mFrameStore->getFrame( 0 );
		Src2Ptr = (BYTE *)mFrameStore->getFrame( 1 );
		DstPtr  = (BYTE *)pFrame.Frame;
		Div     = (BYTE)( 255.0 * mBlendAmount );

		for( int i = 0 ; i < FRAME_BYTE_SIZE ; i++ )
		{
			*DstPtr = *Src1Ptr + (BYTE)( ( ((double)*Src2Ptr) - ((double)*Src1Ptr) ) * mBlendAmount );

			Src1Ptr++;
			Src2Ptr++;
			DstPtr++;
		}
	}
/*
	DstPtr  = (BYTE *)pFrame.Frame;

	for( int i = 0 ; i < (int)( mBlendAmount * (double)FRAME_WIDTH ) ; i++ )
	{
		*DstPtr++ = 0x00;
		*DstPtr++ = 0xFF;
		*DstPtr++ = 0xFF;
		*DstPtr++ = 0xFF;
	}

	DstPtr  = ((BYTE *)pFrame.Frame) + ( ( FRAME_WIDTH * 2 ) * 4 );

	for( int i = 0 ; i < mFrameCount ; i++ )
	{
		*DstPtr++ = 0x00;
		*DstPtr++ = 0xFF;
		*DstPtr++ = 0x00;
		*DstPtr++ = 0xFF;
	}

	DstPtr  = ((BYTE *)pFrame.Frame) + ( ( FRAME_WIDTH * 4 ) * 4 );

	for( int i = 0 ; i < mFrameStore->getFrameCount() ; i++ )
	{
		*DstPtr++ = 0x00;
		*DstPtr++ = 0x00;
		*DstPtr++ = 0xFF;
		*DstPtr++ = 0xFF;
	}
*/
	return( FF_SUCCESS );
}
