// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Projects/ShaderEditor/Core/ISample.h"
#include "Projects/ShaderEditor/Particles/ParticleEmitter.h"

namespace ShaderEditor
{
	
	//
	// Particles Sample
	//

	class ParticlesSample : public ISample
	{
	// types
	private:
		typedef Shared::FreeCameraController	CameraController_t;
		
		struct ParticleVertex
		{
			float3	prevPos;
			float	size;
			float3	position;
			float	time;
			float3	velocity;
			color4u	color;
		};


	// variables
	private:
		CameraController_t		_controller;
		
		RenderState				_renderState;

		ParticleEmitterPtr		_emitter;


	// methods
	private:
		ParticlesSample (const SubSystemsRef ss);

		static const uint	_MaxParticles ()	{ return 1u << 24; }

	public:
		void Init () override;
		void Release () override;
		void Reload () override;

		void Draw (const RenderTargetPtr &rt, TimeD globalTime) override;
		void Update (TimeD dt) override;
		
		bool Next () override;

		static ISamplePtr  New (const SubSystemsRef ss);
	};


}	// ShaderEditor
