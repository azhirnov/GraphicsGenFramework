// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Platforms/Platforms.h"
#include "STL/Defines/DefConfig.h"
#include "STL/Defines/Defines.h"
#include "STL/Defines/EnumHelpers.h"

// in namespace _hidden_
#include "../LibC/libc.h"


namespace GX_STL
{
	namespace _defines_hidden_ {}

	namespace GXTypes
	{
		namespace _types_hidden_ {}
	}
	
	namespace CompileTime
	{
		using namespace GXTypes;

		namespace _ctime_hidden_ {}
	}

	namespace GXMath
	{
		using namespace GXTypes;

		namespace _math_hidden_ {}
		namespace _color_hidden_ {}
	}
	
	namespace  Java
	{
		namespace _java_hidden_ {}
	}
}
