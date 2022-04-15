
#include "Instance.h"

#include <algorithm>

#define STRINGIFY(X) STRINGIFY2(X)
#define STRINGIFY2(X) #X

# define U_AMOUNT "Amount"
# define U_DIRECTION "Direction"
# define U_FADE "Fade"
# define U_LUMMAX "LumMax"
# define U_LUMMIN "LumMin"
# define U_MODE "Mode"
# define U_ROTATION "Rotation"
# define U_TEXBAK "TexBAK"
# define U_TEXRGBA "TexRGBA"
# define U_ZOOM "Zoom"
# define U_HUEROTATE "HueRotate"
# define U_MODX "ModX"
# define U_MODY "ModY"

#if defined( _DEBUG )
//#if defined( TARGET_OS_WIN )
//#define	SHADER_BASE_PATH		"E:\\dev\\Projects\\freeframe\\freeframe\\FreeFrame\\ffgl\\fugFeedbackGL\\"
//#endif

//#if defined( TARGET_OS_MAC )
//#define	SHADER_BASE_PATH		"/Users/alex/Documents/dev/freeframe/freeframe/FreeFrame/ffgl/fugFeedbackGL/"
//#endif

#define SHADER_PATH(x)	SHADER_BASE_PATH #x

#else
#include STRINGIFY( FF_SHADER_VERT_H )
#include STRINGIFY( FF_SHADER_FRAG_H )
#endif

using namespace std;

#if !defined(Q_UNUSED)
#define Q_UNUSED(arg) (void)arg;
#endif

Instance::Instance( Plugin &pPlugin, FreeFrameApiVersion pApiVersion, plugMainUnion &pResult )
	: InstanceBase( pPlugin, pApiVersion, pResult ), mPlugin( pPlugin ), mTexRGBA( -1 ), mTexFBO( 0 ), mTexDst( 1 )
{
	memset( &mTexBuf1, 0, sizeof( mTexBuf1 ) );
	memset( &mTexBuf2, 0, sizeof( mTexBuf2 ) );

#if defined( USE_DRAWTEXT )
	if( ( mFont = dtx_open_font( "E:\\dev\\ArcadeClassic.ttf", 24 ) ) == 0 )
	{
		fprintf(stderr, "failed to open font\n");

		return;
	}

	dtx_use_font( mFont, 24 );
#endif

#if defined( _DEBUG )
	mShader.load( SHADER_PATH( Generic.vert ), SHADER_PATH( fugFeedbackGL.frag ) );
#else
	const GLint VL = GLint( FF_SHADER_VERT_LEN );
	const GLint FL = GLint( FF_SHADER_FRAG_LEN );

	mShader.load( reinterpret_cast<const char *>( FF_SHADER_VERT ), reinterpret_cast<const char *>( FF_SHADER_FRAG ), &VL, &FL );
#endif

	if( mShader.program() == 0 )
	{
		return;
	}

	glUseProgram( mShader.program() );

	if( ( mTexBAK = glGetUniformLocation( mShader.program(), U_TEXBAK ) ) != -1 )
	{
		glUniform1i( mTexBAK, 0 );
	}

	if( ( mTexRGBA = glGetUniformLocation( mShader.program(), U_TEXRGBA ) ) != -1 )
	{
		glUniform1i( mTexRGBA, 1 );
	}

	glUseProgram( 0 );

#if defined( USE_TMP_TEX )
	mFboTmp = 0;
	mTexTmp.Handle = 0;
#endif
}

Instance::~Instance( void )
{
#if defined( USE_TMP_TEX )
	if( mFboTmp != 0 )
	{
		glDeleteFramebuffers( 1, &mFboTmp );

		mFboTmp = 0;
	}

	if( mTexTmp.Handle != 0 )
	{
		glDeleteTextures( 1, &mTexTmp.Handle );

		mTexTmp.Handle = 0;
	}
#endif

	if( mTexFBO != 0 )
	{
		glDeleteFramebuffers( 1, &mTexFBO );

		mTexFBO = 0;
	}

	if( mTexBuf1.Handle != 0 )
	{
		glDeleteTextures( 1, &mTexBuf1.Handle );

		mTexBuf1.Handle = 0;
	}

	if( mTexBuf2.Handle != 0 )
	{
		glDeleteTextures( 1, &mTexBuf2.Handle );

		mTexBuf2.Handle = 0;
	}

	if( mVertexBufferId  )
	{
		glDeleteBuffers( 1, &mVertexBufferId );

		mVertexBufferId = 0;
	}

#if defined( USE_DRAWTEXT )
	if( mFont != 0 )
	{
		dtx_close_font( mFont );

		mFont = 0;
	}
#endif
}

