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

#include "StreamDecoderTCP.h"

#include "StreamDecoderUDP.h"

#define BUFFER_SIZE		( 640 * 480 * 4 * 2 )

StreamDecoderTCP::StreamDecoderTCP( const VideoInfoStruct &pDstInfo )
: StreamDecoder( pDstInfo, true )
{
	WSADATA		wsaData;
	int			Result;

	mListenSocket = SOCKET_ERROR;
	mSocket = SOCKET_ERROR;
	mGotHeader = FALSE;

	mPktBuffer = (FrameData *)new BYTE[ BUFFER_SIZE ];		// Maximum 1920x1080 32bit

	if( ( Result = WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) ) != 0 )
	{
		MessageBox( NULL, L"Couldn't open WSock32 2.2", L"StreamDecoderTCP", MB_OK );

		return;
	}

	startThread();
}

StreamDecoderTCP::~StreamDecoderTCP(void)
{
	stopThread();

	portClose();

	deallocBuffers();

	WSACleanup();
}

void StreamDecoderTCP::allocBuffers( void )
{
}

void StreamDecoderTCP::deallocBuffers( void )
{
	SAFE_DELETE( mPktBuffer );
}

void StreamDecoderTCP::portOpen( const UINT pPort )
{
	struct sockaddr_in		 LocalHost;
	const u_long			 BufferSize = 2 * 1024 * 1024;
	u_long					 NonBlock = true;

	if( ( mListenSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) == SOCKET_ERROR )
	{
		MessageBox( NULL, L"Couldn't open socket", L"fugStreamReceive", MB_OK );

		return;
	}

	ioctlsocket( mListenSocket, FIONBIO, &NonBlock );

	//setsockopt( mSocket, SOL_SOCKET, SO_RCVBUF, (char *)&BufferSize, sizeof(BufferSize));

	memset( &LocalHost, 0, sizeof(LocalHost) );

	LocalHost.sin_addr.s_addr = htonl( INADDR_ANY );
	LocalHost.sin_family      = AF_INET;
	LocalHost.sin_port        = pPort;

	if( bind( mListenSocket, (struct sockaddr *)&LocalHost, sizeof(LocalHost) ) < 0 )
	{
		wchar_t		buff[ 256 ];

		swprintf_s( buff, 256, L"Couldn't bind to socket %d: %d", pPort, WSAGetLastError() );

		MessageBox( NULL, buff, L"fugStreamReceive", MB_OK );

		closesocket( mListenSocket );

		mListenSocket = SOCKET_ERROR;

		return;
	}

	if( listen( mListenSocket, 1 ) == SOCKET_ERROR )
	{
		closesocket( mListenSocket );

		mSocket = SOCKET_ERROR;

		return;
	}

	mCurrPort = pPort;
}

void StreamDecoderTCP::portClose( void )
{
	if( mListenSocket != SOCKET_ERROR )
	{
		closesocket( mListenSocket );

		mListenSocket = SOCKET_ERROR;
	}

	if( mSocket != SOCKET_ERROR )
	{
		closesocket( mSocket );

		mSocket = SOCKET_ERROR;
	}
}

bool StreamDecoderTCP::portConnect( DWORD pTimeout )
{
	timeval			 Timer;
	fd_set			 SetRead;
	int				 SelectResult;
	const u_long	 BufferSize = BUFFER_SIZE;

	Timer.tv_sec  = 0;
	Timer.tv_usec = 20;

	FD_ZERO( &SetRead );

	if( mListenSocket == SOCKET_ERROR )
	{
		Sleep( Timer.tv_usec );

		return( false );
	}
	else if( mSocket == SOCKET_ERROR )
	{
		FD_SET( mListenSocket, &SetRead );
	}
	else
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
	else if( FD_ISSET( mListenSocket, &SetRead ) )
	{
		if( ( mSocket = accept( mListenSocket, NULL, NULL ) ) != SOCKET_ERROR )
		{
			//u_long					 NonBlock = true;

			//ioctlsocket( mSocket, FIONBIO, &NonBlock );

			setsockopt( mSocket, SOL_SOCKET, SO_RCVBUF, (char *)&BufferSize, sizeof(BufferSize));
		}

		return( false );
	}
	else if( FD_ISSET( mSocket, &SetRead ) )
	{
		return( true );
	}

	return( false );
}

