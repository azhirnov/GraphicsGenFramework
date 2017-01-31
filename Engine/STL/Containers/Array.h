﻿// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/STL/Memory/MemoryContainer.h"
#include "CopyStrategy.h"
#include "String.h"
#include "Engine/STL/Math/Mathematics.h"

namespace GX_STL
{
namespace GXTypes
{

	#define RET_ERROR( _ret_ )	{ WARNING("error in array");  return _ret_; }
	#define RET_FALSE			RET_ERROR( false )
	#define RET_VOID			RET_ERROR( ; )
	


	//
	// Array
	//

	template <	typename T,
				typename S = typename AutoDetectCopyStrategy<T>::type,
				typename MC = MemoryContainer<T>
			 >
	struct Array : public CompileTime::CopyQualifiers< CompileTime::FastCopyable, MC >
	{
	// types
	public:
		typedef Array<T,S,MC>		Self;
		typedef S					Strategy;
		typedef MC					MemoryContainer_t;
		typedef T					value_t;
		typedef	T *					iterator;
		typedef const T *			const_iterator;


	// variables
	private:
		MemoryContainer_t	_memory;
		usize				_count;
		usize				_size;

		//  _____________ _________________
		// | used memory | reserved memory |
		// 0          _count            _size
		

	// methods
	private:
		void _Reallocate (usize newSize, bool allowReserve = true);
		void _Move (Self &&other);

		template <typename B>	void _Insert (B *pArray, usize count, usize pos);
		
		static bool  _CheckIntersection (const void *leftBegin, const void *leftEnd,
										 const void *rightBegin, const void *rightEnd);

	public:
		Array (GX_DEFCTOR);

		Array (const Self &other);

		Array (Self &&other);

		Array (Buffer<const T> other);

		~Array ()														{ Free(); }

		T		*		ptr ();
		T const	*		ptr () const;

		T const *		RawPtr () const									{ return _memory.Pointer(); }

		T		&		Back ();
		T const	&		Back () const;
		T		&		Front ();
		T const	&		Front () const;
		
		bool			operator ! () const								{ return not Empty(); }

		T		&		operator [] (usize i);
		T const	&		operator [] (usize i) const;

		Self &			operator << (const T &value)					{ PushBack( value );		return *this; }
		Self &			operator << (Buffer<const T> other)				{ Append( other );			return *this; }
		Self &			operator << (T &&value)							{ PushBack( RVREF(value) );	return *this; }
		Self &			operator << (Self &&other)						{ Append( RVREF(other) );	return *this; }

		friend Self &	operator >> (const T &left, Self &right)		{ right.PushFront( left );  return right; }
		friend Self &	operator >> (Buffer<const T> left, Self &right)	{ right.Insert( left, 0 );  return right; }
		friend Self &	operator >> (T &&left, Self &right)				{ right.PushFront( RVREF(left) );  return right; }
		friend Self &	operator >> (Self &&left, Self &right)			{ right.Insert( RVREF(left), 0 );  return right; }
		
		Self &			operator =  (Buffer<const T> right)				{ Copy( right );			return *this; }
		Self &			operator =  (const Self &right)					{ Copy( right );			return *this; }
		Self &			operator =  (Self &&right)						{ _Move( RVREF(right) );	return *this; }

		bool			operator == (Buffer<const T> right)	const		{ return Buffer<const T>(*this) == right; }
		bool			operator != (Buffer<const T> right)	const		{ return not ( (*this) == right ); }

		
		bool At (usize index, OUT T & value) const;

		void Append (Buffer<const T> other)							{ _Insert<const T>( other.RawPtr(), other.Count(), Count() ); }
		void Append (const Self &other)								{ Append( (Buffer<const T>) other ); }
		void Append (Self &&other)									{ _Insert<T>( other._memory.Pointer(), other.Count(), Count() ); }
		
		void AppendFront (Buffer<const T> other)					{ Insert( other, 0 ); }
		void AppendFront (const Self &other)						{ AppendFront( (Buffer<const T>) other ); }
		void AppendFront (Self &&other)								{ _Insert<T>( other._memory.Pointer(), other.Count(), Count() ); }

		void AppendBack (Buffer<const T> other)						{ Append( other ); }
		void AppendBack (const Self &other)							{ Append( other ); }
		void AppendBack (Self &&other)								{ Append( RVREF( other ) ); }

