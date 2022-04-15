#ifndef WINGETCLIENTRECT_H
#define WINGETCLIENTRECT_H

#include <QVector>
#include <QRect>

#include "windowcapturebase.h"

//#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>

class winGetClientRect : public WindowCaptureBase
{
	Q_OBJECT
public:
	explicit winGetClientRect( QObject *pParent = 0 );

	virtual ~winGetClientRect( void );

	virtual QString name( void ) const
	{
		return( "Basic Window Grabber" );
	}

	virtual size_t windowCount( void ) const
	{
		return( mWindowHandles.size() );
	}

	virtual QString windowName( int i = -1 ) const;

	virtual QSize windowSize( void ) const;

	virtual void setCropping( const QRect &pCropping );

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
	HBITMAP				 mWindowBMP;
	HDC					 mWindowHDC;
	QRect				 mWindowRect;
	QRect				 mCropping;
};

#endif // WINGETCLIENTRECT_H
