
#pragma once

#include "fugVidStream.h"

#define USING_COMPRESSION

#if defined( USING_COMPRESSION )
#include <turbojpeg.h>
#endif

class ImageCompression
{
protected:
	const fugVideoInfo		 mVidInf;
	size_t					 mSrcSze;
	size_t					 mPitchSze;
	bool					 mCompressor;
	uint16_t				 mLevel;

#if defined( USING_COMPRESSION )
	tjhandle				 mHandle;
	int						 mPixFmt;
	unsigned char			*mCmpBuf;
	unsigned long			 mCmpSze;
#endif

	ImageCompression( const fugVideoInfo &pVidInf );

public:
	static ImageCompression *getCompressor( const fugVideoInfo &pVidInf, uint16_t pLevel );
	static ImageCompression *getDecompressor( const fugVideoInfo &pVidInf );

	virtual ~ImageCompression( void );

	size_t compress( const void *pSrcDat, void *pDstDat );

	void decompress( const void *pSrcDat, const size_t pSrcSze, void *pDstDat );
};
