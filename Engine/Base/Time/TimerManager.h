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
		TimerManager (const SubSystemsRef ss);
		~TimerManager ();

		bool CreateTimer (TimeD interval, const OnElapsed_t &cb);


	private:
		void _AddToQueue ();
		void _Update ();
	};


}	// Base
}	// Engine