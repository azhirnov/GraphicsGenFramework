// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "ParallelOp.h"

namespace Engine
{
namespace Base
{

	//
	// Async Task (Java-style)
	//

	template <typename Param, typename Progress, typename Result>
	class AsyncTask : public Referenced
	{
	// types
	public:
		SHARED_POINTER( AsyncTask );


	// variables
	private:
		IParallelThreadPtr	_currentThread;
		IParallelThreadPtr	_targetThread;
		SyncEvent			_event;
		//atomic_bool			_canceled;


	// methods
	public:
		AsyncTask (const IParallelThreadPtr &currentThread, const IParallelThreadPtr &targetThread) :
			_currentThread(currentThread), _targetThread(targetThread) _event( SyncEvent::MANUAL_RESET )
		{
			ASSERT( _currentThread.IsNotNull() );
			ASSERT( _targetThread.IsNotNull() );
		}


		~AsyncTask ()
		{
			Wait();
		}


		void Execute ()
		{
			ASSERT( _currentThread->IsCurrent() );

			PreExecute();
			
			ParallelOp	op;
			FunctionBuilder::Create( op.func, AsyncTaskPtr(this), &AsyncTask::_Process );

			_targetThread->Push( RVREF( op ) );
		}


		void Wait (uint timeMillis = SyncEvent::MAX_WAIT_TIME)
		{
			_event.Wait( timeMillis );
		}


		/*void Cancel ()
		{
			_canceled = true;
		}


		bool IsCanceled () const
		{
			return _canceled;
		}*/


	// interface
	public:
		// in current thread //
		virtual void PreExecute ()		{}
		virtual void UpdateProgress ()	{}
		virtual void PostExecute ()		{}
		//virtual void OnCanceled ()		{}
		
		// in target thread //
		virtual void DoInBackground ()	= 0;


	// hidden
	protected:
		void PublishProgress ()
		{
			ASSERT( _targetThread->IsCurrent() );
			
			ParallelOp	op;
			FunctionBuilder::Create( op.func, AsyncTaskPtr(this), &AsyncTask::_Process );

			_currentThread->Push( RVREF( op ) );
		}


	private:
		/*void _Cancel ()
		{
			_event.Signal();

			if ( not _currentThread->IsCurrent() )
			{
				ParallelOp	op;
				FunctionBuilder::Create( op.func, AsyncTaskPtr(this), &AsyncTask::OnCanceled );
				_currentThread->Push( RVREF( op ) );
			}
			else
			{
				OnCanceled();
			}
		}*/


		void _Process ()
		{
			ASSERT( _targetThread->IsCurrent() );

			/*if ( IsCanceled() )
			{
				_Cancel();
				return;
			}*/

			DoInBackground();

			_event.Signal();
			
			/*if ( IsCanceled() )
			{
				_Cancel();
				return;
			}*/
			
			ParallelOp	op;
			FunctionBuilder::Create( op.func, AsyncTaskPtr(this), &AsyncTask::PostExecute );

			_currentThread->Push( RVREF( op ) );
		}
	};


}	// Base
}	// Engine