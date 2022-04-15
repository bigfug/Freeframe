#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <qmath.h>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>

#if defined( Q_OS_WIN )
#include <shlwapi.h>
#include <ShlObj.h>
#endif

MainWindow::MainWindow( QWidget *pParent )
	: QMainWindow( pParent ), ui( new Ui::MainWindow() ), mDecoder( 0 ), mDecoderThread( 0 ), mLastTime( QDateTime::currentMSecsSinceEpoch() ),
	  mFramesReceived( 0 ), mEncoder( 0 )
{
	ui->setupUi( this );

#if defined( Q_OS_WIN )
	TCHAR szPath[MAX_PATH];

	if( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath ) ) )
	{
		PathAppend( szPath, L"bigfug" );
		PathAppend( szPath, L"fugStream.ini" );

		mSettingsFileName = QString::fromWCharArray( szPath );
	}
#elif defined( Q_OS_MACX )
	mSettingsFileName = "/Library/Preferences/fugStream.ini";
#else
	mSettingsFileName = "/etc/xdg/bigfug/fugStream.ini";
#endif

	foreach( const QHostAddress &address, QNetworkInterface::allAddresses() )
	{
		if( address.protocol() == QAbstractSocket::IPv4Protocol )
		{
			ui->mAddresses->addItem( address.toString() );
		}
	}

	on_mSendConfig_valueChanged( 0 );

	QTimer::singleShot( 40, this, SLOT(onWindowUpdateTimer()) );

	resetStream();
}

MainWindow::~MainWindow()
{
	if( mDecoder != 0 )
	{
		mDecoder->endDecoder();
	}

	if( mDecoderThread != 0 )
	{
		mDecoderThread->quit();
		mDecoderThread->wait();
	}

	if( mDecoder != 0 )
	{
		delete mDecoder;

		mDecoder = 0;
	}

	if( mEncoder != 0 )
	{
		fugDeleteEncoder( mEncoder );

		mEncoder = 0;
	}

	delete ui;
}

void MainWindow::resetStream( void )
{
	if( mDecoder != 0 )
	{
		mDecoder->endDecoder();
	}

	if( mDecoderThread != 0 )
	{
		mDecoderThread->quit();
		mDecoderThread->wait();

		mDecoderThread = 0;
	}

	if( mDecoder != 0 )
	{
		delete mDecoder;

		mDecoder = 0;
	}

	if( ui->mReceiveProtocol->currentIndex() == 0 )
	{
		return;
	}

	fugVideoInfo		 VideoInfo;

	VideoInfo.frameWidth  = ui->mReceiveDisplay->width();
	VideoInfo.frameHeight = ui->mReceiveDisplay->height();
	VideoInfo.bitDepth    = FUG_VID_24BITVIDEO;
	VideoInfo.orientation = FUG_ORIEN_TOP_LEFT;

	if( ( mDecoder = new fugStreamDecoder( (fugStreamProtocol)( ui->mReceiveProtocol->currentIndex() ), VideoInfo, ui->mReceiveChannel->value() ) ) == 0 )
	{
		return;
	}

	if( ( mDecoderThread = new QThread( this ) ) == 0 )
	{
		return;
	}

	connect( mDecoderThread, SIGNAL(started()), mDecoder, SLOT(decoderThread()) );
	//connect( mDecoderThread, SIGNAL(finished()), mDecoder, SLOT(deleteLater()) );

	mDecoder->moveToThread( mDecoderThread );

	connect( mDecoder, SIGNAL(frameReceived()), this, SLOT(frameReceived()) );

	mDecoderThread->start();
}

void MainWindow::frameReceived( void )
{
	if( ui->mReceiveUpdate->isChecked() )
	{
		QImage		I = mDecoder->image();

		ui->mReceiveDisplay->setPixmap( QPixmap::fromImage( I ) );

		ui->mReceiveDisplay->update();
	}

	fugVideoInfo		vidinf = mDecoder->videoinfo();

	mCompressedSize += fugGetCompressedFrameSize( mDecoder->decoder() );
	mFrameSize += fugGetByteSize( &vidinf );

	mFramesReceived++;
}

void MainWindow::onWindowUpdateTimer()
{
	updateWindow();

	QTimer::singleShot( 100, this, SLOT(onWindowUpdateTimer()) );
}

