// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "ThreadManager.h"

namespace Engine
{
namespace Base
{
	
/*
=================================================
	constructor
=================================================
*/
	ThreadManager::ThreadManager (const SubSystemsRef ss) : BaseObject( ss )
	{
		SubSystems()->GetSetter< ThreadManager >().Set( this );
	}

/*
=================================================
	destructor
=================================================
*/
	ThreadManager::~ThreadManager ()
	{
		ReleaseAll();
			
		SubSystems()->GetSetter< ThreadManager >().Set( null );
	}
	
/*
=================================================
	CreateThread
=================================================
*/
	IParallelThreadPtr ThreadManager::CreateThread ()
	{
		ParallelThreadPtr	thread = New<ParallelThread>( SubSystems() );
		AddThread( thread );

		thread->Start();
		return thread;
	}
	
/*
=================================================
	CreateVirtualThread
=================================================
*/
	IParallelThreadPtr ThreadManager::CreateVirtualThread ()
	{
		IParallelThreadPtr	thread = GetCurrent();

		if ( not thread )
		{
			WARNING( "current thread not found" );

			thread = New<MainThread>( SubSystems() );
			AddThread( thread );
		}
		return New<VirtualThread>( thread );
	}
	
/*
=================================================
	AddThread
=================================================
*/
	bool ThreadManager::AddThread (const IParallelThreadPtr &thread)
	{
		CHECK_ERR( thread );
			
		if ( thread->IsVirtualThread() )
			return AddThread( thread->GetTragetThread() );

		SCOPELOCK( _lockThreads );
		_threads.Add( thread->GetThreadId(), thread );

		return true;
	}
	
/*
=================================================
	RemoveThread
=================================================
*/
	void ThreadManager::RemoveThread (const IParallelThreadPtr &thread)
	{
		SCOPELOCK( _lockThreads );
		_threads.Erase( thread->GetThreadId() );
	}
	
/*
=================================================
	FlushAll
=================================================
*/
	void ThreadManager::FlushAll ()
	{
		SCOPELOCK( _lockThreads );

		FOR( i, _threads )
		{
			_threads[i].second->FlushMessages();
		}
	}
	
/*
=================================================
	ReleaseAll
=================================================
*/
	void ThreadManager::ReleaseAll ()
	{
		SCOPELOCK( _lockThreads );

		FOR( i, _threads )
		{
			_threads[i].second->Quit();
		}
	}
	
/*
=================================================
	GetCurrent
=================================================
*/
	IParallelThreadPtr ThreadManager::GetCurrent ()
	{
		SCOPELOCK( _lockThreads );

		threads_t::const_iterator	iter;

		if ( _threads.Find( OS::CurrentThread::GetCurrentThreadId(), iter ) )
			return iter->second;

		return null;
	}
	
/*
=================================================
	Iterate
=================================================
*/
	void ThreadManager::Iterate (const IParallelThreadPtr &current, const IParallelThreadPtr &other)
	{
		CHECK( current and other );

		OS::SyncEvent	ev( OS::SyncEvent::AUTO_RESET );

		other->EnqueueFlushCommand( current );
		other->Push( ParallelOp( FunctionBuilder( &_Signal, &ev ) ) );
		other->FlushMessages();
			
		current->ProcessMessages();
			
		ev.Wait();
	}
	
/*
=================================================
	_Signal
=================================================
*/
	void ThreadManager::_Signal (const sync_event_ptr_t &ev)
	{
		ev->Signal();
	}


}	// Base
}	// Engine