﻿// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CopyStrategy.h"
#include "Engine/STL/CompileTime/NewTypeInfo.h"

#include <vector>

namespace GX_STL
{
namespace GXTypes
{
	
	template <typename T, typename S, typename MC> struct Array;


	//
	// Buffer Wrapper
	//

	template <typename T>
	struct Buffer : public CompileTime::FastCopyable
	{
	// types
	public:
		typedef T					value_t;
		typedef Buffer<T>			Self;

		typedef typename TypeTraits::CopyConstToPointer< T, void *>		void_ptr_t;
		
		typedef typename TypeTraits::RemoveConst<T>		C;

	private:
		struct _CompareElements
		{
			bool operator () (const T& left, const T& right) const
			{
				return left == right;
			}
		};
		

	// variables
	private:
		union {
			T	*				_memory;
			TMemoryViewer<T>	_memView;
		};
		usize					_count;


	// methods
	public:
		Buffer (GX_DEFCTOR);
		Buffer (T *arrayPtr, usize count);
		Buffer (void_ptr_t begin, void_ptr_t end);

		template <usize I>
		Buffer (const C (&arr)[I]);

		Buffer (Self &&other) = default;
		Buffer (const Self &other) = default;
		
		Buffer (std::initializer_list<C> list);

		template <template <typename ...> class LinearMemoryContainerType, typename ...Types>
		Buffer (LinearMemoryContainerType< C, Types... > &container);
		
		template <template <typename ...> class LinearMemoryContainerType, typename ...Types>
		Buffer (const LinearMemoryContainerType< C, Types... > &container);

		std::vector<C> ToStdVector () const;

		Self &		operator =  (Self &&right)		= default;
		Self &		operator =  (const Self &right)	= default;

		T		*	ptr ();
		T const	*	ptr () const;
		
		T *			RawPtr ()					{ return _memory; }
		T const *	RawPtr () const				{ return _memory; }
		
		T		&	Back ()						{ return (*this)[ LastIndex() ]; }
		T const	&	Back () const				{ return (*this)[ LastIndex() ]; }
		T		&	Front ()					{ return (*this)[ 0 ]; }
		T const	&	Front () const				{ return (*this)[ 0 ]; }

		T		&	operator [] (usize i);
		T const	&	operator [] (usize i) const;
		
		bool		operator == (const Self &right) const;
		bool		operator != (const Self &right) const;

		bool		operator ! ()	const		{ return not Empty(); }

		operator	Buffer<const T> ()	const	{ return Buffer<const T>( _memory, _count ); }

		bool		Empty ()		const		{ return _count == 0; }
		usize		Count ()		const		{ return _count; }
		BytesU		Size ()			const		{ return BytesU( _count * sizeof(T) ); }
		usize		LastIndex ()	const		{ return Count()-1; }

		template <typename Cmp>
		bool		Equals (const Self &other, Cmp sCmp) const;
		
		usize		GetIndex (const T &valueRef) const;
		bool		IsInArray (const T &valueRef) const;
		
		template <typename E>
		bool		Find (OUT usize &index, const E &value, usize start = 0) const;

		template <typename E>
		bool		IsExist (const E &value) const;

		bool		Intersects (const Self &other) const;

		Self			SubArray (usize first, usize count = usize(-1));
		Buffer<const T>	SubArray (usize first, usize count = usize(-1)) const;


		typedef	T *			iterator;
		typedef const T *	const_iterator;

		bool			IsBegin (const_iterator iter) const;
		bool			IsEnd (const_iterator iter) const;

		iterator		Begin ()					{ return ptr(); }
		const_iterator	Begin () const				{ return ptr(); }

		iterator		End ()						{ return ptr() + _count; }
		const_iterator	End () const				{ return ptr() + _count; }

		iterator		GetIter (usize i);
		const_iterator	GetIter (usize i) const;


	// static methods
	public:
		template <typename B>
		static Self From (Buffer<B> arr);
		
		template <typename B, typename S, typename MC>
		static Self From (const Array<B,S,MC> &arr);

		static Self FromVoid (void_ptr_t ptr, BytesU size);
		
		template <typename B, usize I>
		static Self From (const B (&arr)[I]);
		
		template <typename B>
		static Self FromStd (const std::vector<B> &vec);
		
