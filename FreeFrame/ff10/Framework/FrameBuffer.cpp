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

#include <math.h>
#include "FrameBuffer.h"
#include <assert.h>

class ffFrameBuffer8 : public ffFrameBuffer
{
public:
	ffFrameBuffer8( DWORD Width, DWORD Height ) : ffFrameBuffer( Width, Height )
	{
	}

	HRESULT getPixel( VideoFrame Buffer, DWORD x, DWORD y, VideoPixel32bit *Pixel )
	{
		const		BYTE		*Frame  = (BYTE *)Buffer.Frame;
		const		LONG		 Offset = x + ( y * this->Width );

		assert( x < this->Width );
		assert( y < this->Height );

		Pixel->red   = Frame[ Offset ];
		Pixel->green = Frame[ Offset ];
		Pixel->blue  = Frame[ Offset ];
		Pixel->alpha = 255;

		return( FF_SUCCESS );
	}

	HRESULT setPixel( VideoFrame Buffer, DWORD x, DWORD y, VideoPixel32bit *Pixel )
	{
		BYTE		*Frame  = (BYTE *)Buffer.Frame;
		const		LONG		 Offset = x + ( y * this->Width );

		assert( x < this->Width );
		assert( y < this->Height );

		const int		Min = min( Pixel->red, min( Pixel->green, Pixel->blue ) );
		const int		Max = max( Pixel->red, max( Pixel->green, Pixel->blue ) );
		const int		MaxPlusMin = Max + Min;

		Frame[ Offset ] = MaxPlusMin / 2;

		return( FF_SUCCESS );
	}

	inline DWORD getPixelSize() const
	{
		return( sizeof(BYTE) );
	}

	DWORD getBitDepth( void ) const
	{
		return( 8 );
	}

	DWORD getFreeFrameBitDepth( void ) const
	{
		return( FF_CAP_8BITVIDEO );
	}

	inline BYTE *getOffset( VideoFrame Buffer, DWORD Offset )
	{
		return( &((BYTE *)Buffer.Frame)[ Offset ] );
	}
};

class ffFrameBuffer16 : public ffFrameBuffer
{
public:
	ffFrameBuffer16( DWORD Width, DWORD Height ) : ffFrameBuffer( Width, Height )
	{
	}

	HRESULT getPixel( VideoFrame Buffer, DWORD x, DWORD y, VideoPixel32bit *Pixel )
	{
		VideoPixel16bit		*Frame = (VideoPixel16bit *)Buffer.Vid16;
		LONG				 Offset;

		if( x > this->Width || y > this->Height )
		{
			return( FF_FAIL );
		}

		Offset = x + ( y * this->Width );

		Pixel->red   = (BYTE)( Frame[ Offset ].red   * 8 );
		Pixel->green = (BYTE)( Frame[ Offset ].green * 4 );
		Pixel->blue  = (BYTE)( Frame[ Offset ].blue  * 8 );
		Pixel->alpha = 255;

		return( FF_SUCCESS );
	}

	HRESULT setPixel( VideoFrame Buffer, DWORD x, DWORD y, VideoPixel32bit *Pixel )
	{
		VideoPixel16bit		*Frame = (VideoPixel16bit *)Buffer.Vid16;
		LONG				 Offset;

		if( x > this->Width || y > this->Height )
		{
			return( FF_FAIL );
		}

		Offset = x + ( y * this->Width );

		Frame[ Offset ].red   = Pixel->red   / 8;
		Frame[ Offset ].green = Pixel->green / 4;
		Frame[ Offset ].blue  = Pixel->blue  / 8;

		return( FF_SUCCESS );
	}

	inline DWORD getPixelSize() const
	{
		return( sizeof(VideoPixel16bit) );
	}

	DWORD getBitDepth( void ) const
	{
		return( 16 );
	}

	DWORD getFreeFrameBitDepth( void ) const
	{
		return( FF_CAP_16BITVIDEO );
	}

	inline BYTE *getOffset( VideoFrame Buffer, DWORD Offset )
	{
		return( (BYTE *)&Buffer.Vid16[ Offset ] );
	}
};

class ffFrameBuffer24 : public ffFrameBuffer
{
public:
	ffFrameBuffer24( DWORD Width, DWORD Height ) : ffFrameBuffer( Width, Height )
	{
	}

	HRESULT getPixel( VideoFrame Buffer, DWORD x, DWORD y, VideoPixel32bit *Pixel )
	{
		VideoPixel24bit		*Frame = (VideoPixel24bit *)Buffer.Vid24;
		LONG				 Offset;

		if( x > this->Width || y > this->Height )
		{
			return( FF_FAIL );
		}

		Offset = x + ( y * this->Width );

		Pixel->red   = Frame[ Offset ].red;
		Pixel->green = Frame[ Offset ].green;
		Pixel->blue  = Frame[ Offset ].blue;
		Pixel->alpha = 255;

		return( FF_SUCCESS );
	}

	HRESULT setPixel( VideoFrame Buffer, DWORD x, DWORD y, VideoPixel32bit *Pixel )
	{
		VideoPixel24bit		*Frame = (VideoPixel24bit *)Buffer.Vid24;
		LONG				 Offset;

		if( x > this->Width || y > this->Height )
		{
			return( FF_FAIL );
		}

		Offset = x + ( y * this->Width );

		Frame[ Offset ].red   = Pixel->red;
		Frame[ Offset ].green = Pixel->green;
		Frame[ Offset ].blue  = Pixel->blue;

		return( FF_SUCCESS );
	}

