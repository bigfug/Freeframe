/*

The MIT License

Copyright (c) 2007 Alex May - www.bigfug.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#pragma once

#include "ImageConvert.h"
#include "StreamEncoder.h"
#include "StreamMemory.h"

class StreamEncoderMemory :	public StreamEncoder
{
protected:
	char					 mSharedName[ MAX_PATH ];
#ifdef USE_WINDOWS_SHARED_MEMORY
	windows_shared_memory	*mSharedMemory;
#else
	shared_memory_object	 mSharedMemory;
#endif
	mapped_region			 mMappedRegion;
	MemoryHeader			*mMemHdr;
	unsigned short			 mCurrPort;

public:
	StreamEncoderMemory( const fugVideoInfo &pSrcInfo );
	virtual ~StreamEncoderMemory( void );

protected:
	bool openMemory( const size_t pSize );
	void freeMemory( void );

	virtual void encode( const void *pData, const size_t pSize );
};
