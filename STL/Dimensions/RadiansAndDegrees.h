// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Helpers.h"
#include "../CompileTime/TemplateMath.h"
#include "../Math/MathTypes.h"

namespace GX_STL
{
namespace GXMath
{

	template <typename T>	struct Radians;
	template <typename T>	struct Degrees;

	template <typename T, usize I>	using RadiansVec = Vec< Radians<T>, I >;
	template <typename T, usize I>	using DegreesVec = Vec< Degrees<T>, I >;



	//
	// Radians
	//

	template <typename T>
	struct Radians : public CompileTime::CopyQualifiers< T >
	{
	// types
	public:
		typedef Radians<T>		Self;
		typedef T				value_t;


	// variables
	private:
		T	_value;


	// methods
	public:
		Radians (GX_DEFCTOR) : _value(0) {}

		explicit
		Radians (T value) : _value(value) {}

		template <typename B>
		explicit
		Radians (Degrees<B> deg);

		operator T () const				{ return _value; }

		T &			ref ()				{ return _value; }
		T const &	ref ()		const	{ return _value; }

		Degrees<T> ToDegrees () const;
		
		template <typename B>
		Degrees<B> ToDegrees () const;

		template <typename B>
		Radians<B> To () const;

		Self	ToInterval (Self minValue, Self maxValue) const;

		String	ToString ()		const			{ return String() << _value << " rad"; }

		_GX_DIM_ALL_FLOAT_OPERATORS_SELF( _value );

		static Self	Pi (T scale)				{ return Self( GXMath::Pi<T> * scale ); }	// Pi * x
		static Self	Pi ()						{ return Self( GXMath::Pi<T> ); }			// Pi
		static Self ReciporalPi ()				{ return Self( GXMath::ReciporalPi<T> ); }	// 1/Pi
		static Self	DoublePi ()					{ return Self( Pi( T(2) ) ); }				// 2*Pi
		static Self	HalfPi ()					{ return Self( Pi( T(0.5) ) ); }			// Pi/2

		static Self FromArcminutes (T value);
		static Self FromArcseconds (T value);
		
		static Self From (T deg, T arcmin = T(0), T arcsec = T(0));
	};



	//
	// Degrees
	//

	template <typename T>
	struct Degrees : public CompileTime::CopyQualifiers< T >
	{
	// types
	public:
		typedef Degrees<T>		Self;
		typedef T				value_t;


	// variables
	private:
		T	_value;


	// methods
	public:
		Degrees (GX_DEFCTOR) : _value(0) {}

		explicit
		Degrees (T value) : _value(value) {}

		template <typename B>
		explicit
		Degrees (Radians<B> rad);

		operator T () const				{ return _value; }

		T &			ref ()				{ return _value; }
		T const &	ref ()		const	{ return _value; }

		Radians<T> ToRadians () const;

		template <typename B>
		Radians<B> ToRadians () const;

		template <typename B>
		Degrees<B> To () const;
		
		Self	ToInterval (Self minValue, Self maxValue) const;
		
		String	ToString ()		const			{ return String() << _value << " deg"; }

		_GX_DIM_ALL_FLOAT_OPERATORS_SELF( _value );
		
		static Self	Pi (T scale)				{ return Self( T(180) * scale ); }				// Pi * x
		static Self	Pi ()						{ return Self( T(180) ); }						// Pi
		static Self ReciporalPi ()				{ return Self( T(0.0055555555555555558) ); }	// 1/Pi
		static Self	DoublePi ()					{ return Self( Pi( T(2) ) ); }					// 2*Pi
		static Self	HalfPi ()					{ return Self( Pi( T(0.5) ) ); }				// Pi/2
		
		static Self FromArcminutes (T value)	{ return Self( value / T(60.0) ); }				// T(0.01666667)
		static Self FromArcseconds (T value)	{ return Self( value / T(3600.0) ); }			// T(0.0002777778)

