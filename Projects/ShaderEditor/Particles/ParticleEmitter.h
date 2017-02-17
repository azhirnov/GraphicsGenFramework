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
		typedef StaticArray< VertexBufferPtr, 2 >		MemBuffer2_t;


	// variables
	private:
		IGeneratorPtr		_particleProcessor;
		IShaderPtr			_particleRenderer;

		TimeD				_prevGlobalTime;

		MemBuffer2_t		_particleBuffers;
		uint				_particlesCount;
		uint				_currBuffer : 1;
		bool				_initialize;


	// methods
	public:
		explicit
		ParticleEmitter (const SubSystemsRef ss);
		~ParticleEmitter ();

		bool Create (const IGeneratorPtr &processor, const IShaderPtr &renderer, uint maxParticles,
					 const VertexAttribsDesc &attribs, BytesU vertexSize);

		void Destroy ();

		bool Render (const RenderTargetPtr &rt, const real4x4 &modelMat,
					 const real4x4 &viewMat, const real4x4 &projMat);

		bool Update (TimeD globalTime);
	};


}	// ShaderEditor