void Instance::processOpengl( plugMainUnion &pResult )
{
	const int	 ParamMode		= mParams[ PARAM_MODE ].getChoice();
	const float			 ParamLumaMin	= mParams[ PARAM_LUMA_MIN ].getFloat();
	const float			 ParamLumaMax	= mParams[ PARAM_LUMA_MAX ].getFloat();
	const float			 ParamRotation	= mParams[ PARAM_ROTATION ].getFloat();
	const float			 ParamZoom		= mParams[ PARAM_ZOOM ].getFloat();
	const float			 ParamDirection	= mParams[ PARAM_DIRECTION ].getFloat();
	const float			 ParamAmount	= mParams[ PARAM_AMOUNT ].getFloat();
	const float			 ParamFade		= mParams[ PARAM_FADE ].getFloat();
	const float			 ParamHueRotate	= mParams[ PARAM_HUE_ROTATE ].getFloat();

#if defined( FF_VERSION_15 )
	ProcessOpenGLStruct		*GL = reinterpret_cast<ProcessOpenGLStruct *>( pResult.ivalue );
#else
	ProcessOpenGLStruct		*GL = reinterpret_cast<ProcessOpenGLStruct *>( pResult.PointerValue );
#endif

	Plugin::checkError();

	pResult.ivalue = FF_FAIL;

	if( !GL )
	{
		return;
	}

	if( GL->numInputTextures != 1 )
	{
		return;
	}

	if( !GL->inputTextures[ 0 ] )
	{
		return;
	}

	//-------------------------------------------------------------------------

#if defined( USE_TMP_TEX )
	if( mFboTmp == 0 )
	{
		glGenFramebuffers( 1, &mFboTmp );

		if( mFboTmp == 0 )
		{
			return;
		}
	}

	//-------------------------------------------------------------------------

	if( !createFBO( *GL->inputTextures[ 0 ], mTexTmp, mFboTmp, GL->HostFBO ) )
	{
		return;
	}

	//-------------------------------------------------------------------------

	glBindFramebuffer( GL_FRAMEBUFFER, mFboTmp );

	glViewport( 0, 0, mTexTmp.HardwareWidth, mTexTmp.HardwareHeight );

	glEnable( GL_TEXTURE_2D );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, GL->inputTextures[ 0 ]->Handle );

	if( true )
	{
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho( 0, mTexTmp.HardwareWidth, 0, mTexTmp.HardwareHeight, -1, +1 );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

		float	x0 = 0.0f;
		float	y0 = 0.0f;
		float	x1 = 1.0f; //GLfloat( GL->inputTextures[ 0 ]->Width  ) / GLfloat( GL->inputTextures[ 0 ]->HardwareWidth  );
		float	y1 = 1.0f; //GLfloat( GL->inputTextures[ 0 ]->Height ) / GLfloat( GL->inputTextures[ 0 ]->HardwareHeight );

		glBegin( GL_QUADS );
			glTexCoord2f( x0, y0 );
			glVertex2f( 0, 0 );
			glTexCoord2f( x1, y0 );
			glVertex2f( mTexTmp.HardwareWidth, 0 );
			glTexCoord2f( x1, y1 );
			glVertex2f( mTexTmp.HardwareWidth, mTexTmp.HardwareHeight );
			glTexCoord2f( x0, y1 );
			glVertex2f( 0, mTexTmp.HardwareHeight );
		glEnd();
	}

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );

	glBindFramebuffer( GL_FRAMEBUFFER, GL->HostFBO );

#endif

	//-------------------------------------------------------------------------

#if defined( USE_TMP_TEX )
	FFGLTextureStruct		&InputTexture = mTexTmp;
#else
	FFGLTextureStruct		&InputTexture = *GL->inputTextures[ 0 ];
