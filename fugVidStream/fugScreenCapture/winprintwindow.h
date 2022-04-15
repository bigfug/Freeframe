#ifndef WINPRINTWINDOW_H
#define WINPRINTWINDOW_H

#include <QObject>

#include "windowcapturebase.h"

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>

class winPrintWindow : public WindowCaptureBase
{
	Q_OBJECT

public:
	explicit winPrintWindow( QObject *pParent = 0);

	virtual QString name( void ) const
	{
		return( "PrintWindow" );
	}

	virtual size_t windowCount( void ) const
	{
		return( mWindowHandles.size() );
	}

	virtual QString windowName( size_t i ) const;

	virtual QSize windowSize( void ) const;

	virtual void setCropping( const QRect &pCropping )
	{
		Q_UNUSED( pCropping );
	}

	virtual void setWindow( size_t i )
	{
		mWindowIndex = i;
	}

signals:

public slots:
	virtual void refeshWindowList( void );

	virtual void captureWindow( void );

private:
	static BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam );

	BOOL CALLBACK EnumWindowsProc( HWND hwnd );

private:
	QVector<HWND>		 mWindowHandles;
	size_t				 mWindowIndex;
};

#endif // WINPRINTWINDOW_H
