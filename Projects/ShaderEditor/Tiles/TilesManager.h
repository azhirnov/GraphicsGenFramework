// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Tile.h"

namespace ShaderEditor
{

	//
	// Tiles Manager
	//

	class TilesManager : public BaseObject
	{
	// types
	private:
		struct MeshVertex
		{
		// variables
			float3	position;
			float3	normal;
			float2	texcoord;

		// methods
			MeshVertex () {}

			MeshVertex (const float3 &position, const float3 &normal, const float2 &texcoord);
			
			static void GetAttribs (OUT VertexAttribsDesc &desc);
		};

		typedef Shared::FreeCameraController	CameraController_t;
		//typedef Shared::FlightCameraController	CameraController_t;

	public:
		typedef CameraController_t::Camera_t	Camera_t;
		typedef CameraController_t::Transform_t	Transform_t;
		typedef CameraController_t::Frustum_t	Frustum_t;
		typedef CameraController_t::mat4_t		Matrix_t;


	// variables
	private:
		CameraController_t		_controller;
		
		Array< TilePtr >		_tiles;
		Queue< TilePtr >		_pendingToInit;
		RectI					_tilesRect;

		ITileInitializerPtr		_initializer;
		real					_maxTileSize;
		int						_globalZoom;	// if tile is too small, than use global scale to increase view distance
		uint					_maxZoom;		// 0 - maxTileSize, 1 - maxTileSize/2, ...
		uint					_maxTiles;		// in 0 zoom level


	// methods
	public:
		TilesManager (const SubSystemsRef ss);
		~TilesManager ();

		bool Initialize ();
		void Deinitialize ();
		bool Reload ();

		void ResetCamera ();

		void Draw (const RenderTargetPtr &rt);
		void Update (TimeD dt);

		void SetTileInitializer (const ITileInitializerPtr &initializer);

		Transform_t const&	GetCameraTransform ()	{ return _controller.GetTransform(); }
		Frustum_t const&	GetCameraFrustum ()		{ return _controller.GetFrustum(); }
		Matrix_t const&		GetCameraViewProj ()	{ return _controller.GetViewProjMatrix(); }


		static MeshGenerator::Mesh CreateTilePlane (const SubSystemsRef ss, uint vertices);

	private:
		void _ClearTiles ();
		void _UpdateTiles ();
		void _InitializeTiles ();
		void _UpdateGlobalScale ();
		real _GetGlobalScale () const;
	};


}	// ShaderEditor
