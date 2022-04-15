#ifndef FREEFRAMETESTER_H
#define FREEFRAMETESTER_H

#include <QObject>
#include <QtTest/QtTest>
#include <QLibrary>
#include <QString>

#include <QOpenGLWindow>
#include <QFile>

#include "FFGL.h"

class FreeframeRenderTester : public QObject
{
	Q_OBJECT

private slots:
	void initTestCase( void );

	void init( void );

	void render( void );

	void cleanup( void );

	void cleanupTestCase( void );

private:
	QString			 mPluginFilename;
	QOpenGLWindow	*mWindow;
	QLibrary		*mLibrary;
	FF_Main_FuncPtr	 mFP;
	void			*mInstance;
};

#endif // FREEFRAMETESTSER_H
