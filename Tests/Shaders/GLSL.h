// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.
/*
	This project used to test GLSL code in C++
*/

#pragma once

#include "Engine/STL/Engine.STL.h"


namespace GX_STL
{
namespace GXMath
{

	template <typename T, usize I>
	inline T  Distance (const Vec<T,I> &left, const Vec<T,I> &right)
	{
		return left.Distance( right );
	}


	template <typename T, usize I>
	inline T  Length (const Vec<T,I> &value)
	{
		return value.Length();
	}


	template <typename T, usize I>
	inline Vec<T,I>  Normalize (const Vec<T,I> &value)
	{
		return value.Normalized();
	}


	template <typename T, usize I>
	inline Vec<bool, I>  Less (const Vec<T,I> &left, const Vec<T,I> &right)
	{
		return left < right;
	}
	
	template <typename T, usize I>
	inline Vec<bool, I>  LessEqual (const Vec<T,I> &left, const Vec<T,I> &right)
	{
		return left <= right;
	}

	template <typename T, usize I>
	inline Vec<bool, I>  Greater (const Vec<T,I> &left, const Vec<T,I> &right)
	{
		return left > right;
	}

	template <typename T, usize I>
	inline Vec<bool, I>  GreaterEqual (const Vec<T,I> &left, const Vec<T,I> &right)
	{
		return left >= right;
	}


	template <typename T, usize I>
	inline bool  AllLess (const Vec<T,I> &left, const Vec<T,I> &right)
	{
		return All( left < right );
	}
	
	template <typename T, usize I>
	inline bool  AllLessEqual (const Vec<T,I> &left, const Vec<T,I> &right)
	{
		return All( left <= right );
	}

	template <typename T, usize I>
	inline bool  AllGreater (const Vec<T,I> &left, const Vec<T,I> &right)
	{
		return All( left > right );
	}

	template <typename T, usize I>
	inline bool  AllGreaterEqual (const Vec<T,I> &left, const Vec<T,I> &right)
	{
		return All( left >= right );
	}


	template <typename T, usize I>
	inline bool  AnyLess (const Vec<T,I> &left, const Vec<T,I> &right)
	{
		return Any( left < right );
	}
	
	template <typename T, usize I>
	inline bool  AnyLessEqual (const Vec<T,I> &left, const Vec<T,I> &right)
	{
		return Any( left <= right );
	}

	template <typename T, usize I>
	inline bool  AnyGreater (const Vec<T,I> &left, const Vec<T,I> &right)
	{
		return Any( left > right );
	}

	template <typename T, usize I>
	inline bool  AnyGreaterEqual (const Vec<T,I> &left, const Vec<T,I> &right)
	{
		return Any( left >= right );
	}


	template <typename T, usize I>
	inline bool  AllEqual (const Vec<T,I> &left, const Vec<T,I> &right)
	{
		return All( left.Equal( right ) );
	}

	template <typename T, usize I>
	inline bool  AnyEqual (const Vec<T,I> &left, const Vec<T,I> &right)
	{
		return Any( left.Equal( right ) );
	}



#	define imageLoad( ... )
#	define imageStore( ... )
#	define texture( ... )

#	define in		const
#	define out		// TODO: ref
#	define inout	// TODO: ref

}	// GXMath
}	// GX_STL

using namespace GX_STL;
using namespace GX_STL::GXTypes;
using namespace GX_STL::GXMath;
