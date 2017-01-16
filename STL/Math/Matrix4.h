// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.
/*
	Description:	4x2, 4x3, 4x4 matrices
*/

#pragma once

#include "MatrixUtils.h"
#include "2D/Rectangle.h"

namespace GX_STL
{
namespace GXMath
{



	//
	// Matrix 4x2
	//
	
#define C	4
#define R	2
#define MATRIX_METHODS() \
		Matrix (T _00, T _10, T _20, T _30, \
				T _01, T _11, T _21, T _31); \
		Matrix (const col_t &c0, \
				const col_t &c1, \
				const col_t &c2, \
				const col_t &c3); \


#include "MatrixCR.h"
	


	template <typename T>
	inline Matrix<T,C,R>::Matrix (T _00, T _10, T _20, T _30,
								   T _01, T _11, T _21, T _31)
	{
		_v[0][0] = _00;		_v[1][0] = _10;		_v[2][0] = _20;		_v[3][0] = _30;
		_v[0][1] = _01;		_v[1][1] = _11;		_v[2][1] = _21;		_v[3][1] = _31;
	}


	template <typename T>
	inline Matrix<T,C,R>::Matrix (const col_t &c0,
								   const col_t &c1,
								   const col_t &c2,
								   const col_t &c3)
	{
		MemCopy( _v[0], c0.ptr(), sizeof(T)*R );
		MemCopy( _v[1], c1.ptr(), sizeof(T)*R );
		MemCopy( _v[2], c2.ptr(), sizeof(T)*R );
		MemCopy( _v[3], c3.ptr(), sizeof(T)*R );
	}

#undef	R
#undef	C
#undef	MATRIX_METHODS

	
	

	//
	// Matrix 4x3
	//
	
#define C	4
#define R	3
#define MATRIX_METHODS() \
		Matrix (T _00, T _10, T _20, T _30, \
				T _01, T _11, T _21, T _31, \
				T _02, T _12, T _22, T _32); \
		Matrix (const col_t &c0, \
				const col_t &c1, \
				const col_t &c2, \
				const col_t &c3); \


#include "MatrixCR.h"
	


	template <typename T>
	inline Matrix<T,C,R>::Matrix (T _00, T _10, T _20, T _30,
								   T _01, T _11, T _21, T _31,
								   T _02, T _12, T _22, T _32)
	{
		_v[0][0] = _00;		_v[1][0] = _10;		_v[2][0] = _20;		_v[3][0] = _30;
		_v[0][1] = _01;		_v[1][1] = _11;		_v[2][1] = _21;		_v[3][1] = _31;
		_v[0][2] = _02;		_v[1][2] = _12;		_v[2][2] = _22;		_v[3][2] = _32;
	}


	template <typename T>
	inline Matrix<T,C,R>::Matrix (const col_t &c0,
								   const col_t &c1,
								   const col_t &c2,
								   const col_t &c3)
	{
		MemCopy( _v[0], c0.ptr(), sizeof(T)*R );
		MemCopy( _v[1], c1.ptr(), sizeof(T)*R );
		MemCopy( _v[2], c2.ptr(), sizeof(T)*R );
		MemCopy( _v[3], c3.ptr(), sizeof(T)*R );
	}

#undef	R
#undef	C
#undef	MATRIX_METHODS

	
	

	//
	// Matrix 4x4
	//
	
#define C	4
#define R	4
#define MATRIX_METHODS() \
		Matrix (T _00, T _10, T _20, T _30, \
				T _01, T _11, T _21, T _31, \
				T _02, T _12, T _22, T _32, \
				T _03, T _13, T _23, T _33); \
		Matrix (const col_t &c0, \
				const col_t &c1, \
				const col_t &c2, \
				const col_t &c3); \
		\
		Self		Inverse () const; \
		\
		Self		ToBillboard () const; \
		static Self	SphericalBillboard (const Vec<T,3> &vCamPos, const Vec<T,3> &vPos); \
		\
		static Self	BuildPerspective (const T& tFovY, const T& tAspect, const Vec<T,2> &vClipPlanes); \
		static Self	BuildRealPerspective (const Vec<T,2> &vSize, const Vec<T,2> &vClipPlanes, const T& tDist); \
		static Self	BuildOrtho (const Rectangle<T> &sRect, const Vec<T,2> &vClipPlanes); \
		static Self	BuildOrtho2D (const Rectangle<T> &sRect); \
		static Self	BuildFrustum (const Rectangle<T> &sRect, const Vec<T,2> &vClipPlanes); \
		static Self	BuildLookAt (const Vec<T,3> &vPos, const Vec<T,3> &vCenter, const Vec<T,3> &vUp); \
		/*Self		GetTextureMatrix () const;*/ \
		\
		static Vec<T,3> Project (const Vec<T,3> &pos, const Self &mvp, const Rectangle<T> &viewport); \
		static Vec<T,3> UnProject (const Vec<T,3> &pos, const Self &mvpInverse, const Rectangle<T> &viewport); \


#include "MatrixCR.h"
	


