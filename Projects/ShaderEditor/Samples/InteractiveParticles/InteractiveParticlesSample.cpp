// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "InteractiveParticlesSample.h"

namespace ShaderEditor
{
	
/*
=================================================
	constructor
=================================================
*/
	InteractiveParticlesSample::InteractiveParticlesSample (const SubSystemsRef ss) :
		ISample( ss ),
		_controller( SubSystems() )
	{
	}
	
/*
=================================================
	Init
=================================================
*/
	void InteractiveParticlesSample::Init ()
	{
		Reload();

		_controller.Reset(Transformation<real>(real3(0.0f, 0.0f, 10.0f), quat()));
	}

/*
=================================================
	Release
=================================================
*/
	void InteractiveParticlesSample::Release ()
	{
		_emitter	= null;
	}

/*
=================================================
	Reload
=================================================
*/
	void InteractiveParticlesSample::Reload ()
	{
		ParticleEmitterPtr	new_emitter		= New<ParticleEmitter>( SubSystems() );
		IGeneratorPtr		new_generator	= IGenerator::Create_InteractiveParticles( SubSystems() );
		IShaderPtr			new_shader		= IShader::Create_SimpleParticles( SubSystems() );
		
		VertexAttribsDesc	desc;
		desc.Add( VertexAttribsDesc::AttribIndex(0), &ParticleVertex::position );
		desc.Add( VertexAttribsDesc::AttribIndex(1), &ParticleVertex::velocity );
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
	void InteractiveParticlesSample::Draw (const RenderTargetPtr &rt, TimeD globalTime)
	{
		rt->SetClearColor( ERenderTarget::Color0, float4(0.0f) );
		rt->Clear();

		real4x4 model	= _controller.GetModelMatrix() * real4x4::FromQuat( quat().RotationX( -90.0_deg ) );
		real4x4	mvp		= _controller.GetProjMatrix() * _controller.GetViewMatrix() * model;
		real4x4	inv_mvp	= mvp.Inverse();

		float3	scr_pos	= float4x4::Project( float3(0.0f), mvp, rt->GetViewport().To<RectF>() );
		float3	wrl_pos	= float4x4::UnProject( float3( _mouse.xy(), scr_pos.z ), inv_mvp, rt->GetViewport().To<RectF>() );
		float4	mouse	= float4( wrl_pos, _mouse.z );


		_emitter->GetParticleProcessor()->SetArg( "mouse",	mouse );

		_emitter->Update( globalTime );

		_emitter->Render( rt, model, _controller.GetViewMatrix(), _controller.GetProjMatrix() );
	}

/*
=================================================
	Update
=================================================
*/
	void InteractiveParticlesSample::Update (TimeD dt)
	{
		_controller.Update( dt );
		
		Ptr< Input >	input	= SubSystems()->Get< Input >();
		const uint		id		= 1;

		if ( input->GetTouch(id).IsPressed() )
		{
			_mouse = float4( float2(input->GetTouch(id).pos), 1.0f, 0.0f );
		}
		else
			_mouse.z = 0.0f;
	}
		
/*
=================================================
	Next
=================================================
*/
	bool InteractiveParticlesSample::Next ()
	{
		// no more shaders
		return false;
	}


}	// ShaderEditor
