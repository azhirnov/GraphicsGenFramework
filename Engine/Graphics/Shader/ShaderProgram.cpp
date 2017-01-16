// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "ShaderProgram.h"
#include "Engine/Graphics/Engine/GraphicsEngine.h"

namespace Engine
{
namespace Graphics
{
	
/*
=================================================
	constructor
=================================================
*/
	ShaderProgram::ShaderProgram (const SubSystemsRef ss) :
		Resource( PackFileID(), EResource::ShaderProgram, ss ), _numUniformBlocks(0)
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	ShaderProgram::~ShaderProgram ()
	{
		_Destroy();
	}
	
/*
=================================================
	_Destroy
=================================================
*/
	void ShaderProgram::_Destroy ()
	{
		SubSystems()->Get< GraphicsEngine >()->GetContext()->DeleteProgram( _program );

		_onBindEvent.Clear();
		_numUniformBlocks = 0;

		_SetLoadState( ELoadState::None );
	}

/*
=================================================
	Compile
=================================================
*/
	bool ShaderProgram::Compile (StringCRef source,
								 ShaderBits_t activeShaders,
								 EShaderCompilationFlags::type compilationFlags,
								 const VertexAttribsState &input,
								 const FragmentOutputState &output)
	{
		_Destroy();

		CHECK_ERR( SubSystems()->Get< GraphicsEngine >()->GetShaderManager()->
			CompileProgram( OUT _program, source, Uninitialized(), activeShaders, compilationFlags, input, output ) );
		
		_SetLoadState( ELoadState::Loaded );
		return true;
	}
	
/*
=================================================
	Load
=================================================
*/
	bool ShaderProgram::Load (StringCRef filename,
							  ShaderBits_t activeShaders,
							  EShaderCompilationFlags::type compilationFlags,
							  const VertexAttribsState &input,
							  const FragmentOutputState &output)
	{
		_Destroy();
		
		CHECK_ERR( SubSystems()->Get< GraphicsEngine >()->GetShaderManager()->
			LoadProgram( OUT _program, filename, activeShaders, compilationFlags, input, output ) );

		SetDebugName( filename );
		_SetLoadState( ELoadState::Loaded );

		return true;
	}

/*
=================================================
	Bind
=================================================
*/
	void ShaderProgram::Bind ()
	{
		SubSystems()->Get< GraphicsEngine >()->GetStateManager()->BindProgram( _program );

		_onBindEvent();
	}

/*
=================================================
	New
=================================================
*/
	ShaderProgramPtr ShaderProgram::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new ShaderProgram( ss ) );
	}
	
/*
=================================================
	SwapData
=================================================
*/
	bool ShaderProgram::SwapData (const ResourcePtr &other)
	{
		CHECK_ERR( Resource::SwapData( other ) );

		ShaderProgram *	prog = other.ToPtr<ShaderProgram>();

		SwapMembers( this, prog, &ShaderProgram::_onBindEvent );
		SwapMembers( this, prog, &ShaderProgram::_program );
		SwapMembers( this, prog, &ShaderProgram::_numUniformBlocks );

		return true;
	}
	
/*
=================================================
	IsValid
=================================================
*/
	bool ShaderProgram::IsValid () const
	{
		return IsResourceLoaded() and _program.IsValid();
	}


}	// Graphics
}	// Engine
