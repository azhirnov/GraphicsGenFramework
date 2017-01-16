// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Base/SyncPrimitives.h"
#include "OSWindows.h"

#ifdef PLATFORM_WINDOWS_SHELL

namespace GX_STL
{
namespace _platform_
{
	using namespace winapi;
	
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
		typedef CriticalSection		Self;


	// variables
	private:
		CRITICAL_SECTION	_crSection;
		bool				_inited;

		
	// methods
	private:
		void _Delete ()
		{
			if ( _inited )
			{
				DeleteCriticalSection( &_crSection );
				ZeroMem( &_crSection, sizeof(_crSection) );
				_inited = false;
			}
		}

		CriticalSection (const CRITICAL_SECTION &cs) : _crSection(cs), _inited(true)
		{}


	public:
		CriticalSection (): _inited(false)
		{
			ZeroMem( &_crSection, sizeof(_crSection) );

			InitializeCriticalSection( &_crSection );
			_inited = true;
		}

		~CriticalSection ()
		{
			_Delete();
		}

		bool IsValid () const
		{
			return _inited;
		}

		void Lock ()
		{
			ASSERT( IsValid() );
			EnterCriticalSection( &_crSection );
		}

		bool TryLock ()
		{
			ASSERT( IsValid() );
			return TryEnterCriticalSection( &_crSection ) != 0;
		}

		void Unlock ()
		{
			ASSERT( IsValid() );
			LeaveCriticalSection( &_crSection );
		}

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
	// Single Read, Multiple Writ
	//

	struct ReadWriteSync
	{
		friend struct ConditionVariable;

	// types
	public:
		typedef ReadWriteSync	Self;


	// variables
	private:
		SRWLOCK		_srw;
		bool		_inited;
		
	private:
		typedef VOID	(CALLBACK *PInitializeSRWLockProc_t)		 (/*out*/   PSRWLOCK SRWLock);
		typedef VOID	(CALLBACK *PAcquireSRWLockExclusiveProc_t)	 (/*inout*/ PSRWLOCK SRWLock);
		typedef BOOLEAN	(CALLBACK *PTryAcquireSRWLockExclusiveProc_t)(/*inout*/ PSRWLOCK SRWLock);
		typedef VOID	(CALLBACK *PReleaseSRWLockExclusiveProc_t)	 (/*inout*/ PSRWLOCK SRWLock);
		typedef VOID	(CALLBACK *PAcquireSRWLockSharedProc_t)		 (/*inout*/ PSRWLOCK SRWLock);
		typedef BOOLEAN	(CALLBACK *PTryAcquireSRWLockSharedProc_t)	 (/*inout*/ PSRWLOCK SRWLock);
		typedef VOID	(CALLBACK *PReleaseSRWLockSharedProc_t)		 (/*inout*/ PSRWLOCK SRWLock);
		typedef	VOID	(CALLBACK *PDeleteSRWLockProc_t)			 (/*inout*/ PSRWLOCK SRWLock);

		static PInitializeSRWLockProc_t				_InitializeSRWLock;
		static PAcquireSRWLockExclusiveProc_t		_AcquireSRWLockExclusive;
		static PTryAcquireSRWLockExclusiveProc_t	_TryAcquireSRWLockExclusive;
		static PReleaseSRWLockExclusiveProc_t		_ReleaseSRWLockExclusive;
		static PAcquireSRWLockSharedProc_t			_AcquireSRWLockShared;
		static PTryAcquireSRWLockSharedProc_t		_TryAcquireSRWLockShared;
		static PReleaseSRWLockSharedProc_t			_ReleaseSRWLockShared;
		static PDeleteSRWLockProc_t					_DeleteSRWLock;
		static bool									_s_bIsInit;
		

	// methods
	private:
		static void	_s_Init();
		
	public:
		ReadWriteSync()
		{
			if ( !_s_bIsInit )
				_s_Init();

			_InitializeSRWLock( &_srw );
		}

		~ReadWriteSync()
		{
			_DeleteSRWLock( &_srw );
		}

		void LockWrite()
		{
			_AcquireSRWLockExclusive( &_srw );
		}

		bool TryLockWrite()
		{
			return _TryAcquireSRWLockExclusive( &_srw ) != 0;
		}

