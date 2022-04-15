#include "syphonoutput.h"

#include <AGL/agl.h>

SyphonOutput::SyphonOutput(QWidget *parent) :
	QGLWidget( parent ), mContext( 0 )
{
}

void SyphonOutput::initializeGL( void )
{
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	glClear( GL_COLOR_BUFFER_BIT );

	glewInit();
}

void SyphonOutput::resizeGL( int pWidth, int pHeight )
{
	Q_UNUSED( pWidth )
	Q_UNUSED( pHeight )

	glClear( GL_COLOR_BUFFER_BIT );
}

void SyphonOutput::paintGL( void )
{
	glViewport( 0, 0, size().width(), size().height() );

	if( mContext == 0 )
	{
		mContext = CGLGetCurrentContext();
	}

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glBindTexture( GL_TEXTURE_2D, mTexToBuf.getTex() );

	glEnable( GL_TEXTURE_2D );

	const FFGLTextureStruct		&TEX = mTexToBuf.texInfo();

	float		tx = float( TEX.Width  ) / float( TEX.HardwareWidth  );
	float		ty = float( TEX.Height ) / float( TEX.HardwareHeight );

	glBegin( GL_QUADS );
		glTexCoord2f( 0.0f, ty );
		glVertex2f( -1.0f, -1.0f );
		glTexCoord2f( tx, ty );
		glVertex2f(  1.0f, -1.0f );
		glTexCoord2f( tx, 0.0f );
		glVertex2f(  1.0f,  1.0f );
		glTexCoord2f( 0.0f, 0.0f );
		glVertex2f( -1.0f,  1.0f );
	glEnd();

	glBindTexture( GL_TEXTURE_2D, 0 );

	glDisable( GL_TEXTURE_2D );
}

void SyphonOutput::updateTexture( const fugVideoInfo &pVidInf, const unsigned char *pVidDat )
{
	VideoInfoStruct		VIS;

	VIS.FrameWidth  = pVidInf.frameWidth;
	VIS.FrameHeight = pVidInf.frameHeight;
	VIS.BitDepth    = pVidInf.bitDepth;
	VIS.Orientation = pVidInf.orientation;

	mTexToBuf.genTex( VIS, pVidDat );

	updateGL();
}

GLuint SyphonOutput::texId()
{
	return( mTexToBuf.getTex() );
}
