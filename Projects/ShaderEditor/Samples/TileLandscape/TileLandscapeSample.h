// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../../Core/ISample.h"
#include "../../Tiles/Tile.h"

namespace ShaderEditor
{
	
	//
	// Tile Landscape Sample
	//

	class TileLandscapeSample : public ISample
	{
	// types
	private:

		class TileInitializer : public ITileInitializer
		{
		// variables
		private:
			IGeneratorPtr			_landscapeGen;
			IShaderPtr				_shader;
			MeshGenerator::Mesh		_planeMesh;

			
		// methods
		public:
			TileInitializer (const SubSystemsRef ss) : ITileInitializer(ss) {}

			void Init ();
			void Reload ();

			void InitTile (const TilePtr &tile) override;
		};


	// variables
	private:
		TileInitializer		_tileInitializer;
		RenderState			_renderState;


	// methods
	private:
		TileLandscapeSample (const SubSystemsRef ss);

		static uint	_GetTileMeshVertices ()		{ return 257; }

	public:
		void Init () override;
		void Release () override;
		void Reload () override;

		void Draw (const RenderTargetPtr &rt, Time<double> globalTime) override;
		void Update (Time<double> dt) override;

		static ISamplePtr  New (const SubSystemsRef ss);
	};


}	// ShaderEditor
