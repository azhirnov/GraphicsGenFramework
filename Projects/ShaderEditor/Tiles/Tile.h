// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Projects/ShaderEditor/Core/Common.h"
#include "Projects/ShaderEditor/Core/IShader.h"
#include "Projects/ShaderEditor/Core/Renderer.h"


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
		typedef real3x3								Matrix3_t;
		typedef real4x4								Matrix4_t;

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

		StaticArray< TilePtr, 4 >	_nodes;
		Transformation_t			_transform;

		float						_tileSize;
		int2						_index;
		int							_zoom;

		bool						_initialized;
		bool						_hasNodes;


	// methods
	protected:
		void _Create (const Transformation_t &tr, float size, const int2 &index, int zoom);

		void _Draw (const real3 &cameraPos, const Matrix4_t &viewMat, const Matrix4_t &projMat);
		
	public:
		explicit
		Tile (const SubSystemsRef ss);
		~Tile ();

		void SetLandscape (const LandscapeTileData &landscape);

		float	GetTileSize ()	const	{ return _tileSize; }
		uint	GetZoomLevel ()	const	{ return _zoom; }
		float	GetScale ()		const;

		Transformation_t const&	GetTransform ()	const	{ return _transform; }
	};


}	// ShaderEditor
