// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Matrix2.h"
#include "Matrix3.h"
#include "Matrix4.h"

#undef MAT_BIN_OPERATOR_SCALAR
#undef MAT_BIN_OPERATOR_MAT
#undef MAT_BIN_OPERATOR

namespace GX_STL
{
namespace GXTypes
{
	
	template <typename T, usize C, usize R>
	struct Hash< GXMath::Matrix<T,C,R> > :
		private Hash< Buffer<const T> >
	{
		typedef GXMath::Matrix<T,C,R>		key_t;
		typedef Hash< Buffer<const T> >		base_t;
		typedef typename base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( base_t::key_t( x.ptr(), x.Count() ) );
		}
	};

}	// GXTypes
}	// GX_STL
