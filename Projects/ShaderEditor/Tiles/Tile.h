// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Projects/ShaderEditor/Core/Common.h"
#include "Projects/ShaderEditor/Core/IShader.h"

namespace ShaderEditor
{
	
	class Tile;
	SHARED_POINTER( Tile );
	

	//
	// Tile Initializer interface
	//

	class ITileInitializer : public BaseObject
	{
	// methods
	protected:
		ITileInitializer (const SubSystemsRef ss) : BaseObject( ss )
		{}
		
	public:
		virtual void InitTile (const TilePtr &tile) = 0;
	};

	SHARED_POINTER( ITileInitializer );



	//
	// Tile
	//

	class Tile : public BaseObject
	{
		friend class TilesManager;

	// types
	public:
		typedef FixedSizeArray< TexturePtr, 8 >		TextureArray_t;
		typedef Transformation<real>				Transformation_t;
		typedef real4x4								Matrix_t;

		struct LandscapeTileData
		{
		// variables
			IShaderPtr			program;
			VertexBufferPtr		vertices;
			IndexBufferPtr		indices;
			TextureArray_t		textures;

		// methods
			LandscapeTileData ()
			{}
		};


	// variables
	private:
		LandscapeTileData			_landscape;

		// changed by tiles manager
		StaticArray< TilePtr, 4 >	_nodes;
		Transformation_t			_transform;
		int2						_index;
		int							_zoom;
		bool						_initialized;
		bool						_hasNodes;


	// methods
	protected:
		Tile (const SubSystemsRef ss);
		~Tile ();

		void _Create (const Transformation_t &tr, const int2 &index, int zoom);

		void _Draw (const real3 &cameraPos, const Matrix_t &viewProjMat);
		
	public:
		void SetLandscape (const LandscapeTileData &landscape);

		uint	GetZoomLevel ()	const	{ return _zoom; }
		float	GetScale ()		const;

		Transformation_t const&	GetTransform ()	const	{ return _transform; }

		static TilePtr  New (const SubSystemsRef ss);
	};


}	// ShaderEditor
