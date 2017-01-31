// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Engine/Base/Engine.Base.h"

using namespace Engine;
using namespace Engine::Base;


class TestApplication : public GameUtils::GameApplication
{
// types
private:
	SHARED_POINTER( TestApplication );


// methods
public:
	TestApplication (const IParallelThreadPtr &thread) :
		GameApplication( thread )
	{
	}


	~TestApplication ()
	{
	}


	// GameApplication //
	void _Update (TimeD dt, bool forceRedraw)
	{
	}

	void _OnInit ()
	{
		CPUTIME_PROFILER("");
		CPUTIME_PROFILER( "OnInit" );
		CPUTIME_PROFILER( "OnInit2", 0.001 );

		SubSystems()->Get< Platform >()->
			InitWindow( Platform::WindowDesc( "Test", uint2(800, 600), MinValue<int2>(), false, true ) );

		SubSystems()->Get< Platform >()->
			InitRender( VideoSettings() );
	}

	void _OnSurfaceCreated ()
	{
	}

	void _OnExit ()
	{
	}

	void _OnResized ()
	{
	}
};

MAIN_APPLICATION_ENTRY( TestApplication );
