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
		ISample( ss ),		_tileInitializer( SubSystems() )
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
		_renderState.depth.test = true;

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
	New
=================================================
*/
	ISamplePtr  TessLandscapeSample::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new TessLandscapeSample( ss ) );
	}
	
/*
=================================================
	Init
=================================================
*/
	void TessLandscapeSample::TileInitializer::Init ()
	{
		/*CHECK( Shared::ImageLoader::Load( _texture, SubSystems(), "images/grass.png" ) );
		
		_texture->GenerateMipmaps();

		SamplerPtr	sampler;

		CHECK( SubSystems()->Get< GraphicsEngine >()->GetContext()->CreateSampler(
			SamplerState( EWrapMode::Clamp, EFilter::MinMagMipLinear ), OUT sampler ) );

		_texture->SetSampler( sampler );*/

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
		VertexBufferPtr	const&	in_vb	= _planeMesh.vertexBuffer;
		VertexBufferPtr			out_vb	= VertexBuffer::New( SubSystems() );
		TexturePtr				out_tex	= Texture::NewTex2D( SubSystems(), uint2(512), EPixelFormat::RGBA8_UNorm );

		CHECK( out_vb->Create() );
		CHECK( out_vb->Allocate( in_vb->Size(), EBufferUsage::Dynamic ) );

		out_vb->SetAttribs( in_vb->Attribs(), in_vb->Stride() );
		out_vb->SetPrimitive( in_vb->Primitive() );

		ComputeBufferPtr	cbuf_in		= ComputeBuffer::New( in_vb, EMemoryAccess::Read );
		ComputeBufferPtr	cbuf_out	= ComputeBuffer::New( out_vb, EMemoryAccess::Write );
		usize				num_verts	= in_vb->Count();
		uint2				dimension	= uint2( _GetTileMeshVertices() );
		float				scale		= tile->GetTransform().GetScale().x;
		float2				position	= tile->GetTransform().Position().xz();
		
		_landscapeGen->SetArg( "inBuffer",		cbuf_in );
		_landscapeGen->SetArg( "outBuffer",		cbuf_out );
		_landscapeGen->SetArg( "numVertices",	num_verts );
		_landscapeGen->SetArg( "dimension",		dimension );
		_landscapeGen->SetArg( "position",		position );
		_landscapeGen->SetArg( "scale",			scale );
		_landscapeGen->SetArg( "outDiffuseTex",	out_tex );

		_landscapeGen->Render();


		Tile::LandscapeTileData	landscape;

		landscape.vertices	= out_vb;
		landscape.indices	= _planeMesh.indexBuffer;
		landscape.program	= _shader;
		landscape.textures	<< out_tex;

		tile->SetLandscape( landscape );
	}


}	// ShaderEditor

