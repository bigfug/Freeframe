#ifndef INSTANCE_H
#define INSTANCE_H

#include "InstanceBase.h"

#include "Plugin.h"

#include "Shader.h"

#if defined( USE_DRAWTEXT )
extern "C"
{
#include "drawtext.h"
}
#endif

//#define USE_TMP_TEX

class Instance : public InstanceBase
{
public:
	Instance( Plugin &pPlugin, FreeFrameApiVersion pApiVersion, plugMainUnion &pResult );

	virtual ~Instance( void );

	virtual void processOpengl( plugMainUnion &pResult );

private:
	static bool createFBO( FFGLTextureStruct &pTexSrc, FFGLTextureStruct &pTexDst, GLuint pFboSrc, GLuint pFboDst );

protected:
	Plugin						&mPlugin;

	Shader						 mShader;

	GLint						 mTexBAK;
	GLint						 mTexRGBA;

#if defined( USE_TMP_TEX )
	FFGLTextureStruct			 mTexTmp;
	GLuint						 mFboTmp;
#endif

	FFGLTextureStruct			 mTexBuf1;
	FFGLTextureStruct			 mTexBuf2;

	GLuint						 mTexFBO;

	int							 mTexDst;

#if defined( USE_DRAWTEXT )
	struct dtx_font				*mFont;
#endif
};

#endif // INSTANCE_H
