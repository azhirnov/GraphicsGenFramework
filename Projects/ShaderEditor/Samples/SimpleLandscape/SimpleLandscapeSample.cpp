// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "SimpleLandscapeSample.h"
#include "Projects/ShaderEditor/Core/ShaderEditor.h"

namespace ShaderEditor
{
	
/*
=================================================
	constructor
=================================================
*/
	SimpleLandscapeSample::SimpleLandscapeSample (const SubSystemsRef ss) :
		ISample( ss ),		_tileInitializer( SubSystems() )
	{
	}
	
/*
=================================================
	Init
=================================================
*/
	void SimpleLandscapeSample::Init ()
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
	void SimpleLandscapeSample::Release ()
	{
		SubSystems()->Get< ShaderEditorCore >()->GetTilesManager()->SetTileInitializer( null );
	}
	
/*
=================================================
	Reload
=================================================
*/
	void SimpleLandscapeSample::Reload ()
	{
		IShaderPtr	new_shader = IShader::Create_Simple3D( SubSystems() );

		if ( new_shader )
			_shader = new_shader;

		_tileInitializer.Reload();
	}
	
/*
=================================================
	Draw
=================================================
*/
	void SimpleLandscapeSample::Draw (const RenderTargetPtr &rt, TimeD globalTime)
	{
		// simple sky
		rt->SetClearColor( ERenderTarget::Color0, float4( 0.5f, 0.75f, 1.0f, 1.0f ) );
		rt->Clear();

		SubSystems()->Get< GraphicsEngine >()->GetStateManager()->SetRenderState( _renderState );

		Ptr< TilesManager >	tm = SubSystems()->Get< ShaderEditorCore >()->GetTilesManager();
		
		tm->Draw( rt );

		/*Frustum<real> const&	frustum = tm->GetCameraFrustum();
		real4x4 const&			vp_mat	= tm->GetCameraViewProj();

		_shader->Bind();*/
	}
	
/*
=================================================
	Update
=================================================
*/
	void SimpleLandscapeSample::Update (TimeD dt)
	{
		SubSystems()->Get< ShaderEditorCore >()->GetTilesManager()->Update( dt );
	}
	
/*
=================================================
	Next
=================================================
*/
	bool SimpleLandscapeSample::Next ()
	{
		// no more shaders
		return false;
	}

/*
=================================================
	New
=================================================
*/
	ISamplePtr  SimpleLandscapeSample::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new SimpleLandscapeSample( ss ) );
	}
	
/*
=================================================
	Init
=================================================
*/
	void SimpleLandscapeSample::TileInitializer::Init ()
	{
		_planeMesh = TilesManager::CreateTilePlane( SubSystems(), _GetTileMeshVertices() );
	}

/*
=================================================
	Reload
=================================================
*/
	void SimpleLandscapeSample::TileInitializer::Reload ()
	{
		IGeneratorPtr	new_generator	= IGenerator::Create_SimpleLandscape( SubSystems() );
		IShaderPtr		new_shader		= IShader::Create_Simple3D( SubSystems() );

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
	void SimpleLandscapeSample::TileInitializer::InitTile (const TilePtr &tile)
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

