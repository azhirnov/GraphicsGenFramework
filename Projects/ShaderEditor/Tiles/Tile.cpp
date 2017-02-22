// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

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
		_zoom( 0 ),				_tileSize( 1.0f ),
		_initialized( false ),	_hasNodes( false )
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
	void Tile::_Create (const Transformation_t &tr, float size, const int2 &index, int zoom)
	{
		_transform		= tr;
		_tileSize		= size;
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
	void Tile::_Draw (const real3 &cameraPos, const Matrix4_t &viewMat, const Matrix4_t &projMat)
	{
		CHECK( _initialized );

		Matrix4_t	model_mat;	_transform.GetModelMatrix( cameraPos, OUT model_mat );

		Matrix4_t	mv_mat		= viewMat * model_mat;
		Matrix4_t	mvp_mat		= projMat * viewMat * model_mat;
		Matrix3_t	norm_mat	= model_mat.To<Matrix3_t>().Inverse().Transpose();

		_landscape.program->SetArg( "mvp",		mvp_mat );
		_landscape.program->SetArg( "mv",		mv_mat );
		_landscape.program->SetArg( "model",	model_mat );
		_landscape.program->SetArg( "normal",	norm_mat );

		_landscape.program->SetTextures( _landscape.textures );
		_landscape.program->Active();

		if ( _landscape.indices )
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


}	// ShaderEditor
