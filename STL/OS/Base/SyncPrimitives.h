// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Common.h"

namespace GX_STL
{
namespace _platform_
{

	//
	// Scope Lock
	//

	struct ScopeLock
	{
	// types
	public:
		typedef void (*_PProc_t)(void *);
		typedef _PProc_t	PLockProc_t;
		typedef _PProc_t	PUnlockProc_t;
		typedef ScopeLock	Self;


	// variables
	private:
		PLockProc_t		_lock;
		PUnlockProc_t	_unlock;
		void	*		_param;
		bool			_locked;


	// methods
	public:
		ScopeLock (void *ptr, PLockProc_t lockProc, PUnlockProc_t unlockProc, bool lock) :
			_lock(lockProc), _unlock(unlockProc), _param(ptr), _locked(false)
		{
			ASSUME( _lock != null and _unlock != null );

			if ( lock )
			{
				_lock( _param );
				_locked = true;
			}
		}

		ScopeLock (const Self &other, bool lock) :
			_lock(null), _unlock(null), _param(null), _locked(false)
		{
			_Create( other, lock );
		}

		template <typename T>
		ScopeLock (T &sync, bool lock) :
			_lock(null), _unlock(null), _param(null), _locked(false)
		{
			_Create( sync.GetScopeLock(), lock );
		}

		~ScopeLock ()
		{
			if ( _locked )
			{
				_locked = false;
				_unlock( _param );
			}
		}

	private:
		void _Create (const Self &other, bool lock)
		{
			ASSERT( not lock or ( lock and not other._locked ) );

			_lock	= other._lock;
			_unlock	= other._unlock;
			_param	= other._param;

			if ( lock )
			{
				_lock( _param );
				_locked = true;
			}
		}
	};


#	define SCOPELOCK( _syncGetScopeLock_ ) \
		GX_STL::_platform_::ScopeLock	__temp_scope_lock( _syncGetScopeLock_, true )

#	define SCOPELOCK_UNIQUE( _syncGetScopeLock_ ) \
		GX_STL::_platform_::ScopeLock	__scope_lock_##_syncGetScopeLock_( _syncGetScopeLock_, true )

#	define SCOPELOCK_NAMED( _name_, _syncGetScopeLock_ ) \
		GX_STL::_platform_::ScopeLock	__scope_lock_##_name_( _syncGetScopeLock_, true )


}	// _platform_
}	// GX_STL