	inline DWORD getPixelSize() const
	{
		return( sizeof(VideoPixel24bit) );
	}

	DWORD getBitDepth( void ) const
	{
		return( 24 );
	}

	DWORD getFreeFrameBitDepth( void ) const
	{
		return( FF_CAP_24BITVIDEO );
	}

	inline BYTE *getOffset( VideoFrame Buffer, DWORD Offset )
	{
		return( (BYTE *)&Buffer.Vid24[ Offset ] );
	}
};

class ffFrameBuffer32 : public ffFrameBuffer
{
public:
	ffFrameBuffer32( DWORD Width, DWORD Height ) : ffFrameBuffer( Width, Height )
	{
	}

	HRESULT getPixel( VideoFrame Buffer, DWORD x, DWORD y, VideoPixel32bit *Pixel )
	{
		VideoPixel32bit		*Frame = (VideoPixel32bit *)Buffer.Vid32;
		LONG				 Offset;

		if( x > this->Width || y > this->Height )
		{
			return( FF_FAIL );
		}

		Offset = x + ( y * this->Width );

		memcpy( Pixel, &Frame[ Offset ], sizeof(VideoPixel32bit) );

		return( FF_SUCCESS );
	}

	HRESULT setPixel( VideoFrame Buffer, DWORD x, DWORD y, VideoPixel32bit *Pixel )
	{
		VideoPixel32bit		*Frame = (VideoPixel32bit *)Buffer.Vid32;
		LONG				 Offset;

		if( x > this->Width || y > this->Height )
		{
			return( FF_FAIL );
		}

		Offset = x + ( y * this->Width );

		memcpy( &Frame[ Offset ], Pixel, sizeof(VideoPixel32bit) );

		return( FF_SUCCESS );
	}

	DWORD getPixelSize( void ) const
	{
		return( sizeof(VideoPixel32bit) );
	}

	DWORD getBitDepth( void ) const
	{
		return( 32 );
	}

	DWORD getFreeFrameBitDepth( void ) const
	{
		return( FF_CAP_32BITVIDEO );
	}

	inline BYTE *getOffset( VideoFrame Buffer, DWORD Offset )
	{
		return( (BYTE *)&Buffer.Vid32[ Offset ] );
	}
};

ffFrameBuffer *ffFrameBufferFactory::getInstance( const VideoInfoStruct *VideoInfo )
{
	switch( VideoInfo->bitDepth )
	{
		case FF_CAP_8BITVIDEO:
			return( new ffFrameBuffer8( VideoInfo->frameWidth, VideoInfo->frameHeight ) );
			break;

		case FF_CAP_16BITVIDEO:
			return( new ffFrameBuffer16( VideoInfo->frameWidth, VideoInfo->frameHeight ) );
			break;

		case FF_CAP_24BITVIDEO:
			return( new ffFrameBuffer24( VideoInfo->frameWidth, VideoInfo->frameHeight ) );
			break;

		case FF_CAP_32BITVIDEO:
			return( new ffFrameBuffer32( VideoInfo->frameWidth, VideoInfo->frameHeight ) );
			break;
	}

	return( NULL );
}

void ffFrameBuffer::DrawLine( VideoFrame OutputFrame, int x1, int y1, int x2, int y2, VideoPixel32bit *Pixel, float Distance )
{
	int		xDiff = x2 - x1;
	int		yDiff = y2 - y1;

	this->DrawLine( OutputFrame, x1, y1, x1 + (int)( xDiff * Distance ), y1 + (int)( yDiff * Distance ), Pixel );
}

void ffFrameBuffer::DrawLine( VideoFrame OutputFrame, int x1, int y1, int x2, int y2, VideoPixel32bit *Pixel )
{
	bool	yLonger = false;
	int		shortLen = y2 - y1;
	int		longLen = x2 - x1;
	int		decInc;

	if( abs( shortLen ) > abs( longLen ) )
	{
		int swap = shortLen;
		shortLen = longLen;
		longLen = swap;				
		yLonger = true;
	}

	if( longLen == 0 )
	{
		decInc = 0;
	}
	else
	{
		decInc = ( shortLen << 16 ) / longLen;
	}

	if( yLonger )
	{
		if( longLen > 0 )
		{
			longLen += y1;

			for( int j = 0x8000 + ( x1 << 16 ) ; y1 <= longLen ; ++y1 )
			{
				setPixel( OutputFrame, j >> 16, y1, Pixel );

				j += decInc;
			}
		}
		else
		{
			longLen += y1;

			for( int j = 0x8000 + ( x1 << 16 ) ; y1 >= longLen ; --y1 )
			{
				setPixel( OutputFrame, j >> 16, y1, Pixel );

				j -= decInc;
			}
		}
	}
	else
	{
		if( longLen > 0 )
		{
			longLen += x1;

			for( int j = 0x8000 + ( y1 << 16 ) ; x1 <= longLen ; ++x1 )
			{
				setPixel( OutputFrame, x1, j >> 16, Pixel );

				j += decInc;
			}
		}
		else
		{
			longLen += x1;

			for( int j = 0x8000 + ( y1 << 16 ) ; x1 >= longLen ; --x1 )
			{
				setPixel( OutputFrame, x1, j >> 16, Pixel );

				j -= decInc;
			}
		}
	}
}

void ffFrameBuffer::DrawLine32bit( VideoPixel32bit *pOutput, int x1, int y1, const int x2, const int y2, const VideoPixel32bit &Pixel, const float Distance )
{
	int		xDiff = x2 - x1;
	int		yDiff = y2 - y1;

	this->DrawLine32bit( pOutput, x1, y1, x1 + (int)( xDiff * Distance ), y1 + (int)( yDiff * Distance ), Pixel );
}

