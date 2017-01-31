// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Trigonometry.h"

namespace GX_STL
{
namespace GXMath
{
	

	//
	// Safe Left Bit Shift
	//
	
	template <typename T>
	inline T  SafeLeftBitShift (const T& x, BitsU shift)
	{
		CompileTime::MustBeInteger<T>();

		ASSUME( shift >= 0 );

		return x << ( usize(shift) & (CompileTime::SizeOf<T>::bits - 1) );
	}
	
	template <typename T, usize I>
	inline Vec<T,I>  SafeLeftBitShift (const Vec<T,I> &x, const Vec<BitsU,I>& shift)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = SafeLeftBitShift( x[i], shift[i] );
		return ret;
	}
	
	template <typename T, usize I>
	inline Vec<T,I>  SafeLeftBitShift (const Vec<T,I> &x, BitsU shift)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = SafeLeftBitShift( x[i], shift );
		return ret;
	}
	


	//
	// Safe Right Bit Shift
	//

	template <typename T>
	inline T  SafeRightBitShift (const T& x, BitsU shift)
	{
		CompileTime::MustBeInteger<T>();

		ASSUME( shift >= 0 );

		return x >> ( usize(shift) & (CompileTime::SizeOf<T>::bits - 1) );
	}
	
	template <typename T, usize I>
	inline Vec<T,I>  SafeRightBitShift (const Vec<T,I> &x, const Vec<BitsU,I>& shift)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = SafeRightBitShift( x[i], shift[i] );
		return ret;
	}
	
	template <typename T, usize I>
	inline Vec<T,I>  SafeRightBitShift (const Vec<T,I> &x, BitsU shift)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = SafeLeftBitShift( x[i], shift );
		return ret;
	}
	


	//
	// Circular Bit Shift
	//

	namespace _math_hidden_
	{
		// from https://en.wikipedia.org/wiki/Circular_shift#Implementing_circular_shifts

		template <typename T>
		forceinline T _BitRotateLeft (T value, usize shift)
		{
			const usize	mask = (CompileTime::SizeOf<T>::bits - 1);

			shift &= mask;
			return (value << shift) | (value >> ( ~(shift-1) & mask ));
		}
		
		template <typename T>
		forceinline T _BitRotateRight (T value, usize shift)
		{
			const usize	mask = (CompileTime::SizeOf<T>::bits - 1);

			shift &= mask;
			return (value >> shift) | (value << ( ~(shift-1) & mask ));
		}
	}

	template <typename T>
	inline T  BitRotateLeft (const T& x, BitsU shift)
	{
		CompileTime::MustBeInteger<T>();
		
		ASSUME( shift >= 0 );

		typedef CompileTime::NearUInt::FromType<T>	Unsigned_t;

		return _math_hidden_::_BitRotateLeft( Unsigned_t(x), usize(shift) );
	}

	template <typename T, usize I>
	inline T  BitRotateLeft (const Vec<T,I> &x, const Vec<BitsU,I>& shift)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = BitRotateLeft( x[i], shift[i] );
		return ret;
	}

	template <typename T, usize I>
	inline T  BitRotateLeft (const Vec<T,I> &x, BitsU shift)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = BitRotateLeft( x[i], shift );
		return ret;
	}


	template <typename T>
	inline T  BitRotateRight (const T& x, BitsU shift)
	{
		CompileTime::MustBeInteger<T>();
		
		ASSUME( shift >= 0 );

		typedef CompileTime::NearUInt::FromType<T>	Unsigned_t;

		return _math_hidden_::_BitRotateRight( Unsigned_t(x), int(shift) );
	}

	template <typename T, usize I>
	inline T  BitRotateRight (const Vec<T,I> &x, const Vec<BitsU,I>& shift)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = BitRotateLeft( x[i], shift[i] );
		return ret;
	}

	template <typename T, usize I>
	inline T  BitRotateRight (const Vec<T,I> &x, BitsU shift)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = BitRotateLeft( x[i], shift );
		return ret;
	}

	

	//
	// To Bit
	//

	template <typename T>
	inline T  ToBit (BitsU bitIndex)
	{
		return SafeLeftBitShift( T(1), (usize)bitIndex );
	}

	template <typename T, usize I>
	inline Vec<T,I>  ToBit (const Vec<BitsU,I> &bitIndex)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ToBit<T>( bitIndex[i] );
		return ret;
	}

	
	//
	// To Mask
	//

	template <typename T>
	inline T  ToMask (BitsU lastBitIndex)
	{
		STATIC_ASSERT( CompileTime::IsInteger<T> /*and CompileTime::IsUnsigned<T>*/ );

		// Test:
		// (1 << lastBitIndex-1) & ToMask( lastBitIndex ) == (1 << lastBitIndex-1)

		return	lastBitIndex < 0 ? T(0) :
					lastBitIndex < CompileTime::SizeOf<T>::bits ?
						(T(1) << (usize)lastBitIndex) - 1 :
						T(-1);
	}

	template <typename T, usize I>
	inline Vec<T,I>  ToMask (const Vec<BitsU,I> &bitIndex)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ToMask<T>( bitIndex[i] );
		return ret;
	}


	template <typename T>
	inline T  ToMask (BitsU first, BitsU last)
	{
		ASSERT( first <= last );
		return ToMask<T>( first ) ^ ToMask<T>( last );
	}

	template <typename T, usize I>
	inline Vec<T,I>  ToMask (const Vec<BitsU,I> first, const Vec<BitsU,I> last)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ToMask<T>( first[i], last[i] );
		return ret;
	}
	

	
	//
	// Get Bit Mask For Type
	//

	template <typename T>
	inline T  GetMaskForType (const T &)
	{
		STATIC_ASSERT( CompileTime::IsInteger<T> /*and CompileTime::IsUnsigned<T>*/ );

		return T(-1);
	}
	
	template <typename T, usize I>
	inline Vec<T,I>  GetMaskForType (const Vec<T,I> &)
	{
		return Vec<T,I>( GetMaskForType<T>() );
	}
	

	
	//
	// BitScanReverse, BitScanForward
	//

	template <typename T>
	inline BitsU BitScanReverse (const T& x)
	{
		return BitsU( IntLog2( x ) );
	}


	template <typename T>
	inline BitsU BitScanForward (const T& x)
	{
		CompileTime::MustBeInteger<T>();

		typedef typename CompileTime::NearUInt::FromType<T>	utype;

		const utype	value = ReferenceCast<utype>(x);

		uint bits = _math_hidden_::_RecursiveBitScanForward< CompileTime::SizeOf<utype>::bits-1 >::Get( value );

		return BitsU( bits > 0 ? CompileTime::SizeOf<utype>::bits - bits : -1 );
	}

	template <typename T, usize I>
	inline Vec<BitsU,I> BitScanForward (const Vec<T,I> &x)
	{
		Vec<BitsU,I>		ret;
		FOR( i, x )		ret[i] = BitScanForward( x[i] );
		return ret;
	}


}	// GXMath
}	// GX_STL
