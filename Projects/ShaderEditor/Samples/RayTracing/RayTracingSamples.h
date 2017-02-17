// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Projects/ShaderEditor/Core/ISample.h"

namespace ShaderEditor
{

	//
	// Ray Tracing Samples
	//

	class RayTracingSamples : public ISample
	{
	// variables
	private:
		IGeneratorPtr			_imageGen;
		ComputeImagePtr			_image;
		uint					_curShader;

		ScaleBiasController		_controller;


	// methods
	private:

		IGeneratorPtr _ChooseShader (uint index) const;

	public:
		explicit
		RayTracingSamples (const SubSystemsRef ss);

		void Init () override;
		void Release () override;
		void Reload () override;

		void Draw (const RenderTargetPtr &rt, TimeD globalTime) override;
		void Update (TimeD dt) override;
		
		bool Next () override;
	};


}	// ShaderEditor
