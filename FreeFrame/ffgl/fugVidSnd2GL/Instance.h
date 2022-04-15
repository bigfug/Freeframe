
#pragma once

#include <InstanceBase.h>
#include <fugVidStream.h>
#include "Plugin.h"
#include <vector>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "TexToBuf.h"

class Instance : public InstanceBase
{
public:
	Instance( Plugin &pPlugin, FreeFrameApiVersion pApiVersion, plugMainUnion &pResult );

	virtual ~Instance( void );

	virtual void processOpengl( plugMainUnion &pResult );

protected:
	Plugin						&mPlugin;
	HVIDSTREAM					 mEncoder;
	int							 mCurrConfig;
	std::string					 mSettingsFileName;
	fugVideoInfo				 mVidInf;
	vector<unsigned char>		 mVidBuf;
	boost::posix_time::ptime	 mLastSend;
	TexToBuf					 mTexToBuf;
};
