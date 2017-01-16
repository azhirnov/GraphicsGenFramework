// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Enums.h"

namespace Engine
{
namespace Graphics
{

	//
	// Texture Array Layer
	//
	
	struct TexArrLayer
	{
	// types
	private:
		typedef TexArrLayer		Self;

	// variables
	private:
		uint		_value;

	// methods
	public:
		TexArrLayer (GX_DEFCTOR) : _value(-1)
		{}

		explicit
		TexArrLayer (uint value) : _value(value)
		{}

		bool IsDefined ()	const	{ return _value != -1; }

		uint Get ()			const	{ return IsDefined() ? _value : 0; }

		_GX_DIM_CMP_OPERATORS_SELF( _value )
	};


}	// Graphics
}	// Engine
