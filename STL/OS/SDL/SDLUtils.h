// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Windows/OSUtils.h"
#include "../Posix/OSUtils.h"

#if defined( PLATFORM_SDL )

namespace GX_STL
{
namespace _platform_
{
	
	//
	// SDL Utils
	//

	struct SDLUtils
	{
	public:
		
		static bool _CheckError (StringCRef sdlCall, StringCRef func, StringCRef file, int line);
	};

	
# ifdef __GX_FAST__

#	define SDL_CALL( ... )		__VA_ARGS__
#	define SDL_CHECK( ... )		__VA_ARGS__

# else

#	define SDL_CALL( ... ) \
	{ \
		bool __sdl_err = not ( __VA_ARGS__ ); \
		if ( __sdl_err ) \
			::GX_STL::_platform_::SDLUtils::_CheckError( TOSTRING(__VA_ARGS__), GX_FUNCTION_NAME, __FILE__, __LINE__ ); \
	}
	
#	define __SDL_CHECK( _func_, _ret_, ... ) \
	{ \
		bool __sdl_err = not ( _func_ ); \
		if ( __sdl_err ) { \
			::GX_STL::_platform_::SDLUtils::_CheckError( TOSTRING(_func_), GX_FUNCTION_NAME, __FILE__, __LINE__ ); \
			return _ret_; \
		} \
	}

#	define SDL_CHECK( ... ) \
		__SDL_CHECK( AUXDEF_GET_FIRST( __VA_ARGS__ ), AUXDEF_GET_SECOND( __VA_ARGS__, false ) )

#endif	// __GX_FAST__


}	// _platform_
}	// GX_STL

#endif	// PLATFORM_SDL
