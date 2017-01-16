﻿// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "MapUtils.h"

namespace GX_STL
{
namespace GXTypes
{
namespace _types_hidden_
{

	//
	// Base Map (Map or MultiMap)
	//

	template <	typename K,
				typename T,
				bool IsUnique,
				typename S,
				typename MC
			 >
	struct BaseMap : public CompileTime::CopyQualifiers< CompileTime::FastCopyable, MC >
	{
	// types
	public:
		typedef BaseMap< K, T, IsUnique, S, MC >	Self;

		typedef K									key_t;
		typedef T									value_t;

		typedef Pair< K, T >						pair_t;
		typedef Pair< const K, T>					const_pair_t;
		
		typedef Ptr< const_pair_t >					iterator;
		typedef Ptr< const const_pair_t >			const_iterator;
		
		typedef Buffer< const_pair_t >				values_range_t;
		typedef Buffer< const const_pair_t >		const_values_range_t;

	private:
		template <typename Key>
		struct _KeySearchCmp
		{
		// variables
			const Key &	_key;
			
		// methods
			_KeySearchCmp (const Key *k): _key(*k) {}
			_KeySearchCmp (const pair_t *p): _key(p->first) {}
			
			bool operator == (const pair_t &pair) const		{ return GXMath::All( _key == pair.first ); }
			bool operator != (const pair_t &pair) const		{ return not ( *this == pair.first ); }
			bool operator <  (const pair_t &pair) const		{ return GXMath::All( _key < pair.first ); }
			bool operator >  (const pair_t &pair) const		{ return GXMath::All( _key > pair.first ); }
		};

		typedef _types_hidden_::MapUtils< Array< pair_t, S, MC >, key_t, _KeySearchCmp, IsUnique >	_MapUtils_t;


	// variables
	private:
		_MapUtils_t		_memory;


	// methods
	public:
		BaseMap (GX_DEFCTOR)
		{}

		BaseMap (const Self &other) : _memory( other._memory )
		{}
		
		BaseMap (Self &&other) : _memory( RVREF( other._memory ) )
		{}
		

		operator	const_values_range_t () const
		{
			return _memory;
		}


		const_pair_t &	operator [] (usize i)
		{
			return ReferenceCast< const_pair_t >( _memory[i] );
		}

		const_pair_t const&	operator [] (usize i) const
		{
			return ReferenceCast< const const_pair_t >( _memory[i] );
		}
		

		const_pair_t &	operator () (const key_t &key)
		{
			usize	i;
			FindIndex( key, i );
			return (*this)[i];
		}
		
		const_pair_t const&	operator () (const key_t &key) const
		{
			usize	i;
			FindIndex( key, i );
			return (*this)[i];
		}


		Self &		operator << (const pair_t &value)
		{
			Add( value );
			return *this;
		}

		Self &		operator << (pair_t &&value)
		{
			Add( RVREF( value ) );
			return *this;
		}


		bool		operator == (const_values_range_t right) const
		{
			return _memory == right._memory;
		}

		bool		operator != (const_values_range_t right) const
		{
			return not (*this == right);
		}
		

		Self &		operator =  (Self &&right)		= default;
		Self &		operator =  (const Self &right)	= default;


		const_pair_t &		Front ()			{ return (*this)[0]; }
		const_pair_t const&	Front ()	const	{ return (*this)[0]; }

		const_pair_t &		Back ()				{ return (*this)[ LastIndex() ]; }
		const_pair_t const&	Back ()		const	{ return (*this)[ LastIndex() ]; }


		//const_pair_t *		ptr ()				{ return (const_pair_t *) _memory.ptr(); }
		//const_pair_t const*	ptr ()		const	{ return (const_pair_t const *) _memory.ptr(); }
		

		bool			Empty ()		const	{ return _memory.Empty(); }
		usize			Count ()		const	{ return _memory.Count(); }
		usize			LastIndex ()	const	{ return _memory.LastIndex(); }
		Bytes<usize>	Size ()			const	{ return _memory.Size(); }

		
		usize Add (const key_t &key, const value_t &value)
		{
			return _memory.Add( RVREF( pair_t( key, value ) ) );
		}
		
		usize Add (key_t &&key, value_t &&value)
		{
			return _memory.Add( RVREF( pair_t( RVREF(key), RVREF(value) ) ) );
		}

		usize Add (const pair_t &value)
		{
			return _memory.Add( value );
		}

		usize Add (pair_t &&value)
		{
			return _memory.Add( RVREF( value ) );
		}
		

		void AddArray (const_values_range_t value)
		{
			FOR( i, value ) {
				Add( value[i] );
			}
		}


		/*usize ChangeKey (const key_t &key, usize idx)
		{
			const const_pair_t	tmp = (*this)[ idx ];

			EraseFromIndex( idx );

			return Add( key, tmp.second );
		}

		usize ChangeKey (const key_t &key, const_iterator iter)
		{
			return ChangeKey( key, GetIndex( iter ) );
		}*/


