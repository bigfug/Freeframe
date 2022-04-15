#ifndef SHADER_H
#define SHADER_H

#include "FFGL.h"

class Shader
{
public:
	Shader( void );

	virtual ~Shader( void );

	void load( const char *pV, const char *pF, const GLint *pLV = nullptr, const GLint *pLF = nullptr );

	inline GLuint program( void )
	{
		return( mProgramId );
	}

private:
	GLuint						 mVertexShaderId;
	GLuint						 mFragmentShaderId;
	GLuint						 mProgramId;
};

#endif // SHADER_H
