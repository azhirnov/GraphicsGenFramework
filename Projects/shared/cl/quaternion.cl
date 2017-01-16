/*
	Quaternion classes
*/

//------------------------------------------------------------------------------//
//									HEADER										//

#include "math.cl"


typedef struct _Quat
{
	real4	v;

} Quat;


Quat Quat_Create ();
Quat Quat_From (real4 xyzw);
Quat Quat_RotateX (real angle);
Quat Quat_RotateY (real angle);
Quat Quat_RotateZ (real angle);
Quat Quat_Rotate (real3 angles);
Quat Quat_FromAngleAxis (real angle, real3 axis);
Quat Quat_Mul (const Quat left, const Quat right);
Quat Quat_Normalize (const Quat q);
Quat Quat_Inverse (const Quat q);
real3 Quat_MulVec (const Quat q, const real3 vec);



//------------------------------------------------------------------------------//
//									SOURCE										//

Quat Quat_Create ()
{
	return Quat_From( (real4)( 0.0, 0.0, 0.0, 1.0 ) );
}


Quat Quat_From (real4 xyzw)
{
	Quat	q;
	q.v = xyzw;
	return q;
}


Quat Quat_RotateX (real angle)
{
	return Quat_From( (real4)( sin( angle * 0.5 ), 0.0, 0.0, cos( angle * 0.5 ) ) );
}


Quat Quat_RotateY (real angle)
{
	return Quat_From( (real4)( 0.0, sin( angle * 0.5 ), 0.0, cos( angle * 0.5 ) ) );
}


Quat Quat_RotateZ (real angle)
{
	return Quat_From( (real4)( 0.0, 0.0, sin( angle * 0.5 ), cos( angle * 0.5 ) ) );
}


Quat Quat_Rotate (real3 angles)
{
	return Quat_Mul( Quat_Mul( Quat_RotateX( angles.x ), Quat_RotateY( angles.y ) ), Quat_RotateZ( angles.z ) );
}


Quat Quat_FromAngleAxis (real angle, real3 axis)
{
	real2	sc = SinCos( angle * 0.5 );
	return Quat_From( (real4)( sc.x * axis.x, sc.x * axis.y, sc.x * axis.z, sc.y ) );
}


Quat Quat_Mul (const Quat left, const Quat right)
{
	return Quat_From( (real4)(
				(right.v.w * left.v.x) + (right.v.x * left.v.w) + (right.v.y * left.v.z) - (right.v.z * left.v.y),
				(right.v.w * left.v.y) + (right.v.y * left.v.w) + (right.v.z * left.v.x) - (right.v.x * left.v.z),
				(right.v.w * left.v.z) + (right.v.z * left.v.w) + (right.v.x * left.v.y) - (right.v.y * left.v.x),
				(right.v.w * left.v.w) - (right.v.x * left.v.x) - (right.v.y * left.v.y) - (right.v.z * left.v.z)
			));
}


Quat Quat_Normalize (const Quat q)
{
	return Quat_From( normalize( q.v ) );
}


Quat Quat_Inverse (const Quat q)
{
	return Quat_From( (real4)( -q.v.x, -q.v.y, -q.v.z, q.v.w ) );
}


real3 Quat_MulVec (const Quat q, const real3 vec)
{
	real3	uv	= cross( q.v.xyz, vec );
	real3	uuv	= cross( q.v.xyz, uv );

	uv	*= q.v.w * 2.0;
	uuv	*= 2.0;

	return vec + uv + uuv;
}
