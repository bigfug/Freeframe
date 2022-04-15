
#include <GL/glew.h>

#ifdef WIN32
#include <crtdbg.h>
#endif

#include <inttypes.h>

#include <algorithm>
#include <iostream>

using namespace std;

#include "TexToBuf.h"

#define		BUF_PADDING		(512)

TexToBuf::TexToBuf( void )
: mTexPBO( 0 ), mBufPBO( 0 )
{
	memset( &mTex, 0, sizeof( FFGLTextureStruct ) );
}

TexToBuf::~TexToBuf( void )
{
	freeGL();
}

void TexToBuf::freeGL( void )
{
	if( mTex.Handle != 0 )
	{
		glDeleteTextures( 1, &mTex.Handle );

		mTex.Handle = 0;
	}

	if( mTexPBO != 0 )
	{
		glDeleteBuffers( 1, &mTexPBO );

		mTexPBO = 0;
	}

	if( mBufPBO != 0 )
	{
		glDeleteBuffers( 1, &mBufPBO );

		mBufPBO = 0;
	}
}

void TexToBuf::resizeBuf( const size_t pSize )
{
	const size_t	PaddedSize = BUF_PADDING + pSize + BUF_PADDING;

	if( mBuf.size() == PaddedSize )
	{
		return;
	}

	mBuf.resize( PaddedSize );

	uint32_t	 D  = 0xdeadf00d;
	GLubyte		*P1 = &mBuf[ 0 ];
	GLubyte		*P2 = &mBuf[ BUF_PADDING + pSize ];

	for( size_t i = 0 ; i < BUF_PADDING ; i++, P1++, P2++ )
	{
		*P1 = *P2 = (GLubyte)( D & 0x000000ff );

		D = D << 8 | D >> 24;
	}
}

bool TexToBuf::checkBuf( void )
{
	if( mBuf.size() <= BUF_PADDING * 2 )
	{
		return( true );
	}

	uint32_t	 D  = 0xdeadf00d;
	GLubyte		*P1 = &mBuf[ 0 ];
	GLubyte		*P2 = &mBuf[ BUF_PADDING + ( mBuf.size() - BUF_PADDING - BUF_PADDING ) ];

	for( size_t i = 0 ; i < BUF_PADDING ; i++, P1++, P2++ )
	{
		GLubyte		C = (GLubyte)( D & 0x000000ff );

		if( *P1 != C || *P2 != C )
		{
			return( false );
		}

		D = D << 8 | D >> 24;
	}

	return( true );
}