		bool FindIndex (const key_t &key, OUT usize &idx) const
		{
			return FindFirstIndex( key, OUT idx );
		}

		bool FindFirstIndex (const key_t &key, OUT usize &idx) const
		{
			return _memory.FindFirstIndex( key, OUT idx );
		}

		void FindLastIndex (usize first, OUT usize &idx) const
		{
			return _memory.FindLastIndex( first, OUT idx );
		}


		bool IsExist (const key_t &key) const
		{
			usize idx = 0;
			return FindIndex( key, idx );
		}

		
		bool Find (const key_t &key, OUT iterator &result)
		{
			usize	idx = -1;

			if ( not FindIndex( key, OUT idx ) )
				return false;
			
			result = &(*this)[ idx ];
			return true;
		}

		bool Find (const key_t &key, OUT const_iterator &result) const
		{
			usize	idx = -1;

			if ( not FindIndex( key, OUT idx ) )
				return false;
			
			result = &(*this)[ idx ];
			return true;
		}


		bool FindAll (const key_t &key, OUT values_range_t &result)
		{
			usize	first;
			usize	last;

			if ( not FindFirstIndex( key, first ) )
				return false;
			
			FindLastIndex( first, last );

			result = values_range_t( ptr() + first, last - first + 1 );
			return true;
		}

		bool FindAll (const key_t &key, OUT const_values_range_t &result) const
		{
			usize	first;
			usize	last;

			if ( not FindFirstIndex( key, first ) )
				return false;
			
			FindLastIndex( first, last );

			result = const_values_range_t( ptr() + first, last - first + 1 );
			return true;
		}


		/*template <typename Key>
		bool LinearSearch (const Key &key) const
		{
			const_iterator	iter;
			return LinearSearch( key, iter );
		}

		template <typename Key>
		bool LinearSearch (const Key &key, OUT iterator &result)
		{
			FOR( i, _memory ) {
				if ( key == _memory[i].first ) {
					result = &_memory[i];
					return true;
				}
			}
			return false;
		}

		template <typename Key>
		bool LinearSearch (const Key &key, OUT const_iterator &result) const
		{
			FOR( i, _memory ) {
				if ( key == _memory[i].first ) {
					result = &_memory[i];
					return true;
				}
			}
			return false;
		}*/


		values_range_t  GetRange (usize first, usize last)
		{
			return const_values_range_t(*this).SubArray( first, last - first + 1 );
		}

		const_values_range_t  GetRange (usize first, usize last) const
		{
			return const_values_range_t(*this).SubArray( first, last - first + 1 );
		}


		void EraseFromIter (const_iterator iter)	{ return EraseFromIndex( _memory.GetIndex( iter ) ); }
		void EraseFromIter (iterator iter)			{ return EraseFromIndex( _memory.GetIndex( iter ) ); }

		bool Erase (const key_t &key)				{ return _memory.Erase( key ); }
		void EraseFromIndex (usize index)			{ _memory.EraseFromIndex( index ); }
		void Free ()								{ _memory.Free(); }
		void Clear ()								{ _memory.Clear(); }
		void Resize (usize size)					{ _memory.Resize( size ); }
		void Reserve (usize size)					{ _memory.Reserve( size ); }
		

		friend void SwapValues (INOUT Self &left, INOUT Self &right)
		{
			SwapValues( left._memory, right._memory );
		}
	};

	
}	// _types_hidden_


	template <	typename K,
				typename T,
				typename S = typename AutoDetectCopyStrategy< Pair<K, T> >::type,
				typename MC = MemoryContainer< Pair<K, T> >
			 >
	using Map = _types_hidden_::BaseMap< K, T, true, S, MC >;
	

	template <	typename K,
				typename T,
				typename S = typename AutoDetectCopyStrategy< Pair<K, T> >::type,
				typename MC = MemoryContainer< Pair<K, T> >
			 >
	using MultiMap = _types_hidden_::BaseMap< K, T, false, S, MC >;
	

	template <typename K, typename T, usize Size>
	using FixedSizeMap = _types_hidden_::BaseMap< K, T, true,
								typename AutoDetectCopyStrategy< Pair<K, T> >::type,
								StaticMemoryContainer< Pair<K, T>, Size > >;
	

	template <typename K, typename T, usize Size>
	using FixedSizeMultiMap = _types_hidden_::BaseMap< K, T, false,
									typename AutoDetectCopyStrategy< Pair<K, T> >::type,
									StaticMemoryContainer< Pair<K, T>, Size > >;

	
	template <typename K, typename T, bool IsUnique, typename S, typename MC>
	struct Hash< _types_hidden_::BaseMap< K, T, IsUnique, S, MC > > :
		private Hash< Buffer< const Pair<const K, T> > >
	{
		typedef _types_hidden_::BaseMap< K, T, IsUnique, S, MC >	key_t;
		typedef Buffer< const Pair<const K, T> >					base_t;
		typedef typename base_t::result_t							result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x );
		}
	};


}	// GXTypes
}	// GX_STL
