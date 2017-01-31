// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/OS/Shared/Platform.h"

#if defined( PLATFORM_SDL )

#include "SDLOpenGL.h"
#include "SDLKeyCodes.h"
#include "Engine/Base/OS/Shared/SystemEvents.h"

namespace Engine
{
namespace SdlPlatform
{
	using namespace Base;

#	if defined( GRAPHICS_API_OPENGL )
		typedef SDLOpenGLContext		GraphicsApiContext;
#	endif

#	define MAIN_APPLICATION_ENTRY( _app_ ) \
		int main (int argc, char *argv[]) \
		{ \
			return ::Engine::SdlPlatform::__AppEntryPoint( &::Engine::Base::Application::EntryPoint< _app_ > ); \
		}

	extern int __AppEntryPoint (void (*) (Base::IParallelThread *thread, OUT Base::Application *&));



	//
	// SDL Platform
	//

	class SDLPlatform : public Platform
	{
	// types
	private:
		enum EPause
		{
			NO_FOCUS	= 0x1,
			USER_PAUSE	= 0x2,
			ON_DESTROY	= 0x4,
		};
		
		typedef StaticArray< String, EDirectory::_Count >	directories_t;


	// variables
	protected:
		GraphicsApiContext			_graphicsContext;

		directories_t				_directories;

		TimeProfilerD				_timer;
		
		SDL_Window	*				_wnd;

		Display						_display;

		VideoSettings				_videoSettings;
		CursorDesc					_cursor;
		WindowDesc					_window;
		String						_windowCaption;
		
		uint						_pause;

		SysEvent::time_t			_lastUpdateTime;

		bool						_isLooping;
		bool						_terminated;
		bool						_surfaceCreated;


	// methods
	public:
		SDLPlatform (const IParallelThreadPtr &thread);
		~SDLPlatform ();

		int  Loop ();

		
	// Platform //
	public:
		bool InitRender (const VideoSettings &vs) override;
		bool InitWindow (const WindowDesc &window) override;
		bool InitDisplay (const Display &disp) override;
		void SetDisplayOrientation (EDisplayOrientation::type orientation) override;
		void SwapBuffers () override;
		
		void GetDisplay (OUT Display &display) const override;
		void GetWindowDesc (OUT WindowDesc &window) const override;
		void GetVideoSettings (OUT VideoSettings &vs) const override;
		void GetCursorState (OUT CursorDesc &cursor) const override;
		void GetMemory (OUT MemoryDesc &mem) const override;

		void EnableAudio (bool enabled) override;
		
		void SetCursorState (const CursorDesc &cursor) override;
		
		void Exit () override;

		void OpenURL (StringCRef url) override;

		StringCRef 	GetDirectoryPath (EDirectory::type type) override	{ return _directories[ type ]; }


	private:
		void  _Resize (uint2 size, bool alignCenter);
		void  _Destroy ();
		bool  _EventLoop ();
		void  _ProcessEvents ();
		void  _UpdateDisplayParams ();
		void  _UpdateWindowParams ();
		
		template <typename T>
		void  _SendEvent (const T &ev);

		// events
		void  _EnterForeground (bool completed);
		void  _EnterBackground (bool completed);
		void  _SurfaceCreated ();
		void  _SurfaceDestroed ();
		void  _VisibilityChanged (bool isVisible);
		void  _OnResized (const uint2 &newSize);
		void  _OnKey (EKey::type keyCode, bool down);
		void  _OnTouch (uint id, bool down, const float2 &point);
		void  _OnTouchMotion (uint id, const float2 &point);
		void  _Update (bool redraw);
		
		// winapi helpers //
		static uint2	_GetScreenResolution ();
		static float	_ScreenDPI ();
		static uint		_GetDisplayFrequency ();
		static int2		_GetWindowPos (SDL_Window *wnd);
		static uint2	_GetWindowSize (SDL_Window *wnd);

		SysEvent::time_t	_GetTimestamp ()	{ return _timer.GetTimeDelta(); }
	};


}	// SdlPlatform
}	// Engine

#endif	// PLATFORM_SDL