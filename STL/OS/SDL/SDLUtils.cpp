// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "SDLUtils.h"

#if defined( PLATFORM_SDL )

namespace GX_STL
{
namespace _platform_
{
	
/*
=================================================
	_CheckError
=================================================
*/
	bool SDLUtils::_CheckError (StringCRef sdlCall, StringCRef func, StringCRef file, int line)
	{
		StringCRef	err = SDL_GetError();

		if ( err.Empty() )
			return true;
		
		String	str("SDL error: ");
		
		str << err << "\n in " << sdlCall << ", function " << func;

		LOG_EXT( str.cstr(), ELog::Warning, file.cstr(), line );

		SDL_ClearError();
		return false;
	}


}	// _platform_
}	// GX_STL

#endif	// PLATFORM_SDL
