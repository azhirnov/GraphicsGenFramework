// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Common.h"
#include "Renderer.h"
#include "ISample.h"
#include "../Particles/ParticleRenderer.h"
#include "../Tiles/TilesManager.h"

namespace ShaderEditor
{
	
	//
	// Shader Editor Core
	//

	class ShaderEditorCore : public Base::GameUtils::GameApplication
	{
	// variables
	private:
		GraphicsEngine		_graphicsEngine;
		ComputeEngine		_computeEngine;
		Renderer			_renderer;
		ParticleRenderer	_particleRenderer;
		TilesManager		_tilesManager;

		Array< ISamplePtr >	_samples;
		usize				_activeSample;
		
		Time<double>		_time;
		bool				_timePaused;
		bool				_initialized;
		bool				_makeScreenShot;


	// methods
	public:
		explicit
		ShaderEditorCore (const IParallelThreadPtr &thread);
		~ShaderEditorCore ();

		void AddSample (const ISamplePtr &sample);

		Ptr< Renderer >			GetRenderer ()			{ return &_renderer; }
		Ptr< ParticleRenderer >	GetParticleRenderer ()	{ return &_particleRenderer; }
		Ptr< TilesManager >		GetTilesManager ()		{ return &_tilesManager; }


	private:
		void _ActiveSample (usize index);

		void _Update (Time<double> dt, bool forceRedraw) override;
		void _Draw ();

		void _OnInit () override;

		void _OnSurfaceCreated () override;

		void _OnExit () override;

		void _OnResized () override;

		void _Reload ();
	};


}	// ShaderEditor