void StreamDecoderTCP::portRead( BYTE *pBuffer )
{
	if( !mGotHeader )
	{
		const int BytesToRead = FRAME_DATA_SIZE;
		const int BytesRead = recv( mSocket, (char *)pBuffer, BytesToRead, 0 );

		if( BytesRead == 0 || BytesRead == SOCKET_ERROR )
		{
			closesocket( mSocket );

			mSocket = SOCKET_ERROR;

			return;
		}

		if( BytesRead < BytesToRead )
		{
			return;
		}

		VideoInfoStruct		FrameInfo;

		mGotHeader = TRUE;
		mBytesReceived = 0;

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
	}

	if( mGotHeader && mBuildBuffer != NULL )
	{
		const int BytesToRead = mPktBuffer->DataSize;
		uLongf DstLen = mFrameConvert->getInputFrame()->getFrameByteSize();
		int BytesRead;

		if( BytesToRead == DstLen )
		{
			BytesRead = recv( mSocket, (char *)&mBuildBuffer[ mBytesReceived ], BytesToRead - mBytesReceived, 0 );
		}
		else
		{
			BytesRead = recv( mSocket, (char *)&pBuffer[ FRAME_DATA_SIZE + mBytesReceived ], BytesToRead - mBytesReceived, 0 );
		}

		if( BytesRead == SOCKET_ERROR )
		{
			closesocket( mSocket );

			mSocket = SOCKET_ERROR;

			return;
		}
		
		mBytesReceived += BytesRead;

		if( mBytesReceived < BytesToRead )
		{
			return;
		}

		mGotHeader = FALSE;

		if( uncompress( mBuildBuffer, &DstLen, &mPktBuffer->Data, mPktBuffer->DataSize ) == Z_OK )
		{
		}

		swapBuffers();

		mCurrFrameNum++;
	}
}

