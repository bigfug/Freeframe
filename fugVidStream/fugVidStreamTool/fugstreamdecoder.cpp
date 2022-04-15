#include "fugstreamdecoder.h"
#include <QDebug>
#include <QThread>

class SleeperThread : public QThread
{
public:
	static void msleep(unsigned long msecs)
	{
		QThread::msleep(msecs);
	}
};

fugStreamDecoder::fugStreamDecoder(fugStreamProtocol pProtocol, const fugVideoInfo &pDstInf, int pChannel, QObject *pParent ) :
	QObject( pParent ), mDecoder( 0 ), mDstVidInf( pDstInf ), mRunning( true ), mFPS( 0 )
{
	if( ( mDecoder = fugGetDecoder( pProtocol, &mDstVidInf ) ) == 0 )
	{
		return;
	}

	fugSetSourceAddress( mDecoder, FUG_BASE_PORT + pChannel );

	fugSetAdaptiveBuffer( mDecoder, true );
}

fugStreamDecoder::~fugStreamDecoder( void )
{
	if( mDecoder != 0 )
	{
		fugDeleteDecoder( mDecoder );

		mDecoder = 0;
	}
}

QPixmap fugStreamDecoder::pixmap()
{
	return( QPixmap() );
}

QImage fugStreamDecoder::image()
{
	if( mRcvVidBuf.empty() )
	{
		return( QImage() );
	}

	switch( mRcvVidInf.bitDepth )
	{
		case FUG_VID_16BITVIDEO:
			return( QImage( mRcvVidBuf.data(), mRcvVidInf.frameWidth, mRcvVidInf.frameHeight, QImage::Format_RGB16 ) );

		case FUG_VID_24BITVIDEO:
			return( QImage( mRcvVidBuf.data(), mRcvVidInf.frameWidth, mRcvVidInf.frameHeight, QImage::Format_RGB888 ) );

		case FUG_VID_32BITVIDEO:
			return( QImage( mRcvVidBuf.data(), mRcvVidInf.frameWidth, mRcvVidInf.frameHeight, QImage::Format_RGB32 ) );
	}

	return( QImage() );
}

fugVideoInfo fugStreamDecoder::videoinfo()
{
	return( mRcvVidInf );
}

HVIDSTREAM fugStreamDecoder::decoder()
{
	return( mDecoder );
}

void fugStreamDecoder::decoderThread( void )
{
	while( mRunning )
	{
		uint8_t		*FrameData = fugLockReadBuffer( mDecoder, 0 );

		if( FrameData == 0 )
		{
			if( mFPS > 0 )
			{
				SleeperThread::msleep( 1000 / mFPS );
			}
			else
			{
				SleeperThread::msleep( 1 );
			}

			continue;
		}

		//qDebug() << "Received frame" << fugGetFrameNumber( mDecoder );

		mRcvVidInf.frameWidth  = fugGetRecvWidth( mDecoder );
		mRcvVidInf.frameHeight = fugGetRecvHeight( mDecoder );
		mRcvVidInf.orientation = fugGetRecvOrientation( mDecoder );
		mRcvVidInf.bitDepth    = fugGetRecvDepth( mDecoder );

		mRcvVidBuf.resize( fugGetByteSize( &mRcvVidInf ) );

		memcpy( mRcvVidBuf.data(), FrameData, mRcvVidBuf.size() );

		fugUnlockReadBuffer( mDecoder );

		emit frameReceived();
	}
}

void fugStreamDecoder::endDecoder()
{
	mRunning = false;
}
