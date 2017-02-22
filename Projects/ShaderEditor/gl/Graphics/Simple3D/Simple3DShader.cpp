// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Projects/ShaderEditor/Core/IShader.h"

namespace ShaderEditor
{

	class Simple3DShader : public IShader
	{
	// types
	private:
		struct ShaderData
		{
			real4x4		mvp;
			real3		lightDir;

			ShaderData ()
			{
				mvp			= real4x4::Identity();
				lightDir	= -real3( 1.0f, 0.0f, 1.0f ).Normalized();
			}
		};

		typedef UniformBlock< ShaderData >		ShaderUB;


	// variables
	private:
		ShaderProgramPtr	_program;
		ShaderUB			_shaderUB;
		TextureUniform		_unDiffuseTex;
		TexturePtr			_diffuseTex;


	// methods
	public:
		Simple3DShader (const SubSystemsRef ss) : IShader( ss )
		{}

		void Active () override
		{
			_unDiffuseTex.Update( _diffuseTex, 0 );

			_program->Bind();
		}

		bool SetTextures (Buffer<const TexturePtr> textures) override
		{
			CHECK_ERR( textures.Count() == 1 );

			_diffuseTex = textures[0];
			return true;
		}

		bool SetArg (StringCRef name, VariantRef data) override
		{
			if ( name == "mvp" )
			{
				CHECK_ERR( data.IsType< real4x4 >() );

				_shaderUB.Get().mvp = data.Get< real4x4 >();
				return true;
			}

			return false;
			//RETURN_ERR( "unknown shader variable name!" );
		}

		bool Load (StringCRef filename)
		{
			_program = New<ShaderProgram>( SubSystems() );

			CHECK_ERR( _program->Load(
				filename,
				ShaderProgram::ShaderBits_t().Set( EShader::Vertex ).Set( EShader::Fragment ),
				EShaderCompilationFlags::Default,
				VertexAttribsState()
					.Add( VertexAttribsState::AttribIndex(0), EAttribute::Float3 )	// position
					.Add( VertexAttribsState::AttribIndex(1), EAttribute::Float3 )	// normal
					.Add( VertexAttribsState::AttribIndex(2), EAttribute::Float2 ),	// texture coord
				FragmentOutputState().Set( EFragOutput::Float4, 0 )					// forward rendering
			) );
			
			CHECK_ERR( _program->CreateUniformBlock( OUT _shaderUB, "ShaderUB" ) );

			CHECK_ERR( _unDiffuseTex.Create( _program, "unDiffuseMap" ) );

			return true;
		}
	};
	
/*
=================================================
	Create_Simple3D
=================================================
*/
	IShaderPtr  IShader::Create_Simple3D (const SubSystemsRef ss)
	{
		auto	ptr = New<Simple3DShader>( ss );

		CHECK_ERR( ptr->Load( "gl/Graphics/Simple3D/shader.glsl" ) );
		return ptr;
	}

}	// ShaderEditor
