// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Helpers.h"
#include "../CompileTime/TTypeInfo.h"

namespace GX_STL
{
namespace GXTypes
{
	
	template <typename T> struct Bytes;
	template <typename T> struct Bits;

	typedef Bytes<usize>	BytesU;
	typedef Bytes<isize>	BytesI;
	typedef Bits<usize>		BitsU;
	typedef Bits<isize>		BitsI;



	//
	// Bytes
	//

	template <typename T>
	struct Bytes :	public CompileTime::CopyQualifiers< T >
	{
		STATIC_ASSERT( CompileTime::IsInteger<T> );

	// types
	public:
		typedef Bytes<T>	Self;
		typedef T			value_t;


	// variables
	private:
		T	_value;


	// methods
	public:
		Bytes (GX_DEFCTOR) : _value(0) {}

		explicit
		Bytes (T value) : _value(value) {}
		
		template <typename B>
		explicit
		Bytes (Bits<B> bits);

		Bits<T> ToBits () const;
		
		template <typename B>
		Bits<B> ToBits () const;

		operator T () const				{ return _value; }

		template <typename B>
		Bytes<B> To () const;

		T		Kb ()	const			{ return _value >> 10; }
		T		Mb ()	const			{ return _value >> 20; }
		T		Gb ()	const			{ return _value >> 30; }
		
		template <typename B>
		B		Kb ()	const			{ return B(_value) / (1 << 10); }

		template <typename B>
		B		Mb ()	const			{ return B(_value) / (1 << 20); }

		template <typename B>
		B		Gb ()	const			{ return B(_value) / (1 << 30); }

		Self &	FromBytes (T value)		{ _value = value;  return *this; }
		Self &	FromKb (T value)		{ _value = value << 10;  return *this; }
		Self &	FromMb (T value)		{ _value = value << 20;  return *this; }
		Self &	FromGb (T value)		{ _value = value << 30;  return *this; }

		template <typename B>
		static Self SizeOf ()			{ return Self( sizeof(B) ); }
		
		template <typename B>
		static Self SizeOf (B value)	{ return Self( sizeof(B) ); }

		//String ToString () const;

		_GX_DIM_ALL_INTEGER_OPERATORS_SELF( _value )
	};



	//
	// Bits
	//

	template <typename T>
	struct Bits : public CompileTime::CopyQualifiers< T >
	{
		STATIC_ASSERT( CompileTime::IsInteger<T> );

	// types
	public:
		typedef Bits<T>		Self;
		typedef T			value_t;


	// variables
	private:
		T	_value;
		

	// methods
	public:
		Bits (GX_DEFCTOR) : _value(0) {}

		explicit
		Bits (T value) : _value(value) {}
		
		template <typename B>
		explicit
		Bits (Bytes<B> bytes);

		Bytes<T> ToBytes () const;
		
		template <typename B>
		Bytes<B> ToBytes () const;
		
		operator T () const				{ return _value; }
		
		template <typename B>
		Bits<B> To () const;

		template <typename B>
		static Self SizeOf ()			{ return Bytes<T>::SizeOf<B>().ToBits(); }
		
		template <typename B>
		static Self SizeOf (B value)	{ return Bytes<T>::SizeOf<B>().ToBits(); }
		
		//String ToString () const;

		_GX_DIM_ALL_INTEGER_OPERATORS_SELF( _value )
	};



	template <typename T>
	constexpr Bytes<T> ToBytes (T value)
	{
		return Bytes<T>( value );
	}
	
	template <typename T>
	constexpr Bits<T> ToBits (T value)
	{
		return Bits<T>( value );
	}
	

	template <typename T>
	constexpr Bytes<usize> SizeOf ()
	{
		return Bytes<usize>::SizeOf<T>();
	}
	
	template <typename T>
	constexpr Bytes<usize> SizeOf (const T&)
	{
		return Bytes<usize>::SizeOf<T>();
	}



//----------------------------------- Bytes --------------------------------//
	
	template <typename T>
	template <typename B>
	inline Bytes<T>::Bytes (Bits<B> bits) : _value( T((B)bits) >> 3 )
	{}


	template <typename T>
	inline Bits<T>  Bytes<T>::ToBits () const
	{
		return Bits<T>( *this );
	}
	

	template <typename T>
	template <typename B>
	inline Bits<B>  Bytes<T>::ToBits () const
	{
		return Bits<B>( *this );
	}
	

	template <typename T>
	template <typename B>
	inline Bytes<B>  Bytes<T>::To () const
	{
		return Bytes<B>( (B)_value );
	}
	

//----------------------------------- Bits ---------------------------------//

	template <typename T>
	template <typename B>
	inline Bits<T>::Bits (Bytes<B> bytes) : _value( T((B)bytes) << 3 )
	{}
	

	template <typename T>
	inline Bytes<T>  Bits<T>::ToBytes () const
	{
		return Bytes<T>( *this );
	}
	

	template <typename T>
	template <typename B>
	inline Bytes<B>  Bits<T>::ToBytes () const
	{
		return Bytes<B>( *this );
	}
	

	template <typename T>
	template <typename B>
	inline Bits<B>  Bits<T>::To () const
	{
		return Bits<B>( (B) _value );
	}

	
	GX_TEMPLATE_TYPE_INFO_WITH_INNER_TYPE( Bytes, "Bytes" );
	GX_TEMPLATE_TYPE_INFO_WITH_INNER_TYPE( Bits,  "Bits"  );

	
	template <typename T>
	struct Hash< Bytes<T> > : private Hash<T>
	{
		typedef Bytes<T>					key_t;
		typedef Hash<T>						base_t;
		typedef typename base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x );
		}
	};


	template <typename T>
	struct Hash< Bits<T> > : private Hash<T>
	{
		typedef Bits<T>						key_t;
		typedef Hash<T>						base_t;
		typedef typename base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x );
		}
	};

}	// GXTypes
}	// GX_STL
