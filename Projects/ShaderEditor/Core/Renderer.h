// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Common.h"

namespace ShaderEditor
{

	struct EImageViewMode
	{
		enum type
		{
			Color		= 0,	// view raw color
			LinearDepth	= 1,	// gray scale, depth converted to linear scape
			ExpDepth	= 2,	// gray scale, depth not converted
			Normals		= 3,	// convert signed normalized to unsigned
		};
	};



	//
	// Renderer
	//

	class Renderer : public BaseObject
	{
	// types
	private:
		struct TextureViewerData
		{
			float2	scale;
			float2	bias;
			float2	valueScaleBias;
			int		viewMode;

			TextureViewerData ()
			{
				bias			= float2( 0.0f );
				scale			= float2( 1.0f );
				valueScaleBias	= float2( 1.0f, 0.0f );
				viewMode		= EImageViewMode::Color;
			}
		};

		template <bool IsInt>
		struct TextureViewShader
		{
			ShaderProgramPtr					prog;
			UniformBlock< TextureViewerData >	ub;
			TextureUniform						texture;

			TextureViewShader (GX_DEFCTOR) : texture(IsInt)
			{}
		};


	// variables
	private:
		MeshGenerator::Mesh			_quadMesh;
		TextureViewShader<false>	_viewImageShader;
		TextureViewShader<true>		_viewImageShaderI;	// integer image
		TextureViewShader<true>		_viewImageShaderU;	// unsigned integer image


	// methods
	public:
		Renderer (const SubSystemsRef ss);
		
		bool Initialize ();
		void Deinitialize ();
		bool Reload ();

		void DrawImage (const Shared::ScaleBiasController &controller, const ComputeImagePtr &image, EImageViewMode::type mode);
		void DrawImage (const ComputeImagePtr &image, const RectF &region, EImageViewMode::type mode);


	private:
		void _DrawImage (const ComputeImagePtr &image, const float2 &scale, const float2 &bias, EImageViewMode::type mode);
	};


}	// ShaderEditor