		static Self From (T deg, T arcmin = T(0), T arcsec = T(0));
	};

	
	
//--------------------------------- Radians --------------------------------//

	template <typename T>
	template <typename B>
	inline Radians<T>::Radians (Degrees<B> deg) :
		_value( T((B)deg) * (T)Pi() * T((B)deg.ReciporalPi()) )
	{
	}
		
	template <typename T>
	inline Degrees<T> Radians<T>::ToDegrees () const
	{
		return Degrees<T>( *this );
	}
		
	template <typename T>
	template <typename B>
	inline Degrees<B> Radians<T>::ToDegrees () const
	{
		return Degrees<B>( *this );
	}
	
	template <typename T>
	template <typename B>
	inline Radians<B> Radians<T>::To () const
	{
		return Radians<B>( B(_value) );
	}
	
	template <typename T>
	inline Radians<T>  Radians<T>::ToInterval (Self minValue, Self maxValue) const
	{
		// 0..+2Pi, 0..+Pi, -Pi..0, -2Pi..0, and other
		return (Self) GXMath::Wrap( _value, (T)minValue, (T)maxValue );
	}
	
	template <typename T>
	inline Radians<T>  Radians<T>::FromArcminutes (T value)
	{
		return Degrees<T>::FromArcminutes( value ).ToRadians();
	}
	
	template <typename T>
	inline Radians<T>  Radians<T>::FromArcseconds (T value)
	{
		return Degrees<T>::FromArcseconds( value ).ToRadians();
	}
	
	template <typename T>
	inline Radians<T>  Radians<T>::From (T deg, T arcmin, T arcsec)
	{
		return Degrees<T>()::From( degm arcmin, arcsec ).ToRadians();
	}
	

//--------------------------------- Degrees --------------------------------//

	template <typename T>
	template <typename B>
	inline Degrees<T>::Degrees (Radians<B> rad) :
		_value( T((B)rad) * (T)Pi() * T((B)rad.ReciporalPi()) )
	{
	}
		
	template <typename T>
	inline Radians<T> Degrees<T>::ToRadians () const
	{
		return Radians<T>( *this );
	}
	
	template <typename T>
	template <typename B>
	inline Radians<B> Degrees<T>::ToRadians () const
	{
		return Radians<B>( *this );
	}
	
	template <typename T>
	template <typename B>
	inline Degrees<B> Degrees<T>::To () const
	{
		return Degrees<B>( B(_value) );
	}
	
	template <typename T>
	inline Degrees<T>  Degrees<T>::ToInterval (Self minValue, Self maxValue) const
	{
		// 0..+2Pi, 0..+Pi, -Pi..0, -2Pi..0, and other
		return (Self) GXMath::Wrap( _value, (T)minValue, (T)maxValue );
	}
	
	template <typename T>
	inline Degrees<T>  Degrees<T>::From (T deg, T arcmin, T arcsec)
	{
		return Degrees<T>( deg + (arcmin / T(60.0)) + (arcsec / T(3600.0)) );
	}

}	// GXMath

namespace GXTypes
{

	GX_TEMPLATE_TYPE_INFO_WITH_INNER_TYPE( GXMath::Radians, "Radians" );
	GX_TEMPLATE_TYPE_INFO_WITH_INNER_TYPE( GXMath::Degrees, "Degrees" );
	
	
	template <typename T>
	struct Hash< GXMath::Radians<T> > : private Hash<T>
	{
		typedef GXMath::Radians<T>			key_t;
		typedef Hash<T>						base_t;
		typedef typename base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x.ref() );
		}
	};
	
	
	template <typename T>
	struct Hash< GXMath::Degrees<T> > : private Hash<T>
	{
		typedef GXMath::Degrees<T>			key_t;
		typedef Hash<T>						base_t;
		typedef typename base_t::result_t	result_t;

		result_t operator () (const key_t &x) const
		{
			return base_t::operator ()( x );
		}
	};

}	// GXTypes
}	// GX_STL
