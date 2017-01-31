// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/STL/Common/Pointer.h"

namespace GX_STL
{
namespace GXTypes
{

	template <typename T>
	struct Buffer;
	
/*
=================================================
	IsAlignedMem
=================================================
*/
	namespace _types_hidden_
	{
		template <typename T>
		forceinline bool IsAlignedMem (void *ptr)
		{
			enum { ALIGN = alignof(T) };
			return (sizeof(T) < ALIGN) or IsAlignedPointer< ALIGN >( ptr );
		}
	}
	
/*
=================================================
	PlacementNew
=================================================
*/
	namespace UnsafeMem
	{
		template <typename T>
		forceinline T * PlacementNew (void *ptr) noexcept
		{
			ASSERT( _types_hidden_::IsAlignedMem<T>( ptr ) );
			return ( new(ptr) T() );
		}

		template <typename T, typename ...Types>
		forceinline T * PlacementNew (void *ptr, Types const&... args) noexcept
		{
			ASSERT( _types_hidden_::IsAlignedMem<T>( ptr ) );
			return ( new(ptr) T( args... ) );
		}

		template <typename T, typename ...Types>
		forceinline T * PlacementNew (void *ptr, Types&&... args) noexcept
		{
			ASSERT( _types_hidden_::IsAlignedMem<T>( ptr ) );
			return ( new(ptr) T( RVREF(args)... ) );
		}

	}	// UnsafeMem
	
/*
=================================================
	PlacementNew
=================================================
*/
	template <typename T, typename C>
	forceinline T * PlacementNew (Buffer<C> buf) noexcept;
			
	template <typename T, typename C, typename ...Types>
	forceinline T * PlacementNew (Buffer<C> buf, Types const&... args) noexcept;

	template <typename T, typename C, typename ...Types>
	forceinline T * PlacementNew (Buffer<C> buf, Types&&... args) noexcept;

/*
=================================================
	PlacementDelete
=================================================
*/
	template <typename T>
	forceinline void PlacementDelete (T &value) noexcept
	{
		value.~T();
		DEBUG_ONLY( ::memset( &value, 0, sizeof(value) ) );
	}


}	// GXTypes
}	// GX_STL

