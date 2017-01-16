// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../BaseTypes/Types.h"
#include "../CompileTime/TypeTraits.h"
#include "../Dimensions/Helpers.h"
#include <functional>

namespace GX_STL
{
namespace GXTypes
{

	namespace _types_hidden_
	{
		inline static usize HashForMemoryBlock (const ubyte *ptr, usize count)
		{
			// MS Visual C++ std implementation
			#ifdef _HASH_SEQ_DEFINED
				return std::_Hash_seq( ptr, count );
			#else
				#error "hash function not defined!"
			#endif
		}
		
	}	// _types_hidden_



	//
	// Hash Result
	//

	struct HashResult
	{
	// types
	private:
		typedef HashResult		Self;

	// variables
	private:
		usize	_value;
		
	// methods
	public:
		HashResult (GX_DEFCTOR) : _value(0) {}
		
		explicit HashResult (usize val) : _value(val) {}

		operator usize () const	{ return _value; }

		//_GX_DIM_CMP_OPERATORS_SELF( _value );
		_GX_DIM_ALL_INTEGER_OPERATORS_SELF( _value );
	};



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