		void UnlockWrite()
		{
			_ReleaseSRWLockExclusive( &_srw );
		}

		void LockRead()
		{
			_AcquireSRWLockShared( &_srw );
		}

		bool TryLockRead()
		{
			return _TryAcquireSRWLockShared( &_srw ) != 0;
		}

		void UnlockRead()
		{
			_ReleaseSRWLockShared( &_srw );
		}

		ScopeLock GetScopeWriteLock()
		{
			struct Util {
				static void Lock (void *p)		{ ((Self *)p)->LockWrite(); }
				static void Unlock (void *p)	{ ((Self *)p)->UnlockWrite(); }
			};
			return ScopeLock( this, &Util::Lock, &Util::Unlock, false );
		}

		ScopeLock GetScopeReadLock()
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
		enum {
			MAX_WAIT_TIME	= 0xFFFFFFFF
		};

		typedef ConditionVariable	Self;


	// variables
	private:
		CONDITION_VARIABLE	_cv;
		bool				_inited;
		
	private:
		typedef VOID	(CALLBACK *PInitializeConditionVariableProc_t)(OUT   PCONDITION_VARIABLE ConditionVariable);
		typedef VOID	(CALLBACK *PWakeConditionVariableProc_t)	  (INOUT PCONDITION_VARIABLE ConditionVariable);
		typedef VOID	(CALLBACK *PWakeAllConditionVariableProc_t)	  (INOUT PCONDITION_VARIABLE ConditionVariable);
		typedef BOOL	(CALLBACK *PSleepConditionVariableCSProc_t)	  (INOUT PCONDITION_VARIABLE ConditionVariable,
																	   INOUT PCRITICAL_SECTION CriticalSection,
																	         DWORD dwMilliseconds);
		typedef BOOL	(CALLBACK *PSleepConditionVariableSRWProc_t)  (INOUT PCONDITION_VARIABLE ConditionVariable,
																	   INOUT PSRWLOCK SRWLock,
																	         DWORD dwMilliseconds,
																	         ULONG Flags);
		typedef VOID	(CALLBACK *PDeleteConditionVariableProc_t)	  (INOUT PCONDITION_VARIABLE ConditionVariable);
		

		static	PInitializeConditionVariableProc_t		_InitializeConditionVariable;
		static	PWakeConditionVariableProc_t			_WakeConditionVariable;
		static	PWakeAllConditionVariableProc_t			_WakeAllConditionVariable;
		static	PSleepConditionVariableCSProc_t			_SleepConditionVariableCS;
		static	PSleepConditionVariableSRWProc_t		_SleepConditionVariableSRW;
		static	PDeleteConditionVariableProc_t			_DeleteConditionVariable;
		static	bool									_s_bIsInit;
		
		static	VOID CALLBACK DefInitializeConditionVariable (OUT   PCONDITION_VARIABLE cv);
		static	VOID CALLBACK DefWakeConditionVariable		 (INOUT PCONDITION_VARIABLE cv);
		static	VOID CALLBACK DefWakeAllConditionVariable	 (INOUT PCONDITION_VARIABLE cv);
		static	BOOL CALLBACK DefSleepConditionVariableCS	 (INOUT PCONDITION_VARIABLE cv,
															  INOUT PCRITICAL_SECTION cs,
															        DWORD dwMilliseconds);
		static	BOOL CALLBACK DefSleepConditionVariableSRW	 (INOUT PCONDITION_VARIABLE cv,
															  INOUT PSRWLOCK SRWLock,
															        DWORD dwMilliseconds,
															        ULONG Flags);
		static	VOID CALLBACK DefDeleteConditionVariable	 (INOUT PCONDITION_VARIABLE cv);


	// methods
	private:
		static void _s_Init ();

		bool _Create ()
		{
			_Destroy();
			_InitializeConditionVariable( &_cv );
			_inited = true;
			return IsValid();
		}

		void _Destroy ()
		{
			if ( IsValid() )
			{
				_inited = false;
				_DeleteConditionVariable( &_cv );
			}
		}

	public:
		ConditionVariable (): _inited(false)
		{
			if ( not _s_bIsInit )
				_s_Init();

			_Create();
		}

		~ConditionVariable ()
		{
			_Destroy();
		}

