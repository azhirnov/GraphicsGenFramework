// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Enums.h"

namespace Engine
{
namespace Graphics
{

	//
	// Memory Barrier Object
	//

	struct MemoryBarrierObj
	{
	// variables
	private:
		uint					_ticks;
		EMemoryBarrier::type	_barrier;

	// methods
	public:
		MemoryBarrierObj () : _ticks( TicksInit() ), _barrier( EMemoryBarrier::type(0) )
		{}

		explicit
		MemoryBarrierObj (uint ticks) : _ticks( ticks ), _barrier( EMemoryBarrier::type(0) )
		{}

		void Reset ()
		{
			_ticks	 = TicksInit();
			_barrier = EMemoryBarrier::type(0);
		}

		void Filter (INOUT EMemoryBarrier::type &flags)
		{
			flags = flags & ~_barrier;

			_barrier |= flags;
		}

		uint	Ticks () const						{ return _ticks; }

		bool	IsActive ()	const					{ return _ticks != TicksInit(); }

		static constexpr uint	TicksInit ()		{ return 1 << 31; }
	};


}	// Graphics
}	// Engine
