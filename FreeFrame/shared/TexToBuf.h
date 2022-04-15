
#pragma once

#include <PluginBase.h>
#include <FreeFrame.h>
#include <FFGL.h>

class TexToBuf
{
protected:
	VideoInfoStruct				mInf;
	FFGLTextureStruct			mTex;
	vector<unsigned char>		mBuf;
	vector<unsigned char>		mCnvBuf;
	GLuint						mTexPBO, mBufPBO;

	void resizeBuf( const size_t pSize );

public:
	TexToBuf( void );

	virtual ~TexToBuf( void );

	void fromTex( const FFGLTextureStruct &pTex, const GLuint pBitDepth );

#if 0
	void fromTexSized( const FFGLTextureStruct &pTex, const DWORD pBitDepth, const size_t pW, const size_t pH );
#endif

	const FFGLTextureStruct &texInfo( void )
	{
		return( mTex );
	}

	unsigned char *getBuf( void );

	GLuint getTex( void ) const;

	GLsizei getBufSize( void );

	void makeTex( const VideoInfoStruct &pBuf );

	void genTex( const VideoInfoStruct &pBuf, const void *pData = 0 );

	double getTexMaxCoordX( void ) const;
	double getTexMaxCoordY( void ) const;

	bool checkBuf( void );

	void freeGL( void );
};
