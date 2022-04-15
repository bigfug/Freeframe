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

StreamEncoderUDP::StreamEncoderUDP( const fugVideoInfo &pSrcInfo )
: StreamEncoder( pSrcInfo ), mDstAddrInfo( 0 ), mSendBuffRead( &mSendBuff1 ), mSendBuffWrite( &mSendBuff1 )
{
	mSendBuff1.mDataAvailable = mSendBuff2.mDataAvailable = false;

	mMaxPktSze = 65500;

	mThread = boost::thread( boost::ref( *this ) );
}

StreamEncoderUDP::~StreamEncoderUDP( void )
{
	mThread.interrupt();

	mThread.join();
}

void StreamEncoderUDP::operator()( void )
{
	bool							Running = true;
	SOCKET							Socket  = INVALID_SOCKET;

#ifdef WIN32
	WSADATA			WSAdata;

	if( ( mWSAresult = WSAStartup( MAKEWORD( 2, 2 ), &WSAdata ) ) != NO_ERROR )
	{
		printf( "Error at WSAStartup()\n" );
	}
#endif

#if defined( _DEBUG )
	std::cout << "UDP-ENC: thread starting\n";
#endif

	struct sockaddr_in saddr;
	struct in_addr iaddr;

	// set content of struct saddr and imreq to zero
	memset(&saddr, 0, sizeof(struct sockaddr_in));
	memset(&iaddr, 0, sizeof(struct in_addr));

	while( Running )
	{
		try
		{
			boost::unique_lock<boost::mutex>	Lock( mEncoderMutex );

			mCondition.wait( Lock );

			if( !mFlags[ ENCODER_HAVE_FORMAT ] || !mFlags[ ENCODER_HAVE_DEST ] )
			{
				continue;
			}

			if( mFlags[ ENCODER_CHANGED_DEST ] )
			{
#if defined( _DEBUG )
				std::cout << "UDP-ENC: changing destination\n";
#endif

				mFrameNum = 0;

				if( Socket != INVALID_SOCKET )
				{
					closesocket( Socket );

					Socket = INVALID_SOCKET;
				}

				// open a UDP socket
				if( ( Socket = socket(PF_INET, SOCK_DGRAM, 0 ) ) < 0 )
				{
					return;
				}

				saddr.sin_family = PF_INET;
				saddr.sin_port = htons(0); // Use the first free port
				saddr.sin_addr.s_addr = htonl(INADDR_ANY); // bind socket to any interface

				if( bind(Socket, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in)) < 0 )
				{
					return;
				}

				iaddr.s_addr = INADDR_ANY; // use DEFAULT interface

				// Set the outgoing interface to DEFAULT
				setsockopt( Socket, IPPROTO_IP, IP_MULTICAST_IF, (const char *)&iaddr,
						   sizeof(struct in_addr));

				unsigned char ttl = 3;
				unsigned char one = 1;

				// Set multicast packet TTL to 3; default TTL is 1
				setsockopt( Socket, IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&ttl,
						   sizeof(unsigned char));

				// send multicast traffic to myself too
				if( setsockopt( Socket, IPPROTO_IP, IP_MULTICAST_LOOP, (const char *)&one, sizeof(unsigned char) ) < 0 )
				{
					return;
				}

				// set destination multicast address
				saddr.sin_family = PF_INET;
				saddr.sin_addr.s_addr = inet_addr( "226.0.0.1" );
				saddr.sin_port = htons( mDstPort );

				mFlags.reset( ENCODER_CHANGED_DEST );
			}
		}
		catch( boost::thread_interrupted )
		{
#if defined( _DEBUG )
			std::cout << "UDP-ENC: thread interrupted\n";
#endif
			Running = false;

			continue;
		}
		catch( std::exception e )
		{
#if defined( _DEBUG )
			std::cout << "UDP-ENC: thread exception:" << e.what() << "\n";
#endif
			continue;
		}
		catch( ... )
		{
#if defined( _DEBUG )
			std::cout << "UDP-ENC: thread exception\n";
#endif

			continue;
		}

		if( Socket == INVALID_SOCKET )
		{
			continue;
		}

		while( mSendBuffRead->mDataAvailable )
		{
			//const size_t	DestByteSize    = fugGetByteSize( &mDstVidInfo );
			const size_t	SpacePerBuffer  = mMaxPktSze - FRAME_DATA_SIZE;

			size_t			SrcSnt = 0;
			int				Error  = 0;

			for( std::vector< std::vector<char> >::iterator it = mSendBuffRead->mBuffer.begin() ; Error != SOCKET_ERROR && it != mSendBuffRead->mBuffer.end() ; it++ )
			{
				std::vector<char>		&CurrBuff = *it;

				FrameData			*FrmDat = (FrameData *)&CurrBuff[ 0 ];

				FrmDat->FrameNum = mFrameNum;

				const size_t		 DataUsed = std::min<size_t>( SpacePerBuffer, mSendBuffRead->mDataSize - SrcSnt );

				SrcSnt += SpacePerBuffer;

				//cout << "UDP-ENC: sending " << ( FRAME_DATA_SIZE + DataUsed ) << " bytes\n";

				Error = sendto( Socket, &CurrBuff[ 0 ], FRAME_DATA_SIZE + DataUsed, 0, (struct sockaddr *)&saddr, sizeof( struct sockaddr_in ) );

				if( SrcSnt >= mSendBuffRead->mDataSize )
				{
					break;
				}
			}

			if( true )
			{
				boost::lock_guard<boost::mutex>		Lock( mEncoderMutex );

				mSendBuffRead->mDataAvailable = false;

				if( mSendBuffRead == &mSendBuff1 )
				{
					mSendBuffRead = &mSendBuff2;
				}
				else
				{
					mSendBuffRead = &mSendBuff1;
				}
			}

			if( Error == SOCKET_ERROR )
			{
#if defined( _DEBUG )
				std::cout << "UDP-ENC: sendto: " << strerror( errno ) << "\n";
#endif
			}

			mFrameNum++;
		}
	}

	if( Socket != INVALID_SOCKET )
	{
		closesocket( Socket );

		Socket = INVALID_SOCKET;
	}

