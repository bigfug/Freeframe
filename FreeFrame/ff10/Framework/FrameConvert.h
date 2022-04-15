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

#include "FreeFrame.h"
#include "FrameBuffer.h"

typedef unsigned int UINT;
typedef unsigned char BYTE;

class FrameConvert
{
protected:
	ffFrameBuffer		*mFrameBufferSource;
	ffFrameBuffer		*mFrameBufferResized;
	ffFrameBuffer		*mFrameBufferOutput;

	VideoPixel32bit		*mFrameDataConverted;
	VideoPixel32bit		*mFrameDataResized;
public:
	FrameConvert( UINT pSrcWidth, UINT pSrcHeight, UINT pSrcDepth, UINT pDstWidth, UINT pDstHeight, UINT pDstDepth );
	FrameConvert( VideoInfoStruct &pSrcInfo, VideoInfoStruct &pDstInfo );
	~FrameConvert( void );

	void convert( BYTE *pSrcData, BYTE *pDstData );
	void convert( BYTE *pSrcData, BYTE **pDstPtr );

	const ffFrameBuffer *getInputFrame( void ) const
	{
		return( mFrameBufferSource );
	}

	const ffFrameBuffer *getOutputFrame( void ) const
	{
		return( mFrameBufferOutput );
	}
};
