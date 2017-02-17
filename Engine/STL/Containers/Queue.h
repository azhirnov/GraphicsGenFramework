﻿// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Array.h"

namespace GX_STL
{
namespace GXTypes
{

	#define RET_ERROR( _ret_ )	{ WARNING("error in Queue");  return _ret_; }
	#define RET_FALSE			RET_ERROR( false )
	#define RET_VOID			RET_ERROR( ; )



	//
	// Queue
	//

	template <	typename T,
				typename S = typename AutoDetectCopyStrategy<T>::type,
				typename MC = MemoryContainer<T>
			 >
	struct Queue : public CompileTime::CopyQualifiers< CompileTime::FastCopyable, MC >
	{
	// types
	public:
		typedef S				Strategy;
		typedef MC				MemoryContainer_t;
		typedef T				value_t;
		typedef Queue<T,S,MC>	Self;
		

	// variables
	private:
		MemoryContainer_t	_memory;
		isize				_first,
							_last;
		usize				_size;

		//  __________ _____________ __________
		// | reserved | used memory | reserved |
		// 0       _first       _last      _size
		

	// methods
	private:
		void _Reallocate (usize newSize);
		void _ReplaceToLeft (usize offset = 1);
		void _ReplaceToRight (usize offset = 1);
		void _Move (Self &&other);
		
		static usize _CalcFirstOffset (usize size);
		static bool  _CheckIntersection (const void *leftBegin, const void *leftEnd,
										 const void *rightBegin, const void *rightEnd);

	public:
		Queue (GX_DEFCTOR);

		Queue (const Self &other);

		Queue (Self &&other);

		Queue (Buffer<const T> other);

		~Queue ()														{ Free(); }

		T		*	ptr ();
		T const	*	ptr () const;

		T		&	Front ();
		T const	&	Front () const;

		T		&	Back ();
		T const	&	Back () const;

		bool		operator ! () const									{ return not Empty(); }

		T		&	operator [] (usize i);
		T const	&	operator [] (usize i) const;

		Self &		operator =  (Buffer<const T> right)					{ Copy( right );				return *this; }
		Self &		operator =  (const Self &right)						{ Copy( right );				return *this; }
		Self &		operator =  (Self &&right)							{ _Move( RVREF( right ) );		return *this; }

		Self &		operator << (const T& right)						{ PushBack( right );			return *this; }
		Self &		operator << (Self &&right)							{ AppendBack( RVREF( right ) );	return *this; }
		Self &		operator << (Buffer<const T> right)					{ AppendBack( right );			return *this; }

		friend Self &	operator >> (const T& left, Self &right)		{ right.PushFront( left );				return right; }
		friend Self &	operator >> (Self &&left, Self &right)			{ right.AppendFront( RVREF( left ) );	return right; }
		friend Self &	operator >> (Buffer<const T> left, Self &right)	{ right.AppendFront( left );			return right; }
		
		bool			operator == (Buffer<const T> right)	const		{ return Buffer<const T>(*this) == right; }
		bool			operator != (Buffer<const T> right)	const		{ return not ( (*this) == right ); }

		void PushBack (const T &value);
		void PushBack (T&& value);

		void PushFront (const T &value);
		void PushFront (T&& value);

		void PopBack ();
		void PopFront ();

		void Resize (usize newSize);
		void Reserve (usize size);

		void Free ();
		void Clear ();

		void Copy (Buffer<const T> other);

		void AppendFront (Buffer<const T> other);
		void AppendFront (Self &&other);
		
		void AppendBack (Buffer<const T> other);
		void AppendBack (Self &&other);

		void Erase (usize pos, usize count);
		void EraseFromFront (usize count);
		void EraseFromBack (usize count);

		usize	Count ()		const				{ return Empty() ? 0 : _last - _first + 1; }
		BytesU	Size ()			const				{ return BytesU( sizeof(T) * Count() ); }
		usize	Capacity ()		const				{ return _size; }
		usize	MaxCapacity ()	const				{ return _memory.MaxSize(); }	// max available for allocation count of elements
		BytesU	FullSize ()		const				{ return BytesU( sizeof(T) * Capacity() ); }
		bool	Empty ()		const				{ return _first > _last; }
		usize	LastIndex ()	const				{ return Count()-1; }
		

		static constexpr bool	IsLinearMemory ()	{ return true; }

