
#include "teststream.h"

#define QTEST_NO_SPECIALIZATIONS

#include <QtTest/QtTest>

#include <QVector>
#include <QRgb>
#include <QThread>

Q_DECLARE_METATYPE( fugStreamProtocol )
Q_DECLARE_METATYPE( fugVideoInfo )
Q_DECLARE_METATYPE( fugStreamCompression )
Q_DECLARE_METATYPE( QVector<uchar> )

uint16_t RGB888_to_RGB565 ( uchar r, uchar g, uchar b )
{
	uint16_t RGB565;

	RGB565 = (((r) >> 3) << 11)|(((g) >> 2) <<  5)|(((b) >> 3));

	return RGB565;
}

void convertImageTo16( const QImage &pImage, QVector<uchar> &pBuffer )
{
	pBuffer.resize( pImage.width() * pImage.height() * 2 );

	uchar		*DstPtr = pBuffer.data();

	for( int y = 0 ; y < pImage.height() ; y++ )
	{
		const QRgb	*SrcPtr = (const QRgb *)pImage.scanLine( y );

		for( int x = 0 ; x < pImage.width() ; x++, SrcPtr++, DstPtr += 2 )
		{
			QRgb		RGB32 = *SrcPtr;
			uint16_t	RGB16 = RGB888_to_RGB565( qRed( RGB32 ), qGreen( RGB32 ), qBlue( RGB32 ) );

			DstPtr[ 1 ] = (RGB16>>8)&0xff;
			DstPtr[ 0 ] = (RGB16>>0)&0xff;
		}
	}

}

void convertImageTo24( const QImage &pImage, QVector<uchar> &pBuffer )
{
	pBuffer.resize( pImage.width() * pImage.height() * 3 );

	uchar		*DstPtr = pBuffer.data();

	for( int y = 0 ; y < pImage.height() ; y++ )
	{
		const QRgb	*SrcPtr = (const QRgb *)pImage.scanLine( y );

		for( int x = 0 ; x < pImage.width() ; x++, SrcPtr++, DstPtr += 3 )
		{
			QRgb		RGB32 = *SrcPtr;

			DstPtr[ 0 ] = qRed( RGB32 );
			DstPtr[ 1 ] = qGreen( RGB32 );
			DstPtr[ 2 ] = qBlue( RGB32 );
		}
	}

}

void convertImageTo32( const QImage &pImage, QVector<uchar> &pBuffer )
{
	pBuffer.resize( pImage.width() * pImage.height() * 4 );

	uchar		*DstPtr = pBuffer.data();

	for( int y = 0 ; y < pImage.height() ; y++ )
	{
		const QRgb	*SrcPtr = (const QRgb *)pImage.scanLine( y );

		for( int x = 0 ; x < pImage.width() ; x++, SrcPtr++, DstPtr += 4 )
		{
			QRgb		RGB32 = *SrcPtr;

			DstPtr[ 0 ] = qRed( RGB32 );
			DstPtr[ 1 ] = qGreen( RGB32 );
			DstPtr[ 2 ] = qBlue( RGB32 );
			DstPtr[ 3 ] = qAlpha( RGB32 );
		}
	}
}

QImage convert16ToImage( const QSize &pSize, const QVector<uchar> &pBuffer )
{
	QImage			 ImgBuf( pSize, QImage::Format_ARGB32 );

	const uchar		*SrcPtr = pBuffer.data();

	for( int y = 0 ; y < pSize.height() ; y++ )
	{
		QRgb	*DstPtr = (QRgb *)ImgBuf.scanLine( y );

		for( int x = 0 ; x < pSize.width() ; x++, SrcPtr += 2 )
		{
			uchar		v1 = SrcPtr[ 1 ];
			uchar		v2 = SrcPtr[ 0 ];

			uint16_t	rgb = ( uint16_t( v1 ) << 8 ) | uint16_t( v2 );

			uint8_t		r = uint8_t( (rgb & 0xf800) >> 11 );
			uint8_t		g = uint8_t( (rgb & 0x07e0) >>  5 );
			uint8_t		b = uint8_t( (rgb & 0x001f) >>  0 );

			*DstPtr++ = qRgb( ( uint32_t( r ) * 527 + 23 ) >> 6, ( uint32_t( g ) * 259 + 33 ) >> 6, ( uint32_t( b ) * 527 + 23 ) >> 6 );
		}
	}

	return( ImgBuf );
}

