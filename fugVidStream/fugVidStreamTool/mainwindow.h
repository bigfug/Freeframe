#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QDateTime>
#include <QTimer>

#include "fugstreamdecoder.h"

#include "ui_mainwindow.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow( QWidget *pParent = 0 );

	virtual ~MainWindow( void );
	
private slots:
	void resetStream( void );

	void frameReceived( void );

	void onWindowUpdateTimer( void );

	void updateWindow( void );

	void onEncoderTimer( void );

	void on_mSendButton_toggled(bool checked);

	void drawEncoderImage( void );

	void on_mReceiveChannel_valueChanged(int arg1);

	void on_mReceiveProtocol_currentIndexChanged(int index);

	void on_mSendProtocol_currentIndexChanged(int index);

	void on_actionAbout_fugVidStreamTester_triggered();

	void on_actionOnline_Help_triggered();

	void on_actionBigfug_com_triggered();

	void on_mSendConfig_valueChanged( int pConfigId );

	void on_mSendSave_clicked();

	void on_mRecvFPS_valueChanged(int arg1);

private:
	Ui::MainWindow			*ui;

	fugStreamDecoder		*mDecoder;
	QThread					*mDecoderThread;

	qint64					 mLastTime;

	volatile qint64			 mFramesReceived;
	volatile qint64			 mCompressedSize;
	volatile qint64			 mFrameSize;

	HVIDSTREAM				 mEncoder;
	fugVideoInfo			 mEncoderVidInf;
	QImage					 mEncoderImage;

	QString					 mSettingsFileName;
};

#endif // MAINWINDOW_H
