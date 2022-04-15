#ifndef INSTANCE_H
#define INSTANCE_H

#include <GL/glew.h>

#include <InstanceBase.h>
#include "Plugin.h"

#include <turbojpeg.h>
#include "TexToBuf.h"

class Instance : public InstanceBase
{
public:
	Instance( Plugin &pPlugin, FreeFrameApiVersion pApiVersion, plugMainUnion &pResult );

	virtual ~Instance( void );

	virtual void processOpengl( plugMainUnion &pResult );

private:

protected:
	Plugin						&mPlugin;
	tjhandle					 mCompressionHandle;
	tjhandle					 mDecompressionHandle;
	unsigned char				*mJpegBuffer;
	unsigned long				 mJpegBufferSize;
	vector<unsigned char>		 mGlitchOutput;
	TexToBuf					 mTexToBuf;
	TexToBuf					 mBufToTex;
};

#endif // INSTANCE_H
