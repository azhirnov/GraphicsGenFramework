// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "SDLPlatform.h"

#if defined( PLATFORM_SDL )

namespace Engine
{
namespace SdlPlatform
{
	
/*
=================================================
	constructor
=================================================
*/
	SDLPlatform::SDLPlatform (const IParallelThreadPtr &thread) :
		Platform( thread ), _graphicsContext(),
		_pause(NO_FOCUS), _wnd(null), _isLooping(false), _terminated(false),
		_surfaceCreated(false)
	{
		_timer.Start();

		_directories[ EDirectory::Internal ]			= "internal";
		_directories[ EDirectory::InternalCache ]		= "internal_cache";
		_directories[ EDirectory::External ]			= "external";
		_directories[ EDirectory::ExternalCache ]		= "external_cache";
		_directories[ EDirectory::SDCard ]				= "sdcard";
		_directories[ EDirectory::Library ]				= SDL_GetBasePath();
		
		_UpdateDisplayParams();
	}
	
/*
=================================================
	destructor
=================================================
*/
	SDLPlatform::~SDLPlatform ()
	{
	}
	
/*
=================================================
	Loop
=================================================
*/
	int SDLPlatform::Loop ()
	{
		_EnterForeground( false );

		_EnterForeground( true );
		
		//_SurfaceCreated();

		CHECK_ERR( _EventLoop(), 2 );

		_SurfaceDestroed();

		_EnterBackground( false );

		_Destroy();

		_EnterBackground( true );

		SubSystems()->Get< Application >()->Destroy();

		return 0;
	}
	
/*
=================================================
	InitRender
=================================================
*/
	bool SDLPlatform::InitRender (const VideoSettings &vs)
	{
		CHECK_ERR( _wnd != null and "Window must be created before init OpenGL!", false );
		
		_videoSettings = vs;
		
		CHECK_ERR( _graphicsContext.Init( _wnd, _videoSettings ) );
		return true;
	}
	
/*
=================================================
	InitWindow
=================================================
*/
	bool SDLPlatform::InitWindow (const WindowDesc &windowDesc)
	{
		if ( _wnd != null )
			_Destroy();

		SDL_CHECK( SDL_Init( SDL_INIT_VIDEO ) == 0 );
		//SDL_CHECK( SDL_VideoInit( null ) == 0 );

		const uint2	scr_res	= _GetScreenResolution();

		_window = windowDesc;

		_window.size = Clamp( _window.size, uint2(0), scr_res );
		
		_windowCaption = _window.caption;

		if ( _windowCaption.Empty() )
			_windowCaption = ENGINE_NAME;
		
		if ( _window.fullscreen )
		{
			_window.pos	 = int2();
			_window.size = _display.Resolution();
		}
		else
		{ 
			_window.pos = Max( int2(scr_res - _window.size) / 2, int2(0) );
		}

		uint	flags = int(SDL_WINDOW_OPENGL) | int(SDL_WINDOW_HIDDEN);
		
		if ( windowDesc.resizeble )
			flags |= SDL_WINDOW_RESIZABLE;

		SDL_CHECK( (_wnd = SDL_CreateWindow( _windowCaption.cstr(),
			_window.pos.x, _window.pos.y,
			_window.size.x, _window.size.y,
			flags )) != null );

		return true;
	}
	
/*
=================================================
	InitDisplay
----
	set display mode for fullscreen window
=================================================
*/
	bool SDLPlatform::InitDisplay (const Display &disp)
	{
		CHECK_ERR( _wnd != null );

		SDL_DisplayMode mode = {0};
		SDL_GetCurrentDisplayMode( 0, &mode );

		mode.w				= disp.Resolution().x;
		mode.h				= disp.Resolution().y;
		mode.refresh_rate	= disp.Frequency();

		SDL_CHECK( SDL_SetWindowDisplayMode( _wnd, &mode ) == 0 );
		
		_UpdateDisplayParams();
		_UpdateWindowParams();
		return true;
	}
	
/*
=================================================
	SetDisplayOrientation
=================================================
*/
	void SDLPlatform::SetDisplayOrientation (EDisplayOrientation::type orientation)
	{
		_display.SetOrientation( orientation );

		_SendEvent( SysEvent::Window( SysEvent::Window::ORIENTATION_CHANGED, orientation ) );

		_Resize( _display.Resolution().yx(), true );
	}
	
/*
=================================================
	SwapBuffers
=================================================
*/
	void SDLPlatform::SwapBuffers ()
	{
		_SendEvent( SysEvent::BeforeFlushFrame() );

		_graphicsContext.SwapBuffers();
		
		_SendEvent( SysEvent::AfterFlushFrame() );
	}
		
/*
=================================================
	GetDisplay
=================================================
*/
	void SDLPlatform::GetDisplay (OUT Display &display) const
	{
		display = _display;
	}
	
/*
=================================================
	GetWindowDesc
=================================================
*/
	void SDLPlatform::GetWindowDesc (OUT WindowDesc &window) const
	{
		window			= _window;
		window.caption	= _windowCaption;
	}
	
/*
=================================================
	GetVideoSettings
=================================================
*/
	void SDLPlatform::GetVideoSettings (OUT VideoSettings &vs) const
	{
		vs = _videoSettings;
	}
	
/*
=================================================
	GetCursorState
=================================================
*/
	void SDLPlatform::GetCursorState (OUT CursorDesc &cursor) const
	{
		cursor = _cursor;
	}
	
/*
=================================================
	GetMemory
=================================================
*/
	void SDLPlatform::GetMemory (OUT MemoryDesc &mem) const
	{
		TODO( "GetMemory" );
	}
	
/*
=================================================
	EnableAudio
=================================================
*/
	void SDLPlatform::EnableAudio (bool enabled)
	{
		_SendEvent( SysEvent::Audio( enabled ? SysEvent::Audio::SET_FOCUS : SysEvent::Audio::KILL_FOCUS ) );
	}
		
/*
=================================================
	SetCursorState
=================================================
*/
	void SDLPlatform::SetCursorState (const CursorDesc &cursor)
	{
		if ( cursor.visible != _cursor.visible )
		{
			const int show = int(cursor.visible);
			SDL_CALL( SDL_ShowCursor( show ) == show );
		}

		_cursor = cursor;

		if ( _cursor.alwaysInCenter )
		{
			WARNING( "cursor.alwaysInCenter not supported in SDL!" );
			_cursor.alwaysInCenter = false;
		}
	}
	
/*
=================================================
	Exit
=================================================
*/
	void SDLPlatform::Exit ()
	{
		if ( _terminated )
			return;

		_terminated = true;

		if ( _isLooping )
		{
			_pause |= ON_DESTROY;

			SDL_Event	ev;
			ev.type = SDL_QUIT;

			SDL_PushEvent( &ev );
		}
	}
	
/*
=================================================
	OpenURL
=================================================
*/
	void SDLPlatform::OpenURL (StringCRef url)
	{
		OS::PlatformUtils::OpenURL( url );
	}
	
/*
=================================================
	_GetScreenResolution
=================================================
*/
	uint2 SDLPlatform::_GetScreenResolution ()
	{
		SDL_DisplayMode mode = {0};
		//SDL_GetDesktopDisplayMode( 0, &mode );
		SDL_GetCurrentDisplayMode( 0, &mode );
		return uint2( mode.w, mode.h );
	}
	
/*
=================================================
	_ScreenDPI
=================================================
*/
	float SDLPlatform::_ScreenDPI ()
	{
		// TODO
		return 96.0f;
	}
	
/*
=================================================
	_GetDisplayFrequency
=================================================
*/
	uint SDLPlatform::_GetDisplayFrequency ()
	{
		SDL_DisplayMode mode = {0};
		SDL_GetCurrentDisplayMode( 0, &mode );
		return mode.refresh_rate;
	}
	
/*
=================================================
	_GetWindowPos
=================================================
*/
	int2 SDLPlatform::_GetWindowPos (SDL_Window *wnd)
	{
		int2 pos;
		SDL_GetWindowPosition( wnd, &pos.x, &pos.y );
		return pos;
	}
	
/*
=================================================
	_GetWindowSize
=================================================
*/
	uint2 SDLPlatform::_GetWindowSize (SDL_Window *wnd)
	{
		int2 size;
		SDL_GetWindowSize( wnd, &size.x, &size.y );
		return uint2(size);
	}

/*
=================================================
	_UpdateDisplayParams
=================================================
*/
	void SDLPlatform::_UpdateDisplayParams ()
	{
		_display.SetResolution( _GetScreenResolution() );
		_display.SetPixelsPerInch( _ScreenDPI() );
		_display.SetPhysicsSize( _display.CalcPhysicsSize().Get() );
		_display.SetOrientation( _display.IsHorizontal() ? EDisplayOrientation::Landscape : EDisplayOrientation::Portrait );
		_display.SetFrequency( _GetDisplayFrequency() );
	}
	
/*
=================================================
	_UpdateWindowParams
=================================================
*/
	void SDLPlatform::_UpdateWindowParams ()
	{
		if ( _wnd == null )
		{
			_window	= WindowDesc();
			_windowCaption.Clear();
			return;
		}

		_window.pos			= _GetWindowPos( _wnd );
		_window.size		= _GetWindowSize( _wnd );
		_window.fullscreen	= IsZero( _window.pos ) and All( _window.size == _display.Resolution() );
	}

/*
=================================================
	_Resize
=================================================
*/
	void SDLPlatform::_Resize (uint2 size, bool alignCenter)
	{
		uint2 const	res = _GetScreenResolution();
		int2		pos;

		size = Clamp( size, uint2(12), res );
		
		SDL_SetWindowSize( _wnd, size.x, size.y );

		if ( alignCenter )
		{
			pos = Max( int2(res - size) / 2, int2(0) );

			SDL_SetWindowPosition( _wnd, pos.x, pos.y );
		}
	}

/*
=================================================
	_Destroy
=================================================
*/
	void SDLPlatform::_Destroy ()
	{
		_graphicsContext.Destroy();

		SDL_VideoQuit();
		
		if ( _wnd != null )
		{
			SDL_DestroyWindow( _wnd );
			_wnd = null;
		}
		
		SDL_Quit();
	}
	
/*
=================================================
	_EventLoop
=================================================
*/
	bool SDLPlatform::_EventLoop ()
	{
		if ( _terminated )
			return true;

		_isLooping = true;
		
		SDL_ShowWindow( _wnd );
		_OnResized( _window.size );

		while ( _isLooping )
		{
			_ProcessEvents();
			
			_Update( false );
		}
		return true;
	}
	
/*
=================================================
	_ProcessEvents
=================================================
*/
	void SDLPlatform::_ProcessEvents ()
	{
		SDL_Event	ev;

		while ( SDL_PollEvent( &ev ) )
		{
			switch ( ev.type )
			{
				// touch screen //
				case SDL_FINGERMOTION :
				{
					TODO( "check" );
					_OnTouchMotion( (int)ev.tfinger.fingerId, float2(ev.tfinger.x, ev.tfinger.y) );
					break;
				}

				case SDL_FINGERDOWN :
				case SDL_FINGERUP :
				{
					TODO( "check" );
					_OnTouch( (int)ev.tfinger.fingerId, ev.tfinger.type == SDL_FINGERDOWN, float2(ev.tfinger.x, ev.tfinger.y) );
					break;
				}


				// key //
				case SDL_KEYDOWN :
				case SDL_KEYUP :
				{
					_OnKey( (EKey::type) ev.key.keysym.scancode, ev.type == SDL_KEYDOWN );
					break;
				}


				// quit //
				case SDL_QUIT :
				case SDL_APP_TERMINATING :
				{
					_isLooping = false;
					break;
				}


				// enter background //
				case SDL_APP_WILLENTERBACKGROUND :
					_EnterBackground( false );
					break;

				case SDL_APP_DIDENTERBACKGROUND :
					_EnterBackground( true );
					break;


				// enter foreground //
				case SDL_APP_WILLENTERFOREGROUND :
					_EnterForeground( false );
					break;

				case SDL_APP_DIDENTERFOREGROUND :
					_EnterForeground( true );
					break;


				// window event //
				case SDL_WINDOWEVENT :
				{
					switch ( ev.window.event )
					{
						// show //
						case SDL_WINDOWEVENT_SHOWN :
							_VisibilityChanged( true );
							break;

						// hide //
						case SDL_WINDOWEVENT_HIDDEN :
							_VisibilityChanged( false );
							break;

						// resize //
						case SDL_WINDOWEVENT_RESIZED :
							_OnResized( uint2( ev.window.data1, ev.window.data2 ) );
							break;

						// move //
						case SDL_WINDOWEVENT_MOVED :
							_window.pos = int2( ev.window.data1, ev.window.data2 );
							break;
					}
					break;
				}

				// mouse //
				case SDL_MOUSEMOTION :
				{
					_SendEvent( SysEvent::RawMouseMove(
						_GetTimestamp(),
						int2( ev.motion.xrel, ev.motion.yrel ).To<float2>()
					) );
					
					const float2	point = int2( ev.motion.x, ev.motion.y ).To<float2>();

					if ( EnumEq( ev.motion.state, SDL_BUTTON_LMASK ) )	_OnTouchMotion( 0, point );
					if ( EnumEq( ev.motion.state, SDL_BUTTON_MMASK ) )	_OnTouchMotion( 1, point );
					if ( EnumEq( ev.motion.state, SDL_BUTTON_RMASK ) )	_OnTouchMotion( 2, point );
					if ( EnumEq( ev.motion.state, SDL_BUTTON_X1MASK ) )	_OnTouchMotion( 3, point );
					if ( EnumEq( ev.motion.state, SDL_BUTTON_X2MASK ) )	_OnTouchMotion( 4, point );

					break;
				}

				// mouse button //
				case SDL_MOUSEBUTTONDOWN :
				case SDL_MOUSEBUTTONUP :
				{
					_OnTouch( ev.button.button - SDL_BUTTON_LEFT,
							  ev.button.state == SDL_PRESSED,
							  int2( ev.button.x, ev.button.y ).To<float2>() );
					break;
				}

				// mouse wheel //
				case SDL_MOUSEWHEEL :
				{
					const short		wheel_delta = (short) ev.wheel.y;
					_SendEvent( SysEvent::RawKey( _GetTimestamp(), wheel_delta > 0 ? EKey::M_WHEEL_UP : EKey::M_WHEEL_DOWN, true ) );	
					break;
				}
			}
		}
	}

/*
=================================================
	_EnterForeground
=================================================
*/
	void SDLPlatform::_EnterForeground (bool completed)
	{
		_SendEvent( SysEvent::Application( SysEvent::Application::ENTER_FOREGROUND, completed ) );
	}
	
/*
=================================================
	_EnterBackground
=================================================
*/
	void SDLPlatform::_EnterBackground (bool completed)
	{
		_SendEvent( SysEvent::Application( SysEvent::Application::ENTER_BACKGROUND, completed ) );
	}
	
/*
=================================================
	_SurfaceCreated
=================================================
*/
	void SDLPlatform::_SurfaceCreated ()
	{
		if ( _graphicsContext.IsCreated() and not _surfaceCreated )
		{
			_surfaceCreated = true;

			_graphicsContext.MakeCurrent();

			_SendEvent( SysEvent::Application( SysEvent::Application::SURFACE_CREATED ) );
		}
	}
	
/*
=================================================
	_SurfaceDestroed
=================================================
*/
	void SDLPlatform::_SurfaceDestroed ()
	{
		if ( _graphicsContext.IsCreated() and _surfaceCreated )
		{
			_surfaceCreated = false;

			_SendEvent( SysEvent::Application( SysEvent::Application::SURFACE_DESTROYED ) );

			_graphicsContext.ResetCurrent();
		}
	}

/*
=================================================
	_VisibilityChanged
=================================================
*/
	void SDLPlatform::_VisibilityChanged (bool isVisible)
	{
		_SendEvent( SysEvent::Window( isVisible ? SysEvent::Window::SHOWN : SysEvent::Window::HIDDEN ) );
	}
	
/*
=================================================
	_OnResized
=================================================
*/
	void SDLPlatform::_OnResized (const uint2 &newSize)
	{
		_window.size = newSize;

		_SendEvent( SysEvent::Window( SysEvent::Window::RESIZED, newSize ) );
	}
	
/*
=================================================
	_OnKey
=================================================
*/
	void SDLPlatform::_OnKey (EKey::type keyCode, bool down)
	{
		_SendEvent( SysEvent::RawKey( _GetTimestamp(), keyCode, down ) );
	}
	
/*
=================================================
	_OnTouch
=================================================
*/
	void SDLPlatform::_OnTouch (uint id, bool down, const float2 &point)
	{
		_OnKey( EKey::type( EKey::MOUSE_1 + id ), down );

		_SendEvent( SysEvent::RawTouch( _GetTimestamp(), float2( point.x, _window.size.y - point.y ), down ? 1.0f : 0.0f, id, down, false ) );
	}
	
/*
=================================================
	_OnTouchMotion
=================================================
*/
	void SDLPlatform::_OnTouchMotion (uint id, const float2 &point)
	{
		_SendEvent( SysEvent::RawTouch( _GetTimestamp(), float2( point.x, _window.size.y - point.y ), 1.0f, id, true, true ) );
	}
	
/*
=================================================
	_Update
=================================================
*/
	void SDLPlatform::_Update (bool redraw)
	{
		_SurfaceCreated();

		if ( _graphicsContext.IsCreated() and not _graphicsContext.IsCurrent() )
			_graphicsContext.MakeCurrent();

		Ptr< Application >	app = SubSystems()->Get< Application >();

		app->FlushMessages();
		app->ProcessMessages();

		const SysEvent::time_t	curr_time = _GetTimestamp();

		_SendEvent( SysEvent::Update( curr_time - _lastUpdateTime, redraw, _graphicsContext.IsCreated() ) );

		_lastUpdateTime = curr_time;
	}
	
/*
=================================================
	_SendEvent
=================================================
*/
	template <typename T>
	inline void SDLPlatform::_SendEvent (const T &ev)
	{
		SubSystems()->Get< Application >()->GetEventSystem()->Send( ev );
	}

	
	//
	// System Thread
	//

	class SystemThread : public MainThread
	{
	// variables
	private:
		EngineSubSystems	_subSystems;

	// methods
	public:
		SystemThread () :
			MainThread( SubSystemsRef( _subSystems ) ),
			_subSystems()
		{
		}
	};

/*
=================================================
	__AppEntryPoint
=================================================
*/
	int __AppEntryPoint (void (*createApp) (Base::IParallelThread *thread, OUT Base::Application *&))
	{
		GXMath::InitializeSTLMath();

		CHECK_ERR( createApp != null );

		SystemThread	sys;

		Ptr< Application >	app;
		createApp( &sys, app.ref() );

		CHECK_ERR( app );
		app->GetEventSystem()->Send( SysEvent::Application( SysEvent::Application::CREATED ) );

		int ret = app->SubSystems()->Get< Platform >().ToPtr< SDLPlatform >()->Loop();

		DEBUG_ONLY( Referenced::s_ChenckNotReleasedObjects() );

		return ret;
	}

}	// SdlPlatform
}	// Engine

#endif	// PLATFORM_SDL
