// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Graphics/Types/Common.h"

namespace Engine
{
namespace Graphics
{

	class ShaderProgram;
	SHARED_POINTER( ShaderProgram );
	
	template <typename T>
	struct UniformBlock;


	//
	// Shader Program
	//

	class ShaderProgram : public Resource
	{
		template <typename T>
		friend struct UniformBlock;

	// types
	public:
		typedef Bitfield< EShader::_Count >		ShaderBits_t;

		typedef Event< void() >					OnBindProgram_t;


	// variables
	private:
		OnBindProgram_t	_onBindEvent;

		ProgramID		_program;

		uint			_numUniformBlocks;


	// methods
	protected:
		ShaderProgram (const SubSystemsRef ss);
		~ShaderProgram ();

		void _Destroy ();

	public:
		bool Compile (StringCRef source,
					  ShaderBits_t activeShaders,
					  EShaderCompilationFlags::type compilationFlags,
					  const VertexAttribsState &input,
					  const FragmentOutputState &output);
		
		bool Load (StringCRef filename,
					ShaderBits_t activeShaders,
					EShaderCompilationFlags::type compilationFlags,
					const VertexAttribsState &input,
					const FragmentOutputState &output);

		ProgramID const &	GetProgramID ()	const	{ return _program; }

		void Bind ();
		
		template <typename T>
		bool CreateUniformBlock (OUT UniformBlock<T> &ub, StringCRef name);

		static ShaderProgramPtr New (const SubSystemsRef ss);
		

	// Resource //
	public:
		bool SwapData (const ResourcePtr &other) override;

		bool IsValid () const override;


	// api specific methods
	public:
		static String FormatUniformBlockSource (StringCRef source);

	private:
		void _CreateUniformBuffer (OUT uint &location, OUT uint &size, StringCRef name);
	};


}	// Graphics
}	// Engine
