// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "ParallelOp.h"

namespace Engine
{
namespace Base
{
	
	class IParallelThread;
	SHARED_POINTER( IParallelThread );


	//
	// Parallel Thread interface
	//

	class IParallelThread : public BaseObject
	{
	// methods
	public:
		explicit
		IParallelThread (const SubSystemsRef ss) :
			BaseObject(ss)
		{
		}

		void EnqueueFlushCommand ()
		{
			return EnqueueFlushCommand( this );
		}

		void EnqueueFlushCommand (const IParallelThreadPtr &forThread)
		{
			ParallelOp	op;
			FunctionBuilder::Create( op.func, forThread, &IParallelThread::FlushMessages );
			Push( RVREF( op ) );
		}


	private:
		IParallelThread (const IParallelThread &other) : BaseObject(other) {}
		
		IParallelThread (IParallelThread &&other) : BaseObject(other) {}
		
		void operator = (const IParallelThread &) {}


	// interface
	public:
		virtual IParallelThreadPtr GetTragetThread ()	{ return null; }
		
		virtual usize GetThreadId () = 0;

		virtual bool IsCurrentThread () const = 0;

		virtual bool IsVirtualThread () const = 0;

		virtual void FlushMessages () = 0;

		virtual void Push (const ParallelOp &) = 0;

		virtual void Push (ParallelOp &&) = 0;

		virtual usize ProcessMessages () = 0;

		virtual void Quit () = 0;
	};

	
}	// Base
}	// Engine
