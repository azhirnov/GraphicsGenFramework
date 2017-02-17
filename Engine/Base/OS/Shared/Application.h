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

	class Application;
	SHARED_POINTER( Application );



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
	protected:
		Application (const IParallelThreadPtr &thread) :
			VirtualThread( thread ),
			_threadManager( SubSystems() ),
			_objFactory( SubSystems() ),
			_resManager( SubSystems() )
		{
			SubSystems()->GetSetter< Application >().Set( this );

			_threadManager.AddThread( this );

			_eventSys = New<EventSystem>( SubSystems() );
		}


		~Application ()
		{
			_eventSys->Clear();

			SubSystems()->GetSetter< Application >().Set( null );
			
			Logger::GetInstance()->Close();
		}

		
	public:
		EventSystemPtr const & GetEventSystem ()
		{
			return _eventSys;
		}


		template <typename App>
		static ApplicationPtr EntryPoint (const IParallelThreadPtr &thread)
		{
			return New<App>( thread );
		}


		virtual void Destroy () = 0;
	};


}	// Base
}	// Engine