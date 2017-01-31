// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Atomic.h"
#include "Engine/STL/OS/OSLowLevel.h"
#include "Engine/STL/Containers/Set.h"

namespace GX_STL
{
namespace GXTypes
{

#	define SHARED_POINTER_TYPE( _name_ ) \
		::GX_STL::GXTypes::CRefCounter< _name_, \
			::GX_STL::GXTypes::Referenced, \
			::GX_STL::GXTypes::TRefCountStrategy< ::GX_STL::GXTypes::Referenced > >

#	define SHARED_POINTER( _name_ ) \
		typedef SHARED_POINTER_TYPE( _name_ )	_name_##Ptr;


	//
	// Reference Counter Strategy
	//

	template <typename T>
	struct TRefCountStrategy	// TODO: rename
	{
		forceinline static void IncRef (T *ptr)		{}
		forceinline static void DecRef (T *ptr)		{}
		forceinline static int  Count (T *ptr)		{ return 0; }
	};



	//
	// Referenced Object
	//

	class Referenced : public CompileTime::TypeQualifier< CompileTime::ETypeQualifier::Def_Noncopyable >
	{
		friend class  CReferencedObject;
		friend struct TRefCountStrategy< Referenced >;

	// variables
	private:
		Atomic< int >		_atomicCounter;

		DEBUG_ONLY(
			static Set< Referenced *>	_objectRefs;
			static OS::Mutex			_mutex;
		)


	// methods
	private:
		void operator = (const Referenced &) {}

	private:
		forceinline void _DebugAddRef ()
		{
			DEBUG_ONLY(
				SCOPELOCK( _mutex );
				_objectRefs.Add( this );
			)
		}

		forceinline void _DebugRemoveRef ()
		{
			DEBUG_ONLY(
				SCOPELOCK( _mutex );

				usize	idx;
				if ( _objectRefs.FindIndex( this, idx ) ) {
					_objectRefs.EraseFromIndex( idx );
				}
			)
		}

	protected:
		Referenced (): _atomicCounter(0)
		{
			_DebugAddRef();
		}
		
		explicit
		Referenced (const Referenced &) : _atomicCounter(0)
		{
			_DebugAddRef();
		}
		
		Referenced (Referenced &&other) : _atomicCounter( other._atomicCounter )
		{
			other._atomicCounter = 0;
		}

		virtual ~Referenced ()
		{
			_DebugRemoveRef();
		}

		forceinline  void _AddRef ()				{ _atomicCounter.Inc(); }
		forceinline  uint _GetRefCount ()	const	{ return _atomicCounter; }
		forceinline  void _ReleaseRef ()			{ ASSERT( int(_atomicCounter) > 0 );  if ( _atomicCounter.Dec() == 0 ) _Release(); }
		virtual		 void _Release ()				{ delete this; }
		
	public:
		DEBUG_ONLY(
		static void s_ChenckNotReleasedObjects (uint refValue = 0)
		{
			SCOPELOCK( _mutex );

			if ( _objectRefs.Count() < refValue )
			{
				GX_BREAK_POINT();
			}
		})
	};


	template <>
	struct TRefCountStrategy< Referenced >	// TODO: rename
	{
		forceinline static void IncRef (Referenced *ptr)	{ ptr->_AddRef(); }
		forceinline static void DecRef (Referenced *ptr)	{ ptr->_ReleaseRef(); }
		forceinline static int  Count (Referenced *ptr)		{ return ptr->_GetRefCount(); }
	};



	//
	// Reference Counter
	//

	template <typename T, typename B, typename S = TRefCountStrategy<B> >
	class CRefCounter : public CompileTime::FastCopyable	// TODO: rename
	{
		template <typename T2, typename B2, typename S2>
		friend class CRefCounter;

	// types
	public:
		typedef CRefCounter<T,B,S>	Self;
		typedef T					value_t;
		typedef B					parent_t;

	private:
		typedef bool				_is_ref_counter;


	// variables
	private:
		T	*	_ptr;
		

	// methods
	private:
		forceinline void _Inc ()
		{
			if ( _ptr != null )
				S::IncRef( (B *)_ptr );
		}

		forceinline void _Dec ()
		{
			if ( _ptr != null )
				S::DecRef( (B *)_ptr );
		}

		template <typename T2>
		forceinline bool _CmpPtr (const T2 *p) const
		{
			return ((const void *) _ptr) == ((const void *) p);
		}

		template <typename T2, typename B2, typename S2>
		forceinline bool _CmpPtr (const CRefCounter<T2,B2,S2> &sRC) const
		{
			return _CmpPtr( sRC.RawPtr() );
		}

		template <typename ToType, typename FromType>
		static void _CheckCast (FromType *p)
		{
			ASSERT( CompileTime::TypeDescriptor::CanCastTo< ToType >( p ) );
		}


	public:
		CRefCounter (GX_DEFCTOR) : _ptr(null)
		{}


		CRefCounter (T *ptr) : _ptr(ptr)
		{
			_Inc();
		}


		CRefCounter (CRefCounter &&sRC) : _ptr(sRC._ptr)
		{
			sRC._ptr = null;
		}


		CRefCounter (const CRefCounter &sRC) : _ptr(sRC._ptr)
		{
			_Inc();
		}


		template <typename T2>
		CRefCounter (const CRefCounter<T2,B,S> &right) : _ptr(null)
		{
			_CheckCast<T>( right.RawPtr() );

			_ptr = (T*)( right.RawPtr() );
			_Inc();
		}


