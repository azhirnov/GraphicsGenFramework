// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "ShaderProgram.h"
#include "Engine/Graphics/Engine/GraphicsEngine.h"

namespace Engine
{
namespace Graphics
{

	//
	// Image Uniform
	//


	struct ImageUniform
	{
	// variables
	private:
		UniformLocationID			_location;
		uint						_imgUnit;
		const EMemoryAccess::type	_access;


	// methods
	public:
		explicit
		ImageUniform (EMemoryAccess::type access);

		ImageUniform& operator = (const ImageUniform &other);

		bool Create (const ShaderProgramPtr &program, StringCRef name);

		void Update (const TexturePtr &tex, uint unit, MipmapLevel level = Uninitialized, TexArrLayer layer = Uninitialized);
	};
	

	
/*
=================================================
	constructor
=================================================
*/
	inline ImageUniform::ImageUniform (EMemoryAccess::type access) :
		_imgUnit(0), _access(access)
	{
	}
	
/*
=================================================
	operator =
=================================================
*/
	inline ImageUniform&  ImageUniform::operator = (const ImageUniform &other)
	{
		ASSERT( this->_access == other._access );

		this->_location	= other._location;
		this->_imgUnit	= other._imgUnit;

		return *this;
	}

/*
=================================================
	Create
=================================================
*/
	inline bool ImageUniform::Create (const ShaderProgramPtr &program, StringCRef name)
	{
		CHECK_ERR( program->SubSystems()->Get< GraphicsEngine >()->GetContext()->CreateUniform(
						program->GetProgramID(), name, OUT _location ) );

		_imgUnit = 0;
		return true;
	}


}	// Graphics
}	// Engine