#endif

	//-------------------------------------------------------------------------

	if( !mTexFBO )
	{
		glGenFramebuffers( 1, &mTexFBO );

		if( !mTexFBO )
		{
			return;
		}
	}

	//-------------------------------------------------------------------------

	if( !createFBO( InputTexture, mTexBuf1, mTexFBO, GL->HostFBO ) )
	{
		return;
	}

	//-------------------------------------------------------------------------

	if( !createFBO( InputTexture, mTexBuf2, mTexFBO, GL->HostFBO ) )
	{
		return;
	}

	//-------------------------------------------------------------------------

	FFGLTextureStruct &SourceTexture = mTexDst == 1 ? mTexBuf2 : mTexBuf1;
	FFGLTextureStruct &OutputTexture = mTexDst == 1 ? mTexBuf1 : mTexBuf2;

	glBindFramebuffer( GL_FRAMEBUFFER, mTexFBO );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, OutputTexture.Handle, 0 );

	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

	if( status != GL_FRAMEBUFFER_COMPLETE )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		return;
	}

	glViewport( 0, 0, OutputTexture.HardwareWidth, OutputTexture.HardwareHeight );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, SourceTexture.Handle );

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, InputTexture.Handle );

	if( mShader.program() )
	{
		glUseProgram( mShader.program() );

		GLint		Mode;

		if( ( Mode = glGetUniformLocation( mShader.program(), U_MODE ) ) != -1 )
		{
			glUniform1i( Mode, ParamMode );
		}

		GLint		LumaMin;

		if( ( LumaMin = glGetUniformLocation( mShader.program(), U_LUMMIN ) ) != -1 )
		{
			glUniform1f( LumaMin, ParamLumaMin );
		}

		GLint		LumaMax;

		if( ( LumaMax = glGetUniformLocation( mShader.program(), U_LUMMAX ) ) != -1 )
		{
			glUniform1f( LumaMax, ParamLumaMax );
		}

		GLint		Rotation;

		if( ( Rotation = glGetUniformLocation( mShader.program(), U_ROTATION ) ) != -1 )
		{
			glUniform1f( Rotation, ParamRotation );
		}

		GLint		Zoom;

		if( ( Zoom = glGetUniformLocation( mShader.program(), U_ZOOM ) ) != -1 )
		{
			glUniform1f( Zoom, ParamZoom );
		}

		GLint		Direction;

		if( ( Direction = glGetUniformLocation( mShader.program(), U_DIRECTION ) ) != -1 )
		{
			glUniform1f( Direction, ParamDirection );
		}

		GLint		Amount;

		if( ( Amount = glGetUniformLocation( mShader.program(), U_AMOUNT ) ) != -1 )
		{
			glUniform1f( Amount, ParamAmount );
		}

		GLint		Fade;

		if( ( Fade = glGetUniformLocation( mShader.program(), U_FADE ) ) != -1 )
		{
			glUniform1f( Fade, ParamFade );
		}

		GLint		HueRotate;

		if( ( HueRotate = glGetUniformLocation( mShader.program(), U_HUEROTATE ) ) != -1 )
		{
			glUniform1f( HueRotate, ParamHueRotate );
		}

		GLint		ModX;

		if( ( ModX = glGetUniformLocation( mShader.program(), U_MODX ) ) != -1 )
		{
			glUniform1f( ModX, GLfloat( SourceTexture.Width ) / GLfloat( SourceTexture.HardwareWidth ) );
		}

		GLint		ModY;

		if( ( ModY = glGetUniformLocation( mShader.program(), U_MODY ) ) != -1 )
		{
			glUniform1f( ModY, GLfloat( SourceTexture.Height ) / GLfloat( SourceTexture.HardwareHeight ) );
		}
	}

	if( true )
	{
		drawFrame();

//		glMatrixMode( GL_PROJECTION );
//		glLoadIdentity();
//		glOrtho( 0, OutputTexture.HardwareWidth, 0, OutputTexture.HardwareHeight, -1, +1 );

//		glMatrixMode( GL_MODELVIEW );
//		glLoadIdentity();

//		float	x00 = 0.0f;
//		float	y00 = 0.0f;
//		float	x01 = 1.0f; //GLfloat( SourceTexture.Width  ) / GLfloat( SourceTexture.HardwareWidth  );
//		float	y01 = 1.0f; //GLfloat( SourceTexture.Height ) / GLfloat( SourceTexture.HardwareHeight );

//		float	x10 = 0.0f;
//		float	y10 = 0.0f;
//		float	x11 = 1.0f; //GLfloat( InputTexture->Width  ) / GLfloat( InputTexture->HardwareWidth  );
//		float	y11 = 1.0f; //GLfloat( InputTexture->Height ) / GLfloat( InputTexture->HardwareHeight );

//		glBegin( GL_QUADS );
//			glMultiTexCoord2f( GL_TEXTURE0, x00, y00 );
//			glMultiTexCoord2f( GL_TEXTURE1, x10, y10 );
//			glVertex2f( 0, 0 );
//			glMultiTexCoord2f( GL_TEXTURE0, x01, y00 );
//			glMultiTexCoord2f( GL_TEXTURE1, x11, y10 );
//			glVertex2f( OutputTexture.HardwareWidth, 0 );
//			glMultiTexCoord2f( GL_TEXTURE0, x01, y01 );
//			glMultiTexCoord2f( GL_TEXTURE1, x11, y11 );
//			glVertex2f( OutputTexture.HardwareWidth, OutputTexture.HardwareHeight );
//			glMultiTexCoord2f( GL_TEXTURE0, x00, y01 );
//			glMultiTexCoord2f( GL_TEXTURE1, x10, y11 );
//			glVertex2f( 0, OutputTexture.HardwareHeight );
//		glEnd();
	}

	if( mShader.program() )
	{
		glUseProgram( 0 );
	}

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, 0 );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );

	glBindFramebuffer( GL_FRAMEBUFFER, GL->HostFBO );

	//-------------------------------------------------------------------------

