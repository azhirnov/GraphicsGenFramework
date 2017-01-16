// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Memory/MemFunc.h"
#include "../Types/Utils.h"
#include "../Defines/Errors.h"
#include "../Containers/CopyStrategy.h"

namespace GX_STL
{
namespace GXTypes
{
	
	//
	// Default Allocator
	//

	template <typename T>
	struct TDefaultAllocator : public Noninstancable
	{
		static bool Allocate (INOUT T *&ptr, INOUT usize &size) noexcept
		{
			ASSUME( size > 0 );

			ptr = (T*) ::operator new( sizeof(T) * size );
			ASSERT( ptr != null and "can't allocate memory!" );

			return ptr != null;
		}

		static void Deallocate (T *&ptr) noexcept
		{
			::operator delete( ptr );
			ptr = null;
		}
	};


	template <>
	struct TDefaultAllocator<void> : public Noninstancable
	{
		static bool Allocate (INOUT void *&ptr, INOUT usize &size) noexcept
		{
			return TDefaultAllocator<ubyte>::Allocate( reinterpret_cast< ubyte *&>(ptr), size );
		}

		static void Deallocate (void *&ptr) noexcept
		{
			return TDefaultAllocator<ubyte>::Deallocate( reinterpret_cast< ubyte *&>( ptr ) );
		}
	};



	//
	// Aligned Allocator
	//
	
	
# ifdef PLATFORM_ANDROID
#	pragma message ("aligned allocation changed to default")
	
	template <typename T, usize Align = 16>
	struct TAlignedAllocator : TDefaultAllocator<T>
	{
		static const usize	ALIGN = Align;
	};

# else

	template <typename T, usize Align = 16>
	struct TAlignedAllocator : public Noninstancable
	{
		static const usize	ALIGN = Align;

		static bool Allocate (INOUT T *&ptr, INOUT usize &size) noexcept
		{
			ASSUME( size > 0 );

			ptr = (T*) _mm_malloc( sizeof(T) * size, Align );
			ASSERT( ptr != null and "can't allocate aligned memory!" );

			return ptr != null;
		}

		static void Deallocate (T *&ptr) noexcept
		{
			_mm_free( ptr );
			ptr = null;
		}
	};


	template <usize Align>
	struct TAlignedAllocator<void, Align> : public Noninstancable
	{
		static const usize	ALIGN = Align;

		static bool Allocate (INOUT void *&ptr, INOUT usize &size) noexcept
		{
			return TAlignedAllocator<ubyte, Align>::Allocate( reinterpret_cast< ubyte *&>(ptr), size );
		}

		static void Deallocate (void *&ptr) noexcept
		{
			return TAlignedAllocator<ubyte, Align>::Deallocate( reinterpret_cast< ubyte *&>( ptr ) );
		}
	};

# endif

}	// GXTypes
}	// GX_STL