void TexToBuf::fromTex(const FFGLTextureStruct &pTex, const GLuint pBitDepth )
{
	GLint		 InternalFormat;
	GLint		 Format;
	GLenum		 DataFormat;
	size_t		 PixelSize;
	size_t		 FrameSize = pTex.Width * pTex.Height;
	size_t		 BufferSize = pTex.HardwareWidth * pTex.HardwareHeight;

	switch( pBitDepth )
	{
		case FF_DEPTH_16:
			InternalFormat = GL_RGB8;
			Format         = GL_BGR;
			DataFormat     = GL_UNSIGNED_SHORT_5_6_5;
			PixelSize  = 2;
			FrameSize *= 2;
			BufferSize *= 2;
			break;

		case FF_DEPTH_24:
			InternalFormat = GL_RGB8;
#if defined( TARGET_OS_MAC )
			Format         = GL_RGB;
#endif
#if defined( TARGET_OS_WIN )
			Format         = GL_BGR;
#endif
			DataFormat     = GL_UNSIGNED_BYTE;
			PixelSize  = 3;
			FrameSize *= 3;
			BufferSize *= 3;
			break;

		case FF_DEPTH_32:
			InternalFormat = GL_RGBA8;
			Format         = GL_BGRA;
			DataFormat     = GL_UNSIGNED_BYTE;
			PixelSize  = 4;
			FrameSize *= 4;
			BufferSize *= 4;
			break;
	}

	memcpy( &mTex, &pTex, sizeof( FFGLTextureStruct ) );

	mTex.Handle = 0;

	mInf.FrameWidth  = pTex.Width;
	mInf.FrameHeight = pTex.Height;
	mInf.BitDepth    = pBitDepth;
	mInf.Orientation = FF_ORIENTATION_BL;

	resizeBuf( FrameSize );

	glActiveTexture( GL_TEXTURE0 );

	glBindTexture( GL_TEXTURE_2D, pTex.Handle );

	glEnable( GL_TEXTURE_2D );

#if !defined( TARGET_OS_MAC )
	if( GLEW_EXT_pixel_buffer_object && mBufPBO == 0 )
	{
		glGenBuffers( 1, &mBufPBO );
	}
#endif

	if( mBufPBO != 0 )
	{
		GLint			OldBuf;

		glGetIntegerv( GL_PIXEL_PACK_BUFFER_BINDING, &OldBuf );

		glBindBuffer( GL_PIXEL_PACK_BUFFER, mBufPBO );

		glBufferData( GL_PIXEL_PACK_BUFFER, pTex.HardwareWidth * pTex.HardwareHeight * PixelSize, NULL, GL_STREAM_READ );

		glGetTexImage( GL_TEXTURE_2D, 0, Format, DataFormat, 0 );

		unsigned char	*SrcDat = (unsigned char *)glMapBuffer( GL_PIXEL_PACK_BUFFER, GL_READ_ONLY );

		if( SrcDat != 0 )
		{
			if( pTex.Width != pTex.HardwareWidth || pTex.Height != pTex.HardwareHeight )
			{
				// Extract sub region

				const size_t	SrcStride = pTex.HardwareWidth * PixelSize;
				const size_t	DstStride = pTex.Width * PixelSize;

				if( pTex.Width == pTex.HardwareWidth )
				{
					memcpy( &mBuf[ BUF_PADDING ], SrcDat, DstStride * pTex.Height );
				}
				else
				{
					for( size_t y = 0 ; y < pTex.Height ; y++ )
					{
						const unsigned char		*SrcPtr = &SrcDat[ SrcStride * y ];
						unsigned char			*DstPtr = &mBuf[ BUF_PADDING + ( DstStride * y ) ];

						memcpy( DstPtr, SrcPtr, DstStride );
					}
				}
			}
			else
			{
				memcpy( &mBuf[ BUF_PADDING ], SrcDat, mBuf.size() - BUF_PADDING - BUF_PADDING );
			}
		}

		glUnmapBuffer( GL_PIXEL_PACK_BUFFER );

		glBindBuffer( GL_PIXEL_PACK_BUFFER, OldBuf );
	}
	else
	{
		if( pTex.Width != pTex.HardwareWidth || pTex.Height != pTex.HardwareHeight )
		{
			mCnvBuf.resize( BufferSize );

			glGetTexImage( GL_TEXTURE_2D, 0, Format, GL_UNSIGNED_BYTE, &mCnvBuf[ 0 ] );

			const size_t	SrcStride = pTex.HardwareWidth * PixelSize;
			const size_t	DstStride = pTex.Width * PixelSize;

			for( size_t y = 0 ; y < pTex.Height ; y++ )
			{
				const unsigned char		*SrcPtr = &mCnvBuf[ SrcStride * y ];
				unsigned char			*DstPtr = &mBuf[ BUF_PADDING + ( DstStride * y ) ];

				memcpy( DstPtr, SrcPtr, DstStride );
			}
		}
		else
		{
			glGetTexImage( GL_TEXTURE_2D, 0, Format, GL_UNSIGNED_BYTE, &mBuf[ BUF_PADDING ] );
		}
	}

	glDisable( GL_TEXTURE_2D );
}

