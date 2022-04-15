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

#include "StreamEncoderTCP.h"
#include <iostream>

StreamEncoderTCP::StreamEncoderTCP( const fugVideoInfo &pSrcInfo )
: StreamEncoder( pSrcInfo ), mSocket( INVALID_SOCKET ), mDstAddrInfo( 0 ), mConnected( false )
{
#ifdef WIN32
	WSADATA			WSAdata;

	if( ( mWSAresult = WSAStartup( MAKEWORD( 2, 2 ), &WSAdata ) ) != NO_ERROR )
	{
		printf( "Error at WSAStartup()\n" );
	}
#endif
}

StreamEncoderTCP::~StreamEncoderTCP(void)
{
	closePort();

#ifdef WIN32
	if( mWSAresult != NO_ERROR )
	{
		WSACleanup();
	}
#endif
}

bool StreamEncoderTCP::openPort( const size_t pSize )
{
	(void)pSize;

	if( mSocket != INVALID_SOCKET )
	{
		return( true );
	}

	boost::format	DstPortFormat( "%1%" );

	DstPortFormat % mDstPort;

	struct addrinfo		 Hints;
	struct addrinfo		*AddrInfo;

	memset( &Hints, 0, sizeof( Hints ) );

	Hints.ai_family   = AF_INET;
	Hints.ai_socktype = SOCK_STREAM;
	Hints.ai_protocol = IPPROTO_TCP;

	if( getaddrinfo( mDstAddr.c_str(), DstPortFormat.str().c_str(), &Hints, &AddrInfo ) != 0 )
	{
		return( false );
	}

	mDstAddrInfo = AddrInfo;

	if( ( mSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) == INVALID_SOCKET )
	{
		return( false );
	}

	int			SendBufferSize = 2 * ( mDstVidSize + FRAME_DATA_SIZE );

	if( setsockopt( mSocket, SOL_SOCKET, SO_SNDBUF, (char *)&SendBufferSize, sizeof( SendBufferSize ) ) != 0 )
	{
		closePort();

		return( false );
	}

#if defined( TARGET_OS_MAC )
	// Stop the SIGPIPE signal on OSX

	int					NoSigPipe = 1;

	setsockopt( mSocket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&NoSigPipe, sizeof( NoSigPipe ) );
#endif

#if 1
	unsigned long		NonBlocking = 1;

	if( ioctlsocket( mSocket, FIONBIO, &NonBlocking ) == SOCKET_ERROR )
	{
		closePort();

		return( false );
	}
#endif

	if( connect( mSocket, mDstAddrInfo->ai_addr, mDstAddrInfo->ai_addrlen ) == SOCKET_ERROR )
	{
#if defined( WIN32 )
		if( WSAGetLastError() != WSAEWOULDBLOCK )
#else
		if( errno != EINPROGRESS )
#endif
		{
			closePort();

			return( false );
		}
	}

	return( true );
}

void StreamEncoderTCP::closePort( void )
{
	if( mSocket != INVALID_SOCKET )
	{
		closesocket( mSocket );

		mSocket = INVALID_SOCKET;
	}

	mConnected = false;
}

void StreamEncoderTCP::encode( const void *pData, const size_t pSize )
{
	if( mFlags[ ENCODER_CHANGED_DEST ] || mFlags[ ENCODER_CHANGED_FORMAT ] )
	{
		closePort();
	}

	if( mPkt.size() != FRAME_DATA_SIZE + pSize )
	{
		mFlags.reset( ENCODER_CHANGED_DEST );
		mFlags.reset( ENCODER_CHANGED_FORMAT );
	}

	//if( mPkt.size() != FRAME_DATA_SIZE + pSize )
	{
		mPkt.resize( FRAME_DATA_SIZE + pSize );

		FrameData		FD;

		memset( &FD, 0, sizeof( FrameData ) );

		FD.Header      = htonl( HEADER_VALUE );
		FD.Version     = HEADER_VERSION;
		FD.Compression = mCompression;
		FD.BitDepth    = (uint8_t)mDstVidInfo.bitDepth;
		FD.Offset      = htonl( 0 );
		FD.Width       = htons( (uint16_t)mDstVidInfo.frameWidth );
		FD.Height      = htons( (uint16_t)mDstVidInfo.frameHeight );
		FD.Orientation = mDstVidInfo.orientation;
		FD.DataSize    = htonl( pSize );
		FD.FrameNum    = mFrameNum;

		memcpy( &mPkt[ 0 ], &FD, sizeof( FrameData ) );
	}

	if( !openPort( pSize ) )
	{
		mFrameNum++;

		return;
	}

	struct timeval		TimeOut;
	fd_set				FDwrite;
	fd_set				FDerror;
	int					Handles;

	FD_ZERO( &FDwrite );
	FD_SET( mSocket, &FDwrite );

	FD_ZERO( &FDerror );
	FD_SET( mSocket, &FDerror );

	TimeOut.tv_sec  = 0;
	TimeOut.tv_usec = 0;

	if( ( Handles = select( mSocket + 1, 0, &FDwrite, &FDerror, &TimeOut ) ) == SOCKET_ERROR )
	{
#if defined( _DEBUG )
		printf( "StreamEncoderTCP: TCP ENCODE SELECT ERROR\n" );
#endif

		mFrameNum++;

		return;
	}

	if( Handles == 0 )
	{
#if defined( _DEBUG )
		//printf( "StreamEncoderTCP: TCP CAN'T WRITE\n" );
#endif

		mFrameNum++;

		return;
	}

	if( FD_ISSET( mSocket, &FDerror ) )
	{
#if defined( _DEBUG )
		printf( "StreamEncoderTCP: TCP ERROR\n" );
#endif

		mFrameNum++;

		closePort();

		return;
	}

	FrameData		*FD = (FrameData *)&mPkt[ 0 ];

	FD->FrameNum = mFrameNum;

	memcpy( &FD->Data, pData, pSize );

	int		BytesSent;

	if( ( BytesSent = send( mSocket, &mPkt[ 0 ], mPkt.size(), 0 ) ) == SOCKET_ERROR )
	{
#ifdef WIN32
		int		LastError = WSAGetLastError();

		if( LastError != WSAEWOULDBLOCK )
#else
		if( errno != EINPROGRESS )
#endif
		{
#if defined( TARGET_OS_WIN ) && defined( _DEBUG )
			std::cerr << "StreamEncoderTCP: BytesSent == SOCKET_ERROR (errno=" << LastError << ")" << std::endl;
#endif

#if defined( TARGET_OS_MAC ) && defined( _DEBUG )
			printf( "StreamEncoderTCP: BytesSent == SOCKET_ERROR (errno=%d)\n", errno );
#endif
			closePort();
		}
		else
		{
#if defined( _DEBUG )
			printf( "StreamEncoderTCP: TCP SEND EINPROGRESS/WSAEWOULDBLOCK\n" );
#endif
		}
	}
	else
	{
#if defined( _DEBUG )
		if( BytesSent == int( mPkt.size() ) )
		{
			//printf( "TCP sent %d bytes\n", BytesSent );
		}
		else
		{
			printf( "StreamEncoderTCP: TCP SEND ERROR: %d != %d\n", BytesSent, int( mPkt.size() ) );

			closePort();
		}
#endif
	}

	mFrameNum++;
}
