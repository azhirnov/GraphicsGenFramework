// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/STL/Common/AllFunc.h"

namespace GX_STL
{
namespace GXTypes
{
	
/*
=================================================
	EnumEqMask
=================================================
*/
	template <typename T1, typename T2, typename T3>
	forceinline constexpr bool EnumEqMask (const T1& left, const T2& right, const T3& mask)
	{
		typedef typename CompileTime::NearUInt::FromSize<
			CompileTime::MaxFrom< usize, sizeof(T1), sizeof(T2), sizeof(T3) >() >	T;

		return ( T(left) & T(mask) ) == ( T(right) & T(mask) );
	}
	
/*
=================================================
	EnumEq
=================================================
*/
	template <typename T1, typename T2>
	forceinline constexpr bool EnumEq (const T1& left, const T2& right)
	{
		typedef typename CompileTime::NearUInt::FromSize<
			CompileTime::Max< usize, sizeof(T1), sizeof(T2) > >		T;

		return ( T(left) & T(right) ) == T(right);
	}
	
/*
=================================================
	EnumAnyEq
=================================================
*/
	template <typename T1, typename T2>
	forceinline constexpr bool  EnumAnyEq (const T1& left, const T2& right)
	{
		typedef typename CompileTime::NearUInt::FromSize<
			CompileTime::MaxFrom< usize, sizeof(T1), sizeof(T2) >() >	T;

		return ( T(left) & T(right) ) != T(0);
	}


}	// GXTypes
}	// GX_STL