#if 0
void TexToBuf::fromTexSized( const FFGLTextureStruct &pTex, const DWORD pBitDepth, const size_t pW, const size_t pH )
{
	PixelFormat	 InternalFormat;
	GLint		 Format;
	GLenum		 DataFormat;
	size_t		 PixelSize;
	size_t		 FrameSize = pW * pH;

	switch( pBitDepth )
	{
		case FF_DEPTH_16:
			InternalFormat = PIX_FMT_BGR565;
			Format         = GL_BGR;		// No 565?
			DataFormat     = GL_UNSIGNED_SHORT_5_6_5;
			PixelSize  = 2;
			FrameSize *= 2;
			break;

		case FF_DEPTH_24:
			InternalFormat = PIX_FMT_BGR24;
			Format         = GL_BGR;
			DataFormat     = GL_UNSIGNED_BYTE;
			PixelSize  = 3;
			FrameSize *= 3;
			break;

		case FF_DEPTH_32:
			InternalFormat = PIX_FMT_BGRA;
			Format         = GL_BGRA;
			DataFormat     = GL_UNSIGNED_BYTE;
			PixelSize  = 4;
			FrameSize *= 4;
			break;
	}

	memcpy( &mTex, &pTex, sizeof( FFGLTextureStruct ) );

	mTex.Handle = 0;

	mInf.FrameWidth  = pW;
	mInf.FrameHeight = pH;
	mInf.BitDepth    = pBitDepth;
	mInf.Orientation = FF_ORIENTATION_BL;

	resizeBuf( FrameSize );

	glActiveTexture( GL_TEXTURE0 );

	glBindTexture( GL_TEXTURE_2D, pTex.Handle );

	glEnable( GL_TEXTURE_2D );

	if( GLEW_EXT_pixel_buffer_object && mBufPBO == 0 )
	{
		glGenBuffers( 1, &mBufPBO );
	}

	if( mBufPBO != 0 )
	{
		GLint			OldBuf;

		glGetIntegerv( GL_PIXEL_PACK_BUFFER_BINDING, &OldBuf );

		glBindBuffer( GL_PIXEL_PACK_BUFFER, mBufPBO );

		glBufferData( GL_PIXEL_PACK_BUFFER, pTex.HardwareWidth * pTex.HardwareHeight * PixelSize, NULL, GL_STREAM_READ );

		glGetTexImage( GL_TEXTURE_2D, 0, Format, DataFormat, 0 );

		unsigned char	*SrcDat = (unsigned char *)glMapBuffer( GL_PIXEL_PACK_BUFFER, GL_READ_ONLY );

		if( SrcDat != 0 )
		{
			if( pTex.Width == pW && pTex.Height == pH )
			{
				if( pTex.Width != pTex.HardwareWidth || pTex.Height != pTex.HardwareHeight )
				{
					// Extract sub region

					const size_t	SrcStride = pTex.HardwareWidth * PixelSize;
					const size_t	DstStride = pTex.Width * PixelSize;

					if( pTex.Width == pTex.HardwareWidth )
					{
						memcpy( &mBuf[ BUF_PADDING ], SrcDat, DstStride * pTex.Height );
					}
					else
					{
						for( size_t y = 0 ; y < pTex.Height ; y++ )
						{
							const unsigned char		*SrcPtr = &SrcDat[ SrcStride * y ];
							unsigned char			*DstPtr = &mBuf[ BUF_PADDING + ( DstStride * y ) ];

							memcpy( DstPtr, SrcPtr, DstStride );
						}
					}
				}
				else
				{
					memcpy( &mBuf[ BUF_PADDING ], SrcDat, mBuf.size() - BUF_PADDING - BUF_PADDING );
				}
			}
			else
			{
				SwsContext		*Context;

				Context = sws_getContext(
					pTex.Width, pTex.Height, InternalFormat,
					pW, pH, InternalFormat,
					SWS_FAST_BILINEAR | SWS_PRINT_INFO | SWS_CPU_CAPS_MMX | SWS_CPU_CAPS_MMX2 | SWS_CPU_CAPS_3DNOW | SWS_CPU_CAPS_ALTIVEC | SWS_CPU_CAPS_BFIN,
					0, 0, 0 );

				if( Context != 0 )
				{
					const uint8_t	*SrcPtr[ 1 ] = { reinterpret_cast<const uint8_t *>( SrcDat ) };
					const int		 SrcStride[ 1 ] = { pTex.HardwareWidth * PixelSize };
					uint8_t			*DstPtr[ 1 ] = { reinterpret_cast< uint8_t *>( &mBuf[ BUF_PADDING ] ) };
					const int		 DstStride[ 1 ] = { pW * PixelSize };

					int L = sws_scale( Context, SrcPtr, SrcStride, 0, pTex.Height, DstPtr, DstStride );

					sws_freeContext( Context );
				}
			}
		}

		glUnmapBuffer( GL_PIXEL_PACK_BUFFER );

		glBindBuffer( GL_PIXEL_PACK_BUFFER, OldBuf );
	}
	else
	{
	}

	glBindTexture( GL_TEXTURE_2D, 0 );

	glDisable( GL_TEXTURE_2D );
}
#endif

unsigned char *TexToBuf::getBuf( void )
{
	return( &mBuf[ BUF_PADDING ] );
}

GLuint TexToBuf::getTex( void ) const
{
	return( mTex.Handle );
}

GLsizei TexToBuf::getBufSize()
{
	return( mBuf.size() - BUF_PADDING - BUF_PADDING );
}

double TexToBuf::getTexMaxCoordX( void ) const
{
	return( (double)mTex.Width  / (double)mTex.HardwareWidth );
}

double TexToBuf::getTexMaxCoordY( void ) const
{
	return( (double)mTex.Height / (double)mTex.HardwareHeight );
}

