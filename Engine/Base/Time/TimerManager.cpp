// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "TimerManager.h"

namespace Engine
{
namespace Base
{
	
/*
=================================================
	constructor
=================================================
*/
	TimerManager::TimerManager (const SubSystemsRef ss) :
		VirtualThread( ss->Get< ThreadManager >()->GetCurrent() )
	{}
	
/*
=================================================
	destructor
=================================================
*/
	TimerManager::~TimerManager ()
	{}
	
/*
=================================================
	CreateTimer
=================================================
*/
	bool TimerManager::CreateTimer (TimeD interval, const OnElapsed_t &cb)
	{
		SCOPELOCK( _lock );

		if ( _timers.Empty() )
		{
			_AddToQueue();
		}

		_timers.PushBack( Timer( cb, _timer.GetCurrentTime() + interval ) );

		return true;
	}
	
/*
=================================================
	_AddToQueue
=================================================
*/
	void TimerManager::_AddToQueue ()
	{
		Push( ParallelOp( FunctionBuilder( TimerManagerPtr( this ), &TimerManager::_Update ) ) );
	}

/*
=================================================
	_Update
=================================================
*/
	void TimerManager::_Update ()
	{
		SCOPELOCK( _lock );

		const TimeD		st = _timer.GetStartTime();		_timer.Start();
		const TimeD		ct = _timer.GetStartTime();
		const TimeD		dt = ct - st;

		FOR( i, _timers )
		{
			if ( _timers[i].endTime >= ct )
			{
				_timers[i].func.SafeCall();
				_timers.Erase( i );			// TODO: FastErase ?
				--i;
			}
		}

		if ( dt < TimeD::FromSeconds( 1.0e-6 ) )
		{
			TODO( "check" );
			OS::Thread::Yield();
		}

		if ( not _timers.Empty() )
			_AddToQueue();
	}


}	// Base
}	// Engine
