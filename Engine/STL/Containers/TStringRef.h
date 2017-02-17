// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Buffer.h"
#include <string>

namespace GX_STL
{
namespace GXTypes
{
	namespace StringUtils
	{
		template <typename T>
		constexpr forceinline T ToLower (const T &c);

	}	// StringUtils
	
	namespace _types_hidden_
	{
		template <typename T>
		forceinline usize StrLength (const T *str);

	}	// _types_hidden_

	
	template <typename T>	struct TStringRef;

	typedef TStringRef< char >			StringRef;
	typedef TStringRef< wchar >			WStringRef;

	typedef TStringRef< const char >	StringCRef;
	typedef TStringRef< const wchar >	WStringCRef;



	//
	// Universal String
	//

	template <typename T>
	struct TStringRef : public CompileTime::FastCopyable
	{
	// types
	public:
		typedef TStringRef<T>		Self;
		typedef T					value_t;

		typedef typename TypeTraits::RemoveConst<T>		C;


	// variables
	private:
		union {
			T *					_memory;
			TMemoryViewer<T>	_memView;
		};
		usize					_count;


	// methods
	public:
		TStringRef (GX_DEFCTOR);
		TStringRef (T *pString);
		TStringRef (T *pString, usize length);
		TStringRef (void *pBegin, void *pEnd);

		TStringRef (const std::basic_string< C, std::char_traits<C>, std::allocator<C> > &str);

		std::basic_string< C, std::char_traits<C>, std::allocator<C> > ToStdString () const;

		TStringRef (Self &&other) = default;
		TStringRef (const Self &other) = default;

		T		&	operator [] (usize i);
		T const	&	operator [] (usize i) const;
		
		Self &		operator =  (Self &&right)		= default;
		Self &		operator =  (const Self &right)	= default;

		bool		operator == (TStringRef<const T> right) const;
		bool		operator != (TStringRef<const T> right) const;
		bool		operator == (const T *right) const;
		bool		operator != (const T *right) const;
		
		friend bool	operator == (const T * left, const Self &right)	{ return right == left; }
		friend bool	operator != (const T * left, const Self &right)	{ return right != left; }
		
		bool		operator ! () const			{ return not Empty(); }
		
		bool		operator <  (TStringRef<const T> right) const;
		bool		operator >  (TStringRef<const T> right) const;
		bool		operator <= (TStringRef<const T> right) const;
		bool		operator >= (TStringRef<const T> right) const;
		
		friend bool	operator <  (const T * left, const Self &right)	{ return Self(left) < right; }
		friend bool	operator >  (const T * left, const Self &right)	{ return Self(left) > right; }
		friend bool	operator <= (const T * left, const Self &right)	{ return Self(left) <= right; }
		friend bool	operator >= (const T * left, const Self &right)	{ return Self(left) >= right; }

		operator	Buffer<T> ()					{ return Buffer<T>( _memory, Length() ); }
		operator	Buffer<const T> ()		const	{ return Buffer<const T>( _memory, Length() ); }
		operator	TStringRef<const T> ()	const	{ return TStringRef<const T>( _memory, Length() ); }

		T		*	ptr ();
		T const	*	ptr ()					const;
		T const	*	cstr ()					const	{ return ptr(); }
		
		T		&	Back ()							{ return (*this)[ Length()-1 ]; }
		T const	&	Back ()					const	{ return (*this)[ Length()-1 ]; }
		T		&	Front ()						{ return *ptr(); }
		T const	&	Front ()				const	{ return *ptr(); }

		usize		Length ()				const	{ return _count - usize(_count > 0); }
		usize		Count ()				const	{ return Length(); }
		BytesU		Size ()					const	{ return BytesU( sizeof(T) * _count ); }
		BytesU		LengthInBytes ()		const	{ return BytesU( sizeof(T) * Length() ); }

		bool		Empty ()				const	{ return _count <= 1; }
		
		T		*	Begin ()						{ return ptr(); }
		T const	*	Begin ()				const	{ return ptr(); }
		T		*	End ()							{ return ptr() + Length(); }
		T const	*	End ()					const	{ return ptr() + Length(); }


		// TODO: rename
		bool  IsInString (const T &valueRef)const	{ return Buffer<const T>(*this).IsInArray( valueRef ); }
		usize GetIndex (const T &valueRef)	const	{ return Buffer<const T>(*this).GetIndex( valueRef ); }

		bool EqualsIC (TStringRef<const T> right)	const;

