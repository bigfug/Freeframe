
#include "ImageConvert.h"
#include <cassert>
#include <xmmintrin.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>

extern "C"
{
//#include <libswscale/swscale.h>
}

#if defined( USE_MMX )

bool CheckForMMX()
{
	uint32_t	 bMMX = 0;
	uint32_t	*pbMMX = &bMMX;

	try
	{
		__asm
		{
			mov eax, 1
			cpuid
			mov edi, pbMMX
			mov [edi], edx
		}
	}
	catch( ... )
	{
		bMMX = 0;
	}

	return ((bMMX & 0x00800000) != 0);  // check bit 23
}

#endif

using namespace std;

void Resize_HQ_4ch( unsigned char* src, int w1, int h1,
					unsigned char* dest, int w2, int h2,
					volatile bool* pQuitFlag )
{
	// Both buffers must be in ARGB format, and a scanline should be w*4 bytes.

	// If pQuitFlag is non-NULL, then at the end of each scanline, it will check
	//    the value at *pQuitFlag; if it's set to 'true', this function will abort.
	// (This is handy if you're background-loading an image, and decide to cancel.)

	// NOTE: THIS WILL OVERFLOW for really major downsizing (2800x2800 to 1x1 or more)
	// (2800 ~ sqrt(2^23)) - for a lazy fix, just call this in two passes.

	vector<int>		g_px1a;
	vector<int>		g_px1ab;

	assert(src);
	assert(dest);
	assert(w1 >= 1);
	assert(h1 >= 1);
	assert(w2 >= 1);
	assert(h2 >= 1);

#if defined( USE_MMX )
	static bool g_bMMX = false;
	static bool g_bMMX_known = false;

	// check for MMX (one time only)
	if (!g_bMMX_known)
	{
		g_bMMX = CheckForMMX();
		g_bMMX_known = true;
	}
#endif

	if (w2*2==w1 && h2*2==h1)
	{
		// perfect 2x2:1 case - faster code
		// (especially important because this is common for generating low (large) mip levels!)
		uint32_t *dsrc  = (uint32_t*)src;
		uint32_t *ddest = (uint32_t*)dest;

#if defined( USE_MMX )
		if (g_bMMX==1)
		{
			// MMX LOOP - about 32% faster
			// PREFETCH: no (...tests determined it was actually SLOWER)
			// MASS PRESERVING (~remainders): *NO*

			__m64 zero;
			zero.m64_i32[0] = 0;
			zero.m64_i32[1] = 0;

			int i = 0;
			for (int y2=0; y2<h2; y2++)
			{
				int y1 = y2*2;
				uint32_t* temp_src = &dsrc[y1*w1];
				for (int x2=0; x2<w2; x2++)
				{
					__m64 a = *(__m64*)(&temp_src[ 0]);  // ABCDEFGH
					__m64 b = *(__m64*)(&temp_src[w1]);  // IJKLMNOP
					register __m64 c = _mm_unpacklo_pi8(a, zero); // 0E0F0G0H      // PUNPCKLBW
					register __m64 d = _mm_unpacklo_pi8(b, zero); // 0M0N0O0P      // PUNPCKLBW
					c = _mm_add_pi16(c, d);
					d = _mm_unpackhi_pi8(a, zero);
					c = _mm_add_pi16(c, d);
					d = _mm_unpackhi_pi8(b, zero);
					c = _mm_add_pi16(c, d);
					c = _mm_srli_pi16(c, 2);
					c = _mm_packs_pu16(c, c);

					ddest[i++] = c.m64_u32[0];
					temp_src += 2;
				}

				if (pQuitFlag && *pQuitFlag)
					break;
			}
			_mm_empty();    // do this always - just that __m64's existence, above, will tamper w/float stuff.
		}
		else
#endif
		{
			// NON-MMX LOOP
			// PREFETCH: no (...tests determined it was actually SLOWER)
			// MASS PRESERVING (~remainders): YES
			uint32_t remainder = 0;
			int i = 0;
			for (int y2=0; y2<h2; y2++)
			{
				int y1 = y2*2;

				uint32_t* temp_src = &dsrc[y1*w1];

				for (int x2=0; x2<w2; x2++)
				{
					uint32_t xUL = temp_src[0];
					uint32_t xUR = temp_src[1];
					uint32_t xLL = temp_src[w1];
					uint32_t xLR = temp_src[w1 + 1];
					// note: uint32_t packing is 0xAARRGGBB

					uint32_t redblue = (xUL & 0x00FF00FF) + (xUR & 0x00FF00FF) + (xLL & 0x00FF00FF) + (xLR & 0x00FF00FF) + (remainder & 0x00FF00FF);
					uint32_t green   = (xUL & 0x0000FF00) + (xUR & 0x0000FF00) + (xLL & 0x0000FF00) + (xLR & 0x0000FF00) + (remainder & 0x0000FF00);
					// redblue = 000000rr rrrrrrrr 000000bb bbbbbbbb
					// green   = xxxxxx00 000000gg gggggggg 00000000
					remainder =  (redblue & 0x00030003) | (green & 0x00000300);
					ddest[i++]   = ((redblue & 0x03FC03FC) | (green & 0x0003FC00)) >> 2;

					temp_src += 2;
				}

				if (pQuitFlag && *pQuitFlag)
					break;
			}
		}
	}
	else
	{
		// arbitrary resize.
		unsigned int *dsrc  = (unsigned int *)src;
		unsigned int *ddest = (unsigned int *)dest;

		bool bUpsampleX = (w1 < w2);
		bool bUpsampleY = (h1 < h2);

		// If too many input pixels map to one output pixel, our 32-bit accumulation values
		// could overflow - so, if we have huge mappings like that, cut down the weights:
		//    256 max color value
		//   *256 weight_x
		//   *256 weight_y
		//   *256 (16*16) maximum # of input pixels (x,y) - unless we cut the weights down...
		int weight_shift = 0;
		float source_texels_per_out_pixel = (   (w1/(float)w2 + 1)
											  * (h1/(float)h2 + 1)
											);
		float weight_per_pixel = source_texels_per_out_pixel * 256 * 256;  //weight_x * weight_y
		float accum_per_pixel = weight_per_pixel*256; //color value is 0-255
		float weight_div = accum_per_pixel / 4294967000.0f;
		if (weight_div > 1)
			weight_shift = (int)ceilf( logf((float)weight_div)/logf(2.0f) );
		weight_shift = min(15, weight_shift);  // this could go to 15 and still be ok.

		float fh = 256*h1/(float)h2;
		float fw = 256*w1/(float)w2;

		if (bUpsampleX && bUpsampleY)
		{
			// faster to just do 2x2 bilinear interp here

			// cache x1a, x1b for all the columns:
			// ...and your OS better have garbage collection on process exit :)
			g_px1a.resize( w2 * 2 * 1 );

			for (int x2=0; x2<w2; x2++)
			{
				// find the x-range of input pixels that will contribute:
				int x1a = (int)(x2*fw);
				x1a = min(x1a, 256*(w1-1) - 1);
				g_px1a[x2] = x1a;
			}

			// FOR EVERY OUTPUT PIXEL
			for (int y2=0; y2<h2; y2++)
			{
				// find the y-range of input pixels that will contribute:
				int y1a = (int)(y2*fh);
				y1a = min(y1a, 256*(h1-1) - 1);
				int y1c = y1a >> 8;

				unsigned int *ddest = &((unsigned int *)dest)[y2*w2 + 0];

				for (int x2=0; x2<w2; x2++)
				{
					// find the x-range of input pixels that will contribute:
					int x1a = g_px1a[x2];//(int)(x2*fw);
					int x1c = x1a >> 8;

					unsigned int *dsrc2 = &dsrc[y1c*w1 + x1c];

					// PERFORM BILINEAR INTERPOLATION on 2x2 pixels
					unsigned int r=0, g=0, b=0; //, a=0;
					unsigned int weight_x = 256 - (x1a & 0xFF);
					unsigned int weight_y = 256 - (y1a & 0xFF);
					for (int y=0; y<2; y++)
					{
						for (int x=0; x<2; x++)
						{
							unsigned int c = dsrc2[x + y*w1];
							unsigned int r_src = (c    ) & 0xFF;
							unsigned int g_src = (c>> 8) & 0xFF;
							unsigned int b_src = (c>>16) & 0xFF;
							unsigned int w = (weight_x * weight_y) >> weight_shift;
							r += r_src * w;
							g += g_src * w;
							b += b_src * w;
							weight_x = 256 - weight_x;
						}
						weight_y = 256 - weight_y;
					}

					unsigned int c = ((r>>16)) | ((g>>8) & 0xFF00) | (b & 0xFF0000) | 0xFF000000;
					*ddest++ = c;//ddest[y2*w2 + x2] = c;
				}
			}
		}
		else
		{
			// cache x1a, x1b for all the columns:
			// ...and your OS better have garbage collection on process exit :)

			g_px1ab.resize( w2 * 2 * 2 );

			for (int x2=0; x2<w2; x2++)
			{
				// find the x-range of input pixels that will contribute:
				int x1a = (int)((x2  )*fw);
				int x1b = (int)((x2+1)*fw);
				if (bUpsampleX) // map to same pixel -> we want to interpolate between two pixels!
					x1b = x1a + 256;
				x1b = min(x1b, 256*w1 - 1);
				g_px1ab[x2*2+0] = x1a;
				g_px1ab[x2*2+1] = x1b;
			}

			// FOR EVERY OUTPUT PIXEL
			for (int y2=0; y2<h2; y2++)
			{
				// find the y-range of input pixels that will contribute:
				int y1a = (int)((y2  )*fh);
				int y1b = (int)((y2+1)*fh);
				if (bUpsampleY) // map to same pixel -> we want to interpolate between two pixels!
					y1b = y1a + 256;
				y1b = min(y1b, 256*h1 - 1);
				int y1c = y1a >> 8;
				int y1d = y1b >> 8;

				for (int x2=0; x2<w2; x2++)
				{
					// find the x-range of input pixels that will contribute:
					int x1a = g_px1ab[x2*2+0];    // (computed earlier)
					int x1b = g_px1ab[x2*2+1];    // (computed earlier)
					int x1c = x1a >> 8;
					int x1d = x1b >> 8;

					// ADD UP ALL INPUT PIXELS CONTRIBUTING TO THIS OUTPUT PIXEL:
					unsigned int r=0, g=0, b=0, a=0;
					for (int y=y1c; y<=y1d; y++)
					{
						unsigned int weight_y = 256;
						if (y1c != y1d)
						{
							if (y==y1c)
								weight_y = 256 - (y1a & 0xFF);
							else if (y==y1d)
								weight_y = (y1b & 0xFF);
						}

						unsigned int *dsrc2 = &dsrc[y*w1 + x1c];
						for (int x=x1c; x<=x1d; x++)
						{
							unsigned int weight_x = 256;
							if (x1c != x1d)
							{
								if (x==x1c)
									weight_x = 256 - (x1a & 0xFF);
								else if (x==x1d)
									weight_x = (x1b & 0xFF);
							}

							unsigned int c = *dsrc2++;//dsrc[y*w1 + x];
							unsigned int r_src = (c    ) & 0xFF;
							unsigned int g_src = (c>> 8) & 0xFF;
							unsigned int b_src = (c>>16) & 0xFF;
							unsigned int w = (weight_x * weight_y) >> weight_shift;
							r += r_src * w;
							g += g_src * w;
							b += b_src * w;
							a += w;
						}
					}

					// write results
					unsigned int c = ((r/a)) | ((g/a)<<8) | ((b/a)<<16) | 0xFF000000;
					*ddest++ = c;//ddest[y2*w2 + x2] = c;
				}

				if (pQuitFlag && *pQuitFlag)
					break;
			}
		}
	}
}

