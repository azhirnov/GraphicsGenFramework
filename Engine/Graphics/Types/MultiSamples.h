// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Enums.h"

namespace Engine
{
namespace Graphics
{

	//
	// Multi Samples
	//
	
	struct MultiSamples
	{
	// types
	private:
		typedef MultiSamples		Self;

	// variables
	private:
		ubyte		_value;

	// methods
	public:
		MultiSamples (GX_DEFCTOR) : _value(0)
		{}

		explicit
		MultiSamples (ubyte samples) : _value(samples)
		{}

		uint Get () const		{ return _value; }

		_GX_DIM_CMP_OPERATORS_SELF( _value )
	};


}	// Graphics
}	// Engine
