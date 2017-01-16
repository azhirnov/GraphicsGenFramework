// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "AxisAlignedBox.h"
#include "Plane.h"

namespace GX_STL
{
namespace GXMath
{
	/*
	//
	// Frustum
	//

	template <typename T>
	struct TFrustum
	{
	public:
		struct e_frustum_plane
		{
			enum type
			{
				NEAR   = 0,
				FAR    = 1,
				LEFT   = 2,
				RIGHT  = 3,
				TOP    = 4,
				BOTTOM = 5,
			};
		};

		TPlane<T>	_aPlanes[6];

	public:
		void Setup(const Matrix<T,4,4> &sVP);
		bool IsVisible(const TAABBox<T> &box) const;
		bool IsVisible(const Vec<T,3> &point) const;
		bool IsVisible(const TFrustum<T> &frustum) const;
		bool IsVisible(const Vec<T,3> &center, T radius) const;
		bool IsVisible(const Vec<T,3> &center, const Vec<T,3> &halfextents) const;

		//template <typename T2>
		//TFrustum<T2> Convert() const;
	};



	template <typename T>
	inline void TFrustum<T>::Setup(const Matrix<T,4,4> &sVP)
	{
		_aPlanes[e_frustum_plane::LEFT].Normal().x = sVP(3,0) + sVP(0,0);
		_aPlanes[e_frustum_plane::LEFT].Normal().y = sVP(3,1) + sVP(0,1);
		_aPlanes[e_frustum_plane::LEFT].Normal().z = sVP(3,2) + sVP(0,2);
		_aPlanes[e_frustum_plane::LEFT].Distance() = sVP(3,3) + sVP(0,3);
		_aPlanes[e_frustum_plane::LEFT].Normalize();
		
		_aPlanes[e_frustum_plane::RIGHT].Normal().x = sVP(3,0) - sVP(0,0);
		_aPlanes[e_frustum_plane::RIGHT].Normal().y = sVP(3,1) - sVP(0,1);
		_aPlanes[e_frustum_plane::RIGHT].Normal().z = sVP(3,2) - sVP(0,2);
		_aPlanes[e_frustum_plane::RIGHT].Distance() = sVP(3,3) - sVP(0,3);
		_aPlanes[e_frustum_plane::RIGHT].Normalize();

		_aPlanes[e_frustum_plane::TOP].Normal().x = sVP(3,0) - sVP(1,0);
		_aPlanes[e_frustum_plane::TOP].Normal().x = sVP(3,1) - sVP(1,1);
		_aPlanes[e_frustum_plane::TOP].Normal().x = sVP(3,2) - sVP(1,2);
		_aPlanes[e_frustum_plane::TOP].Distance() = sVP(3,3) - sVP(1,3);
		_aPlanes[e_frustum_plane::TOP].Normalize();
		
		_aPlanes[e_frustum_plane::BOTTOM].Normal().x = sVP(3,0) + sVP(1,0);
		_aPlanes[e_frustum_plane::BOTTOM].Normal().x = sVP(3,1) + sVP(1,1);
		_aPlanes[e_frustum_plane::BOTTOM].Normal().x = sVP(3,2) + sVP(1,2);
		_aPlanes[e_frustum_plane::BOTTOM].Distance() = sVP(3,3) + sVP(1,3);
		_aPlanes[e_frustum_plane::BOTTOM].Normalize();
		
		_aPlanes[e_frustum_plane::NEAR].Normal().x = sVP(3,0) + sVP(2,0);
		_aPlanes[e_frustum_plane::NEAR].Normal().x = sVP(3,1) + sVP(2,1);
		_aPlanes[e_frustum_plane::NEAR].Normal().x = sVP(3,2) + sVP(2,2);
		_aPlanes[e_frustum_plane::NEAR].Distance() = sVP(3,3) + sVP(2,3);
		_aPlanes[e_frustum_plane::NEAR].Normalize();
		
		_aPlanes[e_frustum_plane::FAR].Normal().x = sVP(3,0) - sVP(2,0);
		_aPlanes[e_frustum_plane::FAR].Normal().x = sVP(3,1) - sVP(2,1);
		_aPlanes[e_frustum_plane::FAR].Normal().x = sVP(3,2) - sVP(2,2);
		_aPlanes[e_frustum_plane::FAR].Distance() = sVP(3,3) - sVP(2,3);
		_aPlanes[e_frustum_plane::FAR].Normalize();
	}
	
			
	template <typename T>
	inline bool TFrustum<T>::IsVisible(const Vec<T,3> &point) const
	{
		return ( _aPlanes[e_frustum_plane::LEFT  ].Intersect( point ) != e_plane_side::NEGATIVE and
				 _aPlanes[e_frustum_plane::RIGHT ].Intersect( point ) != e_plane_side::NEGATIVE and
				 _aPlanes[e_frustum_plane::TOP   ].Intersect( point ) != e_plane_side::NEGATIVE and
				 _aPlanes[e_frustum_plane::BOTTOM].Intersect( point ) != e_plane_side::NEGATIVE and
				 _aPlanes[e_frustum_plane::NEAR  ].Intersect( point ) != e_plane_side::NEGATIVE and
				 _aPlanes[e_frustum_plane::FAR   ].Intersect( point ) != e_plane_side::NEGATIVE );
	}

		
	template <typename T>
	inline bool TFrustum<T>::IsVisible(const TAABBox<T> &box) const
	{
		return IsVisible( box.Center(), box.HalfExtent() );
	}

		
	template <typename T>
	inline bool TFrustum<T>::IsVisible(const Vec<T,3> &center, const T radius) const
	{
		return ( _aPlanes[e_frustum_plane::LEFT  ].Distance( center ) >= -radius and
				 _aPlanes[e_frustum_plane::RIGHT ].Distance( center ) >= -radius and
				 _aPlanes[e_frustum_plane::TOP   ].Distance( center ) >= -radius and
				 _aPlanes[e_frustum_plane::BOTTOM].Distance( center ) >= -radius and
				 _aPlanes[e_frustum_plane::NEAR  ].Distance( center ) >= -radius and
				 _aPlanes[e_frustum_plane::FAR   ].Distance( center ) >= -radius );
	}

		
	template <typename T>
	inline bool TFrustum<T>::IsVisible(const Vec<T,3> &center, const Vec<T,3> &halfextents) const
	{
		return ( _aPlanes[e_frustum_plane::LEFT  ].Intersect( center, halfextents ) != e_plane_side::NEGATIVE and
				 _aPlanes[e_frustum_plane::RIGHT ].Intersect( center, halfextents ) != e_plane_side::NEGATIVE and
				 _aPlanes[e_frustum_plane::TOP   ].Intersect( center, halfextents ) != e_plane_side::NEGATIVE and
				 _aPlanes[e_frustum_plane::BOTTOM].Intersect( center, halfextents ) != e_plane_side::NEGATIVE and
				 _aPlanes[e_frustum_plane::NEAR  ].Intersect( center, halfextents ) != e_plane_side::NEGATIVE and
				 _aPlanes[e_frustum_plane::FAR   ].Intersect( center, halfextents ) != e_plane_side::NEGATIVE );
	}
	

	template <typename T>
	inline bool TFrustum<T>::IsVisible(const TFrustum<T> &frustum) const
	{
		TODO( "intersect plane" );
		return false;
	}
	

	/*template <typename T>
	template <typename T2>
	inline TFrustum<T2> TFrustum<T>::Convert() const
	{
		TFrustum<T2>	ret;

		ret._aPlanes[0]		= _aPlanes[0].Convert<T2>();
		ret._aPlanes[1]		= _aPlanes[1].Convert<T2>();
		ret._aPlanes[2]		= _aPlanes[2].Convert<T2>();
		ret._aPlanes[3]		= _aPlanes[3].Convert<T2>();
		ret._aPlanes[4]		= _aPlanes[4].Convert<T2>();
		ret._aPlanes[5]		= _aPlanes[5].Convert<T2>();

		return ret;
	}*/


}	// GXMath
}	// GX_STL
