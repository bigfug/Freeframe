#ifndef WINDOWCAPTURE_H
#define WINDOWCAPTURE_H

#include <QObject>
#include <QVector>

#include "windowcapturebase.h"

class WindowCapture : public QObject
{
	Q_OBJECT

public:
	explicit WindowCapture( QObject *pParent = 0);

	const QVector<WindowCaptureBase *> interfaces( void )
	{
		return( mWindowCapture );
	}

	WindowCaptureBase *capture( void )
	{
		return( mCurrentInterface );
	}

signals:
	void windowListUpdated( void );

public slots:
	void setCaptureInterface( int i );

private slots:
	void windowListUpdate( void );

private:
	QVector<WindowCaptureBase *>		 mWindowCapture;
	WindowCaptureBase					*mCurrentInterface;
};

#endif // WINDOWCAPTURE_H