		template <typename B>
		static Self FromValue (B &ref);

		static constexpr bool	IsLinearMemory ()	{ return true; }
	};
	

	typedef Buffer< const ubyte >	BinaryBuffer;


	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	inline Buffer<T>::Buffer (UninitializedType) :
		_memory(null), _count(0)
	{}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	inline Buffer<T>::Buffer (void_ptr_t begin, void_ptr_t end) :
		_memory( static_cast<T*>( begin ) ),
		_count( ( usize(end) - usize(begin) ) / sizeof(T) )
	{
		ASSERT( _count == 0 or _memory != null );
		ASSERT( begin <= end );
	}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	inline Buffer<T>::Buffer (T *arrayPtr, usize count) : _memory(arrayPtr), _count(count)
	{
		ASSERT( _count == 0 or _memory != null );
	}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	template <usize I>
	inline Buffer<T>::Buffer (const C (&arr)[I]) :
		_memory( (T *) arr ), _count( I )
	{
		ASSERT( _count == 0 or _memory != null );
	}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	inline Buffer<T>::Buffer (std::initializer_list<C> list) :
		_memory( list.begin() ), _count( list.size() )
	{
		ASSERT( _count == 0 or _memory != null );
	}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	template <template <typename ...> class LinearMemoryContainerType, typename ...Types>
	inline Buffer<T>::Buffer (LinearMemoryContainerType< C, Types... > &container) :
		_memory( null ), _count( container.Count() )
	{
		if ( container.Count() > 0 )
			_memory = container.ptr();
	}
		
	
	template <typename T>
	template <template <typename ...> class LinearMemoryContainerType, typename ...Types>
	inline Buffer<T>::Buffer (const LinearMemoryContainerType< C, Types... > &container) :
		_memory( null ), _count( container.Count() )
	{
		if ( container.Count() > 0 )
			_memory = container.ptr();
	}
	
/*
=================================================
	ToStdVector
=================================================
*/
	template <typename T>
	inline std::vector< typename Buffer<T>::C >  Buffer<T>::ToStdVector () const
	{
		return std::vector<C>( ptr(), Count() );
	}
	
/*
=================================================
	From
=================================================
*/
	template <typename T>
	template <typename B>
	inline Buffer<T>  Buffer<T>::From (Buffer<B> arr)
	{
		if ( not arr.Empty() )
			return FromVoid( static_cast<void_ptr_t>( const_cast< TypeTraits::RemoveConst<B *> >( arr.ptr() ) ), arr.Size() );
		else
			return Buffer<T>();
	}
	
	
	template <typename T>
	template <typename B, usize I>
	inline Buffer<T>  Buffer<T>::From (const B (&arr)[I])
	{
		return From( Buffer<const B>( arr ) );
	}
	
/*
=================================================
	FromStd
=================================================
*/
	template <typename T>
	template <typename B>
	static Buffer<T>  Buffer<T>::FromStd (const std::vector<B> &vec)
	{
		if ( not vec.empty() )
			return FromVoid( (void_ptr_t) &vec[0], vec.size() * sizeof(B) );
		else
			return Buffer<T>();
	}
	
/*
=================================================
	FromVoid
=================================================
*/
	template <typename T>
	inline Buffer<T>  Buffer<T>::FromVoid (void_ptr_t ptr, BytesU size)
	{
		if ( ptr != null and size > 0 )
			return Buffer<T>( static_cast< T *>( const_cast< void *>( ptr ) ), usize( size / SizeOf<T>() ) );
		else
			return Buffer<T>();
	}
	
/*
=================================================
	FromValue
=================================================
*/
	template <typename T>
	template <typename B>
	inline Buffer<T>  Buffer<T>::FromValue (B &ref)
	{
		STATIC_ASSERT( sizeof(B) % sizeof(T) == 0 );
		return Buffer<T>( PointerCast<T>( &ref ), sizeof(B) / sizeof(T) );
	}
	
/*
=================================================
	ptr
=================================================
*/
	template <typename T>
	inline T * Buffer<T>::ptr ()
	{
		ASSUME( _memory != null );
		return _memory;
	}
	
