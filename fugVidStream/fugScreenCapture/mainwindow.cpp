#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QComboBox>
#include <QDebug>
#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow), mCaptureActive( false ), mMPF( 1000 / 15 )
{
#if !defined( DEMO )
	mEncoder = 0;
#endif

	ui->setupUi(this);

#if defined( DEMO )
	setWindowTitle( windowTitle() + " - DEMO" );
#endif

	foreach( WindowCaptureBase *WCB, mWinCap.interfaces() )
	{
		ui->mWinCapLst->addItem( WCB->name() );
	}

	connect( &mWinCap, SIGNAL(windowListUpdated()), this, SLOT(windowListRefreshed()) );

	mWinCap.capture()->refeshWindowList();
}

MainWindow::~MainWindow()
{
#if !defined( DEMO )
	if( mEncoder != 0 )
	{
		fugDeleteEncoder( mEncoder );

		mEncoder = 0;
	}
#endif

	delete ui;
}

void MainWindow::refreshWindowList( void )
{
	ui->mWindowList->clear();

	mWinCap.capture()->refeshWindowList();
}

void MainWindow::windowListRefreshed()
{
	for( size_t i = 0 ; i < mWinCap.capture()->windowCount() ; i++ )
	{
		ui->mWindowList->addItem( mWinCap.capture()->windowName( i ) );
	}

	if( mWinCap.capture()->windowCount() > 0 )
	{
		on_mWindowList_currentIndexChanged( 0 );
	}
}

void MainWindow::on_mWindowList_currentIndexChanged( int index )
{
	if( index == -1 )
	{
		return;
	}

	mWinCap.capture()->setWindow( index );

	updateCapture();

	updatePreview();

	mSize = mWinCap.capture()->windowSize();

	ui->mWindowWidth->setText( QString( "%1" ).arg( mSize.width() ) );
	ui->mWindowHeight->setText( QString( "%1" ).arg( mSize.height() ) );

	mCropping.setSize( mSize );

	ui->mCropLeft->setMaximum( mSize.width() );
	ui->mCropTop->setMaximum( mSize.height() );
	ui->mCropRight->setMaximum( mSize.width() );
	ui->mCropBottom->setMaximum( mSize.height() );

	updateCroppingValues();

	ui->mOutputWidth->setValue( mSize.width() );
	ui->mOutputHeight->setValue( mSize.height() );
}

void MainWindow::on_mWinCapLst_currentIndexChanged(int index)
{
	ui->mWindowList->clear();

	mWinCap.setCaptureInterface( index );

	mWinCap.capture()->refeshWindowList();

	ui->mWindowList->setCurrentIndex( 0 );

	on_mWindowList_currentIndexChanged( 0 );
}

void MainWindow::updateCroppingValues( void )
{
	ui->mCropLeft->setValue( mCropping.left() );
	ui->mCropTop->setValue( mCropping.top() );
	ui->mCropRight->setValue( mSize.width() - mCropping.right() - 1 );
	ui->mCropBottom->setValue( mSize.height() - mCropping.bottom() - 1 );

	mWinCap.capture()->setCropping( mCropping );

	ui->mInputWidth->setText( QString( "%1" ).arg( mCropping.width() ) );
	ui->mInputHeight->setText( QString( "%1" ).arg( mCropping.height() ) );

	ui->mOutputWidth->setMaximum( mCropping.width() );
	ui->mOutputHeight->setMaximum( mCropping.height() );
}

void MainWindow::updateStreaming()
{
	if( !mCaptureActive )
	{
#if !defined( DEMO )
		if( mEncoder != 0 )
		{
			fugDeleteEncoder( mEncoder );

			mEncoder = 0;
		}
#endif
		return;
	}

#if !defined( DEMO )
	bool					EncoderChanged  = false;
	fugStreamProtocol		EncoderProtocol = (fugStreamProtocol)( ui->mProtocol->currentIndex() + 1 );
	QSize					EncoderSize     = mCropping.size();
	QSize					EncoderDestSize = QSize( ui->mOutputWidth->value(), ui->mOutputHeight->value() );
	QString					EncoderAddress  = ui->mAddress->text();
	qint16					EncoderChannel	= ui->mChannel->value();
	fugStreamCompression	EncoderCompression = (fugStreamCompression)ui->mCompression->currentIndex();

	if( mEncoder != 0 )
	{
		if( EncoderProtocol != mEncoderProtocol )
		{
			EncoderChanged = true;
		}
		else if( EncoderSize != mEncoderSize )
		{
			EncoderChanged = true;
		}
		else if( EncoderAddress != mEncoderAddress )
		{
			EncoderChanged = true;
		}
		else if( EncoderChannel != mEncoderChannel )
		{
			EncoderChanged = true;
		}
		else if( EncoderDestSize != mEncoderDestSize )
		{
			EncoderChanged = true;
		}
		else if( EncoderCompression != mEncoderCompression )
		{
			EncoderChanged = true;
		}

		if( EncoderChanged )
		{
			fugDeleteEncoder( mEncoder );

			mEncoder = 0;
		}
	}

	if( mEncoder == 0 && mCropping.width() > 0 && mCropping.height() > 0 )
	{
		mEncoderProtocol    = EncoderProtocol;
		mEncoderSize        = EncoderSize;
		mEncoderDestSize    = EncoderDestSize;
		mEncoderAddress     = EncoderAddress;
		mEncoderChannel     = EncoderChannel;
		mEncoderCompression = EncoderCompression;

//		uint16_t BitDep = mWinCap.capture()->pixmap().depth();

		mEncoderInfo.frameWidth  = mEncoderSize.width();
		mEncoderInfo.frameHeight = mEncoderSize.height();
		mEncoderInfo.orientation = FUG_ORIEN_TOP_LEFT;
		mEncoderInfo.bitDepth    = FUG_VID_32BITVIDEO;

		mEncoderDest.frameWidth  = mEncoderDestSize.width();
		mEncoderDest.frameHeight = mEncoderDestSize.height();
		mEncoderDest.orientation = FUG_ORIEN_TOP_LEFT;
		mEncoderDest.bitDepth    = FUG_VID_32BITVIDEO;

		if( ( mEncoder = fugGetEncoder( mEncoderProtocol, &mEncoderInfo ) ) != 0 )
		{
			fugSetDestAddress( mEncoder, mEncoderAddress.toLocal8Bit().constData(), FUG_BASE_PORT + mEncoderChannel );

			fugSetDestFormat( mEncoder, &mEncoderDest, mEncoderCompression );
		}
	}
#endif
}

