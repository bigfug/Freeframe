
#pragma once

#include <InstanceBase.h>
#include "Plugin.h"
#include <fugVidStream.h>

class Instance : public InstanceBase
{
protected:
	Plugin					&mPlugin;
	fugVideoInfo			 mVidInf;
	size_t					 mVidSze;
	unsigned char			*mVidBuf;
	HVIDSTREAM				 mDecoder;
	fugStreamProtocol		 mLastProtocol;
	bool					 mLastReceive;
	unsigned short			 mLastPort;

public:
	Instance( Plugin &pPlugin, FreeFrameApiVersion pApiVersion, plugMainUnion &pResult );

	virtual ~Instance( void );

	virtual void processFrame( plugMainUnion &pResult );
};
