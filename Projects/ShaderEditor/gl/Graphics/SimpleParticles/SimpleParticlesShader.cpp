// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Projects/ShaderEditor/Core/IShader.h"

namespace ShaderEditor
{

	class SimpleParticlesShader : public IShader
	{
	// types
	private:
		struct ShaderData
		{
			real4x4		proj;
			real4x4		mv;
			real2		screenSize;

			ShaderData ()
			{
				proj		= real4x4::Identity();
				mv			= real4x4::Identity();
				screenSize	= real2();
			}
		};

		typedef UniformBlock< ShaderData >		ShaderUB;


	// variables
	private:
		ShaderProgramPtr	_program;
		ShaderUB			_shaderUB;

		real4x4				_modelMat;
		real4x4				_viewMat;
		real4x4				_projMat;
		uint2				_screenSize;


	// methods
	public:
		SimpleParticlesShader (const SubSystemsRef ss) : IShader( ss )
		{}

		void Active () override
		{
			_shaderUB.Get().proj		= _projMat;
			_shaderUB.Get().mv			= _viewMat * _modelMat;
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
			//StringCRef	filename = "gl/Graphics/SimpleParticles/dots.glsl";
			StringCRef	filename = "gl/Graphics/SimpleParticles/rays.glsl";

			_program = ShaderProgram::New( SubSystems() );

			CHECK_ERR( _program->Load(
				filename,
				ShaderProgram::ShaderBits_t().Set( EShader::Vertex ).Set( EShader::Fragment ).Set( EShader::Geometry ),
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
		return BaseObject::_New( new SimpleParticlesShader( ss ) );
	}

}	// ShaderEditor
