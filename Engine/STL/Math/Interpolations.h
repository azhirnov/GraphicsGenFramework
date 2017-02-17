// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Trigonometry.h"

namespace GX_STL
{
namespace GXMath
{

/*
=================================================
	InvLerp
----
	inverse linear interpolation
=================================================
*/
	template <typename T>
	inline T InvLerp (const T& x, const T& y, const T& interpolated)
	{
		return (interpolated - x) / (y - x);
	}
	
/*
=================================================
	Lerp
----
	linear interpolation
=================================================
*/
	template <typename T, typename B>
	inline T Lerp (const T& x, const T& y, const B& factor)
	{
		return T(factor) * (y - x) + x;
	}

/*
=================================================
	BiLerp
----
	bilinear interpolation
=================================================
*/
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
	

}	// GXMath
}	// GX_STL
