// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "STL/ux_stl.h"

using namespace GX_STL;
using namespace GX_STL::GXTypes;
using namespace GX_STL::GXMath;


extern void Test_OS_Date ()
{
	STATIC_ASSERT( sizeof(Date) == sizeof(ulong) );

	Date	d;	d.Now();

	Date	d0 = d;
	ASSERT( d0 == d );

	Thread::Sleep(1000);
	d0.Now();

	ASSERT( d0 != d  );
	ASSERT( d0 >  d  );
	ASSERT( d  <  d0 );

	d0.FromMillisecondsSince1970( d.ToMillisecondsSince1970() );
	ASSERT( d0 == d );

	Time<ulong> t0 = PerfomanceTimer().Get( t0 );
	d0.FromMillisecondsSinceEpoch( t0.MilliSeconds() );

	Time<ulong> t1;
	t1.FromMilliSeconds( d0.ToMillisecondsSinceEpoch() );
	ASSERT( t0.MilliSeconds() == t1.MilliSeconds() );


	const char format1[] = "yyyy/mm.dm - hh:mi:ss ms";
	const char format2[] = "yy//mm/dm (dee), dww|mmm  dwww -> mmmm";

	DEBUG_CONSOLE( (String("Date with format \"") << format1 << "\":\n" << d.ToString( format1 )).cstr() );
	DEBUG_CONSOLE( (String("Date with format \"") << format2 << "\":\n" << d.ToString( format2 )).cstr() );
}
