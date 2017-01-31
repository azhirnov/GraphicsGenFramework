﻿// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/STL/Common/Platforms.h"
#include "Engine/Config/STL.Config.h"
#include "Engine/STL/Defines/AuxiliaryDefines.h"
#include "Engine/STL/Defines/Defines.h"
#include "Engine/STL/Defines/EnumHelpers.h"
#include "Engine/STL/Defines/Errors.h"
#include "Engine/STL/Defines/OperatorHelpers.h"
#include "Engine/STL/Defines/MemberDetector.h"
#include "Engine/STL/Log/ELog.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <new>


namespace GX_STL
{
	namespace _defines_hidden_ {}

	namespace GXTypes
	{
		namespace _types_hidden_ {}
	}
	
	namespace TypeTraits
	{
		using namespace GXTypes;

		namespace _ttraits_hidden_ {}
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

		void InitializeSTLMath ();
	}
	
	namespace  Java
	{
		using namespace GXTypes;

		namespace _java_hidden_ {}
	}

	namespace OS
	{
		using namespace GXTypes;

		namespace _os_hidden_ {}
	}

	namespace GXFilters
	{
		using namespace GXTypes;
		using namespace GXMath;
	}

	namespace File	// TODO: rename
	{
		using namespace GXTypes;
	}
	
# ifdef GX_SQLITE_DATABASE_ENABLED
	namespace GXDataBase
	{
		using namespace GXTypes;
	}
# endif
	
# ifdef GX_ANGELSCRIPT_ENABLED
	namespace GXScript
	{
		using namespace GXTypes;

		namespace _script_hidden_ {}
	}
# endif

}
