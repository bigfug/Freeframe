
#pragma once

#include <InstanceBase.h>
#include "Plugin.h"

class Instance : public InstanceBase
{
protected:
	Plugin					&mPlugin;
	GLuint					 mProgram;
	vector<GLuint>			 mShaders;

	bool loadShaders( const vector<string> &pFileNames, const GLuint pType );

public:
	Instance( Plugin &pPlugin, plugMainUnion &pResult );

	virtual ~Instance( void );

	virtual void getParameterDisplay( plugMainUnion &pResult );

	virtual void processOpengl( plugMainUnion &pResult );
};