void ffFrameBuffer::DrawLine32bit( VideoPixel32bit *pOutput, int x1, int y1, const int x2, const int y2, const VideoPixel32bit &Pixel )
{
	bool				 yLonger = false;
	int					 shortLen = y2 - y1;
	int					 longLen = x2 - x1;
	int					 decInc;

	if( abs( shortLen ) > abs( longLen ) )
	{
		int swap = shortLen;
		shortLen = longLen;
		longLen = swap;				
		yLonger = true;
	}

	if( longLen == 0 )
	{
		decInc = 0;
	}
	else
	{
		decInc = ( shortLen << 16 ) / longLen;
	}

	if( yLonger )
	{
		if( longLen > 0 )
		{
			longLen += y1;

			for( int j = 0x8000 + ( x1 << 16 ) ; y1 <= longLen ; ++y1 )
			{
				pOutput[ ( Width * y1 ) + ( j >> 16 ) ] = Pixel;

				j += decInc;
			}
		}
		else
		{
			longLen += y1;

			for( int j = 0x8000 + ( x1 << 16 ) ; y1 >= longLen ; --y1 )
			{
				pOutput[ ( Width * y1 ) + ( j >> 16 ) ] = Pixel;

				j -= decInc;
			}
		}
	}
	else
	{
		if( longLen > 0 )
		{
			longLen += x1;

			for( int j = 0x8000 + ( y1 << 16 ) ; x1 <= longLen ; ++x1 )
			{
				pOutput[ ( Width * ( j >> 16 ) ) + x1 ] = Pixel;

				j += decInc;
			}
		}
		else
		{
			longLen += x1;

			for( int j = 0x8000 + ( y1 << 16 ) ; x1 >= longLen ; --x1 )
			{
				pOutput[ ( Width * ( j >> 16 ) ) + x1 ] = Pixel;

				j -= decInc;
			}
		}
	}
}
/*
void ffFrameBuffer::convolve( VideoPixel32bit *pInput, VideoPixel32bit *pOutput, const float pMatrix[], const DWORD pWidth, const DWORD pHeight )
{
	VideoPixel32bit		Pixel;
	const DWORD			SizX = pWidth / 2;
	const DWORD			SizY = pHeight / 2;
	DWORD				OutX, OutY;
	DWORD				MatX, MatY;
	float				Red, Blu, Grn, Sum;

	for( OutY = 0 ; OutY < this->Height ; OutY++ )
	{
		for( OutX = 0 ; OutX < this->Width ; OutX++ )
		{
			Red = Grn = Blu = Sum = 0.0f;

			for( MatY = 0 ; MatY < pHeight ; MatY++ )
			{
				for( MatX = 0 ; MatX < pWidth ; MatX++ )
				{
					const int CurX = OutX - SizX + MatX;
					const int CurY = OutY - SizY + MatY;
					const int CurM = ( MatY * pWidth ) + MatX;

					if( CurX >= 0 && CurX < this->Width && CurY >= 0 && CurY < this->Height )
					{
						const	float	MatrixVal = pMatrix[ CurM ];

						Pixel = pInput[ ( CurY * this->Width ) + CurX ];

						Red += (float)Pixel.red   * MatrixVal;
						Grn += (float)Pixel.green * MatrixVal;
						Blu += (float)Pixel.blue  * MatrixVal;

						Sum += MatrixVal;
					}
				}
			}

			if( Sum != 0.0f )
			{
				Red /= Sum;
				Grn /= Sum;
				Blu /= Sum;
			}

			Red = abs( Red );
			Grn = abs( Grn );
			Blu = abs( Blu );

			Pixel.red   = (BYTE)( Red < 0.0f ? 0.0f : ( Red > 255.0f ? 255.0f : Red ) );
			Pixel.green = (BYTE)( Grn < 0.0f ? 0.0f : ( Grn > 255.0f ? 255.0f : Grn ) );
			Pixel.blue  = (BYTE)( Blu < 0.0f ? 0.0f : ( Blu > 255.0f ? 255.0f : Blu ) );

			pOutput[ ( OutY * this->Width ) + OutX ] = Pixel;
		}
	}
}
*/

void ffFrameBuffer::convolve( VideoPixel32bit *pInput, VideoPixel32bit *pOutput, const int pMatrix[], const DWORD pWidth, const DWORD pHeight )
{
	VideoPixel32bit		Pixel;
	const UINT			Width  = this->Width;
	const UINT			Height = this->Height;
	const UINT			SizX   = pWidth / 2;
	const UINT			SizY   = pHeight / 2;
	UINT				OutX, OutY;
	UINT				MatX, MatY;
	int					Red, Blu, Grn, Sum;

	for( OutY = 0 ; OutY < Height ; OutY++ )
	{
		for( OutX = 0 ; OutX < Width ; OutX++ )
		{
			Red = Grn = Blu = Sum = 0;

			for( MatY = 0 ; MatY < pHeight ; MatY++ )
			{
				for( MatX = 0 ; MatX < pWidth ; MatX++ )
				{
					const INT CurX = OutX - SizX + MatX;
					const INT CurY = OutY - SizY + MatY;
					const INT CurM = ( MatY * pWidth ) + MatX;

					if( CurX >= 0 && CurX < Width && CurY >= 0 && CurY < Height )
					{
						const	INT		MatrixVal = pMatrix[ CurM ];

						Pixel = pInput[ ( CurY * Width ) + CurX ];

						Red += (int)Pixel.red   * MatrixVal;
						Grn += (int)Pixel.green * MatrixVal;
						Blu += (int)Pixel.blue  * MatrixVal;

						Sum += MatrixVal;
					}
				}
			}

			if( Sum != 0.0f )
			{
				Red /= Sum;
				Grn /= Sum;
				Blu /= Sum;
			}

			Pixel.red   = (BYTE)( Red < 0 ? 0 : ( Red > 255 ? 255 : Red ) );
			Pixel.green = (BYTE)( Grn < 0 ? 0 : ( Grn > 255 ? 255 : Grn ) );
			Pixel.blue  = (BYTE)( Blu < 0 ? 0 : ( Blu > 255 ? 255 : Blu ) );

			pOutput[ ( OutY * Width ) + OutX ] = Pixel;
		}
	}
}

