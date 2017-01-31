// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "OSWindows.h"

#ifdef PLATFORM_WINDOWS_SHELL

namespace GX_STL
{
namespace OS
{

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
	// types
	protected:
		typedef HiddenOSTypeFrom< void* >	Handle_t;	// HANDLE


	// varables
	protected:
		Handle_t	_thread;
		usize		_id;


	// methods
	protected:
		explicit
		CurrentThread (const Handle_t &thread, usize id) : _thread(thread), _id(id)
		{}

	public:
		CurrentThread ();

		usize Id () const
		{
			return _id;
		}

		bool IsCurrent () const
		{
			return GetCurrentThreadId() == _id;
		}

		bool IsValid () const;

		bool SetPriority (EThreadPriority::type priority) const;


		static usize GetCurrentThreadId ();

		static void Sleep (TimeU time);

		static void Yield ()
		{
			Sleep( TimeU::FromMilliSeconds(1) );
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

		static const uint	MAX_WAIT_TIME		= 0xFFFFFFFF;
		static const uint	UNKNOWN_EXIT_CODE	= -1;


	// variables
	private:
		PThreadProc_t	_proc;
		void *			_parameter;


	// methods
	private:
		static uint CALLBACK _ThreadProcWrapper (void *param);

	public:
		Thread ();

		explicit
		Thread (PThreadProc_t proc, void *param = null);
		
		~Thread ();

		bool Create (PThreadProc_t proc, void *param = null);
		bool Delete ();
		
		// call in ThreadProc only
		void Exit (uint exitCode = UNKNOWN_EXIT_CODE);

		//!!! not destroy objects in ThreadProc
		bool Terminate ();

		bool Wait () const;

	private:
		uint _GetExitCode () const;

		bool _Wait (uint time = MAX_WAIT_TIME) const;
	};
	

}	// OS
}	// GX_STL

#endif	// PLATFORM_WINDOWS_SHELL