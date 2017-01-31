// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/STL/Engine.STL.h"
#include "Engine/Config/Engine.Config.h"
#include "Engine/Config/Engine.Version.h"


namespace Engine
{
	using namespace GX_STL;
	using namespace GX_STL::GXTypes;
	using namespace GX_STL::GXMath;


	namespace Base
	{
		class Application;
		class Platform;

		class ThreadManager;
		class ResourceManager;
		class FileManager;
		class TimerManager;
		class LocaleManager;
		class Input;

		class SerializableObjectFactory;
	}

	namespace Graphics
	{
		class GraphicsEngine;
	}

	namespace Compute
	{
		class ComputeEngine;
	}

	namespace Audio
	{
		class AudioEngine;
	}

	namespace UI
	{
		class UserInterface;
	}

	namespace Scene
	{
		class SceneRenderer;
		class ScenePhysics;
		class SceneManager;
	}

	namespace Profiler
	{
	}

}	// Engine
