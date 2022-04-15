
#pragma once

#include <InstanceBase.h>
#include <fugVidStream.h>
#include "Plugin.h"

class Instance : public InstanceBase
{
protected:
	Plugin	&mPlugin;
	HVIDSTREAM				 mDecoder;
	fugStreamProtocol		 mLastProtocol;
	bool					 mLastReceive;
	unsigned short			 mLastPort;
	FFGLTextureStruct		 mTexInf;
	fugVideoInfo			 mVidInf;
	fugVideoInfo			 mPrvInf;
	GLint					 mInternalFormat;
	GLint					 mFormat;
	GLuint					 mPBO;

public:
	Instance( Plugin &pPlugin, FreeFrameApiVersion pApiVersion, plugMainUnion &pResult );

	virtual ~Instance( void );

	virtual void processOpengl( plugMainUnion &pResult );
};