#ifdef WIN32
	if( mWSAresult != NO_ERROR )
	{
		WSACleanup();
	}
#endif

#if defined( _DEBUG )
	std::cout << "UDP-ENC: thread exiting\n";
#endif
}

bool StreamEncoderUDP::openPort( const size_t pSize )
{
	(void)pSize;

	//const size_t	DestByteSize    = fugGetByteSize( &mDstVidInfo );
	//const size_t	SpacePerBuffer  = mMaxPktSze - FRAME_DATA_SIZE;
	//const size_t	BuffersRequired = ( DestByteSize / SpacePerBuffer ) + ( DestByteSize % SpacePerBuffer != 0 ? 1 : 0 );

#if defined( _DEBUG )
	std::cout << "UDP-ENC: port opened\n";
#endif

	return( true );
}

void StreamEncoderUDP::closePort( void )
{
}

void StreamEncoderUDP::encode( const void *pData, const size_t pSize )
{
	const size_t	DestByteSize    = fugGetByteSize( &mDstVidInfo );
	const size_t	SpacePerBuffer  = mMaxPktSze - FRAME_DATA_SIZE;
	const size_t	BuffersRequired = ( DestByteSize / SpacePerBuffer ) + ( DestByteSize % SpacePerBuffer != 0 ? 1 : 0 );

	if( mFlags[ ENCODER_CHANGED_FORMAT ] )
	{
#if defined( _DEBUG )
		std::cout << "UDP-ENC: changing format\n";
#endif

		mFlags.reset( ENCODER_CHANGED_FORMAT );
	}

	if( mSendBuffWrite->mBuffer.size() != BuffersRequired )
	{
		size_t			Offset = 0;

		mSendBuffWrite->mBuffer.resize( BuffersRequired );

		for( BuffArray::iterator it = mSendBuffWrite->mBuffer.begin() ; it != mSendBuffWrite->mBuffer.end() ; it++ )
		{
			std::vector<char>		&CurrBuff = *it;

			CurrBuff.resize( mMaxPktSze );

			FrameData		*FrmDat = (FrameData *)&CurrBuff[ 0 ];

			FrmDat->Header      = htonl( HEADER_VALUE );
			FrmDat->Version     = HEADER_VERSION;
			FrmDat->Compression = mCompression;
			FrmDat->BitDepth    = (uint8_t)mDstVidInfo.bitDepth;
			FrmDat->Offset      = htonl( Offset );
			FrmDat->Width       = htons( (uint16_t)mDstVidInfo.frameWidth );
			FrmDat->Height      = htons( (uint16_t)mDstVidInfo.frameHeight );
			FrmDat->Orientation = (uint8_t)mDstVidInfo.orientation;
			FrmDat->DataSize    = htonl( pSize );

			Offset += SpacePerBuffer;
		}
	}

	mSendBuffWrite->mDataSize = pSize;

	const uint8_t	*SrcDat = (const uint8_t *)pData;
	size_t			 SrcRem = pSize;

	for( BuffArray::iterator it = mSendBuffWrite->mBuffer.begin() ; SrcRem > 0 && it != mSendBuffWrite->mBuffer.end() ; it++ )
	{
		std::vector<char>		&CurrBuff = *it;

		FrameData		*FrmDat = (FrameData *)&CurrBuff[ 0 ];

		FrmDat->DataSize = htonl( pSize );

		const size_t	 DataUsed = std::min<size_t>( SpacePerBuffer, SrcRem );

		memcpy( &FrmDat->Data, SrcDat, DataUsed );

		SrcDat += DataUsed;
		SrcRem -= DataUsed;
	}

	if( !mSendBuffRead->mDataAvailable )
	{
		mSendBuffWrite->mDataAvailable = true;

		if( mSendBuffWrite == &mSendBuff1 )
		{
			mSendBuffWrite = &mSendBuff2;
		}
		else
		{
			mSendBuffWrite = &mSendBuff1;
		}

		mCondition.notify_one();
	}
	else
	{
		mSendBuffWrite->mDataAvailable = true;
	}
}