ImageConvert::ImageConvert( const fugVideoInfo &pInput, const fugVideoInfo &pOutput )
: mInput( pInput ), mOutput( pOutput )
{
	switch( mInput.bitDepth )
	{
		case FUG_VID_16BITVIDEO:
			mInputStride = mInput.frameWidth * 2;
			break;

		case FUG_VID_24BITVIDEO:
			mInputStride = mInput.frameWidth * 3;
			break;

		case FUG_VID_32BITVIDEO:
			mInputStride = mInput.frameWidth * 4;
			break;
	}

	switch( mOutput.bitDepth )
	{
		case FUG_VID_16BITVIDEO:
			mOutputStride = mOutput.frameWidth * 2;
			break;

		case FUG_VID_24BITVIDEO:
			mOutputStride = mOutput.frameWidth * 3;
			break;

		case FUG_VID_32BITVIDEO:
			mOutputStride = mOutput.frameWidth * 4;
			break;
	}
}

ImageConvert::~ImageConvert( void )
{
}

void ImageConvert::convert( const void *pInput, void *pOutput )
{
	uint8_t		*Input = (uint8_t *)pInput;

	if( mInput.bitDepth == FUG_VID_16BITVIDEO )
	{
		mConvertionInput.resize( mInput.frameWidth * mInput.frameHeight );

		uint8_t		*Output = (uint8_t *)&mConvertionInput[ 0 ];

		for( int y = 0 ; y < mInput.frameHeight ; y++ )
		{
			for( int x = 0 ; x < mInput.frameWidth ; x++, Input += 2, Output += 4 )
			{
				uint8_t		v1 = Input[ 1 ];
				uint8_t		v2 = Input[ 0 ];

				uint16_t	rgb = ( uint16_t( v1 ) << 8 ) | uint16_t( v2 );

				uint8_t		r = uint8_t( (rgb & 0xf800) >> 11 );
				uint8_t		g = uint8_t( (rgb & 0x07e0) >>  5 );
				uint8_t		b = uint8_t( (rgb & 0x001f) >>  0 );

				Output[ 0 ] = uint8_t( ( uint32_t( r ) * 527 + 23 ) >> 6 );
				Output[ 1 ] = uint8_t( ( uint32_t( g ) * 259 + 33 ) >> 6 );
				Output[ 2 ] = uint8_t( ( uint32_t( b ) * 527 + 23 ) >> 6 );
				Output[ 3 ] = 0xff;
			}
		}

		Input = (uint8_t *)&mConvertionInput[ 0 ];
	}
	else if( mInput.bitDepth == FUG_VID_24BITVIDEO )
	{
		mConvertionInput.resize( mInput.frameWidth * mInput.frameHeight );

		uint8_t		*Output = (uint8_t *)&mConvertionInput[ 0 ];

		for( int y = 0 ; y < mInput.frameHeight ; y++ )
		{
			for( int x = 0 ; x < mInput.frameWidth ; x++, Input += 3, Output += 4 )
			{
				Output[ 0 ] = Input[ 0 ];
				Output[ 1 ] = Input[ 1 ];
				Output[ 2 ] = Input[ 2 ];
				Output[ 3 ] = 0xff;
			}
		}

		Input = (uint8_t *)&mConvertionInput[ 0 ];
	}

	uint8_t		*Output = (uint8_t *)pOutput;

	if( mOutput.bitDepth == FUG_VID_16BITVIDEO )
	{
		mConvertionOutput.resize( mOutput.frameWidth * mOutput.frameHeight );

		Output = (uint8_t *)&mConvertionOutput[ 0 ];
	}
	else if( mOutput.bitDepth == FUG_VID_24BITVIDEO )
	{
		mConvertionOutput.resize( mOutput.frameWidth * mOutput.frameHeight );

		Output = (uint8_t *)&mConvertionOutput[ 0 ];
	}

	Resize_HQ_4ch( Input, mInput.frameWidth, mInput.frameHeight, Output, mOutput.frameWidth, mOutput.frameHeight, 0 );

	Input  = Output;

	Output = (uint8_t *)pOutput;

	if( mOutput.bitDepth == FUG_VID_16BITVIDEO )
	{
		for( int y = 0 ; y < mOutput.frameHeight ; y++ )
		{
			for( int x = 0 ; x < mOutput.frameWidth ; x++, Input += 4, Output += 2 )
			{
				uint16_t	RGB565 = (((Input[ 0 ]) >> 3) << 11)|(((Input[ 1 ]) >> 2) <<  5)|(((Input[ 2 ]) >> 3));

				Output[ 0 ] = (RGB565>>0)&0xff;
				Output[ 1 ] = (RGB565>>8)&0xff;
			}
		}
	}
	else if( mOutput.bitDepth == FUG_VID_24BITVIDEO )
	{
		for( int y = 0 ; y < mOutput.frameHeight ; y++ )
		{
			for( int x = 0 ; x < mOutput.frameWidth ; x++, Input += 4, Output += 3 )
			{
				Output[ 0 ] = Input[ 0 ];
				Output[ 1 ] = Input[ 1 ];
				Output[ 2 ] = Input[ 2 ];
			}
		}
	}
}

size_t ImageConvert::getOutputByteSize( void ) const
{
	const size_t		PixelCount = mOutput.frameWidth * mOutput.frameHeight;

	switch( mOutput.bitDepth )
	{
		case FUG_VID_16BITVIDEO:
			return( PixelCount * 2 );
			break;

		case FUG_VID_24BITVIDEO:
			return( PixelCount * 3 );
			break;

		case FUG_VID_32BITVIDEO:
			return( PixelCount * 4 );
			break;
	}

	return( PixelCount );
}