/*
StreamDecoderTCP::StreamDecoderTCP(void)
{
	mBuffer = 0L;
	mThreadHandle = 0L;
}

StreamDecoderTCP::~StreamDecoderTCP(void)
{
}

bool StreamDecoderTCP::setup( unsigned int pWidth, unsigned int pHeight, unsigned int pDepth, unsigned short pPort )
{
	if( this->mDecoder.setup( pWidth, pHeight, pDepth ) )
	{
		mArgs.pInstance = this;

		this->mBuffer = (FrameData *)new BYTE[ RECV_BUFF_SIZE ];
		this->mPort   = pPort;
		this->mRunThread = true;

		mThreadHandle = CreateThread( 
			NULL,                        // default security attributes 
			0,                           // use default stack size  
			(unsigned long (__stdcall *)(void *))this->ThreadFunc,                  // thread function 
			&mArgs,                // argument to thread function 
			0,                           // use default creation flags 
			&this->mThreadId);                // returns the thread identifier 
	 
	   // Check the return value for success. 
	 
	   if( mThreadHandle == NULL ) 
	   {
			MessageBox( NULL, "CreateThread failed.", "fugStreamReceive", MB_OK );

			return( false );
	   }

	   return( true );
	}

	return( false );
}

bool StreamDecoderTCP::teardown( void )
{
	if( mThreadHandle )
	{
		if( this->mRunThread )
		{
			this->mRunThread = false;

			WaitForSingleObject( mThreadHandle, INFINITE );
		}

		CloseHandle( mThreadHandle );

		mThreadHandle = NULL;
	}

	this->mDecoder.teardown();

	SAFE_DELETE( this->mBuffer );

	return( true );
}

void StreamDecoderTCP::setPort( const unsigned short pPort )
{
	this->mPort = pPort;
}

DWORD WINAPI StreamDecoderTCP::ThreadFunc( StreamTCPThreadArgs *pArgs )
{
	StreamDecoderTCP			*Instance = pArgs->pInstance;
	struct		sockaddr_in		 LocalHost;
	struct		sockaddr_in		 RemoteHost;
	int							 RemoteHostSize = sizeof( RemoteHost );
	SOCKET						 ListenSocket = SOCKET_ERROR;
	SOCKET						 AcceptSocket = SOCKET_ERROR;
	timeval						 Timer;
	fd_set						 SetRead;
	int							 SelectResult;
	bool						 GotData = false;
	u_long						 NonBlock = true;
	const u_long				 BufferSize = 2048 * 1024;

	// Main outer-loop

	while( Instance->mRunThread )
	{
		// (Re)Initialise Socket

		int				 CurrPort = Instance->mPort;

		if( ( ListenSocket = socket( AF_INET, SOCK_STREAM, 0 ) ) == INVALID_SOCKET )
		{
			MessageBox( NULL, "Couldn't open socket", "fugStreamReceive", MB_OK );

			Instance->mRunThread = false;

			return( FF_FAIL );
		}

		ioctlsocket( ListenSocket, FIONBIO, &NonBlock );

		memset( &LocalHost, 0, sizeof(LocalHost) );

		LocalHost.sin_addr.s_addr = htonl( INADDR_ANY );
		LocalHost.sin_family      = AF_INET;
		LocalHost.sin_port        = htons( CurrPort );

		if( bind( ListenSocket, (struct sockaddr *)&LocalHost, sizeof(LocalHost) ) == SOCKET_ERROR )
		{
			char		buff[ 256 ];

			sprintf_s( buff, 256, "Couldn't bind to socket %d: %d", CurrPort, WSAGetLastError() );

			MessageBox( NULL, buff, "fugStreamReceive", MB_OK );

			Instance->mRunThread = false;

			closesocket( ListenSocket );

			return( FF_FAIL );
		}

		if( listen( ListenSocket, 1 ) == SOCKET_ERROR )
		{
			char		buff[ 256 ];

			sprintf_s( buff, 256, "Couldn't listen on socket %d: %d", CurrPort, WSAGetLastError() );

			MessageBox( NULL, buff, "fugStreamReceive", MB_OK );

			Instance->mRunThread = false;

			closesocket( ListenSocket );

			return( FF_FAIL );
		}

		//MessageBox( 0, "I'm alive!", "ThreadFunc", MB_OK );

		while( Instance->mRunThread && CurrPort == Instance->mPort )
		{
			Timer.tv_sec  = 0;
			Timer.tv_usec = 10;

			FD_ZERO( &SetRead );

			if( AcceptSocket == SOCKET_ERROR )
			{
				FD_SET( ListenSocket, &SetRead );
			}
			else
			{
				FD_SET( AcceptSocket, &SetRead );
			}

			SelectResult = select( 0, &SetRead, NULL, NULL, &Timer );

			if( SelectResult == 0 )
			{
				// Timeout
			}
			else if( SelectResult == SOCKET_ERROR )
			{
				// Error

				Instance->mRunThread = false;
			}
			else if( FD_ISSET( ListenSocket, &SetRead ) )
			{
				if( ( AcceptSocket = accept( ListenSocket, (struct sockaddr *)&RemoteHost, &RemoteHostSize ) ) != SOCKET_ERROR )
				{
					ioctlsocket( AcceptSocket, FIONBIO, &NonBlock );

					setsockopt( AcceptSocket, SOL_SOCKET, SO_RCVBUF, (char *)&BufferSize, sizeof( BufferSize ) );
				}
			}
			else if( FD_ISSET( AcceptSocket, &SetRead ) )
			{
				int		RecvResult = recv( AcceptSocket, (char *)Instance->mBuffer, RECV_BUFF_SIZE, 0 );

				if( RecvResult == SOCKET_ERROR )
				{
					closesocket( AcceptSocket );

					AcceptSocket = SOCKET_ERROR;
				}
				else if( RecvResult < sizeof(FrameData) - 1 )
				{
					if( !GotData )
					{
						GotData = true;

						char		buff[ 256 ];

						sprintf_s( buff, 256, "%d < %d", RecvResult, sizeof(FrameData) - 1 );

						MessageBox( 0, buff, "fugStreamReceive", MB_OK );
					}
				}
				else
				{
					Instance->mDecoder.processData( Instance->mBuffer, RecvResult );
				}
			}
		}

		if( AcceptSocket != SOCKET_ERROR )
		{
			closesocket( AcceptSocket );

			AcceptSocket = SOCKET_ERROR;
		}

		closesocket( ListenSocket );

		ListenSocket = SOCKET_ERROR;
	}

	Instance->mRunThread = false;

	return( FF_SUCCESS );
}

*/