// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "MathTypes3D.h"

namespace GX_STL
{
namespace GXMath
{

	// Cartesian -> Spherical
	template <typename T>
	inline Vec<T,3>  CartesianToSpherical(const Vec<T,3> &cartesian)
	{
		// x - radius
		// y - polar angle Theta
		// z - azimuthal angle Phi
		
		CompileTime::MustBeFloat<T>();

		Vec<T,3>	spherical;

		spherical.x = cartesian.Length();
		ASSERT( IsNotZero( spherical.x ) );

		spherical.y = ACos( cartesian.z / spherical.x );
		spherical.z = ATan( cartesian.y, spherical.x );

		return spherical;
	}


	// Spherical -> Cartesian
	template <typename T>
	inline Vec<T,3>  SphericalToCartesian(const Vec<T,3> &spherical)
	{
		// x - radius
		// y - polar angle Theta
		// z - azimuthal angle Phi
		
		CompileTime::MustBeFloat<T>();

		Vec<T,3>		cartesian;
		Vec<T,2> const	theta = SinCos( spherical.y );
		Vec<T,2> const	phi   = SinCos( spherical.z );

		cartesian.x = spherical.x * theta.x * phi.y;
		cartesian.y = spherical.x * theta.x * phi.x;
		cartesian.z = spherical.x * theta.y;

		return cartesian;
	}


	// Spherical -> Cilindrical
	template <typename T>
	inline Vec<T,3>  SphericalToCilindrical(const Vec<T,3> &spherical)
	{
		// spherical:					cilindrical:
		// x - radius					x - radius
		// y - polar angle Theta		y - azimuthal angle Phi
		// z - azimuthal angle Phi		z - elevation
		
		CompileTime::MustBeFloat<T>();

		Vec<T,3>		cilindrical;
		Vec<T,2> const	theta = SinCos( spherical.y );

		cilindrical.x = spherical.x * theta.x;
		cilindrical.y = spherical.z;
		cilindrical.z = spherical.x * theta.y;

		return cilindrical;
	}


	// Cilindrical -> Spherical
	template <typename T>
	inline Vec<T,3>  CilindricalToSpherical(const Vec<T,3> &cilindrical)
	{
		// spherical:					cilindrical:
		// x - radius					x - radius
		// y - polar angle Theta		y - azimuthal angle Phi
		// z - azimuthal angle Phi		z - elevation
		
		CompileTime::MustBeFloat<T>();

		Vec<T,3>		spherical;

		spherical.x = cilindrical.xz().Length();
		spherical.y = ATan( cilindrical.x, cilindrical.y );
		spherical.z = cilindrical.y;

		return spherical;
	}


	// Cartesian -> Cilindrical
	template <typename T>
	inline Vec<T,3>  CartesianToCilindrical(const Vec<T,3> &cartesian)
	{
		// x - radius
		// y - azimuthal angle Phi
		// z - elevation
		
		CompileTime::MustBeFloat<T>();

		Vec<T,3>	cilindrical;

		cilindrical.x = cartesian.xy().Length();
		cilindrical.y = ATan( cartesian.y / cartesian.x );
		cilindrical.z = cartesian.z;

		return cilindrical;
	}


	// Cilindrical -> Cartesian
	template <typename T>
	inline Vec<T,3>  CilindricalToCartesian(const Vec<T,3> &cilindrical)
	{
		// x - radius
		// y - azimuthal angle Phi
		// z - elevation
		
		CompileTime::MustBeFloat<T>();

		Vec<T,3>		cartesian;
		Vec<T,2> const	phi = SinCos( cilindrical.y );

		cartesian.x = cilindrical.x * phi.y;
		cartesian.y = cilindrical.x * phi.x;
		cartesian.z = cilindrical.z;

		return cartesian;
	}


}	// GXMath
}	// GX_STL
