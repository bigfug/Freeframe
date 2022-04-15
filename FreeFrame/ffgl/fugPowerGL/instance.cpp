
#include "instance.h"

#include <algorithm>

using namespace std;

#if !defined(Q_UNUSED)
#define Q_UNUSED(arg) (void)arg;
#endif

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

	mWaveWindow.resize( AUDIO_BUFFER_SIZE );

	mWaveFFT.resize( AUDIO_BUFFER_SIZE );

	mWavePower.resize( AUDIO_BUFFER_SIZE / 2 );

	mWaveLinear.resize( AUDIO_BUFFER_SIZE / 2 );

	mWaveBands.resize( AUDIO_BUFFER_SIZE / 2 );

	if( ( Error = Pa_OpenDefaultStream( &mStream, mChannels, 0, paFloat32, AUDIO_SAMPLE_RATE, AUDIO_BUFFER_SIZE, &Instance::paCallbackStatic, this ) ) != paNoError )
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
	const float			 ParamWidth		= mParams[ PARAM_WIDTH  ].getFloat();
	const unsigned int	 ParamBands		= bands();
	const float			 ParamBandWidth = mParams[ PARAM_BAND_WIDTH  ].getFloat();
	const float			 ParamLines		= mParams[ PARAM_LINES ].getFloat();
	const unsigned int	 ParamPosition	= mParams[ PARAM_POSITION ].getChoice();

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

	mWaveData.resize( ParamBands );

	//float				*WAVDST = &mWaveData[ 0 ];

	switch( ParamArrange )
	{
		case ARRANGE_LEFT:
			mWaveData = mWaveBands;
			break;

		case ARRANGE_MIRROR_LEFT:
			for( int i = 0 ; i < int( ParamBands / 2 ) ; i++ )
			{
				mWaveData[ i ] = mWaveData[ ParamBands - 1 - i ] = ( mWaveBands[ ( i * 2 ) + 0 ] + mWaveBands[ ( i * 2 ) + 1 ] ) / 2.0f;
			}
			break;

		case ARRANGE_MIRROR_REVERSE_LEFT:
			{
				const int	ParamBandsHalf = ParamBands / 2;

				for( int i = 0 ; i < ParamBandsHalf ; i++ )
				{
					mWaveData[ i ] = mWaveData[ ParamBands - 1 - i ] = ( mWaveBands[ ParamBands - 1 - ( i * 2 ) + 0 ] + mWaveBands[ ParamBands - 1 - ( i * 2 ) - 1 ] ) / 2.0f;
				}
			}
			break;
	}

	for( unsigned int i = 0 ; i < ParamBands ; i++ )
	{
		mWaveData[ i ] *= ParamScale;
	}

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	glPointSize( ParamWidth );

	glEnableClientState( GL_VERTEX_ARRAY );

	GLenum			Mode = GL_LINE_STRIP;

	switch( ParamConfig )
	{
		case EFFECT_LINE_1:
			{
				Mode = GL_LINES;

				mElements.resize( ( ParamBands - 1 ) * 4 );

				GLfloat		*DST = &mElements[ 0 ];

				for( GLuint i = 1 ; i < ParamBands ; i++ )
				{
					GLfloat		x = -1.0f + ( float( i + 0 ) / ( float( ParamBands - 0 ) / 2.0f ) );
					GLfloat		y1, y2;

					switch( ParamPosition )
					{
						case POSITION_BOTTOM:
							y1 = -1.0f + ( mWaveData[ i - 1 ] * 2.0f );
							y2 = -1.0f + ( mWaveData[ i + 0 ] * 2.0f );
							break;

						case POSITION_TOP:
							y1 = +1.0f - ( mWaveData[ i - 1 ] * 2.0f );
							y2 = +1.0f - ( mWaveData[ i + 0 ] * 2.0f );
							break;

						case POSITION_CENTER1:
							y1 = mWaveData[ i - 1 ];
							y2 = mWaveData[ i + 0 ];
							break;

						case POSITION_CENTER2:
							y1 = -mWaveData[ i - 1 ];
							y2 = -mWaveData[ i + 0 ];
							break;
					}

					*DST++ = x;
					*DST++ = y1;
					*DST++ = x;
					*DST++ = y2;
				}

				drawElements( Mode );
			}

		case EFFECT_POINTS_1:
			if( ParamWidth == 1.0f )
			{
				Mode = GL_LINES;

				mElements.resize( ParamBands * 4 );

				GLfloat		*DST = &mElements[ 0 ];

				for( GLuint i = 0 ; i < ParamBands ; i++ )
				{
					GLfloat		x1 = -1.0f + ( float( i + 0 ) / ( float( ParamBands - 0 ) / 2.0f ) );
					GLfloat		x2 = -1.0f + ( float( i + 1 ) / ( float( ParamBands - 0 ) / 2.0f ) );
					GLfloat		y;

					switch( ParamPosition )
					{
						case POSITION_BOTTOM:
							y = -1.0f + ( mWaveData[ i ] * 2.0f );
							break;

						case POSITION_TOP:
							y = +1.0f - ( mWaveData[ i ] * 2.0f );
							break;

						case POSITION_CENTER1:
							y = mWaveData[ i ];
							break;

						case POSITION_CENTER2:
							y = -mWaveData[ i ];
							break;
					}

					*DST++ = x1;
					*DST++ = y;
					*DST++ = x2;
					*DST++ = y;
				}

				drawElements( Mode );
			}
			else
			{
				Mode = GL_QUADS;

				mElements.resize( ParamBands * 8 );

				GLfloat		*DST = &mElements[ 0 ];

				for( GLuint i = 0 ; i < ParamBands ; i++ )
				{
					GLfloat		x1 = -1.0f + ( float( i + 0 ) / ( float( ParamBands - 0 ) / 2.0f ) );
					GLfloat		x2 = -1.0f + ( float( i + 1 ) / ( float( ParamBands - 0 ) / 2.0f ) );
					GLfloat		y1, y2;

					switch( ParamPosition )
					{
						case POSITION_BOTTOM:
							y1 = -1.0f + ( mWaveData[ i ] * 2.0f );
							break;

						case POSITION_TOP:
							y1 = +1.0f - ( mWaveData[ i ] * 2.0f );
							break;

						case POSITION_CENTER1:
							y1 = mWaveData[ i ];
							break;

						case POSITION_CENTER2:
							y1 = -mWaveData[ i ];
							break;
					}

					y1 -= ( ParamWidth / float( mViewport.width ) ) * 0.5f;
					y2  = y1 + ( ParamWidth / float( mViewport.width ) );

					*DST++ = x1;
					*DST++ = y1;
					*DST++ = x2;
					*DST++ = y1;
					*DST++ = x2;
					*DST++ = y2;
					*DST++ = x1;
					*DST++ = y2;
				}

				drawElements( Mode );
			}
			break;

		case EFFECT_FILL_1:
			{
				Mode = GL_QUADS;

				mElements.resize( ParamBands * 8 );

				GLfloat		*DST = &mElements[ 0 ];

				for( GLuint i = 0 ; i < ParamBands ; i++ )
				{
					GLfloat		x1 = -1.0f + ( float( i + 0 ) / ( float( ParamBands - 1 ) / 2.0f ) );
					GLfloat		x2 = -1.0f + ( float( i + 1 ) / ( float( ParamBands - 1 ) / 2.0f ) );
					GLfloat		y1 = mWaveData[ i ];
					GLfloat		y2 = -y1;

					GLfloat		xw = x2 - x1;
					GLfloat		bw = xw * ParamBandWidth;

					x1 = x1 + ( xw * 0.5f ) - ( bw * 0.5f );
					x2 = x1 + bw;

					*DST++ = x1;
					*DST++ = y1;
					*DST++ = x2;
					*DST++ = y1;
					*DST++ = x2;
					*DST++ = y2;
					*DST++ = x1;
					*DST++ = y2;
				}

				drawElements( Mode );
			}
			break;

		case EFFECT_LINE2_1:
			{
				Mode = GL_LINES;

				mElements.resize( ( ParamBands - 1 ) * 4 );

				GLfloat		*DST = &mElements[ 0 ];

				for( GLuint i = 1 ; i < ParamBands ; i++ )
				{
					GLfloat		x = -1.0f + ( float( i + 0 ) / ( float( ParamBands - 0 ) / 2.0f ) );
					GLfloat		y1 = fabs( mWaveData[ i - 1 ] );
					GLfloat		y2 = fabs( mWaveData[ i + 0 ] );

					*DST++ = x;
					*DST++ = y1;
					*DST++ = x;
					*DST++ = y2;
				}

				drawElements( Mode );

				DST = &mElements[ 0 ];

				for( GLuint i = 1 ; i < ParamBands ; i++ )
				{
					GLfloat		x = -1.0f + ( float( i + 0 ) / ( float( ParamBands - 0 ) / 2.0f ) );
					GLfloat		y1 = -fabs( mWaveData[ i - 1 ] );
					GLfloat		y2 = -fabs( mWaveData[ i + 0 ] );

					*DST++ = x;
					*DST++ = y1;
					*DST++ = x;
					*DST++ = y2;
				}

				drawElements( Mode );
			}

		case EFFECT_POINTS2_1:
			if( ParamWidth == 1.0f )
			{
				Mode = GL_LINES;

				mElements.resize( ParamBands * 4 );

				GLfloat		*DST = &mElements[ 0 ];

				for( GLuint i = 0 ; i < ParamBands ; i++ )
				{
					GLfloat		x1 = -1.0f + ( float( i + 0 ) / ( float( ParamBands - 0 ) / 2.0f ) );
					GLfloat		x2 = -1.0f + ( float( i + 1 ) / ( float( ParamBands - 0 ) / 2.0f ) );
					GLfloat		y = mWaveData[ i ];

					*DST++ = x1;
					*DST++ = fabs( y );
					*DST++ = x2;
					*DST++ = fabs( y );
				}

				drawElements( Mode );

				DST = &mElements[ 0 ];

				for( GLuint i = 0 ; i < ParamBands ; i++ )
				{
					GLfloat		x1 = -1.0f + ( float( i + 0 ) / ( float( ParamBands - 0 ) / 2.0f ) );
					GLfloat		x2 = -1.0f + ( float( i + 1 ) / ( float( ParamBands - 0 ) / 2.0f ) );
					GLfloat		y = mWaveData[ i ];

					*DST++ = x1;
					*DST++ = -fabs( y );
					*DST++ = x2;
					*DST++ = -fabs( y );
				}

				drawElements( Mode );
			}
			else
			{
				Mode = GL_QUADS;

				mElements.resize( ParamBands * 8 );

				GLfloat		*DST = &mElements[ 0 ];

				for( GLuint i = 0 ; i < ParamBands ; i++ )
				{
					GLfloat		x1 = -1.0f + ( float( i + 0 ) / ( float( ParamBands - 0 ) / 2.0f ) );
					GLfloat		x2 = -1.0f + ( float( i + 1 ) / ( float( ParamBands - 0 ) / 2.0f ) );
					GLfloat		y1, y2;

					y1 = fabs( mWaveData[ i ] ) - ( ( ParamWidth / float( mViewport.width ) ) * 0.5f );
					y2 = y1 + ( ParamWidth / float( mViewport.width ) );

					*DST++ = x1;
					*DST++ = y1;
					*DST++ = x2;
					*DST++ = y1;
					*DST++ = x2;
					*DST++ = y2;
					*DST++ = x1;
					*DST++ = y2;
				}

				drawElements( Mode );

				DST = &mElements[ 0 ];

				for( GLuint i = 0 ; i < ParamBands ; i++ )
				{
					GLfloat		x1 = -1.0f + ( float( i + 0 ) / ( float( ParamBands - 0 ) / 2.0f ) );
					GLfloat		x2 = -1.0f + ( float( i + 1 ) / ( float( ParamBands - 0 ) / 2.0f ) );
					GLfloat		y1, y2;

					y1 = -fabs( mWaveData[ i ] ) - ( ( ParamWidth / float( mViewport.width ) ) * 0.5f );
					y2 = y1 + ( ParamWidth / float( mViewport.width ) );

					*DST++ = x1;
					*DST++ = y1;
					*DST++ = x2;
					*DST++ = y1;
					*DST++ = x2;
					*DST++ = y2;
					*DST++ = x1;
					*DST++ = y2;
				}

				drawElements( Mode );
			}
			break;

		case EFFECT_BANDS:
			{
				Mode = GL_QUADS;

				mElements.resize( ParamBands * 8 );

				GLfloat		*DST = &mElements[ 0 ];

				for( GLuint i = 0 ; i < ParamBands ; i++ )
				{
					GLfloat		x1 = -1.0f + ( float( i + 0 ) / ( float( ParamBands - 1 ) / 2.0f ) );
					GLfloat		x2 = -1.0f + ( float( i + 1 ) / ( float( ParamBands - 1 ) / 2.0f ) );
					GLfloat		y1, y2;

					switch( ParamPosition )
					{
						case POSITION_BOTTOM:
							y1 = -1.0f;
							y2 = -1.0f + ( mWaveData[ i ] * ParamScale * 2.0f );
							break;

						case POSITION_TOP:
							y1 = +1.0f;
							y2 = +1.0f - ( mWaveData[ i ] * ParamScale * 2.0f );
							break;

						case POSITION_CENTER1:
							y1 = -1.0f;
							y2 = mWaveData[ i ] * ParamScale;
							break;

						case POSITION_CENTER2:
							y1 = +1.0f;
							y2 = 0.0f - ( mWaveData[ i ] * ParamScale );
							break;
					}

					GLfloat		xw = x2 - x1;
					GLfloat		bw = xw * ParamBandWidth;

					x1 = x1 + ( xw * 0.5f ) - ( bw * 0.5f );
					x2 = x1 + bw;

					*DST++ = x1;
					*DST++ = y1;
					*DST++ = x2;
					*DST++ = y1;
					*DST++ = x2;
					*DST++ = y2;
					*DST++ = x1;
					*DST++ = y2;
				}

				drawElements( Mode );
			}
			break;
	}

	glDisableClientState( GL_VERTEX_ARRAY );

	glPointSize( 1.0f );

	if( ParamLines > 0.0f )
	{
		const float		yp = max( float( mViewport.height ) * ( ( 1.0f - ParamLines ) * 0.5f ), 2.0f );

		glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );

		glBegin( GL_LINES );

		for( GLuint y = 0 ; y < mViewport.height ; y += GLuint( yp ) )
		{
			glVertex2f( -1.0f, -1.0f + ( float( y ) / ( float( mViewport.height - 1 ) / 2.0f ) ) );
			glVertex2f( +1.0f, -1.0f + ( float( y ) / ( float( mViewport.height - 1 ) / 2.0f ) ) );
		}

		glEnd();
	}

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

	const float	N1 = AUDIO_BUFFER_SIZE - 1;

	for( int i = 0; i < AUDIO_BUFFER_SIZE ; i++ )
	{
		const float PN = 2.0f * float( M_PI ) * float( i );
		const float CN = cosf( PN / N1 );
		const float CM = 1.0f - CN;
		const float HW = 0.5f * CM;

		mWaveWindow[ i ] = static_cast<const float *>( inputBuffer )[ i ] * HW;
	}

	mFFT.real2complex( &mWaveWindow[ 0 ], &mWaveFFT[ 0 ], framesPerBuffer );

	for( int i = 0 ; i < AUDIO_BUFFER_SIZE / 2 ; i++ )
	{
		const float	re = ( real( mWaveFFT.at( i ) ) * 2.0f ) / float( AUDIO_BUFFER_SIZE );
		const float im = ( imag( mWaveFFT.at( i ) ) * 2.0f ) / float( AUDIO_BUFFER_SIZE );

		mWavePower[ i ] = sqrtf( re * re + im * im );

		mWavePower[ i ] *= 10.0f;

		mWaveLinear[ i ] = 0.0f;
	}

	const float		FUND = float( AUDIO_SAMPLE_RATE ) / float( AUDIO_BUFFER_SIZE );
	const float		RNGE = float( AUDIO_SAMPLE_RATE / 2 ) / float( mWaveLinear.size() );

	for( int j = 0 ; j < int( mWaveLinear.size() ) ; j++ )
	{
		const float		linLow = ( 1.0f - log10f( 1.0f + ( 9.0f * ( float( mWaveLinear.size() - 0 - j ) / float( mWaveLinear.size() ) ) ) ) ) * float( AUDIO_SAMPLE_RATE / 2 );
		const float		linHi  = ( 1.0f - log10f( 1.0f + ( 9.0f * ( float( mWaveLinear.size() - 1 - j ) / float( mWaveLinear.size() ) ) ) ) ) * float( AUDIO_SAMPLE_RATE / 2 );
		const float		linMid = linLow + ( ( linHi - linLow ) * 0.5f );

		const int		freq1 = min<int>( linLow / RNGE, mWavePower.size() - 2 );
		const int		freq2 = min<int>( linHi  / RNGE, mWavePower.size() - 2 );

		int				c = 0;

		for( int i = freq1 + 1 ; i <= freq2 + 1 ; i++ )
		{
			const float		freqLow  = FUND * float( i - 1 );
			const float		freqMid  = freqLow + ( FUND * 0.5f );
			//const float		freqHigh = FUND * float( i );

			if( freqMid < linMid )
			{
				//const float		s = ( freqMid - linLow ) / ( linMid - linLow );

				mWaveLinear[ j ] += mWavePower[ i ];
			}
			else
			{
				//const float		s = 1.0f - ( ( freqMid - linMid ) / ( freqMid - linLow ) );

				mWaveLinear[ j ] += mWavePower[ i ];
			}

			c++;
		}

		mWaveLinear[ j ] = log10f( 1.0f + ( 9.0f * ( mWaveLinear[ j ] / float( c ) ) ) );
	}

	const unsigned int	 ParamBands  = bands();
	const float			 ParamAttack = mParams[ PARAM_ATTACK ].getFloat();
	const float			 ParamDecay  = mParams[ PARAM_DECAY ].getFloat();

	const float	BAND_SCALE = float( mWaveLinear.size() ) / float( ParamBands );

	for( int i = 0 ; i < int( ParamBands ) ; i++ )
	{
		float		h = 0.0f;

		for( int j = int( BAND_SCALE * float( i ) ) ; j < int( BAND_SCALE * float( i + 1 ) ) ; j++ )
		{
			if( mWaveLinear[ j ] > h )
			{
				h = mWaveLinear[ j ];
			}
		}

		if( h > mWaveBands[ i ] )
		{
			mWaveBands[ i ] = mWaveBands[ i ] + ( ( h - mWaveBands[ i ] ) * ParamAttack );
		}
		else
		{
			mWaveBands[ i ] = max( mWaveBands[ i ] - ParamDecay, h );
		}
	}

	return( paNoError );
}

int Instance::bands()
{
	switch( mParams[ PARAM_BANDS ].getChoice() )
	{
		case BANDS_8:		return( 8 );
		case BANDS_16:		return( 16 );
		case BANDS_32:		return( 32 );
		case BANDS_64:		return( 64 );
		case BANDS_128:		return( 128 );
		case BANDS_256:		return( 256 );
	}

	return( 0 );
}
