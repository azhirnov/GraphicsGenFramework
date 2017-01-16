// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "SDLOpenGL.h"

#if defined( PLATFORM_SDL ) and ( defined( GRAPHICS_API_OPENGL ) or defined( GRAPHICS_API_OPENGLES ) )

namespace Engine
{
namespace SdlPlatform
{
	
/*
=================================================
	constructor
=================================================
*/
	SDLOpenGLContext::SDLOpenGLContext () :
		_window(null), _context(null)
	{
	}
	
/*
=================================================
	Init
=================================================
*/
	bool SDLOpenGLContext::Init (SDL_Window* wnd, INOUT VideoSettings &vs)
	{
		_window = wnd;

		SDL_GL_ResetAttributes();

		vs.ValidateVersion( true );

#		ifdef GRAPHICS_API_OPENGL
			vs.colorFormat	= VideoSettings::R8_G8_B8_A8;

			SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,	SDL_GL_CONTEXT_PROFILE_CORE );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS,			SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG
																DEBUG_ONLY( | SDL_GL_CONTEXT_DEBUG_FLAG ) );
#		endif

#		ifdef GRAPHICS_API_OPENGLES
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,	SDL_GL_CONTEXT_PROFILE_ES );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_EGL,			1 );
#		endif
			
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION,	vs.MajorVersion() );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION,	vs.MinorVersion() );
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE,				vs.RedBits() );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE,				vs.GreenBits() );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE,				vs.BlueBits() );
		SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE,				vs.AlphaBits() );
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER,			1 );
		SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE,			0 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE,				vs.depthBits );
		SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE,			vs.stencilBits );
		SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL,		1 );
		SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS,		vs.multiSamples > 1 ? 1 : 0 );
		SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES,		vs.multiSamples );

		SDL_CHECK( (_context = SDL_GL_CreateContext( _window )) != null );

		return true;
	}
	
/*
=================================================
	Destroy
=================================================
*/
	void SDLOpenGLContext::Destroy ()
	{
		if ( _window != null )
		{
			SDL_GL_MakeCurrent( _window, null );
			_window = null;
		}

		if ( _context != null )
		{
			SDL_GL_DeleteContext( _context );
			_context = null;
		}
	}


}	// SdlPlatform
}	// Engine

#endif	// PLATFORM_SDL
