// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "IParallelThread.h"
#include "ThreadMessageQueue.h"

namespace Engine
{
namespace Base
{

	class ParallelThread;
	SHARED_POINTER( ParallelThread );


	//
	// Parallel Thread
	//

	class ParallelThread : public IParallelThread
	{
	// variables
	private:
		Thread				_thread;
		ThreadMessageQueue	_messageQueue;
		bool				_quit;


	// methods
	protected:
		static void _ThreadProc (void *parameter)
		{
			ParallelThread *	pthis = (ParallelThread *)parameter;

			pthis->_quit = false;

			while ( not pthis->_quit )
			{
				if ( not pthis->_messageQueue.Process() )
				{
					Thread::Yield();
				}
			}
		}


		void _QuitMessage ()
		{
			_quit = true;
		}


	public:
		explicit
		ParallelThread (const SubSystemsRef ss) :
			IParallelThread( ss ),
			_quit(false)
		{}


		~ParallelThread ()
		{
			_thread.Wait();
		}


		bool Start ()
		{
			return _thread.Create( &ParallelThread::_ThreadProc, this );
		}
		

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
			ParallelOp	op;
			FunctionBuilder::Create( op.func, ParallelThreadPtr(this), &ParallelThread::_QuitMessage );
			Push( RVREF( op ) );

			FlushMessages();

			_thread.Wait();
		}


		static ParallelThreadPtr New (const SubSystemsRef ss)
		{
			return BaseObject::_New( new ParallelThread( ss ) );
		}
	};


}	// Base
}	// Engine