// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Mathematics.h"
#include "../../OS/OSLowLevel.h"

namespace GX_STL
{
namespace GXMath
{

	
	//
	// Default C Random Engne
	//
	
	struct DefaultRandomEngine
	{
		uint operator () ()	const	{ return rand(); }
		uint Max()			const	{ return RAND_MAX; }
	};



	//
	// Platfomr Random Engine
	//

	struct PlatformRandomEngine
	{
	// variables
	private:
		PlatformRandomDevice	_device;

	// methods
	public:
		PlatformRandomEngine () : _device( StringCRef() )
		{}
		
		uint operator () ()	const	{ uint result;  _device.Generate( result );  return result; }
		uint Max()			const	{ return MaxValue< uint >(); }
	};


	/*
#ifdef PLATFORM_WINDOWS
	template <uint MAX = 0xffff>
	struct TRandomEngineTicks
	{
		uint operator () ()	const	{ return ::GetTickCount() & MAX; }
		uint Max()			const	{ return MAX; }
	};
#endif
	*/


}	// GXMath
}	// GX_STL