		friend void SwapValues (INOUT Self &left, INOUT Self &right)
		{
			left._memory.SwapMemory( right._memory );
		
			SwapValues( left._first,	right._first );
			SwapValues( left._last,		right._last );
			SwapValues( left._size,		right._size );
		}
	};
	
	

	template <typename T, usize Size>
	using FixedSizeQueue = Queue< T, typename AutoDetectCopyStrategy<T>::type, StaticMemoryContainer<T, Size> >;


	
/*
=================================================
	constructor
=================================================
*/
	template <typename T, typename S, typename MC>
	inline Queue<T,S,MC>::Queue (UninitializedType) : _size(0), _first(0), _last(-1) {}
		
/*
=================================================
	constructor
=================================================
*/
	template <typename T, typename S, typename MC>
	inline Queue<T,S,MC>::Queue (const Self &other) : _size(0), _first(0), _last(-1)
	{
		Copy( other );
	}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T, typename S, typename MC>
	inline Queue<T,S,MC>::Queue (Self &&other)
	{
		_Move( RVREF( other ) );
	}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T, typename S, typename MC>
	inline Queue<T,S,MC>::Queue (Buffer<const T> other) : _size(0), _first(0), _last(-1)
	{
		Copy( other );
	}

/*
=================================================
	_Move
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::_Move (Self &&other)
	{
		_memory	= RVREF( other._memory );
		_size	= other._size;
		_first	= other._first;
		_last	= other._last;

		other._size = other._first = other._last = 0;
	}
	
/*
=================================================
	_Reallocate
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::_Reallocate (usize newSize)
	{
		ASSERT( newSize > 0 );

		usize				old_first	= _first,
							old_count	= Count(),
							new_count	= newSize < old_count ? newSize : old_count;

		MemoryContainer_t	old_memcont;	old_memcont.MoveFrom( INOUT _memory );

		_size	= newSize;
		CHECK( _memory.Allocate( INOUT _size ) );

		new_count = GXMath::Min( new_count, _size );

		_first	= _CalcFirstOffset( _size );
		_last	= _first + new_count - 1;

		if ( old_memcont.Pointer() == null or old_count == 0 )
			return;

		Strategy::Replace( _memory.Pointer() + _first, old_memcont.Pointer() + old_first, new_count );

		if ( new_count < old_count )
			Strategy::Destroy( old_memcont.Pointer() + old_first + old_count, old_count - new_count );
	}
	
/*
=================================================
	_CalcFirstOffset
=================================================
*/
	template <typename T, typename S, typename MC>
	inline usize Queue<T,S,MC>::_CalcFirstOffset (usize size)
	{
		const usize	nom	= GlobalConst::STL_MemContainerResizingNominator;
		const usize	den	= GlobalConst::STL_MemContainerResizingDenominator;
		return (size * nom + den*2 - 1) / (den * 2);
	}
	
/*
=================================================
	_CheckIntersection
=================================================
*/
	template <typename T, typename S, typename MC>
	inline bool Queue<T,S,MC>::_CheckIntersection (const void *leftBegin, const void *leftEnd,
													const void *rightBegin, const void *rightEnd)
	{
		return CheckPointersAliasing( leftBegin, leftEnd, rightBegin, rightEnd );
	}
	
/*
=================================================
	_ReplaceToLeft
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::_ReplaceToLeft (usize offset)
	{
		if ( _last + (isize)offset < (isize)_size ) {
			_last += offset;
			return;
		}
		
		const isize	free_space	= isize(Capacity() - Count() - offset) / 2;
		const isize	min_size	= GlobalConst::STL_MemContainerResizingMinSize;

		if ( free_space > min_size )
		{
			const isize	new_first = free_space;

			for (usize i = 0; i < Count(); ++i) {
				Strategy::Replace( _memory.Pointer() + new_first + i, _memory.Pointer() + _first + i, 1 );
			}

			_last -= _first - new_first - offset;
			_first = new_first;
			return;
		}

		_last += offset;
		_Reallocate( _size + offset );
	}
	
/*
=================================================
	_ReplaceToRight
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::_ReplaceToRight (usize offset)
	{
		if ( _first > isize(offset + 1) ) {
			_first -= offset;
			return;
		}
		
		const isize	free_space	= isize(Capacity() - Count() - offset) / 2;
		const isize	min_size	= GlobalConst::STL_MemContainerResizingMinSize;
		
		if ( free_space > min_size )
		{
			const isize	new_first = offset + free_space;

			for (isize i = Count()-1; i >= 0; --i) {
				Strategy::Replace( _memory.Pointer() + new_first + i, _memory.Pointer() + _first + i, 1 );
			}

			_last += new_first - _first;
			_first = new_first - offset;
			return;
		}

		_first -= offset;
		_Reallocate( _size + offset );
	}
	
/*
=================================================
	operator []
=================================================
*/
	template <typename T, typename S, typename MC>
	inline T & Queue<T,S,MC>::operator [] (usize i)
	{
		ASSERT( i < Count() );
		return _memory.Pointer()[ i + _first ];
	}
	
