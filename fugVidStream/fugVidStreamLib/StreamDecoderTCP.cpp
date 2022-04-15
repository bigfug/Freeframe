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

StreamDecoderTCP::StreamDecoderTCP( const fugVideoInfo &pDstInfo )
	: StreamDecoder( pDstInfo ), mServerSocket( INVALID_SOCKET ), mClientSocket( INVALID_SOCKET ), mDat1( 0 ), mDat2( 0 ), mReadBuff( 0 ), mWriteBuff( 0 ), mReaderLocked( false )
{
#ifdef WIN32
	WSADATA			WSAdata;

	if( ( mWSAresult = WSAStartup( MAKEWORD( 2, 2 ), &WSAdata ) ) != NO_ERROR )
	{
		printf( "Error at WSAStartup()\n" );
	}
#endif

	mPkt1.resize( FRAME_DATA_SIZE, 0 );
	mPkt2.resize( FRAME_DATA_SIZE, 0 );

	memset( &mHdr1, 0, sizeof( mHdr1 ) );
	memset( &mHdr2, 0, sizeof( mHdr2 ) );
}

StreamDecoderTCP::~StreamDecoderTCP( void )
{
	closePort();

#ifdef WIN32
	if( mWSAresult != NO_ERROR )
	{
		WSACleanup();
	}
#endif
}

bool StreamDecoderTCP::openPort( void )
{
	if( !mFlags[ DECODER_HAVE_SOURCE ] )
	{
		return( false );
	}

	if( mFlags[ DECODER_CHANGED_SOURCE ] )
	{
		closePort();

		mFlags.reset( DECODER_CHANGED_SOURCE );
	}

	if( mServerSocket != INVALID_SOCKET )
	{
		return( true );
	}

	if( ( mServerSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) == INVALID_SOCKET )
	{
		return( false );
	}

	unsigned long		NonBlocking = 1;

	if( ioctlsocket( mServerSocket, FIONBIO, &NonBlocking ) == SOCKET_ERROR )
	{
		return( false );
	}

#if defined( TARGET_OS_MAC )
	// Stop the SIGPIPE signal on OSX

	int					NoSigPipe = 1;

	setsockopt( mServerSocket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&NoSigPipe, sizeof( NoSigPipe ) );
#endif

	sockaddr_in		SockAddr;

	SockAddr.sin_family      = AF_INET;
	SockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	SockAddr.sin_port        = htons( mCurrPort );

	if( bind( mServerSocket, (sockaddr *)&SockAddr, sizeof( SockAddr ) ) == SOCKET_ERROR )
	{
		closePort();

		return( false );
	}

	if( listen( mServerSocket, SOMAXCONN ) == SOCKET_ERROR )
	{
		closePort();

		return( false );
	}

	return( true );
}

void StreamDecoderTCP::closePort( void )
{
	if( mClientSocket != INVALID_SOCKET )
	{
		closesocket( mClientSocket );

		mClientSocket = INVALID_SOCKET;
	}

	if( mServerSocket != INVALID_SOCKET )
	{
		closesocket( mServerSocket );

		mServerSocket = INVALID_SOCKET;
	}

	mPkt1.resize( FRAME_DATA_SIZE, 0 );
	mPkt2.resize( FRAME_DATA_SIZE, 0 );

	mDat1 = 0;
	mDat2 = 0;

	mReadBuff = mWriteBuff = 0;
}

bool StreamDecoderTCP::hasNewFrame( void )
{
	return( waitForNewFrame( 0 ) );
}

bool StreamDecoderTCP::waitForNewFrame( uint32_t pTimeout )
{
	if( !openPort() )
	{
		return( false );
	}

	if( true )
	{
		const FrameData			&HDR = ( mReadBuff == 0 ? mHdr1 : mHdr2 );
		const size_t			&DAT = ( mReadBuff == 0 ? mDat1 : mDat2 );

		if( DAT >= FRAME_DATA_SIZE && HDR.DataSize != 0 && DAT == FRAME_DATA_SIZE + HDR.DataSize )
		{
			return( true );
		}
	}

	read( pTimeout );

	const FrameData			&HDR = ( mReadBuff == 0 ? mHdr1 : mHdr2 );
	const size_t			&DAT = ( mReadBuff == 0 ? mDat1 : mDat2 );

	if( DAT >= FRAME_DATA_SIZE && HDR.DataSize == 0 )
	{
		return( false );
	}

	if( DAT >= FRAME_DATA_SIZE && HDR.DataSize != 0 && DAT == FRAME_DATA_SIZE + HDR.DataSize )
	{
		return( true );
	}

	return( false );
}