		bool StartsWith (TStringRef<const T> right) const;
		bool StartsWithIC (TStringRef<const T> right) const;

		bool EndsWith (TStringRef<const T> right) const;
		bool EndsWithIC (TStringRef<const T> right) const;

		bool HasChar (T right) const;
		bool HasCharIC (T right) const;

		bool HasSubString (TStringRef<const T> right) const;
		bool HasSubStringIC (TStringRef<const T> right) const;

		//bool NumericLess (TStringRef<const T> right) const;

		bool LessThan (TStringRef<const T> right) const;
		bool LessThanIC (TStringRef<const T> right) const;
		
		bool GreaterThan (TStringRef<const T> right) const;
		bool GreaterThanIC (TStringRef<const T> right) const;
			  
		bool EqualsInRange (TStringRef<const T> right, usize begin, usize end) const;
		bool EqualsInRangeIC (TStringRef<const T> right, usize begin, usize end) const;

		bool Find (TStringRef<const T> value, OUT usize &pos, usize start = 0) const;
		bool Find (const T value, OUT usize &pos, usize start = 0) const;

		bool FindIC (TStringRef<const T> value, OUT usize &pos, usize start = 0) const;
		bool FindIC (const T value, OUT usize &pos, usize start = 0) const;

		// get range
		TStringRef<T>		SubString (usize pos, usize count = usize(-1));
		TStringRef<const T>	SubString (usize pos, usize count = usize(-1)) const;
		
		TStringRef<T>		GetInterval (usize begin, usize end);
		TStringRef<const T>	GetInterval (usize begin, usize end) const;

		TStringRef<T>		GetInterval (const T* begin, const T* end);
		TStringRef<const T>	GetInterval (const T* begin, const T* end) const;



		bool				Intersects (TStringRef<const T> other) const;

	private:
		static bool _Equals (const T *left, const T *right, usize length);
		static bool _EqualsIC (const T *left, const T *right, usize length);
	};
	

	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	inline TStringRef<T>::TStringRef (UninitializedType) :
		_memory(null), _count(0)
	{}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	inline TStringRef<T>::TStringRef (T *pString) :
		_memory(pString), _count( pString == null ? 0 : _types_hidden_::StrLength(pString)+1 )
	{}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	inline TStringRef<T>::TStringRef (T *pString, usize length) :
		_memory(pString), _count( pString == null ? 0 : length+1 )
	{}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	inline TStringRef<T>::TStringRef (void *pBegin, void *pEnd) :
		_memory( (T*)pBegin ), _count( ( usize(pEnd) - usize(pBegin) ) / sizeof(T) )
	{}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	inline TStringRef<T>::TStringRef (const std::basic_string< C, std::char_traits<C>, std::allocator<C> > &str) :
		_memory( (T *) str.c_str() ), _count( str.empty() ? 0 : str.length()+1 )
	{}
	
/*
=================================================
	ToStdString
=================================================
*/
	template <typename T>
	inline std::basic_string< typename TStringRef<T>::C, std::char_traits< typename TStringRef<T>::C >, std::allocator< typename TStringRef<T>::C > >
		TStringRef<T>::ToStdString () const
	{
		return std::basic_string< C, std::char_traits<C>, std::allocator<C> >( ptr(), Length() );
	}
	
/*
=================================================
	operator []
=================================================
*/
	template <typename T>
	inline T & TStringRef<T>::operator [] (usize i)
	{
		ASSUME( i < _count );
		return ptr()[i];
	}
	
