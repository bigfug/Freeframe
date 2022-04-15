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

StreamDecoderUDP::StreamDecoderUDP( const fugVideoInfo &pDstInfo )
: StreamDecoder( pDstInfo ), mSocket( INVALID_SOCKET )
{
	mFrameRead = mFrameWrite = &mFrame1;

	mFrame1.mDataAvailable = mFrame2.mDataAvailable = false;

	mFrame1.mDataReceived = mFrame2.mDataReceived = 0;

	mMaxPktSze = 65500;
}

StreamDecoderUDP::~StreamDecoderUDP( void )
{
	mThread.interrupt();

	mThread.join();
}

void StreamDecoderUDP::operator()( void )
{
#if defined( _DEBUG )
	std::cout << "UDP-DEC: thread starting\n";
#endif

#ifdef WIN32
	WSADATA			WSAdata;

	if( ( mWSAresult = WSAStartup( MAKEWORD( 2, 2 ), &WSAdata ) ) != NO_ERROR )
	{
		printf( "Error at WSAStartup()\n" );
	}
#endif

#if defined( _DEBUG )
	std::cout << "UDP-DEC: initialising port " << mCurrPort << "\n";
#endif

	if( ( mSocket = socket( PF_INET, SOCK_DGRAM, IPPROTO_IP ) ) == INVALID_SOCKET )
	{
#if defined( _DEBUG )
		std::cout << "UDP-DEC: socket: " << strerror( errno ) << "\n";
#endif

		return;
	}

	unsigned long		NonBlocking = 1;

	if( ioctlsocket( mSocket, FIONBIO, &NonBlocking ) == SOCKET_ERROR )
	{
		closePort();

		return;
	}

/*			
#ifdef WIN32
	int		OptLen = sizeof( mMaxPktSze );

	if( getsockopt( mSocket, SOL_SOCKET, SO_MAX_MSG_SIZE, (char *)&mMaxPktSze, &OptLen ) != 0 )
	{
		closePort();

		return( false );
	}
#else
	mMaxPktSze = 65507;
#endif
*/

	mPkt.resize( mMaxPktSze );

	int			RecvBufferSize = ( 4 * 1024 * 1024 );

	if( setsockopt( mSocket, SOL_SOCKET, SO_RCVBUF, (char *)&RecvBufferSize, sizeof( RecvBufferSize ) ) != 0 )
	{
#if defined( _DEBUG )
		std::cout << "UDP-DEC: setsockopt: " << strerror( errno ) << "\n";
#endif

		closePort();

		return;
	}

	sockaddr_in		SockAddr;

	SockAddr.sin_family      = PF_INET;
	SockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	SockAddr.sin_port        = htons( mCurrPort );

	if( bind( mSocket, (sockaddr *)&SockAddr, sizeof( SockAddr ) ) == SOCKET_ERROR )
	{
#if defined( _DEBUG )
		std::cout << "UDP-DEC: bind: " << strerror( errno ) << "\n";
#endif

		closePort();

		return;
	}

	struct ip_mreq imreq;

	memset(&imreq, 0, sizeof(struct ip_mreq));

	imreq.imr_multiaddr.s_addr = inet_addr("226.0.0.1");
	imreq.imr_interface.s_addr = INADDR_ANY; // use DEFAULT interface

	if( setsockopt( mSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *)&imreq, sizeof( imreq ) ) != 0 )
	{
#if defined( _DEBUG )
		std::cout << "UDP-DEC: setsockopt: " << strerror( errno ) << "\n";
#endif

		closePort();

		return;
	}

#if defined( _DEBUG )
	std::cout << "UDP-DEC: port opened\n";
