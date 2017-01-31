// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "ParticlesSample.h"

namespace ShaderEditor
{
	
/*
=================================================
	constructor
=================================================
*/
	ParticlesSample::ParticlesSample (const SubSystemsRef ss) :
		ISample( ss ),
		_controller( SubSystems() )
	{
	}
	
/*
=================================================
	Init
=================================================
*/
	void ParticlesSample::Init ()
	{
		Reload();

		_controller.Reset();

		_renderState = RenderState();
		_renderState.depth.test				= true;
		_renderState.depth.write			= true;
		//_renderState.depth.func				= ECompareFunc::LEqual;
		//_renderState.point.programPointSize	= true;
	}

/*
=================================================
	Release
=================================================
*/
	void ParticlesSample::Release ()
	{
		_emitter	= null;
	}

/*
=================================================
	Reload
=================================================
*/
	void ParticlesSample::Reload ()
	{
		ParticleEmitterPtr	new_emitter		= ParticleEmitter::New( SubSystems() );
		IGeneratorPtr		new_generator	= IGenerator::Create_SimpleParticles( SubSystems() );
		IShaderPtr			new_shader		= IShader::Create_SimpleParticles( SubSystems() );
		
		VertexAttribsDesc	desc;
		desc.Add( VertexAttribsDesc::AttribIndex(0), &ParticleVertex::prevPos );
		desc.Add( VertexAttribsDesc::AttribIndex(1), &ParticleVertex::position );
		desc.Add( VertexAttribsDesc::AttribIndex(2), &ParticleVertex::color, true );
		desc.Add( VertexAttribsDesc::AttribIndex(3), &ParticleVertex::size );

		const BytesU	vertex_size = BytesU(CompileTime::AlignToLarge< uint, sizeof(ParticleVertex), 4*4 >);

		if ( new_emitter->Create( new_generator, new_shader, _MaxParticles(), desc, vertex_size ) )
		{
			_emitter = new_emitter;
		}
	}
	
/*
=================================================
	Draw
=================================================
*/
	void ParticlesSample::Draw (const RenderTargetPtr &rt, TimeD globalTime)
	{
		rt->SetClearColor( ERenderTarget::Color0, float4(0.0f) );
		rt->Clear();

		real4x4 model	= _controller.GetModelMatrix() * real4x4::FromQuat( quat().RotationX( -90.0_deg ) );
		
		SubSystems()->Get<GraphicsEngine>()->GetStateManager()->SetRenderState( _renderState );

		_emitter->Update( globalTime );
		_emitter->Render( rt, model, _controller.GetViewMatrix(), _controller.GetProjMatrix() );
	}

/*
=================================================
	Update
=================================================
*/
	void ParticlesSample::Update (TimeD dt)
	{
		_controller.Update( dt );
	}
		
/*
=================================================
	Next
=================================================
*/
	bool ParticlesSample::Next ()
	{
		// no more shaders
		return false;
	}
	
/*
=================================================
	New
=================================================
*/
	ISamplePtr  ParticlesSample::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new ParticlesSample( ss ) );
	}


}	// ShaderEditor
