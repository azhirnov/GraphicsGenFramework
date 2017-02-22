// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "TessLandscapeSample.h"
#include "Projects/ShaderEditor/Core/ShaderEditor.h"

namespace ShaderEditor
{
	
/*
=================================================
	constructor
=================================================
*/
	TessLandscapeSample::TessLandscapeSample (const SubSystemsRef ss) :
		ISample( ss ),		_tileInitializer( SubSystems() ),
		_wireframe( false )
	{
	}
	
/*
=================================================
	Init
=================================================
*/
	void TessLandscapeSample::Init ()
	{
		Ptr<TilesManager>	tm = SubSystems()->Get< ShaderEditorCore >()->GetTilesManager();
		
		tm->SetTileInitializer( &_tileInitializer );
		tm->ResetCamera();

		_tileInitializer.Init();
		
		_renderState = RenderState();
		_renderState.depth.test				= true;
		_renderState.cullMode.enabled		= true;
		_renderState.cullMode.cullBackFace	= true;
		_renderState.cullMode.frontFaceCCW	= false;

		_wireframe = false;

		Reload();
	}
	
/*
=================================================
	Release
=================================================
*/
	void TessLandscapeSample::Release ()
	{
		SubSystems()->Get< ShaderEditorCore >()->GetTilesManager()->SetTileInitializer( null );
	}
	
/*
=================================================
	Reload
=================================================
*/
	void TessLandscapeSample::Reload ()
	{
		_tileInitializer.Reload();
	}
	
/*
=================================================
	Draw
=================================================
*/
	void TessLandscapeSample::Draw (const RenderTargetPtr &rt, TimeD globalTime)
	{
		// simple sky
		rt->SetClearColor( ERenderTarget::Color0, float4( 0.5f, 0.75f, 1.0f, 1.0f ) );
		rt->Clear();

		SubSystems()->Get< GraphicsEngine >()->GetStateManager()->SetRenderState( _renderState );

		SubSystems()->Get< ShaderEditorCore >()->GetTilesManager()->Draw( rt );
	}
	
/*
=================================================
	Update
=================================================
*/
	void TessLandscapeSample::Update (TimeD dt)
	{
		SubSystems()->Get< ShaderEditorCore >()->GetTilesManager()->Update( dt );

		Ptr< Input >	input = SubSystems()->Get< Input >();

		if ( input->IsKeyClicked( EKey::O ) )
			_wireframe = not _wireframe;

		_renderState.polygon.mode = _wireframe ? EPolygonMode::Line : EPolygonMode::Fill;
	}
	
/*
=================================================
	Next
=================================================
*/
	bool TessLandscapeSample::Next ()
	{
		return false;
	}
	
/*
=================================================
	Init
=================================================
*/
	void TessLandscapeSample::TileInitializer::Init ()
	{
		_planeMesh = TilesManager::CreateTilePlane( SubSystems(), _GetTileMeshVertices() );
	}

/*
=================================================
	Reload
=================================================
*/
	void TessLandscapeSample::TileInitializer::Reload ()
	{
		IGeneratorPtr	new_generator	= IGenerator::Create_TessLandscape( SubSystems() );
		IShaderPtr		new_shader		= IShader::Create_TessLandscape( SubSystems() );

		if ( new_shader and
			 new_generator and
			 new_generator->Compile() )
		{
			_shader			= new_shader;
			_landscapeGen	= new_generator;
		}
	}
	
/*
=================================================
	InitTile
=================================================
*/
	void TessLandscapeSample::TileInitializer::InitTile (const TilePtr &tile)
	{
		VertexBufferPtr	const&	in_vb		= _planeMesh.vertexBuffer;
		VertexBufferPtr			out_vb		= New<VertexBuffer>( SubSystems() );
		TexturePtr				out_diff	= Texture::NewTex2D( SubSystems(), uint2(512), EPixelFormat::RGBA8_UNorm );
		TexturePtr				out_disp	= Texture::NewTex2D( SubSystems(), uint2(512), EPixelFormat::RGBA16F );

		CHECK( out_vb->Create() );
		CHECK( out_vb->Allocate( in_vb->Size(), EBufferUsage::Dynamic ) );

		out_vb->SetAttribs( in_vb->Attribs(), in_vb->Stride() );
		out_vb->SetPrimitive( in_vb->Primitive() );

		ComputeBufferPtr	cbuf_in		= ComputeBuffer::New( in_vb, EMemoryAccess::Read );
		ComputeBufferPtr	cbuf_out	= ComputeBuffer::New( out_vb, EMemoryAccess::Write );
		usize				num_verts	= in_vb->Count();
		uint2				dimension	= uint2( _GetTileMeshVertices() );
		float				scale		= tile->GetTileSize();
		float2				position	= tile->GetTransform().Position().xz();
		
		_landscapeGen->SetArg( "inBuffer",				cbuf_in );
		_landscapeGen->SetArg( "outBuffer",				cbuf_out );
		_landscapeGen->SetArg( "numVertices",			num_verts );
		_landscapeGen->SetArg( "dimension",				dimension );
		_landscapeGen->SetArg( "position",				position );
		_landscapeGen->SetArg( "scale",					scale );
		_landscapeGen->SetArg( "outDiffuseTex",			out_diff );
		_landscapeGen->SetArg( "outDisplacementMap",	out_disp );

		_landscapeGen->Render();


		Tile::LandscapeTileData	landscape;

		landscape.vertices	= out_vb;
		landscape.indices	= _planeMesh.indexBuffer;
		landscape.program	= _shader;
		landscape.textures	<< out_diff << out_disp;

		tile->SetLandscape( landscape );
	}


}	// ShaderEditor

