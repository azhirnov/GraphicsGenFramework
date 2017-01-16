// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/BaseObject.h"

namespace Engine
{
namespace Base
{
	
	//
	// Parallel Operation interface
	//
	
	template <usize BufSize = GlobalConst::STL_FunctionInterfaceSize>
	struct TParallelOp
	{
	// types
	public:
		typedef TParallelOp< BufSize >		Self;
		typedef Function< void, BufSize >	func_t;


	// variables
	public:
		func_t		func;


	// methods
	public:
		TParallelOp (GX_DEFCTOR)
		{}

		TParallelOp (const Self &other) : func( other.func )
		{}

		TParallelOp (Self &&other) : func( RVREF( other.func ) )
		{}

		void Process () const
		{
			return func.Call();
		}

		void operator = (Self &other)
		{
			func.MoveTo( other.func );
		}
	};

	typedef TParallelOp<>	ParallelOp;


}	// Base
}	// Engine
