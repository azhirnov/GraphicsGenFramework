// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.
/*
	This project used to test GLSL code in C++
*/

#pragma once

#include "Engine/STL/Engine.STL.h"


namespace GLSL
{
	typedef GX_STL::GXTypes::uint	uint;
	typedef GX_STL::GXTypes::ulong	ulong;
	typedef GX_STL::GXTypes::ilong	ilong;
	typedef GX_STL::GXMath::real	real;
	
	template <typename T, uint I>
	using Vec = GX_STL::GXMath::Vec<T,I>;

	template <typename T, uint Size>
	using Array = GX_STL::GXTypes::StaticArray<T,Size>;


	// Vec2
	typedef Vec< float,		2 >		float2;
	typedef Vec< double,	2 >		double2;
	typedef Vec< int,		2 >		int2;
	typedef Vec< ilong,		2 >		ilong2;
	typedef Vec< uint,		2 >		uint2;
	typedef Vec< ulong,		2 >		ulong2;
	typedef Vec< bool,		2 >		bool2;
	typedef Vec< real,		2 >		real2;


	// Vec3
	typedef Vec< float,		3 >		float3;
	typedef Vec< double,	3 >		double3;
	typedef Vec< int,		3 >		int3;
	typedef Vec< ilong,		3 >		ilong3;
	typedef Vec< uint,		3 >		uint3;
	typedef Vec< ulong,		3 >		ulong3;
	typedef Vec< bool,		3 >		bool3;
	typedef Vec< real,		3 >		real3;


	// Vec4
	typedef Vec< float,		4 >		float4;
	typedef Vec< double,	4 >		double4;
	typedef Vec< int,		4 >		int4;
	typedef Vec< ilong,		4 >		ilong4;
	typedef Vec< uint,		4 >		uint4;
	typedef Vec< ulong,		4 >		ulong4;
	typedef Vec< bool,		4 >		bool4;
	typedef Vec< real,		4 >		real4;


	template <typename T>	inline T					BitScanForward (T x)		{ return (T) GX_STL::GXMath::BitScanForward( x ); }
	template <typename T>	inline T					BitScanReverse (T x)		{ return (T) GX_STL::GXMath::BitScanReverse( x ); }
	template <typename T>	inline T					MinValue (const T &)		{ return GX_STL::GXTypes::MinValue<T>(); }
	template <typename T>	inline T					MaxValue (const T &)		{ return GX_STL::GXTypes::MaxValue<T>(); }
	template <typename T>	inline T					ToMask (const T& x)			{ return GX_STL::GXMath::ToMask<T>( GX_STL::GXTypes::BitsU(x) ); }
	template <typename T, int I> constexpr inline int	CountOf (const T (&)[I])	{ return I; }
	
	template <typename T, int I> inline void			CopyArray (T (&dst)[I], const T (&src)[I])	{ for (int i = 0; i < I; ++i) { dst[i] = src[i]; } }

	template <typename T, uint I> inline T				Distance (const Vec<T,I> &left, const Vec<T,I> &right)			{ return left.Distance( right ); }
	template <typename T, uint I> inline T				Length (const Vec<T,I> &value)									{ return value.Length(); }
	template <typename T, uint I> inline Vec<T,I>		Normalize (const Vec<T,I> &value)								{ return value.Normalized(); }
	template <typename T, uint I> inline Vec<bool, I>	Less (const Vec<T,I> &left, const Vec<T,I> &right)				{ return left < right; }
	template <typename T, uint I> inline Vec<bool, I>	LessEqual (const Vec<T,I> &left, const Vec<T,I> &right)			{ return left <= right; }
	template <typename T, uint I> inline Vec<bool, I>	Greater (const Vec<T,I> &left, const Vec<T,I> &right)			{ return left > right; }
	template <typename T, uint I> inline Vec<bool, I>	GreaterEqual (const Vec<T,I> &left, const Vec<T,I> &right)		{ return left >= right; }
	template <typename T, uint I> inline bool			AllLess (const Vec<T,I> &left, const Vec<T,I> &right)			{ return All( left < right ); }
	template <typename T, uint I> inline bool			AllLessEqual (const Vec<T,I> &left, const Vec<T,I> &right)		{ return All( left <= right ); }
	template <typename T, uint I> inline bool			AllGreater (const Vec<T,I> &left, const Vec<T,I> &right)		{ return All( left > right ); }
	template <typename T, uint I> inline bool			AllGreaterEqual (const Vec<T,I> &left, const Vec<T,I> &right)	{ return All( left >= right ); }
	template <typename T, uint I> inline bool			AnyLess (const Vec<T,I> &left, const Vec<T,I> &right)			{ return Any( left < right ); }
	template <typename T, uint I> inline bool			AnyLessEqual (const Vec<T,I> &left, const Vec<T,I> &right)		{ return Any( left <= right ); }
	template <typename T, uint I> inline bool			AnyGreater (const Vec<T,I> &left, const Vec<T,I> &right)		{ return Any( left > right ); }
	template <typename T, uint I> inline bool			AnyGreaterEqual (const Vec<T,I> &left, const Vec<T,I> &right)	{ return Any( left >= right ); }
	template <typename T, uint I> inline bool			AllEqual (const Vec<T,I> &left, const Vec<T,I> &right)			{ return All( left.Equal( right ) ); }
	template <typename T, uint I> inline bool			AnyEqual (const Vec<T,I> &left, const Vec<T,I> &right)			{ return Any( left.Equal( right ) ); }
	template <typename T> inline bool					Equals (const T& a, const T& b)									{ return GX_STL::GXMath::Equals( a, b ); }

	template <typename T> inline T						Abs (const T& x)												{ return GX_STL::GXMath::Abs( x ); }
	template <typename T> inline T						Max (const T& a, const T& b)									{ return GX_STL::GXMath::Max( a, b ); }
	template <typename T> inline T						Min (const T& a, const T& b)									{ return GX_STL::GXMath::Min( a, b ); }
	template <typename T0, typename T1> inline T0		Clamp (const T0& x, const T1& min, const T1& max)				{ return GX_STL::GXMath::Clamp( x, min, max ); }
	template <typename T> inline T						SafeDiv (const T& a, const T& b, const T& def)					{ return GX_STL::GXMath::SafeDiv( a, b, def ); }

	template <typename T> inline bool					IsFinite (const T& x)											{ return GX_STL::GXMath::IsFinite( x ); }

#	define imageLoad( ... )
#	define imageStore( ... )
#	define texture( ... )

#	define atomicAdd( _counter_, _value_ )		((_counter_) += (_value_))

#	define in		const
#	define out		// TODO: ref
#	define inout	// TODO: ref

#	undef  FOR
#	undef  FOR_rev

	
#	define FOR( _counter_, _array_ ) 		for (int _counter_ = 0; i < CountOf(_array_); ++i)
#	define FOR_rev( _counter_, _array_ ) 	for (int _counter_ = CountOf(_array_)-1; i >= 0; --i)


}	// GLSL

using namespace GLSL;
