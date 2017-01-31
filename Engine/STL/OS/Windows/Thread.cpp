// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Thread.h"

#ifdef PLATFORM_WINDOWS_SHELL

#include "WinHeader.h"
#include <process.h>

namespace GX_STL
{
namespace OS
{
	
/*
=================================================
	constructor
=================================================
*/
	CurrentThread::CurrentThread () :
		_thread( UninitializedT<HANDLE>() ), _id(0)
	{
		_thread	= ::GetCurrentThread();
		_id		= GetCurrentThreadId();
	}
	
/*
=================================================
	IsValid
=================================================
*/
	bool CurrentThread::IsValid () const
	{
		return _thread.IsNotNull<HANDLE>();
	}
	
/*
=================================================
	SetPriority
=================================================
*/
	bool CurrentThread::SetPriority (EThreadPriority::type priority) const
	{
		return SetThreadPriority( _thread.Get<HANDLE>(), priority ) == TRUE;
	}
	
/*
=================================================
	GetCurrentThreadId
=================================================
*/
	usize CurrentThread::GetCurrentThreadId ()
	{
		return ::GetCurrentThreadId();
	}
	
/*
=================================================
	Sleep
=================================================
*/
	void CurrentThread::Sleep (TimeU time)
	{
		::Sleep( time.MilliSeconds() );
	}

//-----------------------------------------------------------------------------
	

	
/*
=================================================
	constructor
=================================================
*/
	Thread::Thread () :
		CurrentThread( Handle_t( UninitializedT<HANDLE>() ), 0),
		_proc(null), _parameter(null)
	{}
	
/*
=================================================
	constructor
=================================================
*/
	Thread::Thread (PThreadProc_t proc, void *param) :
		CurrentThread( Handle_t( UninitializedT<HANDLE>() ), 0),
		_proc(null), _parameter(null)
	{
		Create( proc, param );
	}
		
/*
=================================================
	destructor
=================================================
*/
	Thread::~Thread ()
	{
		Delete();
	}
	
/*
=================================================
	_ThreadProcWrapper
=================================================
*/
	uint CALLBACK Thread::_ThreadProcWrapper (void *param)
	{
		Thread *	thread = (Thread *)param;
		thread->_proc( thread->_parameter );
		return 0;
	}
	
/*
=================================================
	Create
=================================================
*/
	bool Thread::Create (PThreadProc_t proc, void *param)
	{
		Delete();

		uint	tmp	= 0;
		_parameter	= param;
		_proc		= proc;
		_thread		= (HANDLE) ::_beginthreadex( null, 0, &_ThreadProcWrapper, this, 0, &tmp );
		_id			= tmp;
		return IsValid();
	}
	
/*
=================================================
	Delete
=================================================
*/
	bool Thread::Delete ()
	{
		bool	ret = true;

		if ( IsValid() and not _Wait(10000) )
		{
			WARNING( "not complete the function ThreadProc" );
			Terminate();
		}

		if ( IsValid() )
		{
			ret		= ::CloseHandle( _thread.Get<HANDLE>() ) == TRUE;
			_thread = null;
			_id		= 0;
		}

		_proc		= null;
		_parameter	= null;

		return ret;
	}
		
/*
=================================================
	Exit
=================================================
*/
	void Thread::Exit (uint exitCode)
	{
		ASSERT( IsCurrent() );
		_endthreadex( exitCode == UNKNOWN_EXIT_CODE ? _GetExitCode() : exitCode );
		_id = 0;
	}
	
/*
=================================================
	Terminate
=================================================
*/
	bool Thread::Terminate ()
	{
		_id = 0;
		ASSERT( IsValid() );
		return IsValid() and TerminateThread( _thread.Get<HANDLE>(), _GetExitCode() ) == TRUE;
	}
	
/*
=================================================
	Wait
=================================================
*/
	bool Thread::Wait () const
	{
		return _Wait();
	}
	
/*
=================================================
	_GetExitCode
=================================================
*/
	uint Thread::_GetExitCode () const
	{
		DWORD	dwExitCode = 0;
		ASSERT( IsValid() );
		::GetExitCodeThread( _thread.Get<HANDLE>(), &dwExitCode );
		return (uint)dwExitCode;
	}
	
/*
=================================================
	_Wait
=================================================
*/
	bool Thread::_Wait (uint time) const
	{
		return not IsValid() or ::WaitForSingleObject( _thread.Get<HANDLE>(), time ) == WAIT_OBJECT_0;
	}

//-----------------------------------------------------------------------------


}	// OS
}	// GX_STL

#endif	// PLATFORM_WINDOWS_SHELL
