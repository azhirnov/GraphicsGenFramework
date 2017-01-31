// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "ParticleEmitter.h"

namespace ShaderEditor
{

	//
	// Particle Renderer
	//

	class ParticleRenderer : public BaseObject
	{
	// types
	private:


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
