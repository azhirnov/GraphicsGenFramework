// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "OS_SDL.h"

#ifdef PLATFORM_SDL

namespace GX_STL
{
namespace _platform_
{


	struct EThreadPriority
	{
		enum type
		{
			Low				= SDL_THREAD_PRIORITY_LOW,
			BelowNormal		= SDL_THREAD_PRIORITY_LOW,
			Normal			= SDL_THREAD_PRIORITY_NORMAL,
			AboveNormal		= SDL_THREAD_PRIORITY_HIGH,
			High			= SDL_THREAD_PRIORITY_HIGH
		};
	};



	//
	// Current Thread
	//

	struct CurrentThread
	{
	// types
	protected:
		static const usize	INVALID_ID = -1;


	// variables
	protected:
		usize	_id;


	// methods
	protected:
		explicit
		CurrentThread (usize id) : _id(id)
		{}

	public:
		CurrentThread (): _id(INVALID_ID)
		{
			_id = ::SDL_ThreadID();
		}

		usize Id () const
		{
			return _id;
		}

		bool IsValid () const
		{
			return _id != INVALID_ID;
		}

		bool IsCurrent () const
		{
			return GetCurrentThreadId() == _id;
		}

		bool SetPriority (EThreadPriority::type priority) const
		{
			ASSERT( IsValid() );
			ASSERT( IsCurrent() );

			if ( IsCurrent() )
			{
				_SetCurrentThreadPriority( priority );
				return true;
			}
			return false;
		}

		static usize GetCurrentThreadId ()
		{
			return ::SDL_ThreadID();
		}

		static void Sleep (uint timeMS)
		{
			::SDL_Delay( timeMS );
		}

		static void Yield ()
		{
			Sleep( 1 );
		}

	private:
		static void _SetCurrentThreadPriority (EThreadPriority::type priority)
		{
			::SDL_SetThreadPriority( (SDL_ThreadPriority) priority );
		}
	};



	//
	// Thread
	//

	struct Thread : CurrentThread
	{
	// types
	public:
		typedef void (*PThreadProc_t) (void *param);


	// variables
	private:
		Ptr< SDL_Thread >	_thread;
		PThreadProc_t		_proc;
		void *				_parameter;


	// methods
	private:
		static int SDLCALL _ThreadProcWrapper (void *param)
		{
			Thread *	thread = (Thread *)param;
			thread->_proc( thread->_parameter );
			return 0;
		}

	public:
		Thread () : _thread(), CurrentThread(INVALID_ID)
		{}

		~Thread ()
		{
		}

		bool Create (PThreadProc_t proc, void *param)
		{
			ASSERT( not IsValid() );
			
			_parameter	= param;
			_proc		= proc;
			_thread		= ::SDL_CreateThread( _ThreadProcWrapper, null, this );
			_id			= ::SDL_GetThreadID( _thread );
			return IsValid();
		}

		void Delete ()
		{
			_proc		= null;
			_parameter	= null;
		}

		bool IsValid () const
		{
			return _thread.IsNotNull();
		}

		uint Id () const
		{
			return _id;
		}

		bool IsCurrent () const
		{
			return GetCurrentThreadId() == Id();
		}
		
		bool Terminate ()
		{
			// not supported in SDL
			return false;
		}

		void Wait ()
		{
			ASSERT( IsValid() );
			::SDL_WaitThread( _thread, null );
			_thread = null;
			_id		= INVALID_ID;
		}
	};

}	// _platform_
}	// GX_STL

#endif	// PLATFORM_SDL