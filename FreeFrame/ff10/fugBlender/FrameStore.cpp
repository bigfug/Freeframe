#include "StdAfx.h"
#include "FrameStore.h"

FrameStore::FrameStore( ffFrameBuffer *pFrameBuffer )
: mFrameBuffer( pFrameBuffer )
{

}

FrameStore::~FrameStore(void)
{
	BYTE *FramePtr;

	while( ( FramePtr = mFrames.front() ) != NULL )
	{
		delete[] FramePtr;

		mFrames.erase( mFrames.begin() );
	}
}

bool FrameStore::addFrame( const BYTE *pFrame )
{
	BYTE *FramePtr = new BYTE[ mFrameBuffer->getFrameByteSize() ];

	if( FramePtr == NULL )
	{
		return( false );
	}

	memcpy( FramePtr, pFrame, mFrameBuffer->getFrameByteSize() );

	mFrames.push_back( FramePtr );

	return( true );
}

bool FrameStore::delFrame( const int pIndex )
{
	vector<BYTE *>::iterator	Where = mFrames.begin();
	int							i;

	for( i = 0 ; Where != mFrames.end() ; Where++, i++ )
	{
		if( i == pIndex )
		{
			delete[] mFrames[ pIndex ];

			mFrames.erase( Where );

			return( true );
		}
	}

	return( false );
}

BYTE *FrameStore::getFirstFrame( void ) const
{
	return( mFrames.front() );
}

BYTE *FrameStore::getLastFrame( void ) const
{
	return( mFrames.back() );
}

BYTE *FrameStore::getFrame( const int pIndex ) const
{
	return( mFrames.at( pIndex ) );
}

size_t FrameStore::getFrameCount( void ) const
{
	return( mFrames.size() );
}
