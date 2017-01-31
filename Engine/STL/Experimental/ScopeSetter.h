// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/STL/Common/Types.h"

namespace GX_STL
{
namespace GXTypes
{

	//
	// Scope Setter
	//

	template <typename T>
	struct ScopeSetter
	{
	// variables
	private:
		T &			_ref;
		const T		_value;


	// methods
	public:
		explicit
		ScopeSetter (OUT T &ref, const T &value = T()) : _ref( ref ), _value( value )
		{}

		~ScopeSetter ()
		{
			_ref = _value;
		}
	};


}	// GXTypes
}	// GX_STL
