// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "IController.h"

namespace ShaderEditor
{

	//
	// Scale Bias Controller
	//

	class ScaleBiasController : public IController
	{
	// variables
	private:
		float2		_scale;
		float2		_bias;


	// methods
	public:
		ScaleBiasController (const SubSystemsRef ss) :
			IController( ss )
		{
			Reset();
		}

		float2 const &	Scale ()	const	{ return _scale; }
		float2 const &	Bias ()		const	{ return _bias; }

		void Reset ()
		{
			_scale	= float2( 1.0f );
			_bias	= float2( 0.0f );
		}

		void Update (Time<double> dt) override
		{
			Ptr< Input >	input	= SubSystems()->Get< Input >();
			const uint		id		= input->GetLastTouchId();
			
			Platform::WindowDesc	wnd;
			SubSystems()->Get< Platform >()->GetWindowDesc( wnd );

			
			// move plane
			if ( input->GetTouch(id).IsPressed() )
			{
				_bias += input->GetTouch(id).delta.To<float2>() / wnd.size.To<float2>() * _scale;
			}

			// scale plane
			const float	scale_step = 0.1f;

			if ( input->IsKeyPressed( EKey::M_WHEEL_UP ) )
				_scale += scale_step;
			else
			if ( input->IsKeyPressed( EKey::M_WHEEL_DOWN ) )
				_scale -= scale_step;

			// reset position and scaling
			if ( input->IsKeyClicked( EKey::C ) )
			{
				Reset();
			}
		
			// validate values
			_scale = Clamp( _scale, 0.1f, 10.0f );
		}
	};


}	// ShaderEditor
