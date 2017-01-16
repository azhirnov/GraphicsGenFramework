// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "OSWindows.h"

#ifdef PLATFORM_WINDOWS_SHELL

namespace GX_STL
{
namespace _platform_
{
	using namespace winapi;
	

	struct EThreadPriority
	{
		enum type
		{
			Low				= -2,
			BelowNormal		= -1,
			Normal			=  0,
			AboveNormal		=  1,
			High			=  2
		};
	};



	//
	// Current Thread
	//

	struct CurrentThread
	{
	// varables
	protected:
		HANDLE	_thread;
		uint	_id;


	// methods
	protected:
		explicit
		CurrentThread (HANDLE thread, uint id) : _thread(thread), _id(id)
		{}

	public:
		CurrentThread () : _thread(null), _id(0)
		{
			_thread	= GetCurrentThread();
			_id		= (uint) GetCurrentThreadId();
		}

		usize Id () const
		{
			return _id;
		}

		bool IsValid () const
		{
			return _thread != null;
		}

		bool IsCurrent () const
		{
			return GetCurrentThreadId() == _id;
		}

		bool SetPriority (EThreadPriority::type priority) const
		{
			return SetThreadPriority( _thread, priority ) == TRUE;
		}

		static usize GetCurrentThreadId ()
		{
			return winapi::GetCurrentThreadId();
		}

		static void Sleep (uint timeMS)
		{
			winapi::Sleep( timeMS );
		}

		static void Yield ()
		{
			Sleep( 1 );
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

		enum {
			MAX_WAIT_TIME		= 0xFFFFFFFF,
			UNKNOWN_EXIT_CODE	= -1
		};


	// variables
	private:
		PThreadProc_t	_proc;
		void *			_parameter;


	// methods
	private:
		static uint CALLBACK _ThreadProcWrapper (void *param)
		{
			Thread *	thread = (Thread *)param;
			thread->_proc( thread->_parameter );
			return 0;
		}

	public:
		Thread () :
			CurrentThread(null, 0), _proc(null), _parameter(null)
		{}

		explicit
		Thread (PThreadProc_t proc, void *param = null) :
			CurrentThread(null, 0), _proc(null), _parameter(null)
		{
			Create( proc, param );
		}
		
		~Thread ()
		{
			Delete();
		}

		bool Create (PThreadProc_t proc, void *param = null)
		{
			Delete();
			_parameter	= param;
			_proc		= proc;
			_thread		= (HANDLE) _beginthreadex( null, 0, &_ThreadProcWrapper, this, 0, &_id );
			return IsValid();
		}

		bool Delete ()
		{
			bool	ret = true;

			if ( IsValid() and not _Wait(10000) ) {
				WARNING( "not complete the function ThreadProc" );
				Terminate();
			}
			if ( _thread != null ) {
				ret = CloseHandle( _thread ) == TRUE;
				_thread = null;
				_id		= 0;
			}
			_proc		= null;
			_parameter	= null;

			return ret;
		}
		
		// call in ThreadProc only
		void Exit (uint exitCode = UNKNOWN_EXIT_CODE)
		{
			ASSERT( IsCurrent() );
			_endthreadex( exitCode == UNKNOWN_EXIT_CODE ? _GetExitCode() : exitCode );
			_id = 0;
		}

		//!!! not destroy objects in ThreadProc
		bool Terminate ()
		{
			_id = 0;
			ASSERT( IsValid() );
			return IsValid() and TerminateThread( _thread, _GetExitCode() ) == TRUE;
		}

		bool Wait () const
		{
			return _Wait();
		}

	private:
		uint _GetExitCode () const
		{
			DWORD	dwExitCode = 0;
			ASSERT( IsValid() );
			GetExitCodeThread( _thread, &dwExitCode );
			return (uint)dwExitCode;
		}

		bool _Wait (uint time = MAX_WAIT_TIME) const
		{
			return not IsValid() or WaitForSingleObject( _thread, time ) == WAIT_OBJECT_0;
		}
	};
	

}	// _platform_
}	// GX_STL

#endif	// PLATFORM_WINDOWS_SHELL