void MainWindow::updateCapture()
{
	mWinCap.capture()->captureWindow();
}

void MainWindow::updatePreview()
{
	const QPixmap		&Pixmap = mWinCap.capture()->pixmap();

	ui->mPreview->setPixmap( Pixmap.scaled( 256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

	ui->mPreview->update();
}

void MainWindow::capture()
{
	qint64		CurrTime = QDateTime::currentMSecsSinceEpoch() - mCaptureStart;
	qint64		CurrSecs = CurrTime - ( CurrTime % 1000 );
	static qint64	FrameCount = 0;
	static qint64	CaptureTime = 0;
	static qint64	EncoderTime = 0;

	if( CurrSecs != mStatusLast )
	{
		LARGE_INTEGER		TF;

		QueryPerformanceFrequency( &TF );

		double		Cap = double( CaptureTime ) / double( TF.QuadPart );
		double		Enc = double( EncoderTime ) / double( TF.QuadPart );

		ui->mStatusBar->showMessage( QString( "Streaming %1s - FPS: %2 - Capture: %3 - Encoding: %4" ).arg( CurrSecs / 1000 ).arg( FrameCount ).arg( Cap ).arg( Enc ) );

		FrameCount = 0;
		CaptureTime = 0;
		EncoderTime = 0;

		mStatusLast = CurrSecs;
	}

	if( CurrTime - mCaptureLast > mMPF )
	{
		LARGE_INTEGER		TS, TE;

		QueryPerformanceCounter( &TS );

		updateCapture();

		QueryPerformanceCounter( &TE );

		CaptureTime += TE.QuadPart - TS.QuadPart;

		FrameCount++;

		mCaptureLast += mMPF;
	}

#if !defined( DEMO )
	if( mCaptureLast > mEncoderLast )
	{
		LARGE_INTEGER		TS, TE;

		QueryPerformanceCounter( &TS );

		QImage		Image = mWinCap.capture()->pixmap().toImage();

		updateStreaming();

		if( mEncoder != 0 )
		{
			fugEncode( mEncoder, Image.bits() );
		}

		QueryPerformanceCounter( &TE );

		EncoderTime += TE.QuadPart - TS.QuadPart;

		mEncoderLast = mCaptureLast;
	}
#endif

	if( mCaptureActive )
	{
		QTimer::singleShot( 1, this, SLOT(capture()) );
	}
}

void MainWindow::on_mCropLeft_valueChanged( int arg1 )
{
	mCropping.setLeft( arg1 );

	updateCroppingValues();

	updateCapture();

	updatePreview();
}

void MainWindow::on_mCropRight_valueChanged(int arg1)
{
	mCropping.setRight( mSize.width() - 1 - arg1 );

	updateCroppingValues();

	updateCapture();

	updatePreview();
}

void MainWindow::on_mCropTop_valueChanged(int arg1)
{
	mCropping.setTop( arg1 );

	updateCroppingValues();

	updateCapture();

	updatePreview();
}

void MainWindow::on_mCropBottom_valueChanged(int arg1)
{
	mCropping.setBottom( mSize.height() - 1 - arg1 );

	updateCroppingValues();

	updateCapture();

	updatePreview();
}

void MainWindow::on_mStreamOutput_toggled( bool checked )
{
#if defined( DEMO )
	QMessageBox::information( this, "DEMO VERSION", "This is the demo version.\n\nCapturing is enabled.\n\nStreaming is disabled.\n\nPlease visit www.bigfug.com to purchase the full version" );
#endif

	if( checked && !mCaptureActive )
	{
		mCaptureStart = QDateTime::currentMSecsSinceEpoch();

		mCaptureLast = 0;
		mEncoderLast = 0;

		QTimer::singleShot( 1, this, SLOT(capture()) );
	}

	mCaptureActive = checked;
}

void MainWindow::on_mFPS_valueChanged( int arg1 )
{
	mMPF = arg1 > 0 ? 1000 / arg1 : 0;
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox		MsgBox;

	MsgBox.setText( "fugScreenCapture v1.0.0 - by Alex May\n\n(c)2014 bigfug.com" );
	MsgBox.exec();
}

void MainWindow::on_actionAboutQt_triggered()
{
	QMessageBox::aboutQt( this );
}

void MainWindow::on_actionDocumentation_triggered()
{
	QDesktopServices::openUrl( QUrl( QString( "http://www.bigfug.com/software/video-streaming/window-capture-video-streaming/?utm_source=fugScreenCapture&utm_medium=online_help" ) ) );
}