	template <typename T>
	inline Matrix<T,C,R>::Matrix (T _00, T _10, T _20, T _30,
								   T _01, T _11, T _21, T _31,
								   T _02, T _12, T _22, T _32,
								   T _03, T _13, T _23, T _33)
	{
		_v[0][0] = _00;		_v[1][0] = _10;		_v[2][0] = _20;		_v[3][0] = _30;
		_v[0][1] = _01;		_v[1][1] = _11;		_v[2][1] = _21;		_v[3][1] = _31;
		_v[0][2] = _02;		_v[1][2] = _12;		_v[2][2] = _22;		_v[3][2] = _32;
		_v[0][3] = _03;		_v[1][3] = _13;		_v[2][3] = _23;		_v[3][3] = _33;
	}


	template <typename T>
	inline Matrix<T,C,R>::Matrix (const col_t &c0,
								   const col_t &c1,
								   const col_t &c2,
								   const col_t &c3)
	{
		MemCopy( _v[0], c0.ptr(), sizeof(T)*R );
		MemCopy( _v[1], c1.ptr(), sizeof(T)*R );
		MemCopy( _v[2], c2.ptr(), sizeof(T)*R );
		MemCopy( _v[3], c3.ptr(), sizeof(T)*R );
	}
	

	template <typename T>
	inline Matrix<T,C,R>  Matrix<T,C,R>::Inverse () const
	{
		const T s00 = _v[2][2] * _v[3][3] - _v[3][2] * _v[2][3];
		const T s01 = _v[2][1] * _v[3][3] - _v[3][1] * _v[2][3];
		const T s02 = _v[2][1] * _v[3][2] - _v[3][1] * _v[2][2];
		const T s03 = _v[2][0] * _v[3][3] - _v[3][0] * _v[2][3];
		const T s04 = _v[2][0] * _v[3][2] - _v[3][0] * _v[2][2];
		const T s05 = _v[2][0] * _v[3][1] - _v[3][0] * _v[2][1];
		const T s06 = _v[1][2] * _v[3][3] - _v[3][2] * _v[1][3];
		const T s07 = _v[1][1] * _v[3][3] - _v[3][1] * _v[1][3];
		const T s08 = _v[1][1] * _v[3][2] - _v[3][1] * _v[1][2];
		const T s09 = _v[1][0] * _v[3][3] - _v[3][0] * _v[1][3];
		const T s10 = _v[1][0] * _v[3][2] - _v[3][0] * _v[1][2];
		const T s11 = _v[1][1] * _v[3][3] - _v[3][1] * _v[1][3];
		const T s12 = _v[1][0] * _v[3][1] - _v[3][0] * _v[1][1];
		const T s13 = _v[1][2] * _v[2][3] - _v[2][2] * _v[1][3];
		const T s14 = _v[1][1] * _v[2][3] - _v[2][1] * _v[1][3];
		const T s15 = _v[1][1] * _v[2][2] - _v[2][1] * _v[1][2];
		const T s16 = _v[1][0] * _v[2][3] - _v[2][0] * _v[1][3];
		const T s17 = _v[1][0] * _v[2][2] - _v[2][0] * _v[1][2];
		const T s18 = _v[1][0] * _v[2][1] - _v[2][0] * _v[1][1];

		const Self	inv(  _v[1][1] * s00 - _v[1][2] * s01 + _v[1][3] * s02,
						- _v[1][0] * s00 + _v[1][2] * s03 - _v[1][3] * s04,
						  _v[1][0] * s01 - _v[1][1] * s03 + _v[1][3] * s05,
						- _v[1][0] * s02 + _v[1][1] * s04 - _v[1][2] * s05,

						- _v[0][1] * s00 + _v[0][2] * s01 - _v[0][3] * s02,
						  _v[0][0] * s00 - _v[0][2] * s03 + _v[0][3] * s04,
						- _v[0][0] * s01 + _v[0][1] * s03 - _v[0][3] * s05,
						  _v[0][0] * s02 - _v[0][1] * s04 + _v[0][2] * s05,

						  _v[0][1] * s06 - _v[0][2] * s07 + _v[0][3] * s08,
						- _v[0][0] * s06 + _v[0][2] * s09 - _v[0][3] * s10,
						  _v[0][0] * s11 - _v[0][1] * s09 + _v[0][3] * s12,
						- _v[0][0] * s08 + _v[0][1] * s10 - _v[0][2] * s12,

						- _v[0][1] * s13 + _v[0][2] * s14 - _v[0][3] * s15,
						  _v[0][0] * s13 - _v[0][2] * s16 + _v[0][3] * s17,
						- _v[0][0] * s14 + _v[0][1] * s16 - _v[0][3] * s18,
						  _v[0][0] * s15 - _v[0][1] * s17 + _v[0][2] * s18 );

		const T	det =	_v[0][0] * inv(0,0) + _v[0][1] * inv(1,0) +
						_v[0][2] * inv(2,0) + _v[0][3] * inv(3,0);

		return ( inv / det );
	}

	
	template <typename T>
	inline Matrix<T,C,R>  Matrix<T,C,R>::ToBillboard () const
	{
		return Self(
			1,				0,				0,				(*this)(3,0),
			0,				1,				0,				(*this)(3,1),
			0,				0,				1,				(*this)(3,2),
			(*this)(0,3),	(*this)(1,3),	(*this)(2,3),	(*this)(3,3) );
	}

	
	template <typename T>
	inline Matrix<T,C,R>  Matrix<T,C,R>::SphericalBillboard (const Vec<T,3> &vCamPos, const Vec<T,3> &vPos)
	{
		Vec<T,3>		v_look_at( 0, 0, 1 ),
						v_obj_to_cam_proj( vCamPos.x - vPos.x, 0, vCamPos.z - vPos.z ),
						v_obj_to_cam,
						v_up_aux;
		Matrix<T,4,4>	s_mv;
		T				t_angle_cosine;


		v_obj_to_cam_proj.Normalize();
		v_up_aux		= Cross( v_look_at, v_obj_to_cam_proj );
		t_angle_cosine	= Dot( v_look_at, v_obj_to_cam_proj );

		if ( t_angle_cosine < (T)0.9999 and t_angle_cosine > (T)-0.9999 )
			s_mv *= Rotation( acos(t_angle_cosine), v_up_aux );


		v_obj_to_cam	= (vCamPos - vPos).Normalized();
		t_angle_cosine	= Dot( v_obj_to_cam_proj, v_obj_to_cam );

		if ( (t_angle_cosine < (T)0.9999) and (t_angle_cosine > (T)-0.9999) )
		{
			if ( v_obj_to_cam.y < (T)0 )
				s_mv *= Rotation( acos(t_angle_cosine), Vec<T,3>( 1, 0, 0 ) );
			else
				s_mv *= Rotation( acos(t_angle_cosine), Vec<T,3>( -1, 0, 0 ) );
		}

		return s_mv;
	}
	
	
	template <typename T>
	inline Matrix<T,C,R>  Matrix<T,C,R>::BuildPerspective (const T& tFovY, const T& tAspect, const Vec<T,2> &vClipPlanes)
	{
		const T	f = T(1) / Tan( tFovY * T(0.5) ),
				A = ( vClipPlanes.y + vClipPlanes.x ) / ( vClipPlanes.x - vClipPlanes.y ),
				B = ( T(2) * vClipPlanes.x * vClipPlanes.y ) / ( vClipPlanes.x - vClipPlanes.y );

		return Self(	f / tAspect,	0,	0,	0,
						0,				f,	0,	0,
						0,				0,	A,	B,
						0,				0,	-1,	0 );
	}

	
	template <typename T>
	inline Matrix<T,C,R>  Matrix<T,C,R>::BuildRealPerspective (const Vec<T,2> &vSize, const Vec<T,2> &vClipPlanes, const T& tDist)
	{
		//vSize - size of window in meters
		const T	asp	= vSize.x / vSize.y,
				f	= (2 * tDist) / vSize.y,
				A	= ( vClipPlanes.y + vClipPlanes.x ) / ( vClipPlanes.x - vClipPlanes.y ),
				B	= ( T(2) * vClipPlanes.x * vClipPlanes.y ) / ( vClipPlanes.x - vClipPlanes.y );
		
		return Self(	f / asp,	0,	0,	0,
						0,			f,	0,	0,
						0,			0,	A,	B,
						0,			0,	-1,	0 );
	}
	

