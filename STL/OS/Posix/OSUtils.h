// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "OSPosix.h"

#ifdef PLATFORM_BASE_POSIX

namespace GX_STL
{
namespace _platform_
{
	using namespace posix;


	//
	// OS Utils
	//

	struct OSUtils : public Noninstancable
	{
		static void OpenURL (StringCRef url);
	};

	
	inline void OSUtils::OpenURL (StringCRef url)
	{
		TODO( "OpenURL" );
	}


}	// _platform_
}	// GX_STL

#endif	// PLATFORM_BASE_POSIX