void MainWindow::updateWindow()
{
	qint64		CurrTime = QDateTime::currentMSecsSinceEpoch();
	qint64		DiffTIme = CurrTime - mLastTime;

	if( DiffTIme < 1000 )
	{
		return;
	}

	if( mDecoder != 0 && mFramesReceived > 0 )
	{
		fugVideoInfo		vidinf = mDecoder->videoinfo();

		ui->mReceiveTransferBytesPerFrame->setText( QString( "%1" ).arg( qFloor( qreal( mCompressedSize ) / qreal( mFramesReceived ) ) ) );

		ui->mReceiveImageBytesPerFrame->setText( QString( "%1" ).arg( fugGetByteSize( &vidinf ) ) );

		ui->mReceiveBytesPerSecond->setText( QString( "%1" ).arg( mFrameSize ) );
		ui->mCompressedSize->setText( QString( "%1" ).arg( mCompressedSize ) );

		ui->mWidth->setText( QString( "%1" ).arg( vidinf.frameWidth ) );
		ui->mHeight->setText( QString( "%1" ).arg( vidinf.frameHeight ) );

		switch( vidinf.bitDepth )
		{
			case FUG_VID_8BITVIDEO:
				ui->mDepth->setText( "8-bit" );
				break;

			case FUG_VID_16BITVIDEO:
				ui->mDepth->setText( "16-bit" );
				break;

			case FUG_VID_24BITVIDEO:
				ui->mDepth->setText( "24-bit" );
				break;

			case FUG_VID_32BITVIDEO:
				ui->mDepth->setText( "32-bit" );
				break;
		}
	}

	ui->mFPS->setText( QString( "%1" ).arg( mFramesReceived ) );

	mFramesReceived = 0;
	mCompressedSize = 0;
	mFrameSize      = 0;

	mLastTime += 1000;
}

void MainWindow::onEncoderTimer()
{
	static qint64		LastTime = QDateTime::currentMSecsSinceEpoch();
	static uint16_t		CurrCount = 0;

	qint64		CurrTime = QDateTime::currentMSecsSinceEpoch();
	qint64		DiffTIme = CurrTime - LastTime;

	if( DiffTIme >= 1000 )
	{
		//qDebug() << CurrCount;

		LastTime += 1000;

		CurrCount = 0;
	}
	else
	{
		CurrCount++;
	}

	if( mEncoder == 0 )
	{
		return;
	}

	drawEncoderImage();

	//mEncoderImage.save( "/tmp/asd.png" );

	if( ui->mSendUpdate->isChecked() )
	{
		ui->mSendDisplay->setPixmap( QPixmap::fromImage( mEncoderImage ) );
	}

	fugEncode( mEncoder, mEncoderImage.bits() );

	QTimer::singleShot( 1000 / ui->mSendFPS->value(), this, SLOT(onEncoderTimer()) );
}

void MainWindow::on_mSendButton_toggled( bool pSend )
{
	if( pSend )
	{
		int		W = ui->mSendWidth->value();
		int		H = ui->mSendHeight->value();

		mEncoderVidInf.frameWidth  = ( W > 0 ? W : 128 );
		mEncoderVidInf.frameHeight = ( H > 0 ? H : 128 );
		mEncoderVidInf.bitDepth    = ui->mSendDepth->currentIndex();
		mEncoderVidInf.orientation = FUG_ORIEN_TOP_LEFT;

		switch( mEncoderVidInf.bitDepth )
		{
			case FUG_VID_8BITVIDEO:
				mEncoderImage = QImage( mEncoderVidInf.frameWidth, mEncoderVidInf.frameHeight, QImage::Format_RGB888 );
				break;

			case FUG_VID_16BITVIDEO:
				mEncoderImage = QImage( mEncoderVidInf.frameWidth, mEncoderVidInf.frameHeight, QImage::Format_RGB16 );
				break;

			case FUG_VID_24BITVIDEO:
				mEncoderImage = QImage( mEncoderVidInf.frameWidth, mEncoderVidInf.frameHeight, QImage::Format_RGB888 );
				break;

			case FUG_VID_32BITVIDEO:
				mEncoderImage = QImage( mEncoderVidInf.frameWidth, mEncoderVidInf.frameHeight, QImage::Format_RGB32 );
				break;
		}

		mEncoderImage.fill( Qt::gray );

		if( mEncoderImage.isNull() )
		{
			return;
		}

		if( ( mEncoder = fugGetEncoder( (fugStreamProtocol)( ui->mSendProtocol->currentIndex() + 1 ), &mEncoderVidInf ) ) == 0 )
		{
			return;
		}

		ui->mSendConfig->setEnabled( false );
		ui->mSendAddress->setEnabled( false );
		ui->mSendChannel->setEnabled( false );
		ui->mSendDepth->setEnabled( false );
		ui->mSendHeight->setEnabled( false );
		ui->mSendProtocol->setEnabled( false );
		ui->mSendWidth->setEnabled( false );
		ui->mSendCompressionType->setEnabled( false );
		ui->mSendCompressionLevel->setEnabled( false );

		fugSetDestFormat( mEncoder, &mEncoderVidInf, ui->mSendCompressionType->currentIndex() == 0 ? FUG_COMPRESS_NONE : FUG_COMPRESS_JPEG, ui->mSendCompressionLevel->value() );
		fugSetDestAddress( mEncoder, qPrintable( ui->mSendAddress->text() ), FUG_BASE_PORT + ui->mSendChannel->value() );

		QTimer::singleShot( 100, this, SLOT(onEncoderTimer()) );
	}
	else
	{
		if( mEncoder != 0 )
		{
			fugDeleteEncoder( mEncoder );

			mEncoder = 0;
		}

		ui->mSendConfig->setEnabled( true );
		ui->mSendAddress->setEnabled( ui->mSendProtocol->currentIndex() != 2 );
		ui->mSendChannel->setEnabled( true );
		ui->mSendDepth->setEnabled( true );
		ui->mSendHeight->setEnabled( true );
		ui->mSendProtocol->setEnabled( true );
		ui->mSendWidth->setEnabled( true );
		ui->mSendCompressionType->setEnabled( true );
		ui->mSendCompressionLevel->setEnabled( true );
	}
}

