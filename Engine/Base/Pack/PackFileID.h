// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/BaseObject.h"

namespace Engine
{
namespace Base
{

	//
	// Pack File ID
	//

	struct PackFileID : public CompileTime::PODStruct
	{
	// types
	private:
		typedef PackFileID		Self;


	// variables
	private:
		uint	_id;


	// methods
	public:
		PackFileID (GX_DEFCTOR) :
			_id(-1)
		{}

		explicit
		PackFileID (uint id) :
			_id(id)
		{}
			
		_GX_DIM_CMP_OPERATORS_SELF( _id )
	};

	
}	// Base
}	// Engine
