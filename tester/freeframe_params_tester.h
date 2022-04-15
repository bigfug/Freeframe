#ifndef FREEFRAMETESTER_H
#define FREEFRAMETESTER_H

#include <QObject>
#include <QtTest/QtTest>
#include <QLibrary>
#include <QString>

class QOpenGLWindow;

class FreeframeParamsTester : public QObject
{
	Q_OBJECT

private slots:
	void initTestCase( void );

	void isLibrary( void );

	void openLibrary( void );

	void function( void );

	void getInfo( void );

	void getPluginCaps( void );

	void initialise( void );

	void cleanupTestCase( void );

private:
	QString			 mPluginFilename;
	QOpenGLWindow	*mWindow;
};

#endif // FREEFRAMETESTSER_H
