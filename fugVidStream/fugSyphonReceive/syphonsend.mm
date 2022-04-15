#include "syphonsend.h"

#import <Syphon/Syphon.h>
#import <Syphon/SyphonServer.h>

SyphonSend::SyphonSend( void )
	: mServer( 0 )
{
}

SyphonSend::~SyphonSend( void )
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	[(SyphonServer *)mServer stop];
	[(SyphonServer *)mServer release];

	[pool drain];
}

void SyphonSend::start( const char *pServerName, void *pContext )
{
	NSAutoreleasePool	*pool = [[NSAutoreleasePool alloc] init];

	NSString			*ServerName = [NSString stringWithUTF8String:pServerName];

	mServer = [[SyphonServer alloc] initWithName:ServerName context:(CGLContextObj)pContext options:nil];

	[pool drain];
}

void SyphonSend::stop()
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	[(SyphonServer *)mServer stop];
	[(SyphonServer *)mServer release];

	[pool drain];

	mServer = 0;
}


bool SyphonSend::hasClients()
{
	if( mServer == 0 )
	{
		return( false );
	}

	return( [(SyphonServer *)mServer hasClients] );
}

void SyphonSend::publishFrame( int pTexId, int pTarget, int pRegW, int pRegH, int pTexW, int pTexH, bool pFlipped)
{
	if( mServer == 0 )
	{
		return;
	}

	NSRect		Region;
	NSSize		TexSze;

	Region.origin.x = 0;
	Region.origin.y = 0;
	Region.size.width = pRegW;
	Region.size.height = pRegH;

	TexSze.width = pTexW;
	TexSze.height = pTexH;

	[(SyphonServer *)mServer publishFrameTexture:pTexId textureTarget:pTarget imageRegion:Region textureDimensions:TexSze flipped:pFlipped ];
}