void TexToBuf::makeTex( const VideoInfoStruct &pBuf )
{
	GLint		 InternalFormat;
	GLint		 Format;
	size_t		 FrameSize = pBuf.FrameWidth * pBuf.FrameHeight;

	memcpy( &mInf, &pBuf, sizeof( VideoInfoStruct ) );

	switch( mInf.BitDepth )
	{
		case FF_DEPTH_16:
			InternalFormat = GL_RGB8;
#if defined( TARGET_OS_MAC )
			Format         = GL_RGB;
#elif defined( TARGET_OS_WIN )
			Format         = GL_BGR;
#endif
			FrameSize *= 2;
			break;

		case FF_DEPTH_24:
			InternalFormat = GL_RGB8;
#if defined( TARGET_OS_MAC )
			Format         = GL_RGB;
#elif defined( TARGET_OS_WIN )
			Format         = GL_BGR;
#endif
			FrameSize *= 3;
			break;

		case FF_DEPTH_32:
			InternalFormat = GL_RGBA8;
#if defined( TARGET_OS_MAC )
			Format         = GL_RGBA;
#elif defined( TARGET_OS_WIN )
			Format         = GL_BGRA;
#endif
			FrameSize *= 4;
			break;

		default:
			return;
	}

	resizeBuf( FrameSize );

	if( mInf.FrameWidth != mTex.Width || mInf.FrameHeight != mTex.Height )
	{
		if( mTex.Handle != 0 )
		{
			glDeleteTextures( 1, &mTex.Handle );

			mTex.Handle = 0;
		}

		if( mTexPBO != 0 )
		{
			glDeleteBuffers( 1, &mTexPBO );

			mTexPBO = 0;
		}
	}

	if( mTex.Handle == 0 )
	{
		glGenTextures( 1, &mTex.Handle );

		if( mTex.Handle == 0 )
		{
			return;
		}

		glBindTexture( GL_TEXTURE_2D, mTex.Handle );

		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

//		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

		GLint		MaxSize = 0;

		glGetIntegerv( GL_MAX_TEXTURE_SIZE, &MaxSize );

		mTex.Width          = mInf.FrameWidth;
		mTex.Height         = mInf.FrameHeight;
		mTex.HardwareWidth  = min( (GLint)mInf.FrameWidth,  MaxSize );
		mTex.HardwareHeight = min( (GLint)mInf.FrameHeight, MaxSize );

#if !defined( TARGET_OS_MAC )
		if( !GLEW_ARB_texture_non_power_of_two )
#endif
		{
			GLint		MaxW = MaxSize;
			GLint		MaxH = MaxSize;

			while( MaxW >= (GLint)mTex.Width )
			{
				MaxW >>= 1;
			}

			MaxW <<= 1;

			while( MaxH >= (GLint)mTex.Height )
			{
				MaxH >>= 1;
			}

			MaxH <<= 1;

			mTex.HardwareWidth  = MaxW;
			mTex.HardwareHeight = MaxH;
		}

		glTexImage2D( GL_TEXTURE_2D, 0, InternalFormat, mTex.HardwareWidth, mTex.HardwareHeight, 0, Format, GL_UNSIGNED_BYTE, 0 );

#if !defined( TARGET_OS_MAC )
		if( GLEW_EXT_pixel_buffer_object )
		{
			glGenBuffers( 1, &mTexPBO );

			if( mTexPBO != 0 )
			{
				glBindBuffer( GL_PIXEL_UNPACK_BUFFER, mTexPBO );
				glBufferData( GL_PIXEL_UNPACK_BUFFER, FrameSize, NULL, GL_STREAM_DRAW );
				glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
			}
		}
#endif
	}
}

void TexToBuf::genTex( const VideoInfoStruct &pBuf, const void *pData )
{
	GLint		 InternalFormat;
	GLint		 Format;
	size_t		 FrameSize = pBuf.FrameWidth * pBuf.FrameHeight;

	makeTex( pBuf );

	switch( mInf.BitDepth )
	{
		case FF_DEPTH_16:
			InternalFormat = GL_RGB8;
			Format         = GL_RGB;		// No 565?
			FrameSize *= 2;
			break;

		case FF_DEPTH_24:
			InternalFormat = GL_RGB8;
#if defined( TARGET_OS_MAC )
			Format         = GL_RGB;
#endif
#if defined( TARGET_OS_WIN )
			Format         = GL_BGR;
#endif
			FrameSize *= 3;
			break;

		case FF_DEPTH_32:
			InternalFormat = GL_RGBA8;
			Format         = GL_BGRA;
			FrameSize *= 4;
			break;
	}

	const void	*SrcDat = ( pData == 0 ? &mBuf[ BUF_PADDING ] : pData );

	if( mTex.Handle == 0 )
	{
		return;
	}

	const void			*PBOsrc = SrcDat;

	glBindTexture( GL_TEXTURE_2D, mTex.Handle );

	if( mTexPBO != 0 )
	{
		void		*PBOdst;

		glBindBuffer( GL_PIXEL_UNPACK_BUFFER, mTexPBO );

		if( ( PBOdst = glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY ) ) != 0 )
		{
			memcpy( PBOdst, PBOsrc, FrameSize );

			PBOsrc = 0;

			glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );
		}
		else
		{
			glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
		}
	}

	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, mTex.Width, mTex.Height, Format, GL_UNSIGNED_BYTE, PBOsrc );

	if( PBOsrc == 0 )
	{
		glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
	}
}
