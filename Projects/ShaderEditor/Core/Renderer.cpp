// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Renderer.h"

namespace ShaderEditor
{
	
/*
=================================================
	constructor
=================================================
*/
	Renderer::Renderer (const SubSystemsRef ss) :
		BaseObject( ss )
	{
	}
	
/*
=================================================
	Initialize
=================================================
*/
	bool Renderer::Initialize ()
	{
		_quadMesh = MeshGenerator::CreateQuad( SubSystems(), RectF( -1.0f, -1.0f, 1.0f, 1.0f ), RectF( 0.0f, 0.0f, 1.0f, 1.0f ) );
		CHECK( _quadMesh.IsCreated() );

		CHECK_ERR( Reload() );
		return true;
	}
	
/*
=================================================
	Reload
=================================================
*/
	bool Renderer::Reload ()
	{
		_viewImageShader		= Uninitialized;
		_viewImageShader.prog	= New<ShaderProgram>( SubSystems() );

		CHECK( _viewImageShader.prog->Load(
			"gl/Shaders/textureView.glsl",
			ShaderProgram::ShaderBits_t().Set( EShader::Fragment ).Set( EShader::Vertex ),
			EShaderCompilationFlags::Default,
			_quadMesh.GetVertexAttribs()->GetAttribsState(),
			FragmentOutputState().Set( EFragOutput::Float4, ERenderTarget::Color0 )
		) );
		CHECK( _viewImageShader.prog->CreateUniformBlock( _viewImageShader.ub, "TextureViewUB" ) );
		CHECK( _viewImageShader.texture.Create( _viewImageShader.prog, "unTexture" ) );
		
		
		_viewImageShaderI		= Uninitialized;
		_viewImageShaderI.prog	= New<ShaderProgram>( SubSystems() );

		CHECK( _viewImageShaderI.prog->Load(
			"gl/Shaders/textureViewI.glsl",
			ShaderProgram::ShaderBits_t().Set( EShader::Fragment ).Set( EShader::Vertex ),
			EShaderCompilationFlags::Default,
			_quadMesh.GetVertexAttribs()->GetAttribsState(),
			FragmentOutputState().Set( EFragOutput::Float4, ERenderTarget::Color0 )
		) );
		CHECK( _viewImageShaderI.prog->CreateUniformBlock( _viewImageShaderI.ub, "TextureViewUB" ) );
		CHECK( _viewImageShaderI.texture.Create( _viewImageShaderI.prog, "unTexture" ) );
		
		
		_viewImageShaderU		= Uninitialized;
		_viewImageShaderU.prog	= New<ShaderProgram>( SubSystems() );

		CHECK( _viewImageShaderU.prog->Load(
			"gl/Shaders/textureViewU.glsl",
			ShaderProgram::ShaderBits_t().Set( EShader::Fragment ).Set( EShader::Vertex ),
			EShaderCompilationFlags::Default,
			_quadMesh.GetVertexAttribs()->GetAttribsState(),
			FragmentOutputState().Set( EFragOutput::Float4, ERenderTarget::Color0 )
		) );
		CHECK( _viewImageShaderU.prog->CreateUniformBlock( _viewImageShaderU.ub, "TextureViewUB" ) );
		CHECK( _viewImageShaderU.texture.Create( _viewImageShaderU.prog, "unTexture" ) );

		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void Renderer::Deinitialize ()
	{
		_quadMesh			= Uninitialized;
		_viewImageShader	= Uninitialized;
		_viewImageShaderI	= Uninitialized;
		_viewImageShaderU	= Uninitialized;
	}
	
/*
=================================================
	DrawImage
=================================================
*/
	void Renderer::DrawImage (const ScaleBiasController &controller, const ComputeImagePtr &image, EImageViewMode::type mode)
	{
		float2	size	= float2(SubSystems()->Get< GraphicsEngine >()->GetRenderTargetManager()->GetCurrent()->GetViewport().Size());
		float2	aspect	= ImageUtils::ResCorrectionAspect( size );

		return _DrawImage( image, controller.Scale() * aspect, controller.Bias() * aspect, mode );
	}
	
/*
=================================================
	DrawImage
=================================================
*/
	void Renderer::DrawImage (const ComputeImagePtr &image, const RectF &region, EImageViewMode::type mode)
	{
		return _DrawImage( image, region.Size() / 2.0f, region.Center(), mode );
	}
	
/*
=================================================
	_DrawImage
=================================================
*/
	void Renderer::_DrawImage (const ComputeImagePtr &image, const float2 &scale, const float2 &bias, EImageViewMode::type mode)
	{
		if ( EPixelFormat::ValueType::IsFloat( image->PixelFormat() ) )
		{
			_viewImageShader.texture.Update( image->GetSharedObject(), 0 );

			TextureViewerData&	ub_data = _viewImageShader.ub.Get();

			ub_data.scale			= scale;
			ub_data.bias			= bias;
			ub_data.valueScaleBias	= float2( 1.0f, 0.0f );
			ub_data.viewMode		= mode;

			_viewImageShader.prog->Bind();
			_quadMesh.Draw();
		}
		else
		if ( EPixelFormat::ValueType::IsSignedInteger( image->PixelFormat() ) )
		{
			_viewImageShaderI.texture.Update( image->GetSharedObject(), 0 );

			TextureViewerData&	ub_data = _viewImageShaderI.ub.Get();
			
			ub_data.scale			= scale;
			ub_data.bias			= bias;
			ub_data.valueScaleBias	= float2( 255.0f, 0.5f );
			ub_data.viewMode		= mode;

			_viewImageShaderI.prog->Bind();
			_quadMesh.Draw();
		}
		else
		if ( EPixelFormat::ValueType::IsUnsignedInteger( image->PixelFormat() ) )
		{
			_viewImageShaderU.texture.Update( image->GetSharedObject(), 0 );

			TextureViewerData&	ub_data = _viewImageShaderU.ub.Get();
			
			ub_data.scale			= scale;
			ub_data.bias			= bias;
			ub_data.valueScaleBias	= float2( 255.0f, 0.0f );
			ub_data.viewMode		= mode;

			_viewImageShaderU.prog->Bind();
			_quadMesh.Draw();
		}
		else
		{
			WARNING( "unknown texture format!" );
		}
	}


}	// ShaderEditor
