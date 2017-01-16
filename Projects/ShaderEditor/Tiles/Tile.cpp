// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#include "Tile.h"

namespace ShaderEditor
{
	
/*
=================================================
	constructor
=================================================
*/
	Tile::Tile (const SubSystemsRef ss) :
		BaseObject( ss ),
		_zoom( 0 ),	_initialized( false ),		_hasNodes( false )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	Tile::~Tile ()
	{
	}
	
/*
=================================================
	SetLandscape
=================================================
*/
	void Tile::SetLandscape (const LandscapeTileData &landscape)
	{
		CHECK( not _initialized );

		_landscape	= landscape;
	}
	
/*
=================================================
	_Create
=================================================
*/
	void Tile::_Create (const Transformation_t &tr, const int2 &index, int zoom)
	{
		_transform		= tr;
		_index			= index;
		_zoom			= zoom;
		_initialized	= false;
		_hasNodes		= false;
	}

/*
=================================================
	_Draw
=================================================
*/
	void Tile::_Draw (const real3 &cameraPos, const Matrix_t &viewProjMat)
	{
		CHECK( _initialized );

		Matrix_t	model_mat;	_transform.GetModelMatrix( cameraPos, OUT model_mat );
		Matrix_t	mvp_mat = viewProjMat * model_mat;

		_landscape.program->SetData( "mvp", VariantRef::From( mvp_mat ) );

		_landscape.program->SetTextures( _landscape.textures );
		_landscape.program->Active();

		if ( _landscape.indices.IsNotNull() )
			_landscape.vertices->DrawIndexed( _landscape.indices );
		else
			_landscape.vertices->Draw();
	}
	
/*
=================================================
	GetScale
=================================================
*/
	real Tile::GetScale () const
	{
		return _zoom >= 0 ? real(1 << _zoom) : 1.0f / real(1 << -_zoom);
	}

/*
=================================================
	New
=================================================
*/
	TilePtr  Tile::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new Tile( ss ) );
	}


}	// ShaderEditor
