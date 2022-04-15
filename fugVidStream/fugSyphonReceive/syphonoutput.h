#ifndef SYPHONOUTPUT_H
#define SYPHONOUTPUT_H

#include <GL/glew.h>

#include <QGLWidget>
#include <CGLCurrent.h>

#include "TexToBuf.h"
#include "fugVidStream.h"

class SyphonOutput : public QGLWidget
{
	Q_OBJECT
public:
	explicit SyphonOutput(QWidget *parent = 0);

	virtual void initializeGL( void );

	virtual void resizeGL( int pWidth, int pHeight );

	virtual void paintGL( void );

	inline void *cgl( void )
	{
		return( mContext );
	}

	void updateTexture( const fugVideoInfo &pVidInf, const unsigned char *pVidDat );

	inline GLuint texId( void );

	TexToBuf &texToBuf( void )
	{
		return( mTexToBuf );
	}

signals:

public slots:

private:
	CGLContextObj		 mContext;
	TexToBuf			 mTexToBuf;
};

#endif // SYPHONOUTPUT_H
