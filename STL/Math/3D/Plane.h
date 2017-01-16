// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "AxisAlignedBox.h"

namespace GX_STL
{
namespace GXMath
{
	/*
	//
	// Plane Side Type
	//

	namespace e_plane_side
	{
		enum	type
		{
			NONE	= 0,
			POSITIVE,
			NEGATIVE,
			BOTH,
		};
	}



	//
	// Plane
	//

	template <typename T>
	struct TPlane : public CompileTime::CopyQualifiers< T >
	{
	private:
		Vec<T,3>	_vNormal;
		T			_tDist;

	public:
		TPlane();
		TPlane(const Vec<T,3> &normal, const T& dist);
		TPlane(const Vec<T,3> &p0, const Vec<T,3> &p1, const Vec<T,3> &p2);

		void Set(const Vec<T,3> &normal, const T& dist);
		void Set(const Vec<T,3> &p0, const Vec<T,3> &p1, const Vec<T,3> &p2);

		e_plane_side::type	Intersect(const Vec<T,3> &point) const;
		e_plane_side::type	Intersect(const TAABBox<T> &box) const;
		e_plane_side::type	Intersect(const Vec<T,3> &center, const Vec<T,3> &halfextent) const;
		
		Vec<T,3>		 &	Normal()			{ return _vNormal; }
		Vec<T,3> const &	Normal()	const	{ return _vNormal; }
		T		&			Distance()			{ return _tDist; }
		T const &			Distance()	const	{ return _tDist; }

		TPlane<T> &	Normalize();
		T			Distance(const Vec<T,3> &point) const;
		Vec<T,3>	Project(const Vec<T,3> &point) const;


		bool	operator == (const TPlane<T> &right) const;
		bool	operator != (const TPlane<T> &right) const;

		template <typename T2>
		TPlane<T2>	Convert() const;

		String		ToString() const;
	};


	
	template <typename T>
	inline TPlane<T>::TPlane(): _vNormal(), _tDist(0)
	{}

	
	template <typename T>
	inline TPlane<T>::TPlane(const Vec<T,3> &normal, const T& dist): _vNormal(normal), _tDist(dist)
	{}

	
	template <typename T>
	inline TPlane<T>::TPlane(const Vec<T,3> &p0, const Vec<T,3> &p1, const Vec<T,3> &p2): _vNormal(), _tDist(0)
	{
		Set( p0, p1, p2 );
	}

	
	template <typename T>
	inline void TPlane<T>::Set(const Vec<T,3> &normal, const T& dist)
	{
		_vNormal	= normal;
		_tDist		= dist;
	}

	
	template <typename T>
	inline void TPlane<T>::Set(const Vec<T,3> &p0, const Vec<T,3> &p1, const Vec<T,3> &p2)
	{
		Vec<T,3>	s_edge1 = p1 - p0,
					s_edge2 = p2 - p0;

		_vNormal = Cross( s_edge1, s_edge2 );
		_vNormal.Normalize();
		_tDist = -_vNormal.Dot( p0 );
	}

	
	template <typename T>
	inline e_plane_side::type TPlane<T>::Intersect(const Vec<T,3> &point) const
	{
		const T	d = Distance( point );

		if ( d < T(0) )	return e_plane_side::NEGATIVE;
		if ( d > T(0) )	return e_plane_side::POSITIVE;
		return e_plane_side::NONE;
	}

	
	template <typename T>
	inline e_plane_side::type TPlane<T>::Intersect(const TAABBox<T> &box) const
	{
		return Intersect( box.Center(), box.HalfExtent() );
	}

	
	template <typename T>
	inline e_plane_side::type TPlane<T>::Intersect(const Vec<T,3> &center, const Vec<T,3> &halfextent) const
	{
		const T	d			= Distance( center );
		const T	max_abs_d	= _vNormal.DotAbs( halfextent );

		if ( d < -max_abs_d )	return e_plane_side::NEGATIVE;
		if ( d >  max_abs_d )	return e_plane_side::POSITIVE;
		return e_plane_side::BOTH;
	}
		
	
	template <typename T>
	inline TPlane<T> & TPlane<T>::Normalize()
	{
		const T	len = _vNormal.Length();

		if ( IsNotZero( len ) )
		{
			const T	inv_len = T(1) / len;
			_vNormal *= inv_len;
			_tDist	 *= inv_len;
		}

		return *this;
	}

	
	template <typename T>
	inline T TPlane<T>::Distance(const Vec<T,3> &point) const
	{
		return _vNormal.Dot( point ) + _tDist;
	}

	
	template <typename T>
	inline Vec<T,3> TPlane<T>::Project(const Vec<T,3> &point) const
	{
		Matrix<T,3,3>		s_matrix;

		s_matrix(0,0) = T(1) - _vNormal.x * _vNormal.x;
		s_matrix(0,1) =		 - _vNormal.x * _vNormal.y;
		s_matrix(0,2) =		 - _vNormal.x * _vNormal.z;
		s_matrix(1,0) =		 - _vNormal.y * _vNormal.x;
		s_matrix(1,1) = T(1) - _vNormal.y * _vNormal.y;
		s_matrix(1,2) =		 - _vNormal.y * _vNormal.z;
		s_matrix(2,0) =		 - _vNormal.z * _vNormal.x;
		s_matrix(2,1) =		 - _vNormal.z * _vNormal.y;
		s_matrix(2,2) = T(1) - _vNormal.z * _vNormal.z;

		return s_matrix * point;
	}


	template <typename T>
	inline bool TPlane<T>::operator == (const TPlane<T> &right) const
	{
		return Equals( _tDist, right._tDist ) and _vNormal == right._vNormal;
	}

	
	template <typename T>
	inline bool TPlane<T>::operator != (const TPlane<T> &right) const
	{
		return not ( *this == right );
	}
	
		
	template <typename T>
	template <typename T2>
	inline TPlane<T2> TPlane<T>::Convert() const
	{
		return TPlane<T2>( _vNormal.Convert<T2>(), T2(_tDist) );
	}

		
	template <typename T>
	inline String TPlane<T>::ToString() const
	{
		return (String("norm: (") << _vNormal.ToString << "), dist: " << _tDist );
	}
	*/

}	// GXMath
}	// GX_STL
