// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

namespace GX_STL
{
namespace _platform_
{
	
	//
	// Read Write Sync
	//
	
	struct ReadWriteSyncEmulation
	{
	// types
	public:
		typedef ReadWriteSyncEmulation	Self;


	// variables
	private:
		CriticalSection		_sCSWrite,
							_sCSRead;
		SyncEvent			_sELocked;
		int					_iCounter;

		static const uint	_s_uTimeOut = 10;


	// methods
	public:
		ReadWriteSyncEmulation ():
			_iCounter(0), _sELocked( SyncEvent::INIT_STATE_SIGNALED )
		{}

		~ReadWriteSyncEmulation ()
		{}

		bool IsValid () const
		{
			return true;
		}
		
		void LockWrite ()
		{
			_sELocked.Wait();
			_sCSWrite.Lock();
		}

		bool TryLockWrite ()
		{
			return ( _sELocked.Wait(_s_uTimeOut) and _sCSWrite.TryLock() );
		}

		void UnlockWrite ()
		{
			_sCSWrite.Unlock();
		}

		void LockRead ()
		{
			_sCSWrite.Lock();
			_sCSRead.Lock();

			if ( ++_iCounter == 1 )
				_sELocked.Reset();

			_sCSWrite.Unlock();
			_sCSRead.Unlock();
		}

		bool TryLockRead ()
		{
			if ( not _sCSWrite.TryLock() )
				return false;
			else
				if ( not _sCSRead.TryLock() ) {
					_sCSWrite.Unlock();
					return false;
				}

			if ( ++_iCounter == 1 )
				_sELocked.Reset();

			_sCSWrite.Unlock();
			_sCSRead.Unlock();
			return true;
		}

		void UnlockRead ()
		{
			_sCSRead.Lock();

			if ( --_iCounter == 0 )
				_sELocked.Signal();

			_sCSRead.Unlock();
		}

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


}	// _platform_
}	// GX_STL
