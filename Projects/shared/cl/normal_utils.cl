/*
	Normal calculation functions
*/

#include "matrix.cl"


//	0 1 2
//	3 4 5
//	6 7 8

force_inline real3 CalculateNormal_v1 (real3 heights[9])
{
	real3	norm = (real3)( 0.0 );
	
	norm += normalize( cross( heights[1] - heights[4], heights[2] - heights[4] ) );
	norm += normalize( cross( heights[5] - heights[4], heights[8] - heights[4] ) );
	norm += normalize( cross( heights[7] - heights[4], heights[6] - heights[4] ) );
	norm += normalize( cross( heights[3] - heights[4], heights[0] - heights[4] ) );

	return normalize( norm );
}


force_inline real3 CalculateNormal_v2 (real3 heights[9])
{
	real3	norm = (real3)( 0.0 );

	norm += normalize( cross( heights[1] - heights[4], heights[5] - heights[4] ) );
	norm += normalize( cross( heights[5] - heights[4], heights[7] - heights[4] ) );
	norm += normalize( cross( heights[7] - heights[4], heights[3] - heights[4] ) );
	norm += normalize( cross( heights[3] - heights[4], heights[1] - heights[4] ) );

	return normalize( norm );
}


force_inline real3 CalculateNormal_v3 (real3 heights[9])
{
	real	s11 = heights[4].z;
	real	s01 = heights[3].z;
	real	s21 = heights[5].z;
	real	s10 = heights[1].z;
	real	s12 = heights[7].z;
	real3	va	= normalize( (real3)( 1.0, 0.0, s21 - s01 ) );
	real3	vb	= normalize( (real3)( 0.0, 1.0, s12 - s10 ) );
	real3	n	= cross( va, vb );
	real4	bump	= (real4)( n, s11 );

	return n;
}


force_inline real3 CalculateNormalFromHeights (real3 heights[9])
{
	return CalculateNormal_v1( heights );
}


force_inline real3 CalculateHeightMapNormal (read_only image2d_t heightMap, sampler_t samp, int2 coord, real3 inScale)
{
	real3	points[9];
	real3	scale = inScale * (real3)( 1.0 / convert_real2( get_image_dim( heightMap ) ), 1.0 );

	#pragma unroill
	for (int y = -1; y <= 1; ++y)
	{
		#pragma unroill
		for (int x = -1; x <= 1; ++x)
		{
			int i = (x + 1) + (y + 1) * 3;

			points[i] = (real3)( convert_real( x ),
								 convert_real( y ),
								 convert_real( read_imagef( heightMap, samp, coord + (int2)( x, y ) ).x ) )
							* scale;
		}
	}

	return CalculateNormalFromHeights( points );
}


force_inline real3 CalculateHeightMapSphericalNormal (read_only image2d_t heightMap, sampler_t samp, int2 coord, real3 inScale)
{
	real3	n		= CalculateHeightMapNormal( heightMap, samp, coord, inScale );
	real2	size	= convert_real2( get_image_dim( heightMap ) );
	real	alpha	= convert_real( coord.x ) / size.x;
	real2	t		= SinCos( Pi() * convert_real( coord.y ) / size.y );

	real3x3 mat;

	mat.col2 = (real3)( SinCos( Pi() * 2.0 * alpha ).yx * t.x, t.y );	// normal
	mat.col1 = (real3)( SinCos( alpha + Pi() * 0.5 ).yx, 0.0 );			// tangent
	mat.col0 = cross( mat.col2, mat.col1 );								// binormal

	return Mul_Mat3x3( n, mat );
}
