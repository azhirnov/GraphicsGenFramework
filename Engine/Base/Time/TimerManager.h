// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Parallel/ThreadManager.h"

namespace Engine
{
namespace Base
{

	class TimerManager;
	SHARED_POINTER( TimerManager );


	//
	// Timer Manager
	//

	class TimerManager : public VirtualThread
	{
	// types
	public:
		typedef Function< void >		OnElapsed_t;

	private:
		struct Timer
		{
		// variables
			OnElapsed_t		func;
			TimeD			endTime;
			
		// methods
			Timer (GX_DEFCTOR) : endTime(0.0)
			{}

			Timer (const OnElapsed_t &func, TimeD endTime) :
				func(func), endTime(endTime)
			{}
		};

		typedef Array< Timer >	timer_array_t;


	// variables
	private:
		OS::Mutex		_lock;
		timer_array_t	_timers;
		TimeProfilerD	_timer;


	// methods
	public:
		explicit
		TimerManager (const SubSystemsRef ss) :
			VirtualThread( ss->Get< ThreadManager >()->GetCurrent() )
		{}


		~TimerManager ()
		{}


		bool CreateTimer (TimeD interval, const OnElapsed_t &func)
		{
			SCOPELOCK( _lock );

			if ( _timers.Empty() )
			{
				_AddToQueue();
			}

			_timers.PushBack( Timer( func, _timer.GetCurrentTime() + interval ) );

			return true;
		}


	private:
		void _AddToQueue ()
		{
			Push( ParallelOp( FunctionBuilder( TimerManagerPtr( this ), &TimerManager::_Update ) ) );
		}


		void _Update ()
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
					_timers.Erase( i );
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
	};


}	// Base
}	// Engine