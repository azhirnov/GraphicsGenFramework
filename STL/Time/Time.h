// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Math/MathFunc.h"
#include "../Containers/String.h"

namespace GX_STL
{
namespace GXTypes
{
	
	template <typename T>
	struct Time;
	
#	define _TIME_IMPL_

#	define T	double
#	include "FloatTimeImpl.h"

#	define T	float
#	include "FloatTimeImpl.h"

#	define T	uint
#	include "IntTimeImpl.h"
	
#	define T	ulong
#	include "IntTimeImpl.h"

#	undef _TIME_IMPL_

}	// GXTypes
}	// GX_STL
