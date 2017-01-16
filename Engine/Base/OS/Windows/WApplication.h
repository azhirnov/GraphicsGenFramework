// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/OS/Shared/Application.h"

#if defined( PLATFORM_WINDOWS ) and not defined( PLATFORM_SDL )

#include "WFileManager.h"
#include "WPlatform.h"

#include "Engine/Base/Input/Input.h"


namespace Engine
{
namespace WinPlatform
{
	using namespace Base;


	//
	// Windows Base Application
	//

	class WindowsApplication : public Application
	{
	// variables
	private:
		WindowsFileManager		_filesystem;
		WindowsPlatform			_platform;
		Input					_input;


	// methods
	public:
		explicit
		WindowsApplication (const IParallelThreadPtr &mainThread) :
			Application( mainThread ),
			_filesystem( SubSystems() ),
			_platform( mainThread ),
			_input( SubSystems() )
		{
		}

		~WindowsApplication ()
		{
		}
		

	// Application interface
	private:
		virtual void Destroy ()	override
		{
			_platform.Exit();

			GetEventSystem()->Send( SysEvent::Application( SysEvent::Application::DESTROY ) );

			delete this;
		}
	};


}	// WinPlatform
}	// Engine

#endif	// PLATFORM_WINDOWS