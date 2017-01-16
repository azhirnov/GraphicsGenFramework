// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../OS/Posix/AtomicOp.h"
#include "../OS/Windows/AtomicOp.h"
#include "../CompileTime/TypeListHelpers.h"

namespace GX_STL
{
namespace GXTypes
{
	
	//
	// Atomic
	//

	template <typename T>
	struct Atomic
	{
	// types
	public:
		typedef T						value_t;
		typedef Atomic<T>				Self;
		typedef _platform_::AtomicOp	Op;

	private:
		typedef CompileTime::TypeListFrom< bool,
					byte, short, int, ilong, isize,
					ubyte, ushort, uint, ulong, usize >		ValidTypeList_t;

		STATIC_ASSERT( ValidTypeList_t::HasType<T> );


	// variables
	private:
		volatile T			_value;


	// methods
	public:
		Atomic (GX_DEFCTOR): _value(0) {}

		explicit Atomic (T val): _value(val) {}
		explicit Atomic (const Self &sObj): _value(sObj.Get()) {}

		operator const volatile T () const		{ return Get(); }

		Self &	operator ++ ()					{ Op::Inc<T>( _value );			return *this; }
		Self &	operator -- ()					{ Op::Dec<T>( _value );			return *this; }
		Self &	operator += (const T& right)	{ Op::Add<T>( _value, right );  return *this; }
		Self &	operator -= (const T& right)	{ Op::Sub<T>( _value, right );  return *this; }
		Self &	operator =  (const T& right)	{ Op::Set<T>( _value, right );  return *this; }
		Self &	operator |= (const T& right)	{ Op::Or<T> ( _value, right );  return *this; }
		Self &	operator &= (const T& right)	{ Op::And<T>( _value, right );  return *this; }
		Self &	operator ^= (const T& right)	{ Op::Xor<T>( _value, right );  return *this; }
		
		T		Inc ()							{ return Op::Inc<T>( _value ); }
		T		Dec ()							{ return Op::Dec<T>( _value ); }
		T		Get ()	const					{ return Op::Get<T>( _value ); }
		T		Add (const T& right)			{ return Op::Add<T>( _value, right ); }
		T		Sub (const T& right)			{ return Op::Sub<T>( _value, right ); }
		T		Set (const T& right)			{ return Op::Set<T>( _value, right ); }
		T		Or  (const T& right)			{ return Op::Or<T> ( _value, right ); }
		T		And (const T& right)			{ return Op::And<T>( _value, right ); }
		T		Zor (const T& right)			{ return Op::Xor<T>( _value, right ); }

		Self	operator ++ (int)
		{
			const Self  ret( _value );
			++(*this);
			return ret;
		}

		Self	operator -- (int)
		{
			const Self  ret( _value );
			--(*this);
			return ret;
		}


		// if *this == compare then *this = val return old *this
		T CompareEx (const T& val, const T& compare)
		{
			return Op::CmpEx<T>( _value, val, compare );
		}
	};


}	// GXTypes
}	// GX_STL
