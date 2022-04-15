#include "windowcapture.h"

#if defined( Q_OS_WIN )
#include "wingetclientrect.h"
#include "winprintwindow.h"
#endif

WindowCapture::WindowCapture(QObject *parent) :
	QObject(parent)
{
#if defined( Q_OS_WIN )
	mWindowCapture.append( new winGetClientRect() );
//	mWindowCapture.append( new winPrintWindow() );
#endif

	foreach( WindowCaptureBase *WCB, mWindowCapture )
	{
		connect( WCB, SIGNAL(windowListUpdated()), this, SLOT(windowListUpdate()) );
	}

	mCurrentInterface = mWindowCapture[ 0 ];
}

void WindowCapture::setCaptureInterface( int i )
{
	mCurrentInterface = mWindowCapture[ i ];
}

void WindowCapture::windowListUpdate()
{
	emit windowListUpdated();
}
