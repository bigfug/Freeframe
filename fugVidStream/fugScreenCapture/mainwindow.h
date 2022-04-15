#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QRect>
#include <QDateTime>

#if !defined( DEMO )
#include "fugVidStream.h"
#endif

#if defined( Q_OS_WIN )
#include <Windows.h>
#endif

#include "windowcapture.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void refreshWindowList();

	void windowListRefreshed( void );

	void on_mWindowList_currentIndexChanged(int index);

	void on_mWinCapLst_currentIndexChanged(int index);

	void updateCroppingValues( void );

	void updateStreaming( void );

	void updateCapture( void );

	void updatePreview( void );

	void capture( void );

	void on_mCropLeft_valueChanged(int arg1);

	void on_mCropRight_valueChanged(int arg1);

	void on_mCropTop_valueChanged(int arg1);

	void on_mCropBottom_valueChanged(int arg1);

	void on_mStreamOutput_toggled(bool checked);

	void on_mFPS_valueChanged(int arg1);

	void on_actionAbout_triggered();

	void on_actionAboutQt_triggered();

	void on_actionDocumentation_triggered();

private:
	Ui::MainWindow		*ui;

#if !defined( DEMO )
	HVIDSTREAM				 mEncoder;
	fugVideoInfo			 mEncoderInfo;
	fugVideoInfo			 mEncoderDest;
	QSize					 mEncoderSize;
	QSize					 mEncoderDestSize;
	fugStreamProtocol		 mEncoderProtocol;
	fugStreamCompression	 mEncoderCompression;
	QString					 mEncoderAddress;
	qint16					 mEncoderChannel;
#endif

	WindowCapture		 mWinCap;
	QSize				 mSize;
	QRect				 mCropping;
	bool				 mCaptureActive;
	qint64				 mCaptureLast;
	qint64				 mStatusLast;
	qint64				 mCaptureStart;
	qint64				 mEncoderLast;
	qint64				 mMPF;
};

#endif // MAINWINDOW_H
