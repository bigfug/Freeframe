#include "freeframe_params_tester.h"

#include <QOpenGLWindow>
#include <QFile>

#include "FFGL.h"

#define Q(x) #x
#define QUOTE(x) Q(x)

void FreeframeParamsTester::initTestCase()
{
	mPluginFilename = QDir( QUOTE( PLUGIN_PATH ) ).absoluteFilePath( QUOTE( PLUGIN_FILENAME ) );

	qInfo() << mPluginFilename;

	QVERIFY( QFile::exists( mPluginFilename ) );

	mWindow = new QOpenGLWindow();

	mWindow->show();
}

void FreeframeParamsTester::isLibrary()
{
#if defined( Q_OS_MAC )
	QSKIP( "QLibrary::isLibrary doesn't apply on macOS" );
#endif

	QVERIFY( QLibrary::isLibrary( mPluginFilename ) );
}

void FreeframeParamsTester::openLibrary()
{
	QLibrary		FF( mPluginFilename );

	FF.load();

	QVERIFY( FF.isLoaded() );

	FF.unload();

	QVERIFY( !FF.isLoaded() );
}

void FreeframeParamsTester::function()
{
	QLibrary			FF( mPluginFilename );

	FF_Main_FuncPtr		FP = reinterpret_cast<FF_Main_FuncPtr>( FF.resolve( "plugMain" ) );

	QVERIFY( FP );
}

void FreeframeParamsTester::getInfo()
{
	QLibrary			FF( mPluginFilename );

	FF_Main_FuncPtr		FP = reinterpret_cast<FF_Main_FuncPtr>( FF.resolve( "plugMain" ) );

	FFMixed				InputValue = { 0 };

	PluginInfoStruct	*IS = reinterpret_cast<PluginInfoStruct *>( FP( FF_GETINFO, InputValue, Q_NULLPTR ).PointerValue );

	QVERIFY( IS );
}

void FreeframeParamsTester::getPluginCaps()
{
	QLibrary			FF( mPluginFilename );

	FF_Main_FuncPtr		FP = reinterpret_cast<FF_Main_FuncPtr>( FF.resolve( "plugMain" ) );

	FFMixed				InputValue = { 0 };

	InputValue.UIntValue = FF_CAP_16BITVIDEO;

	uint32_t			V = FP( FF_GETPLUGINCAPS, InputValue, Q_NULLPTR ).UIntValue;

	QCOMPARE( V, 0 );

	InputValue.UIntValue = FF_CAP_24BITVIDEO;

	V = FP( FF_GETPLUGINCAPS, InputValue, Q_NULLPTR ).UIntValue;

	QCOMPARE( V, 0 );

	InputValue.UIntValue = FF_CAP_32BITVIDEO;

	V = FP( FF_GETPLUGINCAPS, InputValue, Q_NULLPTR ).UIntValue;

	QCOMPARE( V, 0 );

	InputValue.UIntValue = FF_CAP_PROCESSOPENGL;

	V = FP( FF_GETPLUGINCAPS, InputValue, Q_NULLPTR ).UIntValue;

	QCOMPARE( V, FF_SUPPORTED );
}

void FreeframeParamsTester::initialise()
{
	QLibrary			FF( mPluginFilename );

	FF_Main_FuncPtr		FP = reinterpret_cast<FF_Main_FuncPtr>( FF.resolve( "plugMain" ) );

	FFMixed				InputValue = { 0 };

	uint32_t			V1 = FP( FF_INITIALISE, InputValue, Q_NULLPTR ).UIntValue;

	QCOMPARE( V1, FF_SUCCESS );

	uint32_t			V2 = FP( FF_DEINITIALISE, InputValue, Q_NULLPTR ).UIntValue;

	QCOMPARE( V2, FF_SUCCESS );
}

void FreeframeParamsTester::cleanupTestCase()
{
	delete mWindow;
}

QTEST_MAIN( FreeframeParamsTester )

#include "freeframe_params_tester.moc"