		void Copy (Buffer<const T> other)							{ Clear();  Append( other ); }
		void Copy (const Self &other)								{ Clear();  Append( other ); }

		void Free ();
		void FreeReserve ();

		void Clear ();

		void Erase (usize pos, usize count = 1);
		void EraseFromBack (usize count);

		// breake order
		void FastErase (usize pos, usize count);
		void FastErase (usize pos);

		template <typename E>
		bool Find (OUT usize &index, const E &value, usize start = 0) const	{ return Buffer<const T>(*this).Find( index, value, start ); }

		template <typename E>
		bool IsExist (const E &value) const							{ return Buffer<const T>(*this).IsExist( value ); }

		template <typename E>
		bool FindAndErase (const E &value);

		template <typename E>
		bool FindAndFastErase (const E &value);

		void Insert (const T& value, usize pos)						{ _Insert<const T>( &value, 1, pos ); }
		void Insert (T&& value, usize pos)							{ _Insert<T>( &value, 1, pos ); }

		void Insert (Buffer<const T> other, usize pos)				{ _Insert<const T>( other.RawPtr(), other.Count(), pos ); }
		void Insert (const Self &other, usize pos)					{ Insert( (Buffer<const T>) other, pos ); }
		void Insert (Self &&other, usize pos)						{ _Insert<T>( other._memory.Pointer(), other.Count(), Count() ); }

		void PushBack (const T& value);
		void PushBack (T&& value);
		T&   PushBack ()											{ PushBack( T() );  return Back(); }

		void PushFront (const T& value)								{ _Insert<const T>( &value, 1, 0 ); }
		void PushFront (T&& value)									{ _Insert<T>( &value, 1, 0 ); }
		T&   PushFront ()											{ PushFront( T() );  return Front(); }
		
		void PopBack ();
		void PopFront ();
		
		void Resize (usize newSize, bool allowReserve = true);
		
		void Reserve (usize size);
		
		bool Set (usize index, const T &value);
		bool Set (usize i, T &&value);
		
		void Swap (usize first, usize second);

		Buffer<T>		SubArray (usize pos, usize count = usize(-1))			{ return Buffer<T>(*this).SubArray( pos, count ); }
		Buffer<const T>	SubArray (usize pos, usize count = usize(-1))	const	{ return Buffer<const T>(*this).SubArray( pos, count ); }

		template <typename T2, typename S2, typename A2>
		void Convert (OUT Array<T2,S2,A2> &other) const;

		usize GetIndex (const T &value)	const						{ return Buffer<const T>(*this).GetIndex( value ); }
		bool  IsInArray (const T &value) const						{ return Buffer<const T>(*this).IsInArray( value ); }

		bool			Empty ()		const	{ return _count == 0; }
		usize			Count ()		const	{ return _count; }
		usize			Capacity ()		const	{ return _size; }
		usize			MaxCapacity ()	const	{ return _memory.MaxSize(); }	// max available for allocation count of elements
		BytesU			Size ()			const	{ return BytesU( Count() * sizeof(T) ); }
		BytesU			FullSize ()		const	{ return BytesU( Capacity() * sizeof(T) ); }
		usize			LastIndex ()	const	{ return Count()-1; }

		iterator		Begin ();
		const_iterator	Begin ()			const;

		iterator		End ();
		const_iterator	End ()				const;

		iterator		GetIter (usize i);
		const_iterator	GetIter (usize i)	const;

		bool IsBegin (const_iterator iter)	const;
		bool IsEnd (const_iterator iter)	const;


		static constexpr bool	IsLinearMemory ()	{ return true; }

		friend void SwapValues (INOUT Self &left, INOUT Self &right)
		{
			left._memory.SwapMemory( right._memory );

			SwapValues( left._count,	right._count );
			SwapValues( left._size,		right._size );
		}
	};
	

	typedef Array< ubyte >		BinaryArray;


	template <typename T, usize Size>
	using FixedSizeArray = Array< T, typename AutoDetectCopyStrategy<T>::type, StaticMemoryContainer<T, Size> >;
	
	
	template <typename T>
	template <typename B, typename S, typename MC>
	inline Buffer<T>  Buffer<T>::From (const Array<B,S,MC> &arr)
	{
		Buffer<const B> buf = arr;
		return From<const B>( buf );
	}


