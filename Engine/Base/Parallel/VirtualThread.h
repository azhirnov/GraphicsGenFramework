// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.
/*
	VirtualThread is wrapper for parallel thread
*/

#pragma once

#include "IParallelThread.h"

namespace Engine
{
namespace Base
{

	class VirtualThread;
	SHARED_POINTER( VirtualThread );


	
	//
	// Virtual Thread
	//

	class VirtualThread : public IParallelThread
	{
	// variables
	public:
		IParallelThreadPtr	_targetThread;


	// methods
	public:
		explicit
		VirtualThread (const IParallelThreadPtr &target) :
			IParallelThread( target->SubSystems() )
		{
			_SetTargetThread( target );
		}
		

		// IParallelThread //
		IParallelThreadPtr GetTragetThread () override
		{
			return _targetThread;
		}


		bool IsCurrentThread () const override
		{
			return _targetThread->IsCurrentThread();
		}
		

		bool IsVirtualThread () const override
		{
			return true;
		}


		void FlushMessages () override
		{
			return _targetThread->FlushMessages();
		}


		void Push (const ParallelOp &op) override
		{
			return _targetThread->Push( op );
		}


		void Push (ParallelOp &&op) override
		{
			return _targetThread->Push( RVREF( op ) );
		}
			

		usize GetThreadId () override
		{
			return _targetThread->GetThreadId();
		}


		usize ProcessMessages () override
		{
			//RETURN_ERR( "can't process messages in wrapper thread" );
			return _targetThread->ProcessMessages();
		}


		void Quit () override
		{
			_targetThread = null;
		}

		static VirtualThreadPtr New (const IParallelThreadPtr &target)
		{
			CHECK_ERR( target );
			return BaseObject::_New( new VirtualThread( target ) );
		}


	private:
		void _SetTargetThread (const IParallelThreadPtr &target)
		{
			CHECK( not _targetThread );
			CHECK( target != this );
			
			_targetThread = target;

			for (uint i = 0; i < 100 and _targetThread->IsVirtualThread(); ++i)
			{
				_targetThread = _targetThread->GetTragetThread();
			}
			ASSERT( not _targetThread->IsVirtualThread() );
		}
	};
	

}	// Base
}	// Engine
