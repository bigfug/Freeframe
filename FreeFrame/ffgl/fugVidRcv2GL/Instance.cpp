
#include <gl/glew.h>

#include "Instance.h"

#include <iostream>

Instance::Instance( Plugin &pPlugin, FreeFrameApiVersion pApiVersion, plugMainUnion &pResult )
: InstanceBase( pPlugin, pApiVersion, pResult ), mPlugin( pPlugin ), mDecoder( 0 ), mPBO( 0 )
{
	mLastProtocol = FUG_STREAM_UNKNOWN;
	mLastPort     = 0;
	mLastReceive  = false;

	memset( &mTexInf, 0, sizeof( mTexInf ) );

	memset( &mPrvInf, 0, sizeof( mPrvInf ) );

	pResult.ivalue = FF_SUCCESS;
}

Instance::~Instance( void )
{
	if( mTexInf.Handle != 0 )
	{
		glDeleteTextures( 1, &mTexInf.Handle );

		memset( &mTexInf, 0, sizeof( mTexInf ) );
	}

	if( mPBO != 0 )
	{
		glDeleteBuffers( 1, &mPBO );

		mPBO = 0;
	}

	if( mDecoder != 0 )
	{
		fugDeleteDecoder( mDecoder );

		mDecoder = 0;
	}
}

void Instance::processOpengl( plugMainUnion &pResult )
{
	const fugStreamProtocol	 ParamProtocol = (fugStreamProtocol)( mParams[ PARAM_PROTOCOL ].getChoice() + 1 );
	const unsigned short	 ParamPort     = mParams[ PARAM_PORT ].getInt();
#if !defined( FF_IS_SOURCE )
	const bool				 ParamReceive  = mParams[ PARAM_RECEIVE ].getBool();
#endif

	ProcessOpenGLStruct		*GL = reinterpret_cast<ProcessOpenGLStruct *>( pResult.ivalue );

	pResult.ivalue = FF_FAIL;

	if( GL == 0 )
	{
		return;
	}

#if !defined( FF_IS_SOURCE )
	if( GL->numInputTextures != 1 )
	{
		return;
	}

	if( GL->inputTextures == 0 )
	{
		return;
	}

	FFGLTextureStruct		*TS = GL->inputTextures[ 0 ];

	if( TS == 0 )
	{
		return;
	}
#endif

	double		mx = 1.0;
	double		my = 1.0;
	bool		flipY = false;

	glActiveTexture( GL_TEXTURE0 );

#if !defined( FF_IS_SOURCE )
	if( ParamReceive )
#endif
	{
		if( ParamProtocol != mLastProtocol ||
		#if !defined( FF_IS_SOURCE )
			ParamReceive  != mLastReceive ||
		#endif
			ParamPort     != mLastPort )
		{
			if( mTexInf.Handle != 0 )
			{
				glDeleteTextures( 1, &mTexInf.Handle );

				memset( &mTexInf, 0, sizeof( mTexInf ) );
			}

			if( mPBO != 0 )
			{
				glDeleteBuffers( 1, &mPBO );

				mPBO = 0;
			}

			if( mDecoder != 0 )
			{
				fugDeleteDecoder( mDecoder );

				mDecoder = 0;
			}

			mVidInf.frameWidth  = 512;
			mVidInf.frameHeight = 512;
			mVidInf.bitDepth    = FUG_VID_32BITVIDEO;
			mVidInf.orientation = FUG_ORIEN_BOT_LEFT;

			if( ( mDecoder = fugGetDecoder( ParamProtocol, &mVidInf ) ) != 0 )
			{
				fugSetSourceAddress( mDecoder, FUG_BASE_PORT + ParamPort );

				fugSetAdaptiveBuffer( mDecoder, true );
			}
		}

		if( mDecoder != 0 )
		{
			unsigned char		*SrcDat = fugLockReadBuffer( mDecoder, 0 );

			if( SrcDat != 0 )
			{
				mVidInf.frameWidth  = fugGetRecvWidth( mDecoder );
				mVidInf.frameHeight = fugGetRecvHeight( mDecoder );
				mVidInf.bitDepth    = fugGetRecvDepth( mDecoder );
				mVidInf.orientation = fugGetRecvOrientation( mDecoder );

				if( mTexInf.Handle == 0 || memcmp( &mPrvInf, &mVidInf, sizeof( fugVideoInfo ) ) != 0 )
				{
					std::cout << "W=" << mVidInf.frameWidth << " H=" << mVidInf.frameHeight << " D=" << mVidInf.bitDepth << std::endl;

					if( mTexInf.Handle != 0 )
					{
						glDeleteTextures( 1, &mTexInf.Handle );

						memset( &mTexInf, 0, sizeof( mTexInf ) );
					}

					if( mPBO != 0 )
					{
						glDeleteBuffers( 1, &mPBO );

						mPBO = 0;
					}

					memcpy( &mPrvInf, &mVidInf, sizeof( fugVideoInfo ) );

					glGenTextures( 1, &mTexInf.Handle );

					if( mTexInf.Handle != 0 )
					{
						glBindTexture( GL_TEXTURE_2D, mTexInf.Handle );

						glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

						glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
						glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

//						glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//						glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

						switch( mVidInf.bitDepth )
						{
							case FUG_VID_16BITVIDEO:
								mInternalFormat = GL_RGB8;
								mFormat         = GL_RGB;		// No 565?
								break;

							case FUG_VID_24BITVIDEO:
								mInternalFormat = GL_RGB8;
								mFormat         = GL_RGB;
								break;

							case FUG_VID_32BITVIDEO:
								mInternalFormat = GL_RGBA8;
								mFormat         = GL_BGRA;
								break;
						}

						GLint		MaxSize = 0;

						glGetIntegerv( GL_MAX_TEXTURE_SIZE, &MaxSize );

						mTexInf.Width          = mVidInf.frameWidth;
						mTexInf.Height         = mVidInf.frameHeight;
						mTexInf.HardwareWidth  = min( (GLint)mVidInf.frameWidth,  MaxSize );
						mTexInf.HardwareHeight = min( (GLint)mVidInf.frameHeight, MaxSize );

#if defined( TARGET_OS_WIN )
						if( !GLEW_ARB_texture_non_power_of_two )
#endif
						{
							while( MaxSize >= (GLint)mTexInf.Width && MaxSize >= (GLint)mTexInf.Height )
							{
								MaxSize >>= 1;
							}

							MaxSize <<= 1;

							mTexInf.HardwareWidth = mTexInf.HardwareHeight = MaxSize;
						}

						if( false ) // GLEW_EXT_pixel_buffer_object )
						{
							glGenBuffers( 1, &mPBO );

							if( mPBO != 0 )
							{
								glBindBuffer( GL_PIXEL_UNPACK_BUFFER, mPBO );
								glBufferData( GL_PIXEL_UNPACK_BUFFER, fugGetByteSize( &mVidInf ), NULL, GL_STREAM_DRAW );
								glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
							}
						}

						glTexImage2D( GL_TEXTURE_2D, 0, mInternalFormat, mTexInf.HardwareWidth, mTexInf.HardwareHeight, 0, mFormat, GL_UNSIGNED_BYTE, 0 );
					}
				}

				if( mTexInf.Handle != 0 )
				{
					void			*PBOsrc = SrcDat;

					glBindTexture( GL_TEXTURE_2D, mTexInf.Handle );

					if( mPBO != 0 )
					{
						void		*PBOdst;

						glBindBuffer( GL_PIXEL_UNPACK_BUFFER, mPBO );

						if( ( PBOdst = glMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY ) ) != 0 )
						{
							memcpy( PBOdst, PBOsrc, fugGetByteSize( &mVidInf ) );

							PBOsrc = 0;

							glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );
						}
						else
						{
							glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
						}
					}

					glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, mTexInf.Width, mTexInf.Height, mFormat, GL_UNSIGNED_BYTE, PBOsrc );

					//std::cout << "Received frame " << int( fugGetFrameNumber( mDecoder ) ) << " = 0x" << uint32_t( SrcDat[ 0 ] ) << uint32_t( SrcDat[ 1 ] ) << uint32_t( SrcDat[ 2 ] ) << uint32_t( SrcDat[ 3 ] ) << std::endl;

					if( PBOsrc == 0 )
					{
						glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
					}
				}

				fugUnlockReadBuffer( mDecoder );
			}
		}

		if( mTexInf.Handle != 0 )
		{
			if( mVidInf.orientation == FUG_ORIEN_BOT_LEFT )
			{
				flipY = true;
			}

			mx = (double)mTexInf.Width  / (double)mTexInf.HardwareWidth;
			my = (double)mTexInf.Height / (double)mTexInf.HardwareHeight;

			glBindTexture( GL_TEXTURE_2D, mTexInf.Handle );
		}
		else
		{
			glBindTexture( GL_TEXTURE_2D, 0 );
		}
	}
