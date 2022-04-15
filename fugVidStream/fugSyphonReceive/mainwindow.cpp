#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>

#include <gl.h>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow), mDecoder( 0 ), mDecoderThread( 0 )
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	resetDecoder();

	delete ui;
}

void MainWindow::on_pushButton_toggled( bool pChecked )
{
	if( pChecked )
	{
		ui->mServerName->setEnabled( false );

		mSender.start( qPrintable( ui->mServerName->text() ) , ui->mOutput->cgl() );

		fugVideoInfo		 VideoInfo;

		VideoInfo.frameWidth  = 320;
		VideoInfo.frameHeight = 240;
		VideoInfo.bitDepth    = FUG_VID_24BITVIDEO;
		VideoInfo.orientation = FUG_ORIEN_TOP_LEFT;

		if( ( mDecoder = new fugStreamDecoder( (fugStreamProtocol)( ui->mProtocol->currentIndex() + 1 ), VideoInfo, ui->mChannel->value() ) ) == 0 )
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

		//QTimer::singleShot( 200, this, SLOT(demo()) );
	}
	else
	{
		resetDecoder();

		mSender.stop();

		ui->mServerName->setEnabled( true );
	}
}

void MainWindow::resetDecoder()
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
}

void MainWindow::frameReceived( void )
{
	if( !ui->pushButton->isChecked() )
	{
		return;
	}

	ui->mOutput->updateTexture( mDecoder->videoinfo(), mDecoder->buffer().data() );

	if( mSender.hasClients() )
	{
		TexToBuf					&T2B = ui->mOutput->texToBuf();
		const FFGLTextureStruct		&TEX = T2B.texInfo();

		mSender.publishFrame( T2B.getTex(), GL_TEXTURE_2D, TEX.Width, TEX.Height, TEX.HardwareWidth, TEX.HardwareHeight, true );
	}
}