	template <typename T>
	inline T const & TStringRef<T>::operator [] (usize i) const
	{
		ASSUME( i < _count );
		return ptr()[i];
	}
		
/*
=================================================
	operator ==
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::operator == (TStringRef<const T> right) const
	{
		if ( Length() != right.Length() )
			return false;

		if ( right.ptr() == _memory or _count == 0 )
			return true;

		return _Equals( _memory, right.ptr(), Length() );
	}

	template <typename T>
	inline bool TStringRef<T>::operator == (const T *pStr) const
	{
		return ( *this == Self(pStr) );
	}
	
/*
=================================================
	operator !=
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::operator != (TStringRef<const T> right) const
	{
		return not ( *this == right );
	}

	template <typename T>
	inline bool TStringRef<T>::operator != (const T *pStr) const
	{
		return not ( *this == pStr );
	}
		
/*
=================================================
	operator <
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::operator < (TStringRef<const T> right) const
	{
		typedef typename CompileTime::NearUInt::FromType<T>		UType;

		if ( Length() != right.Length() )
			return Length() < right.Length();

		for (usize i = 0; i < right.Length(); ++i)
		{
			if ( (*this)[i] != right[i] )
				return UType( (*this)[i] ) < UType( right[i] );
		}
		return false;
	}
		
/*
=================================================
	operator >
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::operator > (TStringRef<const T> right) const
	{
		typedef typename CompileTime::NearUInt::FromType<T>		UType;

		if ( Length() != right.Length() )
			return Length() > right.Length();

		for (usize i = 0; i < right.Length(); ++i)
		{
			if ( (*this)[i] != right[i] )
				return UType( (*this)[i] ) > UType( right[i] );
		}
		return false;
	}
	
/*
=================================================
	operator <=
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::operator <= (TStringRef<const T> right) const
	{
		return not ( *this > right );
	}
		
/*
=================================================
	operator >=
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::operator >= (TStringRef<const T> right) const
	{
		return not ( *this < right );
	}
	
/*
=================================================
	NumericLess
=================================================
*
	template <typename T>
	inline bool TStringRef<T>::NumericLess (TStringRef<const T> right) const
	{
		typedef typename CompileTime::NearUInt::FromType<T>		UType;

		for (usize l = 0, r = 0; l < Length() or r < right.Length(); ++l, ++r)
		{
			if ( (*this)[l] != right[r] )
				return UType( (*this)[l] ) < UType( right[r] );
		
			if ( (*this)[l] >= T('0') and (*this)[l] <= T('9') )
			{
				while ( (*this)[l] == T('0') ) { ++l; }
				while ( right[r] == T('0') ) { ++r; }

				const usize	l_begin = l,
							r_begin = r;

				while ( ((*this)[l] >= T('0') and (*this)[l] <= T('9')) and
						(right[r] >= T('0') and right[r] <= T('9') ) )
				{
					++l; ++r;
				}

				if ( (l - l_begin) == (r - r_begin) )
				{
					const usize	l_end = l,
								r_end = r;
					l = l_begin;
					r = r_begin;

					while ( l < l_end and r < r_end )
					{
						if ( (*this)[l] != right[r] )
							return UType( (*this)[l] ) < UType( right[r] );

						++l; ++r;
					}
				}
				else
					return ( (l - l_begin) < (r - r_begin) );
			}
		}
		return Length() < right.Length();
	}

/*
=================================================
	ptr
=================================================
*/
	template <typename T>
	inline T * TStringRef<T>::ptr ()
	{
		ASSUME( _memory != null );
		return _memory;
	}
	
