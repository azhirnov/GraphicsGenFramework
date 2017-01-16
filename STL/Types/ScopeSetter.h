// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../BaseTypes/Types.h"

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
		ScopeSetter (T &ref, const T &value = T()) : _ref( ref ), _value( value )
		{}

		~ScopeSetter ()
		{
			_ref = _value;
		}
	};


}	// GXTypes
}	// GX_STL
