// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#include "SDLUtils.h"

#if defined( PLATFORM_SDL )

namespace GX_STL
{
namespace OS
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

		LOG( str.cstr(), ELog::Warning, file.cstr(), line );

		SDL_ClearError();
		return false;
	}


}	// OS
}	// GX_STL

#endif	// PLATFORM_SDL