void ffFrameBuffer::convolve8bit( CHAR *pInput, const DWORD pImgWidth, const DWORD pImgHeight, CHAR *pOutput, const int pMatrix[], const DWORD pWidth, const DWORD pHeight )
{
	CHAR				Pixel;
	const DWORD			SizX = pWidth / 2;
	const DWORD			SizY = pHeight / 2;
	DWORD				OutX, OutY;
	DWORD				MatX, MatY;
	int					Val, Sum;

	for( OutY = 0 ; OutY < pImgHeight ; OutY++ )
	{
		for( OutX = 0 ; OutX < pImgWidth ; OutX++ )
		{
			Val = Sum = 0;

			for( MatY = 0 ; MatY < pHeight ; MatY++ )
			{
				for( MatX = 0 ; MatX < pWidth ; MatX++ )
				{
					const DWORD CurX = OutX - SizX + MatX;
					const DWORD CurY = OutY - SizY + MatY;
					const DWORD CurM = ( MatY * pWidth ) + MatX;

					if( CurX >= 0 && CurX < pImgWidth && CurY >= 0 && CurY < pImgHeight )
					{
						const	int		MatrixVal = pMatrix[ CurM ];

						Pixel = pInput[ ( CurY * pImgWidth ) + CurX ];

						Val += (int)Val * MatrixVal;

						Sum += MatrixVal;
					}
				}
			}

			if( Sum != 0.0f )
			{
				Val /= Sum;
			}

			pOutput[ ( OutY * pImgWidth ) + OutX ] = (CHAR)Val;
		}
	}
}

void ffFrameBuffer::convert16to24( VideoPixel16bit *pInput, VideoPixel24bit *pOutput )
{
	int					Remaining = this->Width * this->Height;
	VideoPixel16bit		TmpIn;
	VideoPixel24bit		TmpOut;
	
	while( Remaining-- > 0 )
	{
		TmpIn = *pInput++;
		
		TmpOut.red   = TmpIn.red   * 8;
		TmpOut.green = TmpIn.green * 4;
		TmpOut.blue  = TmpIn.blue  * 8;

		*pOutput++ = TmpOut;
	}
}

void ffFrameBuffer::convert16to32( VideoPixel16bit *pInput, VideoPixel32bit *pOutput )
{
	int					Remaining = this->Width * this->Height;
	VideoPixel16bit		TmpIn;
	VideoPixel32bit		TmpOut;
	
	TmpOut.alpha = 0xff;

	while( Remaining-- > 0 )
	{
		TmpIn = *pInput++;
		
		TmpOut.red   = TmpIn.red   * 8;
		TmpOut.green = TmpIn.green * 4;
		TmpOut.blue  = TmpIn.blue  * 8;

		*pOutput++ = TmpOut;
	}
}

void ffFrameBuffer::convert24to16( VideoPixel24bit *pInput, VideoPixel16bit *pOutput )
{
	int					Remaining = this->Width * this->Height;
	VideoPixel24bit		TmpIn;
	VideoPixel16bit		TmpOut;
	
	while( Remaining-- > 0 )
	{
		TmpIn = *pInput++;
		
		TmpOut.red   = TmpIn.red   / 8;
		TmpOut.green = TmpIn.green / 4;
		TmpOut.blue  = TmpIn.blue  / 8;

		*pOutput++ = TmpOut;
	}
}

void ffFrameBuffer::convert24to32( VideoPixel24bit *pInput, VideoPixel32bit *pOutput )
{
	int					Remaining = this->Width * this->Height;
	VideoPixel24bit		TmpIn;
	VideoPixel32bit		TmpOut;
	
	TmpOut.alpha = 0xff;

	while( Remaining-- > 0 )
	{
		TmpIn = *pInput++;
		
		TmpOut.red   = TmpIn.red;
		TmpOut.green = TmpIn.green;
		TmpOut.blue  = TmpIn.blue;

		*pOutput++ = TmpOut;
	}
}

void ffFrameBuffer::convert32to16( VideoPixel32bit *pInput, VideoPixel16bit *pOutput )
{
	int					Remaining = this->Width * this->Height;
	VideoPixel32bit		TmpIn;
	VideoPixel16bit		TmpOut;
	
	while( Remaining-- > 0 )
	{
		TmpIn = *pInput++;
		
		TmpOut.red   = TmpIn.red   / 8;
		TmpOut.green = TmpIn.green / 4;
		TmpOut.blue  = TmpIn.blue  / 8;

		*pOutput++ = TmpOut;
	}
}