	template <typename T>
	inline T const * Buffer<T>::ptr () const
	{
		ASSUME( _memory != null );
		return _memory;
	}
	
/*
=================================================
	operator []
=================================================
*/
	template <typename T>
	inline T & Buffer<T>::operator [] (usize i)
	{
		ASSUME( i < _count );
		return _memory[i];
	}

	template <typename T>
	inline T const & Buffer<T>::operator [] (usize i) const
	{
		ASSUME( i < _count );
		return _memory[i];
	}

/*
=================================================
	operator ==
=================================================
*/
	template <typename T>
	inline bool Buffer<T>::operator == (const Self &other) const
	{
		return Equals( other, _CompareElements() );
	}

	template <typename T>
	inline bool Buffer<T>::operator != (const Self &other) const
	{
		return not ( *this == other );
	}
	
/*
=================================================
	Equals
=================================================
*/
	template <typename T>
	template <typename Cmp>
	inline bool Buffer<T>::Equals (const Self &other, Cmp sCmp) const
	{
		if ( _count != other._count )
			return false;

		if ( other._memory == _memory )
		{
			if ( _memory != null )
				WARNING( "compared with self!" );
			return true;
		}

		for (usize i = 0; i < _count; ++i) {
			if ( not sCmp( _memory[i], other._memory[i] ) )
				return false;
		}

		return true;
	}
	
/*
=================================================
	GetIndex
=================================================
*/
	template <typename T>
	inline usize Buffer<T>::GetIndex (const T &valueRef) const
	{
		ASSERT( IsInArray( valueRef ) );
		return usize( &valueRef - Begin() );
	}
	
/*
=================================================
	IsInArray
=================================================
*/
	template <typename T>
	inline bool Buffer<T>::IsInArray (const T &valueRef) const
	{
		return ( &valueRef >= Begin() and &valueRef < End() );
	}
	
/*
=================================================
	Find
=================================================
*/
	template <typename T>
	template <typename E>
	inline bool Buffer<T>::Find (OUT usize &index, const E &value, usize start) const
	{
		index = -1;

		FORv( i, start, *this )
		{
			if ( value == _memory[i] )
			{
				index = i;
				return true;
			}
		}
		return false;
	}
	
/*
=================================================
	IsExist
=================================================
*/
	template <typename T>
	template <typename E>
	inline bool Buffer<T>::IsExist (const E &value) const
	{
		usize	idx;
		return Find( idx, value, 0 );
	}
	
/*
=================================================
	SubArray
=================================================
*/
	template <typename T>
	inline Buffer<T>  Buffer<T>::SubArray (usize first, usize count)
	{
		ASSERT( first <= Count() and (count == usize(-1) or first + count <= Count()) );
		
		if ( first >= Count() )
			return Buffer<T>();
		
		// 'count' can be usize(-1)
		if ( count == usize(-1) or count + first > Count() )
			count = Count() - first;

		return ( Buffer<T>( ptr() + first, count ) );
	}
	
/*
=================================================
	SubArray
=================================================
*/
	template <typename T>
	inline Buffer<const T>  Buffer<T>::SubArray (usize first, usize count) const
	{
		ASSERT( first <= Count() and (count == usize(-1) or first + count <= Count()) );
		
		if ( first >= Count() )
			return Buffer<const T>();

		// 'count' can be usize(-1)
		if ( count == usize(-1) or count + first > Count() )
			count = Count() - first;

		return ( Buffer<const T>( ptr() + first, count ) );
	}
	
/*
=================================================
	Intersects
=================================================
*/
	template <typename T>
	inline bool  Buffer<T>::Intersects (const Self &other) const
	{
		ASSERT( Begin() <= End() );
		ASSERT( other.Begin() <= other.End() );
		return Begin() > other.End() or End() < other.Begin();
	}
	
/*
=================================================
	GetIter
=================================================
*/
	template <typename T>
	inline T * Buffer<T>::GetIter (usize i)
	{
		ASSUME( i < _count );
		return & _memory[i];
	}
	
/*
=================================================
	GetIter
=================================================
*/
	template <typename T>
	inline const T * Buffer<T>::GetIter (usize i) const
	{
		ASSUME( i < _count );
		return & _memory[i];
	}
	
/*
=================================================
	IsBegin
=================================================
*/
	template <typename T>
	inline bool Buffer<T>::IsBegin (const_iterator iter) const
	{
		return Begin() == iter;
	}
	
/*
=================================================
	IsEnd
=================================================
*/
	template <typename T>
	inline bool Buffer<T>::IsEnd (const_iterator iter) const
	{
		return End() == iter;
	}

	
	
/*
=================================================
	Hash
=================================================
*/
	template <typename T>
	struct Hash< Buffer<T> > : private Hash<T>
	{
		typedef Buffer<T>					key_t;
		typedef Hash<T>						base_t;
		typedef typename base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			result_t	value = ~Hash<usize>()( x.Count() );

