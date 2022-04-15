#pragma once

#include "FreeFrame.h"
#include "FrameBuffer.h"
#include <vector>

using namespace std;

class FrameStore
{
protected:
	ffFrameBuffer		*mFrameBuffer;
	vector<BYTE *>		 mFrames;
public:
	FrameStore( ffFrameBuffer *pFrameBuffer );

	virtual ~FrameStore( void );

	bool addFrame( const BYTE *pFrame );
	bool delFrame( const int pIndex );

	BYTE *getFrame( const int pIndex ) const;
	BYTE *getFirstFrame( void ) const;
	BYTE *getLastFrame( void ) const;

	size_t getFrameCount( void ) const;
};
