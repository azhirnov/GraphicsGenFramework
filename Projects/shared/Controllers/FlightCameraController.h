// Copyright © 2014-2015  Zhirnov Andrey. All rights reserved.

#pragma once

#include "IController.h"

namespace Shared
{

	//
	// Flight Camera Controller
	//

	class FlightCameraController : public IController
	{
	// types
	private:
		typedef PerspectiveCamera<real>		Camera_t;
		typedef Camera_t::transform_t		Transform_t;
		typedef Camera_t::mat4_t			mat4_t;
		typedef Camera_t::vec2_t			vec2_t;
		typedef Camera_t::vec3_t			vec3_t;
		typedef Camera_t::quat_t			quat_t;
		typedef Camera_t::radians3_t		radians3_t;


	// variables
	private:
		Camera_t	_camera;
		mat4_t		_mvp;
		mat4_t		_mv;
		int2		_size;


	// methods
	public:
		FlightCameraController (const SubSystemsRef ss) :
			IController( ss )
		{
			Reset();
		}

		mat4_t const &	GetModelViewProjMatrix ()	const	{ return _mvp; }
		mat4_t const &	GetModelViewMatrix ()		const	{ return _mv; }
		mat4_t const &	GetViewProjMatrix ()				{ return _camera.ViewProjMatrix(); }
		mat4_t const &	GetViewMatrix ()			const	{ return _camera.ViewMatrix(); }
		mat4_t const &	GetProjMatrix ()			const	{ return _camera.ProjMatrix(); }
		mat4_t const	GetModelMatrix ()			const	{ return _camera.GetModelMatrix(); }
		
		vec3_t			GetPosition ()				const	{ return _camera.Position(); }
		radians3_t		GetRotation ()				const	{ return _camera.Rotation(); }
		quat_t			GetOrientation ()			const	{ return _camera.Orientation(); }
		vec3_t			GetDirection ()				const	{ return _camera.Orientation().GetDirection(); }
		vec2_t			GetScale ()					const	{ return vec2_t( _camera.GetZoom() ); }
		vec2_t const&	GetClipPlanes ()			const	{ return _camera.ClipPlanes(); }
		
		void Reset (const Transform_t &transform = Uninitialized())
		{
			_camera.Create( transform, _CameraFOV(), 1.0f, real2( 0.1f, 100.0f ) );

			_mvp = mat4_t::Identity();
			_mv  = mat4_t::Identity();
		}

		void Update (Time<double> dt) override
		{
			const real2		scale		= real2( 1.0f, 1.0f ) * 100.0f;
			const real		zoom		= 0.2f;
			const real3		move_factor	= real3( 1.0f, 0.5f, 0.5f );
			const real		velocity	= 1.0f;
			const real		rot_step	= 0.3f;

			Ptr< Input >	input		= SubSystems()->Get< Input >();
			const uint		id			= input->GetLastTouchId();
			const real		move_step	= velocity * real(dt.Seconds());
			
			Platform::WindowDesc	wnd;
			SubSystems()->Get< Platform >()->GetWindowDesc( wnd );

			if ( Any( _size != wnd.size ) )
			{
				_size = wnd.size;
				_camera.Resize( _CameraFOV(), float(_size.x) / _size.y );
			}
			
			real3	rotation;
			real3	motion;
			real	increase = 0.0f;

			// rotation
			if ( input->GetTouch(id).IsPressed() )
				rotation = ( input->GetTouch(id).delta.To<real2>() / _size.To<real2>() * scale ).oyx();

			if ( input->IsKeyPressed( EKey::LEFT ) )		rotation.x -= rot_step;
			if ( input->IsKeyPressed( EKey::RIGHT ) )		rotation.x += rot_step;
			if ( input->IsKeyPressed( EKey::UP ) )			rotation.y += rot_step;
			if ( input->IsKeyPressed( EKey::DOWN ) )		rotation.y -= rot_step;
			if ( input->IsKeyPressed( EKey::Q ) )			rotation.z -= rot_step;
			if ( input->IsKeyPressed( EKey::E ) )			rotation.z += rot_step;

			// motion
			if ( input->IsKeyPressed( EKey::W ) )			motion.x += move_factor.x;
			if ( input->IsKeyPressed( EKey::S ) )			motion.x -= move_factor.x;
			if ( input->IsKeyPressed( EKey::D ) )			motion.y += move_factor.y;
			if ( input->IsKeyPressed( EKey::A ) )			motion.y -= move_factor.y;
			if ( input->IsKeyPressed( EKey::SPACE ) )		motion.z += move_factor.z;
			if ( input->IsKeyPressed( EKey::L_SHIFT ) )		motion.z -= move_factor.z;

			// zooming
			if ( input->IsKeyPressed( EKey::M_WHEEL_UP ) )		increase += zoom;
			if ( input->IsKeyPressed( EKey::M_WHEEL_DOWN ) )	increase -= zoom;
			
			if ( IsNotZero( increase ) )
			{
				increase += _camera.GetZoom();
				_camera.Increase( Clamp( increase, 1.0f, 10.0f ) );
			}
			
			if ( IsNotZero( rotation ) )
				_camera.RotateFlight( (rotation / _camera.GetZoom()).To< DegreesVec<real,3> >() );

			if ( IsNotZero( motion ) )
				_camera.MoveFlight( motion.Normalized() * move_step );

			_mvp = _camera.ViewProjMatrix() * _camera.GetModelMatrix();
			_mv  = _camera.ViewMatrix() * _camera.GetModelMatrix();
		}


	private:
		static Radians<real> _CameraFOV ()
		{
			return Degrees<real>( 60.0f ).ToRadians();
		}
	};


}	// Shared
