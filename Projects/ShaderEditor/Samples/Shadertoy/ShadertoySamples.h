// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../../Core/ISample.h"

namespace ShaderEditor
{

	//
	// Shadertoy Samples
	//

	class ShadertoySamples : public ISample
	{
	// variables
	private:
		IGeneratorPtr					_imageGen;
		ComputeImagePtr					_image;

		Shared::ScaleBiasController		_controller;

		uint2							_cellSize;
		uint2							_subCellSize;


	// methods
	private:
		ShadertoySamples (const SubSystemsRef ss);

		IGeneratorPtr _ChooseShader () const;

	public:
		void Init () override;
		void Release () override;
		void Reload () override;

		void Draw (const RenderTargetPtr &rt, Time<double> globalTime) override;
		void Update (Time<double> dt) override;

		static ISamplePtr  New (const SubSystemsRef ss);
	};


}	// ShaderEditor
