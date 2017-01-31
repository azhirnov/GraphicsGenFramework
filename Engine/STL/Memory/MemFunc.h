// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/STL/Common/Types.h"
#include "Engine/STL/CompileTime/NewTypeInfo.h"
#include "Engine/STL/Dimensions/ByteAndBit.h"

namespace GX_STL
{
namespace GXTypes
{

	template <typename T>
	struct Buffer;

/*
=================================================
	CheckPointersAliasing
----
	Returns true if memory regions are intersected
=================================================
*/
	template <typename T0, typename T1>
	forceinline bool CheckPointersAliasing (const T0 *begin0, const T0 *end0, const T1 *begin1, const T1 *end1)
	{
		ASSERT( begin0 <= end0 );
		ASSERT( begin1 <= end1 );
		return ( begin0 < end1 ) & ( end0 > begin1 );
	}


	namespace UnsafeMem
	{
/*
=================================================
	MemCopy
----
	memory blocks must not intersects
=================================================
*/
		forceinline void * MemCopy (void *dst, const void *src, BytesU size)
		{
			// TODO: add checks
			ASSERT( not CheckPointersAliasing( (ubyte *)dst, (ubyte *)dst + usize(size),
											   (const ubyte *)src, (const ubyte *)src + usize(size) ) );

			return ::memcpy( dst, src, usize(size) );
		}
		
/*
=================================================
	MemMove
----
	memory blocks may intersects
=================================================
*/
		forceinline void * MemMove (void *dst, const void *src, BytesU size)
		{
			// TODO: add checks
			return ::memmove( dst, src, usize(size) );
		}

/*
=================================================
	MemCmp
=================================================
*/
		forceinline int MemCmp (const void *left, const void *right, BytesU size)
		{
			// TODO: add checks
			return ::memcmp( left, right, usize(size) );
		}

/*
=================================================
	ZeroMem
=================================================
*/
		forceinline void ZeroMem (void *dst, BytesU size)
		{
			::memset( dst, 0, usize(size) );
		}

	}	// UnsafeMem
	

/*
=================================================
	MemCopy
----
	memory blocks must not intersects
=================================================
*/
	template <typename T0, typename T1>
	inline void MemCopy (T0 &dst, const T1 &src)
	{
		STATIC_ASSERT( CompileTime::IsPOD<T0> );
		STATIC_ASSERT( CompileTime::IsPOD<T1> );
		STATIC_ASSERT( sizeof(dst) == sizeof(src) );

		UnsafeMem::MemCopy( &dst, &src, sizeof(dst) );
	}
	
	template <typename T0, typename T1>
	inline void MemCopy (Buffer<T0> dst, Buffer<const T1> src);
	
/*
=================================================
	MemMove
----
	memory blocks may intersects
=================================================
*/
	template <typename T0, typename T1>
	inline void MemMove (T0 &dst, const T1 &src)
	{
		STATIC_ASSERT( CompileTime::IsPOD<T0> );
		STATIC_ASSERT( CompileTime::IsPOD<T1> );
		STATIC_ASSERT( sizeof(dst) == sizeof(src) );

		UnsafeMem::MemMove( &dst, &src, sizeof(dst) );
	}

	template <typename T0, typename T1>
	inline void MemMove (Buffer<T0> dst, Buffer<const T1> src);
	
/*
=================================================
	MemCmp
=================================================
*/
	template <typename T0, typename T1>
	inline int MemCmp (const T0 &left, const T1 &right)
	{
		STATIC_ASSERT( sizeof(left) == sizeof(right) );

		return UnsafeMem::MemCmp( &left, &right, sizeof(left) );
	}

	template <typename T0, typename T1>
	inline int MemCmp (Buffer<const T0> left, Buffer<const T1> right);

/*
=================================================
	ZeroMem
=================================================
*/
	template <typename T, usize I>
	inline void ZeroMem (T (&arr)[I])
	{
		STATIC_ASSERT( CompileTime::IsZeroMemAvailable<T> );
		UnsafeMem::ZeroMem( arr, I * SizeOf<T>() );
	}

	template <typename T>
	inline void ZeroMem (T &val)
	{
		STATIC_ASSERT( CompileTime::IsZeroMemAvailable<T> );
		UnsafeMem::ZeroMem( &val, SizeOf(val) );
	}

	template <typename T>
	inline void ZeroMem (Buffer<T> buf);


}	// GXTypes
}	// GX_STL

