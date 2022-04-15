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

#ifndef	__FF_FRAME_BUFFER__
#define __FF_FRAME_BUFFER__

#include "FreeFrame.h"
#include "FreeFramePlugin.h"

class ffFrameBuffer
{
protected:
	DWORD		Width;
	DWORD		Height;
public:
	ffFrameBuffer( DWORD pWidth, DWORD pHeight )
		: Width( pWidth ), Height( pHeight )
	{
	}

	void DrawLine( VideoFrame OutputFrame, int x1, int y1, int x2, int y2, VideoPixel32bit *Pixel );
	void DrawLine( VideoFrame OutputFrame, int x1, int y1, int x2, int y2, VideoPixel32bit *Pixel, float Distance );
	void DrawLine32bit( VideoPixel32bit *pOutput, int x1, int y1, const int x2, const int y2, const VideoPixel32bit &Pixel );
	void DrawLine32bit( VideoPixel32bit *pOutput, int x1, int y1, const int x2, const int y2, const VideoPixel32bit &Pixel, const float Distance );

	//void convolve( VideoPixel32bit *pInput, VideoPixel32bit *pOutput, const float pMatrix[], const DWORD pWidth, const DWORD pHeight );
	
	void convolve( VideoPixel32bit *pInput, VideoPixel32bit *pOutput, const int pMatrix[], const DWORD pWidth, const DWORD pHeight );
	void convolve8bit( CHAR *pInput, const DWORD pImgWidth, const DWORD pImgHeight, CHAR *pOutput, const int pMatrix[], const DWORD pWidth, const DWORD pHeight );

	virtual HRESULT			 getPixel( VideoFrame Buffer, DWORD x, DWORD y, VideoPixel32bit *Pixel ) = 0;
	virtual HRESULT			 setPixel( VideoFrame Buffer, DWORD x, DWORD y, VideoPixel32bit *Pixel ) = 0;
	virtual	BYTE			*getOffset( VideoFrame Buffer, DWORD Offset ) = 0;
	virtual	DWORD			 getPixelSize( void ) const = 0;
	virtual DWORD			 getBitDepth( void ) const = 0;
	virtual DWORD			 getFreeFrameBitDepth( void ) const = 0;

	void convert16to24( VideoPixel16bit *pInput, VideoPixel24bit *pOutput );
	void convert16to32( VideoPixel16bit *pInput, VideoPixel32bit *pOutput );
	void convert24to16( VideoPixel24bit *pInput, VideoPixel16bit *pOutput );
	void convert24to32( VideoPixel24bit *pInput, VideoPixel32bit *pOutput );
	void convert32to16( VideoPixel32bit *pInput, VideoPixel16bit *pOutput );
	void convert32to24( VideoPixel32bit *pInput, VideoPixel24bit *pOutput );

	void toLuma( VideoPixel32bit *pSrcPtr );
	void toLuma( VideoPixel32bit *pSrcPtr,  VideoPixel32bit *pDstPtr );
	void ffFrameBuffer::toLuma( VideoPixel32bit *pSrcPtr, BYTE *pDstPtr );

	void toBrightness( VideoPixel32bit *pSrcPtr, BYTE *pDstPtr );

	void toHue( VideoPixel32bit *pSource, VideoPixel8bit *pOutput );

	void clear( VideoPixel32bit *pFrame );

	void RGB2HSL( VideoPixel32bit *pSource, VideoPixelHSL *pOutput );
	void HSL2RGB( VideoPixelHSL *pSource, VideoPixel32bit *pOutput );

	void resizeNearestNeighbour( VideoPixel32bit *pSrcPtr, VideoPixel32bit *pDstPtr, const int NewWidth, const int NewHeight );
	void resizeBilinear( VideoPixel32bit *pSrcPtr, VideoPixel32bit *pDstPtr, const int NewWidth, const int NewHeight );

	DWORD getWidth( void ) const { return( this->Width ); }
	DWORD getHeight( void ) const { return( this->Height ); }
	DWORD getPixelCount( void ) const { return( this->Width * this->Height ); }
	DWORD getLineByteSize( void ) const { return( this->Width * this->getPixelSize() ); }
	DWORD getFrameByteSize( void ) const { return( this->Width * this->Height * this->getPixelSize() ); }

	void copyFrame( VideoFrame pSrc, VideoFrame pDst );

	static int getFreeFrameDepthFromBits( const int pDepth );

	void flipV( VideoFrame pFrame );
	void flipH( VideoFrame pFrame );

	void flipV( VideoFrame pSrc, VideoFrame pDst );
	void flipH( VideoFrame pSrc, VideoFrame pDst );
};


class ffFrameBufferFactory
{
public:
	static ffFrameBuffer *getInstance( const VideoInfoStruct *VideoInfo );
};

#endif