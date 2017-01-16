/*
	Distance functions.
	Signed distance field.

	from http://iquilezles.org/www/articles/distfunctions/distfunctions.htm
*/

#include "math.cl"
#include "quaternion.cl"
#include "line.cl"


// Shapes

real DistField_Sphere (real3 position, real radius)
{
	return length( position ) - radius;
}


real DistField_Ellipsoid (real3 position, real3 radius)
{
    return ( length( position/radius ) - 1.0 ) * min( min( radius.x, radius.y ), radius.z );
}


real DistField_Box (real3 position, real3 halfSize)
{
	real3	d = Abs( position ) - halfSize;
	return min( max( d.x, max( d.y, d.z ) ), 0.0 ) + length( max( d, 0.0 ) );
}


real DistField_Torus (real3 position, real2 firstAndSecondRadius)
{
	real2	q = (real2)( length( position.xz ) - firstAndSecondRadius.x, position.y );

	return length( q ) - firstAndSecondRadius.y;
}


real DistField_Cylinder (real3 position, real2 radiusHeight)
{
	real2	d = Abs( (real2)( length( position.xz ), position.y ) ) - radiusHeight;

	return min( max( d.x, d.y ), 0.0 ) + length( max( d, 0.0 ) );
}


real DistField_Cone (real3 position, real2 radiusHeight)
{
	real3	up   = (real3)( 0.0, radiusHeight.y, 0.0 );
	real3	down = -up;

	down.xz = normalize( position.xz ) * radiusHeight.x;

	Line3	line;
	Line3_Create( &line, up, down );

	return Line3_MinDistance( &line, position );

	//real2	d = Abs( (real2)( length( position.xz ), position.y ) );// - radiusHeight;

	//d.x -= radiusHeight.x / 

	//return min( max( d.x, d.y ), 0.0 ) + length( max( d, 0.0 ) );

	//return dot( normalize( cone ), (real2)( length( position.xy ), position.z ) );
}


real DistField_Plane (real3 position, real4 n)
{
	// n must be normalized
	return dot( position, n.xyz ) + n.w;
}


real DistField_HexagonalPrism (real3 position, real2 h)
{
	real3	q = Abs( position );

	return max( q.z - h.y, max( q.x * 0.866025 + q.y * 0.5, q.y ) - h.x );
}


real DistField_TriangularPrism (real3 position, real2 h)
{
    real3	q = Abs( position );

    return max( q.z - h.y, max( q.x * 0.866025 + position.y * 0.5, -position.y ) - h.x * 0.5 );
}


real DistField_Capsule (real3 position, real3 a, real3 b, real r)
{
	real3	pa = position - a;
	real3	ba = position - a;
	real	h  = clamp( dot( pa, ba ) / dot( ba, ba ), 0.0, 1.0 );

	return length( pa - ba * h ) - r;
}


// Unions

real DistField_OpUnite (real d1, real d2)
{
	return min( d1, d2 );
}


real DistField_OpSub (real d1, real d2)
{
	return max( d1, -d2 );
}


real DistField_OpIntersect (real d1, real d2)
{
	return max( d1, d2 );
}


// Transformation

real3 DistField_Move (real3 position, real3 move)
{
	return position - move;
}


real3 DistField_Rotate (real3 position, Quat q)
{
	return Quat_MulVec( Quat_Inverse( q ), position );
}


real3 DistField_Transform (real3 position, Quat q, real3 move)
{
	return DistField_Rotate( DistField_Move( position, move ), q );
}

/*
real3 DistField_Transform (real3 position, real4x4 mat)
{
	return Mat4x4_MulVec( Mat4x4_Inverse( mat ), position );
}
*/