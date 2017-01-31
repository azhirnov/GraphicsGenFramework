// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/STL/CompileTime/TypeListHelpers.h"

namespace GX_STL
{
namespace GXTypes
{
	
	//
	// Pair
	//

	template <typename FirstType, typename SecondType>
	struct Pair : public CompileTime::CopyQualifiers< FirstType, SecondType >
	{
	// types
	public:
		typedef FirstType						first_t;
		typedef SecondType						second_t;
		typedef Pair< FirstType, SecondType >	Self;


	// variables
	public:
		first_t		first;
		second_t	second;


	// methods
	public:
		Pair (GX_DEFCTOR) : first(), second() {}

		Pair (const first_t &first, const second_t &second) : first(first), second(second) {}
		Pair (first_t &&first, second_t &&second) : first( RVREF(first) ), second( RVREF(second) ) {}

		Pair (Self &&other)			= default;
		Pair (const Self &other)	= default;
		
		Self &	operator =  (Self &&right)		= default;
		Self &	operator =  (const Self &right)	= default;

		bool	operator == (const Self &right) const	{ return first == right.first and second == right.second; }
		bool	operator != (const Self &right)	const	{ return not (*this == right); }

		friend void SwapValues (INOUT Self &left, INOUT Self &right)
		{
			SwapValues( left.first,		right.first );
			SwapValues( left.second,	right.second );
		}
	};

	
	template <typename FirstType, typename SecondType>
	struct Hash< Pair< FirstType, SecondType > > :
		private Hash< FirstType >, private Hash< SecondType >
	{
		STATIC_ASSERT(( CompileTime::IsSameTypes< typename Hash< FirstType >::result_t,
													typename Hash< SecondType >::result_t > ));

		typedef Pair< FirstType, SecondType >	key_t;
		typedef Hash< FirstType >::result_t		result_t;

		result_t operator () (const key_t &x) const
		{
			return	(Hash< FirstType >::operator ()( x.first ) << 2) ^
					Hash< SecondType >::operator ()( x.second );
		}
	};

}	// GX_STL
}	// GXTypes
