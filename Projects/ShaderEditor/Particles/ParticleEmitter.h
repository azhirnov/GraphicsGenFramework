// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Projects/ShaderEditor/Core/IGenerator.h"
#include "Projects/ShaderEditor/Core/IShader.h"

namespace ShaderEditor
{

	class ParticleEmitter;
	SHARED_POINTER( ParticleEmitter );



	//
	// Particle Emitter
	//

	class ParticleEmitter : public BaseObject
	{
	// types
	private:
		/*struct ParticleVertex
		{
			float3	prevPos;
			float3	position;
			float	size;
			float3	velocity;
			float	time;
			float2	param;	// reserved
			color4u	color;
		};*/

		typedef StaticArray< VertexBufferPtr, 2 >		MemBuffer2_t;


	// variables
	private:
		IGeneratorPtr		_particleProcessor;
		IShaderPtr			_particleRenderer;
		MemBuffer2_t		_particleBuffers;
		uint				_particlesCount;
		uint				_currBuffer : 1;
		bool				_initialize;


	// methods
	protected:
		explicit
		ParticleEmitter (const SubSystemsRef ss);
		~ParticleEmitter ();

	public:
		bool Create (const IGeneratorPtr &processor, const IShaderPtr &renderer, uint maxParticles,
					 const VertexAttribsDesc &attribs, BytesU vertexSize);

		void Destroy ();

		bool Render (const RenderTargetPtr &rt, const real4x4 &modelMat,
					 const real4x4 &viewMat, const real4x4 &projMat);

		bool Update (TimeD globalTime);

		static ParticleEmitterPtr  New (const SubSystemsRef ss);
	};


}	// ShaderEditor
