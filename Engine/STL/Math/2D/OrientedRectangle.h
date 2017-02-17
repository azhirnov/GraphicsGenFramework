// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "MathTypes2D.h"
#include "Rectangle.h"
#include "Line2.h"

namespace GX_STL
{
namespace GXMath
{

	//
	// Oriented Bounding Rectangle
	//

	template <typename T>
	struct OrientedRectangle : public CompileTime::CopyQualifiers< T >
	{
	// types
	public:
		typedef OrientedRectangle<T>	Self;
		typedef T						value_t;
		typedef Vec<T,2>				vec2_t;
		typedef Line2<T>				line_t;


	// variables
	private:
		vec2_t						_center;
		vec2_t						_extent;
		StaticArray< vec2_t, 2 >	_axis;


	// methods
	public:
		OrientedRectangle (GX_DEFCTOR) {}

		OrientedRectangle (const vec2_t &leftBottom, const vec2_t &leftTop,
							const vec2_t &rightTop, const vec2_t &rightBottom);

		OrientedRectangle (Buffer<const vec2_t> points);

		Rectangle<T>	GetAxisAlignedRectangle () const;
		
		vec2_t const&	Center () const			{ return _center; }
		vec2_t const&	Extents () const		{ return _extent; }

		vec2_t const&	Axis1 () const			{ return _axis[0]; }
		vec2_t const&	Axis2 () const			{ return _axis[1]; }

		T				MaxRadius () const;		// radius of circumscribed circle
		T				MinRadius () const;		// radius of inscribed circle

		vec2_t			Corner (usize index) const;

		bool Intersects (const Self &other) const;

		template <usize C, usize R>
		Self Transform (const Matrix<T,C,R> &mat) const;

		template <typename B>
		OrientedRectangle<B> To () const;


	private:
		template <typename B>
		static bool _ApprGaussian2Fit (Buffer<const vec2_t> points, OUT OrientedRectangle<B> &box);
		
		template <typename B>
		static void _SymmetricEigensolver2x2 (B a00, B a01, B a11, int sortType,
											  OUT Vec<B,2> &eval, OUT StaticArray<Vec<B,2>, 2> &evec);
		
