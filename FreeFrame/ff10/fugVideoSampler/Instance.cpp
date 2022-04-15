
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include "Plugin.h"
#include "Instance.h"

ffInstance::ffInstance( ffPlugin *Plugin, VideoInfoStruct *VideoInfo ) : FreeFrameInstance( Plugin, VideoInfo )
{
	DWORD				 PixelSize;

	this->FrameBuffer = ffFrameBufferFactory::getInstance( VideoInfo );
	this->FrameHead   = NULL;
	this->FrameTail   = NULL;
	this->FrameSave   = NULL;
	this->FramePlay   = NULL;
	this->FrameCount  = 0;
	this->Sample      = false;
	this->Play        = false;
	this->Shuttle     = 0.0f;

	switch( this->videoInfo.bitDepth )
	{
		case FF_CAP_16BITVIDEO:
			PixelSize = sizeof(VideoPixel16bit);
			break;

		case FF_CAP_24BITVIDEO:
			PixelSize = sizeof(VideoPixel24bit);
			break;

		case FF_CAP_32BITVIDEO:
			PixelSize = sizeof(VideoPixel32bit);
			break;
	}

	this->FrameSize = PixelSize * this->videoInfo.frameWidth * this->videoInfo.frameHeight;
}

ffInstance::~ffInstance()
{
	this->removeFrames();
}

void ffInstance::removeFrames()
{
	VideoFrameChain		*Curr, *Next;

	for( Curr = this->FrameHead ; Curr != NULL ; Curr = Next )
	{
		Next = Curr->Next;

		free( Curr );
	}

	this->FrameHead  = NULL;
	this->FrameTail  = NULL;
	this->FrameSave  = NULL;
	this->FramePlay  = NULL;
	this->FrameCount = 0;
}

DWORD ffInstance::addFrame( VideoFrame pFrame )
{
	VideoFrameChain		*Curr;

	if( this->FrameSave != NULL )
	{
		memcpy( &this->FrameSave->Frame, pFrame.Frame, this->FrameSize );

		this->FrameSave = this->FrameSave->Next;

		return( FF_SUCCESS );
	}

	if( Curr = (VideoFrameChain *)malloc( sizeof(VideoFrameChain) + this->FrameSize ) )
	{
		memcpy( &Curr->Frame, pFrame.Frame, FrameSize );

		Curr->Next = NULL;

		if( this->FrameHead == NULL )
		{
			Curr->Prev = NULL;

			this->FrameHead = this->FrameTail = Curr;
		}
		else
		{
			Curr->Prev = this->FrameTail;

			this->FrameTail->Next = Curr;
			this->FrameTail       = Curr;
		}

		this->FrameCount++;

		if( this->FrameCount % FRAME_INDEX == 0 )
		{
			this->FrameRecord[ this->FrameCount / FRAME_INDEX ] = Curr;
		}

		return( FF_SUCCESS );
	}

	return( FF_FAIL );
}

DWORD ffInstance::processFrame( VideoFrame pFrame )
{
	if( this->getParamBool( FF_PARAM_SAMPLE ) )
	{
		if( !this->Sample )
		{
			this->removeFrames();
		}

		if( this->FrameCount >= this->getParamInt( FF_PARAM_FRAMES ) )
		{
			return( FF_SUCCESS );
		}

		if( this->addFrame( pFrame ) != FF_SUCCESS )
		{
			return( FF_FAIL );
		}
	}

	this->Sample = this->getParamBool( FF_PARAM_SAMPLE );

	if( this->Sample )
	{
		return( FF_SUCCESS );
	}

	if( this->getParamBool( FF_PARAM_PLAY ) )
	{
		if( this->FramePlay == NULL || !this->Play )
		{
			this->FramePlay = this->FrameHead;
			this->FramePos  = 0;
		}

		this->Play = this->getParamBool( FF_PARAM_PLAY );

		if( this->FramePlay == NULL )
		{
			return( FF_SUCCESS );
		}

		if( this->getParamBool( FF_PARAM_POSITIONING ) )
		{
			int		NewPos  = (DWORD)( this->getParamFloat( FF_PARAM_POSITION ) * ( this->FrameCount - 1 ) );
			int		OrigPos = this->FramePos;

			if( NewPos == 0 )
			{
				this->FramePlay = this->FrameHead;
				this->FramePos  = 0;
			}
			else if( NewPos == this->FrameCount - 1 )
			{
				this->FramePlay = this->FrameTail;
				this->FramePos  = NewPos;
			}
			else if( abs( this->FramePos - NewPos ) < FRAME_INDEX )
			{
				if( this->FramePos < NewPos )
				{
					while( this->FramePlay && this->FramePos < NewPos )
					{
						this->FramePlay = this->FramePlay->Next;
						this->FramePos++;
					}

					if( !this->FramePlay )
					{
						//sprintf( Buff, "#1: Count: %d  Orig: %d  New: %d  Curr: %d", this->FrameCount, OrigPos, NewPos, this->FramePos );

						//MessageBox( NULL, Buff, "Debug", MB_OK );
					}
				}
				else
				{
					while( this->FramePlay && this->FramePos > NewPos )
					{
						this->FramePlay = this->FramePlay->Prev;
						this->FramePos--;
					}

					if( !this->FramePlay )
					{
						//sprintf( Buff, "#2: Count: %d  Orig: %d  New: %d  Curr: %d", this->FrameCount, OrigPos, NewPos, this->FramePos );

						//MessageBox( NULL, Buff, "Debug", MB_OK );
					}
				}
			}
			else
			{
				this->FramePos = FRAME_INDEX * ( NewPos / FRAME_INDEX );

				if( this->FramePos == 0 )
				{
					this->FramePlay = this->FrameHead;
				}
				else
				{
					this->FramePlay = this->FrameRecord[ NewPos / FRAME_INDEX ];
				}

				while( this->FramePlay && this->FramePos < NewPos )
				{
					this->FramePlay = this->FramePlay->Next;
					this->FramePos++;
				}

				if( !this->FramePlay )
				{
					//sprintf( Buff, "#3: Count: %d  Orig: %d  New: %d  Curr: %d", this->FrameCount, OrigPos, NewPos, this->FramePos );

					//MessageBox( NULL, Buff, "Debug", MB_OK );
				}
			}
		}
		else
		{
			this->Shuttle += this->getParamFloat( FF_PARAM_SHUTTLE );

			while( fabsf( this->Shuttle ) > 1.0f )
			{
				if( this->Shuttle < 0.0f )
				{
					this->FramePlay = this->FramePlay->Prev;

					if( this->FramePlay == NULL )
					{
						this->FramePlay = this->FrameTail;
						this->FramePos  = this->FrameCount - 1;
					}
					else
					{
						this->FramePos--;
					}

					this->Shuttle += 1.0f;
				}
				else
				{
					this->FramePlay = this->FramePlay->Next;

					if( this->FramePlay == NULL )
					{
						this->FramePlay = this->FrameHead;
						this->FramePos  = 0;
					}
					else
					{
						this->FramePos++;
					}

					this->Shuttle -= 1.0f;
				}
			}
		}

		if( this->FramePlay == NULL )
		{
			return( FF_FAIL );
		}

		memcpy( pFrame.Frame, &this->FramePlay->Frame, this->FrameSize );
	}

	return( FF_SUCCESS );
}
