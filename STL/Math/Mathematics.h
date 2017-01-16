// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../CompileTime/Constants.h"
#include "../CompileTime/TypeListHelpers.h"
#include "../Memory/MemFunc.h"
#include "FastMath.h"
#include "MathTypes.h"

namespace GX_STL
{
namespace GXMath
{

//-------------------------------------------------------------------
//	Trigonometry


	//
	// To Near Supported float types
	//

	namespace _math_hidden_
	{
		template <bool b64>		struct _ToNearFloat			{ typedef float  type; };
		template <>				struct _ToNearFloat<true>	{ typedef double type; };

		template <typename T>
		using ToNearFloat = typename _ToNearFloat< sizeof(T) >= sizeof(double) >::type;
	}



	//
	// Sin, Cos, SinCos
	//

	template <typename T>
	inline T  Sin (const T& x)
	{
		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::sin( _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  Sin (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Sin( x[i] );
		return ret;
	}


	template <typename T>
	inline T  Cos (const T& x)
	{
		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::cos( _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  Cos (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Cos( x[i] );
		return ret;
	}


	template <typename T, typename S, typename C>
	inline void SinCos (const T& x, OUT S& s, OUT C& c)
	{
		s = Sin( x );
		c = Cos( x );
	}


	template <typename T, usize I, typename S, typename C>
	inline void  SinCos (const Vec<T,I> &x, OUT Vec<S,I> &s, OUT Vec<C,I> &c)
	{
		FOR( i, x )	SinCos( x[i], s[i], c[i] );
	}


	template <typename T>
	inline Vec<T,2>  SinCos (const T& x)
	{
		Vec<T,2>	ret;
		SinCos( x, ret.x, ret.y );
		return ret;
	}




	//
	// ASin, ACos, ASinCos
	//
	
	template <typename T>
	inline T  ASin (const T& x)
	{
		ASSERT( x >= T(-1) and x <= T(1) );

		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::asin( _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  ASin (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ASin( x[i] );
		return ret;
	}


	template <typename T>
	inline T  ACos (const T& x)
	{
		ASSERT( x >= T(-1) and x <= T(1) );

		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::acos( _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  ACos (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ACos( x[i] );
		return ret;
	}

	
	template <typename T, typename S, typename C>
	inline void ASinCos (const T& x, OUT S& s, OUT C& c)
	{
		s = ASin( x );
		c = ACos( x );
	}
	

	template <typename T, usize I, typename S, typename C>
	inline void  ASinCos (const Vec<T,I> &x, OUT Vec<S,I> &s, OUT Vec<C,I> &c)
	{
		FOR( i, x )	ASinCos( x[i], s[i], c[i] );
	}


	template <typename T>
	inline Vec<T,2>  ASinCos (const T& x)
	{
		Vec<T,2>	ret;
		ASinCos( x, ret.x, ret.y );
		return ret;
	}



	//
	// SinH, CosH, SinCosH
	//

	template <typename T>
	inline T  SinH (const T& x)
	{
		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::sinh( _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  SinH (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = SinH( x[i] );
		return ret;
	}


	template <typename T>
	inline T  CosH (const T& x)
	{
		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::cosh( _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  CosH (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = CosH( x[i] );
		return ret;
	}


	template <typename T, typename S, typename C>
	inline void  SinCosH (const T& x, OUT S& s, OUT C& c)
	{
		s = SinH( x );
		c = CosH( x );
	}

	
	template <typename T, usize I, typename S, typename C>
	inline void  SinCosH (const Vec<T,I> &x, OUT Vec<S,I> &s, OUT Vec<C,I> &c)
	{
		FOR( i, x )	SinCosH( x[i], s[i], c[i] );
	}


	template <typename T>
	inline Vec<T,2>  SinCosH (const T& x)
	{
		Vec<T,2>	ret;
		SinCosH( x, ret.x, ret.y );
		return ret;
	}



	//
	// ASinH, ACosH, ASinCosH
	//

	template <typename T>
	inline T  ASinH (const T& x)
	{
		return SignOrZero( x ) * Ln( x + Sqrt( (x*x) + T(1) ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  ASinH (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ASinH( x[i] );
		return ret;
	}


	template <typename T>
	inline T  ACosH (const T& x)
	{
		ASSERT( x >= T(1) );
		return Ln( x + Sqrt( (x*x) - T(1) ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  ACosH (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ACosH( x[i] );
		return ret;
	}


	template <typename T, typename S, typename C>
	inline void  ASinCosH (const T& x, OUT S& s, OUT C& c)
	{
		s = ASinH( x );
		c = ACosH( x );
	}

	
	template <typename T, usize I, typename S, typename C>
	inline void  ASinCosH (const Vec<T,I> &x, OUT Vec<S,I> &s, OUT Vec<C,I> &c)
	{
		FOR( i, x )	ASinCosH( x[i], s[i], c[i] );
	}


	template <typename T>
	inline Vec<T,2>  ASinCosH (const T& x)
	{
		Vec<T,2>	ret;
		ASinCosH( x, ret.x, ret.y );
		return ret;
	}



	//
	// Tan, CoTan, ATan, ACoTan
	//

	template <typename T>
	inline T  Tan (const T& x)
	{
		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::tan( _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  Tan (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Tan( x[i] );
		return ret;
	}


	template <typename T>
	inline T  CoTan (const T& x)
	{
		return SafeDiv( T(1), Tan( x ), T(0) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  CoTan (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = CoTan( x[i] );
		return ret;
	}
	

	template <typename T>
	inline T  TanH (const T& x)
	{
		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::tanh( _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  TanH (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = TanH( x[i] );
		return ret;
	}


	template <typename T>
	inline T  CoTanH (const T& x)
	{
		return SafeDiv( T(1), TanH( x ), T(0) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  CoTanH (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = CoTanH( x[i] );
		return ret;
	}


	template <typename T>
	inline T  ATan (const T& y_over_x)
	{
		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::atan( _float_t( y_over_x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  ATan (const Vec<T,I> &y_over_x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ATan( y_over_x[i] );
		return ret;
	}


	template <typename T>
	inline T  ATan (const T& y, const T& x)
	{
		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::atan2( _float_t( y ), _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  ATan (const Vec<T,I> &y, const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ATan( y[i], x[i] );
		return ret;
	}


	template <typename T>
	inline T  ACoTan (const T& x)
	{
		return SafeDiv( T(1), ATan( x ), T(0) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  ACoTan (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ACoTan( x[i] );
		return ret;
	}


	template <typename T>
	inline T  ATanH (const T& x)
	{
		ASSERT( x > T(-1) and x < T(1) );

		if ( Abs(x) == T(1) )	return Infinity<T>();	else
		if ( Abs(x) > T(1) )	return NaN<T>();		else
								return Ln( (T(1) + x) / (T(1) - x) ) / T(2);
	}


	template <typename T, usize I>
	inline Vec<T,I>  ATanH (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ATanH( x[i] );
		return ret;
	}


	template <typename T>
	inline T  ACoTanH (const T& x)
	{
		return SafeDiv( T(1), ATanH( x ), T(0) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  ACoTanH (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ACoTanH( x[i] );
		return ret;
	}



//-------------------------------------------------------------------
//	Operation with sign


	//
	// Abs
	//
	
	namespace _math_hidden_
	{
		template <bool S, bool I>
		struct _Abs_impl
		{
			template <typename T>	static T abs (const T& val);
		};

		template <>
		struct _Abs_impl<true,true>
		{
			template <typename T>	static T abs (const T& val)
			{
				return (val < T(0)) ? (val == MinValue<T>() ? -(val+1) : -val) : val;
			}
		};

		template <>
		struct _Abs_impl<false,true>
		{
			template <typename T>	static T abs (const T& val)		{ return val; }
		};

		template <>
		struct _Abs_impl<false,false>
		{
			template <typename T>	static T abs (const T& val)		{ return val; }
		};

		template <>
		struct _Abs_impl<true,false>
		{
			template <typename T>	static T abs (const T& val)
			{
				typedef typename CompileTime::Similar<T>::UInt	int_t;
			
				static const int_t	mask = ~( int_t(1) << CompileTime::TypeDescriptor::template GetTypeInfo<T>::SignBit() );

				T	ret( val );
				ReferenceCast< int_t >( ret ) &= mask;
				return ret;
			}
		};
		
		template <typename T>
		struct _Abs
		{
			static T abs (const T& val)
			{
				return _Abs_impl< CompileTime::IsSigned<T>, CompileTime::IsInteger<T> >::template abs( val );
			}
		};
	}
	
	template <typename T>
	inline T Abs (const T& x)
	{
		return _math_hidden_::_Abs<T>::abs( x );
	}


	template <typename T, usize I>
	inline Vec<T,I>  Abs (const Vec<T,I> &x)
	{
		Vec<T,I>	ret;
		FOR( i, ret )	ret[i] = Abs( x[i] );
		return ret;
	}



	//
	// Sign, SignOrZero
	//

	// 1 or -1
	template <typename T>
	inline T Sign (const T& x)
	{
		return ( x < T(0) ? T(-1) : T(1) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  Sign (const Vec<T,I> &x)
	{
		Vec<T,I>	ret;
		FOR( i, ret )	ret[i] = Sign<T>( x[i] );
		return ret;
	}


	// 1 or 0 or -1
	template <typename T>
	inline T SignOrZero (const T& x)
	{
		return T( x > T(0) ) - T( x < T(0) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  SignOrZero (const Vec<T,I> &x)
	{
		Vec<T,I>	ret;
		FOR( i, ret )	ret[i] = SignOrZero( x[i] );
		return ret;
	}



	//
	// Set or Reset Sign
	//

	namespace _math_hidden_
	{
		template <bool bFloat, bool bSigned>
		struct _SetSign_impl
		{
			template <typename T>
			static T setsign (const T& x, bool s);
		};

		template <>
		struct _SetSign_impl< true, true >
		{
			template <typename T>
			static T setsign (const T& x, bool s)
			{
				typedef typename CompileTime::Similar<T>::UInt	uint_t;
				
				const uint	u_sign_bit	= CompileTime::TypeDescriptor::template GetTypeInfo<T>::SignBit();

				T	ret(x);
				ReferenceCast<uint_t>( ret ) &= ~( uint_t(1) << u_sign_bit );
				ReferenceCast<uint_t>( ret ) |=  ( uint_t(s) << u_sign_bit );
				return ret;
			}
		};

		template <>
		struct _SetSign_impl< false, true >
		{
			template <typename T>
			static T setsign (const T& x, bool s)
			{
				if ( (  s and x > (T)0 ) or
					 ( not s and x < (T)0 ) )
				{
					return x == MinValue<T>() ? -(x+1) : -x;
				}
				return x;
			}
		};

		template <>
		struct _SetSign_impl< true, false >
		{
			template <typename T>
			static T setsign (const T& x, bool s)
			{
				ASSUME( not s and "value already unsigned" );
				return x;
			}
		};

		template <>
		struct _SetSign_impl< false, false >
		{
			template <typename T>
			static T setsign (const T& x, bool s)
			{
				ASSUME( not s and "value already unsigned" );
				return x;
			}
		};

		template <typename T>
		struct _SetSign
		{
			static T setsign (const T& x, bool s)
			{
				return _SetSign_impl< CompileTime::IsFloat<T>, CompileTime::IsSigned<T> >::setsign( x, s );
			}
		};
	}

	template <typename T>
	inline T SetSign (const T &x, bool sign)
	{
		return _math_hidden_::_SetSign<T>::setsign( x, sign );
	}


	template <typename T, usize I>
	inline Vec<T,I>  SetSign (const Vec<T,I> &x, bool sign)
	{
		Vec<T,I>	ret;
		FOR( i, ret )	ret[i] = SetSign( x[i], sign );
		return ret;
	}


	template <typename T, usize I>
	inline Vec<T,I>  SetSign (const Vec<T,I> &x, const Vec<bool,I> &sign)
	{
		Vec<T,I>	ret;
		FOR( i, ret )	ret[i] = SetSign( x[i], sign[i] );
		return ret;
	}



	//
	// Copy Sign
	//

	namespace _math_hidden_
	{
		template <bool bFloat, bool bSigned>
		struct _CopySign_impl
		{
			template <typename T>
			static T copysign (const T& x, const T& s);
		};

		template <>
		struct _CopySign_impl<true,true>
		{
			template <typename T>
			static T copysign (const T& x, const T& s)
			{
				typedef typename CompileTime::Similar<T>::UInt	uint_t;
				
				const uint_t	u_sign_mask	= uint_t(1) << CompileTime::TypeDescriptor::template GetTypeInfo<T>::SignBit();

				T	ret(x);
				ReferenceCast<uint_t>( ret ) &= ~u_sign_mask;
				ReferenceCast<uint_t>( ret ) |=  ReferenceCast<uint_t>( s ) & u_sign_mask;
				return ret;
			}
		};

		template <>
		struct _CopySign_impl<false,true>
		{	
			template <typename T>
			static T copysign (const T& x, const T& s)
			{
				if ( ( s >= T(0) and x >= T(0) ) or
					 ( s <= T(0) and x <= T(0) ) )
				{
					return x;
				}
				return -x;
			}
		};

		template <>
		struct _CopySign_impl<true,false>
		{
			template <typename T>
			static T copysign (const T& x, const T& s)
			{
				return x;
			}
		};

		template <>
		struct _CopySign_impl<false,false>
		{
			template <typename T>
			static T copysign (const T& x, const T& s)
			{
				return x;
			}
		};

		template <typename T>
		struct _CopySign
		{
			static T copysign (const T& x, const T& s)
			{
				return _CopySign_impl< CompileTime::IsFloat<T>,
									   CompileTime::IsSigned<T> >::copysign( x, s );
			}
		};
	}
	
	template <typename T>
	inline T CopySign (const T& from, const T& to)
	{
		return _math_hidden_::_CopySign<T>::copysign( to, from );
	}


	template <typename T, usize I>
	inline Vec<T,I>  CopySign (const Vec<T,I> &from, const Vec<T,I> &to)
	{
		Vec<T,I>	ret;
		FOR( i, ret )	ret[i] = CopySign( from[i], to[i] );
		return ret;
	}



//-------------------------------------------------------------------
//	Compare operations


	//
	// IsZero
	//

	namespace _math_hidden_
	{
		template <bool I>
		struct _IsZero_impl
		{
			template <typename T>	static bool is_zero (const T& x);
		};

		template <>
		struct _IsZero_impl<true>
		{
			template <typename T>	static bool is_zero (const T& x)	{ return x == T(0); }
		};

		template <>
		struct _IsZero_impl<false>
		{
			template <typename T>	static bool is_zero (const T& x)	{ return Abs(x) <= CompileTime::TypeDescriptor::template GetTypeInfo<T>::Epsilon(); }
		};

		template <typename T>
		struct _IsZero
		{
			static bool is_zero (const T& x)
			{
				return _IsZero_impl< CompileTime::IsInteger<T> >::is_zero( x );
			}
		};
	}

	template <typename T>
	inline bool IsZero (const T& x)
	{
		return _math_hidden_::_IsZero<T>::is_zero( x );
	}

	template <>
	inline bool IsZero (const bool& x)
	{
		return not x;
	}

	
	template <typename T, usize I>
	inline bool IsZero (const Vec<T,I>& x)
	{
		return All( x.IsZero() );
	}


	template <typename T>
	inline bool IsNotZero (const T& x)
	{
		return not IsZero( x );
	}

	template <>
	inline bool IsNotZero (const bool& x)
	{
		return x;
	}
	


	//
	// Equals
	//

	namespace _math_hidden_
	{
		// integer
		template <bool F>
		struct _Equals_impl
		{
			template <typename T>
			static bool eq (const T& a, const T& b)
			{ 
				return a == b;
			}

			template <typename T>
			static bool eqa (const T& a, const T& b, uint ac)
			{
				typedef typename CompileTime::Similar<T>::Unsigned	uint_t;
				return uint_t( Abs( a - b ) ) <= ( uint_t(1) << ac );
			}
		};

		template <typename T>	struct _FloatAccuracy			{ static const uint	value = CompileTime::SizeOf<T>::bits / 3; };
		template <>				struct _FloatAccuracy<double>	{ static const uint	value = CompileTime::SizeOf<double>::bits * 2 / 3; };

		
		// float
		template <>
		struct _Equals_impl<true>
		{
			template <typename T>
			static bool eq (const T& a, const T& b)
			{
				return eqa( a, b, _FloatAccuracy<T>::value );
			}

			template <typename T>
			static bool eqa (const T& a, const T& b, uint ac)
			{
				typedef typename CompileTime::Similar<T>::Int	int_t;
				typedef typename CompileTime::Similar<T>::UInt	uint_t;

				int_t	i_a = ReferenceCast<int_t>(a);
				int_t	i_b	= ReferenceCast<int_t>(b);

				if ( i_a < int_t(0) )	i_a = MinValue<int_t>() - i_a;
				if ( i_b < int_t(0) )	i_b = MinValue<int_t>() - i_b;

				uint_t	diff = (uint_t)Abs( i_a - i_b );

				return ( diff <= ( uint_t(1) << ac ) );
			}
		};
		
		template <typename T>
		struct _Equals
		{
			static bool eq (const T& a, const T& b)
			{
				return _Equals_impl< CompileTime::IsFloat<T> >::eq( a, b );
			}

			static bool eqa (const T& a, const T& b, uint ac)
			{
				return _Equals_impl< CompileTime::IsFloat<T> >::eqa( a, b, ac );
			}
		};
	}

	template <typename T>
	inline bool Equals (const T& a, const T& b)
	{
		return _math_hidden_::_Equals<T>::eq( a, b );
	}


	template <typename T, usize I>
	inline Vec<bool,I>  Equals (const Vec<T,I> &a, const Vec<T,I> &b)
	{
		Vec<bool,I>	ret;
		FOR( i, ret )	ret[i] = Equals( a[i], b[i] );
		return ret;
	}


	template <typename T>
	inline bool EqualsWithError (const T& a, const T& b, /*Bits*/uint accuracy)
	{
		return _math_hidden_::_Equals<T>::eqa( a, b, accuracy );
	}


	template <typename T, usize I>
	inline Vec<bool,I>  EqualsWithError (const Vec<T,I> &a, const Vec<T,I> &b, /*Bits*/uint accuracy)
	{
		Vec<bool,I>	ret;
		FOR( i, ret )	ret[i] = EqualsWithError( a[i], b[i], accuracy );
		return ret;
	}



	//
	// MiddleValue
	//
	
	namespace _math_hidden_
	{
		// float
		template <typename T, bool IsFloat, bool IsSigned>
		struct _MiddleValue {
			static T Get (const T& a, const T& b)	{ return (a + b) * T(0.5); }	// TODO: is it good for big values?
		};

		// unsigned int
		template <typename T>
		struct _MiddleValue< T, false, false > {
			static T Get (const T& a, const T& b)	{ return a < b ? (a + (b-a) / T(2)) : (b + (a-b) / T(2)); }
		};
		
		// signed int
		template <typename T>
		struct _MiddleValue< T, false, true > {
			static T Get (const T& a, const T& b)	{ return a + (b-a) / T(2); }
		};

	}	// _math_hidden_

	template <typename A, typename B>
	inline typename CompileTime::MainType<A,B>  MiddleValue (const A& a, const B& b)
	{
		typedef typename CompileTime::MainType<A,B>	T;

		return _math_hidden_::_MiddleValue< T, CompileTime::IsFloat<T>, CompileTime::IsSigned<T> >::Get( T(a), T(b) );
	}


	template <typename A, typename B, usize I>
	inline Vec< typename CompileTime::MainType<A,B>, I >  MiddleValue (const Vec<A,I>& a, const Vec<B,I>& b)
	{
		Vec< typename CompileTime::MainType<A,B>, I >	ret;
		FOR( i, ret )	ret[i] = MiddleValue( a[i], b[i] );
		return ret;
	}



	//
	// Clamp (ClampInnerRange)
	//
	
	template <typename A, typename B, typename C>
	inline typename CompileTime::MainType<A,B,C>  Clamp (const A& value, const B& minValue, const C& maxValue)
	{
		ASSERT( minValue <= maxValue );
		return Min( maxValue, Max( value, minValue ) );
	}

	
	template <typename A, typename B, typename C, usize I>
	inline Vec< typename CompileTime::MainType<A,B,C>, I >  Clamp (const Vec<A,I>& value, const Vec<B,I>& minValue, const Vec<C,I>& maxValue)
	{
		Vec< typename CompileTime::MainType<A,B,C>, I >	ret;
		FOR( i, ret )	ret[i] = Clamp( value[i], minValue[i], maxValue[i] );
		return ret;
	}

	
	template <typename A, typename B, typename C, usize I>
	inline Vec< typename CompileTime::MainType<A,B,C>, I >  Clamp (const Vec<A,I>& value, const B& minValue, const C& maxValue)
	{
		Vec< typename CompileTime::MainType<A,B,C>, I >	ret;
		FOR( i, ret )	ret[i] = Clamp( value[i], minValue, maxValue );
		return ret;
	}



	//
	// ClampOut (ClampOuterRange)
	//
	
	template <typename A, typename B, typename C>
	inline typename CompileTime::MainType<A,B,C> ClampOut (const A& value, const B& minValue, const C& maxValue)
	{
		ASSERT( minValue <= maxValue );

		typedef typename CompileTime::MainType<A,B,C>	T;

		const T	middle = MiddleValue( minValue, maxValue );
		
		return	( value < middle ? 
				( value < minValue ?  value : minValue ) :
				( value > maxValue ?  value : maxValue ) );
	}
	

	template <typename A, typename B, typename C, usize I>
	inline Vec< typename CompileTime::MainType<A,B,C>, I >  ClampOut (const Vec<A,I>& value, const Vec<B,I>& minValue, const Vec<C,I>& maxValue)
	{
		Vec< typename CompileTime::MainType<A,B,C>, I >	ret;
		FOR( i, ret )	ret[i] = ClampOut( value[i], minValue[i], maxValue[i] );
		return ret;
	}
	
	
	template <typename A, typename B, typename C, usize I>
	inline Vec< typename CompileTime::MainType<A,B,C>, I >  ClampOut (const Vec<A,I>& value, const B& minValue, const C& maxValue)
	{
		Vec< typename CompileTime::MainType<A,B,C>, I >	ret;
		FOR( i, ret )	ret[i] = ClampOut( value[i], minValue, maxValue );
		return ret;
	}



	//
	// Wrap
	//

	namespace _math_hidden_
	{
		template <typename T, bool isInteger>
		struct _Wrap_Impl
		{
			static T Get (const T& value, const T& minValue, const T& maxValue)
			{
				// check for NaN
				if ( minValue >= maxValue )
					return minValue;

				T	result = T( minValue + Mod( value - minValue, maxValue - minValue ) );
		
				if ( result < minValue )
					result += (maxValue - minValue);

				return result;
			}
		};

		template <typename T>
		struct _Wrap_Impl<T, true>
		{
			static T Get (const T& value, const T& minValue, const T& maxValue)
			{
				return _Wrap_Impl<T, false>::Get( value, minValue, maxValue + T(1) );
			}
		};
	}
	

	template <typename A, typename B, typename C>
	inline typename CompileTime::MainType<A,B,C>  Wrap (const A& value, const B& minValue, const C& maxValue)
	{
		// Warning: float value never equal maxValue!

		typedef typename CompileTime::MainType<A,B,C>	T;

		ASSERT( minValue < maxValue );

		return _math_hidden_::_Wrap_Impl< T, CompileTime::IsInteger<T> >::Get( value, minValue, maxValue );
	}

	
	template <typename A, typename B, typename C, usize I>
	inline Vec< typename CompileTime::MainType<A,B,C>, I >  Wrap (const Vec<A,I>& value, const Vec<B,I>& minValue, const Vec<C,I>& maxValue)
	{
		Vec< typename CompileTime::MainType<A,B,C>, I >	ret;
		FOR( i, ret )	ret[i] = Wrap( value[i], minValue[i], maxValue[i] );
		return ret;
	}

	
	template <typename A, typename B, typename C, usize I>
	inline Vec< typename CompileTime::MainType<A,B,C>, I >  Wrap (const Vec<A,I>& value, const B& minValue, const C& maxValue)
	{
		Vec< typename CompileTime::MainType<A,B,C>, I >	ret;
		FOR( i, ret )	ret[i] = Wrap( value[i], minValue, maxValue );
		return ret;
	}



	//
	// All, Any, Most
	//
	
	namespace _math_hidden_
	{
		template <usize C>
		struct _AnyAllMost_impl
		{
			template <typename T, usize I>	static bool Any (const GXMath::Vec<T,I> &v)		{ return _AnyAllMost_impl<C-1>::Any( v ) | GXMath::IsNotZero( v[C] ); }
			template <typename T, usize I>	static bool All (const GXMath::Vec<T,I> &v)		{ return _AnyAllMost_impl<C-1>::All( v ) & GXMath::IsNotZero( v[C] ); }
			//template <typename T, usize I>	static uint Most (const GXMath::Vec<T,I> &v)	{ return _AnyAllMost_impl<C-1>::Most( v ) + (uint)GXMath::IsNotZero( v[C] ); }
		};
		
		template <>
		struct _AnyAllMost_impl<0>
		{
			template <typename T, usize I>	static bool Any (const GXMath::Vec<T,I> &v)		{ return GXMath::IsNotZero( v[0] ); }
			template <typename T, usize I>	static bool All (const GXMath::Vec<T,I> &v)		{ return GXMath::IsNotZero( v[0] ); }
			//template <typename T, usize I>	static uint Most (const GXMath::Vec<T,I> &v)	{ return (uint) GXMath::IsNotZero( v[0] ); }
		};

		template <typename T, usize I>
		struct _AnyAllMost
		{
			static bool  All(const GXMath::Vec<T,I> &v)		{ return _AnyAllMost_impl<I-1>::All( v ); }
			static bool  Any(const GXMath::Vec<T,I> &v)		{ return _AnyAllMost_impl<I-1>::Any( v ); }
			//static bool  Most(const GXMath::Vec<T,I> &v)	{ return _AnyAllMost_impl<I-1>::Most( v )*2 >= I; }	// 1 of 2; 2 of 3; 2 of 4;
		};
	}
	
	template <typename T>
	inline bool All (const T& x)
	{
		return IsNotZero( x );
	}


	template <typename T, usize I>
	inline bool All (const Vec<T,I> &x)
	{
		return _math_hidden_::_AnyAllMost<T,I>::All( x );
	}

	
	template <typename T>
	inline bool Any (const T& x)
	{
		return IsNotZero( x );
	}


	template <typename T, usize I>
	inline bool Any (const Vec<T,I> &x)
	{
		return _math_hidden_::_AnyAllMost<T,I>::Any( x );
	}

	
	/*template <typename T>
	inline bool Most (const T& x)
	{
		return IsNotZero( x );
	}


	template <typename T, usize I>
	inline bool Most (const Vec<T,I> &x)
	{
		return _math_hidden_::_AnyAllMost<T,I>::Most( x );
	}*/

	

	//
	// Max
	//

	template <typename A, typename B>
	inline typename CompileTime::MainType<A,B>  Max (const A& a, const B& b)
	{
		return ( a > b ? a : b );
	}

	template <typename A, typename B, usize I>
	inline Vec< typename CompileTime::MainType<A,B>, I >  Max (const Vec<A,I> &a, const Vec<B,I> &b)
	{
		Vec< typename CompileTime::MainType<A,B>, I >	ret;
		FOR( i, ret )	ret[i] = Max( a[i], b[i] );
		return ret;
	}
	
	template <typename A, typename B, typename C>
	inline typename CompileTime::MainType<A,B,C>  Max (const A& a, const B& b, const C& c)
	{
		return Max( Max( a, b ), c );
	}
	
	template <typename A, typename B, typename C, typename D>
	inline typename CompileTime::MainType<A,B,C,B>  Max (const A& a, const B& b, const C& c, const D& d)
	{
		return Max( Max( a, b ), Max( c, d ) );
	}
	

	//
	// Min
	//

	template <typename A, typename B>
	inline typename CompileTime::MainType<A,B>  Min (const A& a, const B& b)
	{
		return ( a > b ? b : a );
	}

	template <typename A, typename B, usize I>
	inline Vec< typename CompileTime::MainType<A,B>, I >  Min (const Vec<A,I> &a, const Vec<B,I> &b)
	{
		Vec< typename CompileTime::MainType<A,B>, I >	ret;
		FOR( i, ret )	ret[i] = Min( a[i], b[i] );
		return ret;
	}
	
	template <typename A, typename B, typename C>
	inline typename CompileTime::MainType<A,B,C>  Min (const A& a, const B& b, const C& c)
	{
		return Min( Min( a, b ), c );
	}
	
	template <typename A, typename B, typename C, typename D>
	inline typename CompileTime::MainType<A,B,C,D>  Min (const A& a, const B& b, const C& c, const D& d)
	{
		return Min( Min( a, b ), Min( c, d ) );
	}

	
	//
	// MinAbs
	//

	template <typename A, typename B>
	inline typename CompileTime::MainType<A,B>  MinAbs (const A& a, const B& b)
	{
		return ( Abs(a) > Abs(b) ? b : a );
	}

	template <typename A, typename B, usize I>
	inline Vec< typename CompileTime::MainType<A,B>, I >  MinAbs (const Vec<A,I> &a, const Vec<B,I> &b)
	{
		Vec< typename CompileTime::MainType<A,B>, I >	ret;
		FOR( i, ret )	ret[i] = MinAbs( a[i], b[i] );
		return ret;
	}

	
	//
	// MaxAbs
	//

	template <typename A, typename B>
	inline typename CompileTime::MainType<A,B> MaxAbs (const A& a, const B& b)
	{
		return ( Abs(a) > Abs(b) ? a : b );
	}

	template <typename A, typename B, usize I>
	inline Vec< typename CompileTime::MainType<A,B>, I >  MaxAbs (const Vec<A,I> &a, const Vec<B,I> &b)
	{
		Vec< typename CompileTime::MainType<A,B>, I >	ret;
		FOR( i, ret )	ret[i] = MaxAbs( a[i], b[i] );
		return ret;
	}

	
	//
	// MinMax
	//

	template <typename A, typename B>
	inline void	MinMax (OUT typename CompileTime::MainType<A,B>& tMin,
						OUT typename CompileTime::MainType<A,B>& tMax,
						const A& a, const B& b)
	{
		const bool	res = ( a > b );
		tMin = res ? b : a;
		tMax = res ? a : b;
	}

	template <typename A, typename B, usize I>
	inline void	MinMax (OUT Vec< typename CompileTime::MainType<A,B>, I >& tMin,
						OUT Vec< typename CompileTime::MainType<A,B>, I >& tMax,
						const Vec<A,I>& a, const Vec<B,I>& b)
	{
		FOR( i, a ) {
			MinMax( tMin[i], tMax[i], a[i], b[i] );
		}
	}

	
	//
	// MinMag
	//

	template <typename A, typename B>
	inline typename CompileTime::MainType<A,B>  MinMag (const A& a, const B& b)
	{
		if ( Abs(a) > Abs(b) )	return b;
		if ( Abs(a) < Abs(b) )	return a;
		return Min( a, b );
	}

	template <typename A, typename B, usize I>
	inline Vec< typename CompileTime::MainType<A,B>, I >  MinMag (const Vec<A,I>& a, const Vec<B,I>& b)
	{
		Vec< typename CompileTime::MainType<A,B>, I >	ret;
		FOR( i, ret )	ret[i] = MinMag( a[i], b[i] );
		return ret;
	}

	
	//
	// MaxMag
	//

	template <typename A, typename B>
	inline typename CompileTime::MainType<A,B>  MaxMag (const A& a, const B& b)
	{
		if ( Abs(a) > Abs(b) )	return a;
		if ( Abs(a) < Abs(b) )	return b;
		return Max( a, b );
	}

	template <typename A, typename B, usize I>
	inline Vec< typename CompileTime::MainType<A,B>, I >  MaxMag (const Vec<A,I>& a, const Vec<B,I>& b)
	{
		Vec< typename CompileTime::MainType<A,B>, I >	ret;
		FOR( i, ret )	ret[i] = MaxMag( a[i], b[i] );
		return ret;
	}
	
	
	//
	// Mid
	//

	template <typename A, typename B, typename C>
	inline typename CompileTime::MainType<A,B,C>  Mid (const A& a, const B& b, const C& c)
	{
		return Min( Max( a, b ), Max( b, c ) );
	}

	template <typename A, typename B, typename C, usize I>
	inline Vec< typename CompileTime::MainType<A,B,C>, I >  Mid (const Vec<A,I>& a, const Vec<B,I>& b, const Vec<C,I>& c)
	{
		Vec< typename CompileTime::MainType<A,B,C>, I >	ret;
		FOR( i, ret )	ret[i] = Mid( a[i], b[i], c[i] );
		return ret;
	}
	

//-------------------------------------------------------------------
//	

	//
	// Static Pow (deprecated)
	//
	
	namespace _math_hidden_
	{
		template <int I>
		struct _TPowerStructUnsigned
		{
			template <typename T>	static T power (const T& x)		{ return _TPowerStructUnsigned<I-1>::power( x ) * x; }
		};
			
		template <>
		struct _TPowerStructUnsigned<0>
		{
			template <typename T>	static T power (const T&)		{ return 1; }
		};

		
		template <bool S>
		struct _TPowerStruct
		{
			template <typename T, int I>
			static T power (const T& x)		{ return _TPowerStructUnsigned<I>::power( x ); }
		};

		template <>
		struct _TPowerStruct<true>
		{
			template <typename T, int I>
			static T power (const T& x)		{ return T(1) / _TPowerStructUnsigned<-I>::power( x ); }
		};


		template <int I, typename T>
		inline T StaticPow (const T& x)
		{
			return _TPowerStruct< (I<0) >::template power<T,I>( x );
		}


		template <int P, typename T, usize I>
		inline Vec<T,I>  StaticPow (const Vec<T,I>& x)
		{
			Vec<T,I>	ret;
			FOR( i, ret )	ret[i] = StaticPow<P>( x[i] );
			return ret;
		}

	}	// _math_hidden_





	//
	// Pow
	//

	template <typename T, typename B>
	inline T Pow (const T&x, const B& y)
	{
		typedef typename CompileTime::MainType<T,B>				main_t;
		typedef typename _math_hidden_::ToNearFloat< main_t >	_float_t;
		return (T) ::pow( _float_t( x ), _float_t( y ) );
	}


	template <typename T, typename B, usize I>
	inline Vec<T,I>  Pow (const Vec<T,I>& x, const Vec<B,I>& y)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Pow( x[i], y[i] );
		return ret;
	}
	


	//
	// Pow2, Pow10 (deprecated)
	//

	namespace _math_hidden_
	{
		template <typename R, typename T>
		inline R Pow2 (const int& p)
		{
			if ( p >= 0 )
				return R( T(1) << p );
			else
				return R(1) / R( T(1) << (-p) ); 
		}


		template <typename R, typename T, usize I>
		inline Vec<R,I>  Pow2 (const Vec<int,I>& p)
		{
			Vec<R,I>	ret;
			FOR( i, ret )	ret[i] = Pow2<R,T>( p[i] );
			return ret;
		}


		template <typename R>
		inline R Pow2 (const int& p)
		{
			return Pow2<R,int>( p );
		}


		template <typename R, usize I>
		inline Vec<R,I>  Pow2 (const Vec<int,I>& p)
		{
			Vec<R,I>	ret;
			FOR( i, ret )	ret[i] = Pow2<R,int>( p[i] );
			return ret;
		}
	

		template <typename R>
		inline R  Pow10 (const int p)
		{
			R	ret = R(1);

			for (int i = 0; i < Abs(p); ++i)
				ret *= R(10);

			return ( p < 0  ?  R(1) / ret  :  ret );
		}


		template <typename T, usize I>
		inline Vec<T,I>  Pow10 (const Vec<int,I> &p)
		{
			Vec<T,I>	ret;
			FOR( i, ret )	ret[i] = Pow10<T>( p[i] );
			return ret;
		}

	

		//
		// Static Pow2
		//

		template <int P, typename R, typename T>
		constexpr
		inline R StaticPow2 ()
		{
			if ( P >= 0 )
				return R( T(1) << P );
			else
				return R(1) / R( T(1) << (-P) );
		}


		template <int P, typename R>
		constexpr
		inline R StaticPow2 ()
		{
			return StaticPow2<P,R,int>();
		}

	}	// _math_hidden_



	//
	// Ln, Log, Log2, Log10
	//

	template <typename T>
	inline T Ln (const T& x)
	{
		ASSERT( x >= T(0) );
		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::log( _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  Ln (const Vec<T,I>& x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Ln( x[i] );
		return ret;
	}


	// deprecated
	namespace _math_hidden_
	{
		template <uint Base, typename T>
		inline T Log (const T& x)
		{
			return Log( x, T( Base ) );
		}

		template <uint Base, typename T, usize I>
		inline Vec<T,I>  Log (const Vec<T,I>& x)
		{
			Vec<T,I>		ret;
			FOR( i, ret )	ret[i] = Log<Base>( x[i] );
			return ret;
		}

	}	// _math_hidden_

	
	template <typename T>
	inline T  Log (const T& x, const T& base)
	{
		return Ln( x ) / Ln( base );
	}


	template <typename T, usize I>
	inline Vec<T,I>  Log (const Vec<T,I>& x, const T& base)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Log( x[i], base );
		return ret;
	}


	template <typename T, usize I>
	inline Vec<T,I>  Log (const Vec<T,I>& x, const Vec<T,I>& base)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Log( x[i], base[i] );
		return ret;
	}


	template <typename T>
	inline T Log2 (const T& x)
	{
		static const T	base = Ln( T(2) );
		return Ln( x ) / base;
	}


	template <typename T, usize I>
	inline Vec<T,I>  Log2 (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Log2( x[i] );
		return ret;
	}


	template <typename T>
	inline T Log10 (const T& x)
	{
		ASSERT( x >= T(0) );
		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::log10( _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  Log10 (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Log10( x[i] );
		return ret;
	}



	//
	// Exp, Exp2, Exp10
	//

	template <typename T>
	inline T Exp (const T& x)
	{
		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::exp( _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  Exp (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Exp( x[i] );
		return ret;
	}


	template <typename T>
	inline T Exp2 (const T& x)
	{
		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::pow( _float_t( 2 ), _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  Exp2 (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Exp2( x[i] );
		return ret;
	}
	

	template <typename T>
	inline T  Exp10 (const T& x)
	{
		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::pow( _float_t( 10 ), _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  Exp10 (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Exp10( x[i] );
		return ret;
	}



	//
	// Integer Log2 (BitScanReverse)
	//

	namespace _math_hidden_
	{
		template <uint Bit>
		struct _RecursiveBitScanReverse
		{
			template <typename T>
			inline static uint Get (const T& x)
			{
				return uint( T(x >> Bit) > 0 ) + _RecursiveBitScanReverse< Bit-1 >::Get( x );
			}
		};

		template <>
		struct _RecursiveBitScanReverse<0>
		{
			template <typename T>
			inline static uint Get (const T& x)
			{
				return uint( x > 0 );
			}
		};

		template <uint Bit>
		struct _RecursiveBitScanForward
		{
			template <typename T>
			inline static uint Get (const T& x)
			{
				uint cur = uint( T(x << Bit) > 0 );
				uint prev = _RecursiveBitScanForward< Bit-1 >::Get( x );
				return cur + prev;
			}
		};

		template <>
		struct _RecursiveBitScanForward<0>
		{
			template <typename T>
			inline static uint Get (const T& x)
			{
				return uint( x > 0 );
			}
		};

	}	// _math_hidden_


	template <typename T>
	inline uint IntLog2 (const T& x)
	{
		CompileTime::MustBeInteger<T>();
		
		typedef typename CompileTime::Similar<T>::Unsigned	utype;

		const utype	value = ReferenceCast<utype>(x);

		return _math_hidden_::_RecursiveBitScanReverse< CompileTime::SizeOf<utype>::bits-1 >::Get( value ) - 1;
	}

	template <typename T, usize I>
	inline Vec<uint,I>  IntLog2 (const Vec<T,I> &x)
	{
		Vec<uint,I>	ret;
		FOR( i, ret )	ret[i] = IntLog2( x[i] );
		return ret;
	}

	
	template <typename T>
	inline T CeilPowerOfTwo (const T& x)
	{
		CompileTime::MustBeInteger<T>();

		return ( T(1) << ( IntLog2(x) + not IsPowerOfTwo(x) ) ) * Sign(x);
	}

	template <typename T, usize I>
	inline Vec<T,I>  CeilPowerOfTwo (const Vec<T,I> &x)
	{
		Vec<T,I>	ret;
		FOR( i, ret )	ret[i] = CeilPowerOfTwo( x[i] );
		return ret;
	}

	
	template <typename T>
	inline T FloorPowerOfTwo (const T& x)
	{
		CompileTime::MustBeInteger<T>();

		return ( T(1) << IntLog2(x) ) * Sign(x);
	}

	template <typename T, usize I>
	inline Vec<T,I>  FloorPowerOfTwo (const Vec<T,I> &x)
	{
		Vec<T,I>	ret;
		FOR( i, ret )	ret[i] = FloorPowerOfTwo( x[i] );
		return ret;
	}

	
	template <typename T>
	inline T NearPowerOfTwo (const T& x)
	{
		CompileTime::MustBeInteger<T>();
		
		const T		absx	= Abs( x );
		const uint	pot		= IntLog2( x );
		const T		a		= T(1) << (pot + 1);
		const T		b		= T(1) << pot;
		const T		c		= ((a - b) >> 1) + b;
		return ( absx >= c ? a : b ) * Sign( x );
	}

	template <typename T, usize I>
	inline Vec<T,I>  NearPowerOfTwo (const Vec<T,I> &x)
	{
		Vec<T,I>	ret;
		FOR( i, ret )	ret[i] = NearPowerOfTwo( x[i] );
		return ret;
	}
	
	
	template <typename T>
	inline bool IsPowerOfTwo (const T& x)
	{
		CompileTime::MustBeInteger<T>();
		
		const T	a = Abs( x );
		return ( a != 0 and ( (a & (a - 1)) == 0 ) );
	}

	template <typename T, usize I>
	inline Vec<bool,I>  IsPowerOfTwo (const Vec<T,I> &x)
	{
		Vec<bool,I>	ret;
		FOR( i, ret )	ret[i] = IsPowerOfTwo( x[i] );
		return ret;
	}



	//
	// Square
	//

	template <typename T>
	inline T Square (const T& x)
	{
		return x * x;
	}


	template <typename T, usize I>
	inline Vec<T,I>  Square (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Square( x[i] );
		return ret;
	}
	


	//
	// Sqrt
	//

	template <typename T>
	inline T Sqrt (const T& x)
	{
		ASSERT( x >= T(0) );

		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::sqrt( _float_t( x ) );
	}

#ifdef GX_FAST_MATH
	template <>
	inline T Sqrt (const float& x)
	{
		return FSqrt( x );
	}
#endif


	template <typename T, usize I>
	inline Vec<T,I>  Sqrt (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Sqrt( x[i] );
		return ret;
	}
	


	//
	// Inverse Sqrt
	//

	template <typename T>
	inline T InvSqrt (const T& x)
	{
		return T(1) / Sqrt( x );
	}


#ifdef GX_FAST_MATH
	template <>
	inline float InvSqrt (const float &x)
	{
		return FInvSqrt( x );
	}
#endif


	template <typename T, usize I>
	inline Vec<T,I>  InvSqrt (const Vec<T,I> &x)
	{
		Vec<T,I>	ret;
		FOR( i, ret )	ret[i] = InvSqrt( x[i] );
		return ret;
	}

	

	//
	// Square Sign
	//

	template <typename T>
	inline T SquareSign (const T& x)
	{
		return ( x < T(0) ? -Square(x) : Square(x) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  SquareSign (const Vec<T,I> &x)
	{
		Vec<T,I>	ret;
		FOR( i, ret )	ret[i] = SquareSign( x[i] );
		return ret;
	}

	

//-------------------------------------------------------------------
//	Interpolation

	//
	// Mix, QMix, BiMix, TriMix
	//
	
	template <typename T>
	inline T GetInterpolation (const T& x, const T& y, const T& interpolated)
	{
		return (interpolated - x) / (y - x);
	}


	// linear interpolation
	template <typename T, typename B>
	inline T Mix (const T& x, const T& y, const B& a)
	{
		return T( a * (y - x) + x );
	}
	

	// quadratic interpolation
	template <typename T, typename B>
	inline T QMix (const T& x, const T& y, const T& z, const B& a)
	{
		const B		inv  = B(1) - a;
		const B		mul0 = inv * inv;
		const B		mul1 = B(2) * a * inv;

		return (T)( x * mul0 + y * mul1 + z * Square( a ) );
	}


	// bilinear interpolation
	template <typename T, typename B>
	inline T BiMix (const T& v0, const T& v1, const T& v2, const T& v3, const B& a)
	{
		return Mix( Mix( v0, v3, a[0] ), Mix( v1, v2, a[0] ), a[1] );
	}


	template <typename T, typename B>
	inline T BiMix (const T& v0, const T& v1, const T& v2, const T& v3, const B& a0, const B& a1)
	{
		return Mix( Mix( v0, v3, a0 ), Mix( v1, v2, a0 ), a1 );
	}

	
	// trilinear interpolation
	template <typename T, typename B>
	inline T TriMix (const T &v0, const T &v1, const T &v2, const T &v3, const T &v4,
					 const T &v5, const T &v6, const T &v7, const B& a)
	{
		return Mix( Mix( Mix( v0, v2, a[0] ), Mix( v1, v3, a[0] ), a[1] ),
					Mix( Mix( v4, v6, a[0] ), Mix( v5, v7, a[0] ), a[1] ),
					a[2] );
	}
	  

	// cubic interpolation
	template <typename T>
	inline T CubicMix (const T& v0, const T& v1, const T& v2, const T& v3, const T& a)
	{
		return	((v3 - v2) - (v0 - v1)) * a * a * a +
				(v3 - v2) * a * a +
				(v2 - v0) * a +
				v1;
	}

	
//-------------------------------------------------------------------
//	Other


	//
	// Mod, SafeMod
	//

	namespace _math_hidden_
	{
		template <typename A, typename B, bool IsInt>
		struct ModNearType
		{
			typedef typename CompileTime::MainType<A,B>		main_t;

			static main_t  Mod (const A& l, const B& r)
			{
				typedef CompileTime::NearFloat::FromType< main_t >	_float_t;
				return (main_t) ::fmod( _float_t(l), _float_t(r) );
			}

			template <typename C>
			static main_t  SafeMod (const A& l, const B& r, const C& def)
			{
				typedef CompileTime::NearFloat::FromType< main_t >	_float_t;
				return IsNotZero( r ) ? (main_t) ::fmod( _float_t(l), _float_t(r) ) : maint_t(def);
			}
		};

		template <typename A, typename B>
		struct ModNearType<A,B,true>
		{
			typedef typename CompileTime::MainType<A,B>		main_t;

			static main_t  Mod (const A& l, const B& r)
			{
				return main_t( main_t(l) % main_t(r) );
			}

			template <typename C>
			static main_t  SafeMod (const A& l, const B& r, const C& def)
			{
				return IsNotZero( r ) ? main_t( main_t(l) % main_t(r) ) : main_t(def);
			}
		};

	}	// _math_hidden_


	template <typename A, typename B>
	inline typename CompileTime::MainType<A,B>  Mod (const A& left, const B& right)
	{
		typedef typename CompileTime::MainType<A,B>		main_t;

		return _math_hidden_::ModNearType< A, B, CompileTime::IsInteger< main_t > >::Mod( left, right );
	}

	template <typename A, typename B, usize I>
	inline Vec< typename CompileTime::MainType<A,B>, I >  Mod (const Vec<A,I> &left, const Vec<B,I> &right)
	{
		Vec< typename CompileTime::MainType<A,B>, I >	ret;
		FOR( i, ret )	ret[i] = Mod( left[i], right[i] );
		return ret;
	}

	
	template <typename A, typename B, typename C>
	inline typename CompileTime::MainType<A,B>  SafeMod (const A& left, const B& right, const C& defValue)
	{
		typedef typename CompileTime::MainType<A,B>		main_t;

		return _math_hidden_::ModNearType< A, B, CompileTime::IsInteger< main_t > >::SafeMod( left, right, defValue );
	}

	template <typename A, typename B, usize I, typename C>
	inline Vec< typename CompileTime::MainType<A,B>, I >  SafeMod (const Vec<A,I> &left, const Vec<B,I> &right, const C& defValue)
	{
		Vec< typename CompileTime::MainType<A,B>, I >	ret;
		FOR( i, ret )	ret[i] = SafeMod( left[i], right[i], defValue );
		return ret;
	}



	//
	// Floor, Ceil, Fract, Trunc
	//
	
	template <typename T>
	inline T  Floor (const T& x)
	{
		CompileTime::MustBeFloat<T>();

		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::floor( _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  Floor (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Floor( x[i] );
		return ret;
	}


	template <typename T>
	inline T  Ceil (const T& x)
	{
		CompileTime::MustBeFloat<T>();

		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::ceil( _float_t( x ) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  Ceil (const Vec<T,I>& x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Ceil( x[i] );
		return ret;
	}
	

	// with sign
	template <typename T>
	inline T  Fract (const T& x)
	{
		CompileTime::MustBeFloat<T>();
		//typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		//_float_t	tmp;
		//return (T) ::modf( _float_t( x ), &tmp );	// -1..1
		//return x - Trunc(x);	// -1..1

		return x - Floor(x);	// GLSL style 0..1
	}


	template <typename T, usize I>
	inline Vec<T,I>  Fract (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Fract( x[i] );
		return ret;
	}


	template <typename T>
	inline T  Trunc (const T& x)
	{
		CompileTime::MustBeFloat<T>();

		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;
		return (T) ::trunc( _float_t( x ) );

		//return CopySign( x, Floor( Abs(x) ) );	// if trunc not supported
	}


	template <typename T, usize I>
	inline Vec<T,I>  Trunc (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Trunc( x[i] );
		return ret;
	}


	//
	// ModF
	//

	template <typename T>
	struct ModF_Result
	{
		T	intgr;
		T	fract;

		ModF_Result (T i, T f) : intgr(i), fract(f) {}
	};

	template <typename T, usize I>
	struct ModF_Result< Vec<T,I> >
	{
		Vec<T,I>	intgr;
		Vec<T,I>	fract;
	};


	template <typename T>
	inline ModF_Result<T>  ModF (const T& x)
	{
		CompileTime::MustBeFloat<T>();
		typedef typename _math_hidden_::ToNearFloat<T>  _float_t;

		_float_t	i;
		_float_t	f = ::modf( _float_t( x ), &i );

		return ModF_Result<T>( i, f );
	}


	template <typename T, usize I>
	inline ModF_Result< Vec<T,I> >  ModF (const Vec<T,I> &x)
	{
		ModF_Result< Vec<T,I> >		ret;

		FOR( i, ret )
		{
			auto tmp = ModF( x[i] );

			ret.intgr[i] = tmp.intgr;
			ret.fract[i] = tmp.fract;
		}
		return ret;
	}



	//
	// Round
	//
	
	namespace _math_hidden_
	{
		template <bool ResultIsInteger, bool ValueIsInteger>
		struct _TRound	// int to int, int to float
		{
			template <typename R, typename T>
			static inline R Round (const T& val)
			{
				return R( val );
			}
		};
		
		template <>
		struct _TRound< true, false >	// float to int
		{
			template <typename R, typename T>
			static inline R Round (const T& val)
			{
				typedef typename _math_hidden_::ToNearFloat<T>  _float_t;

				return (R) ::round( _float_t( val ) );
				//return R( val + ( val > T(0) ? T(0.5) : T(-0.5) ) );	// if round not supported
			}
		};
		
		template <>
		struct _TRound< false, false >	// float to float
		{
			template <typename R, typename T>
			static inline R Round (const T& val)
			{
				typedef typename _math_hidden_::ToNearFloat<T>  _float_t;

				return (R) ::round( _float_t( val ) );
				//return (R) Trunc( val + ( val > T(0) ? T(0.5) : T(-0.5) ) );	// if round not supported
			}
		};

		template <typename R, typename T>
		inline R _Round (const T& x)
		{
			return _TRound<	CompileTime::IsInteger<R>, CompileTime::IsInteger<T> >::template Round< R, T >( x );
		}

	}	// _math_hidden_


	template <typename T>
	inline T Round (const T& x)
	{
		CompileTime::MustBeFloat<T>();

		return _math_hidden_::_Round< T, T >( x );
	}


	template <typename T, usize I>
	inline Vec<T,I>  Round (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Round( x[i] );
		return ret;
	}


	template <typename T>
	inline typename CompileTime::NearInt::FromType<T>  RoundToInt (const T& val)
	{
		return _math_hidden_::_Round< typename CompileTime::NearInt::FromType<T>, T >( val );
	}


	template <typename T, usize I>
	inline Vec< typename CompileTime::NearInt::FromType<T>, I >  RoundToInt (const Vec<T,I> &x)
	{
		Vec< typename CompileTime::NearInt::FromType<T>, I >	ret;
		FOR( i, ret )	ret[i] = RoundToInt( x[i] );
		return ret;
	}


	template <typename T>
	inline typename CompileTime::NearUInt::FromType<T>  RoundToUInt (const T& val)
	{
		return _math_hidden_::_Round< typename CompileTime::NearUInt::FromType<T>, T >( val );
	}


	template <typename T, usize I>
	inline Vec< typename CompileTime::NearUInt::FromType<T>, I >  RoundToUInt (const Vec<T,I> &x)
	{
		Vec< typename CompileTime::NearUInt::FromType<T>, I >	ret;
		FOR( i, ret )	ret[i] = RoundToUInt( x[i] );
		return ret;
	}


	template <typename T>
	inline T  RoundTo (const T& x, const T& base)
	{
		return Round( x / base ) * base;
	}

	template <typename T, usize I>
	inline Vec<T,I>  RoundTo (const Vec<T,I> &x, const T& base)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = RoundTo( x[i] );
		return ret;
	}



	//
	// IsNaN, IsInfinity, IsFinite
	//

	template <typename T, usize I>
	inline Vec<bool,I>  IsNaN (const Vec<T,I>& x)
	{
		Vec<bool,I>	ret;
		FOR( i, ret )	ret[i] = IsNaN( x[i] );
		return ret;
	}

	template <typename T, usize I>
	inline Vec<bool,I>  IsInfinity (const Vec<T,I>& x)
	{
		Vec<bool,I>	ret;
		FOR( i, ret )	ret[i] = IsInfinity( x[i] );
		return ret;
	}

	template <typename T, usize I>
	inline Vec<bool,I>  IsFinite (const Vec<T,I>& x)
	{
		Vec<bool,I>	ret;
		FOR( i, ret )	ret[i] = IsFinite( x[i] );
		return ret;
	}



	//
	// Safe Division
	//

	template <typename T>
	inline T SafeDiv (const T& left, const T& right, const T& defVal)
	{
		return IsNotZero(right) ? (left / right) : defVal;
	}

	template <typename T, usize I>
	inline Vec<T,I>  SafeDiv (const Vec<T,I> &left, const Vec<T,I> &right, const T& defVal)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = SafeDiv( left[i], right[i], defVal );
		return ret;
	}
	


	//
	// Int Factorials
	//

	template <typename T>
	inline T  IntFactorial (const T& x)
	{
		ASSERT( x >= T(0) );

		typedef CompileTime::NearUInt::FromType<T>	uint_t;

		const uint_t	count	= RoundToUInt( x );
		T				result  = T(1);

		for (uint_t i = 2; i <= count; ++i) {
			result *= T(i);
		}
		return result;
	}

	template <typename T, usize I>
	inline Vec<T,I>  IntFactorial (const Vec<T,I>& x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = IntFactorial( x[i] );
		return ret;
	}


	template <typename T>
	inline T  IntSuperFactorial (const T& x)
	{
		ASSERT( x >= T(0) );
		
		typedef CompileTime::NearUInt::FromType<T>	uint_t;

		const uint_t	count	= RoundToUInt( x );
		T				result  = T(1);

		for (uint_t i = 1; i < count; ++i) {
			result *= Pow( T(count-i+1), T(i) );
		}
		return result;
	}

	template <typename T, usize I>
	inline Vec<T,I>  IntSuperFactorial (const Vec<T,I>& x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = IntSuperFactorial( x[i] );
		return ret;
	}
	

	template <typename T>
	inline T  IntHyperFactorial (const T& x)
	{
		ASSERT( x >= T(0) );
		
		typedef CompileTime::NearUInt::FromType<T>	uint_t;

		const uint_t	count	= RoundToUInt( x );
		T				result  = T(1);

		for (uint_t i = 2; i <= count; ++i) {
			result *= Pow( T(i), T(i) );
		}
		return result;
	}

	template <typename T, usize I>
	inline Vec<T,I>  IntHyperFactorial (const Vec<T,I>& x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = IntHyperFactorial( x[i] );
		return ret;
	}


	template <typename T>
	inline T  IntDoubleFactorial (const T& x)
	{
		ASSERT( x >= T(0) );
		
		typedef CompileTime::NearUInt::FromType<T>	uint_t;

		const uint_t	val		= RoundToUInt( x );
		const uint_t	count	= val / 2;
		T				result  = T(1);

		for (uint_t i = 0; i < count; ++i) {
			result *= ( T(val) - T(2*i) );
		}
		return result;
	}

	template <typename T, usize I>
	inline Vec<T,I>  IntDoubleFactorial (const Vec<T,I>& x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = IntDoubleFactorial( x[i] );
		return ret;
	}
	

	template <typename T>
	inline T  IntGammaFunction (const T& x)
	{
		return IntFactorial( x - T(1) );
	}

	template <typename T, usize I>
	inline Vec<T,I>  IntGammaFunction (const Vec<T,I>& x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = IntGammaFunction( x[i] );
		return ret;
	}

	
	template <typename T>
	inline T  IntLnGammaFunction (const T& x)
	{
		return Ln( Abs( IntGammaFunction( x ) ) );
	}

	template <typename T, usize I>
	inline Vec<T,I>  IntLnGammaFunction (const Vec<T,I>& x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = IntLnGammaFunction( x[i] );
		return ret;
	}



	//
	// (Float) Factorials
	//
	
	template <typename T>
	inline T  Factorial (const T& x)
	{
		return (T) FastFactorial( double(x) );
	}

	template <typename T, usize I>
	inline Vec<T,I>  Factorial (const Vec<T,I>& x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Factorial( x[i] );
		return ret;
	}


	template <typename T>
	inline T  Gamma (const T& x)
	{
		return (T) FastGammaFunction( double(x) );
	}

	template <typename T, usize I>
	inline Vec<T,I>  Gamma (const Vec<T,I>& x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Gamma( x[i] );
		return ret;
	}


	template <typename T>
	inline T LnGamma (const T& x)
	{
		return (T) FastLnGammaFunction( double(x) );
	}

	template <typename T, usize I>
	inline Vec<T,I>  LnGamma (const Vec<T,I>& x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = LnGammaFunction( x[i] );
		return ret;
	}



	//
	// Safe Left Bit Shift
	//
	
	template <typename T, typename B>
	inline T  SafeLeftBitShift (const T& x, const B& shift)
	{
		CompileTime::MustBeInteger<T>();
		CompileTime::MustBeInteger<B>();
		
		ASSUME( shift >= 0 );

		return x << ( uint(shift) & ( CompileTime::SizeOf<T>::bits - 1 ) );
	}
	
	template <typename T, usize I, typename B>
	inline Vec<T,I>  SafeLeftBitShift (const Vec<T,I> &x, const Vec<B,I>& shift)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = SafeLeftBitShift( x[i], shift[i] );
		return ret;
	}
	
	template <typename T, usize I, typename B>
	inline Vec<T,I>  SafeLeftBitShift (const Vec<T,I> &x, const B& shift)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = SafeLeftBitShift( x[i], shift );
		return ret;
	}
	


	//
	// Safe Right Bit Shift
	//

	template <typename T, typename B>
	inline T  SafeRightBitShift (const T& x, const B& shift)
	{
		CompileTime::MustBeInteger<T>();
		CompileTime::MustBeInteger<B>();
		
		ASSUME( shift >= 0 );

		return x >> ( uint(shift) & (CompileTime::SizeOf<T>::bits - 1) );
	}
	
	template <typename T, usize I, typename B>
	inline Vec<T,I>  SafeRightBitShift (const Vec<T,I> &x, const Vec<B,I>& shift)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = SafeRightBitShift( x[i], shift[i] );
		return ret;
	}
	
	template <typename T, usize I, typename B>
	inline Vec<T,I>  SafeRightBitShift (const Vec<T,I> &x, const B& shift)
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
		inline T _BitRotateLeft (T value, int shift)
		{
			const uint	mask = GetMaskForType( value );

			shift &= mask;
			return (value << shift) | (value >> ( uint(-shift) & mask ));
		}
		
		template <typename T>
		inline T _BitRotateRight (T value, int shift)
		{
			const uint	mask = GetMaskForType( value );

			shift &= mask;
			return (value >> shift) | (value << ( uint(-shift) & mask ));
		}
	}

	template <typename T, typename B>
	inline T  BitRotateLeft (const T& x, const B& shift)
	{
		CompileTime::MustBeInteger<T>();
		CompileTime::MustBeInteger<B>();
		
		ASSUME( shift >= 0 );

		typedef CompileTime::NearUInt::FromType<T>	Unsigned_t;

		return _math_hidden_::_BitRotateLeft( Unsigned_t(x), int(shift) );
	}

	template <typename T, usize I, typename B>
	inline T  BitRotateLeft (const Vec<T,I> &x, const Vec<B,I>& shift)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = BitRotateLeft( x[i], shift[i] );
		return ret;
	}

	template <typename T, usize I, typename B>
	inline T  BitRotateLeft (const Vec<T,I> &x, const B& shift)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = BitRotateLeft( x[i], shift );
		return ret;
	}


	template <typename T, typename B>
	inline T  BitRotateRight (const T& x, const B& shift)
	{
		CompileTime::MustBeInteger<T>();
		CompileTime::MustBeInteger<B>();
		
		ASSUME( shift >= 0 );

		typedef CompileTime::NearUInt::FromType<T>	Unsigned_t;

		return _math_hidden_::_BitRotateRight( Unsigned_t(x), int(shift) );
	}

	template <typename T, usize I, typename B>
	inline T  BitRotateRight (const Vec<T,I> &x, const Vec<B,I>& shift)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = BitRotateLeft( x[i], shift[i] );
		return ret;
	}

	template <typename T, usize I, typename B>
	inline T  BitRotateRight (const Vec<T,I> &x, const B& shift)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = BitRotateLeft( x[i], shift );
		return ret;
	}

	

	//
	// To Bit
	//

	template <typename T, typename B>
	inline T  ToBit (const B bitIndex)
	{
		return SafeLeftBitShift( T(1), bitIndex );
	}

	template <typename T, usize I, typename B>
	inline Vec<T,I>  ToBit (const Vec<B,I> &bitIndex)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ToBit<T>( bitIndex[i] );
		return ret;
	}

	
	//
	// To Mask
	//

	template <typename T, typename B>
	inline T  ToMask (B lastBitIndex)
	{
		CompileTime::MustBeInteger<T>();
		CompileTime::MustBeInteger<B>();

		// Test:
		// (1 << lastBitIndex-1) & ToMask( lastBitIndex ) != 0

		return	lastBitIndex < 0 ? T(0) :
					lastBitIndex < CompileTime::SizeOf<T>::bits ?
						(T(1) << lastBitIndex) - 1 :
						T(-1);
	}

	template <typename T, usize I, typename B>
	inline Vec<T,I>  ToMask (const Vec<B,I> bitIndex)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ToMask<T>( bitIndex[i] );
		return ret;
	}


	template <typename T, typename B>
	inline T  ToMask (const B first, const B last)
	{
		ASSERT( first <= last );
		return ToMask<T>( first ) ^ ToMask<T>( last );
	}

	template <typename T, usize I, typename B>
	inline Vec<T,I>  ToMask (const Vec<B,I> first, const Vec<B,I> last)
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
		CompileTime::MustBeInteger<T>();
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
	inline uint BitScanReverse (const T& x)
	{
		return IntLog2( x );
	}


	template <typename T>
	inline uint BitScanForward (const T& x)
	{
		CompileTime::MustBeInteger<T>();

		typedef typename CompileTime::Similar<T>::Unsigned	utype;

		const utype	value = ReferenceCast<utype>(x);

		uint bits = _math_hidden_::_RecursiveBitScanForward< CompileTime::SizeOf<utype>::bits-1 >::Get( value );

		return bits > 0 ? CompileTime::SizeOf<utype>::bits - bits : -1;
	}

	template <typename T, usize I>
	inline Vec<uint,I> BitScanForward (const Vec<T,I> &x)
	{
		Vec<uint,I>		ret;
		FOR( i, x )		ret[i] = BitScanForward( x[i] );
		return ret;
	}



	//
	// IsOdd, IsEven
	//

	template <typename T>
	inline bool  IsOdd (const T& x)
	{
		CompileTime::MustBeInteger<T>();
		return ( x & T(1) ) == T(1);
	}


	template <typename T, usize I>
	inline Vec<bool,I>  IsOdd (const Vec<T,I> &x)
	{
		Vec<bool,I>		ret;
		FOR( i, ret )	ret[i] = IsOdd( x[i] );
		return ret;
	}


	template <typename T>
	inline bool  IsEven (const T& x)
	{
		CompileTime::MustBeInteger<T>();
		return ( x & T(1) ) == T(0);
	}

	
	template <typename T, usize I>
	inline Vec<bool,I>  IsEven (const Vec<T,I> &x)
	{
		Vec<bool,I>		ret;
		FOR( i, ret )	ret[i] = IsEven( x[i] );
		return ret;
	}



	//
	// ToOdd, ToEven
	//

	template <typename T>
	inline T  ToOdd (const T &x)
	{
		CompileTime::MustBeInteger<T>();
		return ( x | T(1) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  ToOdd (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ToOdd( x[i] );
		return ret;
	}


	template <typename T>
	inline T  ToEven (const T &x)
	{
		CompileTime::MustBeInteger<T>();
		return ( x & ~T(1) );
	}


	template <typename T, usize I>
	inline Vec<T,I>  ToEven (const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = ToEven( x[i] );
		return ret;
	}
	


	//
	// Step, SmoothStep
	//

	template <typename T>
	inline T  Step (const T& edge, const T& x)
	{
		return x < edge ? T(0) : T(1);
	}

	template <typename T, usize I>
	inline Vec<T,I>  Step (const Vec<T,I> &edge, const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Step( edge[i], x[i] );
		return ret;
	}

	template <typename T, usize I>
	inline Vec<T,I>  Step (const T &edge, const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = Step( edge, x[i] );
		return ret;
	}


	template <typename T>
	inline T  SmoothStep (const T& edge0, const T& edge1, const T& x)
	{
		ASSERT( edge0 < edge1 );

		T t = Clamp( (x - edge0) / (edge1 - edge0), 0.0, 1.0 );
		return t * t * (T(3) - T(2) * t);
	}

	template <typename T, usize I>
	inline Vec<T,I>  SmoothStep (const Vec<T,I> &edge0, const Vec<T,I> &edge1, const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = SmoothStep( edge0[i], edge1[i], x[i] );
		return ret;
	}

	template <typename T, usize I>
	inline Vec<T,I>  SmoothStep (const T &edge0, const T &edge1, const Vec<T,I> &x)
	{
		Vec<T,I>		ret;
		FOR( i, ret )	ret[i] = SmoothStep( edge0, edge1, x[i] );
		return ret;
	}


}	// GXMath
}	// GX_STL