			if ( CompileTime::IsPOD<T> )
			{
				value += _types_hidden_::HashForMemoryBlock( (const ubyte *)x.RawPtr(), (usize)x.Size() );
			}
			else
			{
				base_t	hasher;

				FOR( i, x ) {
					value += hasher( x[i] );
				}
			}
			return value;
		}
	};
	

/*
=================================================
	PlacementNew
=================================================
*/
	template <typename T, typename C>
	forceinline T * PlacementNew (Buffer<C> buf) noexcept
	{
		STATIC_ASSERT( not TypeTraits::IsConst<C> );
		STATIC_ASSERT( CompileTime::IsPOD<C> );
		ASSERT( buf.Size() >= SizeOf<T>() );

		return UnsafeMem::PlacementNew<T>( buf.ptr() );
	}
			
	template <typename T, typename C, typename ...Types>
	forceinline T * PlacementNew (Buffer<C> buf, Types const&... args) noexcept
	{
		STATIC_ASSERT( not TypeTraits::IsConst<C> );
		STATIC_ASSERT( CompileTime::IsPOD<C> );
		ASSERT( buf.Size() >= SizeOf<T>() );

		return UnsafeMem::PlacementNew<T>( buf.ptr(), args... );
	}

	template <typename T, typename C, typename ...Types>
	forceinline T * PlacementNew (Buffer<C> buf, Types&&... args) noexcept
	{
		STATIC_ASSERT( not TypeTraits::IsConst<C> );
		STATIC_ASSERT( CompileTime::IsPOD<C> );
		ASSERT( buf.Size() >= SizeOf<T>() );

		return UnsafeMem::PlacementNew<T>( buf.ptr(), FW<Types>(args)... );
	}
	
/*
=================================================
	ZeroMem
=================================================
*/
	template <typename T>
	inline void ZeroMem (Buffer<T> buf)
	{
		STATIC_ASSERT( not TypeTraits::IsConst<T> );
		STATIC_ASSERT( CompileTime::IsPOD<T> );

		UnsafeMem::ZeroMem( buf.ptr(), buf.Size() );
	}
	
/*
=================================================
	MemCopy
----
	memory blocks must not intersects
=================================================
*/
	template <typename T0, typename T1>
	inline void MemCopy (Buffer<T0> dst, Buffer<const T1> src)
	{
		STATIC_ASSERT( not TypeTraits::IsConst<T0> );
		STATIC_ASSERT( CompileTime::IsPOD<T0> and CompileTime::IsPOD<T1> );
		ASSERT( dst.Size() >= src.Size() );

		UnsafeMem::MemCopy( dst.ptr(), src.ptr(), GXMath::Min( dst.Size(), src.Size() ) );
	}
	
/*
=================================================
	MemMove
----
	memory blocks may intersects
=================================================
*/
	template <typename T0, typename T1>
	inline void MemMove (Buffer<T0> dst, Buffer<const T1> src)
	{
		STATIC_ASSERT( not TypeTraits::IsConst<T0> );
		STATIC_ASSERT( CompileTime::IsPOD<T0> and CompileTime::IsPOD<T1> );
		ASSERT( dst.Size() >= src.Size() );

		UnsafeMem::MemMove( dst.ptr(), src.ptr(), Min( dst.Size(), src.Size() ) );
	}
	
/*
=================================================
	MemCmp
=================================================
*/
	template <typename T0, typename T1>
	inline int MemCmp (Buffer<const T0> left, Buffer<const T1> right)
	{
		ASSERT( dst.Size() == src.Size() );

		return UnsafeMem::MemCmp( left.ptr(), right.ptr(), Min( left.Size(), right.Size() ) );
	}


}	// GXTypes
}	// GX_STL
