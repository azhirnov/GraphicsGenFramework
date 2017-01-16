// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Array.h"
#include "../CompileTime/TypeListHelpers.h"

namespace GX_STL
{
namespace GXTypes
{

	
	#define RET_ERROR( _ret_ )	{ WARNING("error in static array");  return _ret_; }
	#define RET_FALSE			RET_ERROR( false )
	#define RET_VOID			RET_ERROR( ; )



	//
	// Static Array
	//

	template <	typename T,
				usize C,
				typename S = typename AutoDetectCopyStrategy<T>::type
			 >
	struct StaticArray : public CompileTime::CopyQualifiers< CompileTime::PODStruct, T >
	{
		STATIC_ASSERT( C > 0, "invalid array size" );

	// types
	public:
		typedef StaticArray<T,C,S>		Self;
		typedef S						Strategy;
		typedef T						value_t;
		typedef	T *						iterator;
		typedef const T *				const_iterator;
		
		static const usize	STATIC_COUNT	= C;
		static const usize	STATIC_SIZE		= C * sizeof(T);


	// variables
	protected:
		T	_memory[C];


	// methods
	private:
		void _Create (const T *pMem, usize count);

	public:
		StaticArray (GX_DEFCTOR);

		StaticArray (const Self &other);

		explicit
		StaticArray (const T& value);

		explicit
		StaticArray (Buffer<const T> other);

		StaticArray (Self &&other) = default;

		T		*	ptr ()										{ return _memory; }
		const T	*	ptr () const								{ return _memory; }
		
		T		&	Back ()										{ return _memory[Count()-1]; }
		const T	&	Back () const								{ return _memory[Count()-1]; }
		T		&	Front ()									{ return _memory[0]; }
		const T	&	Front () const								{ return _memory[0]; }

		bool		operator !  () const						{ return not Empty(); }
		
		bool		operator == (Buffer<const T> right) const	{ return Buffer<const T>(*this) == right; }
		bool		operator != (Buffer<const T> right) const	{ return not ( *this == right ); }
		
		Self &		operator =  (Self &&right)		= default;
		Self &		operator =  (const Self &right)	= default;

		operator	Buffer<T> ()								{ return Buffer<T>( _memory, Count() ); }
		operator	Buffer<const T> () const					{ return Buffer<const T>( _memory, Count() ); }

		T		&	operator [] (usize i);
		const T	&	operator [] (usize i) const;

		
		bool At (usize index, OUT T & value) const;

		void Free ();
		void Clear ();
		
		void Copy (Buffer<const T> other);

		bool Set (usize index, const T &value);
		bool Set (usize i, T &&value);

		void Swap (usize first, usize second);
		
		usize GetIndex (const T &value) const	{ return Buffer<const T>(*this).GetIndex( value ); }

		bool			Empty ()		const	{ return false; }
		usize			Count ()		const	{ return C; }
		usize			Capacity ()		const	{ return C; }
		usize			MaxCapacity ()	const	{ return Capacity(); }	// max available for allocation count of elements
		Bytes<usize>	Size ()			const	{ return Bytes<usize>( Count() * sizeof(T) ); }
		Bytes<usize>	FullSize ()		const	{ return Bytes<usize>( Capacity() * sizeof(T) ); }
		usize			LastIndex ()	const	{ return Count()-1; }
	
		Buffer<T>		SubArray (usize pos, usize count = usize(-1))			{ return Buffer<T>(*this).SubArray( pos, count ); }
		Buffer<const T>	SubArray (usize pos, usize count = usize(-1))	const	{ return Buffer<const T>(*this).SubArray( pos, count ); }


		iterator		Begin ()				{ return _memory; }
		const_iterator	Begin () const			{ return _memory; }

		iterator		End ()					{ return _memory + Count(); }
		const_iterator	End () const			{ return _memory + Count(); }

		iterator		GetIter (usize index);
		const_iterator	GetIter (usize index) const;

		bool IsBegin (const_iterator iter)	const;
		bool IsEnd (const_iterator iter)	const;
		

		static constexpr bool	IsLinearMemory ()	{ return true; }

		friend void SwapValues (INOUT Self &left, INOUT Self &right)
		{
			for (usize i = 0; i < Count(); ++i) {
				SwapValues( left[i], right[i] );
			}
		}
	};


	
	template <typename T, usize C, typename S>
	inline StaticArray<T,C,S>::StaticArray (Uninitialized)
	{
		Strategy::Create( _memory, Count() );
	}
		

