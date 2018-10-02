// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/STL/Common/Types.h"
#include "Engine/STL/CompileTime/TypeTraits.h"
#include <functional>

namespace GX_STL
{

// BitRotateLeft depends of Bits
// Bits depends of Hash
// hidden _BitRotateLeft not depends of this
namespace GXMath
{
namespace _math_hidden_
{
	template <typename T>
	forceinline T _BitRotateLeft (T value, usize shift);

}	// _math_hidden_
}	// GXMath

namespace GXTypes
{

	//
	// Hash Result
	//

	struct HashResult
	{
	// types
	private:
		typedef HashResult		Self;
		typedef usize			value_t;


	// variables
	private:
		usize	_value;
		

	// methods
	public:
		HashResult (GX_DEFCTOR) : _value(0) {}
		
		constexpr explicit HashResult (usize val) : _value(val) {}


		Self operator ~ () const
		{
			return Self( ~_value );
		}


		Self operator + (const Self &right) const
		{
			return Self(*this) += right;
		}

		Self& operator += (const Self &right)
		{
			_value ^= GXMath::_math_hidden_::_BitRotateLeft( right._value, 1 );
			return *this;
		}


		_GX_DIM_CMP_OPERATORS_SELF( _value );

		//operator usize () const	{ return _value; }

		//_GX_DIM_ALL_INTEGER_OPERATORS_SELF( _value )
		//_GX_DIM_ALL_INTEGER_OPERATORS_TYPE( _value, value_t, )
	};
	

	namespace _types_hidden_
	{
		inline static HashResult HashForMemoryBlock (const ubyte *ptr, usize count) noexcept
		{
			// MS Visual C++ std implementation
			#if defined(COMPILER_MSVC)
			# if defined(_HASH_SEQ_DEFINED)
				return HashResult{std::_Hash_seq( ptr, count )};
			# elif COMPILER_VERSION >= 1911
				return HashResult{std::_Hash_bytes( ptr, count )};
			# endif
			#elif defined(COMPILER_GCC) //or defined(PLATFORM_ANDROID)
				return HashResult{std::_Hash_bytes( ptr, count, 0 )};
			#elif defined(COMPILER_CLANG)
				return HashResult{std::__murmur2_or_cityhash<size_t>()( ptr, count )};
			#else
				#error "hash function not defined!"
			#endif
		}
		
	}	// _types_hidden_



	//
	// Hash function
	//

	template <typename T>
	struct Hash
	{
		typedef HashResult	result_t;

		typedef typename TypeTraits::RemoveConstVolatile< T >	key_t;

		result_t operator () (const key_t &x) const
		{
			return (result_t) std::hash< key_t >()( x );
		}
	};


}	// GXTypes
}	// GX_STL
