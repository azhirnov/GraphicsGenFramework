// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Renderer.h"
#include "../Shaders/IShader.h"
#include "../Generators/IGenerator.h"

namespace ShaderEditor
{

	//
	// Sample interface
	//

	class ISample : public BaseObject
	{
	// methods
	protected:
		ISample (const SubSystemsRef ss) : BaseObject( ss )
		{}


	// interface
	public:
		virtual void Init () = 0;		// load resources
		virtual void Release () = 0;	// release resources
		virtual void Reload () = 0;		// reload shaders, but save states

		virtual void Draw (const RenderTargetPtr &rt, Time<double> globalTime) = 0;
		virtual void Update (Time<double> dt) = 0;
	};

	SHARED_POINTER( ISample );


}	// ShaderEditor
