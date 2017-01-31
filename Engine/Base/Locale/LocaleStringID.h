// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/Common.h"

namespace Engine
{
namespace Base
{

	//
	// Locale String ID
	//

	struct LocaleStringID : public CompileTime::PODStruct
	{
	// types
	private:
		typedef LocaleStringID		Self;


	// variables
	private:
		uint	_id;


	// methods
	public:
		LocaleStringID (GX_DEFCTOR) : _id(-1)
		{}

		explicit
		LocaleStringID (uint id) : _id(id)
		{}

		_GX_DIM_CMP_OPERATORS_SELF( _id )
	};


}	// Base
}	// Engine
