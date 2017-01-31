// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/STL/CompileTime/TypeList.h"
#include "Engine/STL/CompileTime/NewTypeInfo.h"
#include "VariantRef.h"

namespace GX_STL
{
namespace GXTypes
{

	//
	// Union Type
	//
	
	template <typename ...Types>
	struct Union : public CompileTime::CopyQualifiers< CompileTime::PODStruct, Types... >
	{
	// types
	public:
		typedef typename CompileTime::TypeListFrom< Types... >	TypeList_t;
		typedef Union< Types... >								Self;

		static const uint		INVALID_INDEX = uint(-1);
		
	private:
		struct _TypeList_Destroy;
		struct _TypeList_GetTypeId;
		struct _TypeList_GetSizeOf;
		struct _TypeList_Copy;
		struct _TypeList_Move;

		template <usize CmpType>
		struct _TypeList_Compare;
		
		template <typename Func, typename Data>
		struct _TypeList_Apply;

		static const usize	_DataSize	= TypeList_t::MaxSizeOf();
		static const usize	_DataAlign	= TypeList_t::MaxAlignOf();

		typedef typename std::aligned_union< 0, Types... >		_AlignedUnion_t;

		STATIC_ASSERT( _AlignedUnion_t::_Max_len == _DataSize );
		STATIC_ASSERT( _AlignedUnion_t::alignment_value == _DataAlign );


	// variables
	private:
		alignas(_DataAlign)	char	_data[ _DataSize ];
		uint						_currentIndex;


	// methods
	public:
		Union (GX_DEFCTOR);
		Union (const Self &other);
		Union (Self &&other);
		
		template <typename T>
		explicit Union (const T &value);

		~Union ();

		TypeId_t  GetCurrentTypeId () const;

		usize GetCurrentIndex () const;

		bool IsDefined () const;
		
		void const * GetPointer () const;

		BytesU GetSizeOf () const;

		bool IsSame (const Self &other) const;

		void Destroy ();

		VariantRef  GetVariantRef ()					{ return VariantRef::FromVoid( _data, GetCurrentTypeId() ); }


		bool operator == (const Self &right) const;
		bool operator >  (const Self &right) const;
		bool operator <  (const Self &right) const;
		bool operator != (const Self &right) const		{ return not (*this == right); }
		bool operator >= (const Self &right) const		{ return not (*this <  right); }
		bool operator <= (const Self &right) const		{ return not (*this >  right); }

		Self& operator = (const Self &right);
		Self& operator = (Self &&right);
		
		operator bool () const							{ return IsDefined(); }


		// Type Access //
		template <typename T>		Self &  Create (const T &value);
		template <typename T>		Self &  Create2 (T &&value);
		template <typename T>		Self &	Set (const T &value);

		template <typename Func>	void	Apply (Func& func);
		template <typename Func>	void	Apply (const Func& func) const;

		template <typename T>		bool	Is () const;

		template <typename T>		T &			Get ();
		template <typename T>		T const &	Get () const;
		template <typename T>		T			Get (const T &defaultValue) const;

		template <typename T>		T &			Cast ();
		template <typename T>		T const &	Cast () const;


	private:
		void _Copy (const Self &other);
		void _Move (Self &&other);

		template <typename T>
		static void _HasType ();
	};


	
	template <typename ...Types>
	struct Union<Types...>::_TypeList_Destroy
	{
		const usize	index;
		void *		ptr;

		_TypeList_Destroy (void *ptr, usize index) : ptr(ptr), index(index)
		{}

		template <typename T, usize Index>
		forceinline void Process ()
		{
			if ( index == Index )
			{
				((T *) ptr)->~T();
			}
		}
	};
	
	
	template <typename ...Types>
	struct Union<Types...>::_TypeList_GetTypeId
	{
		TypeId_t	id;
		const usize	index;

		_TypeList_GetTypeId (usize index) : index(index)
		{}
			
		template <typename T, usize Index>
		forceinline void Process ()
		{
			if ( index == Index )
			{
				id = TypeId<T>();
			}
		}
	};
	
	
	template <typename ...Types>
	struct Union<Types...>::_TypeList_GetSizeOf
	{
		usize		sizeOf;
		const usize	index;

		_TypeList_GetSizeOf (usize index) : index(index), sizeOf(0)
		{}
			
		template <typename T, usize Index>
		forceinline void Process ()
		{
			if ( index == Index )
			{
				sizeOf = sizeof( T );
			}
		}
	};
	
	
	template <typename ...Types>
	template <typename Func, typename Data>
	struct Union<Types...>::_TypeList_Apply
	{
		Data		data;
		Func		func;
		const usize	index;

		_TypeList_Apply (Func func, Data data, usize index) :
			func(func), data(data), index(index)
		{}

		template <typename T, usize Index>
		forceinline void Process ()
		{
			typedef TypeTraits::CopyConstToPointer< Data, T *>	result_t;

			if ( index == Index )
			{
				func( *reinterpret_cast< result_t >( data ) );
			}
		}
	};
	
	
	template <typename ...Types>
	struct Union<Types...>::_TypeList_Copy
	{
		Self const& left;
		Self &		right;

