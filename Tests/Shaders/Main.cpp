// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Engine/STL/Engine.STL.h"

using namespace GX_STL;
using namespace GX_STL::GXTypes;
using namespace GX_STL::GXMath;


extern void TestShader_Temp ();


void main ()
{
	InitializeSTLMath();

	TestShader_Temp();
}