	template <typename T, typename S, typename MC>
	inline const T & Queue<T,S,MC>::operator [] (usize i) const
	{
		ASSERT( i < Count() );
		return _memory.Pointer()[ i + _first ];
	}
	
/*
=================================================
	ptr
=================================================
*/
	template <typename T, typename S, typename MC>
	inline T * Queue<T,S,MC>::ptr ()
	{
		ASSUME( _memory.Pointer() != null and _first < (isize)_size );
		return _memory.Pointer() + _first;
	}
	
	template <typename T, typename S, typename MC>
	inline const T * Queue<T,S,MC>::ptr () const
	{
		ASSUME( _memory.Pointer() != null and _first < (isize)_size );
		return _memory.Pointer() + _first;
	}
	
/*
=================================================
	Front
=================================================
*/
	template <typename T, typename S, typename MC>
	inline T & Queue<T,S,MC>::Front ()
	{
		ASSUME( _memory.Pointer() != null and _first < (isize)_size );
		return _memory.Pointer()[_first];
	}
	
	template <typename T, typename S, typename MC>
	inline const T & Queue<T,S,MC>::Front () const
	{
		ASSUME( _memory.Pointer() != null and _first < (isize)_size );
		return _memory.Pointer()[_first];
	}
	
/*
=================================================
	Back
=================================================
*/
	template <typename T, typename S, typename MC>
	inline T & Queue<T,S,MC>::Back ()
	{
		ASSUME( _memory.Pointer() != null and _last < (isize)_size );
		return _memory.Pointer()[_last];
	}
	
