// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/Common.h"


namespace Engine
{
namespace Base
{

	//
	// Serializable Value Type
	//

	struct ESerializableValue
	{
		enum type : ubyte
		{
			BOOL = 1,
			CHAR,
			WCHAR,
			INT8,
			INT16,
			INT32,
			INT64,
			UINT8,
			UINT16,
			UINT32,
			UINT64,
			FLOAT32,
			FLOAT64,
			VECTOR,
			MATRIX,
			POD_STRUCT,
			_POD,

			STRING,
			ARRAY,
			MAP,
			SET,
			POINTER,
			REFERENCED_PTR,
			STRUCT,

			_STRUCT_BEGIN = STRUCT,
			_STRUCT_PARENT,
			_STRUCT_END,

			_FILE_BEGIN,
			_FILE_BEGIN_READ_ONLY,
			_FILE_END
		};


		static constexpr bool IsPOD (int value)
		{
			return value < _POD;
		}

		
		template <typename T>
		static constexpr type GetValueTypeOf (T const&) {
			return CompileTime::IsMemCopyFromFileAvailable< T > ? POD_STRUCT : STRUCT;
		}

		template <typename T>
		static constexpr type GetValueTypeOf (T* const&) {
			return POINTER;
		}

		template <typename T>
		static constexpr type GetValueTypeOf (Ptr<T> const&) {
			return POINTER;
		}
	
		template <typename T, typename B, typename S>
		static constexpr type GetValueTypeOf (CRefCounter<T,B,S> const&) {
			return REFERENCED_PTR;
		}
	
		static constexpr type GetValueTypeOf (bool const&)			{ return BOOL; }
		static constexpr type GetValueTypeOf (char const&)			{ return CHAR; }
		static constexpr type GetValueTypeOf (wchar const&)			{ return WCHAR; }
		static constexpr type GetValueTypeOf (byte const&)			{ return INT8; }
		static constexpr type GetValueTypeOf (short const&)			{ return INT16; }
		static constexpr type GetValueTypeOf (int const&)			{ return INT32; }
		static constexpr type GetValueTypeOf (ilong const&)			{ return INT64; }
		static constexpr type GetValueTypeOf (ubyte const&)			{ return UINT8; }
		static constexpr type GetValueTypeOf (ushort const&)		{ return UINT16; }
		static constexpr type GetValueTypeOf (uint const&)			{ return UINT32; }
		static constexpr type GetValueTypeOf (ulong const&)			{ return UINT64; }
		static constexpr type GetValueTypeOf (float const&)			{ return FLOAT32; }
		static constexpr type GetValueTypeOf (double const&)		{ return FLOAT64; }

		template <typename T, usize I>
		static constexpr type GetValueTypeOf (Vec<T,I> const&) {
			return VECTOR;
		}

		template <typename T, usize C, usize R>
		static constexpr type GetValueTypeOf (Matrix<T,C,R> const&) {
			return MATRIX;
		}
	
		template <typename T, typename S, typename A>
		static constexpr type GetValueTypeOf (TString<T,S,A> const&) {
			return STRING;
		}
	
		template <typename T, typename S, typename A>
		static constexpr type GetValueTypeOf (Array<T,S,A> const&) {
			return ARRAY;
		}
	
		template <typename T, usize C, typename S>
		static constexpr type GetValueTypeOf (StaticArray<T,C,S> const&) {
			return ARRAY;
		}
	
		template <typename K, typename T, typename S, typename A>
		static constexpr type GetValueTypeOf (Map<K,T,S,A> const&) {
			return MAP;
		}

		template <typename K, typename S, typename A>
		static constexpr type GetValueTypeOf (Set<K,S,A> const&) {
			return SET;
		}
	
		template <typename V, typename D, typename S>
		static constexpr type GetValueTypeOf (PhysicsValue<V,D,S> const&) {
			return GetValueTypeOf( V() );
		}
	
		template <typename T, usize I>
		static constexpr type GetValueTypeOf (PhysicsValueVec<T,I> const&) {
			return VECTOR;
		}
	
		template <typename T>
		static constexpr type GetValueTypeOf (Radians<T> const&) {
			return GetValueTypeOf( T() );
		}
	
		template <typename T>
		static constexpr type GetValueTypeOf (Degrees<T> const&) {
			return GetValueTypeOf( T() );
		}

		template <typename T>
		static constexpr type GetValueTypeOf (Percentage<T> const&) {
			return GetValueTypeOf( T() );
		}
	};


}	// Base
}	// Engine
