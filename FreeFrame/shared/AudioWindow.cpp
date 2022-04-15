
#include <cmath>
#include <cstring>

#include "AudioWindow.h"

AudioWindow::AudioWindow( const unsigned int pFunction, const unsigned int pNumSamples )
: mNumSamples( pNumSamples ), mFunction( pFunction )
{
	if( pFunction >= this->NumWindowFuncs() )
	{
	}
}

AudioWindow::~AudioWindow(void)
{
}

inline unsigned int AudioWindow::NumWindowFuncs()
{
   return( 4 );
}

const char *AudioWindow::WindowFuncName( const unsigned int whichFunction )
{
	switch (whichFunction)
	{
		case 1:
			return "Bartlett";
		case 2:
			return "Hamming";
		case 3:
			return "Hanning";
	}

	return "Rectangular";
}

void AudioWindow::WindowFunc( float *in )
{
	const	float				fNumDivTwo = (float)this->mNumSamples / 2.0f;
	const	int					iNumDivTwo = int( fNumDivTwo );
	const	int					NumSubOne  = this->mNumSamples - 1;
	unsigned int i;

	switch( this->mFunction )
	{
		case 1:
			// Bartlett (triangular) window
			for( i = 0; i < iNumDivTwo ; i++ )
			{
				in[ i ] *= ( i / fNumDivTwo );
				in[ i + iNumDivTwo ] *= ( 1.0f - float( i / NumSubOne ) );
			}
			break;

		case 2:
			// Hamming
			for( i = 0; i < this->mNumSamples ; i++ )
			{
				in[ i ] *= 0.54f - 0.46f * cosf( 2.0f * float( M_PI ) * float( i / NumSubOne ) );
			}
			break;

		case 3:
			// Hanning
			for( i = 0; i < this->mNumSamples; i++ )
			{
				in[ i ] *= 0.50f - 0.50f * cosf( 2.0f * float( M_PI ) * float( i / NumSubOne ) );
			}
			break;
	}
}

void AudioWindow::ProcessWindow( const float *pInput, float *pOutput )
{
	const	unsigned	int		BufferSize = sizeof(float) * this->mNumSamples;

	memcpy( pOutput, pInput, BufferSize );

	this->WindowFunc( pOutput );
}

