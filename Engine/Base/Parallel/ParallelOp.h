// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/BaseObject.h"

namespace Engine
{
namespace Base
{
	
	//
	// Parallel Operation
	//
	
	struct ParallelOp
	{
	// types
	public:
		typedef ParallelOp			Self;
		typedef Function< void >	Func_t;


	// variables
	private:
		Func_t		_func;


	// methods
	public:
		ParallelOp (GX_DEFCTOR) : _func()
		{}

		ParallelOp (Self &&) = default;
		ParallelOp (const Self &) = default;

		Self& operator = (Self &&) = default;
		Self& operator = (const Self &) = default;

		explicit
		ParallelOp (Func_t &&func) : _func( RVREF( func ) )
		{}

		void Process () const
		{
			return _func.Call();
		}
	};


}	// Base
}	// Engine
