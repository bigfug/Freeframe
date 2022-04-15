#include "wingetclientrect.h"

#include <QScreen>
#include <QApplication>

winGetClientRect::winGetClientRect( QObject *pParent ) :
	WindowCaptureBase( pParent ), mWindowIndex( 0 ), mWindowBMP( 0 ), mWindowHDC( 0 )
{
}

winGetClientRect::~winGetClientRect()
{
	if( mWindowHDC != 0 )
	{
		DeleteDC( mWindowHDC );

		mWindowHDC = 0;
	}

	if( mWindowBMP != 0 )
	{
		DeleteObject( mWindowBMP );

		mWindowBMP = 0;
	}
}

QString winGetClientRect::windowName( int i ) const
{
	WCHAR		 Name[ 256 ];

	if( i < 0 ) i = mWindowIndex;

	GetWindowText( mWindowHandles[ i ], Name, 256 );

	if( Name[ 0 ] != 0 )
	{
		return( QString::fromWCharArray( Name ) );
	}

	return( QString( "0x%1" ).arg( qlonglong( mWindowHandles[ i ] ) ) );
}

QSize winGetClientRect::windowSize( void ) const
{
	return( mWindowRect.size() );
}

void winGetClientRect::setCropping( const QRect &pCropping )
{
	mCropping = pCropping;
}

void winGetClientRect::captureWindow()
{
	HWND		hwnd = mWindowHandles[ mWindowIndex ];

	if( hwnd == 0 )
	{
		return;
	}

	RECT		wtmp;

	if( !GetClientRect( hwnd, &wtmp ) )
	{
		return;
	}

	QRect		wrct( QPoint( wtmp.left, wtmp.top ), QPoint( wtmp.right, wtmp.bottom ) );

	if( mWindowRect != wrct )
	{
		if( mWindowHDC != 0 )
		{
			DeleteDC( mWindowHDC );

			mWindowHDC = 0;
		}

		if( mWindowBMP != 0 )
		{
			DeleteObject( mWindowBMP );

			mWindowBMP = 0;
		}

		mWindowRect = mCropping = wrct;
	}

#if 0
	HDC			whdc = GetDC( hwnd );

	if( whdc == 0 )
	{
		return;
	}

	if( mWindowHDC == 0 && ( mWindowHDC = CreateCompatibleDC( whdc ) ) == 0 )
	{
		return;
	}

	if( mWindowBMP == 0 && ( mWindowBMP = CreateCompatibleBitmap( whdc, mCropping.width(), mCropping.height() ) ) == 0 )
	{
		return;
	}

	HBITMAP		obmp = (HBITMAP)SelectObject( mWindowHDC, mWindowBMP );

	BitBlt( mWindowHDC, 0, 0, mCropping.width(), mCropping.height(), whdc, mCropping.left(), mCropping.top(), SRCCOPY );

	mWindowBMP = (HBITMAP)SelectObject( mWindowHDC, obmp );

	mPixmap = QPixmap::fromWinHBITMAP( mWindowBMP );

#endif

	//mPixmap = QPixmap::grabWindow( hwnd, mCropping.left(), mCropping.top(), mCropping.width(), mCropping.height() );

	mPixmap = qApp->primaryScreen()->grabWindow( WId( hwnd ), mCropping.left(), mCropping.top(), mCropping.width(), mCropping.height() );
}

void winGetClientRect::refeshWindowList()
{
	mWindowHandles.clear();

	EnumWindows( &winGetClientRect::EnumWindowsProc, reinterpret_cast<LPARAM>( this ) );

	emit windowListUpdated();
}

BOOL winGetClientRect::EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
	winGetClientRect	*Main = reinterpret_cast<winGetClientRect *>( lParam );

	return( Main->EnumWindowsProc( hwnd ) );
}

BOOL winGetClientRect::EnumWindowsProc( HWND hwnd )
{
	RECT		wrct;

	if( !GetClientRect( hwnd, &wrct ) )
	{
		return( TRUE );
	}

	if( wrct.right > 0 && wrct.bottom > 0 )
	{
		mWindowHandles.append( hwnd );
	}

	return( TRUE );
}