		bool IsValid () const
		{
			return _inited;
		}

		void Signal ()
		{
			ASSERT( IsValid() );
			_WakeConditionVariable( &_cv );
		}

		void Broadcast ()
		{
			ASSERT( IsValid() );
			_WakeAllConditionVariable( &_cv );
		}

		bool Wait (CriticalSection &cs, uint time = MAX_WAIT_TIME)
		{
			ASSERT( IsValid() );
			return _SleepConditionVariableCS( &_cv, &cs._crSection, time ) == TRUE;
		}
	};



	//
	// Synchronization Event
	//
	
	struct SyncEvent
	{
	// types
	public:
		typedef SyncEvent	Self;

		enum EFlags {
			MANUAL_RESET		= 0,
			AUTO_RESET			= 0x1,
			INIT_STATE_SIGNALED = 0x2,
		};

		enum {
			MAX_WAIT_TIME	= 0xFFFFFFFF
		};


	// variables
	private:
		HANDLE	_event;


	// methods
	public:
		explicit
		SyncEvent (EFlags flags = AUTO_RESET): _event(null)
		{
			_Create( flags );
		}

		~SyncEvent ()
		{
			_Delete();
		}

		bool IsValid () const
		{
			return _event != null;
		}


		bool Signal ()
		{
			return SetEvent( _event ) == TRUE;
		}

		bool Reset ()
		{
			return ResetEvent( _event ) == TRUE;
		}

		bool Pulse ()
		{
			return PulseEvent( _event ) == TRUE;
		}

		bool Wait (uint time = MAX_WAIT_TIME)
		{
			return WaitForSingleObject( _event, time ) == WAIT_OBJECT_0;
		}

		static int WaitEvents (Buffer<const Self *> events, bool waitAll, uint time = MAX_WAIT_TIME)
		{
			STATIC_ASSERT( sizeof(SyncEvent) == sizeof(HANDLE) );

			DWORD	res		= WaitForMultipleObjects( (DWORD)events.Count(), (HANDLE *) events.ptr(), waitAll, time );
			bool	success	= (res >= WAIT_OBJECT_0 and res <= WAIT_OBJECT_0 + events.Count());
			return success ? res - WAIT_OBJECT_0 : -1;
		}

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
		bool _Create (EFlags flags)
		{
			_Delete();
			_event = CreateEvent( null,
								  not EnumEq( flags, AUTO_RESET ),
								  EnumEq( flags, INIT_STATE_SIGNALED ),
								  (const char *)null );
			return IsValid();
		}

		bool _Delete()
		{
			bool	ret = true;
			if ( IsValid() ) {
				ret = CloseHandle( _event ) == TRUE;
				_event = null;
			}
			return ret;
		}
	};



	//
	// Semaphore
	//

	struct Semaphore
	{
	// types
	public:
		typedef Semaphore	Self;
		
		enum : uint {
			MAX_WAIT_TIME	= 0xFFFFFFFF
		};


	// variables
	private:
		HANDLE	_sem;


	// methods
	private:
		bool _Create (uint initialValue)
		{
			_sem = CreateSemaphore( null, initialValue, MaxValue<int>(), (const char *)null );
			return IsValid();
		}

		void _Destroy ()
		{
			if ( _sem != null )
			{
				CloseHandle( _sem );
				_sem = null;
			}
		}

	public:
		explicit
		Semaphore (uint initialValue) : _sem(null)
		{
			_Create( initialValue );
		}

		~Semaphore ()
		{
			_Destroy();
		}

		bool IsValid () const
		{
			return _sem != null;
		}

		void Lock ()
		{
			uint result = WaitForSingleObject( _sem, MAX_WAIT_TIME );
			ASSERT( result == winapi::WAIT_OBJECT_0 );
		}

		bool TryLock ()
		{
			uint result = WaitForSingleObject( _sem, 1 );
			return result == winapi::WAIT_OBJECT_0;
		}

		void Unlock ()
		{
			bool	res = ReleaseSemaphore( _sem, 1, null ) != FALSE;
			ASSERT( res );
		}

		uint GetValue ()
		{
			TODO( "GetValue" );
			return 0;
		}
		
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


}	// _platform_
}	// GX_STL

#endif	// PLATFORM_WINDOWS_SHELL