#if !defined( FF_IS_SOURCE )
	else
	{
		if( mDecoder != 0 )
		{
			fugDeleteDecoder( mDecoder );

			mDecoder = 0;
		}

		if( mPBO != 0 )
		{
			glDeleteBuffers( 1, &mPBO );

			mPBO = 0;
		}

		if( mTexInf.Handle != 0 )
		{
			glDeleteTextures( 1, &mTexInf.Handle );

			memset( &mTexInf, 0, sizeof( mTexInf ) );

			memset( &mPrvInf, 0, sizeof( mPrvInf ) );
		}

		glBindTexture( GL_TEXTURE_2D, TS->Handle );

		mx = (double)TS->Width  / (double)TS->HardwareWidth;
		my = (double)TS->Height / (double)TS->HardwareHeight;
	}
#endif

	if( mTexInf.Handle == 0 )
	{
		glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
		glClear( GL_COLOR_BUFFER_BIT );
	}
	else
	{
		glEnable( GL_TEXTURE_2D );

		glColor4f( 1.0, 1.0, 1.0, 1.0 );

		if( !flipY )
		{
			glBegin( GL_QUADS );
				glTexCoord2d( 0.0, 0.0 );
				glVertex2d( -1.0, -1.0 );
				glTexCoord2d( mx, 0.0 );
				glVertex2d( 1.0, -1.0 );
				glTexCoord2d( mx, my );
				glVertex2d( 1.0, 1.0 );
				glTexCoord2d( 0.0, my );
				glVertex2d( -1.0, 1.0 );
			glEnd();
		}
		else
		{
			glBegin( GL_QUADS );
				glTexCoord2d( 0.0, my );
				glVertex2d( -1.0, -1.0 );
				glTexCoord2d( mx, my );
				glVertex2d( 1.0, -1.0 );
				glTexCoord2d( mx, 0.0 );
				glVertex2d( 1.0, 1.0 );
				glTexCoord2d( 0.0, 0.0 );
				glVertex2d( -1.0, 1.0 );
			glEnd();
		}

		glDisable( GL_TEXTURE_2D );

		glBindTexture( GL_TEXTURE_2D, 0 );
	}

#if !defined( FF_IS_SOURCE )
	mLastReceive  = ParamReceive;
#endif
	mLastProtocol = ParamProtocol;
	mLastPort     = ParamPort;

	pResult.ivalue = FF_SUCCESS;
}
