/*
	Fractal functions
*/

#include "base.cl"


// period: 13.0
real JuliaFractal (real2 coord, real time, int maxIterations)
{
	real	dmin = 1000.0;
	real2	cc	 = (real2)( cos(0.5*time), sin(0.5*time*1.423) );	
	real2	z	 = (-1.0 + 2.0 * coord) * 0.7;
	
	for (int i = 0; i < maxIterations; ++i)
	{
		z = cc + (real2)( z.x*z.x - z.y*z.y, 2.0*z.x*z.y );
		real	m2 = dot( z, z );
		
		if ( m2 > 100000.0 )
			break;
		
		dmin = min( dmin, m2 );
	}
	
	return sqrt( sqrt( dmin ) ) * 0.7;
}


real JuliaFractal2 (real2 coord, real time)
{
	const int maxIterations = 300;

	const real2	cc = (real2)( -0.7, 0.27015 );

	real2	z = coord * (real2)( 1.5, 1.0 );
	int		i;

	#pragma unroll
	for (i = 0; i < maxIterations; ++i)
	{
		z = cc + (real2)( z.x*z.x - z.y*z.y, 2.0*z.x*z.y );

		if ( dot(z, z) > 4.0 )
			break;
	}

	return convert_real(i) / convert_real(maxIterations);
}


real MandelbrotFractal (real2 coord, real time)
{
	const int maxIterations = 300;
	
	real	dmin = 1000.0;
	real2	cc	= coord * (real2)( 1.5, 1.0 );
	real2	z	= (real2)( 0.0 );
	int		i;

	for (i = 0; i < maxIterations; ++i)
	{
		z = cc + (real2)( z.x*z.x - z.y*z.y, 2.0*z.x*z.y );

		if ( dot( z, z ) > 4.0 )
			break;
	}

	return convert_real(i) / convert_real(maxIterations);
}