uint8_t *StreamDecoderTCP::lockFrame( uint32_t pTimeout, size_t &pSize )
{
	pSize = 0;

	if( true )
	{
		const std::vector<char>	&PKT = ( mReadBuff == 0 ? mPkt1 : mPkt2 );
		const FrameData			&HDR = ( mReadBuff == 0 ? mHdr1 : mHdr2 );
		const size_t			&DAT = ( mReadBuff == 0 ? mDat1 : mDat2 );

		if( DAT >= FRAME_DATA_SIZE && HDR.DataSize != 0 && DAT == FRAME_DATA_SIZE + HDR.DataSize )
		{
			mReaderLocked = true;

			pSize = HDR.DataSize;

			return( (uint8_t *)&((FrameData *)&PKT[ 0 ])->Data );
		}
	}

	if( !waitForNewFrame( pTimeout ) )
	{
		return( 0 );
	}

	const std::vector<char>	&PKT = ( mReadBuff == 0 ? mPkt1 : mPkt2 );
	const FrameData			&HDR = ( mReadBuff == 0 ? mHdr1 : mHdr2 );
	const size_t			&DAT = ( mReadBuff == 0 ? mDat1 : mDat2 );

	if( DAT >= FRAME_DATA_SIZE && HDR.DataSize != 0 && DAT == FRAME_DATA_SIZE + HDR.DataSize )
	{
		mReaderLocked = true;

		pSize = HDR.DataSize;

		return( (uint8_t *)&((FrameData *)&PKT[ 0 ])->Data );
	}

	return( 0 );
}

void StreamDecoderTCP::freeFrame( void )
{
	if( mReadBuff == 0 )
	{
		mDat1 = 0;

		mReadBuff = 1;

		const FrameData			&HDR = ( mReadBuff == 0 ? mHdr1 : mHdr2 );
		const size_t			&DAT = ( mReadBuff == 0 ? mDat1 : mDat2 );

		if( DAT >= FRAME_DATA_SIZE && HDR.DataSize != 0 && DAT == FRAME_DATA_SIZE + HDR.DataSize )
		{
			mWriteBuff = 0;
		}
	}
	else
	{
		mDat2 = 0;

		mReadBuff = 0;

		const FrameData			&HDR = ( mReadBuff == 0 ? mHdr1 : mHdr2 );
		const size_t			&DAT = ( mReadBuff == 0 ? mDat1 : mDat2 );

		if( DAT >= FRAME_DATA_SIZE && HDR.DataSize != 0 && DAT == FRAME_DATA_SIZE + HDR.DataSize )
		{
			mWriteBuff = 1;
		}
	}

	mReaderLocked = false;
}