		template <typename B>
		static void _FromPoints (Buffer<const vec2_t> points, OUT Self &box);
	};
	
	
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	inline OrientedRectangle<T>::OrientedRectangle (
						const vec2_t &leftBottom, const vec2_t &leftTop,
						const vec2_t &rightTop, const vec2_t &rightBottom)
	{
		_center = (leftBottom + leftTop + rightTop + rightBottom) * T(0.25);

		const vec2_t	x_vec = MiddleValue( rightTop, rightBottom ) - MiddleValue( leftBottom, leftTop );
		const vec2_t	y_vec = MiddleValue( leftTop, rightTop ) - MiddleValue( leftBottom, rightBottom );
		const T			x_len = xVec.Length();
		const T			y_len = yVec.Length();

		_axis[0] = x_vec.Normalized();
		_axis[1] = y_vec.Normalized();

		if (x_len > y_len)
			axis[1] = line_t( axis[0] ).LeftNormal();
		else
			axis[0] = line_t( axis[1] ).RightNormal();
		
		_extent.x = x_len * T(0.5);
		_extent.y = y_len * T(0.5);
	}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	inline OrientedRectangle<T>::OrientedRectangle (Buffer<const vec2_t> points)
	{
		// float to double
		typedef typename CompileTime::NearFloat::FromSize<
			CompileTime::Min< uint, sizeof(T)+1, sizeof(double) >() >	precised_t;

		*this = _FromPoints< precised_t >( points );
	}
	
/*
=================================================
	GetAxisAlignedRectangle
=================================================
*/
	template <typename T>
	inline Rectangle<T>  OrientedRectangle<T>::GetAxisAlignedRectangle () const
	{
		return Rectangle<T>( StaticArrayBuilder::Create( Corner(0), Corner(1), Corner(2), Corner(3) ) );
	}

/*
=================================================
	MaxRadius
=================================================
*/
	template <typename T>
	inline T  OrientedRectangle<T>::MaxRadius () const
	{
		return Max( _extent.x, _extent.y );
	}
	
/*
=================================================
	MinRadius
=================================================
*/
	template <typename T>
	inline T  OrientedRectangle<T>::MinRadius () const
	{
		return Min( _extent.x, _extent.y );
	}
	
/*
=================================================
	Corner
=================================================
*/
	template <typename T>
	inline Vec<T,2>  OrientedRectangle<T>::Corner (usize index) const
	{
		assert(index < 4);
		return _center +
			T(index & 1 ? 1 : -1) * (_extent[0] * _axis[0]) +
			T(index & 2 ? 1 : -1) * (_extent[1] * _axis[1]);
	}
	
/*
=================================================
	Intersects
=================================================
*/
	template <typename T>
	inline bool  OrientedRectangle<T>::Intersects (const Self &other) const
	{
		Buffer<const vec2_t>	aa0 = this->_axis;
		Buffer<const vec2_t>	aa1 = other._axis;

		vec2_t const&	E0 = this->_extent;
		vec2_t const&	E1 = other._extent;

		vec2_t const	D  = other._center - this->_center;

		T	abs_aa0_d_aa1[2][2];
		T	r_sum;

		abs_aa0_d_aa1[0][0] = Abs( Dot( aa0[0], aa1[0] ) );
		abs_aa0_d_aa1[0][1] = Abs( Dot( aa0[0], aa1[1] ) );

		r_sum = E0[0] + E1[0] * abs_aa0_d_aa1[0][0] + E1[1] * abs_aa0_d_aa1[0][1];

		if ( Abs( Dot( aa0[0], D ) ) > r_sum )
			return false;

		abs_aa0_d_aa1[1][0] = Abs( Dot( aa0[1], aa1[0] ) );
		abs_aa0_d_aa1[1][1] = Abs( Dot( aa0[1], aa1[1] ) );

		r_sum = E0[1] + E1[0] * abs_aa0_d_aa1[1][0] + E1[1] * abs_aa0_d_aa1[1][1];

		if ( Abs( Dot( aa0[1], D ) ) > r_sum )
			return false;

		r_sum = E1[0] + E0[0] * abs_aa0_d_aa1[0][0] + E0[1] * abs_aa0_d_aa1[1][0];

		if ( Abs( Dot( aa1[0], D ) ) > r_sum )
			return false;

		r_sum = E1[1] + E0[0] * abs_aa0_d_aa1[0][1] + E0[1] * abs_aa0_d_aa1[1][1];

		if ( Abs( Dot( aa1[1], D ) ) > r_sum )
			return false;

		return true;
	}
	
/*
=================================================
	Transform
=================================================
*/
	template <typename T>
	template <usize C, usize R>
	inline OrientedRectangle<T>  OrientedRectangle<T>::Transform (const Matrix<T,C,R> &mat) const
	{
		return Self( StaticArrayBuilder::Create(
						(mat * Corner(0)).xy(), (mat * Corner(1)).xy(),
						(mat * Corner(2)).xy(), (mat * Corner(3)).xy() ) );
	}
	
/*
=================================================
	To
=================================================
*/
	template <typename T>
	template <typename B>
	inline OrientedRectangle<B>  OrientedRectangle<T>::To () const
	{
		OrientedRectangle<B> result;

		result._center	= _center.To< Vec<B,2> >();
		result._extent	= _extent.To< Vec<B,2> >();
		result._axis[0]	= _axis[0].To< Vec<B,2> >();
		result._axis[1]	= _axis[1].To< Vec<B,2> >();

		return result;
	}
	
/*
=================================================
	_FromPoints
=================================================
*/
	template <typename T>
	template <typename B>
	inline void OrientedRectangle<T>::_FromPoints (Buffer<const vec2_t> points, OUT Self &result)
	{
		OrientedRectangle<B> box;

		if ( _ApprGaussian2Fit<T>( points, box ) )
		{
			Vec<B,2>	diff = points[0].To< Vec<B,2> >() - box._center;
			Vec<B,2>	pmin( diff.Dot( box._axis[0] ), diff.Dot( box._axis[1] ) );
			Vec<B,2>	pmax = pmin;

			FORv( i, 1, points )
			{
				diff = points[i].To< Vec<B,2> >() - box._center;

				for (int j = 0; j < 2; ++j)
				{
					const B	dot = diff.Dot( box._axis[j] );

					if ( dot < pmin[j] )
					{
						pmin[j] = dot;
					}
					else
					if ( dot > pmax[j] )
					{
						pmax[j] = dot;
					}
				}
			}

			for (int j = 0; j < 2; ++j)
			{
				box._center		+= B(0.5) * ( pmin[j] + pmax[j] ) * box._axis[j];
				box._extent[j]	 = B(0.5) * ( pmax[j] - pmin[j] );
			}
		}

		result = box.To< Self >();
	}
	
/*
=================================================
	_ApprGaussian2Fit
=================================================
*/
	template <typename T>
	template <typename B>
	inline bool OrientedRectangle<T>::_ApprGaussian2Fit (Buffer<const vec2_t> points, OUT OrientedRectangle<B> &box)
	{
		if ( points.Count() >= 2 )
		{
			Vec<B,2>	mean;

			FOR( i, points ) {
				mean += points[i].To< Vec<B,2> >();
			}

			const B	inv_size = B(1) / B(points.Count());

			mean *= inv_size;

			B covar00 = B(0);
			B covar01 = B(0);
			B covar11 = B(0);

			FOR( i, points )
			{
				const Vec<B,2>	diff = points[i].To< Vec<B,2> >() - mean;

				covar00 += diff.x * diff.x;
				covar01 += diff.x * diff.y;
				covar11 += diff.y * diff.y;
			}

			covar00 *= inv_size;
			covar01 *= inv_size;
			covar11 *= inv_size;

			Vec<B,2>					eval;
			StaticArray<Vec<B,2>, 2>	evec;

			SymmetricEigensolver2x2( covar00, covar01, covar11, +1, eval, evec );

			box._center		= mean;
			box._axis[0]	= evec[0];
			box._axis[1]	= evec[1];
			box._extent		= eval;
			return true;
		}
		
		assert(false);
		return false;
	}
	
/*
=================================================
	_SymmetricEigensolver2x2
=================================================
*/
	template <typename T>
	template <typename B>
	inline void OrientedRectangle<T>::_SymmetricEigensolver2x2 (B a00, B a01, B a11, int sortType,
										  OUT Vec<B,2> &eval, OUT StaticArray<Vec<B,2>, 2> &evec)
	{
		B		c2			= B(0.5) * (a00 - a11);
		B		s2			= a01;
		const B	maxAbsComp	= Max( Abs(c2), Abs(s2) );

		if ( maxAbsComp > B(0) )
		{
			c2 /= maxAbsComp;  // in [-1,1]
			s2 /= maxAbsComp;  // in [-1,1]

			const B	length = Sqrt( c2 * c2 + s2 * s2 );

			c2 /= length;
			s2 /= length;

			if ( c2 > B(0) )
			{
				c2 = -c2;
				s2 = -s2;
			}
		}
		else
		{
			c2 = B(-1);
			s2 = B(0);
		}

		const B	s = Sqrt( B(0.5) * (B(1) - c2) );  // >= 1/sqrt(2)
		const B	c = B(0.5) * s2 / s;

		Vec<B,2>	diagonal;
		B			csqr = c * c;
		B			ssqr = s * s;
		B			mid  = s2 * a01;

		diagonal.x = csqr * a00 + mid + ssqr * a11;
		diagonal.y = csqr * a11 - mid + ssqr * a00;

		if ( sortType == 0 or
			 sortType * diagonal.x <= sortType * diagonal.y )
		{
			eval		= diagonal;
			evec[0].x	= c;
			evec[0].y	= s;
			evec[1].x	= -s;
			evec[1].y	= c;
		}
		else
		{
			eval		= diagonal.yx();
			evec[0].x	= s;
			evec[0].y	= -c;
			evec[1].x	= c;
			evec[1].y	= s;
		}
	}


}	// GXMath

namespace GXTypes
{
	
	template <typename T>
	struct Hash< GXMath::OrientedRectangle<T> > :
		private Hash< typename GXMath::OrientedRectangle<T>::vec_t >
	{
		typedef GXMath::OrientedRectangle<T>							key_t;
		typedef Hash< typename GXMath::OrientedRectangle<T>::vec_t >	base_t;
		typedef typename base_t::result_t								result_t;

		result_t operator () (const key_t &x) const
		{
			return	base_t::operator ()( x.Center() )  +
					base_t::operator ()( x.Extents() ) +
					base_t::operator ()( x.Axis1() )   +
					base_t::operator ()( x.Axis2() );
		}
	};

}	// GXTypes
}	// GX_STL
