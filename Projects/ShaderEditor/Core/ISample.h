// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Renderer.h"
#include "IShader.h"
#include "IGenerator.h"

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
		// load resources
		virtual void Init () = 0;

		// release resources
		virtual void Release () = 0;

		// reload shaders, but save states
		virtual void Reload () = 0;

		virtual void Draw (const RenderTargetPtr &rt, TimeD globalTime) = 0;
		virtual void Update (TimeD dt) = 0;

		// switch to next shader and return true,
		// if no more shaders then reset counter and return false
		virtual bool Next () = 0;
	};

	SHARED_POINTER( ISample );


}	// ShaderEditor
