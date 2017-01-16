// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../BaseTypes/Types.h"
#include "../CompileTime/TemplateMath.h"
#include "../Math/MathTypes.h"

namespace GX_STL
{
namespace GXMath
{

	template <typename T> struct Percentage;



	//
	// Percentage
	//

	template <typename T>
	struct Percentage :	public CompileTime::CopyQualifiers< T >
	{
	// types
	public:
		typedef Percentage<T>		Self;
		typedef T					value_t;


	// variables
	private:
		T	_value;


	// methods
	public:
		Percentage (GX_DEFCTOR) : _value(T(0)) {}

		explicit
		Percentage (T value) : _value(value)	{ ASSERT( value >= T(0) ); }

		T const&	ref ()				const	{ return _value; }
		T &			ref ()						{ return _value; }

		_GX_DIM_ALL_FLOAT_OPERATORS_SELF( _value )

		T			GetPercents ()		const	{ return _value; }					// 0..100%
		T			GetFraction ()		const	{ return _value * T(0.01); }		// 0..1

		template <typename B>
		B			Of (const B &value)	const	{ return B( value * GetFraction() ); }

		String		ToString ()			const	{ return String().FormatF( GetPercents(), StringFormatF().Fmt(0,2) ) << '%'; }

		static Self	FromPercents (T value)		{ return Self( value ); }
		static Self	FromFraction (T value)		{ return Self( value * T(100.0) ); }

		static Self	Max ()						{ return Self( T(100.0) ); }
		static Self	Min ()						{ return Self( T(0.0) ); }
	};
	

}	// GXMath

namespace GXTypes
{

	GX_TEMPLATE_TYPE_INFO_WITH_INNER_TYPE( GXMath::Percentage, "Percentage" );


	template <typename T>
	struct Hash< GXMath::Percentage<T> > : private Hash<T>
	{
		typedef GXMath::Percentage<T>		key_t;
		typedef Hash<T>						base_t;
		typedef typename base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x.GetPercents() );
		}
	};

}	// GXTypes
}	// GX_STL
