// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/OS/Shared/Application.h"

#if defined( PLATFORM_SDL ) and ( defined( GRAPHICS_API_OPENGL ) or defined( GRAPHICS_API_OPENGLES ) )

namespace Engine
{
namespace SdlPlatform
{
	using namespace Base;



	//
	// SDL OpenGL Library
	//

	struct SDLLibOpenGL
	{
	// methods
	public:
		SDLLibOpenGL ()
		{}

		bool Load (StringCRef name = StringCRef())
		{
			return SDL_GL_LoadLibrary( name.Empty() ? null : name.cstr() ) == 0;
		}

		void Unload ()
		{
			SDL_GL_UnloadLibrary();
		}

		void * GetProc (StringCRef address) const
		{
			return SDL_GL_GetProcAddress( address.cstr() );
		}

		static StringCRef  GetDefaultName ()
		{
			return null;
		}
	};



	//
	// SDL OpenGL Context
	//

	class SDLOpenGLContext : public Noncopyable
	{
	// variables
	private:
		SDL_Window *	_window;
		SDL_GLContext	_context;


	// methods
	public:
		SDLOpenGLContext ();

		bool Init (SDL_Window* wnd, INOUT VideoSettings &vs);

		void Destroy ();

		void MakeCurrent ()
		{
			CHECK( SDL_GL_MakeCurrent( _window, _context ) == 0 );
		}

		void ResetCurrent ()
		{
			CHECK( SDL_GL_MakeCurrent( _window, null ) == 0 );
		}

		bool IsCreated () const
		{
			return _window != null and _context != null;
		}

		bool IsCurrent ()
		{
			return IsCreated() and ::SDL_GL_GetCurrentContext() == _context;
		}

		void SwapBuffers ()
		{
			if ( IsCreated() )
			{
				SDL_GL_SwapWindow( _window );
			}
		}
	};


}	// SdlPlatform
}	// Engine

#endif	// PLATFORM_SDL
