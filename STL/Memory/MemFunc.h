// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../BaseTypes/Types.h"
#include "../Math/MathFunc.h"

namespace GX_STL
{
namespace GXTypes
{


	//
	// Memory Viewer
	//

	template <typename T, usize I = 400>
	struct TMemoryViewer
	{
		// types
		typedef T					(*p_static_array_t)[I];
		typedef TMemoryViewer<T,I>	Self;

		// value
		p_static_array_t	ptr;

		// cast
		static const Self Cast (const void *ptr)	{ return *PointerCast< Self >( &ptr ); }

		// check
		STATIC_ASSERT( sizeof(T*) == sizeof(p_static_array_t), "size mismatch" );
	};


/*
=================================================
	PlacementNew
=================================================
*/
	namespace _types_hidden_
	{
		template <typename T>
		inline bool IsAlignedMem (void *ptr)
		{
			return	( sizeof(T) < sizeof(void*) ) or
					( ( ((usize const &) ptr) & (sizeof(void*)-1) ) == 0 );
		}
	}

	template <typename T>
	inline T * PlacementNew (void *ptr) noexcept
	{
		ASSERT( _types_hidden_::IsAlignedMem<T>( ptr ) );
		return ( new(ptr) T() );
	}

	template <typename T>
	inline T * PlacementNew (void *ptr, const T& value) noexcept
	{
		ASSERT( _types_hidden_::IsAlignedMem<T>( ptr ) );
		return ( new(ptr) T(value) );
	}
	
	template <typename T>
	inline T * PlacementNew (void *ptr, T& value) noexcept
	{
		ASSERT( _types_hidden_::IsAlignedMem<T>( ptr ) );
		return ( new(ptr) T(value) );
	}
	
	template <typename T>
	inline T * PlacementNew (void *ptr, T&& value) noexcept
	{
		ASSERT( _types_hidden_::IsAlignedMem<T>( ptr ) );
		return ( new(ptr) T( RVREF( value ) ) );
	}

	template <typename T, typename ...Types>
	inline T * PlacementNew (void *ptr, Types&&... args) noexcept
	{
		ASSERT( _types_hidden_::IsAlignedMem<T>( ptr ) );
		return ( new(ptr) T( RVREF(args)... ) );
	}

/*
=================================================
	PlacementDelete
=================================================
*/
	template <typename T>
	inline void PlacementDelete (T &value) noexcept
	{
		value.~T();
		DEBUG_ONLY( ::memset( &value, 0, sizeof(value) ) );
	}
	
/*
=================================================
	MovePointer
=================================================
*/
	namespace _types_hidden_
	{
		template <typename T, typename R>
		struct _PtrMove
		{
		};
		
		template <typename T, typename R>
		struct _PtrMove< T *, R >
		{
			typedef R *	result_t;

			static result_t  Move (T * val, isize offset) {
				return PointerCast< R >( PointerCast< ubyte >( val ) + offset );
			}
		};
		
		template <typename T, typename R>
		struct _PtrMove < const T *, R >
		{
			typedef const R *	result_t;

			static result_t  Move (const T * val, isize offset) {
				return PointerCast< R >( PointerCast< ubyte >( val ) + offset );
			}
		};
	}

	template <typename R, typename T>
	inline typename _types_hidden_::_PtrMove< T, R >::result_t  MovePointer (T ptr, isize offset)
	{
		STATIC_ASSERT( TypeTraits::IsPointer<T>, "argument must be pointer" );
		return _types_hidden_::_PtrMove< T, R >::Move( ptr, offset );
	}

	template <typename T>
	inline T  MovePointer (T ptr, isize offset)
	{
		return MovePointer< TypeTraits::RemovePointer< T >, T >( ptr, offset );
	}

/*
=================================================
	ComparePointers
=================================================
*/
	template <typename T0, typename T1>
	inline bool ComparePointers (const T0 p0, const T1 p1)
	{
		return ((const void *) p0) == ((const void *) p1);
	}
	
/*
=================================================
	CheckPointersAliasing
----
	Returns true if memory regions are intersected
=================================================
*/
	template <typename T0, typename T1>
	inline bool CheckPointersAliasing (const T0 *begin0, const T0 *end0, const T1 *begin1, const T1 *end1)
	{
		ASSERT( begin0 <= end0 );
		ASSERT( begin1 <= end1 );
		return ( begin0 < end1 ) & ( end0 > begin1 );
	}

/*
=================================================
	MemCopy
----
	memory blocks must not intersects
=================================================
*/
	inline void * MemCopy (void *pDst, const void *pSrc, usize uSize)
	{
		// TODO: add checks
		ASSERT( not CheckPointersAliasing( (ubyte *)pDst, (ubyte *)pDst + uSize,
										   (const ubyte *)pSrc, (const ubyte *)pSrc + uSize ) );

		return ::memcpy( pDst, pSrc, uSize );
	}

	template <typename T0, typename T1>
	inline void MemCopy (T0 &dst, const T1 &src)
	{
		STATIC_ASSERT( CompileTime::IsMemCopyAvailable<T0> );
		STATIC_ASSERT( CompileTime::IsMemCopyAvailable<T1> );
		STATIC_ASSERT( sizeof(dst) == sizeof(src) );

		MemCopy( &dst, &src, sizeof(dst) );
	}
	
/*
=================================================
	MemMove
----
	memory blocks may intersects
=================================================
*/
	inline void * MemMove (void *pDst, const void *pSrc, usize uSize)
	{
		// TODO: add checks
		return ::memmove( pDst, pSrc, uSize );
	}
	
/*
=================================================
	MemCmp
=================================================
*/
	inline int MemCmp (const void *pLeft, const void *pRight, usize uSize)
	{
		// TODO: add checks
		return ::memcmp( pLeft, pRight, uSize );
	}

/*
=================================================
	ZeroMem
	TODO: hide this low level functions
=================================================
*/
	inline void ZeroMem (void *dst, usize size)
	{
		::memset( dst, 0, size );
	}

	template <typename T, usize I>
	inline void ZeroMem (T (&arr)[I])
	{
		STATIC_ASSERT( CompileTime::IsZeroMemAvailable<T> );
		ZeroMem( arr, I * sizeof(T) );
	}

	template <typename T>
	inline void ZeroMem (T &val)
	{
		STATIC_ASSERT( CompileTime::IsZeroMemAvailable<T> );
		ZeroMem( &val, sizeof(T) );
	}
	
/*
=================================================
	SwapValues
=================================================
*/
	template <typename T>
	inline void SwapValues (INOUT T &a, INOUT T &b)
	{
	#if 0
		T c(a);
		a = b;
		b = c;
	#else
		T c	= RVREF( a );
		a	= RVREF( b );
		b	= RVREF( c );
	#endif
	}

	template <typename T>
	inline void SwapValuesWithTemp (INOUT T &a, INOUT T &b, INOUT T &temp)
	{
	#if 0
		temp = a;
		a    = b;
		b    = temp;
	#else
		temp = RVREF( a );
		a	 = RVREF( b );
		b	 = RVREF( temp );
	#endif
	}
	
/*
=================================================
	SwapValues
=================================================
*/
	template <typename C, typename T>
	inline void SwapMembers (C *a, C *b, T C:: *member)
	{
		SwapValues( a->*member, b->*member );
	}

	template <typename C, typename T>
	inline void SwapMembers (INOUT C &a, INOUT C &b, T C:: *member)
	{
		SwapValues( a.*member, b.*member );
	}


}	// GXTypes
}	// GX_STL
