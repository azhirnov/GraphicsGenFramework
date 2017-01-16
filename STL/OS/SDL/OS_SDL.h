// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Base/Common.h"

#ifdef PLATFORM_SDL

#	pragma warning (push)
#	pragma warning (disable: 4005)	// macros redefinition

#	define SDL_MAIN_HANDLED
#	include <External/SDL2/include/SDL.h>

#	if defined( __GX_DEBUG__ )
#		pragma comment(lib, "SDL2_d.lib")
#	else
#		pragma comment(lib, "SDL2.lib")
#	endif

#	pragma warning (pop)


namespace GX_STL
{
namespace _platform_
{
	using namespace GXTypes;

}	// _platform_
}	// GX_STL

#endif	// PLATFORM_SDL