void MainWindow::drawEncoderImage()
{
	QPainter		Painter( &mEncoderImage );

	int				x1 = ( qint64( qrand() ) * qint64( mEncoderImage.width() ) ) / qint64( RAND_MAX );
	int				y1 = ( qint64( qrand() ) * qint64( mEncoderImage.height() ) ) / qint64( RAND_MAX );
	int				x2 = ( qint64( qrand() ) * qint64( mEncoderImage.width() ) ) / qint64( RAND_MAX );
	int				y2 = ( qint64( qrand() ) * qint64( mEncoderImage.height() ) ) / qint64( RAND_MAX );

	int				r  = ( qint64( qrand() ) * 255 ) / qint64( RAND_MAX );
	int				g  = ( qint64( qrand() ) * 255 ) / qint64( RAND_MAX );
	int				b  = ( qint64( qrand() ) * 255 ) / qint64( RAND_MAX );

	QColor			rgb( r, g, b );

	QPen			pen( rgb, 2, Qt::SolidLine );

	Painter.setPen( pen );
	Painter.drawLine( x1, y1, x2, y2 );
}

void MainWindow::on_mReceiveChannel_valueChanged( int pChannel )
{
	Q_UNUSED( pChannel )

	resetStream();
}

void MainWindow::on_mReceiveProtocol_currentIndexChanged( int pIndex )
{
	Q_UNUSED( pIndex )

	resetStream();
}

void MainWindow::on_mSendProtocol_currentIndexChanged( int pIndex )
{
	switch( pIndex )
	{
		case 0:
		case 1:
			ui->mSendAddress->setEnabled( true );
			break;

		case 2:
			ui->mSendAddress->setEnabled( false );
			break;
	}
}

void MainWindow::on_actionAbout_fugVidStreamTester_triggered()
{
	QMessageBox		MsgBox;

	MsgBox.setText( "fugVidStreamTool v1.1.0 - by Alex May\n\n(c)2013 bigfug.com" );
	MsgBox.exec();
}

void MainWindow::on_actionOnline_Help_triggered()
{
	QDesktopServices::openUrl( QUrl( QString( "http://www.bigfug.com/software/fugvidstreamtool/?utm_source=fugVidStreamTool&utm_medium=online_help" ) ) );
}

void MainWindow::on_actionBigfug_com_triggered()
{
	QDesktopServices::openUrl( QUrl( QString( "http://www.bigfug.com/?utm_source=fugVidStreamTool&utm_medium=website" ) ) );
}

void MainWindow::on_mSendConfig_valueChanged( int pConfigId )
{
	QSettings		Settings( mSettingsFileName, QSettings::IniFormat );

	Settings.beginGroup( QString( "%1" ).arg( pConfigId ) );

	ui->mSendProtocol->setCurrentIndex( Settings.value( "protocol", ui->mSendProtocol->currentIndex() + 1 ).toInt() - 1 );
	ui->mSendChannel->setValue( Settings.value( "channel", ui->mSendChannel->value() ).toInt() );
	ui->mSendAddress->setText( Settings.value( "address", ui->mSendAddress->text() ).toString() );
	ui->mSendWidth->setValue( Settings.value( "width", ui->mSendWidth->value() ).toInt() );
	ui->mSendHeight->setValue( Settings.value( "height", ui->mSendHeight->value() ).toInt() );
	ui->mSendDepth->setCurrentIndex( Settings.value( "depth", ui->mSendDepth->currentIndex() ).toInt() );
	ui->mSendCompressionType->setCurrentIndex( Settings.value( "compressiontype", ui->mSendCompressionType->currentIndex() ).toInt() );
	ui->mSendCompressionLevel->setValue( Settings.value( "compressionlevel", ui->mSendCompressionLevel->value() ).toInt() );

	Settings.endGroup();

	on_mSendProtocol_currentIndexChanged( ui->mSendProtocol->currentIndex() );
}

void MainWindow::on_mSendSave_clicked()
{
	QSettings		Settings( mSettingsFileName, QSettings::IniFormat );

	Settings.beginGroup( QString( "%1" ).arg( ui->mSendConfig->value() ) );

	Settings.setValue( "protocol", ui->mSendProtocol->currentIndex() + 1 );
	Settings.setValue( "channel", ui->mSendChannel->value() );
	Settings.setValue( "address", ui->mSendAddress->text() );
	Settings.setValue( "width", ui->mSendWidth->value() );
	Settings.setValue( "height", ui->mSendHeight->value() );
	Settings.setValue( "depth", ui->mSendDepth->currentIndex() );
	Settings.setValue( "compressiontype", ui->mSendCompressionType->currentIndex() );
	Settings.setValue( "compressionlevel", ui->mSendCompressionLevel->value() );

	Settings.endGroup();
}

void MainWindow::on_mRecvFPS_valueChanged( int pFPS )
{
	mDecoder->setFPS( pFPS );
}