QImage convert24ToImage( const QSize &pSize, const QVector<uchar> &pBuffer )
{
	QImage			 ImgBuf( pSize, QImage::Format_ARGB32 );

	const uchar		*SrcPtr = pBuffer.data();

	for( int y = 0 ; y < pSize.height() ; y++ )
	{
		QRgb	*DstPtr = (QRgb *)ImgBuf.scanLine( y );

		for( int x = 0 ; x < pSize.width() ; x++, SrcPtr += 3 )
		{
			*DstPtr++ = qRgb( SrcPtr[ 0 ], SrcPtr[ 1 ], SrcPtr[ 2 ] );
		}
	}

	return( ImgBuf );
}

QImage convert32ToImage( const QSize &pSize, const QVector<uchar> &pBuffer )
{
	QImage			 ImgBuf( pSize, QImage::Format_ARGB32 );

	const uchar		*SrcPtr = pBuffer.data();

	for( int y = 0 ; y < pSize.height() ; y++ )
	{
		QRgb	*DstPtr = (QRgb *)ImgBuf.scanLine( y );

		for( int x = 0 ; x < pSize.width() ; x++, SrcPtr += 4 )
		{
			*DstPtr++ = qRgba( SrcPtr[ 0 ], SrcPtr[ 1 ], SrcPtr[ 2 ], SrcPtr[ 3 ] );
		}
	}

	return( ImgBuf );
}

void TestStream::initTestCase()
{
	mSrcImg1 = QImage( "G:\\dev\\Projects\\freeframe\\freeframe\\fugVidStream\\fugVidStreamTests\\IMAG0178.jpg" );
	mSrcImg2 = QImage( "G:\\dev\\Projects\\freeframe\\freeframe\\fugVidStream\\fugVidStreamTests\\IMAG0180.jpg" );

	QVERIFY( !mSrcImg1.isNull() );
	QVERIFY( !mSrcImg2.isNull() );

	QCOMPARE( mSrcImg1.format(), QImage::Format_RGB32 );
	QCOMPARE( mSrcImg2.format(), QImage::Format_RGB32 );
}

