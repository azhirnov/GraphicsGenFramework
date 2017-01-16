/*
	Math functions.
*/

//------------------------------------------------------------------------------//
//									HEADER										//

#include "base.cl"


real Pi ();

real2 SinCos (real value);

#define IntLog2( _x_ )	clz( _x_ )

#define Abs( _x_ )		fabs( _x_ )

real3 SphericalToCartesian (real2 spherical, real radius);

real  Fract (real x);
real2 Fract2 (real2 x);
real3 Fract3 (real3 x);
real4 Fract4 (real4 x);

real Cross2 (real2 a, real2 b);

real Epsilon ();

bool IsZero (real x);
bool IsZero2 (real2 x);
bool IsZero3 (real3 x);
bool IsZero4 (real4 x);

bool Equals (real a, real b);
bool Equals2 (real2 a, real2 b);
bool Equals3 (real3 a, real3 b);
bool Equals4 (real4 a, real4 b);

// 0 - minus, 1 - plus
real2 SignMask ();

int QuadraticEquation (real a, real b, real c, OUT real result[2]);



//------------------------------------------------------------------------------//
//									SOURCE										//

/*
=================================================
	Pi
=================================================
*/
force_inline real Pi ()
{
	return (real)( 3.14159265358979323846 );
}

/*
=================================================
	SinCos
=================================================
*/
force_inline real2 SinCos (real value)
{
	real	x, y;
	x = sincos( value, (private real *) &y );
	return (real2)( x, y );
}

/*
=================================================
	SphericalToCartesian
=================================================
*/
force_inline real3 SphericalToCartesian (real2 spherical, real radius)
{
	real2	a = SinCos( spherical.x );
	real2	b = SinCos( spherical.y );
	return (real3)(	radius * a.s0 * b.s1,
					radius * a.s0 * b.s0,
					radius * a.s1 );
}

/*
=================================================
	Fract
=================================================
*/
force_inline real Fract (real x)
{
	real itpr;
	return fract( x, &itpr );
}

force_inline real2 Fract2 (real2 x)
{
	real2 itpr;
	return fract( x, &itpr );
}

force_inline real3 Fract3 (real3 x)
{
	real3 itpr;
	return fract( x, &itpr );
}

force_inline real4 Fract4 (real4 x)
{
	real4 itpr;
	return fract( x, &itpr );
}

/*
=================================================
	Cross2
=================================================
*/
force_inline real Cross2 (real2 a, real2 b)
{
	return a.x * b.y - a.y * b.x;
}

/*
=================================================
	Epsilon
=================================================
*/
force_inline real Epsilon ()
{
	#if COMPUTE_FP == 64
		const real	epsilon = (real)( 1.0e-8 );
	#else
		const real	epsilon = (real)( 1.0e-3 );
	#endif

	return epsilon;
}

/*
=================================================
	IsZero
=================================================
*/
force_inline bool IsZero (real x)
{
	return isless( fabs(x), Epsilon() );
}

force_inline bool IsZero2 (real2 x)
{
	return all( isless( fabs(x), (real2)( Epsilon() ) ) );
}

force_inline bool IsZero3 (real3 x)
{
	return all( isless( fabs(x), (real3)( Epsilon() ) ) );
}

force_inline bool IsZero4 (real4 x)
{
	return all( isless( fabs(x), (real4)( Epsilon() ) ) );
}

/*
=================================================
	Equals
=================================================
*/
force_inline bool Equals (real a, real b)
{
	return IsZero( a - b );
}

force_inline bool Equals2 (real2 a, real2 b)
{
	return IsZero2( a - b );
}

force_inline bool Equals3 (real3 a, real3 b)
{
	return IsZero3( a - b );
}

force_inline bool Equals4 (real4 a, real4 b)
{
	return IsZero4( a - b );
}

/*
=================================================
	Equals
=================================================
*/
real2 SignMask ()
{
	return (real2)( -1.0, 1.0 );
}

/*
=================================================
	QuadraticEquation
=================================================
*/
int QuadraticEquation (real a, real b, real c, OUT real result[2])
{
	real	disc = b * b - 4.0 * a * c;
			
	if ( disc < 0.0 )
		return 0;

	real	bb_4ac_2a	= sqrt( disc ) / ( 2.0 * a );
	real	b_2a		= -b / ( 2.0 * a );

	result[0] = ( b_2a + bb_4ac_2a );
	result[1] = ( b_2a - bb_4ac_2a );

	return IsZero( disc ) ? 1 : 2;
}
