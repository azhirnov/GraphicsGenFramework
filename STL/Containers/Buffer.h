// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CopyStrategy.h"
#include "../CompileTime/NewTypeInfo.h"

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

		//Buffer (const std::vector<C> &vec);

		std::vector<C> ToStdVector () const;

		Buffer (Self &&other) = default;
		Buffer (const Self &other) = default;

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

		bool			Empty ()		const	{ return _count == 0; }
		usize			Count ()		const	{ return _count; }
		Bytes<usize>	Size ()			const	{ return Bytes<usize>( _count * sizeof(T) ); }
		usize			LastIndex ()	const	{ return Count()-1; }

		template <typename Cmp>
		bool		Equals (const Self &other, Cmp sCmp) const;
		
		usize		GetIndex (const T &value) const;
		bool		IsInArray (const T &value) const;
		
		template <typename E>
		bool		Find (OUT usize &index, const E &value, usize start = 0) const;

		template <typename E>
		bool		IsExist (const E &value) const;


		Self			SubArray (usize first, usize count = usize(-1));
		Buffer<const T>	SubArray (usize first, usize count = usize(-1)) const;

		bool		Intersects (const Self &other) const;


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

		static Self FromVoid (void_ptr_t ptr, usize size);

		template <typename B>
		static Self FromType (B &ref);

		template <typename B, usize I>
		static Self From (const B (&arr)[I]);

		static Self From (const T &value);
		
		template <typename B>
		static Self FromStd (const std::vector<B> &vec);
	};
	

	typedef Buffer< const ubyte >	BinaryBuffer;


	
	template <typename T>
	inline Buffer<T>::Buffer (Uninitialized) :
		_memory(null), _count(0)
	{}


	template <typename T>
	inline Buffer<T>::Buffer (void_ptr_t begin, void_ptr_t end) :
		_memory( static_cast<T*>( begin ) ),
		_count( ( usize(end) - usize(begin) ) / sizeof(T) )
	{
		ASSUME( _count == 0 or _memory != null );
		ASSUME( begin <= end );
	}


	template <typename T>
	inline Buffer<T>::Buffer (T *arrayPtr, usize count) : _memory(arrayPtr), _count(count)
	{
		ASSUME( _count == 0 or _memory != null );
	}
	

	template <typename T>
	template <usize I>
	inline Buffer<T>::Buffer (const C (&arr)[I]) :
		_memory( (T *) arr ), _count( I )
	{
		ASSUME( _count == 0 or _memory != null );
	}
	
	
	/*template <typename T>
	inline Buffer<T>::Buffer (const std::vector<C> &vec) :
		_memory( &vec[0] ), _count( vec.size() )
	{
		ASSUME( _count == 0 or _memory != null );
	}*/
	

	template <typename T>
	inline std::vector< typename Buffer<T>::C >  Buffer<T>::ToStdVector () const
	{
		return std::vector<C>( ptr(), Count() );
	}

	
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
	

	template <typename T>
	inline Buffer<T>  Buffer<T>::From (const T &value)
	{
		return Buffer<T>( &value, 1 );
	}
	
	
	template <typename T>
	template <typename B>
	static Buffer<T>  Buffer<T>::FromStd (const std::vector<B> &vec)
	{
		if ( not vec.empty() )
			return FromVoid( (void_ptr_t) &vec[0], vec.size() * sizeof(B) );
		else
			return Buffer<T>();
	}


	template <typename T>
	inline Buffer<T>  Buffer<T>::FromVoid (void_ptr_t ptr, usize size)
	{
		if ( ptr != null and size > 0 )
			return Buffer<T>( static_cast< T *>( const_cast< void *>( ptr ) ), size / sizeof(T) );
		else
			return Buffer<T>();
	}

	
	template <typename T>
	template <typename B>
	inline Buffer<T>  Buffer<T>::FromType (B &ref)
	{
		STATIC_ASSERT( sizeof(B) % sizeof(T) == 0 );
		return FromVoid( static_cast< void_ptr_t >( &ref ), sizeof(B) );
	}


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


	template <typename T>
	inline usize Buffer<T>::GetIndex (const T &value) const
	{
		ASSERT( IsInArray( value ) );
		return usize( &value - Begin() );
	}
	
	
	template <typename T>
	inline bool Buffer<T>::IsInArray (const T &value) const
	{
		return ( &value >= Begin() and &value < End() );
	}
	
	
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

	
	template <typename T>
	template <typename E>
	inline bool Buffer<T>::IsExist (const E &value) const
	{
		usize	idx;
		return Find( idx, value, 0 );
	}


	template <typename T>
	inline Buffer<T>  Buffer<T>::SubArray (usize first, usize count)
	{
		ASSERT( first < Count() and first + count <= Count() );
		
		if ( first >= Count() )
			return Buffer<T>();
		
		// count can be usize(-1)
		if ( count >= Count() or count + first >= Count() )
			count = Count() - first;

		return ( Buffer<T>( ptr() + first, count ) );
	}

	
	template <typename T>
	inline Buffer<const T>  Buffer<T>::SubArray (usize first, usize count) const
	{
		ASSERT( first < Count() and first + count <= Count() );
		
		if ( first >= Count() )
			return Buffer<const T>();

		// count can be usize(-1)
		if ( count >= Count() or count + first >= Count() )
			count = Count() - first;

		return ( Buffer<const T>( ptr() + first, count ) );
	}
	

	template <typename T>
	inline bool  Buffer<T>::Intersects (const Self &other) const
	{
		ASSERT( Begin() <= End() );
		ASSERT( other.Begin() <= other.End() );
		return Begin() > other.End() or End() < other.Begin();
	}

	
	template <typename T>
	inline T * Buffer<T>::GetIter (usize i)
	{
		ASSUME( i < _count );
		return & _memory[i];
	}

	
	template <typename T>
	inline const T * Buffer<T>::GetIter (usize i) const
	{
		ASSUME( i < _count );
		return & _memory[i];
	}

	
	template <typename T>
	inline bool Buffer<T>::IsBegin (const_iterator iter) const
	{
		return Begin() == iter;
	}

	
	template <typename T>
	inline bool Buffer<T>::IsEnd (const_iterator iter) const
	{
		return End() == iter;
	}

	

	template <typename T>
	struct Hash< Buffer<T> > : private Hash<T>
	{
		typedef Buffer<T>					key_t;
		typedef Hash<T>						base_t;
		typedef typename base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			usize	value = ~Hash<usize>()( x.Count() );

			if ( CompileTime::IsPOD<T> )
			{
				value ^= _types_hidden_::HashForMemoryBlock( (const ubyte *)x.RawPtr(), x.Size() );
			}
			else
			{
				base_t	hasher;

				FOR( i, x ) {
					value ^= ( hasher( x[i] ) << (i&3) ) >> ((i+1) & 1);
				}
			}
			return (result_t) value;
		}
	};

}	// GXTypes
}	// GX_STL