	template <typename T, usize C, typename S>
	inline StaticArray<T,C,S>::StaticArray (const StaticArray<T,C,S> &other)
	{
		Strategy::Copy( _memory, other._memory, Count() );
	}
	
		
	template <typename T, usize C, typename S>
	inline StaticArray<T,C,S>::StaticArray (Buffer<const T> other)
	{
		Copy( other );
	}
	
	
	template <typename T, usize C, typename S>
	inline void StaticArray<T,C,S>::Copy (Buffer<const T> other)
	{
		if ( other.Empty() )
		{
			Free();
			return;
		}

		if ( CheckPointersAliasing( Begin(), End(), other.Begin(), other.End() ) )
			RET_VOID;

		if ( other.Count() > Count() )
		{
			WARNING( "source array is too big!" );
			Strategy::Copy( _memory, other.ptr(), Count() );
		}
		else
		{
			Strategy::Copy( _memory, other.ptr(), other.Count());
			Strategy::Create( _memory + other.Count(), Count() - other.Count() );
		}
	}

		
	template <typename T, usize C, typename S>
	inline StaticArray<T,C,S>::StaticArray (const T& value)
	{
		for (usize i = 0; i < Count(); ++i) {
			Strategy::Copy( _memory + i, &value, 1 );
		}
	}
		
	template <typename T, usize C, typename S>
	inline T & StaticArray<T,C,S>::operator [] (usize index)
	{
		ASSUME( index < Count() );
		return _memory[ index ];
	}

		
	template <typename T, usize C, typename S>
	inline const T & StaticArray<T,C,S>::operator [] (usize index) const
	{
		ASSUME( index < Count() );
		return _memory[ index ];
	}
	
	
	template <typename T, usize C, typename S>
	inline bool StaticArray<T,C,S>::At (usize index, T & value) const
	{
		if ( index >= Count() )  return false;
		Strategy::Copy( &value, &_memory + index, 1 );
		return true;
	}

	
	template <typename T, usize C, typename S>
	inline bool StaticArray<T,C,S>::Set (usize index, const T &value)
	{
		if ( index >= Count() ) return false;
		Strategy::Copy( _memory + index, &value, 1 );
		return true;
	}
	

	template <typename T, usize C, typename S>
	inline bool StaticArray<T,C,S>::Set (usize index, T&& value)
	{
		if ( index >= Count() ) return false;
		Strategy::Move( _memory + index, &value, 1 );
		return true;
	}

		
	template <typename T, usize C, typename S>
	inline void StaticArray<T,C,S>::Free ()
	{
		Clear();
	}

		
	template <typename T, usize C, typename S>
	inline void StaticArray<T,C,S>::Clear ()
	{
		Strategy::Destroy( _memory, Count() );
		Strategy::Create(  _memory, Count() );
	}

		
	template <typename T, usize C, typename S>
	inline void StaticArray<T,C,S>::Swap (usize first, usize second)
	{
		if ( first >= Count() or second >= Count() )
			RET_VOID;

		T	temp;
		Strategy::Replace( &temp,				_memory + second,	1 );
		Strategy::Replace( _memory + second,	_memory + first,	1 );
		Strategy::Replace( _memory + first,		&temp,				1 );
	}

		
	template <typename T, usize C, typename S>
	inline T * StaticArray<T,C,S>::GetIter (usize index)
	{
		ASSERT( index < Count() );
		return _memory + index;
	}

		
	template <typename T, usize C, typename S>
	inline const T * StaticArray<T,C,S>::GetIter (usize index) const
	{
		ASSERT( index < Count() );
		return _memory + index;
	}

		
	template <typename T, usize C, typename S>
	inline bool StaticArray<T,C,S>::IsBegin (const_iterator iter) const
	{
		return iter == Begin();
	}


	template <typename T, usize C, typename S>
	inline bool StaticArray<T,C,S>::IsEnd (const_iterator iter) const
	{
		return iter == End();
	}


