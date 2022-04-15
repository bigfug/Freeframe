
#include "Instance.h"

#if !defined(Q_UNUSED)
#define Q_UNUSED(arg) (void)arg;
#endif

#define	AUDIO_BUFFER_SIZE		(1024)

Instance::Instance( Plugin &pPlugin, FreeFrameApiVersion pApiVersion, plugMainUnion &pResult )
	: InstanceBase( pPlugin, pApiVersion, pResult ), mPlugin( pPlugin ), mStream( 0 ), mChannels( 1 )
{
	PaError		Error;

	PaDeviceIndex	InputDeviceIndex = Pa_GetDefaultInputDevice();

	const PaDeviceInfo *InputDeviceInfo = Pa_GetDeviceInfo( InputDeviceIndex );

	if( InputDeviceInfo != 0 )
	{
		mChannels = min( 1, InputDeviceInfo->maxInputChannels );
	}

	mWaveForm.resize( AUDIO_BUFFER_SIZE * mChannels );

	if( ( Error = Pa_OpenDefaultStream( &mStream, mChannels, 0, paFloat32, 44100, AUDIO_BUFFER_SIZE, &Instance::paCallbackStatic, this ) ) != paNoError )
	{
		const char *ErrorMessage = Pa_GetErrorText( Error );

		std::cerr << ErrorMessage;

		return;
	}

	if( ( Error = Pa_StartStream( mStream ) ) != paNoError )
	{
		const char *ErrorMessage = Pa_GetErrorText( Error );

		std::cerr << ErrorMessage;

		return;
	}
}

Instance::~Instance( void )
{
	PaError		Error;

	if( ( Error = Pa_StopStream( mStream ) ) != paNoError )
	{
		const char *ErrorMessage = Pa_GetErrorText( Error );

		std::cerr << ErrorMessage;
	}

	if( ( Error = Pa_CloseStream( mStream ) ) != paNoError )
	{
		const char *ErrorMessage = Pa_GetErrorText( Error );

		std::cerr << ErrorMessage;
	}
}

