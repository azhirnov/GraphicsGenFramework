// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Mathematics.h"
#include "Vec.h"
#include "Vec.h"

namespace GX_STL
{
namespace GXMath
{
	
	void InitializeSTLMath ();

	
/*
=================================================
	Align
----
	align to largest value
=================================================
*/
	template <uint A, typename T>
	inline T Align (const T& value)
	{
		STATIC_ASSERT( (CompileTime::IsPowerOfTwo<uint, A>()), "align must be power of two!" );

		const uint	ALIGN_POT	= CompileTime::IntLog2<uint, A>;
		const uint	ALIGN_MASK	= (1 << ALIGN_POT) - 1;

		return ((value + ALIGN_MASK) & ~ALIGN_MASK);
	}

	template <uint A, typename T, usize I>
	inline Vec<T,I> Align (const Vec<T,I> &v)
	{
		Vec<T,I> res;
		FOR( i, res )	res[i] = Align<A>( v[i] );
		return res;
	}

	template <typename T>
	inline T Align (const T& value, const usize align)
	{
		return T( ((value + (align-1)) / align) * align );
	}

	template <typename T, usize I>
	inline Vec<T,I> Align (const Vec<T,I>& value, const usize align)
	{
		Vec<T,I> res;
		FOR( i, res )	res[i] = Align( value[i], align );
		return res;
	}
	
/*
=================================================
	ResCorrectionAspect
----
	resolution correction for normalized coords
=================================================
*/
	template <typename T>
	inline Vec<T,2> ResCorrectionAspect (const Vec<T,2> &dim)
	{
		// proportional_coords = normalized_coords * aspect
		return	dim.x < dim.y ?
				Vec<T,2>( T(1), dim.x / dim.y ) :
				Vec<T,2>( dim.y / dim.x, T(1) );
	}


}	// GXMath
}	// GX_STL