		_TypeList_Copy (Self const &left, Self &right) :
			left(left), right(right)
		{}
			
		template <typename T, usize Index>
		forceinline void Process ()
		{
			if ( left.GetCurrentIndex() == Index )
			{
				right.Create( left.Get<T>() );
			}
		}
	};
	
	
	template <typename ...Types>
	struct Union<Types...>::_TypeList_Move
	{
		Self &		left;
		Self &		right;

		_TypeList_Move (Self &left, Self &right) :
			left(left), right(right)
		{}
			
		template <typename T, usize Index>
		forceinline void Process ()
		{
			if ( left.GetCurrentIndex() == Index )
			{
				right.Create2( RVREF( left.Get<T>() ) );
			}
		}
	};
	
	
	template <typename ...Types>
	template <usize CmpType>
	struct Union<Types...>::_TypeList_Compare
	{
		Self const&	left;
		Self const&	right;
		bool		result;

		_TypeList_Compare (Self const& left, Self const& right) :
			left(left), right(right), result(false)
		{}

		template <typename T, usize Index>
		forceinline void Process ()
		{
			if ( left.GetCurrentIndex() == Index )
			{
				if ( CmpType == 0 )
				{
					result = ( left.Get<T>() == right.Get<T>() );
				}
				else
				if ( CmpType == 1 )
				{
					result = ( left.Get<T>() <  right.Get<T>() );
				}
				else
				if ( CmpType == 2 )
				{
					result = ( left.Get<T>() <  right.Get<T>() );
				}

				STATIC_ASSERT( CmpType < 3 );
			}
		}
	};

	
	template <typename ...Types>
	inline Union<Types...>::Union (UninitializedType) :
		_currentIndex(INVALID_INDEX)
	{
		DEBUG_ONLY( ZeroMem( _data ) );
	}

		
	template <typename ...Types>
	inline Union<Types...>::Union (const Self &other) :
		_currentIndex(INVALID_INDEX)
	{
		DEBUG_ONLY( ZeroMem( _data ) );

		_Copy( other );
	}

		
	template <typename ...Types>
	inline Union<Types...>::Union (Self &&other) :
		_currentIndex(other._currentIndex)
	{
		DEBUG_ONLY( ZeroMem( _data ) );

		_Move( RVREF( other ) );
	}
		
		
	template <typename ...Types>
	template <typename T>
	inline Union<Types...>::Union (const T &value) :
		_currentIndex(INVALID_INDEX)
	{
		Create( value );
	}
		
	template <typename ...Types>
	inline Union<Types...>::~Union ()
	{
		Destroy();
	}

	
	template <typename ...Types>
	forceinline TypeId_t  Union<Types...>::GetCurrentTypeId () const
	{
		// TODO: cache this value
		_TypeList_GetTypeId		func( _currentIndex );
		TypeList_t::RuntimeForEach( func );
		return func.id;
	}
		
	
	template <typename ...Types>
	inline usize Union<Types...>::GetCurrentIndex () const
	{
		return _currentIndex;
	}

	
	template <typename ...Types>
	inline bool Union<Types...>::IsDefined () const
	{
		return _currentIndex != INVALID_INDEX;
	}

	
	template <typename ...Types>
	inline void const * Union<Types...>::GetPointer () const
	{
		return _data;
	}

	
	template <typename ...Types>
	forceinline BytesU Union<Types...>::GetSizeOf () const
	{
		_TypeList_GetSizeOf		func( _currentIndex );
		TypeList_t::RuntimeForEach( func );
		return BytesU( func.sizeOf );
	}

	
	template <typename ...Types>
	template <typename T>
	forceinline Union<Types...> &  Union<Types...>::Create (const T &value)
	{
		_HasType<T>();
		Destroy();

		STATIC_ASSERT( sizeof(value) <= sizeof(_data) );

		_currentIndex = TypeList_t::IndexOf<T>;
			
		UnsafeMem::PlacementNew<T>( _data, value );
		return *this;
	}

		
	template <typename ...Types>
	template <typename T>
	forceinline Union<Types...> &  Union<Types...>::Create2 (T &&value)
	{
		_HasType<T>();
		Destroy();

		_currentIndex = TypeList_t::IndexOf<T>;
			
		UnsafeMem::PlacementNew<T>( _data, RVREF( value ) );
		return *this;
	}
		
	
	template <typename ...Types>
	template <typename T>
	forceinline bool Union<Types...>::Is () const
	{
		_HasType<T>();
		return _currentIndex == TypeList_t::IndexOf<T>;
	}

	
	template <typename ...Types>
	forceinline bool Union<Types...>::IsSame (const Self &other) const
	{
		return this->GetCurrentIndex() == other.GetCurrentIndex();
	}

	
	template <typename ...Types>
	template <typename T>
	forceinline T &  Union<Types...>::Get ()
	{
		ASSERT( Is<T>() );
		return *PointerCast< T >( _data );
	}
	

