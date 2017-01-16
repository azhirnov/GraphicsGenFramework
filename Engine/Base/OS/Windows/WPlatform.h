// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/OS/Shared/Platform.h"

#if defined( PLATFORM_WINDOWS ) and not defined( PLATFORM_SDL )

#include "WOpenGL.h"
#include "WKeyCodes.h"
#include "Engine/Base/OS/Shared/SystemEvents.h"

namespace Engine
{
namespace WinPlatform
{
	using namespace Base;

#	if defined( GRAPHICS_API_OPENGL )
		typedef WindowsOpenGLContext		GraphicsApiContext;
#	endif

#	define MAIN_APPLICATION_ENTRY( _app_ ) \
		int main (int argc, char *argv[]) \
		{ \
			return ::Engine::WinPlatform::__AppEntryPoint( &::Engine::Base::Application::EntryPoint< _app_ > ); \
		}

	extern int __AppEntryPoint (void (*) (Base::IParallelThread *thread, OUT Base::Application *&));



	//
	// Windows Platform
	//

	class WindowsPlatform : public Platform
	{
	// types
	private:
		enum EPause
		{
			NO_FOCUS	= 0x1,
			USER_PAUSE	= 0x2,
			ON_DESTROY	= 0x4,
		};
		
		typedef StaticArray< String, EDirectory::_COUNT >	directories_t;


	// variables
	protected:
		GraphicsApiContext			_graphicsContext;

		directories_t				_directories;

		TimeProfiler<double>		_timer;
		
		winapi::HWND				_wnd;
		winapi::HDC					_dc;

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
		WindowsPlatform (const IParallelThreadPtr &thread);
		~WindowsPlatform ();

		int  Loop ();

		
	// Platform //
	public:
		bool InitRender (const VideoSettings &vs) override;
		bool InitWindow (const WindowDesc &window) override;
		void InitDisplay (const Display &disp) override;
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
		bool  _RegisterClass ();
		bool  _InitRawInput ();
		void  _Resize (int2 size, bool alignCenter);
		isize _ProcessMessage (UINT uMsg, WPARAM wParam, LPARAM lParam);
		void  _Destroy ();
		bool  _EventLoop ();
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
		void  _OnResized (const int2 &newSize);
		void  _OnKey (EKey::type keyCode, bool down);
		void  _OnTouch (uint id, bool down, const float2 &point);
		void  _OnTouchMotion (uint id, const float2 &point);
		void  _Update (bool redraw);

		static LRESULT CALLBACK _MsgProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		
		// winapi helpers //
		static int2		_GetScreenResolution ();
		static float2	_ScreenPhysicalSize ();
		static uint		_GetDisplayFrequency ();
		static bool		_ChangeScreenResolution (const int2 &size, uint freq = 0);
		static bool		_ChangeToDefaultResolution ();
		static bool		_GetLibraryPath (OUT String &path);
		static int2		_GetWindowPos (HWND wnd);
		static int2		_GetWindowSize (HWND wnd);
		static String	_GetClassName ();
		static HMODULE	_GetInstance ();

		SysEvent::time_t	_GetTimestamp ()	{ return _timer.GetTimeDelta(); }
	};


}	// WinPlatform
}	// Engine

#endif	// PLATFORM_WINDOWS