// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Defines/Init.h"

namespace GX_STL
{
namespace GXTypes
{

	//
	// Unique Ptr Strategy
	//

	template <typename T>
	struct TUniquePtrStrategy
	{
		static void Delete(T *ptr)	{ delete ptr; }
	};



	//
	// Unique Ptr
	//

	template <typename T, typename B = T, typename S = TUniquePtrStrategy<B> >
	struct UniquePtr : public CompileTime::FastCopyable
	{
	// types
	public:
		typedef UniquePtr< T, B, S >	Self;
		typedef T						value_t;
		

	// variables
	private:
		T	*_ptr;


	// methods
	private:
		void _Delete()
		{
			if ( _ptr != null )
			{
				S::Delete( (B*)_ptr );
				_ptr = null;
			}
		}


	public:
		UniquePtr (GX_DEFCTOR) : _ptr(null)
		{}


		explicit UniquePtr (T *p) : _ptr(p)
		{}
		

		UniquePtr (Self &&other) : _ptr(other._ptr)
		{
			other._ptr = null;
		}


		~UniquePtr()
		{
			_Delete();
		}


		template <typename T2>
		Self & operator = (T2 *right)
		{
			if ( _ptr == right ) {
				WARNING( "" );
				return *this;
			}

			_Delete();
			_ptr = right;
			return *this;
		}


		Self & operator = (T *right)
		{
			if ( right != null and _ptr == right ) {
				WARNING( "" );
				return *this;
			}

			_Delete();
			_ptr = right;
			return *this;
		}


		template <typename T2>
		Self & operator = (UniquePtr<T2,B,S> &right)
		{
			if ( (void*)_ptr == (void*)right._ptr ) {
				WARNING( "" );
				return *this;
			}

			_Delete();
			_ptr = right._ptr;
			right._ptr = null;
			return *this;
		}


		Self & operator = (Self &right)
		{
			if ( _ptr == right._ptr ) {
				WARNING( "" );
				return *this;
			}

			_Delete();
			_ptr = right._ptr;
			right._ptr = null;
			return *this;
		}


		T * operator -> () const
		{
			ASSUME( _ptr != null );
			return _ptr;
		}


		T & operator * () const
		{
			ASSUME( _ptr != null );
			return *_ptr;
		}


		T *& ref()
		{
			return _ptr;
		}


		T * ptr()
		{
			ASSUME( _ptr != null );
			return _ptr;
		}

		const T * const ptr() const
		{
			ASSUME( _ptr != null );
			return _ptr;
		}


		bool Empty() const
		{
			return ( _ptr == null );
		}


		bool IsNull() const
		{
			return ( _ptr == null );
		}


		bool IsNotNull() const
		{
			return not IsNull();
		}


		void Reset()
		{
			_ptr = null;
		}


		template <typename T2>
		UniquePtr<T2,B,S> To()
		{
			return UniquePtr<T2,B,S>(this);
		}
		

		bool operator == (const T *right) const
		{
			return _ptr == right;
		}

		
		bool operator == (const Self &right) const
		{
			return _ptr == right._ptr;
		}

		
		bool operator != (const T *right) const
		{
			return _ptr != right;
		}
		

		bool operator != (const Self &right) const
		{
			return _ptr != right._ptr;
		}

		
		bool operator ! () const
		{
			return _ptr != null;
		}


		template <typename T2>
		static Self CreateCopy (const T2& value)
		{
			return Self( new T( value ) );
		}
	};

	
	template <typename T, typename B, typename S>
	struct Hash< UniquePtr< T, B, S > > :
		private Hash< typename UniquePtr<T,B,S>::value_t const * >
	{
		typedef UniquePtr< T, B, S >						key_t;
		typedef Hash< typename key_t::value_t const * >		base_t;
		typedef typename base_t::result_t					result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x.ptr() );
		}
	};

}	// GXTypes
}	// GX_STL