	template <typename T, typename S, typename MC>
	inline Array<T,S,MC>::Array (UninitializedType): _count(0), _size(0)
	{}
	
		
	template <typename T, typename S, typename MC>
	inline Array<T,S,MC>::Array (const Self &other): _count(0), _size(0)
	{
		Copy( other );
	}
	

	template <typename T, typename S, typename MC>
	inline Array<T,S,MC>::Array (Self &&other)
	{
		_Move( RVREF(other) );
	}
	

	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::_Move (Self &&other)
	{
		_memory	= RVREF( other._memory );
		_count	= other._count;
		_size	= other._size;

		other._count = other._size = 0;
	}
	

	template <typename T, typename S, typename MC>
	inline Array<T,S,MC>::Array (Buffer<const T> other): _count(0), _size(0)
	{
		Copy( other );
	}


	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::_Reallocate (usize newSize, bool allowReserve)
	{
		ASSERT( newSize > 0 );

		MemoryContainer_t	old_memcont;	old_memcont.MoveFrom( INOUT _memory );
		_size = newSize;

		CHECK( _memory.Allocate( INOUT _size, allowReserve ) );
		newSize = GXMath::Min( newSize, _size );

		const usize	new_count = _count < newSize ? _count : newSize;

		if ( old_memcont.Pointer() == null or _count == 0 ) {
			_count = 0;
			return;
		}

		Strategy::Replace( _memory.Pointer(), old_memcont.Pointer(), new_count );

		if ( new_count < _count )
			Strategy::Destroy( old_memcont.Pointer() + new_count, _count - new_count );

		_count = new_count;
	}
	

	template <typename T, typename S, typename MC>
	inline bool Array<T,S,MC>::_CheckIntersection (const void *leftBegin, const void *leftEnd,
													const void *rightBegin, const void *rightEnd)
	{
		return CheckPointersAliasing( leftBegin, leftEnd, rightBegin, rightEnd );
	}


	template <typename T, typename S, typename MC>
	template <typename B>
	inline void Array<T,S,MC>::_Insert (B *pArray, const usize count, const usize pos)
	{
		if ( pArray == null or count == 0 )
			return;

		if ( pos > _count or (not Empty() and _CheckIntersection( Begin(), End(), pArray, pArray + count )) )
			RET_VOID;
		
		_count += count;
		
		const usize	num_move = _count - pos - count;

		// initialize
		if ( _size == 0 )
		{
			_size = _count;
			CHECK( _memory.Allocate( INOUT _size ) );
			_count = GXMath::Min( _count, _size );
		}
		else
		// reallocate
		if ( _count > _size )
		{
			MemoryContainer_t	old_memcont;	old_memcont.MoveFrom( INOUT _memory );

			_size = _count;
			CHECK( _memory.Allocate( INOUT _size ) );
			_count = GXMath::Min( _count, _size );

			Strategy::Replace( _memory.Pointer(), old_memcont.Pointer(), pos );
			Strategy::Replace( _memory.Pointer() + pos + count, old_memcont.Pointer() + pos, num_move );
		}
		else
		// move
		{
			// if not intersected
			if ( num_move <= count ) {
				Strategy::Replace( _memory.Pointer() + pos + count, _memory.Pointer() + pos, num_move );
			}
			else {
				for (usize i = 0; i < num_move; ++i) {
					Strategy::Replace( _memory.Pointer() + _count-1 - i, _memory.Pointer() + _count-1 - count - i, 1 );
				}
			}
		}

		if ( TypeTraits::IsConst<B> )
			Strategy::Copy( _memory.Pointer() + pos, pArray, count );
		else
			Strategy::Move( _memory.Pointer() + pos, (T *) pArray, count );
	}
	

	template <typename T, typename S, typename MC>
	inline bool Array<T,S,MC>::At (usize index, T & value) const
	{
		if ( index >= Count() )  return false;
		Strategy::Copy( &value, _memory.Pointer() + index, 1 );
		return true;
	}

	
	template <typename T, typename S, typename MC>
	inline bool Array<T,S,MC>::Set (usize index, const T &value)
	{
		if ( index >= Count() ) return false;
		Strategy::Copy( _memory.Pointer() + index, &value, 1 );
		return true;
	}
	

	template <typename T, typename S, typename MC>
	inline bool Array<T,S,MC>::Set (usize index, T&& value)
	{
		if ( index >= Count() ) return false;
		Strategy::Move( _memory.Pointer() + index, &value, 1 );
		return true;
	}

	
	template <typename T, typename S, typename MC>
	inline T * Array<T,S,MC>::ptr ()
	{
		ASSUME( _memory.Pointer() != null );
		return _memory.Pointer();
	}
	

