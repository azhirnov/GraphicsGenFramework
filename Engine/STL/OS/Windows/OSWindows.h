// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/STL/OS/Base/Common.h"

#ifdef PLATFORM_WINDOWS

#if defined( PLATFORM_SDL )
	// disable WinAPI Shells
#	undef PLATFORM_WINDOWS_SHELL
#	include "Engine/STL/OS/SDL/OS_SDL.h"
#else
	// enable WinAPI Shells
#	define PLATFORM_WINDOWS_SHELL
#endif


namespace GX_STL
{
namespace OS
{
	
	//
	// Hidden OS Type without destructor
	//
	
	template <usize Size, usize Align, bool WithDestructor = false>
	struct HiddenOSType : Noncopyable
	{
	// types
	private:
		typedef HiddenOSType< Size, Align, false >	Self;


	// variables
	protected:
		alignas(Align)	char	_data[ Size ];
		const TypeId_t			_typeid;
		bool					_isDefined;


	// methods
	public:
		template <typename T>
		explicit
		HiddenOSType (UninitializedT<T>) : _typeid( TypeId<T>() ), _isDefined(false)
		{
			_Create<T>();
		}

		template <typename T>
		explicit
		HiddenOSType (const T &value) : _typeid( TypeId<T>() ), _isDefined(false)
		{
			_Create( value );
		}

		HiddenOSType (const Self &other) :
			_typeid( other._typeid ),
			_isDefined( other._isDefined )
		{
			UnsafeMem::MemCopy( _data, other._data, SizeOf(_data) );
		}

		~HiddenOSType ()
		{
			_Destroy();
		}

		
		template <typename T>
		forceinline Self& operator = (const T &value)
		{
			ASSERT( TypeId<T>() == _typeid );

			// destructor not needed
			_Create( value );
			return *this;
		}

		forceinline Self& operator = (NullPtr_t value)
		{
			_Destroy();
			return *this;
		}

		forceinline Self& operator = (const Self &other)
		{
			ASSERT( _typeid == other._typeid );

			UnsafeMem::MemCopy( _data, other._data, SizeOf(_data) );
			_isDefined = other._isDefined;
			return *this;
		}


		template <typename T>
		forceinline T&  Get ()
		{
			ASSERT( IsDefined() and TypeId<T>() == _typeid );
			return ReferenceCast<T>( _data );
		}

		template <typename T>
		forceinline T const&  Get () const
		{
			ASSERT( IsDefined() and TypeId<T>() == _typeid );
			return ReferenceCast<T>( _data );
		}


		forceinline bool IsDefined () const
		{
			return _isDefined;
		}
		
		template <typename T>
		forceinline bool IsNull () const
		{
			return not IsNotNull<T>();
		}

		template <typename T>
		forceinline bool IsNotNull () const
		{
			ASSERT( TypeId<T>() == _typeid );
			return IsDefined() and ReferenceCast<T>( _data ) != 0;
		}


	protected:
		explicit HiddenOSType (TypeId_t typeId) : _typeid(typeId), _isDefined(false)
		{}

		template <typename T>
		forceinline void _Create ()
		{
			PlacementNew<T>( Buffer<char>( _data ) );
			_isDefined = true;
		}

		template <typename T>
		forceinline void _Create (const T &value)
		{
			PlacementNew<T>( Buffer<char>( _data ), value );
			_isDefined = true;
		}

		forceinline void _Destroy ()
		{
			if ( IsDefined() )
			{
				DEBUG_ONLY( ZeroMem( _data ) );

				_isDefined = false;
			}
		}
	};
	


	//
	// Hidden OS Type with destructor
	//
	
	template <usize Size, usize Align>
	struct HiddenOSType< Size, Align, true > : HiddenOSType< Size, Align, false >
	{
	// types
	private:
		typedef HiddenOSType< Size, Align, true >	Self;
		typedef HiddenOSType< Size, Align, false >	Base_t;
		
		typedef void (*Destructor_t) (void *);


	// variables
	private:
		Destructor_t	_destructor;


	// methods
	public:
		template <typename T>
		explicit
		HiddenOSType (UninitializedT<T>) : Base_t( TypeId<T>() ), _destructor(null)
		{
			_Create<T>();
		}

		template <typename T>
		explicit
		HiddenOSType (const T &value) : Base_t( TypeId<T>() ), _destructor(null)
		{
			_Create( value );
		}

		HiddenOSType (const Self &other) = delete;

		~HiddenOSType ()
		{
			if ( IsDefined() and _destructor != null )
				_destructor( _data );
		}

		
		template <typename T>
		forceinline Self& operator = (const T &value)
		{
			ASSERT( TypeId<T>() == _typeid );

			_Destroy();
			_Create( value );

			return *this;
		}

		forceinline Self& operator = (NullPtr_t value)
		{
			_Destroy();
			return *this;
		}
		
		forceinline Self& operator = (const Self &other) = delete;


	private:
		template <typename T>
		forceinline void _Create ()
		{
			Base_t::_Create<T>();

			_destructor	= &_CallDestructor<T>;
		}


		template <typename T>
		forceinline void _Create (const T &value)
		{
			Base_t::_Create( value );

			_destructor	= &_CallDestructor<T>;
		}

		forceinline void _Destroy ()
		{
			if ( IsDefined() and _destructor != null )
				_destructor( _data );

			Base_t::_Destroy();

			_destructor = null;
		}
	};



	//
	// Hidden OS Type from same type
	//

	template <typename T, bool WithDestructor = false>
	using HiddenOSTypeFrom = HiddenOSType< sizeof(T), alignof(T), WithDestructor >;


}	// OS
}	// GX_STL

#endif	// PLATFORM_WINDOWS
