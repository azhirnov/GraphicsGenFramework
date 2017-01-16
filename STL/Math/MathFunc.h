// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../BaseTypes/Types.h"
#include "../Types/Hash.h"

namespace GX_STL
{
namespace GXMath
{

	//
	// Trigonometry
	//

	template <typename T>				inline T		Sin (const T& x);
	template <typename T>				inline T		Cos (const T& x);

	template <typename T>				inline T		ASin (const T& x);
	template <typename T>				inline T		ACos (const T& x);

	template <typename T>				inline T		SinH (const T& x);
	template <typename T>				inline T		CosH (const T& x);

	template <typename T>				inline T		ASinH (const T& x);
	template <typename T>				inline T		ACosH (const T& x);

	template <typename T>				inline T		Tan (const T& x);
	template <typename T>				inline T		CoTan (const T& x);
	
	template <typename T>				inline T		TanH (const T& x);
	template <typename T>				inline T		CoTanH (const T& x);

	template <typename T>				inline T		ATan (const T& y_over_x);
	template <typename T>				inline T		ATan (const T& y, const T& x);
	template <typename T>				inline T		ACoTan (const T& x);

	template <typename T>				inline T		ATanH (const T& x);
	template <typename T>				inline T		ACoTanH (const T& x);



	//
	// Copare operations
	//
	
	template <typename T>				inline bool		All (const T& x);
	template <typename T>				inline bool		Any (const T& x);
	//template <typename T>				inline bool		Most (const T& x);
	template <typename T>				inline bool		IsZero (const T& x);
	template <typename T>				inline bool		IsNotZero (const T& x);
	template <typename T>				inline bool		Equals (const T& a, const T& b);
	


	//
	// Operation with sign
	//

	template <typename T>				inline T		Abs (const T& x);
	template <typename T>				inline T		SetSign (const T &x, bool bSign = true);
	template <typename T>				inline T		CopySign (const T& from, const T& to);
	template <typename T>				inline T		Sign (const T& x);
	template <typename T>				inline T		SignOrZero (const T& x);



	//
	// Pow, Exp
	//

	template <typename T, typename B>	inline T		Pow (const T&x, const B& base);
	template <typename T>				inline T		Ln (const T& x);
	template <typename T>				inline T		Log (const T& x, const T& base);
	template <typename T>				inline T		Log2 (const T& x);
	template <typename T>				inline T		Log10 (const T& x);
	template <typename T>				inline T		Exp (const T& x);
	template <typename T>				inline T		Exp2 (const T& x);
	template <typename T>				inline T		Exp10 (const T& x);
	template <typename T>				inline uint		IntLog2 (const T &x);
	template <typename T>				inline T		CeilPowerOfTwo (const T& x);
	template <typename T>				inline T		FloorPowerOfTwo (const T& x);
	template <typename T>				inline T		NearPowerOfTwo (const T& x);
	template <typename T>				inline bool		IsPowerOfTwo (const T& x);
	template <typename T>				inline T		Square (const T& x);
	template <typename T>				inline T		Sqrt (const T& x);
	template <typename T>				inline T		SquareSign (const T& x);
	template <typename T>				inline T		InvSqrt (const T& x);



	//
	// Interpolation
	//

	template <typename T>				inline T		GetInterpolation (const T& x, const T& y, const T& interpolated);

	// TODO: rename
	template <typename T, typename B>	inline T		Mix (const T& x, const T& y, const B& a);
	template <typename T, typename B>	inline T		QMix (const T& x, const T& y, const T& z, const B& a);
	template <typename T, typename B>	inline T		BiMix (const T& v0, const T& v1, const T& v2, const T& v3, const B& a2);
	template <typename T, typename B>	inline T		BiMix (const T& v0, const T& v1, const T& v2, const T& v3, const B& a0, const B& a1);
	template <typename T, typename B>	inline T		TriMix (const T &v0, const T &v1, const T &v2, const T &v3, const T &v4, const T &v5, const T &v6, const T &v7, const B& a3);


	
	//
	// Other
	//

	template <typename T>				inline T		Floor (const T& x);
	template <typename T>				inline T		Ceil (const T& x);
	template <typename T>				inline T		Fract (const T& x);
	template <typename T>				inline T		Trunc (const T& x);
	template <typename T>				inline T		Round (const T& x);
	template <typename T>				inline T		RoundTo (const T& x, const T& base);
	template <typename T>				inline bool		IsOdd (const T& x);
	template <typename T>				inline bool		IsEven (const T& x);

	template <typename T>				inline T		SafeDiv (const T& left, const T& right, const T& defVal);

	template <typename T>				inline T		IntFactorial (const T& x);
	template <typename T>				inline T		IntSuperFactorial (const T& x);
	template <typename T>				inline T		IntHyperFactorial (const T& x);
	template <typename T>				inline T		IntDoubleFactorial (const T& x);
	template <typename T>				inline T		IntGammaFunction (const T& x);
	template <typename T>				inline T		IntLnGammaFunction (const T& x);

	template <typename T>				inline T		Factorial (const T& x);
	template <typename T>				inline T		Gamma (const T& x);
	template <typename T>				inline T		LnGamma (const T& x);

	template <typename T>				inline T		Step (const T& edge, const T& x);
	template <typename T>				inline T		SmoothStep (const T& edge0, const T& edge1, const T& x);
	


	//
	// Bit Operators
	//

	template <typename T, typename B>	inline T		ToBit (const B bitIndex);
	template <typename T, typename B>	inline T		ToMask (const B bitIndex);
	template <typename T, typename B>	inline T		ToMask (const B first, const B last);
	template <typename T>				inline T		GetMaskForType (const T &type = T());

	template <typename T, typename B>	inline T		SafeLeftBitShift (const T& x, const B& shift);	// <<
	template <typename T, typename B>	inline T		SafeRightBitShift (const T& x, const B& shift);	// >>
	
	template <typename T, typename B>	inline T		BitRotateLeft (const T& x, const B& shift);
	template <typename T, typename B>	inline T		BitRotateRight (const T& x, const B& shift);

	template <typename T>				inline uint		BitScanReverse (const T& x);	// clz, findMSB
	template <typename T>				inline uint		BitScanForward (const T& x);	// ctz, findLSB


}	// GXMath
}	// GX_STL
