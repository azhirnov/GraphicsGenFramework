// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "GL4ComputeProgram.h"

#if defined( COMPUTE_API_OPENGL )

namespace Engine
{
namespace Compute
{
	
/*
=================================================
	constructor
=================================================
*/
	GL4ComputeProgram::GL4ComputeProgram (const SubSystemsRef ss) :
		BaseObject( ss ), _flags( EShaderCompilationFlags::Unknown )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	GL4ComputeProgram::~GL4ComputeProgram ()
	{
		_Delete();
	}
	
/*
=================================================
	Create
=================================================
*/
	bool GL4ComputeProgram::Create (StringCRef filename, EProgramUnitFlags::type compilationFlags)
	{
		_Delete();

		_flags = EShaderCompilationFlags::type(compilationFlags) | EShaderCompilationFlags::ComputeVariableGroupSize;

		CHECK_ERR( SubSystems()->Get< GraphicsEngine >()->GetShaderManager()->
			LoadShader( OUT _shaders, OUT _header, filename, EShader::Compute, _flags ) );

		return true;
	}
	
/*
=================================================
	_Delete
=================================================
*/
	void GL4ComputeProgram::_Delete ()
	{
		_shaders.Clear();
		_header.Clear();

		_flags = EShaderCompilationFlags::Unknown;
	}
	
/*
=================================================
	IsCreated
=================================================
*/
	bool GL4ComputeProgram::IsCreated () const
	{
		return not _shaders.Empty() and not _header.Empty();
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
