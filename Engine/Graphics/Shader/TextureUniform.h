// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "ShaderProgram.h"
#include "Engine/Graphics/Engine/GraphicsEngine.h"

namespace Engine
{
namespace Graphics
{

	//
	// Texture Uniform
	//

	struct TextureUniform
	{
	// variables
	private:
		UniformLocationID	_location;
		uint				_texUnit;
		bool				_isInteger;


	// methods
	public:
		explicit
		TextureUniform (bool isInteger = false);

		bool Create (const ShaderProgramPtr &program, StringCRef name);

		void Update (const TexturePtr &tex, uint unit);
	};
	

	
/*
=================================================
	constructor
=================================================
*/
	inline TextureUniform::TextureUniform (bool isInteger) :
		_texUnit(0), _isInteger(isInteger)
	{
	}
	
/*
=================================================
	Create
=================================================
*/
	inline bool TextureUniform::Create (const ShaderProgramPtr &program, StringCRef name)
	{
		CHECK_ERR( program->SubSystems()->Get< GraphicsEngine >()->GetContext()->CreateUniform(
						program->GetProgramID(), name, OUT _location ) );

		_texUnit = 0;
		return true;
	}


}	// Graphics
}	// Engine
