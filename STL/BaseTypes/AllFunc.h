// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Types.h"
#include "../Math/MathFunc.h"
#include "../CompileTime/PlainOldDataType.h"

namespace GX_STL
{
namespace GXTypes
{

	//
	// Type Info
	//
	
	namespace TypeTraits {}

	//template <typename T>				struct TypeInfo;

	template <typename T>				constexpr T		MinValue (const T& v = T());
	template <typename T>				constexpr T		MaxValue (const T& v = T());
	template <typename T>				inline T		Infinity ();
	template <typename T>				inline T		NaN ();

	template <typename T>				inline bool		IsNaN (const T& x);
	template <typename T>				inline bool		IsInfinity (const T& x);
	template <typename T>				inline bool		IsFinite (const T& x);



	//
	// Type Cast
	//

	template <typename R, typename T>	inline const R & ReferenceCast (const T & t);
	template <typename R, typename T>	inline R & ReferenceCast (T & t);

	//template <typename R, typename T>	inline R LimitClampCast (const T &t);
	//template <typename R, typename T>	inline R LimitCheckCast (const T &t);



	//
	// Converters
	//

	template <typename T>				inline T *		AddressOf (T &tValue);
	template <typename T>				inline const T * const AddressOf (const T &tValue);
	
	template <typename T, usize I>		constexpr usize	CountOf (const T (&arr)[I]);



	//
	// Enum Compare functions
	//
	template <typename T1, typename T2>
	inline bool  EnumEq (const T1& left, const T2& right);

	template <typename T1, typename T2, typename T3>
	inline bool  EnumEqMask (const T1& left, const T2& right, const T3& mask);
	
	template <typename T1, typename T2>
	inline bool  EnumAnyEq (const T1& left, const T2& right);



	//
	// Memory Operations
	//

	template <typename T, usize I>	inline void ZeroMem (T (&tArr)[I]);
	template <typename T>			inline void ZeroMem (T &val);


	
}	// GXTypes
}	// GX_STL
