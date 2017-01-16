// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

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
		ParallelThreadPtr	thread = ParallelThread::New( SubSystems() );
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

		if ( thread.IsNull() )
		{
			WARNING( "current thread not found" );

			thread = MainThread::New( SubSystems() );
			AddThread( thread );
		}
		return VirtualThread::New( thread );
	}
	
/*
=================================================
	AddThread
=================================================
*/
	void ThreadManager::AddThread (const IParallelThreadPtr &thread)
	{
		CHECK_ERR( thread.IsNotNull(), void() );
			
		if ( thread->IsVirtualThread() )
			return AddThread( thread->GetTragetThread() );

		SCOPELOCK( _lockThreads );
		_threads.Add( thread->GetThreadId(), thread );
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

		if ( _threads.Find( CurrentThread::GetCurrentThreadId(), iter ) )
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
		CHECK( current.IsNotNull() and other.IsNotNull() );

		SyncEvent	ev( SyncEvent::AUTO_RESET );

		other->EnqueueFlushCommand( current );

		ParallelOp	op;
		FunctionBuilder::Create( op.func, &_Signal, &ev );
			
		other->Push( RVREF( op ) );
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