// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

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

		operator ParallelOp ()
		{
			return ParallelOp( FunctionBuilder( RunnablePtr(this), &Runnable::Run ) );
		}
	};


}	// Base
}	// Engine