	template <typename T>
	inline Matrix<T,C,R>  Matrix<T,C,R>::BuildOrtho (const Rectangle<T> &sRect, const Vec<T,2> &vClipPlanes)
	{
		const T	tx = - ( sRect.right + sRect.left ) / ( sRect.right - sRect.left ),
				ty = - ( sRect.top + sRect.bottom ) / ( sRect.top - sRect.bottom ),
				tz = - ( vClipPlanes.y + vClipPlanes.x ) / ( vClipPlanes.y - vClipPlanes.x ),
				sx = T(2) / ( sRect.right - sRect.left ),
				sy = T(2) / ( sRect.top - sRect.bottom ),
				sz = T(2) / ( vClipPlanes.x - vClipPlanes.y );

		return Self(	sx,	0,	0,	tx,
						0,	sy,	0,	ty,
						0,	0,	sz,	tz,
						0,	0,	0,	1 );
	}

	
	template <typename T>
	inline Matrix<T,C,R>  Matrix<T,C,R>::BuildOrtho2D (const Rectangle<T> &sRect)
	{
		return BuildOrtho( sRect, Vec<T,2>( -1, 1 ) );
	}

	
	template <typename T>
	inline Matrix<T,C,R>  Matrix<T,C,R>::BuildFrustum (const Rectangle<T> &sRect, const Vec<T,2> &vClipPlanes)
	{
		const T	sx = ( T(2) * vClipPlanes.x ) / ( sRect.left - sRect.right ),
				sy = ( T(2) * vClipPlanes.x ) / ( sRect.top - sRect.bottom ),
				sz = ( vClipPlanes.y + vClipPlanes.x ) / ( vClipPlanes.x - vClipPlanes.y ),
				rx = ( sRect.left + sRect.right ) / ( sRect.left - sRect.right ),
				ry = ( sRect.top + sRect.bottom ) / ( sRect.top - sRect.bottom ),
				tz = ( T(2) * vClipPlanes.x * vClipPlanes.y ) / ( vClipPlanes.x - vClipPlanes.y );

		return Self(	sx,	0,	rx,	0,
						0,	sy,	ry,	0,
						0,	0,	sz,	tz,
						0,	0,	-1,	0 );
	}


