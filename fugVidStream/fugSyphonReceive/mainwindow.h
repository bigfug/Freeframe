#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "syphonsend.h"

#include "fugstreamdecoder.h"

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
	void on_pushButton_toggled(bool checked);

	void resetDecoder( void );

	void frameReceived( void );

private:
	Ui::MainWindow			*ui;

	SyphonSend				 mSender;

	fugStreamDecoder		*mDecoder;
	QThread					*mDecoderThread;
};

#endif // MAINWINDOW_H
