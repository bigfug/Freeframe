/*

The MIT License

Copyright (c) 2007 Alex May - www.bigfug.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "StreamEncoder.h"

StreamEncoder::StreamEncoder( const VideoInfoStruct &pSrcInfo, bool pThreaded )
: mThreaded( pThreaded ), mWriteFreq( 0 )
{
	mFrameConvert = NULL;
	mThreadHandle = NULL;
	mRunThread = true;

	mFrameBuffer1 = mFrameBuffer2 = NULL;
	mPktBuffer1 = mPktBuffer2 = NULL;
	mWritePtr = NULL;
	mLastFrame = NULL;

	mWriteEvent  = CreateEvent( NULL, FALSE, FALSE, NULL );
	mConfigEvent = CreateEvent( NULL, FALSE, TRUE,  NULL );

	memcpy( &mSrcVidInfo, &pSrcInfo, sizeof( VideoInfoStruct ) );
}

void StreamEncoder::startThread( void )
{
	mThreadArgs.mInstance = this;

	if( mThreaded && mThreadHandle == NULL )
	{
		mThreadHandle = CreateThread( 
			NULL,                        // default security attributes 
			0,                           // use default stack size  
			(LPTHREAD_START_ROUTINE)&StreamEncoder::ThreadFunc,                  // thread function 
			&mThreadArgs,                // argument to thread function 
			0,                           // use default creation flags 
			&this->mThreadId);                // returns the thread identifier 
	 
	   // Check the return value for success. 
	 
		if( mThreadHandle == NULL )
		{
			MessageBox( NULL, L"CreateThread failed.", L"fugStreamReceive", MB_OK );
		}
	}
}

void StreamEncoder::stopThread( void )
{
	if( mThreadHandle != NULL )
	{
		if( mRunThread )
		{
			mRunThread = false;

			SetEvent( this->mWriteEvent );

			WaitForSingleObject( mThreadHandle, INFINITE );
		}

		CloseHandle( mThreadHandle );

		mThreadHandle = NULL;
	}
}

StreamEncoder::~StreamEncoder(void)
{
	if( this->mConfigEvent != NULL )
	{
		CloseHandle( mConfigEvent );

		mConfigEvent = NULL;
	}

	if( this->mWriteEvent != NULL )
	{
		CloseHandle( this->mWriteEvent );

		mWriteEvent = NULL;
	}

	SAFE_DELETE( mFrameConvert );

	SAFE_DELETE( mLastFrame );
}

void StreamEncoder::setDstAddr( struct sockaddr_in *pAddr )
{
	if( memcmp( &mDstAddr, pAddr, sizeof( struct sockaddr_in ) ) == 0 )
	{
		return;
	}

	if( !mThreaded || WaitForSingleObject( mConfigEvent, INFINITE ) == WAIT_OBJECT_0 )
	{
		memcpy( &mDstAddr, pAddr, sizeof( SockAddr ) );

		if( mThreaded )
		{
			SetEvent( mConfigEvent );
		}
	}
}

void StreamEncoder::setDstFormat( VideoInfoStruct &pDstInfo )
{
	if( mFrameConvert != NULL && memcmp( &pDstInfo, &mDstVidInfo, sizeof( VideoInfoStruct ) ) == 0 )
	{
		return;
	}

	if( !mThreaded || WaitForSingleObject( mConfigEvent, INFINITE ) == WAIT_OBJECT_0 )
	{
		SAFE_DELETE( mLastFrame );

		SAFE_DELETE( mFrameConvert );

		memcpy( &mDstVidInfo, &pDstInfo, sizeof( VideoInfoStruct ) );

		if( ( mFrameConvert = new FrameConvert( mSrcVidInfo, mDstVidInfo ) ) != NULL )
		{
			SAFE_DELETE( this->mFrameBuffer2 );
			SAFE_DELETE( this->mFrameBuffer1 );

			deallocBuffers();

			allocBuffers();

			if( ( mLastFrame = new BYTE[ mFrameConvert->getOutputFrame()->getFrameByteSize() ] ) == NULL )
			{
			}

			memset( mLastFrame, 0xDF, mFrameConvert->getOutputFrame()->getFrameByteSize() );

			this->mWritePtr = this->mFrameBuffer1;
		}

		if( mThreaded )
		{
			SetEvent( mConfigEvent );
		}
	}
}

void StreamEncoder::encode( VideoFrame pSrcData )
{
	if( mFrameConvert == NULL )
	{
		return;
	}

	if( mWritePtr == NULL && ( mThreadHandle != NULL || !portConnect() ) )
	{
		return;
	}

	if( mWritePtr == NULL )
	{
		return;
	}

	mFrameConvert->convert( (BYTE *)pSrcData.Frame, mWritePtr );

	int			 FirstLine = -1;
	int			 LastLine = -1;
/*
	const int	 Height = mFrameConvert->getOutputFrame()->getHeight();
	const int	 Width  = mFrameConvert->getOutputFrame()->getLineByteSize();
	BYTE		*Src1Ptr = mWritePtr;
	BYTE		*Src2Ptr = mLastFrame;

	for( int y = 0 ; y < Height ; y++ )
	{
		if( memcmp( Src1Ptr, Src2Ptr, Width ) != 0 )
		{
			if( FirstLine == -1 )
			{
				FirstLine = y;
			}
			else
			{
				LastLine = y;
			}
		}

		Src1Ptr += Width;
		Src2Ptr += Width;
	}

	if( FirstLine == -1 )
	{
		// No change

		return;
	}
*/

	if( memcmp( mLastFrame, mWritePtr, mFrameConvert->getOutputFrame()->getFrameByteSize() ) == 0 )
	{
		// No change

		return;
	}

	memcpy( mLastFrame, mWritePtr, mFrameConvert->getOutputFrame()->getFrameByteSize() );

	mFirstLine = FirstLine;
	mLastLine  = LastLine;

	if( mThreadHandle == NULL )
	{
		if( portWrite( mWritePtr ) )
		{
		}
		else
		{
		}
	}
	else
	{
		mWritePtr = NULL;

		SetEvent( mWriteEvent );
	}
}

