// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Types.h"
#include "AllFunc.h"
#include "../CompileTime/TypeTraits.h"

namespace GX_STL
{
namespace GXTypes
{

	//
	// Reinterpret Cast for pointers
	//

	namespace _types_hidden_
	{
		template <typename T, typename B>
		struct _PointerCast
		{
		};
		
		template <typename T, typename B>
		struct _PointerCast< T *, B >
		{
			typedef B *	result_t;

			static result_t Cast (T * val) {
				return static_cast< result_t >( static_cast< void * >( val ) );
			}
		};
		
		template <typename T, typename B>
		struct _PointerCast < const T *, B >
		{
			typedef const B *	result_t;

			static result_t Cast (const T * val) {
				return static_cast< result_t >( static_cast< const void * const >( val ) );
			}
		};
	}

	template <typename T, typename B>
	inline typename _types_hidden_::_PointerCast< B, T >::result_t  PointerCast (B val)
	{
		STATIC_ASSERT( TypeTraits::IsPointer<B>, "argument must be pointer" );
		return _types_hidden_::_PointerCast< B, T >::Cast( val );
	}


	//
	// Reference Cast
	//

	template <typename R, typename T>
	inline const R & ReferenceCast (const T & t)
	{
		STATIC_ASSERT( sizeof(R) == sizeof(T), "type size mismatch" );
		return reinterpret_cast<const R &>( t );
	}
	
	template <typename R, typename T>
	inline R & ReferenceCast (T & t)
	{
		STATIC_ASSERT( sizeof(R) == sizeof(T), "type size mismatch" );
		return reinterpret_cast<R &>( t );
	}


	/*
	//
	// Limit Clamp Cast
	//

	template <typename R, typename T>
	inline R LimitClampCast (const T &t)
	{
		const R	r = R(t);

		//if ( T(r) == t )
		//	return r;

		return (R) GXMath::Clamp( t, MinValue<R>(), MaxValue<R>() );
		/*if ( t < MinValue<R>() )	return MinValue<R>();
		if ( t > MaxValue<R>() )	return MaxValue<R>();

		return (R)t;* /
	}



	//
	// Limit Chack Cast
	//

	template <typename R, typename T>
	inline R LimitCheckCast (const T &t)
	{
		ASSERT( t < MinValue<R>() );
		ASSERT( t > MaxValue<R>() );
		return R(t);
	}*/


}	// GXTypes
}	// GX_STL
