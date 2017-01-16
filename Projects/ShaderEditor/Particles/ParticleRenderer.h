// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Core/Common.h"

namespace ShaderEditor
{

	//
	// Particle Renderer
	//

	class ParticleRenderer : public BaseObject
	{
	// variables
	private:


	// methods
	public:
		ParticleRenderer (const SubSystemsRef ss);
		~ParticleRenderer ();

		bool Initialize ();
		void Deinitialize ();
		bool Reload ();
	};


}	// ShaderEditor
