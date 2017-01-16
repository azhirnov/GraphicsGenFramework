// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "IParallelThread.h"
#include "ThreadMessageQueue.h"

namespace Engine
{
namespace Base
{

	class MainThread;
	SHARED_POINTER( MainThread );


	//
	// Main Thread
	//

	class MainThread : public IParallelThread
	{
	// variables
	private:
		CurrentThread		_thread;
		ThreadMessageQueue	_messageQueue;


	// methods
	public:
		explicit
		MainThread (const SubSystemsRef ss) :
			IParallelThread( ss )
		{}
		

		// IParallelThread //
		bool IsCurrentThread () const override
		{
			return _thread.IsCurrent();
		}
		
		bool IsVirtualThread () const override
		{
			return false;
		}

		void FlushMessages () override
		{
			_messageQueue.Flush();
		}
		
		
		void Push (const ParallelOp &op) override
		{
			_messageQueue.Push( op );
		}


		void Push (ParallelOp &&op) override
		{
			_messageQueue.Push( RVREF( op ) );
		}
			

		usize GetThreadId () override
		{
			return _thread.Id();
		}


		usize ProcessMessages () override
		{
			ASSERT( IsCurrentThread() );
			return _messageQueue.ProcessAll();
		}


		void Quit () override
		{
		}


		static MainThreadPtr  New (const SubSystemsRef ss)
		{
			return BaseObject::_New( new MainThread( ss ) );
		}
	};


}	// Base
}	// Engine