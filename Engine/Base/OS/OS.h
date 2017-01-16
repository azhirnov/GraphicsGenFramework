// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Windows/WApplication.h"
#include "SDL/SDLApplication.h"

namespace Engine
{
namespace Base
{
	
#	if defined( PLATFORM_SDL )
		
		typedef SdlPlatform::SDLApplication		BaseApplication;
		typedef SdlPlatform::SDLLibOpenGL		LibOpenGL;

#	endif

#	if defined( PLATFORM_WINDOWS ) and not defined( PLATFORM_SDL )
		
		typedef WinPlatform::WindowsApplication		BaseApplication;
		typedef WinPlatform::WindowsLibOpenGL		LibOpenGL;

#	endif

}	// Base
}	// Engine
