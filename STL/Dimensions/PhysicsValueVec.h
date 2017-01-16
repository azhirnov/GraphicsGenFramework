// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "PhysicsValue.h"


namespace GX_STL
{
namespace GXMath
{
	
	template <typename T, usize I>
	struct PhysicsValueVec;


#define I	1
#include "PhysicsValueVecI.h"
#undef	I


#define I	2
#include "PhysicsValueVecI.h"
#undef	I

	
#define I	3
#include "PhysicsValueVecI.h"
#undef	I


#define I	4
#include "PhysicsValueVecI.h"
#undef	I


	
	//
	// operator *
	//

	template <typename ValueType,
			  typename Dimensions,
			  typename ValueScale,
			  usize I
			 >
	inline PhysicsValueVec< PhysicsValue<ValueType, Dimensions, ValueScale>, I >
		operator * (const Vec<ValueType,I> &left, const PhysicsValue<ValueType, Dimensions, ValueScale> &right)
	{
		PhysicsValueVec< PhysicsValue<ValueType, Dimensions, ValueScale>, I >	ret;
		FOR( i, ret )	ret[i] = left[i] * right;
		return ret;
	}
	

	template <typename ValueType,
			  typename Dimensions,
			  typename ValueScale,
			  usize I
			 >
	inline PhysicsValueVec< PhysicsValue<ValueType, Dimensions, ValueScale>, I >
		operator * (const PhysicsValue<ValueType, Dimensions, ValueScale> &left, const Vec<ValueType,I> &right)
	{
		PhysicsValueVec< PhysicsValue<ValueType, Dimensions, ValueScale>, I >	ret;
		FOR( i, ret )	ret[i] = left * right[i];
		return ret;
	}
	


	//
	// operator /
	//

	template <typename ValueType,
			  typename Dimensions,
			  typename ValueScale,
			  usize I
			 >
	inline PhysicsValueVec< PhysicsValue<ValueType, Dimensions, ValueScale>, I >
		operator / (const Vec<ValueType,I> &left, const PhysicsValue<ValueType, Dimensions, ValueScale> &right)
	{
		PhysicsValueVec< PhysicsValue<ValueType, Dimensions, ValueScale>, I >	ret;
		FOR( i, ret )	ret[i] = left[i] / right;
		return ret;
	}
	

	template <typename ValueType,
			  typename Dimensions,
			  typename ValueScale,
			  usize I
			 >
	inline PhysicsValueVec< PhysicsValue<ValueType, Dimensions, ValueScale>, I >
		operator / (const PhysicsValue<ValueType, Dimensions, ValueScale> &left, const Vec<ValueType,I> &right)
	{
		PhysicsValueVec< PhysicsValue<ValueType, Dimensions, ValueScale>, I >	ret;
		FOR( i, ret )	ret[i] = left / right[i];
		return ret;
	}
	


	//
	// Equals
	//
	
	template <typename ValueType,
			  typename Dimensions,
			  typename ValueScale
			 >
	inline bool Equals (const PhysicsValue<ValueType, Dimensions, ValueScale>& a,
						const PhysicsValue<ValueType, Dimensions, ValueScale>& b)
	{
		return Equals( a.ref(), b.ref() );
	}

	
	template <typename ValueType,
			  typename Dimensions,
			  typename ValueScale,
			  usize I
			 >
	inline Vec<bool,I>  Equals (const PhysicsValueVec< PhysicsValue<ValueType, Dimensions, ValueScale>, I > &a,
								 const PhysicsValueVec< PhysicsValue<ValueType, Dimensions, ValueScale>, I > &b)
	{
		Vec<bool,I>	ret;
		FOR( i, ret )	ret[i] = Equals( a[i], b[i] );
		return ret;
	}

	
	template <typename ValueType,
			  typename Dimensions,
			  typename ValueScale
			 >
	inline bool Equals (const PhysicsValue<ValueType, Dimensions, ValueScale>& a,
						const PhysicsValue<ValueType, Dimensions, ValueScale>& b,
						/*Bits*/uint accuracy)
	{
		return Equals( a.ref(), b.ref(), accuracy );
	}

	
	template <typename ValueType,
			  typename Dimensions,
			  typename ValueScale,
			  usize I
			 >
	inline Vec<bool,I>  Equals (const PhysicsValueVec< PhysicsValue<ValueType, Dimensions, ValueScale>, I > &a,
								 const PhysicsValueVec< PhysicsValue<ValueType, Dimensions, ValueScale>, I > &b,
								 /*Bits*/uint accuracy)
	{
		Vec<bool,I>	ret;
		FOR( i, ret )	ret[i] = Equals( a[i], b[i], accuracy );
		return ret;
	}

}	// GXMath

namespace GXTypes
{
	
	template <typename ValueType,
			  typename Dimensions,
			  typename ValueScale,
			  usize I
			 >
	struct Hash< GXMath::PhysicsValueVec< GXMath::PhysicsValue< ValueType, Dimensions, ValueScale >, I > > :
		private Hash< GXMath::Vec< ValueType, I > >
	{
		typedef GXMath::PhysicsValueVec< GXMath::PhysicsValue< ValueType, Dimensions, ValueScale >, I >	key_t;
		typedef Hash< GXMath::Vec< ValueType, I > >														base_t;
		typedef typename base_t::result_t																result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x.ref() );
		}
	};
	
}	// GXTypes
}	// GX_STL