	template <typename T, typename S, typename MC>
	inline const T * Array<T,S,MC>::ptr () const
	{
		ASSUME( _memory.Pointer() != null );
		return _memory.Pointer();
	}

	
	template <typename T, typename S, typename MC>
	inline T & Array<T,S,MC>::operator [] (usize index)
	{
		ASSUME( index < _count );
		return _memory.Pointer()[index];
	}
	

	template <typename T, typename S, typename MC>
	inline const T & Array<T,S,MC>::operator [] (usize index) const
	{
		ASSUME( index < _count );
		return _memory.Pointer()[index];
	}
	

	template <typename T, typename S, typename MC>
	inline T & Array<T,S,MC>::Back ()
	{
		ASSUME( _count != 0 );
		return _memory.Pointer()[_count-1];
	}
	

	template <typename T, typename S, typename MC>
	inline const T & Array<T,S,MC>::Back () const
	{
		ASSUME( _count != 0 );
		return _memory.Pointer()[_count-1];
	}
	

	template <typename T, typename S, typename MC>
	inline T & Array<T,S,MC>::Front ()
	{
		ASSUME( _memory.Pointer() != null );
		return _memory.Pointer()[0];
	}

	
	template <typename T, typename S, typename MC>
	inline const T & Array<T,S,MC>::Front () const
	{
		ASSUME( _memory.Pointer() != null );
		return _memory.Pointer()[0];
	}


	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::Free ()
	{
		if ( _memory.Pointer() != null )
		{
			Strategy::Destroy( _memory.Pointer(), _count );
			_memory.Deallocate();
		}
		_count = 0;
		_size  = 0;
	}


	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::Clear ()
	{
		if ( _memory.Pointer() != null )
		{
			Strategy::Destroy( _memory.Pointer(), _count );
		}
		_count = 0;
	}


	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::Erase (usize pos, usize count)
	{
		if ( pos >= _count or count == 0 or _count == 0 )
			RET_VOID;

		if ( pos + count > _count )
			count = _count - pos;

		_count -= count;

		Strategy::Destroy( _memory.Pointer() + pos, count );
		Strategy::Replace( _memory.Pointer() + pos, _memory.Pointer() + pos + count, _count - pos );
	}
	

	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::EraseFromBack (usize count)
	{
		ASSERT( count < _count );

		if ( count >= _count ) {
			Clear();
			return;
		}

		Strategy::Destroy( _memory.Pointer() + _count - count, count );
		_count -= count;
	}
	
	
	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::FastErase (usize pos, usize count)
	{
		if ( pos >= _count or count == 0 or _count == 0 )
			RET_VOID;
		
		if ( pos + count >= _count )
			count = _count - pos;

		usize const	u_max = pos + count;
		usize		j	  = _count-1;
		usize		i	  = pos;
		
		_count -= count;

		// move from back
		for (; j >= u_max and i < u_max; --j or ++i)
		{
			Strategy::Destroy( _memory.Pointer() + i, 1 );
			Strategy::Replace( _memory.Pointer() + i, _memory.Pointer() + j );
		}

		if ( i == u_max )
			return;

		// delete
		for (; j >= i; --j)
		{
			Strategy::Destroy( _memory.Pointer() + j, 1 );
		}
	}

	
	template <typename T, typename S, typename MC>
	template <typename E>
	inline bool Array<T,S,MC>::FindAndErase (const E &value)
	{
		usize	idx = -1;

		if ( Find<E>( idx, value ) )
		{
			Erase( idx );
			return true;
		}
		return false;
	}

	
	template <typename T, typename S, typename MC>
	template <typename E>
	inline bool Array<T,S,MC>::FindAndFastErase (const E &value)
	{
		usize	idx = -1;

		if ( Find<E>( idx, value ) )
		{
			FastErase( idx );
			return true;
		}
		return false;
	}
	
	
	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::FastErase (usize pos)
	{
		ASSERT( pos < _count );

		if ( pos == _count-1 )
			PopBack();
		else
		{
			Strategy::Destroy( _memory.Pointer() + pos, 1 );
			Strategy::Replace( _memory.Pointer() + pos, &Back(), 1 );
			--_count;
		}
	}


	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::PushBack (const T& value)
	{
		if ( _count >= _size )
			_Reallocate( _count + 1 );

		Strategy::Copy( _memory.Pointer() + _count, &value, 1 );
		++_count;
	}
	

	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::PushBack (T&& value)
	{
		if ( _count >= _size )
			_Reallocate( _count + 1 );

		Strategy::Move( _memory.Pointer() + _count, &value, 1 );
		++_count;
	}


	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::PopBack ()
	{
		ASSERT( _count != 0 );

		if ( _count != 0 ) {
			--_count;
			Strategy::Destroy( _memory.Pointer() + _count, 1 );
		}
	}

	
	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::PopFront ()
	{
		ASSERT( _count != 0 );

		Erase( 0 );
	}


	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::FreeReserve ()
	{
		if ( _count > _size )
			_Reallocate( _count, false );
	}


	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::Resize (usize newSize, bool allowReserve)
	{
		if ( newSize == _count )
			return;
		
		if ( newSize == 0 )
		{
			Clear();
			return;
		}

		if ( newSize < _count )
		{
			Strategy::Destroy( _memory.Pointer() + newSize, _count - newSize );
			_count = newSize;
			return;
		}

		_Reallocate( newSize, allowReserve );
		Strategy::Create( _memory.Pointer() + _count, newSize - _count );
		_count = newSize;
	}


	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::Reserve (usize size)
	{
		if ( size > _memory.MaxSize() )
			size = _memory.MaxSize();

		if ( size <= _size )
			return;

		_Reallocate( size, false );
	}
	

