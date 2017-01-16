// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "../../Math/Mathematics.h"
#include "SyncPrimitives.h"

#ifdef PLATFORM_WINDOWS_SHELL

#include "Library.h"
#include "../Base/ReadWriteSyncEmulation.h"
#include "../Base/ConditionVariableEmulation.h"

namespace GX_STL
{
namespace _platform_
{

	template <typename T, typename B>
	inline T *& HandleToPointerRef (B &h)
	{
		return reinterpret_cast< T *&>( h->Ptr );
	}


	template <typename T, typename B>
	inline T * HandleToPointer (const B& h)
	{
		return PointerCast< T >( h->Ptr );
	}


	template <typename T>
	inline bool SetPointer (T *&to, void *from)
	{
		if ( from != null ) {
			to = PointerCast< T >( from );
			return true;
		}
		return false;
	}

	typedef ReadWriteSyncEmulation		TDefReadWriteSync;
	typedef ConditionVariableEmulation	TDefConditionVariable;




	VOID CALLBACK DefInitializeSRWLock (/*out*/ PSRWLOCK SRWLock)
	{
		HandleToPointerRef< TDefReadWriteSync >( SRWLock ) = new TDefReadWriteSync;
	}


	VOID CALLBACK DefAcquireSRWLockExclusive (/*inout*/ PSRWLOCK SRWLock)
	{
		HandleToPointer< TDefReadWriteSync >( SRWLock )->LockWrite();
	}


	BOOLEAN CALLBACK DefTryAcquireSRWLockExclusive (/*inout*/ PSRWLOCK SRWLock)
	{
		return HandleToPointer< TDefReadWriteSync >( SRWLock )->TryLockWrite();
	}


	VOID CALLBACK DefReleaseSRWLockExclusive (/*inout*/ PSRWLOCK SRWLock)
	{
		HandleToPointer< TDefReadWriteSync >( SRWLock )->UnlockWrite();
	}


	VOID CALLBACK DefAcquireSRWLockShared (/*inout*/ PSRWLOCK SRWLock)
	{
		HandleToPointer< TDefReadWriteSync >( SRWLock )->LockRead();
	}


	BOOLEAN CALLBACK DefTryAcquireSRWLockShared (/*inout*/ PSRWLOCK SRWLock)
	{
		return HandleToPointer< TDefReadWriteSync >( SRWLock )->TryLockRead();
	}


	VOID CALLBACK DefReleaseSRWLockShared (/*inout*/ PSRWLOCK SRWLock)
	{
		HandleToPointer< TDefReadWriteSync >( SRWLock )->UnlockRead();
	}
	

	VOID CALLBACK DefDeleteSRWLock (/*inout*/ PSRWLOCK SRWLock)
	{
		delete HandleToPointer< TDefReadWriteSync >( SRWLock );
		HandleToPointerRef< TDefReadWriteSync >( SRWLock ) = null;
	}
	

	VOID CALLBACK WinDeleteSRWLock (/*inout*/ PSRWLOCK SRWLock)
	{
		GX_UNUSED( SRWLock );
		return;
	}
	
	

	ReadWriteSync::PInitializeSRWLockProc_t				ReadWriteSync::_InitializeSRWLock			= & DefInitializeSRWLock;
	ReadWriteSync::PAcquireSRWLockExclusiveProc_t		ReadWriteSync::_AcquireSRWLockExclusive		= & DefAcquireSRWLockExclusive;
	ReadWriteSync::PTryAcquireSRWLockExclusiveProc_t	ReadWriteSync::_TryAcquireSRWLockExclusive	= & DefTryAcquireSRWLockExclusive;
	ReadWriteSync::PReleaseSRWLockExclusiveProc_t		ReadWriteSync::_ReleaseSRWLockExclusive		= & DefReleaseSRWLockExclusive;
	ReadWriteSync::PAcquireSRWLockSharedProc_t			ReadWriteSync::_AcquireSRWLockShared		= & DefAcquireSRWLockShared;
	ReadWriteSync::PTryAcquireSRWLockSharedProc_t		ReadWriteSync::_TryAcquireSRWLockShared		= & DefTryAcquireSRWLockShared;
	ReadWriteSync::PReleaseSRWLockSharedProc_t			ReadWriteSync::_ReleaseSRWLockShared		= & DefReleaseSRWLockShared;
	ReadWriteSync::PDeleteSRWLockProc_t					ReadWriteSync::_DeleteSRWLock				= & DefDeleteSRWLock;
	bool												ReadWriteSync::_s_bIsInit					= false;
		


	void ReadWriteSync::_s_Init()
	{
		Library		s_lib("kernel32.dll");
		bool		res = true;

		if ( s_lib.IsValid() )
		{
			res &= SetPointer( _InitializeSRWLock,			s_lib.GetProc("InitializeSRWLock") );
			res &= SetPointer( _AcquireSRWLockExclusive,	s_lib.GetProc("AcquireSRWLockExclusive") );
			res &= SetPointer( _TryAcquireSRWLockExclusive,	s_lib.GetProc("TryAcquireSRWLockExclusive") );
			res &= SetPointer( _ReleaseSRWLockExclusive,	s_lib.GetProc("ReleaseSRWLockExclusive") );
			res &= SetPointer( _AcquireSRWLockShared,		s_lib.GetProc("AcquireSRWLockShared") );
			res &= SetPointer( _TryAcquireSRWLockShared,	s_lib.GetProc("TryAcquireSRWLockShared") );
			res &= SetPointer( _ReleaseSRWLockShared,		s_lib.GetProc("ReleaseSRWLockShared") );
			_DeleteSRWLock = & WinDeleteSRWLock;
		}

		if ( !res )
		{
			_InitializeSRWLock			= & DefInitializeSRWLock;
			_AcquireSRWLockExclusive	= & DefAcquireSRWLockExclusive;
			_TryAcquireSRWLockExclusive	= & DefTryAcquireSRWLockExclusive;
			_ReleaseSRWLockExclusive	= & DefReleaseSRWLockExclusive;
			_AcquireSRWLockShared		= & DefAcquireSRWLockShared;
			_TryAcquireSRWLockShared	= & DefTryAcquireSRWLockShared;
			_ReleaseSRWLockShared		= & DefReleaseSRWLockShared;
			_DeleteSRWLock				= & DefDeleteSRWLock;
		}

		_s_bIsInit = true;
	}


		

