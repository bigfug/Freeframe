#ifndef STREAMDECODER_H
#define STREAMDECODER_H

#include <QObject>
#include <QVector>
#include <QPixmap>
#include <QImage>

#include <fugVidStream.h>

class fugStreamDecoder : public QObject
{
	Q_OBJECT

public:
	explicit fugStreamDecoder( fugStreamProtocol pProtocol, const fugVideoInfo &pDstInf, int pChannel, QObject *pParent = 0 );

	virtual ~fugStreamDecoder( void );

	QPixmap pixmap( void );

	QImage image( void );

	fugVideoInfo videoinfo( void );

	HVIDSTREAM decoder( void );

	const QVector<uint8_t> &buffer( void )
	{
		return( mRcvVidBuf );
	}

signals:
	void frameReceived( void );

public slots:
	void decoderThread( void );

	void endDecoder( void );

	void setFPS( int pFPS )
	{
		mFPS = pFPS;
	}

private:
	HVIDSTREAM			 mDecoder;

	fugVideoInfo		 mDstVidInf;
	fugVideoInfo		 mRcvVidInf;

	QVector<uint8_t>	 mRcvVidBuf;

	bool				 mRunning;

	int					 mFPS;
};

#endif // STREAMDECODER_H