	template <typename T, typename S, typename MC>
	inline const T & Queue<T,S,MC>::Back () const
	{
		ASSUME( _memory.Pointer() != null and _last < (isize)_size );
		return _memory.Pointer()[_last];
	}
	
/*
=================================================
	PushBack
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::PushBack (const T &value)
	{
		_ReplaceToLeft();
		Strategy::Copy( _memory.Pointer() + _last, &value, 1 );
	}
	
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::PushBack (T &&value)
	{
		_ReplaceToLeft();
		Strategy::Move( _memory.Pointer() + _last, &value, 1 );
	}
	
/*
=================================================
	PushFront
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::PushFront (const T &value)
	{
		_ReplaceToRight();
		Strategy::Copy( _memory.Pointer() + _first, &value, 1 );
	}

	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::PushFront (T &&value)
	{
		_ReplaceToRight();
		Strategy::Move( _memory.Pointer() + _first, &value, 1 );
	}
	
/*
=================================================
	PopBack
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::PopBack ()
	{
		if ( _first <= _last ) {
			Strategy::Destroy( _memory.Pointer() + _last, 1 );
			--_last;
		}
	}
	
/*
=================================================
	PopFront
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::PopFront ()
	{
		if ( _first <= _last ) {
			Strategy::Destroy( _memory.Pointer() + _first, 1 );
			++_first;
		}
	}
	
/*
=================================================
	Resize
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::Resize (usize newSize)
	{
		const usize	old_count = Count();

		if ( newSize == old_count )
			return;

		if ( newSize == 0 )
		{
			Clear();
			return;
		}
		
		if ( newSize < old_count )
		{
			Strategy::Destroy( _memory.Pointer() + _first + newSize, old_count - newSize );
			_last = _first + newSize-1;
			return;
		}

		_Reallocate( newSize );
		Strategy::Create( _memory.Pointer() + _first + old_count, newSize - old_count );
		_last = _first + newSize-1;
	}
	
/*
=================================================
	Reserve
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::Reserve (usize size)
	{
		if ( size > _memory.MaxSize() )
			size = _memory.MaxSize();

		if ( size <= _size )
			return;

		_Reallocate( size );
	}
	
/*
=================================================
	Free
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::Free ()
	{
		if ( _memory.Pointer() != null )
		{
			Strategy::Destroy( _memory.Pointer() + _first, Count() );
			_memory.Deallocate();
		}
		_first	= 0;
		_size	= 0;
		_last	= -1;
	}
	
/*
=================================================
	Clear
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::Clear ()
	{
		if ( _memory.Pointer() != null ) {
			Strategy::Destroy( _memory.Pointer() + _first, Count() );
		}
		_first	= _CalcFirstOffset( _size );
		_last	= _first-1;
	}
	
/*
=================================================
	Copy
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::Copy (Buffer<const T> other)
	{
		Clear();
		
		if ( arrayPtr == null or count == 0 )
			return;

		if ( not Empty() and _CheckIntersection( _memory.Pointer(), _memory.Pointer() + _size, arrayPtr, arrayPtr + count ) )
			RET_VOID;

		if ( count > _size )
		{
			_size = count;
			_memory.Deallocate();
			CHECK( _memory.Allocate( INOUT _size ) );
		}

		_first  = _CalcFirstOffset( _size );
		_last	 = GXMath::Min( _first + count, _size );

		Strategy::Copy( _memory.Pointer() + _first, arrayPtr, Count() );
	}
		
/*
=================================================
	AppendFront
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::AppendFront (Buffer<const T> other)
	{
		if ( other.Empty() )
			RET_VOID;

		_ReplaceToRight( other.Count() );
		Strategy::Copy( _memory.Pointer() + _first, other.ptr(), other.Count() );
	}
		
/*
=================================================
	AppendFront
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::AppendFront (Self &&other)
	{
		if ( other.Empty() )
			RET_VOID;

		_ReplaceToRight( other.Count() );
		Strategy::Move( _memory.Pointer() + _first, other.ptr(), other.Count() );
	}
		
/*
=================================================
	AppendBack
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::AppendBack (Buffer<const T> other)
	{
		if ( other.Empty() )
			return;

		_ReplaceToLeft( other.Count() );
		Strategy::Copy( _memory.Pointer() + _last - other.Count() + 1, other.ptr(), other.Count() );
	}
		
/*
=================================================
	AppendBack
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::AppendBack (Self &&other)
	{
		if ( other.Empty() )
			return;

		_ReplaceToLeft( other.Count() );
		Strategy::Move( _memory.Pointer() + _last - other.Count() + 1, other.ptr(), other.Count() );
	}
	
/*
=================================================
	Erase
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::Erase (usize pos, usize count)
	{
		const usize		u_count = Count();

		if ( pos >= u_count or count == 0 or u_count == 0 )
			RET_VOID;

		if ( pos + count >= u_count )
			count = u_count - pos;

		if ( pos <= (u_count - pos - count) )
		{
			// erase from left
			Strategy::Destroy( _memory.Pointer() + _first + pos, count );
			Strategy::Replace( _memory.Pointer() + _first + pos, _memory.Pointer() + _first, pos );
			_first += count;
		}
		else
		{
			// erase from right
			Strategy::Destroy( _memory.Pointer() + _first + pos, count );
			Strategy::Replace( _memory.Pointer() + _first + pos, _memory.Pointer() + _last - pos - count, u_count - pos - count );
			_last -= count;
		}
	}
	
/*
=================================================
	EraseFromFront
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::EraseFromFront (usize count)
	{
		return Erase( 0, count );
	}
	
/*
=================================================
	EraseFromBack
=================================================
*/
	template <typename T, typename S, typename MC>
	inline void Queue<T,S,MC>::EraseFromBack (usize count)
	{
		ASSERT( count < Count() );
		return Erase( Count() - count, count );
	}

	#undef  RET_ERROR
	#undef  RET_FALSE
	#undef  RET_VOID

	
/*
=================================================
	Hash
=================================================
*/
	template <typename T, typename S, typename MC>
	struct Hash< Queue<T,S,MC> > :
		private Hash< Buffer<const T> >
	{
		typedef Queue<T,S,MC>				key_t;
		typedef Hash< Buffer<const T> >		base_t;
		typedef typename base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x );
		}
	};

}	// ECType
}	// GX_STL