void ffFrameBuffer::convert32to24( VideoPixel32bit *pInput, VideoPixel24bit *pOutput )
{
	int					Remaining = this->Width * this->Height;
	VideoPixel32bit		TmpIn;
	VideoPixel24bit		TmpOut;
	
	while( Remaining-- > 0 )
	{
		TmpIn = *pInput++;
		
		TmpOut.red   = TmpIn.red;
		TmpOut.green = TmpIn.green;
		TmpOut.blue  = TmpIn.blue;

		*pOutput++ = TmpOut;
	}
}

void ffFrameBuffer::toLuma( VideoPixel32bit *pSrcPtr )
{
	int					Remaining = this->Width * this->Height;

	while( Remaining-- > 0 )
	{
		const int		Min = min( pSrcPtr->red, min( pSrcPtr->green, pSrcPtr->blue ) );
		const int		Max = max( pSrcPtr->red, max( pSrcPtr->green, pSrcPtr->blue ) );
		const int		MaxPlusMin = Max + Min;

		pSrcPtr->red = pSrcPtr->green = pSrcPtr->blue = MaxPlusMin / 2;

		pSrcPtr++;
	}
}

void ffFrameBuffer::toLuma( VideoPixel32bit *pSrcPtr, VideoPixel32bit *pDstPtr )
{
	int					Remaining = this->Width * this->Height;

	while( Remaining-- > 0 )
	{
		const int		Min = min( pSrcPtr->red, min( pSrcPtr->green, pSrcPtr->blue ) );
		const int		Max = max( pSrcPtr->red, max( pSrcPtr->green, pSrcPtr->blue ) );
		const int		MaxPlusMin = Max + Min;

		pDstPtr->red = pDstPtr->green = pDstPtr->blue = MaxPlusMin / 2;

		pSrcPtr++;
		pDstPtr++;
	}
}

void ffFrameBuffer::toLuma( VideoPixel32bit *pSrcPtr, BYTE *pDstPtr )
{
	int					Remaining = this->Width * this->Height;

	while( Remaining-- > 0 )
	{
		const int		Min = min( pSrcPtr->red, min( pSrcPtr->green, pSrcPtr->blue ) );
		const int		Max = max( pSrcPtr->red, max( pSrcPtr->green, pSrcPtr->blue ) );
		const int		MaxPlusMin = Max + Min;

		*pDstPtr++ = MaxPlusMin / 2;

		pSrcPtr++;
	}
}

void ffFrameBuffer::toBrightness( VideoPixel32bit *pSrcPtr, BYTE *pDstPtr )
{
	int					Remaining = this->Width * this->Height;

	while( Remaining-- > 0 )
	{
		*pDstPtr++ = max( pSrcPtr->red, max( pSrcPtr->green, pSrcPtr->blue ) );

		pSrcPtr++;
	}
}

void ffFrameBuffer::clear( VideoPixel32bit *pFrame )
{
	memset( pFrame, 0, sizeof( VideoPixel32bit ) * this->Width * this->Height );
}

void ffFrameBuffer::RGB2HSL( VideoPixel32bit *pSource, VideoPixelHSL *pOutput )
{
	int					Remaining = this->Width * this->Height;

	while( Remaining-- > 0 )
	{
		const float		R = (float)pSource->red / 255.0f;
		const float		G = (float)pSource->green / 255.0f;
		const float		B = (float)pSource->blue / 255.0f;
		const float		Min = min( R, min( G, B ) );
		const float		Max = max( R, max( G, B ) );
		const float		MaxPlusMin = Max + Min;

		pOutput->mLum = 0.5f * MaxPlusMin;

		if( Min == Max )
		{
			pOutput->mHue = pOutput->mSat = 0.0f;
		}
		else
		{
			const float MaxMinusMin = Max - Min;

			if( Max == R )
			{
				pOutput->mHue = ( G - B ) / MaxMinusMin;
			}
			else if( Max == G )
			{
				pOutput->mHue = 2.0f + ( ( B - R ) / MaxMinusMin );
			}
			else
			{
				pOutput->mHue = 4.0f + ( ( R - G ) / MaxMinusMin );
			}

			pOutput->mHue *= 60.0f;

			if( pOutput->mHue < 0.0f )
			{
				pOutput->mHue += 360.0f;
			}
			else if( pOutput->mHue > 360.0f )
			{
				pOutput->mHue -= 360.0f;
			}

			if( pOutput->mLum <= 0.5f )
			{
				pOutput->mSat = MaxMinusMin / MaxPlusMin;
			}
			else
			{
				pOutput->mSat = MaxMinusMin / ( 2.0f - MaxMinusMin );
			}
		}

		pSource++;
		pOutput++;
	}
}

