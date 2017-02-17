// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/STL/OS/Base/ScopeLock.h"
#include "OSWindows.h"

#ifdef PLATFORM_WINDOWS_SHELL

namespace GX_STL
{
namespace OS
{
	
	struct CriticalSection;
	struct ReadWriteSync;
	struct ConditionVariable;


	//
	// Critical Section
	//

	struct CriticalSection
	{
		friend struct ConditionVariable;

	// types
	public:
		static const uint	_CSsize = sizeof(void*) * 4 + 8;

		typedef CriticalSection							Self;
		typedef HiddenOSType< _CSsize, alignof(void*) >	Handle_t;	// CRITICAL_SECTION


	// variables
	private:
		Handle_t	_crSection;
		bool		_inited;

		
	// methods
	private:
		void _Delete ();

	public:
		CriticalSection ();
		~CriticalSection ();

		bool IsValid () const
		{
			return _inited;
		}

		void Lock ();
		bool TryLock ();
		void Unlock ();

		ScopeLock GetScopeLock ()
		{
			struct Util {
				static void Lock (void *p)		{ ((Self *)p)->Lock(); }
				static void Unlock (void *p)	{ ((Self *)p)->Unlock(); }
			};

			return ScopeLock( this, &Util::Lock, &Util::Unlock, false );
		}

		operator ScopeLock ()
		{
			return GetScopeLock();
		}
	};



	//
	// Single Read, Multiple Write (with WinXP support)
	//

	struct ReadWriteSync
	{
		friend struct ConditionVariable;

	// types
	public:
		typedef ReadWriteSync				Self;
		typedef HiddenOSTypeFrom< void* >	Handle_t;	// SRWLOCK


	// variables
	private:
		Handle_t	_srw;
		

	private:
		static bool _isInitialized;
		static void _InitSRWFuncPointers ();


	// methods
	public:
		ReadWriteSync ();
		~ReadWriteSync ();

		void LockWrite ();
		bool TryLockWrite ();
		void UnlockWrite ();

		void LockRead ();
		bool TryLockRead ();
		void UnlockRead ();

		ScopeLock GetScopeWriteLock ()
		{
			struct Util {
				static void Lock (void *p)		{ ((Self *)p)->LockWrite(); }
				static void Unlock (void *p)	{ ((Self *)p)->UnlockWrite(); }
			};
			return ScopeLock( this, &Util::Lock, &Util::Unlock, false );
		}

		ScopeLock GetScopeReadLock ()
		{
			struct Util {
				static void Lock (void *p)		{ ((Self *)p)->LockRead(); }
				static void Unlock (void *p)	{ ((Self *)p)->UnlockRead(); }
			};
			return ScopeLock( this, &Util::Lock, &Util::Unlock, false );
		}
	};



	//
	// Condition Variable
	//

	struct ConditionVariable
	{
	// types
	public:
		static const uint	MAX_WAIT_TIME	= 0xFFFFFFFF;

		typedef ConditionVariable			Self;
		typedef HiddenOSTypeFrom< void* >	Handle_t;	// CONDITION_VARIABLE


	// variables
	private:
		Handle_t	_cv;
		bool		_inited;


	// methods
	private:
		static bool _isInitialized;
		static void _InitCondVarFuncPointers ();

		bool _Create ();
		void _Destroy ();

	public:
		ConditionVariable ();
		~ConditionVariable ();

		bool IsValid () const
		{
			return _inited;
		}

		void Signal ();
		void Broadcast ();

		bool Wait (CriticalSection &cs, uint time = MAX_WAIT_TIME);
	};



	//
	// Synchronization Event
	//
	
	struct SyncEvent
	{
	// types
	public:
		typedef SyncEvent					Self;
		typedef HiddenOSTypeFrom< void* >	Handle_t;	// HANDLE

		enum EFlags {
			MANUAL_RESET		= 0,
			AUTO_RESET			= 0x1,
			INIT_STATE_SIGNALED = 0x2,
		};

		static const uint	MAX_WAIT_TIME	= 0xFFFFFFFF;


	// variables
	private:
		Handle_t	_event;


	// methods
	public:
		explicit
		SyncEvent (EFlags flags = AUTO_RESET);
		~SyncEvent ();

		bool IsValid () const
		{
			return _event.IsDefined();
		}

		bool Signal ();
		bool Reset ();
		bool Pulse ();


		bool Wait (uint time = MAX_WAIT_TIME);

		static int WaitEvents (Buffer<Self *> events, bool waitAll, uint time = MAX_WAIT_TIME);


		ScopeLock GetScopeLock()
		{
			struct Util {
				static void Lock (void *p)		{ ((Self *)p)->Wait(); }
				static void Unlock (void *p)	{ ((Self *)p)->Signal(); }
			};
			return ScopeLock( this, &Util::Lock, &Util::Unlock, false );
		}

		operator ScopeLock ()
		{
			return GetScopeLock();
		}
	
	private:
		bool _Create (EFlags flags);
		bool _Delete();
	};



	//
	// Semaphore
	//

	struct Semaphore
	{
	// types
	public:
		typedef Semaphore					Self;
		typedef HiddenOSTypeFrom< void* >	Handle_t;	// HANDLE
		
		static const uint 	MAX_WAIT_TIME	= 0xFFFFFFFF;


	// variables
	private:
		Handle_t	_sem;


	// methods
	private:
		bool _Create (uint initialValue);
		void _Destroy ();

	public:
		explicit
		Semaphore (uint initialValue);

		~Semaphore ();

		bool IsValid () const
		{
			return _sem.IsDefined();
		}

		void Lock ();

		bool TryLock ();

		void Unlock ();

		uint GetValue ();
		
		ScopeLock GetScopeLock()
		{
			struct Util {
				static void Lock (void *p)		{ ((Self *)p)->Lock(); }
				static void Unlock (void *p)	{ ((Self *)p)->Unlock(); }
			};

			return ScopeLock( this, &Util::Lock, &Util::Unlock, false );
		}

		operator ScopeLock ()
		{
			return GetScopeLock();
		}
	};


}	// OS
}	// GX_STL

#endif	// PLATFORM_WINDOWS_SHELL