// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/STL/Common/Types.h"
#include "Engine/STL/CompileTime/TypeTraits.h"
#include <functional>

namespace GX_STL
{
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

		//operator usize () const	{ return _value; }

		_GX_DIM_ALL_INTEGER_OPERATORS_SELF( _value )
		_GX_DIM_ALL_INTEGER_OPERATORS_TYPE( _value, value_t, )
	};
	

	namespace _types_hidden_
	{
		inline static HashResult HashForMemoryBlock (const ubyte *ptr, usize count)
		{
			// MS Visual C++ std implementation
			#if defined(COMPILER_MSVC) and defined(_HASH_SEQ_DEFINED)
				return (HashResult) std::_Hash_seq( ptr, count );
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