//	glEnable( GL_TEXTURE_2D );

	glViewport( mViewport.x, mViewport.y, mViewport.width, mViewport.height );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, OutputTexture.Handle );
	//glBindTexture( GL_TEXTURE_2D, InputTexture.Handle );

	if( true )
	{
		drawFrame();

//		glMatrixMode( GL_PROJECTION );
//		glLoadIdentity();

//		glOrtho( mViewport.x, mViewport.x + mViewport.width, mViewport.y, mViewport.y + mViewport.height, -1, +1 );

//		glMatrixMode( GL_MODELVIEW );
//		glLoadIdentity();

		float	x0 = 0.0f;
		float	y0 = 0.0f;
		float	x1 = GLfloat( OutputTexture.Width  ) / GLfloat( OutputTexture.HardwareWidth  );
		float	y1 = GLfloat( OutputTexture.Height ) / GLfloat( OutputTexture.HardwareHeight );

//		glBegin( GL_QUADS );
//			glTexCoord2f( x0, y0 );
//			glVertex2f( 0, 0 );
//			glTexCoord2f( x1, y0 );
//			glVertex2f( GLfloat( OutputTexture.Width ), 0 );
//			glTexCoord2f( x1, y1 );
//			glVertex2f( GLfloat( OutputTexture.Width ), GLfloat( OutputTexture.Height ) );
//			glTexCoord2f( x0, y1 );
//			glVertex2f( 0, GLfloat( OutputTexture.Height ) );
//		glEnd();
	}

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );

	//-------------------------------------------------------------------------

#if defined( USE_DRAWTEXT )
	GLdouble	w = GLdouble( mViewport.width  );
	GLdouble	h = GLdouble( mViewport.height );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glOrtho( -w/2., w/2., -h/2., h/2., -1, 1);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glTranslatef( 0, 0, 0 );

	glColor3f( 1, 0, 1 );

	char		buf[ 512 ];

	sprintf( buf, "w=%d h=%d\nw=%d h=%d\n\nw=%d h=%d\nw=%d h=%d", InputTexture.Width, InputTexture.Height, InputTexture.HardwareWidth, InputTexture.HardwareHeight, OutputTexture.Width, OutputTexture.Height, OutputTexture.HardwareWidth, OutputTexture.HardwareHeight );

	dtx_string( buf );
#endif

	//-------------------------------------------------------------------------

	mTexDst = ( mTexDst == 1 ? 2 : 1 );

	pResult.ivalue = FF_SUCCESS;

//	static bool	PntDat = true;

//	if( PntDat )
//	{
//		FILE		*F;

//		if( ( F = fopen( "E:\\dev\\fugFeedbackGL.txt", "w" ) ) != nullptr )
//		{
//			fprintf( F, "VP: x=%d y=%d w=%d h=%d\n\n", mViewport.x, mViewport.y, mViewport.width, mViewport.height );

//			fprintf( F, "InputTexture.Width=%d InputTexture.Height=%d\nInputTexture.HardwareWidth=%d InputTexture.HardwareHeight=%d\n\nOutputTexture.Width=%d OutputTexture.Height=%d\nOutputTexture.HardwareWidth=%d OutputTexture.HardwareHeight=%d", InputTexture.Width, InputTexture.Height, InputTexture.HardwareWidth, InputTexture.HardwareHeight, OutputTexture.Width, OutputTexture.Height, OutputTexture.HardwareWidth, OutputTexture.HardwareHeight );

//			fclose( F );
//		}

//		PntDat = false;
//	}
}

bool Instance::createFBO( FFGLTextureStruct &pTexSrc, FFGLTextureStruct &pTexDst, GLuint pFboSrc, GLuint pFboDst )
{
	if( !pTexDst.Handle || pTexDst.HardwareWidth != pTexSrc.HardwareWidth || pTexDst.HardwareHeight != pTexSrc.HardwareHeight )
	{
#if defined( TARGET_OS_WIN )
		updateTexture( &pTexSrc, pTexDst, GL_RGBA8, GL_BGRA );
#endif

#if defined( TARGET_OS_MAC )
		updateTexture( &pTexSrc, pTexDst, GL_RGBA8, GL_RGBA );
#endif

		if( !pTexDst.Handle )
		{
			return( false );
		}

		glBindFramebuffer( GL_FRAMEBUFFER, pFboSrc );

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pTexDst.Handle, 0 );

		GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

		if( status != GL_FRAMEBUFFER_COMPLETE )
		{
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );

			return( false );
		}

		glClearColor( 0.0, 0.0, 0.0, 0.0 );

		glClear( GL_COLOR_BUFFER_BIT );

		glBindFramebuffer( GL_FRAMEBUFFER, pFboDst );
	}

	return( true );
}