	template <typename T, typename S, typename MC>
	inline void Array<T,S,MC>::Swap (usize first, usize second)
	{
		if ( first >= _count or second >= _count )
			RET_VOID;

		T	temp;
		Strategy::Replace( &temp,						_memory.Pointer() + second,	1 );
		Strategy::Replace( _memory.Pointer() + second,	_memory.Pointer() + first,	1 );
		Strategy::Replace( _memory.Pointer() + first,	&temp,						1 );
	}


	template <typename T, typename S, typename MC>
	template <typename T2, typename S2, typename A2>
	inline void Array<T,S,MC>::Convert (Array<T2,S2,A2> &other) const
	{
		other.Resize( Count(), false );

		for (usize i = 0; i < Count(); ++i)
		{
			other[i] = T2( _memory.Pointer()[i] );
		}
	}

	
	template <typename T, typename S, typename MC>
	inline T * Array<T,S,MC>::Begin ()
	{
		ASSERT( _memory.Pointer() != null );
		return &_memory.Pointer()[0];
	}
	

	template <typename T, typename S, typename MC>
	inline const T * Array<T,S,MC>::Begin () const
	{
		ASSERT( _memory.Pointer() != null );
		return &_memory.Pointer()[0];
	}

	
	template <typename T, typename S, typename MC>
	inline T * Array<T,S,MC>::End ()
	{
		ASSERT( _count != 0 );
		return &_memory.Pointer()[_count];
	}
	

	template <typename T, typename S, typename MC>
	inline const T * Array<T,S,MC>::End () const
	{
		ASSERT( _count != 0 );
		return &_memory.Pointer()[_count];
	}

	
	template <typename T, typename S, typename MC>
	inline T * Array<T,S,MC>::GetIter (usize index)
	{
		ASSERT( index < _count );
		return &_memory.Pointer()[index];
	}
	

	template <typename T, typename S, typename MC>
	inline const T * Array<T,S,MC>::GetIter (usize index) const
	{
		ASSERT( index < _count );
		return &_memory.Pointer()[index];
	}
	

	template <typename T, typename S, typename MC>
	inline bool Array<T,S,MC>::IsBegin (const_iterator iter) const
	{
		return ( iter == Begin() );
	}
	

	template <typename T, typename S, typename MC>
	inline bool Array<T,S,MC>::IsEnd (const_iterator iter) const
	{
		return ( iter == End() );
	}

	
	#undef  RET_ERROR
	#undef	RET_FALSE
	#undef  RET_VOID

	
	template <typename T, typename S, typename MC>
	struct Hash< Array<T,S,MC> > :
		private Hash< Buffer<const T> >
	{
		typedef Array<T,S,MC>				key_t;
		typedef Hash< Buffer<const T> >		base_t;
		typedef typename base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x );
		}
	};

}	// GX_STL
}	// GXTypes
