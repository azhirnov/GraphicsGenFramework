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
		typedef FreeCameraController	CameraController_t;
		
		struct ParticleVertex
		{
			float3	position;
			float	size;
			float3	velocity;
			color4u	color;
			float4	param;
		};


	// variables
	private:
		CameraController_t		_controller;

		ParticleEmitterPtr		_emitter;


	// methods
	private:
		static const uint	_MaxParticles ()	{ return 1u << 18; }

	public:
		explicit
		ParticlesSample (const SubSystemsRef ss);

		void Init () override;
		void Release () override;
		void Reload () override;

		void Draw (const RenderTargetPtr &rt, TimeD globalTime) override;
		void Update (TimeD dt) override;
		
		bool Next () override;
	};


}	// ShaderEditor
