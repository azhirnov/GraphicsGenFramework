// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Projects/ShaderEditor/Core/IShader.h"

namespace ShaderEditor
{
	// 0 - dots
	// 1 - rays
	// 2 - point sprites
#	define PARTICLE_DRAW_MODE	1

	// 0 - none
	// 1 - additive
#	define BLEND_MODE	1


	class SimpleParticlesShader : public IShader
	{
	// types
	private:
		struct ShaderData
		{
			real4x4		proj;
			real4x4		mv;
			real4x4		mvp;
			real2		screenSize;

			ShaderData ()
			{
				proj		= real4x4::Identity();
				mv			= real4x4::Identity();
				mvp			= real4x4::Identity();
				screenSize	= real2();
			}
		};

		typedef UniformBlock< ShaderData >		ShaderUB;


	// variables
	private:
		ShaderProgramPtr	_program;
		ShaderUB			_shaderUB;

		RenderState			_renderState;

		real4x4				_modelMat;
		real4x4				_viewMat;
		real4x4				_projMat;
		uint2				_screenSize;


	// methods
	public:
		SimpleParticlesShader (const SubSystemsRef ss) : IShader( ss )
		{
			_renderState = RenderState();

			// blending
			#if BLEND_MODE == 0
				_renderState.depth.test					= true;
				_renderState.depth.write				= true;
				_renderState.depth.func					= ECompareFunc::LEqual;

			#elif BLEND_MODE == 1
				_renderState.colors[0].blend			= true;
				_renderState.colors[0].blendFuncSrcRGB	= _renderState.colors[0].blendFuncSrcA	= EBlendFunc::One;
				_renderState.colors[0].blendFuncDstRGB	= _renderState.colors[0].blendFuncDstA	= EBlendFunc::One;
				_renderState.colors[0].blendModeRGB		= _renderState.colors[0].blendModeA		= EBlendEq::Add;

			#endif


			// particle mode
			#if PARTICLE_DRAW_MODE == 0 or \
				PARTICLE_DRAW_MODE == 1
				// none

			#elif PARTICLE_DRAW_MODE == 2
				_renderState.point.programPointSize		= true;
				_renderState.point.spriteCoordOrigin	= EPointSpriteCoordOrigin::LowerLeft;
			#endif
		}

		void Active () override
		{
			SubSystems()->Get<GraphicsEngine>()->GetStateManager()->SetRenderState( _renderState );

			_shaderUB.Get().proj		= _projMat;
			_shaderUB.Get().mv			= _viewMat * _modelMat;
			_shaderUB.Get().mvp			= _projMat * _viewMat * _modelMat;
			_shaderUB.Get().screenSize	= _screenSize.To<real2>();

			_program->Bind();
		}

		bool SetTextures (Buffer<const TexturePtr> textures) override
		{
			CHECK_ERR( textures.Count() == 0 );

			return true;
		}

		bool SetArg (StringCRef name, VariantRef data) override
		{
			if ( name == "modelMat" and data.IsType<real4x4>() )
			{
				_modelMat = data.Get<real4x4>();
				return true;
			}
			
			if ( name == "viewMat" and data.IsType<real4x4>() )
			{
				_viewMat = data.Get<real4x4>();
				return true;
			}
			
			if ( name == "projMat" and data.IsType<real4x4>() )
			{
				_projMat = data.Get<real4x4>();
				return true;
			}

			if ( name == "screenSize" and data.IsType<uint2>() )
			{
				_screenSize = data.Get<uint2>();
				return true;
			}

			if ( name == "attribs" and data.IsType<VertexAttribsState>() and not _program )
			{
				CHECK_ERR( Load( data.Get<VertexAttribsState>() ) );
				return true;
			}

			RETURN_ERR( "unknown shader variable name!" );
		}

		bool Load (const VertexAttribsState &attribs)
		{
			#if PARTICLE_DRAW_MODE == 0
				StringCRef					filename		= "gl/Graphics/SimpleParticles/dots.glsl";
				ShaderProgram::ShaderBits_t	activeShaders	= ShaderProgram::ShaderBits_t().Set( EShader::Vertex ).Set( EShader::Fragment ).Set( EShader::Geometry );
			
			#elif PARTICLE_DRAW_MODE == 1
				StringCRef					filename		= "gl/Graphics/SimpleParticles/rays.glsl";
				ShaderProgram::ShaderBits_t	activeShaders	= ShaderProgram::ShaderBits_t().Set( EShader::Vertex ).Set( EShader::Fragment ).Set( EShader::Geometry );
			
			#elif PARTICLE_DRAW_MODE == 2
				StringCRef					filename		= "gl/Graphics/SimpleParticles/point_sprites.glsl";
				ShaderProgram::ShaderBits_t	activeShaders	= ShaderProgram::ShaderBits_t().Set( EShader::Vertex ).Set( EShader::Fragment );
			
			#endif

			_program = New<ShaderProgram>( SubSystems() );

			CHECK_ERR( _program->Load(
				filename,
				activeShaders,
				EShaderCompilationFlags::Default,
				attribs,
				FragmentOutputState().Set( EFragOutput::Float4, 0 )
			) );
			
			CHECK_ERR( _program->CreateUniformBlock( OUT _shaderUB, "ShaderUB" ) );

			return true;
		}
	};
	
/*
=================================================
	Create_SimpleParticles
=================================================
*/
	IShaderPtr  IShader::Create_SimpleParticles (const SubSystemsRef ss)
	{
		return New<SimpleParticlesShader>( ss );
	}

}	// ShaderEditor
