// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Enums.h"

namespace Engine
{
namespace Graphics
{

	//
	// Mipmap Level
	//
	
	struct MipmapLevel
	{
	// types
	private:
		typedef MipmapLevel		Self;

	// variables
	private:
		uint		_value;

	// methods
	public:
		MipmapLevel (GX_DEFCTOR) : _value(0)
		{}

		explicit
		MipmapLevel (uint value) : _value(value)
		{}

		uint Get () const		{ return _value; }

		_GX_DIM_CMP_OPERATORS_SELF( _value )
	};



	//
	// Mipmap Levels Range
	//
	
	struct MipmapLevelsRange
	{
	// types
	private:
		typedef MipmapLevelsRange		Self;

	// variables
	private:
		uint2		_value;

	// methods
	public:
		MipmapLevelsRange (GX_DEFCTOR) : _value(0)
		{}

		explicit
		MipmapLevelsRange (uint2 value) : _value(value)
		{}

		uint2 Get () const		{ return _value; }
	};


}	// Graphics
}	// Engine
