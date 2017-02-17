// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Projects/ShaderEditor/Core/Common.h"

namespace ShaderEditor
{

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


}	// ShaderEditor
