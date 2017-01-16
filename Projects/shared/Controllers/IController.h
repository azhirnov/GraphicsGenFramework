// Copyright © 2014-2015  Zhirnov Andrey. All rights reserved.

#pragma once

namespace Shared
{
	using namespace Engine;
	using namespace Engine::Base;


	//
	// Controller interface
	//

	class IController : public BaseObject
	{
	// methods
	protected:
		IController (const SubSystemsRef ss) : BaseObject( ss )
		{}


	// interface
	public:
		virtual void Update (Time<double> dt) = 0;
	};

	SHARED_POINTER( IController );


}	// Shared
