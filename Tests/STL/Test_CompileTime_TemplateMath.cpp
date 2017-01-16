// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "STL/ux_stl.h"

using namespace GX_STL;
using namespace GX_STL::GXTypes;
using namespace GX_STL::GXMath;


extern void Test_CompileTime_TemplateMath ()
{
	STATIC_ASSERT( (CompileTime::IntLog2< uint, (1<<12) >) == 12 );
	STATIC_ASSERT( (CompileTime::IntLog2< uint, 0 >) == 0 );
	STATIC_ASSERT( (CompileTime::IntLog2< uint, (1<<31) >) == 31 );
	STATIC_ASSERT( (CompileTime::IntLog2< ulong, (ulong(1) << 60) >) == 60 );

	STATIC_ASSERT( (CompileTime::FloorPowerOfTwo< uint, (1<<10) >) == (1<<10) );
	STATIC_ASSERT( (CompileTime::FloorPowerOfTwo< uint, (1<<10) + 20 >) == (1<<10) );
	STATIC_ASSERT( (CompileTime::FloorPowerOfTwo< uint, (1<<10) - 20 >) == (1<<9) );
	
	STATIC_ASSERT( (CompileTime::CeilPowerOfTwo< uint, (1<<15) >) == (1<<15) );
	STATIC_ASSERT( (CompileTime::CeilPowerOfTwo< uint, (1<<15) + 20 >) == (1<<16) );
	STATIC_ASSERT( (CompileTime::CeilPowerOfTwo< uint, (1<<15) - 20 >) == (1<<15) );
}