	template <typename T>
	inline T const * TStringRef<T>::ptr () const
	{
		ASSUME( _memory != null );
		return _memory;
	}
	
/*
=================================================
	EqualsIC
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::EqualsIC (TStringRef<const T> right) const
	{
		if ( Length() != right.Length() )
			return false;

		return _Equals( ptr(), right.ptr(), Length() );
	}
	
/*
=================================================
	StartsWith
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::StartsWith (TStringRef<const T> right) const
	{
		if ( Length() < right.Length() )
			return false;

		return _Equals( ptr(), right.ptr(), right.Length() );
	}
	
/*
=================================================
	StartsWithIC
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::StartsWithIC (TStringRef<const T> right) const
	{
		if ( Length() < right.Length() )
			return false;

		return _EqualsIC( ptr(), right.ptr(), right.Length() );
	}
	
/*
=================================================
	EndsWith
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::EndsWith (TStringRef<const T> right) const
	{
		if ( Length() < right.Length() )
			return false;

		return _Equals( ptr() + (Length() - right.Length()), right.ptr(), right.Length() );
	}
	
/*
=================================================
	EndsWithIC
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::EndsWithIC (TStringRef<const T> right) const
	{
		if ( Length() < right.Length() )
			return false;

		return _EqualsIC( ptr() + (Length() - right.Length()), right.ptr(), right.Length() );
	}
	
/*
=================================================
	HasChar
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::HasChar (T right) const
	{
		usize	pos;
		return Find( right, pos );
	}
	
/*
=================================================
	HasCharIC
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::HasCharIC (T right) const
	{
		usize	pos;
		return FindIC( right, pos );
	}
	
/*
=================================================
	HasSubString
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::HasSubString (TStringRef<const T> right) const
	{
		usize	pos;
		return Find( right, pos );
	}
	
/*
=================================================
	HasSubStringIC
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::HasSubStringIC (TStringRef<const T> right) const
	{
		usize	pos;
		return FindIC( right, pos );
	}
	
/*
=================================================
	_Equals
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::_Equals (const T *left, const T *right, usize length)
	{
		for (usize i = 0; i < length; ++i) {
			if ( left[i] != right[i] )
				return false;
		}
		return true;
	}
	
/*
=================================================
	_EqualsIC
=================================================
*/
	template <typename T>
	inline bool TStringRef<T>::_EqualsIC (const T *left, const T *right, usize length)
	{
		for (usize i = 0; i < length; ++i) {
			if ( StringUtils::ToLower( left[i] ) != StringUtils::ToLower( right[i] ) )
				return false;
		}
		return true;
	}
	
/*
=================================================
	Find
=================================================
*/
	template <typename T>
	inline bool  TStringRef<T>::Find (TStringRef<const T> value, usize &pos, usize start) const
	{
		if ( Empty() or value.Empty() )
			return false;

		usize	j = 0;

		for (usize i = start; i < _count-2; ++i)
		{
			while ( value[j] == _memory[i+j] and i+j < _count-1 and j < value.Length() )
			{
				++j;
				if ( j >= value.Length() ) {
					pos = i;
					return true;
				}
			}
			j = 0;
		}
		return false;
	}
	
/*
=================================================
	Find
=================================================
*/
	template <typename T>
	inline bool  TStringRef<T>::Find (const T tValue, usize &pos, usize start) const
	{
		if ( Empty() )
			return false;

		for (usize i = start; i < _count-2; ++i)
		{
			if ( _memory[i] == tValue ) {
				pos = i;
				return true;
			}
		}
		return false;
	}
	
/*
=================================================
	FindIC
=================================================
*/
	template <typename T>
	inline bool  TStringRef<T>::FindIC (TStringRef<const T> value, usize &pos, usize start) const
	{
		if ( Empty() or value.Empty() )
			return false;

		usize	j = 0;

		for (usize i = start; i < _count-2; ++i)
		{
			while ( StringUtils::ToLower( value[j] ) == StringUtils::ToLower( _memory[i+j] ) and
					i+j < _count-1 and j < value.Length() )
			{
				++j;
				if ( j >= value.Length() ) {
					pos = i;
					return true;
				}
			}
			j = 0;
		}
		return false;
	}
	
/*
=================================================
	FindIC
=================================================
*/
	template <typename T>
	inline bool  TStringRef<T>::FindIC (const T tValue, usize &pos, usize start) const
	{
		if ( Empty() )
			return false;

		const T	val = StringUtils::ToLower( tValue );

		for (usize i = start; i < _count-2; ++i)
		{
			if ( StringUtils::ToLower( _memory[i] ) == val ) {
				pos = i;
				return true;
			}
		}
		return false;
	}
	
/*
=================================================
	SubString
=================================================
*/
	template <typename T>
	inline TStringRef<T>  TStringRef<T>::SubString (usize pos, usize count)
	{
		if ( pos >= _count )
			return TStringRef<T>();

		if ( (count >= _count) or (count + pos >= _count) )
			count = _count - pos - 1;

		return ( TStringRef<T>( _memory + pos, count ) );
	}
	
/*
=================================================
	SubString
=================================================
*/
	template <typename T>
	inline TStringRef<const T>  TStringRef<T>::SubString (usize pos, usize count) const
	{
		if ( pos >= _count )
			return TStringRef<const T>();
		
		if ( (count >= _count) or (count + pos >= _count) )
			count = _count - pos - 1;

		return ( TStringRef<const T>( _memory + pos, count ) );
	}
	
/*
=================================================
	GetInterval
=================================================
*/
	template <typename T>
	inline TStringRef<T>  TStringRef<T>::GetInterval (usize begin, usize end)
	{
		ASSERT( begin < end );
		return SubString( begin, end - begin );
	}

	template <typename T>
	inline TStringRef<const T>  TStringRef<T>::GetInterval (usize begin, usize end) const
	{
		ASSERT( begin < end );
		return SubString( begin, end - begin );
	}
	
/*
=================================================
	GetInterval
=================================================
*/
	template <typename T>
	inline TStringRef<T>  TStringRef<T>::GetInterval (const T* begin, const T* end)
	{
		return GetInterval( GetIndex(*begin), GetIndex(*end) );
	}

