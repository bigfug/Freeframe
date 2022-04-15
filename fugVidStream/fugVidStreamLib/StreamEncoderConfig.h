
#pragma once

#include <string>
#include "fugVidStream.h"

using namespace std;

class StreamEncoderConfig
{
protected:

public:
	StreamEncoderConfig( string pConfig );

	virtual ~StreamEncoderConfig( void );
};
