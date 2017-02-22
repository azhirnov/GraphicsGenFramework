// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Projects/ShaderEditor/Core/IShader.h"

namespace ShaderEditor
{

	class TessLandscapeShader : public IShader
	{
	// types
	private:
		struct ShaderData
		{
			real4x4		mvp;
			real4x4		model;
			//real3x4		normalMat;
			real3		lightDir;
			real		maxTessLevel;
			real		heightScale;
			real		detailLevel;
			real		scrAspect;

			ShaderData ()
			{
				mvp				= real4x4::Identity();
				model			= real4x4::Identity();
				//normalMat		= real3x4::Identity();
				lightDir		= real3( 0.0f, 0.0f, -1.0f ).Normalized();
				maxTessLevel	= 20.0f;
				heightScale		= 1.0f;
				detailLevel		= 100.0f;
				scrAspect		= 1.0f;
			}
		};

		typedef UniformBlock< ShaderData >		ShaderUB;


	// variables
	private:
		ShaderProgramPtr	_program;
		ShaderUB			_landscapeUB;
		TextureUniform		_unDiffuseMap;
		TextureUniform		_unDisplacementMap;
		TexturePtr			_diffuseMap;
		TexturePtr			_displacementMap;


	// methods
	public:
		TessLandscapeShader (const SubSystemsRef ss) : IShader( ss )
		{}

		void Active () override
		{
			_unDiffuseMap.Update( _diffuseMap, 0 );
			_unDisplacementMap.Update( _displacementMap, 1 );

			_program->Bind();
		}

		bool SetTextures (Buffer<const TexturePtr> textures) override
		{
			CHECK_ERR( textures.Count() == 2 );

			_diffuseMap			= textures[0];
			_displacementMap	= textures[1];
			return true;
		}

		bool SetArg (StringCRef name, VariantRef data) override
		{
			if ( name == "mvp" )
			{
				CHECK_ERR( data.IsType< real4x4 >() );

				_landscapeUB.Get().mvp = data.Get< real4x4 >();
				return true;
			}

			if ( name == "mv" )
			{
				return true;
			}

			if ( name == "model" )
			{
				CHECK_ERR( data.IsType< real4x4 >() );

				_landscapeUB.Get().model = data.Get< real4x4 >();
				return true;
			}

			if ( name == "normal" )
			{
				/*CHECK_ERR( data.IsType< real3x3 >() );

				_landscapeUB.Get().normalMat.Inject( data.Get< real3x3 >() );*/
				return true;
			}

			RETURN_ERR( "unknown shader variable name!" );
		}

		bool Load (StringCRef filename)
		{
			_program = New<ShaderProgram>( SubSystems() );

			CHECK_ERR( _program->Load(
				filename,
				ShaderProgram::ShaderBits_t().Set( EShader::Vertex ).Set( EShader::Fragment )
									.Set( EShader::TessControl ).Set( EShader::TessEvaluation ),
				EShaderCompilationFlags::Default,
				VertexAttribsState()
					.Add( VertexAttribsState::AttribIndex(0), EAttribute::Float3 )	// position
					.Add( VertexAttribsState::AttribIndex(1), EAttribute::Float3 )	// normal
					.Add( VertexAttribsState::AttribIndex(2), EAttribute::Float2 ),	// texture coord
				FragmentOutputState().Set( EFragOutput::Float4, 0 )					// forward rendering
			) );
			
			CHECK_ERR( _program->CreateUniformBlock( OUT _landscapeUB, "LandscapeUB" ) );

			CHECK_ERR( _unDiffuseMap.Create( _program, "unDiffuseMap" ) );
			CHECK_ERR( _unDisplacementMap.Create( _program, "unDisplacementMap" ) );

			return true;
		}
	};
	
/*
=================================================
	Create_TessLandscape
=================================================
*/
	IShaderPtr  IShader::Create_TessLandscape (const SubSystemsRef ss)
	{
		auto	ptr = New<TessLandscapeShader>( ss );

		CHECK_ERR( ptr->Load( "gl/Graphics/TessLandscape/shader.glsl" ) );
		return ptr;
	}

}	// ShaderEditor
