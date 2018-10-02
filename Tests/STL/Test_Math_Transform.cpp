// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Engine/STL/Engine.STL.h"

using namespace GX_STL;
using namespace GX_STL::GXTypes;
using namespace GX_STL::GXMath;

typedef Transformation< real >		transform_t;


static void Transform_Inverse ()
{
	const transform_t	a(	real3( 1.0f, 2.0f, 3.0f ), 
							quat().Rotation( real3( 45.0f, 0.0f, 10.0f ).To< DegreesVec<real,3> >().To< RadiansVec<real,3> >() ),
							2.0f );

	const transform_t	b = a.Inversed().Inversed();

	ASSERT( a == b );
}


static void Transform_AddInverse ()
{
	const transform_t	a(	real3( 1.0f, 2.0f, 3.0f ),
							quat().Rotation( real3( 45.0f, 0.0f, 10.0f ).To< DegreesVec<real,3> >().To< RadiansVec<real,3> >() ),
							2.0f );

	const transform_t	b = a + a.Inversed();

	ASSERT( b.IsZero() );
}


static void Transform_MulVec ()
{
	const transform_t	tr(	real3( 1.0f, 2.0f, 3.0f ),
							quat().Rotation( real3( 45.0f, 0.0f, 10.0f ).To< DegreesVec<real,3> >().To< RadiansVec<real,3> >() ),
							2.0f );

	transform_t::mat_t	mat;	tr.GetMatrix( OUT mat );

	real3 const			point0( -1.0f, -1.0f, -1.0f );
	real3 const			point1( 1.0f, 1.0f, 1.0f );

	real3	mat_point0	= (mat * real4( point0, 1.0f )).xyz();
	real3	mat_point1	= (mat * real4( point1, 1.0f )).xyz();

	real3	tr_point0	= tr.Transform( point0 );
	real3	tr_point1	= tr.Transform( point1 );

	ASSERT( All( mat_point0 == tr_point0 ) );
	ASSERT( All( mat_point1 == tr_point1 ) );
}


static void Transform_MulVec2 ()
{
	const quat			rotation = quat().RotationX( -90.0_deg.ToRadians() );
	
	const transform_t	tr( real3(-32.0f, -32.0f, 0.0f), rotation, 32.0f );
	
	transform_t::mat_t	mat;	tr.GetMatrix( OUT mat );
	
	real3 const			point0( 1.0f, 0.0f, 0.0f );
	real3 const			point1( 0.0f, 1.0f, 0.0f );
	real3 const			point2( 0.0f, 0.0f, 1.0f );
	real3 const			point3( 0.0f, 0.0f, 0.0f );
	real3 const			point4( 1.0f, 1.0f, 1.0f );
	
	real3	mat_point0	= (mat * real4( point0, 1.0f )).xyz();
	real3	mat_point1	= (mat * real4( point1, 1.0f )).xyz();
	real3	mat_point2	= (mat * real4( point2, 1.0f )).xyz();
	real3	mat_point3	= (mat * real4( point3, 1.0f )).xyz();
	real3	mat_point4	= (mat * real4( point4, 1.0f )).xyz();

	real3	tr_point0	= tr.Transform( point0 );
	real3	tr_point1	= tr.Transform( point1 );
	real3	tr_point2	= tr.Transform( point2 );
	real3	tr_point3	= tr.Transform( point3 );
	real3	tr_point4	= tr.Transform( point4 );

	ASSERT( All( mat_point0 == tr_point0 ) );
	ASSERT( All( mat_point1 == tr_point1 ) );
	ASSERT( All( mat_point2 == tr_point2 ) );
	ASSERT( All( mat_point3 == tr_point3 ) );
	ASSERT( All( mat_point4 == tr_point4 ) );
}


extern void Test_Math_Transform ()
{
	Transform_Inverse();
	Transform_AddInverse();
	Transform_MulVec();
	Transform_MulVec2();
}
