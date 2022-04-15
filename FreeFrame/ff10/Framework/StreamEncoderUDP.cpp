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

#include "StreamEncoderUDP.h"

StreamEncoderUDP::StreamEncoderUDP( const VideoInfoStruct &pSrcInfo )
: StreamEncoder( pSrcInfo, true )
{
	mSocket = SOCKET_ERROR;

	if( !mThreaded )
	{
		portOpen();
	}
	
	startThread();
}

StreamEncoderUDP::~StreamEncoderUDP(void)
{
	stopThread();

	if( !mThreaded )
	{
		portClose();
	}

	deallocBuffers();
}

void StreamEncoderUDP::allocBuffers( void )
{
	if( ( mPktBuffer1 = reinterpret_cast<FrameData *>( new BYTE[ PACKET_BUFFER_SIZE ] ) ) == NULL )
	{
		MessageBox( NULL, L"Out of memory", L"fugStreamReceive", MB_OK );

		return;
	}

	if( ( mPktBuffer2 = reinterpret_cast<FrameData *>( new BYTE[ PACKET_BUFFER_SIZE ] ) ) == NULL )
	{
		MessageBox( NULL, L"Out of memory", L"fugStreamReceive", MB_OK );

		return;
	}

	this->mPktBuffer1->FrameNum = 0;
	
	this->mPktBuffer1->BitDepth = (BYTE)ffFrameBuffer::getFreeFrameDepthFromBits( mFrameConvert->getOutputFrame()->getBitDepth() );
	this->mPktBuffer1->Width    = (unsigned short)mFrameConvert->getOutputFrame()->getWidth();
	this->mPktBuffer1->Height   = (unsigned short)mFrameConvert->getOutputFrame()->getHeight();

	memcpy( this->mPktBuffer2, this->mPktBuffer1, FRAME_DATA_SIZE );

	if( ( this->mFrameBuffer1 = new BYTE[ mFrameConvert->getOutputFrame()->getFrameByteSize() ] ) == NULL )
	{
		MessageBox( NULL, L"Out of memory", L"fugStreamReceive", MB_OK );

		return;
	}

	if( ( this->mFrameBuffer2 = new BYTE[ mFrameConvert->getOutputFrame()->getFrameByteSize() ] ) == NULL )
	{
		MessageBox( NULL, L"Out of memory", L"fugStreamReceive", MB_OK );

		return;
	}
}

void StreamEncoderUDP::deallocBuffers( void )
{
	SAFE_DELETE( mPktBuffer1 );
	SAFE_DELETE( mPktBuffer2 );

	SAFE_DELETE( mFrameBuffer1 );
	SAFE_DELETE( mFrameBuffer2 );
}

void StreamEncoderUDP::portOpen( void )
{
	SockAddr			 LocalHost;
	const u_long		 BufferSize = 2048 * 1024;
	int					 Result;
	BOOL				 OptFlag;

	if( ( mSocket = socket( AF_INET, SOCK_DGRAM, 0 ) ) == SOCKET_ERROR )
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

	// Allow broadcast

	OptFlag = TRUE;

	setsockopt( mSocket, SOL_SOCKET, SO_BROADCAST, (char *)&OptFlag, sizeof(OptFlag) );

	setsockopt( mSocket, SOL_SOCKET, SO_SNDBUF, (char *)&BufferSize, sizeof(BufferSize));
}

void StreamEncoderUDP::portClose( void )
{
	if( mSocket != SOCKET_ERROR )
	{
		closesocket( mSocket );

		mSocket = SOCKET_ERROR;
	}
}

bool StreamEncoderUDP::portConnect( void )
{
	return( true );
}

bool StreamEncoderUDP::portWrite( const BYTE *pBuffer )
{
	const unsigned int			 PACKET_SIZE = 60000;
	const unsigned int			 LINES_PER_PKT = ( PACKET_SIZE - FRAME_DATA_SIZE ) / mFrameConvert->getOutputFrame()->getLineByteSize();
	unsigned int				 LinesInPacket;
	const		DWORD			 BytesPerLine = mFrameConvert->getOutputFrame()->getLineByteSize();
	int							 SentBytes;
	DWORD						 SrcLen;

	for( unsigned int i = 0 ; i < mPktBuffer1->Height ; i += LINES_PER_PKT )
	{
		mPktBuffer1->Line = i;

		LinesInPacket = min( LINES_PER_PKT, mPktBuffer1->Height - i );

		SrcLen = BytesPerLine * LinesInPacket;

#ifdef USE_ZLIB
		DWORD	DstLen = PACKET_BUFFER_SIZE - FRAME_DATA_SIZE;

		if( compress( &mPktBuffer1->Data, &DstLen, pBuffer + ( BytesPerLine * i ), SrcLen ) == Z_OK )
		{
			SrcLen = DstLen;
		}
		else
		{
			memcpy( &mPktBuffer1->Data, pBuffer + ( BytesPerLine * i ), SrcLen );
		}
#else
		memcpy( &mPktBuffer1->Data, pBuffer + ( BytesPerLine * i ), SrcLen );
#endif
		SentBytes = sendto( mSocket, (char *)mPktBuffer1, FRAME_DATA_SIZE + SrcLen, 0, (sockaddr *)&mDstAddr, sizeof( mDstAddr ) );

		if( SentBytes == SOCKET_ERROR )
		{
			wchar_t		buff[ 256 ];

			swprintf_s( buff, 256, L"Sending %d bytes - Error: %d", FRAME_DATA_SIZE + ( BytesPerLine * LinesInPacket ), WSAGetLastError() );

			MessageBox( NULL, buff, L"StreamSend", MB_OK );
		}
		else if( SentBytes != FRAME_DATA_SIZE + SrcLen )
		{
			MessageBox( NULL, L"SentBytes incorrect", L"StreamSend", MB_OK );
		}
	}

	return( true );
}
