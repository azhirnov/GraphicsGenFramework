// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Types.h"
#include "../CompileTime/NewTypeInfo.h"
#include "../Math/MathFunc.h"

namespace GX_STL
{
namespace GXTypes
{

	//
	// Bitfield
	//
	
	template <uint B>
	struct Bitfield : public CompileTime::PODType
	{
	// types
	public:
		struct BitRef;
		typedef Bitfield< B >									Self;
		typedef typename CompileTime::NearUInt::FromBits< B >	T;
		typedef T												value_t;


	// variables
	private:
		T	_bits	: B;

		//  ||||||||||||
		// B-1         0
		
		static const T	_MAX_VALUE = CompileTime::ToMask< T, B >;


	// methods
	public:
		Bitfield (GX_DEFCTOR): _bits( T(0) )		{}

		explicit
		Bitfield (const T bits): _bits(bits)		{}
		
		bool	Get (uint i) const					{ ASSUME(i<B);  return !!( _bits & (T(1) << i) ); }
		Self &	Set (uint i)						{ ASSUME(i<B);  _bits |= T(1) << i;  return *this; }
		Self &	Reset (uint i)						{ ASSUME(i<B);  _bits ^= T(1) << i;  return *this; }
		Self &	SetAt (uint i, bool bit)			{ ASSUME(i<B); (_bits &= ~(T(1) << i)) |= T(bit) << i;  return *this; }

		Self &	And (uint i, bool bit)				{ ASSUME(i<B);  _bits &= T(bit) << i;  return *this; }
		Self &	Or (uint i, bool bit)				{ ASSUME(i<B);  _bits |= T(bit) << i;  return *this; }
		Self &	Xor (uint i, bool bit)				{ ASSUME(i<B);  _bits ^= T(bit) << i;  return *this; }

		Self &	SetAll (bool bit = true)			{ _bits = bit ? _MAX_VALUE : T(0);  return *this; }

		bool	IsZero ()	const					{ return _bits == 0; }
		bool	IsNotZero () const					{ return _bits != 0; }
		bool	Empty ()	const					{ return IsZero(); }

		uint	MinIndex () const					{ return GXMath::BitScanForward( _bits ); }
		uint	MaxIndex () const					{ return GXMath::BitScanReverse( _bits ); }

		operator T () const							{ return _bits; }

		static uint Count ()						{ return B; }

		Self	operator ~  ()	const				{ return Self( ~_bits ); }

		Self &	operator =  (const T right)			{ _bits =   right;  return *this; }

		Self &	operator |= (const T right)			{ _bits |=  right;  return *this; }
		Self &	operator &= (const T right)			{ _bits &=  right;  return *this; }
		Self &	operator ^= (const T right)			{ _bits ^=  right;  return *this; }
		
		Self &	operator >>= (const T right)		{ _bits >>= right;  return *this; }
		Self &	operator <<= (const T right)		{ _bits <<= right;  return *this; }

		Self	operator |  (const T right) const	{ return Self( _bits | right ); }
		Self	operator &  (const T right) const	{ return Self( _bits & right ); }
		Self	operator ^  (const T right) const	{ return Self( _bits ^ right ); }
		
		Self	operator >> (const T right) const	{ return Self( _bits >> right ); }
		Self	operator << (const T right) const	{ return Self( _bits << right ); }

		bool 	operator [] (uint i) const			{ return Get( i ); }
		BitRef	operator [] (uint i)				{ ASSUME(i < B);  return BitRef( *this, i ); }
		

		void SetInterval (uint first, uint last)
		{
			ASSUME( first <= last and last < B );
			_bits |= GXMath::ToMask<T>( first, last+1 );
		}

		void ResetInterval (uint first, uint last)
		{
			ASSUME( first <= last and last < B );
			_bits ^= GXMath::ToMask<T>( first, last+1 );
		}

		bool HasInterval (uint first, uint last) const
		{
			ASSUME( first <= last and last < B );
			const T	mask = GXMath::ToMask<T>( first, last+1 );
			return EnumEq( _bits, mask );
		}

		bool HasAll () const
		{
			return HasInterval( 0, B-1 );
		}

		bool BitsEq (const Self &right) const
		{
			// Compare only setted bits!
			// Warning: not same result as right.BitsEq( *this )

			return EnumEq( _bits, right._bits );
		}

		bool AnyBitEq (const Self &right) const
		{
			return (_bits & right._bits) != 0;
		}


	public:
		struct BitRef
		{
		private:
			Bitfield<B> &	_bitfield;
			uint			_index;

			void _Set ()			{ _bitfield.Set( _index ); }
			void _Reset ()			{ _bitfield.Reset( _index ); }
			void _SetAt (bool bit)	{ _bitfield.SetAt( _index, bit ); }
			void _And (bool bit)	{ _bitfield.And( _index, bit ); }
			void _Or (bool bit)		{ _bitfield.Or( _index, bit ); }
			void _Xor (bool bit)	{ _bitfield.Xor( _index, bit ); }
			bool _Get () const		{ return _bitfield.Get( _index ); }

		public:
			BitRef (Bitfield<B> &bitfield, uint index):
				_bitfield( bitfield ), _index(index)
			{}

			BitRef & operator = (const BitRef &bit)
			{
				_Set( bit._Get() );
				return *this;
			}

			template <typename C>
			BitRef & operator = (const C &right)
			{
				_Set( (bool)right );
				return *this;
			}

			operator bool () const
			{
				return _Get();
			}

			template <typename C>
			operator C () const
			{
				return C( _Get() );
			}

			template <typename C>
			BitRef &	operator |= (const C right)
			{
				_Or( (bool)right );
				return *this;
			}

			template <typename C>
			BitRef &	operator &= (const C right)
			{
				_And( (bool)right );
				return *this;
			}

			template <typename C>
			BitRef &	operator ^= (const C right)
			{
				_Xor( (bool)right );
				return *this;
			}
		};


		STATIC_ASSERT( CompileTime::SizeOf<T>::bits >= B );
	};

	
	template <uint B>
	struct Hash < Bitfield<B> > :
		private Hash< typename Bitfield<B>::value_t >
	{
		typedef Bitfield<B>						key_t;
		typedef Hash< typename key_t::value_t >	base_t;
		typedef typename base_t::result_t		result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x );
		}
	};

}	// GXTypes
}	// GX_STL
