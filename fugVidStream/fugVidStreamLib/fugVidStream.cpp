// fugVidStream.cpp : Defines the entry point for the DLL application.
//

#include "fugVidStream.h"
#include "Stream.h"
#include <assert.h>
#include <list>
#include "StreamDecoder.h"
#include "StreamEncoder.h"
#include "StreamEncoderFactory.h"
#include "StreamDecoderFactory.h"

#if defined( WIN32 )
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ObjBase.h>

FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void) { return _iob; }

#endif

using namespace std;

class fugVidStream
{
protected:
	boost::mutex				 mMutex;
	list<StreamDecoder *>		 mDecoders;
	list<StreamEncoder *>		 mEncoders;

	fugVidStream( void )
	{
	}

	virtual ~fugVidStream( void )
	{
	}

public:
	static fugVidStream &getInstance( void )
	{
		static fugVidStream		Instance;

		return( Instance );
	}

	StreamEncoder *getStreamEncoder( fugStreamProtocol pProtocol, fugVideoInfo &pSrcInfo )
	{
		StreamEncoder		*Encoder = StreamEncoderFactory::getInstance( pProtocol, pSrcInfo );

		if( Encoder != 0 )
		{
			boost::lock_guard<boost::mutex>		Lock( mMutex );

			mEncoders.push_back( Encoder );
		}

		return( Encoder );
	}

	StreamDecoder *getStreamDecoder( fugStreamProtocol pProtocol, fugVideoInfo &pSrcInfo )
	{
		StreamDecoder		*Decoder = StreamDecoderFactory::getInstance( pProtocol, pSrcInfo );

		if( Decoder != 0 )
		{
			boost::lock_guard<boost::mutex>		Lock( mMutex );

			mDecoders.push_back( Decoder );
		}

		return( Decoder );
	}

	bool checkValidEncoder( StreamEncoder *pEncoder )
	{
		boost::lock_guard<boost::mutex>		Lock( mMutex );

		for( list<StreamEncoder *>::iterator it = mEncoders.begin() ; it != mEncoders.end() ; it++ )
		{
			if( pEncoder == *it )
			{
				return( true );
			}
		}

		return( false );
	}

	bool checkValidDecoder( StreamDecoder *pDecoder )
	{
		boost::lock_guard<boost::mutex>		Lock( mMutex );

		for( list<StreamDecoder *>::iterator it = mDecoders.begin() ; it != mDecoders.end() ; it++ )
		{
			if( pDecoder == *it )
			{
				return( true );
			}
		}

		return( false );
	}

	bool freeEncoder( StreamEncoder *pEncoder )
	{
		boost::lock_guard<boost::mutex>		Lock( mMutex );

		for( list<StreamEncoder *>::iterator it = mEncoders.begin() ; it != mEncoders.end() ; it++ )
		{
			if( pEncoder == *it )
			{
				mEncoders.erase( it );

				delete pEncoder;

				return( true );
			}
		}

		return( false );
	}

	bool freeDecoder( StreamDecoder *pDecoder )
	{
		boost::lock_guard<boost::mutex>		Lock( mMutex );

		for( list<StreamDecoder *>::iterator it = mDecoders.begin() ; it != mDecoders.end() ; it++ )
		{
			if( pDecoder == *it )
			{
				mDecoders.erase( it );

				delete pDecoder;

				return( true );
			}
		}

		return( false );
	}
};

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN32

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
	(void)hModule;
	(void)lpReserved;

	switch( ul_reason_for_call )
	{
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			break;
	}

	return( TRUE );
}

#endif

// For encoder
HVIDSTREAM fugGetEncoder( fugStreamProtocol pProtocol, fugVideoInfo *pSrcInfo )
{
	return( fugVidStream::getInstance().getStreamEncoder( pProtocol, *pSrcInfo ) );
}

