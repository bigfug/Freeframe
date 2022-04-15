#include "freeframe_render_tester.h"

#include <QOpenGLFramebufferObject>
#include <QOpenGLTexture>

#define Q(x) #x
#define QUOTE(x) Q(x)

constexpr int BUFFER_WIDTH = 640;
constexpr int BUFFER_HEIGHT = 360;

void FreeframeRenderTester::initTestCase()
{
	mPluginFilename = QDir( QUOTE( PLUGIN_PATH ) ).absoluteFilePath( QUOTE( PLUGIN_FILENAME ) );

	qInfo() << mPluginFilename;

	QVERIFY( QFile::exists( mPluginFilename ) );

	QSurfaceFormat	Format = QSurfaceFormat::defaultFormat();

#if defined( FF_VERSION_16 )
	Format.setProfile( QSurfaceFormat::CoreProfile );
	Format.setMajorVersion( 2 );
	Format.setMinorVersion( 1 );
#elif defined( FF_VERSION_20 )
	Format.setProfile( QSurfaceFormat::CoreProfile );
	Format.setMajorVersion( 4 );
	Format.setMinorVersion( 0 );
#endif

	qInfo() << "Desired Version:" << Format.version();

	QSurfaceFormat::setDefaultFormat( Format );

	mWindow = new QOpenGLWindow();

	mWindow->show();

	QOpenGLContext		*C = QOpenGLContext::currentContext();

	qDebug() << "Reported Version:" << C->format().version();

	int		MajVer, MinVer;

#if defined( FF_VERSION_16 )
	MajVer = 2; MinVer = 1;
#elif defined( FF_VERSION_20 )
	MajVer = 4; MinVer = 1;
#endif

	QCOMPARE( C->format().version().first,  MajVer );
	QCOMPARE( C->format().version().second, MinVer );
}

void FreeframeRenderTester::init()
{
	mLibrary = new QLibrary( mPluginFilename );

	mFP = reinterpret_cast<FF_Main_FuncPtr>( mLibrary->resolve( "plugMain" ) );

	FFMixed				InputValue = { 0 };

	uint32_t			V = mFP( FF_INITIALISE, InputValue, Q_NULLPTR ).UIntValue;

	QCOMPARE( V, FF_SUCCESS );

	FFGLViewportStruct	VS;

	VS.x = 0;
	VS.y = 0;
	VS.width = BUFFER_WIDTH;
	VS.height = BUFFER_HEIGHT;

	InputValue.PointerValue = &VS;

	mInstance = mFP( FF_INSTANTIATEGL, InputValue, Q_NULLPTR ).PointerValue;

	QVERIFY( mInstance );
}

void FreeframeRenderTester::render()
{
	QOpenGLTexture					TEX( QOpenGLTexture::Target2D );

	TEX.setSize( BUFFER_WIDTH, BUFFER_HEIGHT );

	QOpenGLFramebufferObject		FBO( BUFFER_WIDTH, BUFFER_HEIGHT );

	FBO.bind();

	FFMixed				InputValue = { 0 };

	FFGLTextureStruct	Textures[ 1 ];

	Textures[ 0 ].Width   = TEX.width();
	Textures[ 0 ].Height  = TEX.height();
	Textures[ 0 ].Handle  = TEX.textureId();
	Textures[ 0 ].HardwareWidth = TEX.width();
	Textures[ 0 ].HardwareHeight = TEX.height();

	QVector<FFGLTextureStruct *>	TexPtr( 1 );

	TexPtr[ 0 ] = &Textures[ 0 ];

	ProcessOpenGLStruct	PS;

	PS.numInputTextures = 1;
	PS.inputTextures = TexPtr.data();
	PS.HostFBO = QOpenGLContext::currentContext()->defaultFramebufferObject();

	InputValue.PointerValue = &PS;

	mFP( FF_PROCESSOPENGL, InputValue, mInstance );

	FBO.bindDefault();

	FBO.toImage().save( "render.png" );
}

void FreeframeRenderTester::cleanup()
{
	FFMixed				InputValue = { 0 };

	InputValue.PointerValue = mInstance;

	uint32_t			V = mFP( FF_DEINSTANTIATEGL, InputValue, mInstance ).UIntValue;

	QCOMPARE( V, FF_SUCCESS );

	V = mFP( FF_DEINITIALISE, InputValue, mInstance ).UIntValue;

	QCOMPARE( V, FF_SUCCESS );

	mInstance = Q_NULLPTR;

	mFP = Q_NULLPTR;

	delete mLibrary;

	mLibrary = Q_NULLPTR;
}

void FreeframeRenderTester::cleanupTestCase()
{
	delete mWindow;
}

QTEST_MAIN( FreeframeRenderTester )

#include "freeframe_render_tester.moc"
