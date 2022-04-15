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

#define BUFFER_SIZE		( 640 * 480 * 4 * 2 )

StreamEncoderTCP::StreamEncoderTCP( const VideoInfoStruct &pSrcInfo )
: StreamEncoder( pSrcInfo, true )
{
	mSocket     = SOCKET_ERROR;
	mPortStatus = PORT_NOT_CONNECTED;

	if( !mThreaded )
	{
		portOpen();
	}
	
	startThread();
}

StreamEncoderTCP::~StreamEncoderTCP(void)
{
	stopThread();

	if( !mThreaded )
	{
		portClose();
	}

	deallocBuffers();
}

void StreamEncoderTCP::allocBuffers( void )
{
	const unsigned long PacketDataSize = FRAME_DATA_SIZE + compressBound( mFrameConvert->getOutputFrame()->getFrameByteSize() );

	if( ( mPktBuffer1 = reinterpret_cast<FrameData *>( new BYTE[ PacketDataSize ] ) ) == NULL )
	{
		MessageBox( NULL, L"Out of memory", L"fugStreamReceive", MB_OK );

		return;
	}

	if( ( mPktBuffer2 = reinterpret_cast<FrameData *>( new BYTE[ PacketDataSize ] ) ) == NULL )
	{
		MessageBox( NULL, L"Out of memory", L"fugStreamReceive", MB_OK );

		return;
	}

	mFrameBuffer1 = &mPktBuffer1->Data;
	mFrameBuffer2 = &mPktBuffer2->Data;

	this->mPktBuffer1->FrameNum = 0;
	
	this->mPktBuffer1->BitDepth = (BYTE)ffFrameBuffer::getFreeFrameDepthFromBits( mFrameConvert->getOutputFrame()->getBitDepth() );
	this->mPktBuffer1->Width    = (unsigned short)mFrameConvert->getOutputFrame()->getWidth();
	this->mPktBuffer1->Height   = (unsigned short)mFrameConvert->getOutputFrame()->getHeight();

	memcpy( this->mPktBuffer2, this->mPktBuffer1, FRAME_DATA_SIZE );
}

void StreamEncoderTCP::deallocBuffers( void )
{
	SAFE_DELETE( mPktBuffer1 );
	SAFE_DELETE( mPktBuffer2 );

	mFrameBuffer1 = mFrameBuffer2 = NULL;
}

void StreamEncoderTCP::portOpen( void )
{
	SockAddr			 LocalHost;
	const u_long		 BufferSize = BUFFER_SIZE;
	int					 Result;

	if( ( mSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) == SOCKET_ERROR )
	{
		MessageBox( NULL, L"Couldn't open socket", L"StreamSend", MB_OK );

		return;
	}

	memset( &LocalHost, 0, sizeof(LocalHost) );

	LocalHost.sin_addr.s_addr = htonl( INADDR_ANY );
	LocalHost.sin_family      = AF_INET;
	LocalHost.sin_port        = htons( 0 );

	if( ( Result = bind( mSocket, (struct sockaddr *)&LocalHost, sizeof(LocalHost) ) ) == SOCKET_ERROR )
	{
		wchar_t		Buff[ 256 ];

		swprintf_s( Buff, L"Couldn't bind to socket: %d", h_errno );

		MessageBox( NULL, Buff, L"StreamSend", MB_OK );

		return;
	}

	u_long NonBlock = true;

	ioctlsocket( mSocket, FIONBIO, &NonBlock );

	setsockopt( mSocket, SOL_SOCKET, SO_SNDBUF, (char *)&BufferSize, sizeof(BufferSize) );
}

void StreamEncoderTCP::portClose( void )
{
	if( mSocket != SOCKET_ERROR )
	{
		closesocket( mSocket );

		mSocket = SOCKET_ERROR;

		mPortStatus = PORT_NOT_CONNECTED;
	}
}

bool StreamEncoderTCP::portConnect( void )
{
	timeval			 Timer;
	fd_set			 SetRead;
	fd_set			 SetWrite;
	fd_set			 SetExcept;
	int				 SelectResult;

	if( mPortStatus == PORT_NOT_CONNECTED )
	{
		if( connect( mSocket, (sockaddr *)&mDstAddr, sizeof( mDstAddr ) ) == SOCKET_ERROR )
		{
			int Error = WSAGetLastError();

			if( Error == WSAEWOULDBLOCK || Error == WSAEALREADY )
			{
				mPortStatus = PORT_CONNECTING;
			}
			else if( Error == WSAEISCONN )
			{
				mPortStatus = PORT_CONNECTED;
			}
			else
			{
				return( false );
			}
		}
	}

	Timer.tv_sec  = 0;
	Timer.tv_usec = 100;

	FD_ZERO( &SetRead );
	FD_ZERO( &SetWrite );
	FD_ZERO( &SetExcept );

	FD_SET( mSocket, &SetRead );
	FD_SET( mSocket, &SetWrite );
	FD_SET( mSocket, &SetExcept );

	SelectResult = select( 0, &SetRead, &SetWrite, &SetExcept, &Timer );

	if( SelectResult == 0 )
	{
		return( false );
	}
	else if( SelectResult == SOCKET_ERROR )
	{
		//Instance->mRunThread = false;

		return( false );
	}

	if( FD_ISSET( mSocket, &SetRead ) )
	{
		portClose();

		portOpen();

		return( false );
	}
	
	if( FD_ISSET( mSocket, &SetWrite ) )
	{
		if( mPortStatus == PORT_CONNECTING )
		{
			mPortStatus = PORT_CONNECTED;
		}

		return( true );
	}

	if( FD_ISSET( mSocket, &SetExcept ) )
	{
		int Error = WSAGetLastError();

		if( Error == WSAEWOULDBLOCK )
		{
			portClose();

			portOpen();

			return( false );
		}
		else if( mPortStatus == PORT_CONNECTING )
		{
			mPortStatus = PORT_NOT_CONNECTED;
		}
	}

	return( false );
}

bool StreamEncoderTCP::portWrite( const BYTE *pBuffer )
{
	FrameData		*PktBuffer = ( pBuffer == mFrameBuffer1 ? mPktBuffer1 : mPktBuffer2 );

	DWORD	SrcLen = mFrameConvert->getOutputFrame()->getFrameByteSize();
	DWORD	DstLen = compressBound( SrcLen );

	if( compress2( &PktBuffer->Data, &DstLen, pBuffer, SrcLen, Z_BEST_SPEED ) == Z_OK )
	{
		SrcLen = DstLen;
	}

	PktBuffer->DataSize = SrcLen;

	send( mSocket, (char *)PktBuffer, FRAME_DATA_SIZE + SrcLen, 0 );

	return( true );
}