void fugSetDestAddress( HVIDSTREAM pStream, const char *pAddr, const uint16_t pPort )
{
	((StreamEncoder *)pStream)->setDstAddr( pAddr, pPort );
}

void fugSetDestFormat( HVIDSTREAM pStream, const fugVideoInfo *pDstInfo, const fugStreamCompression pCompression, boost::uint16_t pLevel )
{
	((StreamEncoder *)pStream)->setDstFormat( *pDstInfo, pCompression, pLevel );
}

void fugEncode( HVIDSTREAM pStream, void *pSrcData )
{
	assert( pStream != 0 );
	assert( pSrcData != 0 );

	((StreamEncoder *)pStream)->encode( (const unsigned char *)pSrcData );
}

// For decoder
HVIDSTREAM fugGetDecoder( fugStreamProtocol pProtocol, fugVideoInfo *pSrcInfo )
{
	return( fugVidStream::getInstance().getStreamDecoder( pProtocol, *pSrcInfo ) );
}

void fugSetSourceAddress( HVIDSTREAM pStream, const uint16_t pPort )
{
	((StreamDecoder *)pStream)->setSrcAddr( pPort );
}

boost::uint8_t * fugLockReadBuffer( HVIDSTREAM pStream, boost::uint32_t pTimeout )
{
	return( ((StreamDecoder *)pStream)->lockReadBuffer( pTimeout ) );
}

void fugUnlockReadBuffer( HVIDSTREAM pStream )
{
	((StreamDecoder *)pStream)->unlockReadBuffer();
}

bool fugHasNewFrame( HVIDSTREAM pStream )
{
	return( ((StreamDecoder *)pStream)->hasNewFrame() );
}

bool fugWaitForNewFrame( HVIDSTREAM pStream, boost::uint32_t pTimeout )
{
	return( ((StreamDecoder *)pStream)->waitForNewFrame( pTimeout ) );
}

uint16_t fugGetRecvWidth( HVIDSTREAM pStream )
{
	return( ((StreamDecoder *)pStream)->getRecvWidth() );
}

uint16_t fugGetRecvHeight( HVIDSTREAM pStream )
{
	return( ((StreamDecoder *)pStream)->getRecvHeight() );
}

uint8_t fugGetRecvDepth( HVIDSTREAM pStream )
{
	return( ((StreamDecoder *)pStream)->getRecvDepth() );
}

uint8_t fugGetFrameNumber( HVIDSTREAM pStream )
{
	return( ((StreamDecoder *)pStream)->getFrameNumber() );
}

size_t fugGetCompressedFrameSize( HVIDSTREAM pStream )
{
	return( ((StreamDecoder *)pStream)->getCompressedFrameSize() );
}

void fugDeleteEncoder( HVIDSTREAM pStream )
{
	fugVidStream::getInstance().freeEncoder( (StreamEncoder *)pStream );
}

void fugDeleteDecoder( HVIDSTREAM pStream )
{
	fugVidStream::getInstance().freeDecoder( (StreamDecoder *)pStream );
}

void fugSetAdaptiveBuffer( HVIDSTREAM pStream, bool pIsAdaptive )
{
	((StreamDecoder *)pStream)->setAdaptiveBuffer( pIsAdaptive );
}

uint8_t fugGetRecvOrientation( HVIDSTREAM pStream )
{
	return( ((StreamDecoder *)pStream)->getRecvOrientation() );
}

size_t fugGetByteSize( fugVideoInfo *pVidInfo )
{
	size_t		ByteSize = pVidInfo->frameWidth * pVidInfo->frameHeight;

	switch( pVidInfo->bitDepth )
	{
		case FUG_VID_16BITVIDEO:
			ByteSize *= 2;
			break;

		case FUG_VID_24BITVIDEO:
			ByteSize *= 3;
			break;

		case FUG_VID_32BITVIDEO:
			ByteSize *= 4;
			break;
	}

	return( ByteSize );
}

#ifdef __cplusplus
}
#endif