#endif

	try
	{
		while( !mThread.interruption_requested() )
		{
			struct timeval		TimeOut;
			fd_set				FDread;
			fd_set				FDerror;
			int					Handles;

			FD_ZERO( &FDread );
			FD_SET( mSocket, &FDread );

			FD_ZERO( &FDerror );
			FD_SET( mSocket, &FDerror );

			TimeOut.tv_sec  = 0;
			TimeOut.tv_usec = 100;

			if( ( Handles = select( mSocket + 1, &FDread, 0, &FDerror, &TimeOut ) ) == SOCKET_ERROR )
			{
				continue;
			}

			if( Handles == 0 )
			{
				continue;
			}

			if( FD_ISSET( mSocket, &FDerror ) )
			{
				continue;
			}

			if( !FD_ISSET( mSocket, &FDread ) )
			{
				continue;
			}

			int		BytesReceived = 0;

			while( !mThread.interruption_requested() && ( BytesReceived = recv( mSocket, &mPkt[ 0 ], mPkt.size(), 0 ) ) != SOCKET_ERROR )
			{
				if( BytesReceived < FRAME_DATA_SIZE )
				{
					continue;
				}

				FrameData		*FrmDat = (FrameData *)&mPkt[ 0 ];
				const size_t	 FrmSze = BytesReceived - FRAME_DATA_SIZE;

				// Convert the header from network order to host order

				FrmDat->Header   = ntohl( FrmDat->Header );
				FrmDat->Width    = ntohs( FrmDat->Width );
				FrmDat->Height   = ntohs( FrmDat->Height );
				FrmDat->Offset   = ntohl( FrmDat->Offset );
				FrmDat->DataSize = ntohl( FrmDat->DataSize );

				if( FrmDat->Header != HEADER_VALUE || FrmDat->Version != HEADER_VERSION )
				{
#if defined( _DEBUG )
					printf( "UDP INVALID HEADER\n" );
#endif

					closePort();

					return;
				}

#if defined( _DEBUG )
				//printf( "UDP received %d bytes for frame %d\n", FrmSze, FrmDat->FrameNum );
#endif

				fugVideoInfo	 VidInf;

				VidInf.bitDepth    = FrmDat->BitDepth;
				VidInf.frameHeight = FrmDat->Height;
				VidInf.frameWidth  = FrmDat->Width;
				VidInf.orientation = FrmDat->Orientation;

				if( memcmp( &mSrcVidInfo, &VidInf, sizeof( fugVideoInfo ) ) != 0 || mCompression != FrmDat->Compression )
				{
					setSrcFormat( VidInf, (fugStreamCompression)FrmDat->Compression );

					mFrameWrite->mDataReceived = 0;
				}

				if( FrmDat->FrameNum != mFrameWrite->mFrameNumber )
				{
					if( mFrameWrite->mDataReceived > 0 )
					{
#if defined( _DEBUG )
						printf( "UDP INCOMPLETE FRAME %d (%d/%d)\n", mFrameWrite->mFrameNumber, mFrameWrite->mDataReceived, mFrameWrite->mBuffer.size() );
#endif
					}

					mFrameWrite->mDataReceived = 0;

					mFrameWrite->mFrameNumber = FrmDat->FrameNum;
				}

				if( mFlags[ DECODER_VALID ] )
				{
					if( mFrameWrite->mBuffer.size() != FrmDat->DataSize )
					{
						mFrameWrite->mBuffer.resize( FrmDat->DataSize );
					}

					if( FrmDat->Offset + FrmSze <= mFrameWrite->mBuffer.size() )
					{
						memcpy( &mFrameWrite->mBuffer[ FrmDat->Offset ], &FrmDat->Data, FrmSze );
					}

					mFrameWrite->mDataReceived += FrmSze;

					if( mFrameWrite->mDataReceived >= mFrameWrite->mBuffer.size() )
					{
						//boost::lock_guard<boost::mutex>		Lock( mDecoderMutex );

						if( !mFrameRead->mDataAvailable )
						{
							mFrameWrite->mDataAvailable = true;

							if( mFrameWrite == &mFrame1 )
							{
								mFrameWrite = &mFrame2;
							}
							else
							{
								mFrameWrite = &mFrame1;
							}

							mCondition.notify_one();
						}
						else
						{
							mFrameWrite->mDataAvailable = true;
						}
					}
				}
			}

			if( BytesReceived == SOCKET_ERROR )
			{
#if defined( _DEBUG )
				std::cout << "UDP-DEC: recv: " << strerror( errno ) << "\n";
#endif
			}
		}
	}
	catch( boost::thread_interrupted )
	{
		std::cout << "UDP-DEC: thread interrupted\n";
	}
	catch( std::exception e )
	{
		std::cout << "UDP-DEC: thread exception:" << e.what() << "\n";
	}
	catch( ... )
	{
		std::cout << "UDP-DEC: thread exception\n";
	}

	if( mSocket != INVALID_SOCKET )
	{
		closesocket( mSocket );

		mSocket = INVALID_SOCKET;
	}

#ifdef WIN32
	if( mWSAresult != NO_ERROR )
	{
		WSACleanup();
	}
#endif

	std::cout << "UDP-DEC: thread exiting\n";
}

bool StreamDecoderUDP::openPort( void )
{
	if( !mFlags[ DECODER_HAVE_SOURCE ] )
	{
		return( false );
	}

	if( mFlags[ DECODER_CHANGED_SOURCE ] )
	{
		if( mThread.get_id() != boost::thread::id() )
		{
			mThread.interrupt();

			mThread.join();
		}

		mFlags.reset( DECODER_CHANGED_SOURCE );
	}

	if( mThread.get_id() == boost::thread::id() )
	{
		mThread = boost::thread( boost::ref( *this ) );
	}

	return( mThread.get_id() != boost::thread::id() );
}

void StreamDecoderUDP::closePort( void )
{
}

void StreamDecoderUDP::read( unsigned long pWait )
{
	(void)pWait;
}

bool StreamDecoderUDP::hasNewFrame( void )
{
	return( waitForNewFrame( 0 ) );
}

bool StreamDecoderUDP::waitForNewFrame( uint32_t pTimeout )
{
	if( !openPort() )
	{
		return( false );
	}

	if( mFrameRead->mDataAvailable )
	{
		mCurrFrameNum = mFrameRead->mFrameNumber;

		return( true );
	}

	if( pTimeout == INFINITE )
	{
		boost::unique_lock<boost::mutex>	Lock( mDecoderMutex );

		mCondition.wait( Lock );
	}
	else
	{
		//boost::unique_lock<boost::mutex>	Lock( mDecoderMutex );

		//mCondition.wait( Lock );
	}

	if( mFrameRead->mDataAvailable )
	{
		mCurrFrameNum = mFrameRead->mFrameNumber;

		return( true );
	}

	return( false );
}

uint8_t *StreamDecoderUDP::lockFrame( uint32_t pTimeout, size_t &pSize )
{
	pSize = 0;

	if( !waitForNewFrame( pTimeout ) )
	{
		return( 0 );
	}

	pSize = mFrameRead->mDataReceived;

	return( (uint8_t *)&mFrameRead->mBuffer[ 0 ] );
}

void StreamDecoderUDP::freeFrame( void )
{
	mFrameRead->mDataReceived  = 0;
	mFrameRead->mDataAvailable = false;

	if( mFrameRead == &mFrame1 )
	{
		mFrameRead = &mFrame2;
	}
	else
	{
		mFrameRead = &mFrame1;
	}
}
