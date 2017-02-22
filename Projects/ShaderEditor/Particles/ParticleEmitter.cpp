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
		_particlesCount( 0 ),	_initialize(false)
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
		_particleBuffer = New<VertexBuffer>( SubSystems() );
			
		CHECK( _particleBuffer->Create() );
		CHECK( _particleBuffer->Allocate( BytesU(_particlesCount) * vertexSize, EBufferUsage::Dynamic ) );

		_particleBuffer->SetAttribs( attribs, vertexSize );
		_particleBuffer->SetPrimitive( EPrimitive::Point );

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
		_particleBuffer		= null;
		_particleRenderer	= null;
		_particleProcessor	= null;
		_particlesCount		= 0;
		_initialize			= false;
		_prevGlobalTime		= Uninitialized;
	}
	
/*
=================================================
	Update
=================================================
*/
	bool ParticleEmitter::Update (TimeD globalTime)
	{
		CHECK_ERR( _particleProcessor );
		
		TimeD	dt = globalTime - _prevGlobalTime;

		_prevGlobalTime = globalTime;

		if ( dt.IsZero() and not _initialize )
			return true;

		_particleProcessor->SetArg( "buffer",		_particleBuffer );
		_particleProcessor->SetArg( "globalTime",	globalTime );
		_particleProcessor->SetArg( "timeDelta",	dt );
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

		_particleBuffer->Draw();
		return true;
	}


}	// ShaderEditor
