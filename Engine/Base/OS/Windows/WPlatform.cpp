// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "WPlatform.h"

#if defined( PLATFORM_WINDOWS ) and not defined( PLATFORM_SDL )

namespace Engine
{
namespace WinPlatform
{
/*
=================================================
	constructor
=================================================
*/
	WindowsPlatform::WindowsPlatform (const IParallelThreadPtr &thread) :
		Platform( thread ), _graphicsContext(),
		_pause(NO_FOCUS), _wnd(null), _dc(null), _isLooping(false), _terminated(false),
		_surfaceCreated(false)
	{
		_timer.Start();

		_directories[ EDirectory::Internal ]		= "internal";
		_directories[ EDirectory::InternalCache ]	= "internal_cache";
		_directories[ EDirectory::External ]		= "external";
		_directories[ EDirectory::ExternalCache ]	= "external_cache";
		_directories[ EDirectory::SDCard ]			= "sdcard";
		_GetLibraryPath( _directories[ EDirectory::Library ] );
		
		_UpdateDisplayParams();
	}

/*
=================================================
	destructor
=================================================
*/
	WindowsPlatform::~WindowsPlatform ()
	{
	}

/*
=================================================
	Loop
=================================================
*/
	int WindowsPlatform::Loop ()
	{
		_EnterForeground( false );

		_EnterForeground( true );
		
		//_SurfaceCreated();

		CHECK_ERR( _EventLoop(), 1 );

		_SurfaceDestroed();

		_EnterBackground( false );

		_Destroy();

		_EnterBackground( true );

		SubSystems()->Get< Application >()->Destroy();

		return 0;
	}
	
/*
=================================================
	SwapBuffers
=================================================
*/
	void WindowsPlatform::SwapBuffers ()
	{
		_SendEvent( SysEvent::BeforeFlushFrame() );

		_graphicsContext.SwapBuffers();
		
		_SendEvent( SysEvent::AfterFlushFrame() );
	}
	
/*
=================================================
	EnableAudio
=================================================
*/
	void WindowsPlatform::EnableAudio (bool enabled)
	{
		_SendEvent( SysEvent::Audio( enabled ? SysEvent::Audio::SET_FOCUS : SysEvent::Audio::KILL_FOCUS ) );
	}
	
/*
=================================================
	SetDisplayOrientation
=================================================
*/
	void WindowsPlatform::SetDisplayOrientation (EDisplayOrientation::type orientation)
	{
		_display.SetOrientation( orientation );

		_SendEvent( SysEvent::Window( SysEvent::Window::ORIENTATION_CHANGED, orientation ) );

		_Resize( _display.Resolution().yx(), true );
	}
	
/*
=================================================
	GetDisplay
=================================================
*/
	void WindowsPlatform::GetDisplay (OUT Display &display) const
	{
		display = _display;
	}
	
/*
=================================================
	GetWindowDesc
=================================================
*/
	void WindowsPlatform::GetWindowDesc (OUT WindowDesc &window) const
	{
		window			= _window;
		window.caption	= _windowCaption;
	}
	
/*
=================================================
	GetVideoSettings
=================================================
*/
	void WindowsPlatform::GetVideoSettings (OUT VideoSettings &vs) const
	{
		vs = _videoSettings;
	}

/*
=================================================
	GetCursorState
=================================================
*/
	void WindowsPlatform::GetCursorState (OUT CursorDesc &cursor) const
	{
		cursor = _cursor;
	}

/*
=================================================
	GetMemory
=================================================
*/
	void WindowsPlatform::GetMemory (OUT MemoryDesc &mem) const
	{
		MEMORYSTATUSEX stat;
		stat.dwLength = sizeof(stat);
		GlobalMemoryStatusEx( &stat );
		mem.total		= Bytes<ulong>( stat.ullTotalPhys );
		mem.available	= Bytes<ulong>( stat.ullAvailPhys );
	}
	
/*
=================================================
	SetCursorState
=================================================
*/
	void WindowsPlatform::SetCursorState (const CursorDesc &cursor)
	{
		if ( cursor.visible != _cursor.visible )
		{
			const int show = cursor.visible ? TRUE : FALSE;
			CHECK( ShowCursor( cursor.visible ) == show );
		}

		_cursor = cursor;
	}
	
/*
=================================================
	Exit
=================================================
*/
	void WindowsPlatform::Exit ()
	{
		if ( _terminated )
			return;

		_terminated = true;

		if ( _isLooping )
		{
			_pause |= ON_DESTROY;
			PostMessageA( _wnd, WM_CLOSE, 0, 0 );
		}
	}

/*
=================================================
	OpenURL
=================================================
*/
	void WindowsPlatform::OpenURL (StringCRef url)
	{
		PlatformUtils::OpenURL( url );
	}
	
/*
=================================================
	InitDisplay
=================================================
*/
	void WindowsPlatform::InitDisplay (const Display &disp)
	{
		if ( Any( disp.Resolution() != _display.Resolution() ) or
			 disp.Frequency() != _display.Frequency() )
		{
			_ChangeScreenResolution( disp.Resolution(), disp.Frequency() );
		}
		
		_display = disp;

		_UpdateDisplayParams();
		_UpdateWindowParams();
	}
	
/*
=================================================
	_UpdateDisplayParams
=================================================
*/
	void WindowsPlatform::_UpdateDisplayParams ()
	{
		_display.SetResolution( _GetScreenResolution() );
		_display.SetPhysicsSize( _ScreenPhysicalSize() );
		_display.SetPixelsPerInch( _display.CalcPixelsPerInch().Max() );
		_display.SetOrientation( _display.IsHorizontal() ? EDisplayOrientation::Landscape : EDisplayOrientation::Portrait );
		_display.SetFrequency( _GetDisplayFrequency() );
	}

/*
=================================================
	InitRender
=================================================
*/
	bool WindowsPlatform::InitRender (const VideoSettings &vs)
	{
		CHECK_ERR( _dc != null );

		_videoSettings = vs;

		CHECK_ERR( _graphicsContext.Init( _dc, _videoSettings ) );
		return true;
	}

/*
=================================================
	InitWindow
=================================================
*/
	bool WindowsPlatform::InitWindow (const WindowDesc &windowDesc)
	{
		if ( _dc != null )
			_Destroy();
		
		const int2	scr_res	= _GetScreenResolution();

		_window = windowDesc;

		_window.size = Clamp( _window.size, int2(0), scr_res );
		
		_windowCaption = _window.caption;

		if ( _windowCaption.Empty() )
			_windowCaption = ENGINE_NAME;

		DWORD	wndStyle	= 0;
		DWORD	wndExtStyle	= WS_EX_APPWINDOW;
		RECT	winRect		= { 0, 0, _window.size.x, _window.size.y };

		CHECK_ERR( _RegisterClass() );

		if ( _window.fullscreen )
		{
			wndStyle		|= WS_POPUP;
			_window.pos		 = int2();
			_window.size	 = _display.Resolution();
		}
		else
		{ 
			if ( windowDesc.resizeble )
				wndStyle = WS_OVERLAPPEDWINDOW;
			else
				wndStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

			AdjustWindowRectEx( &winRect, wndStyle, 0, wndExtStyle );

			if ( All( _window.pos < -_window.size ) )
			{
				_window.pos.x = Max( 0, scr_res.x - int(winRect.right - winRect.left) ) / 2;
				_window.pos.y = Max( 0, scr_res.y - int(winRect.bottom - winRect.top) ) / 2;		
			}
		}

		_wnd = CreateWindowEx(	wndExtStyle,
								_GetClassName().cstr(),
								_windowCaption.cstr(),
								wndStyle,
								_window.pos.x,
								_window.pos.y,
								winRect.right - winRect.left,
								winRect.bottom - winRect.top,
								null,
								null,
								_GetInstance(),
								null );

		CHECK_ERR( _wnd != null );

		_dc = GetDC( _wnd );
		CHECK_ERR( _dc != null );

		_InitRawInput();

		_UpdateDisplayParams();
		_UpdateWindowParams();
		return true;
	}
	
/*
=================================================
	_UpdateWindowParams
=================================================
*/
	void WindowsPlatform::_UpdateWindowParams ()
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
	_RegisterClass
=================================================
*/
	bool WindowsPlatform::_RegisterClass ()
	{
		HMODULE			instance	= _GetInstance();
		const String	class_name	= _GetClassName();

		WNDCLASSA	tmp = {0};
		bool		ret = GetClassInfo( instance, class_name.cstr(), &tmp ) == TRUE;
		
		if ( ret or ( class_name == tmp.lpszClassName and instance == tmp.hInstance ) )
			return true;

		WNDCLASSEXA		windowClass = {0};
		windowClass.cbSize			= sizeof(windowClass);
		windowClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		windowClass.lpfnWndProc		= &DefWindowProcA;
		windowClass.cbClsExtra		= 0;
		windowClass.cbWndExtra		= 0;
		windowClass.hInstance		= instance;
		windowClass.hbrBackground	= (HBRUSH) COLOR_WINDOW+1; //(HBRUSH) COLOR_WINDOWFRAME;
		windowClass.hIcon			= (HICON) null;
		windowClass.hCursor			= LoadCursor( null, IDC_ARROW );
		windowClass.lpszMenuName	= null;
		windowClass.lpszClassName	= class_name.cstr();
		windowClass.hIconSm			= (HICON) null;

		CHECK_ERR( RegisterClassEx( &windowClass ) != 0 );

		return true;
	}
	
/*
=================================================
	_InitRawInput
=================================================
*/
	bool WindowsPlatform::_InitRawInput ()
	{
		CHECK_ERR( _wnd != null );

		RAWINPUTDEVICE	Rid[2];
		// usage params:
		// 0x02 - mouse, 0x04 - joystick, 0x05 - game pad, 0x06 - keyboard
		
		// mouse
		Rid[0].usUsagePage	= 0x01;
		Rid[0].usUsage		= 0x02;
		Rid[0].dwFlags		= 0;
		Rid[0].hwndTarget	= _wnd;
		
		// keyboard
		Rid[1].usUsagePage	= 0x01;
		Rid[1].usUsage		= 0x06;
		Rid[1].dwFlags		= 0;	// RIDEV_INPUTSINK | RIDEV_NOHOTKEYS | RIDEV_NOLEGACY | RIDEV_REMOVE;
		Rid[1].hwndTarget	= _wnd;
		
		CHECK_ERR( RegisterRawInputDevices( &Rid[0], 2, sizeof(Rid[0]) ) == TRUE );
		return true;
	}
	
/*
=================================================
	_Resize
=================================================
*/
	void WindowsPlatform::_Resize (int2 size, bool alignCenter)
	{
		int2 const	res = _GetScreenResolution().To<int2>();
		int2		pos;

		size = Clamp( size, int2(12), res );

		DWORD dwStyle	= (DWORD) GetWindowLongA( _wnd, GWL_STYLE );
		DWORD dwExStyle	= (DWORD) GetWindowLongA( _wnd, GWL_EXSTYLE );

		RECT	winRect = { 0, 0, size.x, size.y };
		AdjustWindowRectEx( &winRect, dwStyle, FALSE, dwExStyle );
		
		TODO( "is window size setted?" );

		if ( alignCenter )
		{
			pos.x = Max( 0, res.x - int(winRect.right  - winRect.left) ) / 2;
			pos.y = Max( 0, res.y - int(winRect.bottom - winRect.top) ) / 2;

			SetWindowPos( _wnd, HWND_TOP, pos.x, pos.y, winRect.right - winRect.left,
							winRect.bottom - winRect.top, SWP_FRAMECHANGED );
		}
	}

/*
=================================================
	_ProcessMessage
=================================================
*/
	isize WindowsPlatform::_ProcessMessage (UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// WM_PAINT //
		if ( uMsg == WM_PAINT and _pause == 0 )
		{
			_Update( true );
		}
		else

		// WM_INPUT //
		if ( uMsg == WM_INPUT and _pause == 0 )
		{
			ubyte	a_data[60];
			uint	u_size = sizeof(a_data);

			if ( GetRawInputData( (HRAWINPUT)lParam, RID_INPUT, a_data, &u_size, sizeof(RAWINPUTHEADER) ) != -1 )
			{
				RAWINPUT  *	p_data = (RAWINPUT *)a_data;

				// KEYBOARD //
				if ( p_data->header.dwType == RIM_TYPEKEYBOARD )
				{
					ushort	u_right = p_data->data.keyboard.Flags & RI_KEY_E0 ? EKey::right_offset : 0;
					
					if ( p_data->data.keyboard.VKey + u_right >= EKey::_COUNT )
						u_right = 0;

					bool	pressed = not EnumEq( p_data->data.keyboard.Flags, RI_KEY_BREAK );
					int		keyCode = p_data->data.keyboard.VKey + u_right;

					_SendEvent( SysEvent::RawKey( _GetTimestamp(), EKey::type(keyCode), pressed ) );
				}
				else
					
				// MOUSE //
				if ( p_data->header.dwType == RIM_TYPEMOUSE )
				{
					_SendEvent( SysEvent::RawMouseMove(
						_GetTimestamp(),
						float2( (float)p_data->data.mouse.lLastX, (float)p_data->data.mouse.lLastY )
					) );

					if ( EnumEq( p_data->data.mouse.usButtonFlags, RI_MOUSE_WHEEL ) )
					{
						const short		wheel_delta = short( p_data->data.mouse.usButtonData ) / WHEEL_DELTA;
						_SendEvent( SysEvent::RawKey( _GetTimestamp(), wheel_delta > 0 ? EKey::M_WHEEL_UP : EKey::M_WHEEL_DOWN, true ) );
					}
				}
			}
		}
		else

		// WM_MOUSEMOVE //
		// WM_LBUTTONUP //
		// WM_LBUTTONDOWN //
		if ( _pause == 0 and (
			(uMsg == WM_MOUSEMOVE or
			 uMsg == WM_LBUTTONDOWN or uMsg == WM_LBUTTONUP or
			 uMsg == WM_RBUTTONDOWN or uMsg == WM_RBUTTONUP or
			 uMsg == WM_MBUTTONDOWN or uMsg == WM_MBUTTONUP or
			 uMsg == WM_XBUTTONDOWN or uMsg == WM_XBUTTONUP ) ) )
		{
			const float2	point( LOWORD( lParam ), HIWORD( lParam ) );

			if ( uMsg == WM_MOUSEMOVE )
			{
				if ( EnumEq( wParam, MK_LBUTTON ) )		_OnTouchMotion( 0, point );
				if ( EnumEq( wParam, MK_RBUTTON ) )		_OnTouchMotion( 1, point );
				if ( EnumEq( wParam, MK_MBUTTON ) )		_OnTouchMotion( 2, point );
				if ( EnumEq( wParam, MK_XBUTTON1 ) )	_OnTouchMotion( 3, point );
				if ( EnumEq( wParam, MK_XBUTTON2 ) )	_OnTouchMotion( 4, point );
			}
			else
			{
				if ( uMsg == WM_LBUTTONDOWN or uMsg == WM_LBUTTONUP )
					_OnTouch( 0, uMsg == WM_LBUTTONDOWN, point );
				else
				if ( uMsg == WM_RBUTTONDOWN or uMsg == WM_RBUTTONUP )
					_OnTouch( 1, uMsg == WM_RBUTTONDOWN, point );
				else
				if ( uMsg == WM_MBUTTONDOWN or uMsg == WM_MBUTTONUP )
					_OnTouch( 2, uMsg == WM_MBUTTONDOWN, point );
				else
				if ( uMsg == WM_XBUTTONDOWN or uMsg == WM_XBUTTONUP )
					_OnTouch( 3, uMsg == WM_XBUTTONDOWN, point );
			}
		}
		else

		// WM_MOVE //
		if ( uMsg == WM_MOVE )
		{
			RECT	winRect = {0,0,0,0};
			GetWindowRect( _wnd, &winRect );
			int2	new_pos = int2( winRect.left, winRect.top );

			_window.pos = new_pos;
		}
		else

		// WM_KILLPOCUS //
		if ( uMsg == WM_KILLFOCUS )
		{
			_pause |= NO_FOCUS;
			_VisibilityChanged( false );
		}
		else

		// WM_SETFOCUS //
		if ( uMsg == WM_SETFOCUS )
		{
			_pause &= ~NO_FOCUS;
			_VisibilityChanged( true );
		}
		else
			
		// WM_SIZE //
		if ( uMsg == WM_SIZE )
		{
			if ( wParam != SIZE_MINIMIZED )
			{
				_OnResized( int2( LOWORD( lParam ), HIWORD( lParam ) ) );
			}
		}
		else

		// WM_SYSCOMMAND //
		if ( uMsg == WM_SYSCOMMAND and (wParam == SC_SCREENSAVE or wParam == SC_MONITORPOWER) )
			return 0;
		else

		// WM_CLOSE //
		if ( uMsg == WM_CLOSE ) {
			PostMessageA( _wnd, WM_QUIT, 0, 0 );
			return 1;
		}

		return DefWindowProcA( _wnd, uMsg, wParam, lParam );
	}
	
/*
=================================================
	_MsgProc
=================================================
*/
	LRESULT CALLBACK WindowsPlatform::_MsgProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WindowsPlatform *	wp = (WindowsPlatform *)GetWindowLongPtrA( hWnd, GWLP_USERDATA );
		return wp->_ProcessMessage( uMsg, wParam, lParam );
	}

/*
=================================================
	_EventLoop
=================================================
*/
	bool WindowsPlatform::_EventLoop ()
	{
		if ( _terminated )
			return true;

		CHECK_ERR( _wnd != null );

		MSG		msg	= {0};
		
		_isLooping = true;
		
		_graphicsContext.ResetCurrent();

		SetWindowLongPtrA( _wnd, GWLP_USERDATA, (LONG_PTR) this );
		SetWindowLongPtrA( _wnd, GWLP_WNDPROC,  (LONG_PTR) &_MsgProc );

		SetForegroundWindow( _wnd );
		ShowWindow( _wnd, SW_SHOWNA );

		while ( _isLooping )
		{
			while ( PeekMessageA( &msg, 0, 0, 0, PM_REMOVE ) )
			{
				if ( WM_QUIT == msg.message )
					_isLooping = false;
				else
					DispatchMessageA( &msg );
			}

			_Update( false );
		}
		
		SetWindowLongPtrA( _wnd, GWLP_WNDPROC, (LONG_PTR) DefWindowProcA );

		CHECK_ERR( ((int)msg.wParam) != -1 );
		return true;
	}

/*
=================================================
	_Destroy
=================================================
*/
	void WindowsPlatform::_Destroy ()
	{
		if ( _dc != null )
		{
			_graphicsContext.Destroy();

			ReleaseDC( _wnd, _dc );
			_dc = null;
		}

		if ( _wnd != null )
		{
			DestroyWindow( _wnd );
			_wnd = null;

			if ( _window.fullscreen )
				_ChangeToDefaultResolution();
		}

		UnregisterClass( _GetClassName().cstr(), _GetInstance() );

		_window = WindowDesc();
	}
	
/*
=================================================
	_GetScreenResolution
=================================================
*/
	int2 WindowsPlatform::_GetScreenResolution ()
	{
		enum {
			SM_CXSCREEN	= 0,
			SM_CYSCREEN	= 1,
		};
		return int2( GetSystemMetrics( SM_CXSCREEN ),
					 GetSystemMetrics( SM_CYSCREEN ) );
	}
	
/*
=================================================
	_ScreenPhysicalSize
=================================================
*/
	float2 WindowsPlatform::_ScreenPhysicalSize ()
	{
		enum {
			HORZSIZE	= 4,
			VERTSIZE	= 6,
		};
		float2	size;
		HDC		hDCScreen	= GetDC( null );
		
		size.x = (float) GetDeviceCaps( hDCScreen, HORZSIZE );
		size.y = (float) GetDeviceCaps( hDCScreen, VERTSIZE );
		
		ReleaseDC( null, hDCScreen );
		return size * 0.001f;
	}
	
/*
=================================================
	_GetDisplayFrequency
=================================================
*/
	uint WindowsPlatform::_GetDisplayFrequency ()
	{
		enum {
			VREFRESH	= 116
		};
		HDC		hDCScreen	= GetDC( null );
		
		uint freq = GetDeviceCaps( hDCScreen, VREFRESH );
		
		ReleaseDC( null, hDCScreen );
		return freq;
	}

/*
=================================================
	_ChangeScreenResolution
=================================================
*/
	bool WindowsPlatform::_ChangeScreenResolution (const int2 &size, uint freq)
	{
		DEVMODEA	mode = {0};
		mode.dmSize				= sizeof( mode );
		mode.dmPelsWidth		= (DWORD) size.x;
		mode.dmPelsHeight		= (DWORD) size.y;
		mode.dmBitsPerPel		= (DWORD) 8;
		mode.dmDisplayFrequency	= (DWORD) freq;
		mode.dmFields			= DM_BITSPERPEL | DM_PELSWIDTH |
								  DM_PELSHEIGHT | (freq ? DM_DISPLAYFREQUENCY : 0);

		if ( ChangeDisplaySettings( &mode, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
		{
			mode.dmFields &= ~DM_DISPLAYFREQUENCY;

			CHECK_ERR( ChangeDisplaySettings( &mode, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL );
		}

		return true;
	}
	
/*
=================================================
	_ChangeToDefaultResolution
=================================================
*/
	bool WindowsPlatform::_ChangeToDefaultResolution ()
	{
		CHECK_ERR( ChangeDisplaySettings( (DEVMODEA *)null, CDS_RESET ) != DISP_CHANGE_SUCCESSFUL );
		return true;
	}

/*
=================================================
	_GetLibraryPath
=================================================
*/
	bool WindowsPlatform::_GetLibraryPath (OUT String &path)
	{
		char		buf[ MAX_PATH<<2 ] = {0};
		const usize	len = GetModuleFileName( _GetInstance(), buf, (DWORD) CountOf(buf) );

		if ( len != 0 )
		{
			path = FileAddressUtils::GetPath( StringCRef( buf ) );
			return true;
		}
		return false;
	}
	
/*
=================================================
	_GetWindowPos
=================================================
*/
	int2 WindowsPlatform::_GetWindowPos (HWND wnd)
	{
		RECT	win_rect = {0,0,0,0};
		winapi::GetWindowRect( wnd, &win_rect );
		return int2( win_rect.left, win_rect.top );
	}
	
/*
=================================================
	_GetWindowSize
=================================================
*/
	int2 WindowsPlatform::_GetWindowSize (HWND wnd)
	{
		RECT	win_rect = {0,0,0,0};
		winapi::GetClientRect( wnd, &win_rect );
		return int2( win_rect.right - win_rect.left, win_rect.bottom - win_rect.top );
	}

/*
=================================================
	_GetClassName
=================================================
*/
	String WindowsPlatform::_GetClassName ()
	{
		return String(ENGINE_NAME) << "_class";
	}
	
/*
=================================================
	_GetInstance
=================================================
*/
	HMODULE WindowsPlatform::_GetInstance ()
	{
		return GetModuleHandle( (LPCSTR) null );
	}

/*
=================================================
	_EnterForeground
=================================================
*/
	void WindowsPlatform::_EnterForeground (bool completed)
	{
		_SendEvent( SysEvent::Application( SysEvent::Application::ENTER_FOREGROUND, completed ) );
	}
	
/*
=================================================
	_EnterBackground
=================================================
*/
	void WindowsPlatform::_EnterBackground (bool completed)
	{
		_SendEvent( SysEvent::Application( SysEvent::Application::ENTER_BACKGROUND, completed ) );
	}
	
/*
=================================================
	_SurfaceCreated
=================================================
*/
	void WindowsPlatform::_SurfaceCreated ()
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
	void WindowsPlatform::_SurfaceDestroed ()
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
	void WindowsPlatform::_VisibilityChanged (bool isVisible)
	{
		_SendEvent( SysEvent::Window( isVisible ? SysEvent::Window::SHOWN : SysEvent::Window::HIDDEN ) );
	}
	
/*
=================================================
	_OnResized
=================================================
*/
	void WindowsPlatform::_OnResized (const int2 &newSize)
	{
		_window.size = newSize;

		_SendEvent( SysEvent::Window( SysEvent::Window::RESIZED, newSize ) );
	}
	
/*
=================================================
	_OnKey
=================================================
*/
	void WindowsPlatform::_OnKey (EKey::type keyCode, bool down)
	{
		_SendEvent( SysEvent::RawKey( _GetTimestamp(), keyCode, down ) );
	}
	
/*
=================================================
	_OnTouch
=================================================
*/
	void WindowsPlatform::_OnTouch (uint id, bool down, const float2 &point)
	{
		_OnKey( EKey::type( EKey::MOUSE_1 + id ), down );

		_SendEvent( SysEvent::RawTouch( _GetTimestamp(), float2( point.x, _window.size.y - point.y ), down ? 1.0f : 0.0f, id, down, false ) );
	}
	
/*
=================================================
	_OnTouchMotion
=================================================
*/
	void WindowsPlatform::_OnTouchMotion (uint id, const float2 &point)
	{
		_SendEvent( SysEvent::RawTouch( _GetTimestamp(), float2( point.x, _window.size.y - point.y ), 1.0f, id, true, true ) );
	}
	
/*
=================================================
	_Update
=================================================
*/
	void WindowsPlatform::_Update (bool redraw)
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

		if ( _cursor.alwaysInCenter )
		{
			const int2	pos = _window.size / 2;
			SetCursorPos( pos.x, pos.y );
		}
	}
	
/*
=================================================
	_SendEvent
=================================================
*/
	template <typename T>
	inline void WindowsPlatform::_SendEvent (const T &ev)
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

		CHECK_ERR( app.IsNotNull() );

		app->GetEventSystem()->Send( SysEvent::Application( SysEvent::Application::CREATED ) );

		int ret = app->SubSystems()->Get< Platform >().ToPtr< WindowsPlatform >()->Loop();

		DEBUG_ONLY( Referenced::s_ChenckNotReleasedObjects() );

		return ret;
	}

	
}	// WinPlatform
}	// Engine

#endif	// PLATFORM_WINDOWS
