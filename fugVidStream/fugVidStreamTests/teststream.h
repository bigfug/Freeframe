#ifndef TESTSTREAM_H
#define TESTSTREAM_H

#include <QObject>
#include <QImage>

#include "fugVidStream.h"

class TestStream : public QObject
{
	Q_OBJECT

private slots:
	void initTestCase( void );

	void stream_data( void );
	void stream( void );

private:
	QImage				mSrcImg1;
	QImage				mSrcImg2;

	fugVideoInfo		mSrcVidInf;
	fugVideoInfo		mDstVidInf;

	QVector<uchar>		mSrcBuf1;
	QVector<uchar>		mSrcBuf2;
};


#endif // TESTSTREAM_H