	template <typename ...Types>
	template <typename T>
	forceinline T const &  Union<Types...>::Get () const
	{
		ASSERT( Is<T>() );
		return *PointerCast< T >( _data );
	}

	
	template <typename ...Types>
	template <typename T>
	forceinline T  Union<Types...>::Get (const T &defaultValue) const
	{
		return Is<T>() ? *PointerCast< T >( _data ) : defaultValue;
	}

	
	template <typename ...Types>
	template <typename T>
	forceinline T &  Union<Types...>::Cast ()
	{
		_HasType<T>();
		ASSERT( IsDefined() );
		return *PointerCast< T >( _data );
	}

	
	template <typename ...Types>
	template <typename T>
	forceinline T const &  Union<Types...>::Cast () const
	{
		_HasType<T>();
		ASSERT( IsDefined() );
		return *PointerCast< T >( _data );
	}

	
	template <typename ...Types>
	template <typename T>
	forceinline Union<Types...> &  Union<Types...>::Set (const T &value)
	{
		Get<T>() = value;
		return *this;
	}

	
	template <typename ...Types>
	template <typename Func>
	forceinline void Union<Types...>::Apply (Func& func)
	{
		ASSERT( IsDefined() );
		_TypeList_Apply< Func&, void* >	iter( func, _data, _currentIndex );
		TypeList_t::RuntimeForEach( iter );
	}

	
	template <typename ...Types>
	template <typename Func>
	forceinline void Union<Types...>::Apply (const Func& func) const
	{
		ASSERT( IsDefined() );
		_TypeList_Apply< const Func&, void const * const >	iter( func, _data, _currentIndex );
		TypeList_t::RuntimeForEach( iter );
	}

	
	template <typename ...Types>
	forceinline bool Union<Types...>::operator == (const Self &right) const
	{
		if ( not IsSame( right ) )
			return false;

		_TypeList_Compare<0>	func( *this, right );
		TypeList_t::RuntimeForEach( func );
		return func.result;
	}

	
	template <typename ...Types>
	forceinline bool Union<Types...>::operator >  (const Self &right) const
	{
		if ( not IsSame( right ) )
			return false;
		
		_TypeList_Compare<1>	func( *this, right );
		TypeList_t::RuntimeForEach( func );
		return func.result;
	}

	
	template <typename ...Types>
	forceinline bool Union<Types...>::operator <  (const Self &right) const
	{
		if ( not IsSame( right ) )
			return false;
		
		_TypeList_Compare<2>	func( *this, right );
		TypeList_t::RuntimeForEach( func );
		return func.result;
	}
	

	template <typename ...Types>
	forceinline Union<Types...>&  Union<Types...>::operator = (const Self &right)
	{
		Destroy();
		_Copy( right );
		return *this;
	}


	template <typename ...Types>
	forceinline Union<Types...>&  Union<Types...>::operator = (Self &&right)
	{
		Destroy();
		_Move( RVREF( right ) );
		return *this;
	}

	
	template <typename ...Types>
	forceinline void Union<Types...>::Destroy ()
	{
		if ( not IsDefined() )
			return;
		
		if ( CompileTime::IsDtorAvailable< Self > )
		{
			_TypeList_Destroy	func( _data, _currentIndex );
			TypeList_t::RuntimeForEach( func );
		}

		DEBUG_ONLY( ZeroMem( _data ) );
		_currentIndex	= INVALID_INDEX;
	}

	
	template <typename ...Types>
	template <typename T>
	forceinline void Union<Types...>::_HasType ()
	{
		STATIC_ASSERT( (TypeList_t::HasType<T>), "type not presented in union" );
	}
	

	template <typename ...Types>
	forceinline void Union<Types...>::_Copy (const Self &other)
	{
		if ( not other.IsDefined() )
			return;

		if ( CompileTime::IsMemCopyAvailable< Self > )
		{
			UnsafeMem::MemMove( _data, other._data, SizeOf(_data) );
			_currentIndex = other._currentIndex;
		}
		else
		{
			_TypeList_Copy	func( other, *this );
			TypeList_t::RuntimeForEach( func );
		}
	}


	template <typename ...Types>
	forceinline void Union<Types...>::_Move (Self &&other)
	{
		if ( not other.IsDefined() )
			return;
		
		if ( CompileTime::IsMemCopyAvailable< Self > )
		{
			UnsafeMem::MemMove( _data, other._data, SizeOf(_data) );
			other._currentIndex = INVALID_INDEX;
		}
		else
		{
			_TypeList_Move	func( other, *this );
			TypeList_t::RuntimeForEach( func );
		}
	}


	
	template <typename ...Types>
	struct Hash< Union<Types...> > :
		private Hash< BinaryBuffer >
	{
		typedef Union<Types...>				key_t;
		typedef Hash< BinaryBuffer >		base_t;
		typedef typename base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( BinaryBuffer::FromVoid( x.GetPointer(), x.GetSizeOf() ) );
		}
	};

}	// GXTypes
}	// GX_STL
