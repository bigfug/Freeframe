
#pragma once

#include <InstanceBase.h>
#include "Plugin.h"
#include <fugVidStream.h>

#include <boost/date_time/posix_time/posix_time.hpp>

class Instance : public InstanceBase
{
public:
	Instance( Plugin &pPlugin, FreeFrameApiVersion pApiVersion, plugMainUnion &pResult );

	virtual ~Instance( void );

	virtual void processFrame( plugMainUnion &pResult );

protected:
	Plugin						&mPlugin;
	HVIDSTREAM					 mEncoder;
	int							 mCurrConfig;
	std::string					 mSettingsFileName;
	std::vector<uint8_t>		 mConBuf;
	boost::posix_time::ptime	 mLastSend;
};
