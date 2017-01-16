// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.
/*
	Construct	- call contructor
	Destruct	- call destructor
	Create		- call default constructors for elements
	Destroy		- call destructors for elements
	Copy		- copy elements (copy-ctor)
	Move		- move external objects (move-ctor)
	Replace		- move internal elements (move-ctor + dtor)
*/

#pragma once

#include "../Memory/MemFunc.h"
#include "../Types/Utils.h"
#include "../Dimensions/ByteAndBit.h"

namespace GX_STL
{
namespace GXTypes
{

	struct CopyStrategy
	{
	
		//
		// Copy Strategy with MemCopy & MemMove
		//

		template <typename T>
		struct MemCopyWithCtor : public Noninstancable
		{
			STATIC_ASSERT(	CompileTime::IsMemCopyAvailable<T> and
							not CompileTime::IsNoncopyable<T> );

			static void Create (T *ptr, usize count)
			{
				for (usize i = 0; i < count; ++i) {
					PlacementNew<T>( ptr + i );
				}
			}

			static void Destroy (T *ptr, usize count)
			{
				for (usize i = 0; i < count; ++i) {
					PlacementDelete( ptr[i] );
				}
			}

			static void Copy (T *to, const T * const from, const usize count)
			{
				for (usize i = 0; i < count; ++i) {
					PlacementNew<T>( to+i, from[i] );
				}
			}

			static void Move (T *to, T *from, const usize count)
			{
				for (usize i = 0; i < count; ++i) {
					PlacementNew<T>( to+i, RVREF( from[i] ) );
				}
			}
			
			static void Replace (T *to, T *from, const usize count)
			{
				MemMove( to, from, sizeof(T)*count );
			}
		};



		//
		// Copy Strategy with Copy Ctor and Move Ctor
		//

		template <typename T>
		struct CopyAndMoveCtor : public Noninstancable
		{
			STATIC_ASSERT( (CompileTime::IsCtorAvailable<T> or
							CompileTime::IsDtorAvailable<T>) and
							not CompileTime::IsNoncopyable<T> );

			static void Create (T *ptr, usize count)
			{
				for (usize i = 0; i < count; ++i) {
					PlacementNew<T>( ptr + i );
				}
			}

			static void Destroy (T *ptr, usize count)
			{
				for (usize i = 0; i < count; ++i) {
					PlacementDelete( ptr[i] );
				}
			}

			static void Copy (T *to, const T * const from, const usize count)
			{
				for (usize i = 0; i < count; ++i) {
					PlacementNew<T>( to+i, from[i] );
				}
			}

			static void Move (T *to, T *from, const usize count)
			{
				for (usize i = 0; i < count; ++i) {
					PlacementNew<T>( to+i, RVREF( from[i] ) );
				}
			}
			
			static void Replace (T *to, T *from, const usize count)
			{
				for (usize i = 0; i < count; ++i) {
					PlacementNew<T>( to+i, RVREF( from[i] ) );
					PlacementDelete( from[i] );
				}
			}
		};



		//
		// Copy Strategy with MemCopy & MemMove without Ctors
		//

		template <typename T>
		struct MemCopyWithoutCtor : public Noninstancable
		{
			STATIC_ASSERT(	CompileTime::IsMemCopyAvailable<T>	 and
							CompileTime::IsZeroMemAvailable<T>	 and
							not CompileTime::IsCtorAvailable<T>  and
							not CompileTime::IsDtorAvailable<T>  and
							not CompileTime::IsNoncopyable<T> );

			static void Create (T *ptr, usize count)
			{
				GXTypes::ZeroMem( ptr, sizeof(T)*count );
			}

			static void Destroy (T *ptr, usize count)
			{
				DEBUG_ONLY( GXTypes::ZeroMem( ptr, sizeof(T)*count ) );
			}

			static void Copy (T *to, const T * const from, const usize count)
			{
				MemCopy( to, from, sizeof(T)*count );
			}

			static void Move (T *to, T * from, const usize count)
			{
				MemMove( to, from, sizeof(T)*count );
			}
			
			static void Replace (T *to, T *from, const usize count)
			{
				Move( to, from, count );
			}
		};

	};

	
	//
	// Auto Detect Copy Strategy
	//

	template <typename T>
	struct AutoDetectCopyStrategy : public Noninstancable
	{
			STATIC_ASSERT( not CompileTime::IsNoncopyable<T> );

		static const bool _is_fastcopy	= CompileTime::IsMemCopyAvailable<T>;
		static const bool _is_pod		= _is_fastcopy and not CompileTime::IsCtorAvailable<T> and not CompileTime::IsDtorAvailable<T>;

		typedef typename CompileTime::SwitchType< _is_pod, CopyStrategy::MemCopyWithoutCtor<T>,
							typename CompileTime::SwitchType< _is_fastcopy, CopyStrategy::MemCopyWithCtor<T>,
																			CopyStrategy::CopyAndMoveCtor<T> > >		type;
	};


}	// GXTypes
}	// GX_STL