	template <typename T>
	inline TStringRef<const T>  TStringRef<T>::GetInterval (const T* begin, const T* end) const
	{
		return GetInterval( GetIndex(*begin), GetIndex(*end) );
	}
	
/*
=================================================
	LessThan
=================================================
*/
	template <typename T>
	inline bool  TStringRef<T>::LessThan (TStringRef<const T> right) const
	{
		typedef typename CompileTime::NearUInt::FromType<T>		UType;

		const usize	min_length = GXMath::Min( Length(), right.Length() );

		for (usize i = 0; i < min_length; ++i)
		{
			const UType a = UType( (*this)[i] );
			const UType b = UType( right[i] );

			if ( a != b )
				return a < b;
		}
		return Length() < right.Length();
	}
	
/*
=================================================
	GreaterThan
=================================================
*/
	template <typename T>
	inline bool  TStringRef<T>::GreaterThan (TStringRef<const T> right) const
	{
		typedef typename CompileTime::NearUInt::FromType<T>		UType;

		const usize	min_length = GXMath::Min( Length(), right.Length() );

		for (usize i = 0; i < min_length; ++i)
		{
			const UType a = UType( (*this)[i] );
			const UType b = UType( right[i] );

			if ( a != b )
				return a > b;
		}
		return Length() > right.Length();
	}
	
/*
=================================================
	LessThanIC
=================================================
*/
	template <typename T>
	inline bool  TStringRef<T>::LessThanIC (TStringRef<const T> right) const
	{
		typedef typename CompileTime::NearUInt::FromType<T>		UType;

		const usize	min_length = GXMath::Min( Length(), right.Length() );

		for (usize i = 0; i < min_length; ++i)
		{
			const UType a = UType( StringUtils::ToLower( (*this)[i] ) );
			const UType b = UType( StringUtils::ToLower( right[i] ) );

			if ( a != b )
				return a < b;
		}
		return Length() < right.Length();
	}
	
/*
=================================================
	GreaterThanIC
=================================================
*/
	template <typename T>
	inline bool  TStringRef<T>::GreaterThanIC (TStringRef<const T> right) const
	{
		typedef typename CompileTime::NearUInt::FromType<T>		UType;

		const usize	min_length = GXMath::Min( Length(), right.Length() );

		for (usize i = 0; i < min_length; ++i)
		{
			const UType a = UType( StringUtils::ToLower( (*this)[i] ) );
			const UType b = UType( StringUtils::ToLower( right[i] ) );

			if ( a != b )
				return a > b;
		}
		return Length() > right.Length();
	}
	
/*
=================================================
	EqualsInRange
=================================================
*/
	template <typename T>
	inline bool  TStringRef<T>::EqualsInRange (TStringRef<const T> right, usize begin, usize end) const
	{
		if ( begin < end or Length() <= end or right.Length() <= end ) {
			WARNING( "invalid args" );
			return false;
		}

		return _Equals( ptr() + begin, right.ptr(), end - begin );
	}
	
/*
=================================================
	EqualsInRangeIC
=================================================
*/
	template <typename T>
	inline bool  TStringRef<T>::EqualsInRangeIC (TStringRef<const T> right, usize begin, usize end) const
	{
		if ( begin < end or Length() <= end or right.Length() <= end ) {
			WARNING( "invalid args" );
			return false;
		}

		return _EqualsIC( ptr() + begin, right.ptr(), end - begin );
	}
	
/*
=================================================
	Intersects
=================================================
*/
	template <typename T>
	inline bool  TStringRef<T>::Intersects (TStringRef<const T> other) const
	{
		ASSERT( Begin() <= End() );
		ASSERT( other.Begin() <= other.End() );
		return Begin() > other.End() or End() < other.Begin();
	}
	
	
/*
=================================================
	StrLength
=================================================
*/
	namespace _types_hidden_
	{
		template <typename T>
		forceinline usize StrLength (const T *str)
		{
			usize	i = 0;

			while ( *str != 0 )
			{
				++str;
				++i;
			}
			return i;
		}

		template <>
		forceinline usize StrLength (const char *str)
		{
			return ::strlen( str );
		}

	}	// _types_hidden_

	
/*
=================================================
	Hash
=================================================
*/
	template <typename T>
	struct Hash< TStringRef<T> > :
		private Hash< Buffer<const T> >
	{
		typedef TStringRef<T>				key_t;
		typedef Hash< Buffer<const T> >		base_t;
		typedef typename base_t::result_t	result_t;

		result_t operator () (const TStringRef<T> &x) const
		{
			return base_t::operator ()( x );
		}
	};

}	// GXTypes
}	// GX_STL
