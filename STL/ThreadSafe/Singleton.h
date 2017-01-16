// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../OS/OSLowLevel.h"
#include "../Types/Ptr.h"

namespace GX_STL
{
namespace GXTypes
{

	//
	// Singleton for multi threading
	//

#	ifdef GX_THREADSAFE_STATIC_SUPPORTED
	
	struct SingletonMultiThread
	{
	public:
		template <typename T>
		static Ptr<T> Instance ()
		{
			static T inst;
			return &inst;
		}
	};

#	else

	struct SingletonMultiThread
	{
	protected:
		static Mutex	_mutex;

		template <typename T>
		static T & _Instance ()
		{
			static T inst;
			return inst;
		}


	public:
		template <typename T>
		static Ptr<T> Instance ()
		{
			static T * volatile	inst;

			if ( inst == null )
			{
				T *	temp = null;
				{
					SCOPELOCK( _mutex );
					temp = & _Instance<T>();
				}
				inst = temp;
			}
			return inst;
		}
	};

#endif	// GX_THREADSAFE_STATIC_SUPPORTED



	//
	// Singleton per thread
	//

#	ifdef GX_THREADSAFE_STATIC_SUPPORTED
	
	struct SingletonPerThread
	{
	public:
		template <typename T>
		static Ptr<T> Instance ()
		{
			static GX_THREAD_LOCAL T  inst;
			return &inst;
		}
	};

#	else

	template <typename T>
	struct TSingletonLocalConstructor
	{
		static T *&	Get (T *&ptr)				{ return ( ptr != null ? ptr : (ptr = new T()) ); }
		static void Set (T *&ptr, T *newPtr)	{ if (ptr != null) { delete ptr; }  ptr = newPtr; }
	};


	struct SingletonPerThread
	{
	protected:
		template <typename T>
		static T *& _Instance()
		{
			static GX_THREAD_LOCAL T *	inst = null;
			return inst;
		}


	public:
		template <typename T>
		static Ptr<T> Instance()
		{
			T *&	ptr = _Instance<T>();
			return ( ptr != null ? ptr : (ptr = new T()) );
		}
	};

#	endif	// GX_THREADSAFE_STATIC_SUPPORTED



	//
	// Singleton for single thread
	//

	struct SingletonSingleThread
	{
	public:
		template <typename T>
		static T * Instance()
		{
			static T inst;
			return &inst;
		}
	};


}	// GXTypes
}	// GX_STL