void StreamDecoderTCP::read( uint32_t pWait )
{
	struct timeval		TimeOut;
	fd_set				FDread;
	fd_set				FDerror;
	int					Handles;

	while( true )
	{
		FD_ZERO( &FDread );

		FD_SET( mServerSocket, &FDread );

		if( mClientSocket != INVALID_SOCKET )
		{
			FD_SET( mClientSocket, &FDread );
		}

		FD_ZERO( &FDerror );

		FD_SET( mServerSocket, &FDerror );

		if( mClientSocket != INVALID_SOCKET )
		{
			FD_SET( mClientSocket, &FDerror );
		}

		TimeOut.tv_sec  = pWait / 1000;
		TimeOut.tv_usec = pWait % 1000;

		if( ( Handles = select( std::max<SOCKET>( mServerSocket, mClientSocket ) + 1, &FDread, 0, &FDerror, &TimeOut ) ) == SOCKET_ERROR )
		{
#if defined( _DEBUG )
			printf( "StreamDecoderTCP: Handles == SOCKET_ERROR\n" );
#endif
			return;
		}

		if( Handles == 0 )
		{
#if defined( _DEBUG )
			//printf( "StreamDecoderTCP: Handles == 0\n" );
#endif

			return;
		}

		if( mClientSocket != INVALID_SOCKET )
		{
			if( FD_ISSET( mClientSocket, &FDread ) )
			{
				std::vector<char>	*PKT = ( mWriteBuff == 0 ? &mPkt1 : &mPkt2 );
				FrameData			*HDR = ( mWriteBuff == 0 ? &mHdr1 : &mHdr2 );
				size_t				*DAT = ( mWriteBuff == 0 ? &mDat1 : &mDat2 );

				int		CurrByteCount;
				size_t	CurrByteOffset = *DAT;

				while( ( CurrByteCount = recv( mClientSocket, &PKT->at( CurrByteOffset ), ( CurrByteOffset < FRAME_DATA_SIZE ? FRAME_DATA_SIZE - CurrByteOffset : ( FRAME_DATA_SIZE + HDR->DataSize ) - CurrByteOffset ), 0 ) ) != SOCKET_ERROR )
				{
					// If the socket is connection oriented and the remote side has shut down the connection gracefully,
					// and all data has been received, a recv will complete immediately with zero bytes received.

					if( CurrByteCount == 0 )
					{
						printf( "TCP: SOCKET CLOSED\n" );

						closePort();

						*DAT = 0;

						CurrByteOffset = 0;

						CurrByteCount = 0;

						break;
					}

					if( CurrByteOffset == 0 )
					{
						*DAT = 0;
					}

					// Do we have a whole header?

					if( CurrByteOffset + CurrByteCount < FRAME_DATA_SIZE )
					{
						CurrByteOffset += CurrByteCount;

						continue;
					}

					// Have we just received a new, whole header?

					if( CurrByteOffset < FRAME_DATA_SIZE && CurrByteOffset + CurrByteCount >= FRAME_DATA_SIZE )
					{
						//printf( "TCP: NEW FRAME\n" );

						memcpy( HDR, &PKT->at( 0 ), sizeof( FrameData ) );

						// Convert the header from network order to host order

						HDR->Header   = ntohl( HDR->Header );
						HDR->Width    = ntohs( HDR->Width );
						HDR->Height   = ntohs( HDR->Height );
						HDR->Offset   = ntohl( HDR->Offset );
						HDR->DataSize = ntohl( HDR->DataSize );

						if( HDR->Header != HEADER_VALUE || HDR->Version != HEADER_VERSION )
						{
							printf( "TCP: LOST SYNC\n" );

							closesocket( mClientSocket );

							mClientSocket = INVALID_SOCKET;

							*DAT = 0;

							CurrByteOffset = 0;

							CurrByteCount = 0;

							break;
						}

						if( PKT->size() < FRAME_DATA_SIZE + HDR->DataSize )
						{
							PKT->resize( FRAME_DATA_SIZE + HDR->DataSize );
						}
					}

					CurrByteOffset += CurrByteCount;

					if( CurrByteOffset < FRAME_DATA_SIZE + HDR->DataSize )
					{
						continue;
					}

					*DAT = CurrByteOffset;

					CurrByteOffset = 0;

					if( !mReaderLocked )
					{
						mReadBuff = mWriteBuff;
					}

					if( mWriteBuff == mReadBuff )
					{
						if( mWriteBuff == 0 )
						{
							mWriteBuff = 1;
						}
						else
						{
							mWriteBuff = 0;
						}

						PKT = ( mWriteBuff == 0 ? &mPkt1 : &mPkt2 );
						HDR = ( mWriteBuff == 0 ? &mHdr1 : &mHdr2 );
						DAT = ( mWriteBuff == 0 ? &mDat1 : &mDat2 );
					}
				}

				if( CurrByteOffset > 0 )
				{
					*DAT = CurrByteOffset;
				}

				if( true )
				{
					const FrameData			&RHDR = ( mReadBuff == 0 ? mHdr1 : mHdr2 );
					const size_t			 RDAT = ( mReadBuff == 0 ? mDat1 : mDat2 );

					if( RDAT >= FRAME_DATA_SIZE && RDAT == FRAME_DATA_SIZE + RHDR.DataSize )
					{
						mCurrFrameNum = RHDR.FrameNum;

						//printf( "TCP: Returning %d\n", mCurrFrameNum );

						fugVideoInfo	 VidInf;

						VidInf.bitDepth    = RHDR.BitDepth;
						VidInf.frameHeight = RHDR.Height;
						VidInf.frameWidth  = RHDR.Width;
						VidInf.orientation = RHDR.Orientation;

						if( memcmp( &mSrcVidInfo, &VidInf, sizeof( fugVideoInfo ) ) != 0 || mCompression != RHDR.Compression )
						{
							setSrcFormat( VidInf, (fugStreamCompression)RHDR.Compression );
						}
					}

					return;
				}
			}

			if( mClientSocket != INVALID_SOCKET && FD_ISSET( mClientSocket, &FDerror ) )
			{
				closesocket( mClientSocket );

				mClientSocket = INVALID_SOCKET;
			}
		}

		if( FD_ISSET( mServerSocket, &FDread ) )
		{
			if( mClientSocket != INVALID_SOCKET )
			{
				struct sockaddr_in	 TempAddr;
				socklen_t			 TempAddrLength = sizeof( TempAddr );
				SOCKET				 TempSocket;

				if( ( TempSocket = accept( mServerSocket, (sockaddr *)&TempAddr, &TempAddrLength ) ) != INVALID_SOCKET )
				{
#if defined( _DEBUG )
						printf( "StreamDecoderTCP: TempSocket == INVALID_SOCKET\n" );
#endif

					closesocket( TempSocket );
				}
			}
			else
			{
				mClientAddrLength = sizeof( mClientAddr );

				if( mClientSocket == INVALID_SOCKET && ( mClientSocket = accept( mServerSocket, (sockaddr *)&mClientAddr, &mClientAddrLength ) ) != INVALID_SOCKET )
				{
					int			RecvBufferSize = ( 1 * 1024 * 1024 );
					int			Result;

					if( ( Result = setsockopt( mClientSocket, SOL_SOCKET, SO_RCVBUF, (char *)&RecvBufferSize, sizeof( RecvBufferSize ) ) ) != SOCKET_ERROR )
					{
						unsigned long		NonBlocking = 1;

						Result = ioctlsocket( mClientSocket, FIONBIO, &NonBlocking );
					}

					if( Result == SOCKET_ERROR )
					{
						closesocket( mClientSocket );

						mClientSocket = INVALID_SOCKET;
					}
				}
			}
		}

		if( FD_ISSET( mServerSocket, &FDerror ) )
		{
			closePort();

#if defined( _DEBUG )
			printf( "StreamDecoderTCP: FD_ISSET( mServerSocket, &FDerror )\n" );
#endif

			return;
		}
	}
}
