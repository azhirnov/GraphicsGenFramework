// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "StaticArray.h"
#include "Engine/STL/Types/Bitfield.h"

namespace GX_STL
{
namespace GXTypes
{

	//
	// Static Bit Array
	//

	template <usize C>
	struct StaticBitArray : public CompileTime::FastCopyable
	{
		STATIC_ASSERT( C > 0, "invalid array size" );

	// types
	private:
		static const usize	_BITS_PER_ELEM	=	C % 32 == 0 ? 32 :
												C % 16 == 0 ? 16 :
												8;

		static const usize	_BPE_POT		= CompileTime::IntLog2< usize, _BITS_PER_ELEM >;
		static const usize	_ELEM_IDX_MASK	= (1 << _BPE_POT) - 1;

		static const usize	_ARR_COUNT		= ( C + _BITS_PER_ELEM - 1 ) / _BITS_PER_ELEM;

		typedef Bitfield< _BITS_PER_ELEM >			_elem_t;
		typedef typename _elem_t::BitRef			_elem_ref_t;
		typedef StaticArray< _elem_t, _ARR_COUNT >	_arr_t;


	public:
		typedef StaticBitArray<C>		Self;
		typedef _elem_ref_t				BitRef;
		typedef _elem_t					value_t;
		
		static const usize	STATIC_COUNT = C;


	// variables
	private:
		_arr_t		_memory;


	// methods
	private:
		static usize _ToArrayIndex (usize i)	{ return i >> _BPE_POT; }
		static usize _ToElemIndex (usize i)		{ return (i & _ELEM_IDX_MASK); }


	public:
		StaticBitArray (GX_DEFCTOR) {}
		
		StaticBitArray (const Self &other) : _memory(other._memory) {}
		
		StaticBitArray (Self &&other) = default;
		
		Self &	operator =  (Self &&right)		= default;
		Self &	operator =  (const Self &right)	= default;

		bool	operator == (const Self &right) const;
		bool	operator != (const Self &right) const;

		BitRef	operator [] (usize i);
		bool	operator [] (usize i) const;
		
		void	SetAt (usize i, bool bit);

		void	Set (usize i);
		void	Reset (usize i);
		void	And (usize i, bool bit);
		void	Or (usize i, bool bit);
		void	Xor (usize i, bool bit);
		
		void	SetAll (bool bit);

		bool	Empty ()		const				{ return false; }
		usize	Count ()		const				{ return C; }
		usize	Capacity ()		const				{ return C; }
		usize	MaxCapacity ()	const				{ return Capacity(); }
		BytesU	Size ()			const				{ return _memory.Size(); }
		BytesU	FullSize ()		const				{ return _memory.FullSize(); }
		usize	LastIndex ()	const				{ return Count()-1; }

		operator Buffer<const value_t> () const		{ return _memory; }
		

		static constexpr bool	IsLinearMemory ()	{ return true; }

		friend void SwapValues (INOUT Self &left, INOUT Self &right)
		{
			SwapValues( left._memory, right._memory );
		}
	};

	
	
	template <usize C>
	inline bool  StaticBitArray<C>::operator == (const Self &right) const
	{
		return _memory == right._memory;
	}

	
	template <usize C>
	inline bool  StaticBitArray<C>::operator != (const Self &right) const
	{
		return not (*this == right);
	}

	
	template <usize C>
	inline typename StaticBitArray<C>::BitRef  StaticBitArray<C>::operator [] (usize index)
	{
		ASSUME( index < Count() );
		return _memory[ _ToArrayIndex( index ) ][ _ToElemIndex( index ) ];
	}

	
	template <usize C>
	inline bool  StaticBitArray<C>::operator [] (usize index) const
	{
		ASSUME( index < Count() );
		return _memory[ _ToArrayIndex( index ) ][ _ToElemIndex( index ) ];
	}
	

	template <usize C>
	inline void  StaticBitArray<C>::SetAt (usize index, bool bit)
	{
		ASSUME( index < Count() );
		return _memory[ _ToArrayIndex( index ) ].SetAt( _ToElemIndex( index ), bit );
	}

	
	template <usize C>
	inline void  StaticBitArray<C>::Set (usize index)
	{
		ASSUME( index < Count() );
		return _memory[ _ToArrayIndex( index ) ].Set( _ToElemIndex( index ) );
	}

	
	template <usize C>
	inline void  StaticBitArray<C>::Reset (usize index)
	{
		ASSUME( index < Count() );
		return _memory[ _ToArrayIndex( index ) ].Reset( _ToElemIndex( index ) );
	}

	
	template <usize C>
	inline void  StaticBitArray<C>::And (usize index, bool bit)
	{
		ASSUME( index < Count() );
		return _memory[ _ToArrayIndex( index ) ].And( _ToElemIndex( index ) );
	}

	
	template <usize C>
	inline void  StaticBitArray<C>::Or (usize index, bool bit)
	{
		ASSUME( index < Count() );
		return _memory[ _ToArrayIndex( index ) ].Or( _ToElemIndex( index ) );
	}

	
	template <usize C>
	inline void  StaticBitArray<C>::Xor (usize index, bool bit)
	{
		ASSUME( index < Count() );
		return _memory[ _ToArrayIndex( index ) ].Xor( _ToElemIndex( index ) );
	}
	
	
	template <usize C>
	inline void  StaticBitArray<C>::SetAll (bool bit)
	{
		FOR( i, _memory ) {
			_memory[i].SetAll( bit );
		}
	}

	
	template <usize C>
	struct Hash< StaticBitArray<C> > :
		private Hash< Buffer<const typename StaticBitArray<C>::value_t > >
	{
		typedef StaticBitArray<C>								key_t;
		typedef Hash< Buffer<const typename key_t::value_t > >	base_t;
		typedef typename base_t::result_t						result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x );
		}
	};

}	// GXTypes
}	// GX_STL
