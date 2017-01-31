// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Trigonometry.h"

namespace GX_STL
{
namespace GXMath
{


	//
	// Lerp, InvLerp, BiLerp
	//
	
	// inverse linear interpolation
	template <typename T>
	inline T InvLerp (const T& x, const T& y, const T& interpolated)
	{
		return (interpolated - x) / (y - x);
	}


	// linear interpolation
	template <typename T, typename B>
	inline T Lerp (const T& x, const T& y, const B& factor)
	{
		return T(factor) * (y - x) + x;
	}


	// bilinear interpolation
	template <typename T, typename B>
	inline T BiLerp (const T& v0, const T& v1, const T& v2, const T& v3, const B& factorX, const B& factorY)
	{
		return Lerp( Lerp( v0, v3, factorX ), Lerp( v1, v2, factorX ), factorY );
	}

	template <typename T, typename B>
	inline T BiLerp (const T& v0, const T& v1, const T& v2, const T& v3, const Vec<B,2> &factor)
	{
		return BiLerp( v0, v1, v2, v3, factor.x, factor.y );
	}
	

	// quadratic interpolation
	/*template <typename T, typename B>
	inline T QMix (const T& x, const T& y, const T& z, const B& a)
	{
		const B		inv  = B(1) - a;
		const B		mul0 = inv * inv;
		const B		mul1 = B(2) * a * inv;

		return (T)( x * mul0 + y * mul1 + z * Square( a ) );
	}*/

	
	// trilinear interpolation
	/*template <typename T, typename B>
	inline T TriMix (const T &v0, const T &v1, const T &v2, const T &v3, const T &v4,
					 const T &v5, const T &v6, const T &v7, const B& a)
	{
		return Mix( Mix( Mix( v0, v2, a[0] ), Mix( v1, v3, a[0] ), a[1] ),
					Mix( Mix( v4, v6, a[0] ), Mix( v5, v7, a[0] ), a[1] ),
					a[2] );
	}
	  

	// cubic interpolation
	template <typename T>
	inline T CubicMix (const T& v0, const T& v1, const T& v2, const T& v3, const T& a)
	{
		return	((v3 - v2) - (v0 - v1)) * a * a * a +
				(v3 - v2) * a * a +
				(v2 - v0) * a +
				v1;
	}*/

}	// GXMath
}	// GX_STL
