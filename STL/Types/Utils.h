// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../CompileTime/TypeListHelpers.h"

namespace GX_STL
{
namespace GXTypes
{

/*
=================================================
	AddressOf
----
	skip overloaded operator &
=================================================
*/
	template <typename T>
	inline T * AddressOf (T &tValue)
	{
		return ( (T*) &reinterpret_cast<ubyte volatile &>( tValue ) );
	}

	template <typename T>
	inline const T * const AddressOf (const T &tValue)
	{
		return ( (const T * const) &reinterpret_cast<const ubyte volatile &>( tValue ) ); 
	}
	
/*
=================================================
	OffsetOf
=================================================
*/
	template <typename A, typename B>
	inline usize OffsetOf (A (B::*member))
	{
		return (usize) &reinterpret_cast< char const volatile &>( ((B*)null)->*member );
	}

/*
=================================================
	CountOf
=================================================
*/
	template <typename T, usize I>
	constexpr
	inline usize CountOf (const T (&)[I])
	{
		return I;
	}

/*
=================================================
	EnumEqMask
=================================================
*/
	template <typename T1, typename T2, typename T3>
	inline bool EnumEqMask (const T1& left, const T2& right, const T3& mask)
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
	inline bool EnumEq (const T1& left, const T2& right)
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
	inline bool  EnumAnyEq (const T1& left, const T2& right)
	{
		typedef typename CompileTime::NearUInt::FromSize<
			CompileTime::MaxFrom< usize, sizeof(T1), sizeof(T2) >() >	T;

		return ( T(left) & T(right) ) != T(0);
	}

/*
=================================================
	UnrollLoop
=================================================
*/
	template <usize Size>
	struct UnrollLoop
	{
		template <typename T>
		inline static
		void _Iterate (T &iter)
		{
			UnrollLoop< Size-1 >::_Iterate( iter );
			iter( Size );
		}

		template <typename T>
		inline static
		void Run (T &iter)
		{
			UnrollLoop< Size-1 >::_Iterate( iter );
		}
	};


	template <>
	struct UnrollLoop <0>
	{
		template <typename T>
		inline static
		void _Iterate (T &iter)
		{
			iter( 0 );
		}
	};
	

}	// GXTypes
}	// GX_STL
