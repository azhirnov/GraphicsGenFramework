// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "ParallelOp.h"

namespace Engine
{
namespace Base
{


	//
	// Runnable (Java-style)
	//

	class Runnable : public Referenced
	{
	// types
	public:
		SHARED_POINTER( Runnable );


	// methods
	public:
		Runnable ()
		{}

		virtual void Run () = 0;

		operator RValueRef< ParallelOp >::type ()
		{
			ParallelOp	op;
			FunctionBuilder::Create( op.func, RunnablePtr(this), &Runnable::Run );
			return RVREF( op );
		}
	};


}	// Base
}	// Engine