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

#include "StreamDecoderUDP.h"

StreamDecoderUDP::StreamDecoderUDP( const VideoInfoStruct &pDstInfo )
: StreamDecoder( pDstInfo, true )
{
	mSocket = SOCKET_ERROR;

	mPktBuffer = (FrameData *)new BYTE[ 65536 ];

	startThread();
}

StreamDecoderUDP::~StreamDecoderUDP(void)
{
	stopThread();

	portClose();

	deallocBuffers();
}

void StreamDecoderUDP::allocBuffers( void )
{
}

void StreamDecoderUDP::deallocBuffers( void )
{
	SAFE_DELETE( mPktBuffer );
}

void StreamDecoderUDP::portOpen( const UINT pPort )
{
	struct sockaddr_in		 LocalHost;
	const u_long			 BufferSize = 2 * 1024 * 1024;
	u_long					 NonBlock = true;

	if( ( mSocket = socket( AF_INET, SOCK_DGRAM, 0 ) ) == SOCKET_ERROR )
	{
		MessageBox( NULL, L"Couldn't open socket", L"fugStreamReceive", MB_OK );

		return;
	}

	ioctlsocket( mSocket, FIONBIO, &NonBlock );

	setsockopt( mSocket, SOL_SOCKET, SO_RCVBUF, (char *)&BufferSize, sizeof(BufferSize));

	memset( &LocalHost, 0, sizeof(LocalHost) );

	LocalHost.sin_addr.s_addr = htonl( INADDR_ANY );
	LocalHost.sin_family      = AF_INET;
	LocalHost.sin_port        = htons( pPort );

	if( bind( mSocket, (struct sockaddr *)&LocalHost, sizeof(LocalHost) ) < 0 )
	{
		wchar_t		buff[ 256 ];

		swprintf_s( buff, 256, L"Couldn't bind to socket %d: %d", pPort, WSAGetLastError() );

		MessageBox( NULL, buff, L"fugStreamReceive", MB_OK );

		closesocket( mSocket );

		mSocket = SOCKET_ERROR;

		return;
	}

	mCurrPort = pPort;
}

void StreamDecoderUDP::portClose( void )
{
	if( mSocket != SOCKET_ERROR )
	{
		closesocket( mSocket );

		mSocket = SOCKET_ERROR;
	}
}

bool StreamDecoderUDP::portConnect( DWORD pTimeout )
{
	timeval			 Timer;
	fd_set			 SetRead;
	int				 SelectResult;

	Timer.tv_sec  = 0;
	Timer.tv_usec = 20;

	FD_ZERO( &SetRead );

	if( mSocket != SOCKET_ERROR )
	{
		FD_SET( mSocket, &SetRead );
	}

	SelectResult = select( 0, &SetRead, NULL, NULL, &Timer );

	if( SelectResult == 0 )
	{
		return( false );
	}
	else if( SelectResult == SOCKET_ERROR )
	{
		//Instance->mRunThread = false;

		return( false );
	}
	else if( FD_ISSET( mSocket, &SetRead ) )
	{
		return( true );
	}

	return( false );
}

void StreamDecoderUDP::portRead( BYTE *pBuffer )
{
	int BytesRead = recv( mSocket, (char *)pBuffer, 65536, 0 );
	UINT Return = 0;

	if( BytesRead == SOCKET_ERROR )
	{
		mRunThread = false;

		return;
	}
	
	if( BytesRead < FRAME_DATA_SIZE )
	{
		return;
	}

	VideoInfoStruct		FrameInfo;

	if( mPktBuffer->FrameNum != mCurrFrameNum )
	{
		swapBuffers();

		mCurrFrameNum = mPktBuffer->FrameNum;
	}

	memset( &FrameInfo, 0, sizeof( FrameInfo ) );

	FrameInfo.frameWidth  = mPktBuffer->Width;
	FrameInfo.frameHeight = mPktBuffer->Height;
	FrameInfo.bitDepth    = mPktBuffer->BitDepth;

	if( memcmp( &FrameInfo, &mSrcVidInfo, sizeof( FrameInfo ) ) != 0 )
	{
		memcpy( &mSrcVidInfo, &FrameInfo, sizeof( FrameInfo ) );

		SAFE_DELETE( mFrameConvert );
		SAFE_DELETE( mBuildBuffer );

		if( ( mFrameConvert = new FrameConvert( mSrcVidInfo, mDstVidInfo ) ) != NULL )
		{
			mBuildBuffer = new BYTE[ mFrameConvert->getInputFrame()->getFrameByteSize() ];
		}
	}

	if( mBuildBuffer != NULL )
	{
		const int			 BytesPerLine  = mFrameConvert->getInputFrame()->getLineByteSize();
		const int			 LinesReceived = ( BytesRead - FRAME_DATA_SIZE ) / BytesPerLine;

#ifdef USE_ZLIB
		DWORD			 BuffRemain = mFrameConvert->getInputFrame()->getFrameByteSize() - ( BytesPerLine * mPktBuffer->Line );

		if( uncompress( &mBuildBuffer[ BytesPerLine * mPktBuffer->Line ], &BuffRemain, &mPktBuffer->Data, BytesRead - FRAME_DATA_SIZE ) == Z_OK )
		{
		}
		else
		{
			memcpy( &mBuildBuffer[ BytesPerLine * mPktBuffer->Line ], &mPktBuffer->Data, BytesRead - FRAME_DATA_SIZE );
		}
#else
		memcpy( &mBuildBuffer[ BytesPerLine * mPktBuffer->Line ], &mPktBuffer->Data, LinesReceived * BytesPerLine );
#endif

		// If we've received the last part of the frame, assume it's done
/*
		if( mPktBuffer->Line + LinesReceived == mPktBuffer->Height )
		{
			swapBuffers();

			mCurrFrameNum++;
		}
*/
	}
}