		~CRefCounter ()
		{
			_Dec();
		}


		template <typename T2>
		forceinline Self & operator = (const CRefCounter<T2,B,S> &right)
		{
			if ( _CmpPtr( right ) )
				return *this;

			_CheckCast<T>( right.RawPtr() );

			_Dec();
			_ptr = (T *)( right.RawPtr() );
			_Inc();
			return *this;
		}

		
		forceinline Self & operator = (const Self &right)
		{
			if ( _CmpPtr( right ) )
				return *this;

			_Dec();
			_ptr = right._ptr;
			_Inc();
			return *this;
		}
		

		template <typename T2>
		forceinline Self & operator = (T2 *right)
		{
			if ( _CmpPtr( right ) )
				return *this;
			
			_CheckCast<T>( right );

			_Dec();
			_ptr = (T *)( right );
			_Inc();
			return *this;
		}

		
		forceinline operator bool () const
		{
			return IsNotNull();
		}


		forceinline T * operator -> () const
		{
			ASSUME( _ptr != null );
			return _ptr;
		}


		forceinline T & operator * () const
		{
			ASSUME( _ptr != null );
			return *_ptr;
		}


		forceinline T * ptr () const
		{
			ASSUME( _ptr != null );
			return _ptr;
		}

		forceinline T * RawPtr () const
		{
			return _ptr;
		}


		forceinline T* & raw_ref ()
		{
			return _ptr;
		}

		forceinline T* const & raw_ref () const
		{
			return _ptr;
		}


		//const T * const c_ptr () const
		//{
		//	ASSUME( _ptr != null );
		//	return _ptr;
		//}


		forceinline bool Empty () const
		{
			return _ptr == null;
		}

		forceinline bool IsNull () const
		{
			return ( _ptr == null );
		}

		forceinline bool IsNotNull () const
		{
			return ( _ptr != null );
		}

		template <typename T2>
		forceinline T2 To ()
		{
			typedef typename T2::value_t	type_t;
			STATIC_ASSERT( typename T2::_is_ref_counter(true) );

			_CheckCast< type_t >( _ptr );
			return T2( (type_t *)_ptr );
		}

		template <typename T2>
		forceinline const T2 To () const
		{
			typedef typename T2::value_t	type_t;
			STATIC_ASSERT( typename T2::_is_ref_counter(true) );

			_CheckCast< type_t >( _ptr );
			return T2( (type_t *)_ptr );
		}

		template <typename T2>
		forceinline T2 * ToPtr () const
		{
			_CheckCast<T2>( _ptr );
			return (T2 *) ptr();
		}

		template <typename T2>
		forceinline T2 const * ToCPtr () const
		{
			_CheckCast<T2 const>( _ptr );
			return (T2 const *) ptr();
		}

		forceinline int GetRefCount () const
		{
			if ( _ptr != null )	return S::Count( (B *)_ptr );
			else				{ WARNING(""); return -1; }
		}

		
		forceinline bool operator == (const T *right) const
		{
			return _ptr == right;
		}

		
		forceinline bool operator == (const Self &right) const
		{
			return _ptr == right._ptr;
		}

		
		forceinline bool operator != (const T *right) const
		{
			return _ptr != right;
		}
		
		
		forceinline bool operator != (const Self &right) const
		{
			return _ptr != right._ptr;
		}
		

		forceinline bool operator > (const Self &right) const
		{
			return _ptr > right._ptr;
		}
		
		forceinline bool operator < (const Self &right) const
		{
			return _ptr < right._ptr;
		}
	};
	


	//
	// Referenced Object
	//

	class CReferencedObject : protected Referenced		// TODO: rename
	{
	// variables
	private:
		bool	_isDynamicObj;


	// methods
	public:
		CReferencedObject () : _isDynamicObj(true)
		{
			_SetDynamicObj( false );
		}
		

		explicit
		CReferencedObject (const CReferencedObject &other) :
			Referenced(other), _isDynamicObj(true)
		{
			_SetDynamicObj( false );
		}


		CReferencedObject (CReferencedObject &&other) :
			Referenced( other ), _isDynamicObj(true)
		{
			_SetDynamicObj( false );
		}


		~CReferencedObject ()
		{
			ASSERT( (_GetRefCount() != 0) == not _isDynamicObj );
		}


	protected:
		// create static New method and call _New method for dynamic object creation
		forceinline void * operator new (usize size)
		{
			return ::operator new( size );
		}


		// can't use delete for shared pointer
		forceinline void operator delete (void* p)
		{
			return ::operator delete( p );
		}
		

		template <typename T>
		forceinline static SHARED_POINTER_TYPE( T ) _New (T *ptr)
		{
			SHARED_POINTER_TYPE( T ) tmp = ptr;
			tmp->_SetDynamicObj( true );
			return tmp;
		}


	private:
		forceinline void _SetDynamicObj (bool isDynamic)
		{
			if ( _isDynamicObj == isDynamic )
				return;

			_isDynamicObj = isDynamic;

			if ( not _isDynamicObj )
			{
				_AddRef();
				_DebugRemoveRef();
			}
			else
			{
				_ReleaseRef();
				_DebugAddRef();
			}
		}


		// Referenced //
		virtual void _Release () override final
		{
			if ( _isDynamicObj )
			{
				delete this;
			}
		}
	};


}	// GXTypes
}	// GX_STL