	template <typename T>
	inline Matrix<T,C,R>  Matrix<T,C,R>::BuildLookAt (const Vec<T,3> &vPos, const Vec<T,3> &vCenter, const Vec<T,3> &vUp)
	{
		const Vec<T,3>	f = ( vPos - vCenter ).Normalized(),
						s = Cross( vUp, f ).Normalized(),
						u = Cross( f, s ).Normalized();

		return Self(	s.x,	s.y,	s.z,	-Dot( s, vPos ),
						u.x,	u.y,	u.z,	-Dot( u, vPos ),
						f.x,	f.y,	f.z,	-Dot( f, vPos ),
						0,		0,		0,		1 );
	}

	/*
	template <typename T>
	inline Matrix<T,C,R>  Matrix<T,C,R>::GetTextureMatrix () const
	{
		return Self(	(*this)(0,0),	(*this)(1,0),	0,	(*this)(3,0),
						(*this)(0,1),	(*this)(1,1),	0,	(*this)(3,1),
						0,				0,				1,	(*this)(3,2),
						(*this)(0,3),	(*this)(1,3),	0,	(*this)(3,3) );
	}
	*/
	
	template <typename T>
	inline Vec<T,3> Matrix<T,C,R>::Project (const Vec<T,3> &pos, const Matrix<T,C,R> &mvp, const Rectangle<T> &viewport)
	{
		Vec<T,4>	v_temp	= mvp * Vec<T,4>( pos, T(1) );
		Vec<T,2>	v_scr	= viewport.Size();

		v_temp  /= v_temp.w;
		v_temp   = v_temp * T(0.5) + T(0.5);
		v_temp.x = v_temp.x * v_scr.x + viewport.left;
		v_temp.y = v_temp.y * v_scr.y + viewport.bottom;

		return v_temp.xyz();
	}

	
	template <typename T>
	inline Vec<T,3> Matrix<T,C,R>::UnProject (const Vec<T,3> &pos, const Matrix<T,C,R> &mvpInverse, const Rectangle<T> &viewport)
	{
		Vec<T,4>	v_temp	= Vec<T,4>( pos, T(1) );
		Vec<T,2>	v_scr	= viewport.Size();

		v_temp.x = (v_temp.x - viewport.left) / v_scr.x;
		v_temp.y = (v_temp.y - viewport.bottom) / v_scr.y;
		v_temp	 = v_temp * T(2) - T(1);

		v_temp	 = mvpInverse * v_temp;
		v_temp	/= v_temp.w;

		return v_temp.xyz();
	}


#undef	R
#undef	C
#undef	MATRIX_METHODS


}	// GXMath
}	// GX_STL
