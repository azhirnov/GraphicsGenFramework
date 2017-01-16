// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

// for VA args
#define AUXDEF_GET_FIRST( _1_, ... )			_1_
#define AUXDEF_GET_SECOND( _1_, _2_, ... )		_2_

// get values from macros
#define AUXDEF_GETRAW( _value_ )				_value_


// unite
#define AUXDEF_UNITE_RAW( _arg0_, _arg1_ )		AUXDEF_GETRAW( _arg0_ ) ## AUXDEF_GETRAW( _arg1_ )
#define AUXDEF_UNITE( _arg0_, _arg1_ )			_arg0_ ## _arg1_


// Call function for each argument in variable template parameters
namespace GX_STL
{
	namespace _defines_hidden_
	{
		struct __ExpandType
		{
			template <typename... T>
			__ExpandType (T&&...)
			{}
		};
	}
}

#define GX_FOREACH_ARG( _pattern_ ) ::GX_STL::_defines_hidden_::__ExpandType{ 0, ((_pattern_), 0)... }
