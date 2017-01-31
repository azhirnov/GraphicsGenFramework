// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "TilesManager.h"

namespace ShaderEditor
{
	
/*
=================================================
	constructor
=================================================
*/
	TilesManager::TilesManager (const SubSystemsRef ss) :
		BaseObject( ss ),
		_controller( SubSystems() ),
		_maxTileSize( 10.0f ),		_globalZoom( 0 ),
		_maxZoom( 4 ),				_maxTiles( 0 )
	{
	}

/*
=================================================
	destructor
=================================================
*/
	TilesManager::~TilesManager ()
	{
	}
	
/*
=================================================
	Initialize
=================================================
*/
	bool TilesManager::Initialize ()
	{
		const real2	clip_planes	= _controller.GetClipPlanes();
		const uint	max_zoom	= 5;

		_maxTileSize	= (real) NearPowerOfTwo( int( clip_planes.y ) );
		_maxZoom		= Clamp( IntLog2( (int)_maxTileSize ), 1u, max_zoom );
		_maxTiles		= (uint) Ceil( clip_planes.y * 4.0f / _maxTileSize );
		_globalZoom		= 0;

		CHECK_ERR( Reload() );

		ResetCamera();
		return true;
	}
	
/*
=================================================
	Reload
=================================================
*/
	bool TilesManager::Reload ()
	{
		_ClearTiles();

		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void TilesManager::Deinitialize ()
	{
		_ClearTiles();

		_initializer = null;
	}
	
/*
=================================================
	ResetCamera
=================================================
*/
	void TilesManager::ResetCamera ()
	{
		_controller.Reset( Transformation<real>( real3( 0.0f, 5.0f, 0.0f ), quat() ) );

		_globalZoom = 0;

		_ClearTiles();
		_UpdateTiles();
	}

/*
=================================================
	Draw
=================================================
*/
	void TilesManager::Draw (const RenderTargetPtr &rt)
	{
		struct RecursiveDraw
		{
			const real4x4	viewProjMat;
			const real3		cameraPos;
			const real		globalScale;

			RecursiveDraw (CameraController_t &controller, real globalScale) :
				cameraPos( controller.GetPosition() ),
				viewProjMat( controller.GetViewProjMatrix() ),
				globalScale( globalScale )
			{}

			void DrawTile (const TilePtr &tile) const
			{
				if ( tile->_initialized )
					tile->_Draw( cameraPos * globalScale, viewProjMat );
			}

			void operator () (const TilePtr &tile) const
			{
				if ( not tile->_hasNodes )
				{
					DrawTile( tile );
					return;
				}

				// is all nodes initialized
				bool	is_init = true;

				FOR( i, tile->_nodes ) {
					is_init &= tile->_nodes[i]->_initialized;
				}

				if ( not is_init )
				{
					DrawTile( tile );
					return;
				}

				FOR( i, tile->_nodes ) {
					(*this)( tile->_nodes[i] );
				}
			}
		};

		RecursiveDraw	draw_helper( _controller, _GetGlobalScale() );

		FOR( i, _tiles )
		{
			// TODO: check visibility
			
			draw_helper( _tiles[i] );
		}
	}
	
/*
=================================================
	Update
=================================================
*/
	void TilesManager::Update (TimeD dt)
	{
		real3 const	prev_pos	= _controller.GetPosition();

		_controller.Update( dt );

		_UpdateGlobalScale();
		_UpdateTiles();

		// TODO: update frustum

		_InitializeTiles();
	}

/*
=================================================
	SetTileInitializer
=================================================
*/
	void TilesManager::SetTileInitializer (const ITileInitializerPtr &initializer)
	{
		_ClearTiles();

		_initializer = initializer;
	}
	
/*
=================================================
	_ClearTIles
=================================================
*/
	void TilesManager::_ClearTiles ()
	{
		_tiles.Clear();
		_pendingToInit.Clear();
	}
	
/*
=================================================
	_UpdateTiles
=================================================
*/
	void TilesManager::_UpdateTiles ()
	{
		struct RecursiveDivide
		{
			Queue< TilePtr >&	pendingToInit;
			real3 const			cameraPos;
			real const			farPlane;
			int const			maxZoom;
			int const			globalZoom;

			RecursiveDivide (Queue<TilePtr> &pendingToInit, const real3 &cameraPos, real farPlane, int maxZoom, int globalZoom) :
				pendingToInit( pendingToInit ), cameraPos( cameraPos ),
				farPlane( farPlane ), maxZoom( maxZoom ), globalZoom( globalZoom )
			{}

			void operator () (const TilePtr &base_tile) const
			{
				// check detail level
				int		zoom = 0;
				{
					// tile quad corner points
					float4x4	mat;
					base_tile->_transform.GetMatrix( OUT mat );

					real3	p0 = base_tile->_transform.Transform( real2( 0.0f, 0.0f ).xoy() );
					real3	p1 = base_tile->_transform.Transform( real2( 0.0f, 1.0f ).xoy() );
					real3	p2 = base_tile->_transform.Transform( real2( 1.0f, 0.0f ).xoy() );
					real3	p3 = base_tile->_transform.Transform( real2( 1.0f, 1.0f ).xoy() );

					real	md   = MaxValue<real>();
					RectF	bbox = RectF( p0.xz(), p0.xz() ).Join( p1.xz() ).Join( p2.xz() ).Join( p3.xz() );

					if ( All( cameraPos.xz() >= bbox.LeftBottom() ) and
						 All( cameraPos.xz() <= bbox.RightTop() ) )
					{
						// camera into tile quad in 2D
						md = Abs( p0.y - cameraPos.y );
					}
					else
					{
						// camera outside of tile quad
						md = Min( Min( p0.Distance( cameraPos ), p1.Distance( cameraPos ) ),
								  Min( p2.Distance( cameraPos ), p3.Distance( cameraPos ) ) );
					}

					zoom = RoundToInt( Log2( farPlane / md ) ) - globalZoom;
				}

				if ( base_tile->_zoom < zoom	and
					 base_tile->_zoom < maxZoom	and
					 not base_tile->_hasNodes )
				{
					// increase detail level
					FOR( j, base_tile->_nodes )
					{
						TilePtr						tile 	= Tile::New( base_tile->SubSystems() );
						Transformation<real> const&	base_tr	= base_tile->_transform;
						int2						index	= usize2( j & 1, j >> 1 ).To<int2>();

						tile->_Create(
							Transformation<real>(
								base_tr.Position() + base_tr.GetScale().x * 0.5f * real2(index).xoy(),
								base_tr.Orientation(),
								base_tr.GetScale() * real3( 0.5f, 1.0f, 0.5f ) ),
							index,
							base_tile->_zoom + 1
						);

						base_tile->_nodes[j] = tile;
						pendingToInit.PushBack( tile );

						//(*this)( tile );
					}

					base_tile->_hasNodes = true;
				}
				else
				if ( base_tile->_hasNodes )
				{
					if ( zoom < base_tile->_zoom )
					{
						// decrease detail level
						base_tile->_nodes.Clear();
						base_tile->_hasNodes = false;
					}
					else
					{
						// check child nodes
						FOR( j, base_tile->_nodes )
						{
							(*this)( base_tile->_nodes[j] );
						}
					}
				}
			}
		};


		const real		global_scale	= _GetGlobalScale();
		const real3		camera_pos		= _controller.GetPosition() * global_scale;
		const real		far_plane		= _controller.GetClipPlanes().y;
		const real		max_tile_size	= _maxTileSize;
		const int2		new_center		= RoundToInt( camera_pos.xz() / max_tile_size );
		const RectI		new_rect		= RectI( new_center - _maxTiles/2, new_center + _maxTiles/2 );

		RecursiveDivide	div_helper( _pendingToInit, camera_pos, far_plane, _maxZoom, _globalZoom );

		// create tiles
		if ( _tiles.Empty() )
		{
			_tilesRect = new_rect;

			const int2	rect_size = _tilesRect.Size();

			_tiles.Resize( rect_size.Area() );

			FOR( i, _tiles )
			{
				TilePtr	tile 	= Tile::New( SubSystems() );
				int2	index	= usize2( i % rect_size.x, i / rect_size.x ).To<int2>() + _tilesRect.LeftBottom();

				ASSERT( All( index >= _tilesRect.LeftBottom() ) and
						All( index <  _tilesRect.RightTop() ) );
				
				tile->_Create(
					Transformation<real>( max_tile_size * real2(index).xoy(),
										  quat(),
										  real3( max_tile_size, 1.0f, max_tile_size ) ),
					index,
					0
				);

				_tiles[i] = tile;
				_pendingToInit.PushBack( tile );
			}
		}

		// divide tiles
		FOR( i, _tiles )
		{
			// move to other grid cell
			bool	in_rect =	All( _tiles[i]->_index >= new_rect.LeftBottom() ) and
								All( _tiles[i]->_index <  new_rect.RightTop() );

			if ( not in_rect )
			{
				Transformation<real> const&	tr = _tiles[i]->_transform;

				TilePtr	tile 	= Tile::New( SubSystems() );
				int2	index	= Wrap( _tiles[i]->_index, new_rect.LeftBottom(), new_rect.RightTop()-1 );

				ASSERT( All( index >= new_rect.LeftBottom() ) and
						All( index <  new_rect.RightTop() ) );

				tile->_Create(
					Transformation<real>( max_tile_size * real2(index).xoy(),
										  tr.Orientation(),
										  tr.GetScale() ),
					index,
					0
				);
				
				_tiles[i] = tile;
				_pendingToInit.PushBack( tile );
			}

			div_helper( _tiles[i] );
		}

		_tilesRect = new_rect;
	}
	
/*
=================================================
	_InitializeTiles
=================================================
*/
	void TilesManager::_InitializeTiles ()
	{
		// init tile (1 per frame)
		if ( not _pendingToInit.Empty() )
		{
			TilePtr tile = _pendingToInit.Front();	_pendingToInit.PopFront();

			if ( _initializer )
				_initializer->InitTile( tile );

			tile->_initialized = true;
		}
	}
	
/*
=================================================
	_UpdateGlobalScale
=================================================
*/
	void TilesManager::_UpdateGlobalScale ()
	{
		//const real3		camera_pos	= _controller.GetPosition();
		//const real		far_plane	= _controller.GetClipPlanes().y * 0.25f;

		int		zoom = 0; //RoundToInt( Log2( far_plane / camera_pos.y ) );

		if ( zoom != _globalZoom )
		{
			_globalZoom = zoom;

			_ClearTiles();
		}
	}
	
/*
=================================================
	_GetGlobalScale
=================================================
*/
	real TilesManager::_GetGlobalScale () const
	{
		return _globalZoom >= 0 ? real(1 << _globalZoom) : 1.0f / real(1 << -_globalZoom);
	}
	
/*
=================================================
	constructor
=================================================
*/
	TilesManager::MeshVertex::MeshVertex (const float3 &position, const float3 &normal, const float2 &texcoord)
	{
		this->position	= position;
		this->normal	= normal;
		this->texcoord	= texcoord;
	}
	
/*
=================================================
	GetAttribs
=================================================
*/
	void TilesManager::MeshVertex::GetAttribs (OUT VertexAttribsDesc &desc)
	{
		desc.Add( VertexAttribsDesc::AttribIndex(0), &MeshVertex::position );
		desc.Add( VertexAttribsDesc::AttribIndex(1), &MeshVertex::normal,	true );
		desc.Add( VertexAttribsDesc::AttribIndex(2), &MeshVertex::texcoord,	true );
	}

/*
=================================================
	_GetGlobalScale
=================================================
*/
	MeshGenerator::Mesh TilesManager::CreateTilePlane (const SubSystemsRef ss, uint numVertices)
	{
		CHECK_ERR( numVertices > 0 );

		Array<MeshVertex>	vertices;
		Array<uint>			indices;
		uint				count		= numVertices + 1;
		uint				max_size	= numVertices;
		float const			uv_scale	= 1.0f / numVertices;
		float3 const		pos_scale	= float3( uv_scale, uv_scale, 1.0f );
		bool const			addBorder	= true;

		if ( addBorder )
		{
			numVertices	+= 2;
			count		+= 2;
		}

		vertices.Reserve( count * count );
		indices.Reserve( numVertices * numVertices * 4 );

		for (uint h = 0; h < count; ++h)
		{
			for (uint w = 0; w < count; ++w)
			{
				float3	pos;

				if ( addBorder )
				{
					pos = Clamp( int2(w, h)-1, int2(0), int2(max_size) ).To<float2>().xyo();

					if ( w == 0 or h == 0 or w == numVertices or h == numVertices )
						pos.z = -1.0f;
				}
				else
					pos = int2(w, h).To<float2>().xyo();

				vertices.PushBack( MeshVertex(
					(pos * pos_scale).xzy(),
					float3( 0.0f, 0.0f, 1.0f ),
					pos.xy() * uv_scale
				) );

				if ( w != 0 and h != 0 )
				{
					indices.PushBack( h*count		+ w );
					indices.PushBack( h*count		+ w-1 );
					indices.PushBack( (h-1)*count	+ w-1 );

					indices.PushBack( (h-1)*count	+ w-1 );
					indices.PushBack( (h-1)*count	+ w );
					indices.PushBack( h*count		+ w );
				}
			}
		}

		VertexAttribsDesc	attr;
		MeshVertex::GetAttribs( attr );
		
		MeshGenerator::Mesh	mesh( VertexBuffer::New( ss ), IndexBuffer::New( ss ) );

		CHECK( mesh.vertexBuffer->Create() );
		CHECK( mesh.vertexBuffer->SetData( BinaryBuffer::From( vertices ), EBufferUsage::Static ) );

		mesh.vertexBuffer->SetAttribs( attr, BytesU::SizeOf<MeshVertex>() );
		mesh.vertexBuffer->SetPrimitive( EPrimitive::Triangle );

		CHECK( mesh.indexBuffer->Create() );
		CHECK( mesh.indexBuffer->SetData( BinaryBuffer::From( indices ), EBufferUsage::Static ) );

		mesh.indexBuffer->SetIndexType( EIndex::UInt );

		return mesh;
	}


}	// ShaderEditor
