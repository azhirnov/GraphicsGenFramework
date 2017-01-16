// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#include "STL/ux_stl.h"

using namespace GX_STL;
using namespace GX_STL::GXTypes;
using namespace GX_STL::GXMath;


typedef Union< byte, short, int, ilong, float, double >		Union_t;


void Union_Test1 ()
{
	float		f1 = 1.74323f;
	Union_t		u;	u.Create( f1 );
	float		f2 = u.Get<float>();

	ASSERT( f1 == f2 );
}


extern void Test_Types_Union ()
{
	Union_Test1();
}
