#ifndef WINDOWCAPTUREBASE_H
#define WINDOWCAPTUREBASE_H

#include <QObject>
#include <QPixmap>

class WindowCaptureBase : public QObject
{
	Q_OBJECT

public:
	explicit WindowCaptureBase( QObject *pParent = 0 );

	virtual QString name( void ) const = 0;

	virtual size_t windowCount( void ) const = 0;

	virtual QString windowName( int i = -1 ) const = 0;

	virtual QSize windowSize( void ) const = 0;

	virtual void setWindow( size_t i ) = 0;

	virtual void setCropping( const QRect &pCropping ) = 0;

	const QPixmap &pixmap( void ) const
	{
		return( mPixmap );
	}

signals:
	void windowListUpdated( void );

public slots:
	virtual void refeshWindowList( void ) = 0;

	virtual void captureWindow( void ) = 0;

protected:
	QPixmap								 mPixmap;
};

#endif // WINDOWCAPTUREBASE_H
