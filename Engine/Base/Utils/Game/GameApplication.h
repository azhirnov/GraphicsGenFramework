// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.
/*
	This is helper class for game applications
*/

#pragma once

#include "Engine/Base/OS/OS.h"

namespace Engine
{
namespace Base
{
namespace GameUtils
{

	//
	// Game Application
	//

	class GameApplication : public BaseApplication
	{
	// types
	private:
		SHARED_POINTER( GameApplication );


	// variables
	private:
		//DEBUG_ONLY(
			TimeProfilerD	__fpsTimer;
			uint			__frameCounter;
		//)
		
		bool				__entered		: 1;
		bool				__inited		: 1;
		bool				__subscribed	: 1;


	// methods
	private:
		void _UpdateFPS ()
		{
			//DEBUG_ONLY(
				++__frameCounter;

				if ( __fpsTimer.GetTimeDelta().Seconds() > 5.0 )
				{
					float fps = float( __frameCounter / __fpsTimer.GetTimeDelta().Seconds() );
					DEBUG_CONSOLE( (String("FPS: ") << String().FormatF( fps, StringFormatF().Fmt(0,1) )).cstr() );

					__frameCounter = 0;
					__fpsTimer.Start();
				}
			//)
		}

		void _Subscribe ()
		{
			if ( not __subscribed )
			{
				GetEventSystem()->Subscribe( GameApplicationPtr(this), &GameApplication::_OnUpdate );
				GetEventSystem()->Subscribe( GameApplicationPtr(this), &GameApplication::_OnAppEvent );
				GetEventSystem()->Subscribe( GameApplicationPtr(this), &GameApplication::_OnWindowEvent );
				__subscribed = true;
			}
		}

		void _Unsubscribe ()
		{
			if ( __subscribed )
			{
				__subscribed = false;
				GetEventSystem()->Unsubscribe( GameApplicationPtr(this), &GameApplication::_OnWindowEvent );
				GetEventSystem()->Unsubscribe( GameApplicationPtr(this), &GameApplication::_OnAppEvent );
				GetEventSystem()->Unsubscribe( GameApplicationPtr(this), &GameApplication::_OnUpdate );
			}
		}


	protected:
		explicit
		GameApplication (const IParallelThreadPtr &thread) :
			BaseApplication( thread ),
			__entered(false), __inited(false), __subscribed(false)
		{
			_Subscribe();

			//DEBUG_ONLY(
				__frameCounter = 0;
				__fpsTimer.Start();
			//)
		}


		~GameApplication ()
		{
			ASSERT( not __subscribed );
		}
	

		virtual void _Update (TimeD dt, bool forceRedraw) = 0;

		virtual void _OnInit () = 0;

		virtual void _OnSurfaceCreated () = 0;

		virtual void _OnExit () = 0;

		virtual void _OnResized () = 0;

		virtual void _OnVisibilityChanged (bool visible) {}


	private:
		void _OnUpdate (const SysEvent::Update &ev)
		{
			if ( ev.renderAvailable )
			{
				_UpdateFPS();

				SubSystems()->Get< Input >()->PreUpdate();

				_Update( ev.timeDelta, ev.forceRedraw );

				SubSystems()->Get< Input >()->PostUpdate();
			}
			else
			{
				OS::Thread::Sleep( TimeU::FromMilliSeconds( 10 ) );
			}
		}
	

		void _OnAppEvent (const SysEvent::Application &ev)
		{
			switch ( ev.type )
			{
				case SysEvent::Application::CREATED :
					DEBUG_CONSOLE( "Application Created" );
					break;
				
				case SysEvent::Application::DESTROY :
					DEBUG_CONSOLE( "Application Destroy" );
					MOBILE_ONLY(
						_OnExit();
						_Unsubscribe();
					)
					break;
				
				case SysEvent::Application::ENTER_BACKGROUND :
					DEBUG_CONSOLE( (String("Application Enter Background ") << (ev.completed ? "completed" : "")).cstr() );
					break;
				
				case SysEvent::Application::ENTER_FOREGROUND :
					DEBUG_CONSOLE( (String("Application Enter Foreground ") << (ev.completed ? "completed" : "")).cstr() );
					if ( ev.completed and not __inited ) {
						_OnInit();
						__inited = true;
					}
					break;
				
				case SysEvent::Application::SURFACE_CREATED :
					DEBUG_CONSOLE( "Application Surface Created" );
					if ( not __entered ) {
						_OnSurfaceCreated();
						__entered = true;
					}
					break;
				
				case SysEvent::Application::SURFACE_DESTROYED :
					DEBUG_CONSOLE( "Application Surface Destroyed" );
					_OnExit();
					_Unsubscribe();
					break;
				
				case SysEvent::Application::LOW_MEMORY :
					DEBUG_CONSOLE( "Application Low Memory" );
					_OnExit();
					_Unsubscribe();
					break;
			}
		}

		void _OnWindowEvent (const SysEvent::Window &ev)
		{
			switch ( ev.type )
			{
				case SysEvent::Window::RESIZED :
					_OnResized();
					return;
					
				case SysEvent::Window::SHOWN :
					_OnVisibilityChanged( true );
					return;

				case SysEvent::Window::HIDDEN :
					_OnVisibilityChanged( false );
					return;
			}
		}
	};

}	// GameUtils
}	// Base
}	// Engine