void ffFrameBuffer::HSL2RGB( VideoPixelHSL *pSource, VideoPixel32bit *pOutput )
{
	VideoPixel32bit		Colour;
	int					Remaining = this->Width * this->Height;
	float				temp1, temp2, temp3;
	float				R, G, B;

	Colour.alpha = 0xff;

	while( Remaining-- > 0 )
	{
		Colour.value = 0x00000000;

		const	float	S = pSource->mSat;
		const	float	L = pSource->mLum;

		if( S == 0.0f )
		{
			Colour.red   = (BYTE)( L * 255.0f );
			Colour.green = Colour.red;
			Colour.blue  = Colour.red;
		}
		else
		{
			const	float	H = pSource->mHue / 360.0f;

			if( L < 0.5f )
			{
				temp2 = L * ( 1.0f + S );
			}
			else
			{
				temp2 = ( L + S ) - ( L * S );
			}

			temp1 = 2.0f * L - temp2;

			// Red

			temp3 = H + 1.0f / 3.0f;

			if( temp3 < 0.0f ) temp3 += 1.0f;
			if( temp3 > 1.0f ) temp3 -= 1.0f;

			if( temp3 * 6.0f < 1.0f ) R = temp1 + ( temp2 - temp1 ) * 6.0f * temp3;
			else if( temp3 * 2.0f < 1.0f ) R = temp2;
			else if( temp3 * 3.0f < 2.0f ) R = temp1 + ( temp2 - temp1 ) * ( ( 2.0f / 3.0f ) - temp3 ) * 6.0f;
			else R = temp1;

			// Green

			temp3 = H;

			if( temp3 < 0.0f ) temp3 += 1.0f;
			if( temp3 > 1.0f ) temp3 -= 1.0f;

			if( temp3 * 6.0f < 1.0f ) G = temp1 + ( temp2 - temp1 ) * 6.0f * temp3;
			else if( temp3 * 2.0f < 1.0f ) G = temp2;
			else if( temp3 * 3.0f < 2.0f ) G = temp1 + ( temp2 - temp1 ) * ( ( 2.0f / 3.0f ) - temp3 ) * 6.0f;
			else G = temp1;

			// Blue

			temp3 = H - 1.0f / 3.0f;

			if( temp3 < 0.0f ) temp3 += 1.0f;
			if( temp3 > 1.0f ) temp3 -= 1.0f;

			if( temp3 * 6.0f < 1.0f ) B = temp1 + ( temp2 - temp1 ) * 6.0f * temp3;
			else if( temp3 * 2.0f < 1.0f ) B = temp2;
			else if( temp3 * 3.0f < 2.0f ) B = temp1 + ( temp2 - temp1 ) * ( ( 2.0f / 3.0f ) - temp3 ) * 6.0f;
			else B = temp1;

			Colour.red   = (BYTE)( R * 255.0f );
			Colour.green = (BYTE)( G * 255.0f );
			Colour.blue  = (BYTE)( B * 255.0f );
		}

		pSource++;

		*pOutput++ = Colour;
	}
}

void ffFrameBuffer::toHue( VideoPixel32bit *pSource, VideoPixel8bit *pOutput )
{
	int					Remaining = this->Width * this->Height;

	while( Remaining-- > 0 )
	{
		const float		R = (float)pSource->red / 255.0f;
		const float		G = (float)pSource->green / 255.0f;
		const float		B = (float)pSource->blue / 255.0f;
		const float		Min = min( R, min( G, B ) );
		const float		Max = max( R, max( G, B ) );
		const float		MaxPlusMin = Max + Min;
		float			Hue;

		if( Min == Max )
		{
			*pOutput = max( pSource->red, max( pSource->green, pSource->blue ) );
		}
		else
		{
			const float MaxMinusMin = Max - Min;

			if( Max == R )
			{
				Hue = ( G - B ) / MaxMinusMin;
			}
			else if( Max == G )
			{
				Hue = 2.0f + ( ( B - R ) / MaxMinusMin );
			}
			else
			{
				Hue = 4.0f + ( ( R - G ) / MaxMinusMin );
			}

			Hue *= 60.0f;

			if( Hue < 0.0f )
			{
				Hue += 360.0f;
			}
			else if( Hue > 360.0f )
			{
				Hue -= 360.0f;
			}

			*pOutput = (VideoPixel8bit)( ( Hue / 360.0f ) * 255.0f );
		}

		pSource++;
		pOutput++;
	}
}

void ffFrameBuffer::copyFrame( VideoFrame pSrc, VideoFrame pDst )
{
	memcpy( pDst.Frame, pSrc.Frame, this->getFrameByteSize() );
}

void ffFrameBuffer::resizeNearestNeighbour( VideoPixel32bit *pSrcPtr, VideoPixel32bit *pDstPtr, const int pNewWidth, const int pNewHeight )
{
	const int			 Width    = this->Width;
	const int			 Height   = this->Height;
	const float			 TransX   = (float)Width  / (float)pNewWidth;
	const float			 TransY   = (float)Height / (float)pNewHeight;

	for( int y = 0 ; y < pNewHeight ; y++ )
	{
		for( int x = 0 ; x < pNewWidth ; x++ )
		{
			const	int		NewX = (int)( (float)x * TransX );
			const	int		NewY = (int)( (float)y * TransY );

			*pDstPtr++ = pSrcPtr[ ( NewY * Width ) + NewX ];
		}
	}
}

inline float minmax( const float pMin, const float pVal, const float pMax )
{
	if( pVal < pMin ) return( pMin );
	if( pVal > pMax ) return( pMax );

	return( pVal );
}

