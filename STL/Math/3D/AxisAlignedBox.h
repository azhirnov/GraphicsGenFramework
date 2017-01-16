// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Sphere.h"

namespace GX_STL
{
namespace GXMath
{
	/*
	//
	// AABB Corner
	//

	namespace e_corner
	{
		enum	type
		{
			FAR_LEFT_BOTTOM		= 0,
			FAR_LEFT_TOP		= 1,
			FAR_RIGHT_TOP		= 2,
			FAR_RIGHT_BOTTOM	= 3,
			NEAR_RIGHT_TOP		= 4,
			NEAR_LEFT_TOP		= 5,
			NEAR_LEFT_BOTTOM	= 6,
			NEAR_RIGHT_BOTTOM	= 7,
		};
	}



	//
	// Axis Aligned Bounding Box
	//

	template <typename T>
	struct TAABBox : public CompileTime::CopyQualifiers< T >
	{
	public:
		typedef TAABBox<T>		Self;

	private:
		Vec<T,3>	_vMin,
					_vMax;

	public:
		TAABBox();
		TAABBox(const TAABBox<T> &box);
		TAABBox(const Vec<T,3> &vMin, const Vec<T,3> &vMax);

		Vec<T,3>		&	Max()				{ return _vMax; }
		Vec<T,3>		&	Min()				{ return _vMin; }

		Vec<T,3> const &	Max()		const	{ return _vMax; }
		Vec<T,3> const &	Min()		const	{ return _vMin; }

		Vec<T,3> const		Center()	const	{ return (_vMin + _vMax) * T(0.5); }
		Vec<T,3> const		Extent()	const	{ return _vMax - _vMin; }
		Vec<T,3> const		HalfExtent()const	{ return Extent() * T(0.5); }
		bool				Empty()		const	{ return All( _vMax == _vMin ); }

		T		*			ptr()				{ return _vMin.ptr(); }
		T const	*			ptr()		const	{ return _vMin.ptr(); }


		bool	operator == (const TAABBox<T> &right) const;
		bool	operator != (const TAABBox<T> &right) const;

		Self &	operator += (const Vec<T,3> &right);
		Self &	operator += (const TAABBox<T> &right);

		Self &	operator *= (const Matrix<T,4,4> &right);
		Self &	operator *= (const Matrix<T,3,3> &right);

		Self	operator +  (const Vec<T,3> &right) const;
		Self	operator +  (const TAABBox<T> &right) const;

		Self	operator *  (const Matrix<T,4,4> &right) const;
		Self	operator *  (const Matrix<T,3,3> &right) const;


		T		Volume() const;
		T		MinRadius() const;
		T		MaxRadius() const;
		
		void	Add(const Vec<T,3> &vPoint);
		void	Add(const TAABBox<T> &sBox);
		
		void	Repair();

		void	Move(const Vec<T,3> &vDelta);
		void	Scale(const Vec<T,3> &vScale);

		void	Rotate(const Matrix<T,3,3> &sMatrix);
		void	Rotate(const Matrix<T,4,4> &sMatrix);
		void	Transform(const Matrix<T,4,4> &sMatrix);
		void	TransformExt(const Matrix<T,4,4> &sMatrix);

		void	InterpolateL(const TAABBox<T> &sBox1, const TAABBox<T> &sBox2, T tK);
		void	InterpolateQ(const TAABBox<T> &sBox1, const TAABBox<T> &sBox2, const TAABBox<T> &sBox3, T tK);

		bool	Intersect(const TAABBox<T> &box) const;
		Self	Intersection(const TAABBox<T> &box) const;

		bool	IsInside(const TAABBox<T> &box) const;
		bool	IsInside(const Vec<T,3> &point) const;

		bool	IsFullInside(const TAABBox<T> &box) const;
		bool	IsFullInside(const Vec<T,3> &point) const;

		Self &	Set(const Vec<T,3> &sPoint);
		Self &	Set(const Vec<T,3> &vMin, const Vec<T,3> &vMax);
		Self &	SetBBox(const Vec<T,3> &center, const Vec<T,3> &extents);

		Vec<T,3>	GetCorner(e_corner::type eType) const;
		void		GetPoints(Vec<T,3> aPoints[8]) const;

		template <typename T2>
		TAABBox<T2>	Convert() const;

		String		ToString() const;

		TSphere<T>	ToSphere() const;
	};


	
	template <typename T>
	inline TAABBox<T>::TAABBox(): _vMin(), _vMax()
	{}

		
	template <typename T>
	inline TAABBox<T>::TAABBox(const TAABBox<T> &box): _vMin(box._vMin), _vMax(box._vMax)
	{}

	
	template <typename T>
	inline TAABBox<T>::TAABBox(const Vec<T,3> &vMin, const Vec<T,3> &vMax): _vMin(vMin), _vMax(vMax)
	{}
	
		
	template <typename T>
	inline TAABBox<T> & TAABBox<T>::Set(const Vec<T,3> &vPoint)
	{
		_vMin = vPoint;
		_vMax = vPoint;
		return *this;
	}

		
	template <typename T>
	inline TAABBox<T> & TAABBox<T>::Set(const Vec<T,3> &vMin, const Vec<T,3> &vMax)
	{
		_vMin = vMin;
		_vMax = vMax;
		return *this;
	}

	
	template <typename T>
	inline TAABBox<T> & TAABBox<T>::SetBBox(const Vec<T,3> &center, const Vec<T,3> &extents)
	{
		_vMin = center - extents * T(0.5);
		_vMax = center + extents * T(0.5);
		return *this;
	}
	

	template <typename T>
	inline void TAABBox<T>::Add(const Vec<T,3> &vPoint)
	{
		_vMax.MakeCeil( vPoint );
		_vMin.MakeFloor( vPoint );
	}


	template <typename T>
	inline void TAABBox<T>::Add(const TAABBox<T> &sBox)
	{
		Add( sBox.Min() );
		Add( sBox.Max() );
	}

		
	template <typename T>
	inline void TAABBox<T>::Repair()
	{
		if ( _vMin.x > _vMax.x )	SwapValues( _vMin.x, _vMax.x );
		if ( _vMin.y > _vMax.y )	SwapValues( _vMin.y, _vMax.y );
		if ( _vMin.z > _vMax.z )	SwapValues( _vMin.z, _vMax.z );
	}
	
		
	template <typename T>
	inline void TAABBox<T>::Move(const Vec<T,3> &vDelta)
	{
		_vMin += vDelta;
		_vMax += vDelta;
	}
	
		
	template <typename T>
	inline void TAABBox<T>::Scale(const Vec<T,3> &vScale)
	{
		_vMin *= vScale;
		_vMax *= vScale;
	}


	template <typename T>
	inline void TAABBox<T>::Rotate(const Matrix<T,3,3> &sMatrix)
	{
		Vec<T,3>	v_min	 = _vMin,
					v_max	 = _vMax,
					v_corner = _vMin;

		Set( sMatrix.RotateVect( v_corner ) );

		v_corner.z = v_max.z;
		Add( sMatrix.RotateVect( v_corner ) );

		v_corner.y = v_max.y;
		Add( sMatrix.RotateVect( v_corner ) );

		v_corner.z = v_min.z;
		Add( sMatrix.RotateVect( v_corner ) );

		v_corner.x = v_max.x;
		Add( sMatrix.RotateVect( v_corner ) );

		v_corner.z = v_max.z;
		Add( sMatrix.RotateVect( v_corner ) );

		v_corner.y = v_min.y;
		Add( sMatrix.RotateVect( v_corner ) );

		v_corner.z = v_min.z;
		Add( sMatrix.RotateVect( v_corner ) );
	}


	template <typename T>
	inline void TAABBox<T>::Rotate(const Matrix<T,4,4> &sMatrix)
	{
		Vec<T,3>	v_min	 = _vMin,
					v_max	 = _vMax,
					v_corner = _vMin;

		Set( sMatrix.RotateVect( v_corner ) );

		v_corner.z = v_max.z;
		Add( sMatrix.RotateVect( v_corner ) );

		v_corner.y = v_max.y;
		Add( sMatrix.RotateVect( v_corner ) );

		v_corner.z = v_min.z;
		Add( sMatrix.RotateVect( v_corner ) );

		v_corner.x = v_max.x;
		Add( sMatrix.RotateVect( v_corner ) );

		v_corner.z = v_max.z;
		Add( sMatrix.RotateVect( v_corner ) );

		v_corner.y = v_min.y;
		Add( sMatrix.RotateVect( v_corner ) );

		v_corner.z = v_min.z;
		Add( sMatrix.RotateVect( v_corner ) );
	}

		
	template <typename T>
	inline void TAABBox<T>::Transform(const Matrix<T,4,4> &sMatrix)
	{
		Vec<T,3>	v_min	 = _vMin,
					v_max	 = _vMax,
					v_corner = _vMin;

		Set( sMatrix.TransformVect( v_corner ) );

		v_corner.z = v_max.z;
		Add( sMatrix.TransformVect( v_corner ) );

		v_corner.y = v_max.y;
		Add( sMatrix.TransformVect( v_corner ) );

		v_corner.z = v_min.z;
		Add( sMatrix.TransformVect( v_corner ) );

		v_corner.x = v_max.x;
		Add( sMatrix.TransformVect( v_corner ) );

		v_corner.z = v_max.z;
		Add( sMatrix.TransformVect( v_corner ) );

		v_corner.y = v_min.y;
		Add( sMatrix.TransformVect( v_corner ) );

		v_corner.z = v_min.z;
		Add( sMatrix.TransformVect( v_corner ) );
	}


	template <typename T>
	inline void TAABBox<T>::TransformExt(const Matrix<T,4,4> &sMatrix)
	{
		Vec<T,3>	v_center	= Center() + sMatrix.Translation(),
					v_extent	= Extent() * T(0.5),
					v_min		= -v_extent,
					v_max		= v_extent;

		_vMin = _vMax = v_center;

		for (uint i = 0; i < 3; ++i)
		{
			for (uint j = 0; j < 3; ++j)
			{
				const T	a = sMatrix(j,i) * v_min[i];
				const T	b = sMatrix(j,i) * v_max[j];

				if ( a < b ) {
					_vMin[i] += a;
					_vMax[i] += b;
				}
				else {
					_vMin[i] += b;
					_vMax[i] += a;
				}
			}
		}
	}

		
	template <typename T>
	inline void TAABBox<T>::InterpolateL(const TAABBox<T> &sBox1, const TAABBox<T> &sBox2, T tK)
	{
		_vMin.InterpolateL( sBox1.Min(), sBox2.Min(), tK );
		_vMax.InterpolateL( sBox1.Max(), sBox2.Max(), tK );
	}

		
	template <typename T>
	inline void TAABBox<T>::InterpolateQ(const TAABBox<T> &sBox1, const TAABBox<T> &sBox2, const TAABBox<T> &sBox3, T tK)
	{
		_vMin.InterpolateQ( sBox1.Min(), sBox2.Min(), sBox3.Min(), tK );
		_vMax.InterpolateQ( sBox1.Max(), sBox2.Max(), sBox3.Max(), tK );
	}


	template <typename T>
	inline bool TAABBox<T>::operator == (const TAABBox<T> &right) const
	{
		return ( _vMin == right._vMin and _vMax == right._vMax );
	}
	
	
	template <typename T>
	inline bool TAABBox<T>::operator != (const TAABBox<T> &right) const
	{
		return not ( *this == right );
	}
	
		
	template <typename T>
	inline TAABBox<T> & TAABBox<T>::operator += (const Vec<T,3> &right)
	{
		Add( right );
		return *this;
	}

		
	template <typename T>
	inline TAABBox<T> & TAABBox<T>::operator += (const TAABBox<T> &right)
	{
		Add( right );
		return *this;
	}

		
	template <typename T>
	inline TAABBox<T> TAABBox<T>::operator + (const Vec<T,3> &right) const
	{
		return TAABBox<T>(*this) += right;
	}

		
	template <typename T>
	inline TAABBox<T> TAABBox<T>::operator + (const TAABBox<T> &right) const
	{
		return TAABBox<T>(*this) += right;
	}

		
	template <typename T>
	inline TAABBox<T> & TAABBox<T>::operator *= (const Matrix<T,4,4> &right)
	{
		TransformExt( right );
		return *this;
	}

		
	template <typename T>
	inline TAABBox<T> & TAABBox<T>::operator *= (const Matrix<T,3,3> &right)
	{
		Rotate( right );
		return *this;
	}

		
	template <typename T>
	inline TAABBox<T> TAABBox<T>::operator * (const Matrix<T,4,4> &right) const
	{
		TAABBox<T>	ret(*this);
		ret.TransformExt( right );
		return ret;
	}

		
	template <typename T>
	inline TAABBox<T> TAABBox<T>::operator * (const Matrix<T,3,3> &right) const
	{
		TAABBox<T>	ret(*this);
		ret.Transform( right );
		return ret;
	}


	template <typename T>
	inline T TAABBox<T>::Volume() const
	{
		Vec<T,3> const	ext = Extent();
		return ext.x * ext.y * ext.z;
	}
	

	template <typename T>
	inline T TAABBox<T>::MinRadius() const
	{
		const Vec<T,3>	side = Extent();
		return (T)0.5 * GXMath::Min( side.x, side.y, side.z );
	}


	template <typename T>
	inline T TAABBox<T>::MaxRadius() const
	{
		const Vec<T,3>	side = Extent();
		return Sqrt_3<T>()/T(2) * GXMath::Max( side.x, side.y, side.z );
	}

	
	template <typename T>
	inline bool TAABBox<T>::IsInside(const Vec<T,3> &point) const
	{
		return ( point.x >= _vMin.x and point.x <= _vMax.x and
				 point.y >= _vMin.y and point.y <= _vMax.y and
				 point.z >= _vMin.z and point.z <= _vMax.z );
	}


	template <typename T>
	inline bool TAABBox<T>::IsFullInside(const Vec<T,3> &point) const
	{
		return ( point.x > _vMin.x and point.x < _vMax.x and
				 point.y > _vMin.y and point.y < _vMax.y and
				 point.z > _vMin.z and point.z < _vMax.z );
	}
	
	
	template <typename T>
	inline bool TAABBox<T>::IsInside(const TAABBox<T> &box) const
	{
		return ( box._vMin.x >= _vMin.x and box._vMax.x <= _vMax.x and
				 box._vMin.y >= _vMin.y and box._vMax.y <= _vMax.y and
				 box._vMin.z >= _vMin.z and box._vMax.z <= _vMax.z );
	}

	
	template <typename T>
	inline bool TAABBox<T>::IsFullInside(const TAABBox<T> &box) const
	{
		return ( box._vMin.x > _vMin.x and box._vMax.x < _vMax.x and
				 box._vMin.y > _vMin.y and box._vMax.y < _vMax.y and
				 box._vMin.z > _vMin.z and box._vMax.z < _vMax.z );
	}


	template <typename T>
	inline bool TAABBox<T>::Intersect(const TAABBox<T> &box) const
	{
		return ( _vMax.x < box._vMin.x or _vMax.y < box._vMin.y or _vMax.z < box._vMin.z or
				 _vMin.x > box._vMax.x or _vMin.y > box._vMax.y or _vMin.z > box._vMax.z );
	}
	
		
	template <typename T>
	inline TAABBox<T> TAABBox<T>::Intersection(const TAABBox<T> &box) const
	{
		TAABBox<T>		s_ret( *this );

		s_ret.Min().MakeCeil( box.Min() );
		s_ret.Max().MakeFloor( box.Max() );
		s_ret.Repair();

		return s_ret;
	}


	template <typename T>
	inline Vec<T,3> TAABBox<T>::GetCorner(e_corner::type eType) const
	{
		switch ( eType )
		{
			case e_corner::FAR_LEFT_BOTTOM	:	return _vMin;
			case e_corner::FAR_LEFT_TOP		:	return Vec<T,3>( _vMin.x, _vMax.y, _vMin.z );
			case e_corner::FAR_RIGHT_TOP	:	return Vec<T,3>( _vMax.x, _vMax.y, _vMin.z );
			case e_corner::FAR_RIGHT_BOTTOM	:	return Vec<T,3>( _vMax.x, _vMin.y, _vMin.z );
			case e_corner::NEAR_RIGHT_BOTTOM:	return Vec<T,3>( _vMax.x, _vMin.y, _vMax.z );
			case e_corner::NEAR_LEFT_BOTTOM	:	return Vec<T,3>( _vMin.x, _vMin.y, _vMax.z );
			case e_corner::NEAR_LEFT_TOP	:	return Vec<T,3>( _vMin.x, _vMax.y, _vMax.z );
			case e_corner::NEAR_RIGHT_TOP	:	return _vMax;
		}

		WARNING( "unknown corner type" );
		return Vec<T,3>();
	}
	

	template <typename T>
	inline void TAABBox<T>::GetPoints(Vec<T,3> aPoints[8]) const
	{
		aPoints[0] = _vMin;
		aPoints[1] = Vec<T,3>( _vMin.x, _vMax.y, _vMin.z );
		aPoints[2] = Vec<T,3>( _vMax.x, _vMax.y, _vMin.z );
		aPoints[3] = Vec<T,3>( _vMax.x, _vMin.y, _vMin.z );
		aPoints[4] = Vec<T,3>( _vMax.x, _vMin.y, _vMax.z );
		aPoints[5] = Vec<T,3>( _vMin.x, _vMin.y, _vMax.z );
		aPoints[6] = Vec<T,3>( _vMin.x, _vMax.y, _vMax.z );
		aPoints[7] = _vMax;
	}


	template <typename T>
	template <typename T2>
	inline TAABBox<T2> TAABBox<T>::Convert() const
	{
		return TAABBox<T2>( _vMin.Convert<T2>(), _vMax.Convert<T2>() );
	}

		
	template <typename T>
	inline String TAABBox<T>::ToString() const
	{
		return (String("min( ") << _vMin.ToStringFull() << " ), max( " << _vMax.ToStringFull() << " )");
	}
	

	template <typename T>
	inline TSphere<T> TAABBox<T>::ToSphere() const
	{
		return TSphere<T>( Center(), MaxRadius() );
	}*/


}	// GXMath
}	// GX_STL
