// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Parallel/ThreadManager.h"
#include "Engine/Base/Serializing/SerializableObjectFactory.h"
#include "Engine/Base/Resources/ResourceManager.h"
#include "Engine/Base/Events/EventSystem.h"
#include "VideoSettings.h"

namespace Engine
{
namespace Base
{

	//
	// Base Application
	//

	class Application : public VirtualThread
	{
	// variables
	private:
		ThreadManager				_threadManager;
		SerializableObjectFactory	_objFactory;
		ResourceManager				_resManager;
		EventSystemPtr				_eventSys;


	// methods
	public:
		Application (const IParallelThreadPtr &thread) :
			VirtualThread( thread ),
			_threadManager( SubSystems() ),
			_objFactory( SubSystems() ),
			_resManager( SubSystems() )
		{
			SubSystems()->GetSetter< Application >().Set( this );

			_threadManager.AddThread( this );

			_eventSys = EventSystem::New( SubSystems() );
		}


		~Application ()
		{
			_eventSys->Clear();

			SubSystems()->GetSetter< Application >().Set( null );
		}


		EventSystemPtr const & GetEventSystem ()
		{
			return _eventSys;
		}


		template <typename AppType>
		static void EntryPoint (IParallelThread *thread, OUT Application *&app)
		{
			app = new AppType( thread );
		}

		static void CreateInstance (void (*) (Base::IParallelThread *thread, OUT Base::Application *&));


	// interface
	public:
		virtual void Destroy () = 0;
	};


}	// Base
}	// Engine