void ffFrameBuffer::resizeBilinear( VideoPixel32bit *pSrcPtr, VideoPixel32bit *pDstPtr, const int pNewWidth, const int pNewHeight )
{
	const int			 Width    = this->Width;
	const int			 Height   = this->Height;
	const float			 TransX   = (float)Width  / (float)pNewWidth;
	const float			 TransY   = (float)Height / (float)pNewHeight;

	for( int y = 0 ; y < pNewHeight ; y++ )
	{
		const	float	CY = (float)y * TransY;
		const	float	Y1 = minmax( 0.0f, ( (float)y - 0.5f ) * TransY, (float)Height - 1.0f );
		const	float	Y2 = minmax( 0.0f, ( (float)y + 0.5f ) * TransY, (float)Height - 1.0f );

		for( int x = 0 ; x < pNewWidth ; x++ )
		{
			const	float	CX = (float)x * TransX;
			const	float	X1 = minmax( 0.0f, ( (float)x - 0.5f ) * TransX, (float)Width - 1.0f );
			const	float	X2 = minmax( 0.0f, ( (float)x + 0.5f ) * TransX, (float)Width - 1.0f );
			const	float	ConstDiv = ( X2 - X1 ) * ( Y2 - Y1 );
				
			const	VideoPixel32bit		Q11 = pSrcPtr[ ( (int)Y1 * Width ) + (int)X1 ];
			const	VideoPixel32bit		Q12 = pSrcPtr[ ( (int)Y2 * Width ) + (int)X1 ];
			const	VideoPixel32bit		Q21 = pSrcPtr[ ( (int)Y1 * Width ) + (int)X2 ];
			const	VideoPixel32bit		Q22 = pSrcPtr[ ( (int)Y2 * Width ) + (int)X2 ];

			const	float	CXmX1 = CX - X1;
			const	float	X2mCX = X2 - CX;
			const	float	Y2mCY = Y2 - CY;
			const	float	CYmY1 = CY - Y1;

			const	float	X2mCXxY2mCY = X2mCX * Y2mCY;
			const	float	CXmX1xY2mCY = CXmX1 * Y2mCY;
			const	float	X2mCXxCYmY1 = X2mCX * CYmY1;
			const	float	CXmX1xCYmY1 = CXmX1 * CYmY1;

			pDstPtr->red = (BYTE)(
				( ( (float)Q11.red / ConstDiv ) * X2mCXxY2mCY ) +
				( ( (float)Q21.red / ConstDiv ) * CXmX1xY2mCY ) +
				( ( (float)Q12.red / ConstDiv ) * X2mCXxCYmY1 ) +
				( ( (float)Q21.red / ConstDiv ) * CXmX1xCYmY1 )
				);
			pDstPtr->green = (BYTE)(
				( ( (float)Q11.green / ConstDiv ) * X2mCXxY2mCY ) +
				( ( (float)Q21.green / ConstDiv ) * CXmX1xY2mCY ) +
				( ( (float)Q12.green / ConstDiv ) * X2mCXxCYmY1 ) +
				( ( (float)Q21.green / ConstDiv ) * CXmX1xCYmY1 )
				);
			pDstPtr->blue  = (BYTE)(
				( ( (float)Q11.blue / ConstDiv ) * X2mCXxY2mCY ) +
				( ( (float)Q21.blue / ConstDiv ) * CXmX1xY2mCY ) +
				( ( (float)Q12.blue / ConstDiv ) * X2mCXxCYmY1 ) +
				( ( (float)Q21.blue / ConstDiv ) * CXmX1xCYmY1 )
				);
			pDstPtr->alpha  = (BYTE)(
				( ( (float)Q11.alpha / ConstDiv ) * X2mCXxY2mCY ) +
				( ( (float)Q21.alpha / ConstDiv ) * CXmX1xY2mCY ) +
				( ( (float)Q12.alpha / ConstDiv ) * X2mCXxCYmY1 ) +
				( ( (float)Q21.alpha / ConstDiv ) * CXmX1xCYmY1 )
				);

			pDstPtr++;
		}
	}
}

int ffFrameBuffer::getFreeFrameDepthFromBits( const int pDepth )
{
	switch( pDepth )
	{
		case 16:	return( FF_CAP_16BITVIDEO );
		case 24:	return( FF_CAP_24BITVIDEO );
		case 32:	return( FF_CAP_32BITVIDEO );
	}

	return( -1 );
}