DWORD WINAPI StreamEncoder::ThreadFunc( StreamEncoderThreadArgs *pArgs )
{
	StreamEncoder		*Instance = pArgs->mInstance;
	BYTE				*CurrWriteFrame = NULL;

	Instance->mWritePtr = Instance->mFrameBuffer1;

	Instance->portOpen();

	while( Instance->mRunThread )
	{
		if( WaitForSingleObject( Instance->mWriteEvent, INFINITE ) != WAIT_OBJECT_0 )
		{
			continue;
		}

		if( !Instance->mRunThread )
		{
			continue;
		}

		if( Instance->mFrameBuffer1 == NULL )
		{
			continue;
		}

		if( WaitForSingleObject( Instance->mConfigEvent, INFINITE ) != WAIT_OBJECT_0 )
		{
			continue;
		}

		if( CurrWriteFrame == NULL || ( CurrWriteFrame != Instance->mFrameBuffer1 && CurrWriteFrame != Instance->mFrameBuffer2 ) )
		{
			CurrWriteFrame = Instance->mFrameBuffer1;
		}

		Instance->mWritePtr = ( CurrWriteFrame == Instance->mFrameBuffer1 ? Instance->mFrameBuffer2 : Instance->mFrameBuffer1 );

		Instance->mPktBuffer1->FrameNum++;
		Instance->mPktBuffer2->FrameNum++;

		if( Instance->portConnect() )
		{
			Instance->portWrite( CurrWriteFrame );
		}

		CurrWriteFrame = ( CurrWriteFrame == Instance->mFrameBuffer1 ? Instance->mFrameBuffer2 : Instance->mFrameBuffer1 );

		SetEvent( Instance->mConfigEvent );
	}

	Instance->portClose();

	return( 0 );
}