void TestStream::stream_data( void )
{
	QTest::addColumn< QVector<uchar> >( "pSrcBuf1" );
	QTest::addColumn< QVector<uchar> >( "pSrcBuf2" );
	QTest::addColumn<fugVideoInfo>( "pSrcVidInf" );
	QTest::addColumn<fugVideoInfo>( "pDstVidInf" );
	QTest::addColumn<fugStreamProtocol>( "pProtocol" );
	QTest::addColumn<fugStreamCompression>( "pCompression" );
	QTest::addColumn<uint16_t>( "pLevel" );
	QTest::addColumn<QString>( "pFileNameBase" );

#if 1
	//-------------------------------------------------------------------------

	convertImageTo16( mSrcImg1, mSrcBuf1 );
	convertImageTo16( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_16BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight;
	mDstVidInf.bitDepth    = FUG_VID_16BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_UDP.16.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_UDP.16.clone";
	QTest::newRow( "STREAM_MEM.16.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_MEM.16.clone";
	QTest::newRow( "STREAM_TCP.16.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_TCP.16.clone";

	//-------------------------------------------------------------------------

	convertImageTo24( mSrcImg1, mSrcBuf1 );
	convertImageTo24( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_24BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight;
	mDstVidInf.bitDepth    = FUG_VID_24BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_TCP.24.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_TCP.24.clone";
	QTest::newRow( "STREAM_UDP.24.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_UDP.24.clone";
	QTest::newRow( "STREAM_MEM.24.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_MEM.24.clone";

	//-------------------------------------------------------------------------

	convertImageTo32( mSrcImg1, mSrcBuf1 );
	convertImageTo32( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_32BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight;
	mDstVidInf.bitDepth    = FUG_VID_32BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_TCP.32.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_TCP.32.clone";
	QTest::newRow( "STREAM_UDP.32.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_UDP.32.clone";
	QTest::newRow( "STREAM_MEM.32.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_MEM.32.clone";
#endif
	//-------------------------------------------------------------------------
#if 1
	convertImageTo16( mSrcImg1, mSrcBuf1 );
	convertImageTo16( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_16BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight;
	mDstVidInf.bitDepth    = FUG_VID_32BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_MEM.16.32.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_MEM.16.32.clone";
	QTest::newRow( "STREAM_TCP.16.32.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_TCP.16.32.clone";
	QTest::newRow( "STREAM_UDP.16.32.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_UDP.16.32.clone";

	//-------------------------------------------------------------------------

	convertImageTo24( mSrcImg1, mSrcBuf1 );
	convertImageTo24( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_24BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight;
	mDstVidInf.bitDepth    = FUG_VID_32BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_MEM.24.32.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_MEM.24.32.clone";
	QTest::newRow( "STREAM_TCP.24.32.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_TCP.24.32.clone";
	QTest::newRow( "STREAM_UDP.24.32.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_UDP.24.32.clone";
#endif

	//-------------------------------------------------------------------------
#if 1
	convertImageTo16( mSrcImg1, mSrcBuf1 );
	convertImageTo16( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_16BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth / 2;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight / 2;
	mDstVidInf.bitDepth    = FUG_VID_16BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_TCP.16.half" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_TCP.16.half";
	QTest::newRow( "STREAM_UDP.16.half" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_UDP.16.half";
	QTest::newRow( "STREAM_MEM.16.half" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_MEM.16.half";

	//-------------------------------------------------------------------------

	convertImageTo24( mSrcImg1, mSrcBuf1 );
	convertImageTo24( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_24BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth / 2;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight / 2;
	mDstVidInf.bitDepth    = FUG_VID_24BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_TCP.24.half" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_TCP.24.half";
	QTest::newRow( "STREAM_UDP.24.half" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_UDP.24.half";
	QTest::newRow( "STREAM_MEM.24.half" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_MEM.24.half";

	//-------------------------------------------------------------------------

	convertImageTo32( mSrcImg1, mSrcBuf1 );
	convertImageTo32( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_32BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth / 2;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight / 2;
	mDstVidInf.bitDepth    = FUG_VID_32BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_TCP.32.half" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_TCP.32.half";
	QTest::newRow( "STREAM_UDP.32.half" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_UDP.32.half";
	QTest::newRow( "STREAM_MEM.32.half" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_MEM.32.half";
#endif
	//-------------------------------------------------------------------------
#if 1
	convertImageTo16( mSrcImg1, mSrcBuf1 );
	convertImageTo16( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_16BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth * 1.5;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight * 1.5;
	mDstVidInf.bitDepth    = FUG_VID_16BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_TCP.16.inc" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_TCP.16.inc";
	QTest::newRow( "STREAM_UDP.16.inc" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_UDP.16.inc";
	QTest::newRow( "STREAM_MEM.16.inc" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_MEM.16.inc";

	//-------------------------------------------------------------------------

	convertImageTo24( mSrcImg1, mSrcBuf1 );
	convertImageTo24( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_24BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth * 1.5;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight * 1.5;
	mDstVidInf.bitDepth    = FUG_VID_24BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_TCP.24.inc" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_TCP.24.inc";
	QTest::newRow( "STREAM_UDP.24.inc" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_UDP.24.inc";
	QTest::newRow( "STREAM_MEM.24.inc" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_MEM.24.inc";

	//-------------------------------------------------------------------------

	convertImageTo32( mSrcImg1, mSrcBuf1 );
	convertImageTo32( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_32BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth * 1.5;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight * 1.5;
	mDstVidInf.bitDepth    = FUG_VID_32BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_TCP.32.inc" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_TCP.32.inc";
	QTest::newRow( "STREAM_UDP.32.inc" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_UDP.32.inc";
	QTest::newRow( "STREAM_MEM.32.inc" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_MEM.32.inc";
#endif
	//-------------------------------------------------------------------------
#if 1
	convertImageTo16( mSrcImg1, mSrcBuf1 );
	convertImageTo16( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_16BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth / 1.5;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight / 1.5;
	mDstVidInf.bitDepth    = FUG_VID_16BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_TCP.16.dec" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_TCP.16.dec";
	QTest::newRow( "STREAM_UDP.16.dec" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_UDP.16.dec";
	QTest::newRow( "STREAM_MEM.16.dec" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_MEM.16.dec";

	//-------------------------------------------------------------------------

	convertImageTo24( mSrcImg1, mSrcBuf1 );
	convertImageTo24( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_24BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth / 1.5;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight / 1.5;
	mDstVidInf.bitDepth    = FUG_VID_24BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_TCP.24.dec" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_TCP.24.dec";
	QTest::newRow( "STREAM_UDP.24.dec" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_UDP.24.dec";
	QTest::newRow( "STREAM_MEM.24.dec" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_MEM.24.dec";

	//-------------------------------------------------------------------------

	convertImageTo32( mSrcImg1, mSrcBuf1 );
	convertImageTo32( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_32BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth / 1.5;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight / 1.5;
	mDstVidInf.bitDepth    = FUG_VID_32BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_TCP.32.dec" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_TCP.32.dec";
	QTest::newRow( "STREAM_UDP.32.dec" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_UDP.32.dec";
	QTest::newRow( "STREAM_MEM.32.dec" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_NONE << uint16_t( 100 ) << "STREAM_MEM.32.dec";
#endif
	//-------------------------------------------------------------------------
#if 1
	convertImageTo16( mSrcImg1, mSrcBuf1 );
	convertImageTo16( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_16BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight;
	mDstVidInf.bitDepth    = FUG_VID_16BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_MEM.16.JPEG.20.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_JPEG << uint16_t( 1 ) << "STREAM_MEM.16.JPEG.20.clone";
	QTest::newRow( "STREAM_TCP.16.JPEG.20.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_JPEG << uint16_t( 1 ) << "STREAM_TCP.16.JPEG.20.clone";
	QTest::newRow( "STREAM_UDP.16.JPEG.20.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_JPEG << uint16_t( 1 ) << "STREAM_UDP.16.JPEG.20.clone";

	//-------------------------------------------------------------------------

	convertImageTo24( mSrcImg1, mSrcBuf1 );
	convertImageTo24( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_24BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight;
	mDstVidInf.bitDepth    = FUG_VID_24BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_MEM.24.JPEG.20.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_JPEG << uint16_t( 1 ) << "STREAM_MEM.24.JPEG.20.clone";
	QTest::newRow( "STREAM_TCP.24.JPEG.20.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_JPEG << uint16_t( 1 ) << "STREAM_TCP.24.JPEG.20.clone";
	QTest::newRow( "STREAM_UDP.24.JPEG.20.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_JPEG << uint16_t( 1 ) << "STREAM_UDP.24.JPEG.20.clone";

	//-------------------------------------------------------------------------

	convertImageTo32( mSrcImg1, mSrcBuf1 );
	convertImageTo32( mSrcImg2, mSrcBuf2 );

	mSrcVidInf.frameWidth  = mSrcImg1.width();
	mSrcVidInf.frameHeight = mSrcImg1.height();
	mSrcVidInf.bitDepth    = FUG_VID_32BITVIDEO;
	mSrcVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	mDstVidInf.frameWidth  = mSrcVidInf.frameWidth;
	mDstVidInf.frameHeight = mSrcVidInf.frameHeight;
	mDstVidInf.bitDepth    = FUG_VID_32BITVIDEO;
	mDstVidInf.orientation = FUG_ORIEN_TOP_LEFT;

	QTest::newRow( "STREAM_TCP.32.JPEG.20.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_TCP << FUG_COMPRESS_JPEG << uint16_t( 1 ) << "STREAM_TCP.32.JPEG.20.clone";
	QTest::newRow( "STREAM_UDP.32.JPEG.20.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_UDP << FUG_COMPRESS_JPEG << uint16_t( 1 ) << "STREAM_UDP.32.JPEG.20.clone";
	QTest::newRow( "STREAM_MEM.32.JPEG.20.clone" ) << mSrcBuf1 << mSrcBuf2 << mSrcVidInf << mDstVidInf << FUG_STREAM_MEM << FUG_COMPRESS_JPEG << uint16_t( 1 ) << "STREAM_MEM.32.JPEG.20.clone";
#endif
}

class SleeperThread : public QThread
{
public:
	static void msleep(unsigned long msecs)
	{
		QThread::msleep(msecs);
	}
};

void TestStream::stream( void )
{
	QFETCH( QVector<uchar>, pSrcBuf1 );
	QFETCH( QVector<uchar>, pSrcBuf2 );
	QFETCH( fugVideoInfo, pSrcVidInf );
	QFETCH( fugVideoInfo, pDstVidInf );
	QFETCH( fugStreamProtocol, pProtocol );
	QFETCH( fugStreamCompression, pCompression );
	QFETCH( uint16_t, pLevel );
	QFETCH( QString, pFileNameBase );

	QVector<uchar>	DstBuf;

	switch( pDstVidInf.bitDepth )
	{
		case FUG_VID_16BITVIDEO:
			DstBuf.resize( pDstVidInf.frameWidth * pDstVidInf.frameHeight * 2 );
			break;

		case FUG_VID_24BITVIDEO:
			DstBuf.resize( pDstVidInf.frameWidth * pDstVidInf.frameHeight * 3 );
			break;

		case FUG_VID_32BITVIDEO:
			DstBuf.resize( pDstVidInf.frameWidth * pDstVidInf.frameHeight * 4 );
			break;

		case FUG_VID_8BITVIDEO:
			DstBuf.resize( pDstVidInf.frameWidth * pDstVidInf.frameHeight * 1 );
			break;

		default:
			QFAIL( "Unknown image format" );
			break;
	}

	const uint32_t		Port = 40002;

	// Create an encoder

	HVIDSTREAM		Enc = fugGetEncoder( pProtocol, &pSrcVidInf );

	QVERIFY( Enc != 0 );

	fugSetDestAddress( Enc, "localhost", Port );
	fugSetDestFormat( Enc, &pDstVidInf, pCompression, pLevel );

	// Create a decoder

	HVIDSTREAM		Dec = fugGetDecoder( pProtocol, &pDstVidInf );

	QVERIFY( Dec != 0 );

	fugSetSourceAddress( Dec, Port );

	SleeperThread::msleep( 1000 );

	QVERIFY( !fugHasNewFrame( Dec ) );

	// Send a frame

	for( int i = 0 ; i < 5 ; i++ )
	{
		bool			HasFrame;

		for( size_t f = 0 ; f < 1 ; f++ )
		{
			if( i % 2 == 0 )
			{
				fugEncode( Enc, pSrcBuf1.data() );
			}
			else
			{
				fugEncode( Enc, pSrcBuf2.data() );
			}
		}

		for( size_t f = 0 ; f < 10 && !( HasFrame = fugHasNewFrame( Dec ) ) ; f++ )
		{
			SleeperThread::msleep( 1000 );
		}

		QVERIFY( HasFrame );

		void		*DstDat = fugLockReadBuffer( Dec, 0 );

		QVERIFY( DstDat != 0 );

		if( DstDat != 0 )
		{
			fugVideoInfo	TstInf;

			TstInf.frameWidth  = fugGetRecvWidth( Dec );
			TstInf.frameHeight = fugGetRecvHeight( Dec );
			TstInf.bitDepth    = fugGetRecvDepth( Dec );
			TstInf.orientation = fugGetRecvOrientation( Dec );

			QCOMPARE( TstInf.frameWidth, pDstVidInf.frameWidth );
			QCOMPARE( TstInf.frameHeight, pDstVidInf.frameHeight );
			QCOMPARE( TstInf.bitDepth, pDstVidInf.bitDepth );
			QCOMPARE( TstInf.orientation, pDstVidInf.orientation );

			memcpy( DstBuf.data(), DstDat, DstBuf.size() );

			printf( "Received frame %d...\n", fugGetFrameNumber( Dec ) );

			if( true )
			{
				QString		FileName( QString( "%1.%2.png" ).arg( pFileNameBase ).arg( i ) );

				QImage		DstImg;

				switch( pDstVidInf.bitDepth )
				{
					case FUG_VID_16BITVIDEO:
						DstImg = convert16ToImage( QSize( pDstVidInf.frameWidth, pDstVidInf.frameHeight ), DstBuf );
						break;

					case FUG_VID_24BITVIDEO:
						DstImg = convert24ToImage( QSize( pDstVidInf.frameWidth, pDstVidInf.frameHeight ), DstBuf );
						break;

					case FUG_VID_32BITVIDEO:
						DstImg = convert32ToImage( QSize( pDstVidInf.frameWidth, pDstVidInf.frameHeight ), DstBuf );
						break;

					case FUG_VID_8BITVIDEO:
						break;

					default:
						QFAIL( "Unknown image format" );
						break;
				}

				QVERIFY( !DstImg.isNull() );

				DstImg.save( FileName );
			}

			if( memcmp( &pSrcVidInf, &pDstVidInf, sizeof( fugVideoInfo ) ) == 0 && pCompression == FUG_COMPRESS_NONE && 1 == 1 )
			{
				if( fugGetFrameNumber( Dec ) % 2 == 0 )
				{
					QVERIFY( memcmp( pSrcBuf1.data(), DstBuf.data(), DstBuf.size() ) == 0 );
				}
				else
				{
					QVERIFY( memcmp( pSrcBuf2.data(), DstBuf.data(), DstBuf.size() ) == 0 );
				}
			}

			fugUnlockReadBuffer( Dec );
		}
	}

	fugDeleteDecoder( Dec );

	fugDeleteEncoder( Enc );
}
