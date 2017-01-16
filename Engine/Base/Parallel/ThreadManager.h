// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "ParallelThread.h"
#include "VirtualThread.h"
#include "MainThread.h"
#include "Runnable.h"
#include "AsyncTask.h"

namespace Engine
{
namespace Base
{

	//
	// Parallel Thread Manager
	//

	class ThreadManager : public BaseObject
	{
	// types
	private:
		typedef SyncEvent *							sync_event_ptr_t;

		typedef Map< usize, IParallelThreadPtr >	threads_t;


	// variables
	private:
		threads_t		_threads;
		Mutex			_lockThreads;


	// methods
	public:
		explicit
		ThreadManager (const SubSystemsRef ss);
		~ThreadManager ();

		IParallelThreadPtr GetCurrent ();
		IParallelThreadPtr CreateThread ();
		IParallelThreadPtr CreateVirtualThread ();


		void AddThread (const IParallelThreadPtr &thread);
		void RemoveThread (const IParallelThreadPtr &thread);

		void FlushAll ();
		void ReleaseAll ();


		// Utils //
		static void Iterate (const IParallelThreadPtr &current, const IParallelThreadPtr &other);


	private:
		static void _Signal (const sync_event_ptr_t &ev);
	};




}	// Base
}	// Engine