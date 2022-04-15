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

#include "StreamDecoder.h"
#include <assert.h>

StreamDecoder::StreamDecoder( const VideoInfoStruct &pDstInfo, bool pThreaded )
: mThreaded( pThreaded )
{
	ffFrameBuffer		*Buffer = ffFrameBufferFactory::getInstance( &pDstInfo );

	mFrameConvert = NULL;
	mThreadHandle = NULL;
	mRunThread = true;
	mCurrPort = 0;
	mAdaptiveBuffer = false;

	mBuildBuffer = NULL;
	mPktBuffer = NULL;

	mConfigEvent = CreateEvent( NULL, FALSE, TRUE, NULL );
	mFrameEvent  = CreateEvent( NULL, FALSE, TRUE, NULL );
	mNewFrameEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	memcpy( &mDstVidInfo, &pDstInfo, sizeof( VideoInfoStruct ) );

	mFrameBuffer1 = new BYTE[ Buffer->getFrameByteSize() ];
	mFrameBuffer2 = new BYTE[ Buffer->getFrameByteSize() ];

	mWritePtr = mFrameBuffer1;
	mReadPtr  = mFrameBuffer2;

	SAFE_DELETE( Buffer );
}

StreamDecoder::~StreamDecoder( void )
{
	if( this->mNewFrameEvent != NULL )
	{
		CloseHandle( mNewFrameEvent );

		mNewFrameEvent = NULL;
	}

	if( this->mConfigEvent != NULL )
	{
		CloseHandle( mConfigEvent );

		mConfigEvent = NULL;
	}

	if( this->mFrameEvent != NULL )
	{
		CloseHandle( mFrameEvent );

		mFrameEvent = NULL;
	}

	SAFE_DELETE( mFrameBuffer1 );
	SAFE_DELETE( mFrameBuffer2 );

	SAFE_DELETE( mFrameConvert );
}

void StreamDecoder::startThread( void )
{
	mThreadArgs.mInstance = this;

	if( mThreaded && mThreadHandle == NULL )
	{
		mThreadHandle = CreateThread(
			NULL,                        // default security attributes
			0,                           // use default stack size
			(LPTHREAD_START_ROUTINE)&StreamDecoder::ThreadFunc,                  // thread function
			&mThreadArgs,                // argument to thread function
			0,                           // use default creation flags
			&this->mThreadId);                // returns the thread identifier

	   // Check the return value for success.

		if( mThreadHandle == NULL )
		{
			//MessageBox( NULL, L"CreateThread failed.", L"fugStreamReceive", MB_OK );
		}
	}
}

void StreamDecoder::stopThread( void )
{
	if( mThreadHandle != NULL )
	{
		if( mRunThread )
		{
			mRunThread = false;

			WaitForSingleObject( mThreadHandle, INFINITE );
		}

		CloseHandle( mThreadHandle );

		mThreadHandle = NULL;
	}
}

void StreamDecoder::setSrcAddr( struct sockaddr_in *pAddr )
{
	if( pAddr->sin_port == mCurrPort )
	{
		return;
	}

	if( !mThreaded || WaitForSingleObject( mConfigEvent, INFINITE ) == WAIT_OBJECT_0 )
	{
		portClose();

		portOpen( pAddr->sin_port );

		if( mThreaded )
		{
			SetEvent( mConfigEvent );
		}
	}
}

BYTE *StreamDecoder::lockReadBuffer( DWORD pTimeout )
{
	if( !mThreaded )
	{
		if( portConnect( pTimeout ) )
		{
			portRead( mWritePtr );

			ResetEvent( mNewFrameEvent );
		}
	}

	if( !mThreaded || WaitForSingleObject( mFrameEvent, INFINITE ) == WAIT_OBJECT_0 )
	{
		return( mReadPtr );
	}

	return( NULL );
}

void StreamDecoder::unlockReadBuffer( void )
{
	if( mThreaded )
	{
		SetEvent( mFrameEvent );
	}
}

bool StreamDecoder::hasNewFrame( void )
{
	if( !mThreaded )
	{
		if( portConnect( 0 ) )
		{
			portRead( mWritePtr );
		}
	}

	return( WaitForSingleObject( mNewFrameEvent, 0 ) == WAIT_OBJECT_0 );
}

bool StreamDecoder::waitForNewFrame( DWORD pTimeout )
{
	if( !mThreaded )
	{
		if( portConnect( pTimeout ) )
		{
			portRead( mWritePtr );
		}
	}

	return( WaitForSingleObject( mNewFrameEvent, pTimeout ) == WAIT_OBJECT_0 );
}

void StreamDecoder::swapBuffers( void )
{
	if( mFrameConvert != NULL && mBuildBuffer != NULL && mWritePtr != NULL )
	{
		mFrameConvert->convert( mBuildBuffer, mWritePtr );
	}

	// Switch Read/Write Pointers

	if( !mThreaded || WaitForSingleObject( mFrameEvent, 0 ) == WAIT_OBJECT_0 )
	{
		if( mWritePtr == mFrameBuffer1 )
		{
			mWritePtr = mFrameBuffer2;
			mReadPtr  = mFrameBuffer1;
		}
		else
		{
			mWritePtr = mFrameBuffer1;
			mReadPtr  = mFrameBuffer2;
		}

		if( mThreaded )
		{
			SetEvent( mFrameEvent );
		}

		SetEvent( mNewFrameEvent );
	}
}

DWORD WINAPI StreamDecoder::ThreadFunc( StreamDecoderThreadArgs *pArgs )
{
	StreamDecoder		*Instance = pArgs->mInstance;

	while( Instance->mRunThread )
	{
		if( WaitForSingleObject( Instance->mConfigEvent, INFINITE ) != WAIT_OBJECT_0 )
		{
			continue;
		}

		if( !Instance->mRunThread )
		{
			continue;
		}

		if( Instance->portConnect() )
		{
			Instance->portRead( (BYTE *)Instance->mPktBuffer );
		}

		SetEvent( Instance->mConfigEvent );
	}

	Instance->portClose();

	return( 0 );
}

UINT StreamDecoder::getRecvWidth( void )
{
	return( mFrameConvert == NULL ? 0 : mFrameConvert->getInputFrame()->getWidth() );
}

UINT StreamDecoder::getRecvHeight( void )
{
	return( mFrameConvert == NULL ? 0 : mFrameConvert->getInputFrame()->getHeight() );
}

UINT StreamDecoder::getRecvDepth( void )
{
	return( mFrameConvert == NULL ? 0 : mFrameConvert->getInputFrame()->getBitDepth() );
}

UINT StreamDecoder::getRecvOrientation( void )
{
	return( 1 );
}

UINT StreamDecoder::getFrameNumber( void )
{
	return( mCurrFrameNum );
}