	VOID CALLBACK ConditionVariable::DefInitializeConditionVariable (OUT PCONDITION_VARIABLE cv)
	{
		HandleToPointerRef< TDefConditionVariable >( cv ) = new TDefConditionVariable;
	}


	VOID CALLBACK ConditionVariable::DefWakeConditionVariable (INOUT PCONDITION_VARIABLE cv)
	{
		HandleToPointer< TDefConditionVariable >( cv )->Signal();
	}


	VOID CALLBACK ConditionVariable::DefWakeAllConditionVariable (INOUT PCONDITION_VARIABLE cv)
	{
		HandleToPointer< TDefConditionVariable >( cv )->Broadcast();
	}


	BOOL CALLBACK ConditionVariable::DefSleepConditionVariableCS (INOUT PCONDITION_VARIABLE cv,
									INOUT PCRITICAL_SECTION cs, DWORD dwMilliseconds)
	{
		CriticalSection	tmp( *cs );
		BOOL	res = HandleToPointer< TDefConditionVariable >( cv )->Wait( tmp, dwMilliseconds );
		tmp._inited = false;
		return res;
	}


	BOOL CALLBACK ConditionVariable::DefSleepConditionVariableSRW (INOUT PCONDITION_VARIABLE cv,
									INOUT PSRWLOCK SRWLock, DWORD dwMilliseconds, ULONG Flags)
	{
		//return HandleToPointer< TDefConditionVariable >( cv )->Sleep( SRWLock, dwMilliseconds, Flags );
		GX_UNUSED( cv, SRWLock, dwMilliseconds, Flags );
		TODO("");
		return false;
	}
	

	VOID CALLBACK ConditionVariable::DefDeleteConditionVariable (INOUT PCONDITION_VARIABLE cv)
	{
		delete HandleToPointer< TDefConditionVariable >( cv );
		HandleToPointerRef< TDefConditionVariable >( cv ) = null;
	}
	

	VOID CALLBACK WinDeleteConditionVariable(/*inout*/ PCONDITION_VARIABLE cv)
	{
		GX_UNUSED( cv );
	}


	ConditionVariable::PInitializeConditionVariableProc_t	ConditionVariable::_InitializeConditionVariable	= & DefInitializeConditionVariable;
	ConditionVariable::PWakeConditionVariableProc_t			ConditionVariable::_WakeConditionVariable		= & DefWakeConditionVariable;
	ConditionVariable::PWakeAllConditionVariableProc_t		ConditionVariable::_WakeAllConditionVariable	= & DefWakeAllConditionVariable;
	ConditionVariable::PSleepConditionVariableCSProc_t		ConditionVariable::_SleepConditionVariableCS	= & DefSleepConditionVariableCS;
	ConditionVariable::PSleepConditionVariableSRWProc_t		ConditionVariable::_SleepConditionVariableSRW	= & DefSleepConditionVariableSRW;
	ConditionVariable::PDeleteConditionVariableProc_t		ConditionVariable::_DeleteConditionVariable		= & DefDeleteConditionVariable;
	bool													ConditionVariable::_s_bIsInit					= false;
		

	void ConditionVariable::_s_Init()
	{
		Library		s_lib("kernel32.dll");
		bool		res = true;

		if ( s_lib.IsValid() )
		{
			res &= SetPointer( _InitializeConditionVariable,s_lib.GetProc("InitializeConditionVariable") );
			res &= SetPointer( _WakeConditionVariable,		s_lib.GetProc("WakeConditionVariable") );
			res &= SetPointer( _WakeAllConditionVariable,	s_lib.GetProc("WakeAllConditionVariable") );
			res &= SetPointer( _SleepConditionVariableCS,	s_lib.GetProc("SleepConditionVariableCS") );
			res &= SetPointer( _SleepConditionVariableSRW,	s_lib.GetProc("SleepConditionVariableSRW") );
			_DeleteConditionVariable = & WinDeleteConditionVariable;
		}

		if ( !res )
		{
			_InitializeConditionVariable	= & DefInitializeConditionVariable;
			_WakeConditionVariable			= & DefWakeConditionVariable;
			_WakeAllConditionVariable		= & DefWakeAllConditionVariable;
			_SleepConditionVariableCS		= & DefSleepConditionVariableCS;
			_SleepConditionVariableSRW		= & DefSleepConditionVariableSRW;
			_DeleteConditionVariable		= & DefDeleteConditionVariable;
		}

		_s_bIsInit = true;
	}

//-------------------------------------------------------------------

}	// _platform_
}	// GX_STL

#endif	// PLATFORM_WINDOWS_SHELL