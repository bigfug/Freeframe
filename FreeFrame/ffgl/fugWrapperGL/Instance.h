
#pragma once

#include <InstanceBase.h>
#include "Plugin.h"
#include <TexToBuf.h>

class Instance : public InstanceBase
{
protected:
	Plugin					&mPlugin;
	void					*mInstance;
	VideoInfoStruct			 mVidInf;
	vector<TexToBuf>		 mTexToBuf;
	TexToBuf				 mOutput;

public:
	Instance( Plugin &pPlugin, plugMainUnion &pResult );

	virtual ~Instance( void );

	virtual void getParameterDisplay( plugMainUnion &pResult ) override;
	virtual void setParameter( plugMainUnion &pResult );
	virtual void getParameter( plugMainUnion &pResult );

	virtual void getInputStatus( plugMainUnion &pResult );

#if !defined( FF_VERSION_10 )
	virtual void processOpengl( plugMainUnion &pResult );

	virtual void setTime( plugMainUnion &pResult );
#endif
};
