// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Containers/Buffer.h"
#include "../Math/Utils.h"

namespace GX_STL
{
namespace GXTypes
{
	
/*
=================================================
	AlignedCopy
=================================================
*/
	template <typename T>
	inline usize AlignedCopy (Buffer<T> dst, Buffer<const T> src, const GXMath::uint2 &dim, uint uAlign)
	{
		usize	u_aligned = GXMath::Align( dim.x, uAlign );
	
		if ( dst.Size() < u_aligned * dim.y )
			return -1;
	
		usize		u_ret = 0;
		const T *	p_src = src.ptr();

		for (uint i = 0; i < dim.y; ++i)
		{
			MemCopy( MovePointer( dst.ptr(), u_ret ), p_src, dim.x );
			
			u_ret += u_aligned;
			p_src  = MovePointer( p_src, dim.x );
		}
		return u_ret;
	}
	
/*
=================================================
	MemCopy
=================================================
*/
	template <typename T0, typename T1>
	inline void MemCopy (Buffer<T0> dst, Buffer<const T1> src)
	{
		ASSERT( dst.Size() == src.Size() );

		const usize	size = GXMath::Min( dst.Size(), src.Size() );

		MemCopy( dst.ptr(), src.ptr(), size );
	}
	
/*
=================================================
	MovePointer
=================================================
*/
	template <typename R, typename T>
	inline typename _types_hidden_::_PtrMove< T, R >::result_t  MovePointer (Buffer<T> arr, isize offset)
	{
		ASSERT( offset < arr.Size() );
		return _types_hidden_::_PtrMove< T, R >::Move( arr.ptr(), offset );
	}


}	// GXTypes
}	// GX_STL
