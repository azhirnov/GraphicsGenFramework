// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/OS/Shared/Application.h"

#if defined( PLATFORM_SDL )

#include "SDLFileManager.h"
#include "SDLPlatform.h"

#include "Engine/Base/Input/Input.h"

namespace Engine
{
namespace SdlPlatform
{
	using namespace Base;


	//
	// SDL Base Application
	//

	class SDLApplication : public Application
	{
	// variables
	private:
		SDLFileManager		_filesystem;
		SDLPlatform			_platform;
		Input				_input;


	// methods
	public:
		explicit
		SDLApplication (const IParallelThreadPtr &mainThread) :
			Application( mainThread ),
			_filesystem( SubSystems() ),
			_platform( mainThread ),
			_input( SubSystems() )
		{
		}

		~SDLApplication ()
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


}	// SdlPlatform
}	// Engine

#endif	// PLATFORM_SDL
