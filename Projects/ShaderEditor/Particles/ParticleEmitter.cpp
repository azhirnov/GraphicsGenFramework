// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "ParticleEmitter.h"

namespace ShaderEditor
{
	
/*
=================================================
	constructor
=================================================
*/
	ParticleEmitter::ParticleEmitter (const SubSystemsRef ss) :
		BaseObject( ss ),
		_particlesCount( 0 ),	_currBuffer( 0 ),	_initialize(false)
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	ParticleEmitter::~ParticleEmitter ()
	{
		Destroy();
	}
	
/*
=================================================
	Create
=================================================
*/
	bool ParticleEmitter::Create (const IGeneratorPtr &processor, const IShaderPtr &renderer, uint maxParticles,
								  const VertexAttribsDesc &attribs, BytesU vertexSize)
	{
		CHECK_ERR( processor and renderer );
		CHECK_ERR( maxParticles > 0 );

		Destroy();

		_particlesCount = maxParticles;
		
		FOR( i, _particleBuffers )
		{
			_particleBuffers[i] = VertexBuffer::New( SubSystems() );
			
			CHECK( _particleBuffers[i]->Create() );
			CHECK( _particleBuffers[i]->Allocate( BytesU(_particlesCount) * vertexSize, EBufferUsage::Dynamic ) );

			_particleBuffers[i]->SetAttribs( attribs, vertexSize );
			_particleBuffers[i]->SetPrimitive( EPrimitive::Point );
		}

		_particleProcessor = processor;
		CHECK_ERR( _particleProcessor->Compile() );

		_particleRenderer = renderer;
		CHECK_ERR( _particleRenderer->SetArg( "attribs", VariantRef::FromConst( VertexAttribsState(attribs) ) ) );

		_initialize = true;
		return true;
	}
	
/*
=================================================
	Destroy
=================================================
*/
	void ParticleEmitter::Destroy ()
	{
		_particleBuffers.Clear();

		_particleRenderer	= null;
		_particleProcessor	= null;
		_particlesCount		= 0;
		_currBuffer			= 0;
		_initialize			= false;
	}
	
/*
=================================================
	Update
=================================================
*/
	bool ParticleEmitter::Update (TimeD globalTime)
	{
		CHECK_ERR( _particleProcessor );

		uint	in_idx	= _currBuffer;
		uint	out_idx	= ++_currBuffer;

		_particleProcessor->SetArg( "inBuffer",		_particleBuffers[in_idx] );
		_particleProcessor->SetArg( "outBuffer",	_particleBuffers[out_idx] );
		_particleProcessor->SetArg( "globalTime",	globalTime );
		_particleProcessor->SetArg( "count",		_particlesCount );
		_particleProcessor->SetArg( "initialize",	_initialize );

		_particleProcessor->Render();

		_initialize = false;
		return true;
	}
	
/*
=================================================
	Render
=================================================
*/
	bool ParticleEmitter::Render (const RenderTargetPtr &rt, const real4x4 &modelMat,
								  const real4x4 &viewMat, const real4x4 &projMat)
	{
		CHECK_ERR( _particleRenderer );

		_particleRenderer->SetArg( "modelMat",		VariantRef::FromConst( modelMat ) );
		_particleRenderer->SetArg( "viewMat",		VariantRef::FromConst( viewMat ) );
		_particleRenderer->SetArg( "projMat",		VariantRef::FromConst( projMat ) );
		_particleRenderer->SetArg( "screenSize",	VariantRef::FromConst( rt->GetViewport().Size() ) );

		_particleRenderer->Active();

		_particleBuffers[ _currBuffer ]->Draw();

		return true;
	}

/*
=================================================
	New
=================================================
*/
	ParticleEmitterPtr  ParticleEmitter::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new ParticleEmitter( ss ) );
	}


}	// ShaderEditor
