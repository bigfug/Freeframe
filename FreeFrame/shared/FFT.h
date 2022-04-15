#pragma once

#include <complex>
#include <cmath>

#if !defined( M_PI )
#define M_PI       3.14159265358979323846
#endif

using namespace std;

template<typename T=double> class DanielsonLanczos
{
public:
	void real2complex( const T *in, complex<T> *out, const signed N )
	{
		out[ 0 ] = complex<T>( 0.0, 0.0 );

		for( signed n = 0 ; n < N ; n++ )
		{
			out[ 0 ] += complex<T>( in[ n ], 0.0 );
		}

		for( signed m = 1 ; m < N / 2 ; m++ )
		{
			out[ m ] = complex<T>( 0.0, 0.0 );

			for( signed n = 0 ; n < N ; n++ )
			{
				out[ m ] += complex<T>( in[ n ] * cos( T(2) * T(M_PI) * n * m / N ), -in[ n ] * sin( T(2) * T(M_PI) * n * m / N ) );
			}
		}
	}
};
