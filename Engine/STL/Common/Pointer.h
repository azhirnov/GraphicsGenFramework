// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Types.h"
#include "AllFunc.h"
#include "Engine/STL/CompileTime/TypeTraits.h"
#include "Engine/STL/CompileTime/TemplateMath.h"

namespace GX_STL
{
namespace GXTypes
{
	
/*
=================================================
	PointerCast
=================================================
*/
	namespace _types_hidden_
	{
		template <typename T, typename B>
		struct _PointerCast
		{};
		
		template <typename T, typename B>
		struct _PointerCast< T *, B >
		{
			typedef B *	result_t;

			forceinline static result_t Cast (T * val) {
				return static_cast< result_t >( static_cast< void * >( val ) );
			}
		};
		
		template <typename T, typename B>
		struct _PointerCast < const T *, B >
		{
			typedef const B *	result_t;

			forceinline static result_t Cast (const T * val) {
				return static_cast< result_t >( static_cast< const void * const >( val ) );
			}
		};
	}

	template <typename T, typename B>
	forceinline typename _types_hidden_::_PointerCast< B, T >::result_t  PointerCast (B val)
	{
		STATIC_ASSERT( TypeTraits::IsPointer<B>, "argument must be pointer" );
		return _types_hidden_::_PointerCast< B, T >::Cast( val );
	}

/*
=================================================
	ReferenceCast
=================================================
*/
	template <typename R, typename T>
	forceinline const R & ReferenceCast (const T & t)
	{
		STATIC_ASSERT( sizeof(R) == sizeof(T), "type size mismatch" );
		return reinterpret_cast<const R &>( t );
	}
	
	template <typename R, typename T>
	forceinline R & ReferenceCast (T & t)
	{
		STATIC_ASSERT( sizeof(R) == sizeof(T), "type size mismatch" );
		return reinterpret_cast<R &>( t );
	}
	
/*
=================================================
	IsAlignedPointer
=================================================
*/
	template <usize AlignInBytes>
	forceinline bool IsAlignedPointer (void const * const ptr)
	{
		STATIC_ASSERT( (CompileTime::IsPowerOfTwo< usize, AlignInBytes >), "Align must be power of 2" );
		return (ReferenceCast<usize>( ptr ) & (AlignInBytes-1)) == 0;
	}


}	// GXTypes
}	// GX_STL