void Instance::processOpengl( plugMainUnion &pResult )
{
	const unsigned int	 ParamConfig  = mParams[ PARAM_CONFIG ].getChoice();
	const unsigned int	 ParamArrange = mParams[ PARAM_ARRANGE ].getChoice();
	const float			 ParamScale   = mParams[ PARAM_SCALE  ].getFloat();
	const float			 ParamWidth   = mParams[ PARAM_WIDTH  ].getFloat();

#if defined( FF_VERSION_16 )
	ProcessOpenGLStruct		*GL = reinterpret_cast<ProcessOpenGLStruct *>( pResult.PointerValue );
#else
	ProcessOpenGLStruct		*GL = reinterpret_cast<ProcessOpenGLStruct *>( pResult.ivalue );
#endif

	Plugin::checkError();

	pResult.ivalue = FF_FAIL;

	if( GL == 0 )
	{
		return;
	}

	const float		Scale = float( AUDIO_BUFFER_SIZE ) / float( mViewport.width );

	mWavSrc.resize( mViewport.width );

	float				*WAVDST = &mWavSrc[ 0 ];

	switch( ParamArrange )
	{
		case ARRANGE_LEFT:
			for( int i = 0 ; i < int( mViewport.width ) ; i++ )
			{
				int		vc1 = ( i + 0 ) * mChannels;
				int		vc2 = ( i + 1 ) * mChannels;

				int		v1 = int( float( vc1 ) * float( Scale ) );
				int		v2 = int( float( vc2 ) * float( Scale ) );

				float	vm = mWaveForm[ v1 ];

				for( int j = v1 ; j < v2 ; j += mChannels )
				{
					vm = max( vm, mWaveForm[ j + 0 ] );
				}

				*WAVDST++ = vm * ParamScale;
			}
			break;

		case ARRANGE_MIRROR_LEFT:
			{
				const int		HalfWidth = int( mViewport.width / 2 );
				const float		HalfScale = float( AUDIO_BUFFER_SIZE ) / float( HalfWidth );

				for( int i = 0 ; i < HalfWidth ; i++ )
				{
					int		vc1 = ( i + 0 ) * mChannels;
					int		vc2 = ( i + 1 ) * mChannels;

					int		v1 = int( float( vc1 ) * float( HalfScale ) );
					int		v2 = int( float( vc2 ) * float( HalfScale ) );

					float	vm = mWaveForm[ v1 ];

					for( int j = v1 ; j < v2 ; j += mChannels )
					{
						vm = max( vm, mWaveForm[ j + 0 ] );
					}

					*WAVDST++ = vm * ParamScale;
				}

				for( int i = 0 ; i < HalfWidth ; i++ )
				{
					mWavSrc[ HalfWidth + ( HalfWidth - i - 1 ) ] = mWavSrc[ i ];
				}
			}
			break;

		case ARRANGE_MIRROR_REVERSE_LEFT:
			{
				const int		HalfWidth = int( mViewport.width / 2 );
				const float		HalfScale = float( AUDIO_BUFFER_SIZE ) / float( HalfWidth );

				WAVDST = &mWavSrc[ HalfWidth ];

				for( int i = 0 ; i < HalfWidth ; i++ )
				{
					int		vc1 = ( i + 0 ) * mChannels;
					int		vc2 = ( i + 1 ) * mChannels;

					int		v1 = int( float( vc1 ) * float( HalfScale ) );
					int		v2 = int( float( vc2 ) * float( HalfScale ) );

					float	vm = mWaveForm[ v1 ];

					for( int j = v1 ; j < v2 ; j += mChannels )
					{
						vm = max( vm, mWaveForm[ j + 0 ] );
					}

					*WAVDST++ = vm * ParamScale;
				}

				for( int i = 0 ; i < HalfWidth ; i++ )
				{
					mWavSrc[ i ] = mWavSrc[ HalfWidth + ( HalfWidth - i - 1 ) ];
				}
			}
			break;
	}

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	glPointSize( ParamWidth );

	glEnableClientState( GL_VERTEX_ARRAY );

	GLenum			Mode = GL_LINE_STRIP;

	switch( ParamConfig )
	{
		case EFFECT_POINTS_1:
			Mode = GL_POINTS;

		case EFFECT_LINE_1:
			{
				if( ParamConfig == EFFECT_LINE_1 && ParamWidth != 1.0f )
				{
					const float		R = float( ParamWidth ) / float( mViewport.height );

					mElements.resize( mViewport.width * 4 );

					GLfloat		*DST = &mElements[ 0 ];

					for( GLuint i = 0 ; i < mViewport.width ; i++ )
					{
						GLfloat		x = -1.0f + ( float( i ) / ( float( mViewport.width - 1 ) / 2.0f ) );
						GLfloat		y = mWavSrc[ i ];

						*DST++ = x;
						*DST++ = y - R;
						*DST++ = x;
						*DST++ = y + R;
					}

					//Mode = GL_POINTS;
				}
				else
				{
					mElements.resize( mViewport.width * 2 );

					GLfloat		*DST = &mElements[ 0 ];

					for( GLuint i = 0 ; i < mViewport.width ; i++ )
					{
						GLfloat		x = -1.0f + ( float( i ) / ( float( mViewport.width - 1 ) / 2.0f ) );
						GLfloat		y = mWavSrc[ i ];

						*DST++ = x;
						*DST++ = y;
					}
				}

				drawElements( Mode );
			}
			break;

		case EFFECT_FILL_1:
			{
				Mode = GL_LINES;

				mElements.resize( mViewport.width * 4 );

				GLfloat		*DST = &mElements[ 0 ];

				for( GLuint i = 0 ; i < mViewport.width ; i++ )
				{
					GLfloat		x = -1.0f + ( float( i ) / ( float( mViewport.width - 1 ) / 2.0f ) );
					GLfloat		y = fabs( mWavSrc[ i ] );

					*DST++ = x;
					*DST++ = y;
					*DST++ = x;
					*DST++ = -y;
				}

				drawElements( Mode );
			}
			break;

		case EFFECT_POINTS2_1:
			Mode = GL_POINTS;

		case EFFECT_LINE2_1:
			{
				mElements.resize( mViewport.width * 2 );

				GLfloat		*DST = &mElements[ 0 ];

				for( GLuint i = 0 ; i < mViewport.width ; i++ )
				{
					GLfloat		x = -1.0f + ( float( i ) / ( float( mViewport.width - 1 ) / 2.0f ) );
					GLfloat		y = mWavSrc[ i ];

					*DST++ = x;
					*DST++ = fabs( y );
				}

				drawElements( Mode );

				DST = &mElements[ 0 ];

				for( GLuint i = 0 ; i < mViewport.width ; i++ )
				{
					GLfloat		x = -1.0f + ( float( i ) / ( float( mViewport.width - 1 ) / 2.0f ) );
					GLfloat		y = mWavSrc[ i ];

					*DST++ = x;
					*DST++ = -fabs( y );
				}

				drawElements( Mode );
			}
			break;

		case EFFECT_FILL_BOTTOM_1:
			{
				mElements.resize( mViewport.width * 4 );

				GLfloat		*DST = &mElements[ 0 ];

				for( GLuint i = 0 ; i < mViewport.width ; i++ )
				{
					GLfloat		x = -1.0f + ( float( i ) / ( float( mViewport.width - 1 ) / 2.0f ) );
					GLfloat		y = mWavSrc[ i ];

					*DST++ = x;
					*DST++ = y;
					*DST++ = x;
					*DST++ = -1.0f;
				}

				drawElements( Mode );
			}
			break;

		case EFFECT_FILL_TOP_1:
			{
				mElements.resize( mViewport.width * 4 );

				GLfloat		*DST = &mElements[ 0 ];

				for( GLuint i = 0 ; i < mViewport.width ; i++ )
				{
					GLfloat		x = -1.0f + ( float( i ) / ( float( mViewport.width - 1 ) / 2.0f ) );
					GLfloat		y = mWavSrc[ i ];

					*DST++ = x;
					*DST++ = y;
					*DST++ = x;
					*DST++ = 1.0f;
				}

				drawElements( Mode );
			}
			break;
	}

	glDisableClientState( GL_VERTEX_ARRAY );

	glPointSize( 1.0f );

	pResult.ivalue = FF_SUCCESS;
}

void Instance::drawElements( GLenum pMode )
{
	glVertexPointer( 2, GL_FLOAT, 0, &mElements[ 0 ] );

	glDrawArrays( pMode, 0, mElements.size() / 2 );
}

int Instance::paCallbackStatic( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{
	return( reinterpret_cast<Instance *>( userData )->paCallback( inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags ) );
}

int Instance::paCallback( const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags )
{
	Q_UNUSED( outputBuffer )
	Q_UNUSED( framesPerBuffer )
	Q_UNUSED( timeInfo )
	Q_UNUSED( statusFlags )

	memcpy( &mWaveForm[ 0 ], inputBuffer, sizeof( float ) * framesPerBuffer );

	return( paNoError );
}
