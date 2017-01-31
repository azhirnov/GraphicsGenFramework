// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Engine/STL/Engine.STL.h"

using namespace GX_STL;
using namespace CompileTime;


extern void Test_CompileTime_TemplateMath ()
{
	STATIC_ASSERT(( IntLog2< uint, (1<<12) > == 12 ));
	STATIC_ASSERT(( IntLog2< uint, 0 > == 0 ));
	STATIC_ASSERT(( IntLog2< uint, (1<<31) > == 31 ));
	STATIC_ASSERT(( IntLog2< ulong, (ulong(1) << 60) > == 60 ));

	STATIC_ASSERT(( FloorPowerOfTwo< uint, (1<<10) > == (1<<10) ));
	STATIC_ASSERT(( FloorPowerOfTwo< uint, (1<<10) + 20 > == (1<<10) ));
	STATIC_ASSERT(( FloorPowerOfTwo< uint, (1<<10) - 20 > == (1<<9) ));
	
	STATIC_ASSERT(( CeilPowerOfTwo< uint, (1<<15) > == (1<<15) ));
	STATIC_ASSERT(( CeilPowerOfTwo< uint, (1<<15) + 20 > == (1<<16) ));
	STATIC_ASSERT(( CeilPowerOfTwo< uint, (1<<15) - 20 > == (1<<15) ));

	STATIC_ASSERT(( GreatestCommonDivisor< uint, 8, 16 > == 8 ));
}
