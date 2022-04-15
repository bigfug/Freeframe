#include "winprintwindow.h"

winPrintWindow::winPrintWindow(QObject *parent) :
	WindowCaptureBase(parent)
{
}

QString winPrintWindow::windowName(size_t i) const
{
	WCHAR		 Name[ 256 ];

	GetWindowText( mWindowHandles[ i ], Name, 256 );

	return( QString::fromWCharArray( Name ) );
}

QSize winPrintWindow::windowSize(void) const
{
	return( QSize() );
}

void winPrintWindow::refeshWindowList()
{
	mWindowHandles.clear();

	EnumWindows( &winPrintWindow::EnumWindowsProc, reinterpret_cast<LPARAM>( this ) );

	emit windowListUpdated();
}

void winPrintWindow::captureWindow()
{
	HWND		hwnd = mWindowHandles[ mWindowIndex ];

	if( hwnd == 0 )
	{
		return;
	}

	HDC			whdc = GetDC( hwnd );

	if( whdc == 0 )
	{
		return;
	}

	HDC			mhdc = CreateCompatibleDC( whdc );

	if( mhdc == 0 )
	{
		return;
	}

	RECT		wrct;

	if( !GetClientRect( hwnd, &wrct ) )
	{
		return;
	}

#if 0
	HBITMAP		mbmp = CreateCompatibleBitmap( whdc, wrct.right, wrct.bottom );

	HBITMAP		obmp = (HBITMAP)SelectObject( mhdc, mbmp );

	PrintWindow( hwnd, mhdc, PW_CLIENTONLY );

	mbmp = (HBITMAP)SelectObject( mhdc, obmp );

	DeleteDC( mhdc );

	mPixmap = QPixmap::fromImageWinHBITMAP( mbmp );

	DeleteObject( mbmp );
#endif
}

BOOL winPrintWindow::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	winPrintWindow	*Main = reinterpret_cast<winPrintWindow *>( lParam );

	return( Main->EnumWindowsProc( hwnd ) );
}

BOOL winPrintWindow::EnumWindowsProc(HWND hwnd)
{
	mWindowHandles.append( hwnd );

	return( TRUE );
}