/*
// For resizing into a frame of the same size

		const int			 Width    = this->videoInfo.frameWidth;
		const int			 Height   = this->videoInfo.frameHeight;
		const float			 TransX   = (float)Width  / 50.0f;
		const float			 TransY   = (float)Height / 50.0f;
		VideoPixel32bit		*TmpFrame = new VideoPixel32bit[ Height * Width ];
		VideoPixel32bit		 BlkPixel = { 0 };
		//VideoPixel32bit		*SrcPtr;
		VideoPixel32bit		*DstPtr = TmpFrame;

		memset( &BlkPixel, 0, sizeof(BlkPixel) );

		for( int y = 0 ; y < Height ; y++ )
		{
			for( int x = 0 ; x < Width ; x++ )
			{
				// Nearest Neighbour

				const	int		NewX = (int)( (float)x * TransX );
				const	int		NewY = (int)( (float)y * TransY );

				if( NewX >= Width || NewY >= Height )
				{
					SrcPtr = &BlkPixel;
				}
				else
				{
					SrcPtr = &pFrame.Vid32[ ( NewY * Width ) + NewX ];
				}

				*DstPtr++ = *SrcPtr;

				// Bilinear

				const	float	CX = (float)x * TransX;
				const	float	CY = (float)y * TransY;
				const	float	X1 = ( (float)x - 0.5f ) * TransX;
				const	float	Y1 = ( (float)y - 0.5f ) * TransY;
				const	float	X2 = ( (float)x + 0.5f ) * TransX;
				const	float	Y2 = ( (float)y + 0.5f ) * TransY;
				const	float	ConstDiv = ( X2 - X1 ) * ( Y2 - Y1 );

				if( X1 >= Width || Y1 >= Height )
				{
					*DstPtr++ = BlkPixel;
				}
				else
				{
					const	VideoPixel32bit		Q11 = pFrame.Vid32[ ( (int)Y1 * Width ) + (int)X1 ];
					const	VideoPixel32bit		Q12 = ( Y2 >= Height ? pFrame.Vid32[ ( (int)Y1 * Width ) + (int)X1 ] : pFrame.Vid32[ ( (int)Y2 * Width ) + (int)X1 ] );
					const	VideoPixel32bit		Q21 = ( X2 >= Width  ? pFrame.Vid32[ ( (int)Y1 * Width ) + (int)X1 ] : pFrame.Vid32[ ( (int)Y1 * Width ) + (int)X2 ] );
					const	VideoPixel32bit		Q22 = ( X2 >= Width  ? ( Y2 >= Height ? pFrame.Vid32[ ( (int)Y1 * Width ) + (int)X1 ] : pFrame.Vid32[ ( (int)Y2 * Width ) + (int)X1 ] ) : ( Y2 >= Height ? pFrame.Vid32[ ( (int)Y1 * Width ) + (int)X2 ] : pFrame.Vid32[ ( (int)Y2 * Width ) + (int)X2 ] ) );

					DstPtr->red   = (BYTE)(
						( ( (float)Q11.red / ConstDiv ) * ( X2 - CX ) * ( Y2 - CY ) ) +
						( ( (float)Q21.red / ConstDiv ) * ( CX - X1 ) * ( Y2 - CY ) ) +
						( ( (float)Q12.red / ConstDiv ) * ( X2 - CX ) * ( CY - Y1 ) ) +
						( ( (float)Q21.red / ConstDiv ) * ( CX - X1 ) * ( CY - Y1 ) )
						);
					DstPtr->green = (BYTE)(
						( ( (float)Q11.green / ConstDiv ) * ( X2 - CX ) * ( Y2 - CY ) ) +
						( ( (float)Q21.green / ConstDiv ) * ( CX - X1 ) * ( Y2 - CY ) ) +
						( ( (float)Q12.green / ConstDiv ) * ( X2 - CX ) * ( CY - Y1 ) ) +
						( ( (float)Q21.green / ConstDiv ) * ( CX - X1 ) * ( CY - Y1 ) )
						);
					DstPtr->blue  = (BYTE)(
						( ( (float)Q11.blue / ConstDiv ) * ( X2 - CX ) * ( Y2 - CY ) ) +
						( ( (float)Q21.blue / ConstDiv ) * ( CX - X1 ) * ( Y2 - CY ) ) +
						( ( (float)Q12.blue / ConstDiv ) * ( X2 - CX ) * ( CY - Y1 ) ) +
						( ( (float)Q21.blue / ConstDiv ) * ( CX - X1 ) * ( CY - Y1 ) )
						);
					DstPtr->alpha  = (BYTE)(
						( ( (float)Q11.alpha / ConstDiv ) * ( X2 - CX ) * ( Y2 - CY ) ) +
						( ( (float)Q21.alpha / ConstDiv ) * ( CX - X1 ) * ( Y2 - CY ) ) +
						( ( (float)Q12.alpha / ConstDiv ) * ( X2 - CX ) * ( CY - Y1 ) ) +
						( ( (float)Q21.alpha / ConstDiv ) * ( CX - X1 ) * ( CY - Y1 ) )
						);

					DstPtr++;
				}
			}
		}

		memcpy( pFrame.Vid32, TmpFrame, ( Height * Width * sizeof( VideoPixel32bit ) ) );

		delete TmpFrame;
*/


void ffFrameBuffer::flipV( VideoFrame pFrame )
{
	const int			 FRAME_SIZE = getFrameByteSize();
	const int			 LINE_WIDTH = getLineByteSize();
	BYTE				*Buff = new BYTE[ FRAME_SIZE ];

	memcpy( Buff, pFrame.Frame, FRAME_SIZE );

	for( DWORD y = 0 ; y < Height ; y++ )
	{
		memcpy( &((BYTE *)pFrame.Frame)[ y * LINE_WIDTH ], &Buff[ ( Height - 1 - y ) * LINE_WIDTH ], LINE_WIDTH );
	}

	delete[] Buff;
}

void ffFrameBuffer::flipH( VideoFrame pFrame )
{
	const int			 FRAME_SIZE = getFrameByteSize();
	const int			 LINE_WIDTH = getLineByteSize();
	BYTE				*Buff = new BYTE[ FRAME_SIZE ];

	memcpy( Buff, pFrame.Frame, FRAME_SIZE );

	for( DWORD y = 0 ; y < Height ; y++ )
	{
		for( DWORD x = 0 ; x < Width ; x++ )
		{

		}
	}

	delete[] Buff;
}

void ffFrameBuffer::flipV( VideoFrame pSrc, VideoFrame pDst )
{
	const int			 LINE_WIDTH = getLineByteSize();

	for( DWORD y = 0 ; y < Height ; y++ )
	{
		memcpy( &((BYTE *)pDst.Frame)[ y * LINE_WIDTH ], &((BYTE *)pSrc.Frame)[ ( Height - 1 - y ) * LINE_WIDTH ], LINE_WIDTH );
	}
}

void ffFrameBuffer::flipH( VideoFrame pSrc, VideoFrame pDst )
{
	const int			 LINE_WIDTH = getLineByteSize();
	const int			 PIXEL_SIZE = getPixelSize();
	BYTE				*SrcPtr, *DstPtr;

	for( DWORD y = 0 ; y < Height ; y++ )
	{
		SrcPtr = &((BYTE *)pSrc.Frame)[ y * LINE_WIDTH ];
		DstPtr = &((BYTE *)pDst.Frame)[ ( y * LINE_WIDTH ) + ( LINE_WIDTH - PIXEL_SIZE ) ];

		for( DWORD x = 0 ; x < Width ; x++, SrcPtr += PIXEL_SIZE, DstPtr -= PIXEL_SIZE )
		{
			memcpy( DstPtr, SrcPtr, PIXEL_SIZE );
		}
	}
}