	#undef RET_ERROR
	#undef RET_FALSE
	#undef RET_VOID
	
	
	template <typename T, usize C, typename S>
	struct Hash< StaticArray<T,C,S> > :
		private Hash< Buffer<const T> >
	{
		typedef StaticArray<T,C,S>			key_t;
		typedef Hash< Buffer<const T> >		base_t;
		typedef typename base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x );
		}
	};


	// Make Array Function Helpers
#	define _MAKE_ARRAY( _count_, _params_, _args_ ) \
		template <typename T> \
		static inline StaticArray< T, _count_ > Create ( _params_ ) \
		{ \
			const T	tmp[] = { _args_ }; \
			STATIC_ASSERT( COUNT_OF(tmp) == _count_ ); \
			StaticArray< T, _count_ > res( tmp ); \
			return res; \
		}

	struct StaticArrayBuilder
	{
		_MAKE_ARRAY(  1,
					ARGS( T a00 ),
					ARGS( a00 )
		)
		_MAKE_ARRAY(  2,
					ARGS( T a00, T a01 ),
					ARGS( a00, a01 )
		)
		_MAKE_ARRAY(  3,
					ARGS( T a00, T a01, T a02 ),
					ARGS( a00, a01, a02 )
		)
		_MAKE_ARRAY(  4,
					ARGS( T a00, T a01, T a02, T a03 ),
					ARGS( a00, a01, a02, a03 )
		)
		_MAKE_ARRAY(  5,
					ARGS( T a00, T a01, T a02, T a03, T a04 ),
					ARGS( a00, a01, a02, a03, a04 )
		)
		_MAKE_ARRAY(  6,
					ARGS( T a00, T a01, T a02, T a03, T a04, T a05 ),
					ARGS( a00, a01, a02, a03, a04, a05 )
		)
		_MAKE_ARRAY(  7,
					ARGS( T a00, T a01, T a02, T a03, T a04, T a05, T a06 ),
					ARGS( a00, a01, a02, a03, a04, a05, a06 )
		)
		_MAKE_ARRAY(  8,
					ARGS( T a00, T a01, T a02, T a03, T a04, T a05, T a06, T a07 ),
					ARGS( a00, a01, a02, a03, a04, a05, a06, a07 )
		)
		_MAKE_ARRAY(  9,
					ARGS( T a00, T a01, T a02, T a03, T a04, T a05, T a06, T a07, T a08 ),
					ARGS( a00, a01, a02, a03, a04, a05, a06, a07, a08 )
		)
		_MAKE_ARRAY( 10,
					ARGS( T a00, T a01, T a02, T a03, T a04, T a05, T a06, T a07, T a08, T a09 ),
					ARGS( a00, a01, a02, a03, a04, a05, a06, a07, a08, a09 )
		)
		_MAKE_ARRAY( 11,
					ARGS( T a00, T a01, T a02, T a03, T a04, T a05, T a06, T a07, T a08, T a09, T a10 ),
					ARGS( a00, a01, a02, a03, a04, a05, a06, a07, a08, a09, a10 )
		)
		_MAKE_ARRAY( 12,
					ARGS( T a00, T a01, T a02, T a03, T a04, T a05, T a06, T a07, T a08, T a09, T a10, T a11 ),
					ARGS( a00, a01, a02, a03, a04, a05, a06, a07, a08, a09, a10, a11 )
		)
		_MAKE_ARRAY( 13,
					ARGS( T a00, T a01, T a02, T a03, T a04, T a05, T a06, T a07, T a08, T a09, T a10, T a11, T a12 ),
					ARGS( a00, a01, a02, a03, a04, a05, a06, a07, a08, a09, a10, a11, a12 )
		)
		_MAKE_ARRAY( 14,
					ARGS( T a00, T a01, T a02, T a03, T a04, T a05, T a06, T a07, T a08, T a09, T a10, T a11, T a12, T a13 ),
					ARGS( a00, a01, a02, a03, a04, a05, a06, a07, a08, a09, a10, a11, a12, a13 )
		)
		_MAKE_ARRAY( 15,
					ARGS( T a00, T a01, T a02, T a03, T a04, T a05, T a06, T a07, T a08, T a09, T a10, T a11, T a12, T a13, T a14 ),
					ARGS( a00, a01, a02, a03, a04, a05, a06, a07, a08, a09, a10, a11, a12, a13, a14 )
		)
	};

#	undef _MAKE_ARRAY


}	// GXTypes
}	// GX_STL
