// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

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
		typedef Time<double>			Time_t;

	private:
		struct Timer
		{
		// variables
			OnElapsed_t		func;
			Time_t			endTime;
			
		// methods
			Timer (GX_DEFCTOR) : endTime(0.0)
			{}

			Timer (const OnElapsed_t &func, Time_t endTime) :
				func(func), endTime(endTime)
			{}
		};

		typedef Array< Timer >	timer_array_t;


	// variables
	private:
		Mutex					_lock;
		timer_array_t			_timers;
		TimeProfiler<double>	_timer;


	// methods
	public:
		explicit
		TimerManager (const SubSystemsRef ss) :
			VirtualThread( ss->Get< ThreadManager >()->GetCurrent() )
		{}


		~TimerManager ()
		{}


		bool CreateTimer (Time_t interval, const OnElapsed_t &func)
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
			ParallelOp	op;
			FunctionBuilder::Create( op.func, TimerManagerPtr( this ), &TimerManager::_Update );

			Push( RVREF( op ) );
		}


		void _Update ()
		{
			SCOPELOCK( _lock );

			const Time_t	st = _timer.GetStartTime();		_timer.Start();
			const Time_t	ct = _timer.GetStartTime();
			const Time_t	dt = ct - st;

			FOR( i, _timers )
			{
				if ( _timers[i].endTime >= ct )
				{
					_timers[i].func.SafeCall();
					_timers.Erase( i );
					--i;
				}
			}

			if ( dt < Time_t().FromSeconds( 1.0e-6 ) )
			{
				TODO( "check" );
				Thread::Yield();
			}

			if ( not _timers.Empty() )
				_AddToQueue();
		}
	};


}	// Base
}	// Engine