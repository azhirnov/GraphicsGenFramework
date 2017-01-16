// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#include "ShadertoySamples.h"
#include "../../Core/ShaderEditor.h"

namespace ShaderEditor
{
	
/*
=================================================
	constructor
=================================================
*/
	ShadertoySamples::ShadertoySamples (const SubSystemsRef ss) :
		ISample( ss ),
		_controller( SubSystems() ),
		_cellSize(10),	_subCellSize(4)
	{
	}
		
/*
=================================================
	Init
=================================================
*/
	void ShadertoySamples::Init ()
	{
		_imageGen	= _ChooseShader();
		_image		= ComputeImage::New( SubSystems() );

		CHECK( _image->Create( uint2(1 << 12).xyoo(), ETexture::Tex2D, EPixelFormat::RGBA8_UNorm ) );

		CHECK( _imageGen->Compile() );
	}
	
/*
=================================================
	Release
=================================================
*/
	void ShadertoySamples::Release ()
	{
		_imageGen	= null;
		_image		= null;
	}
	
/*
=================================================
	Reload
=================================================
*/
	void ShadertoySamples::Reload ()
	{
		CHECK( _imageGen->Compile() );
	}
	
/*
=================================================
	_ChooseShader
=================================================
*/
	IGeneratorPtr ShadertoySamples::_ChooseShader () const
	{
		return IGenerator::CreateMicroschemeShader( SubSystems() );
	}

/*
=================================================
	Draw
=================================================
*/
	void ShadertoySamples::Draw (const RenderTargetPtr &rt, Time<double> globalTime)
	{
		// resize target
		/*if ( Any( uint2(rt->GetViewport().Size()) != _image->Dimension().xy() ) )
		{
			CHECK( _image->Create( uint2(rt->GetViewport().Size()).xyoo(), ETexture::Tex2D, EPixelFormat::RGBA8_UNorm ) );
		}*/

		SubSystems()->Get< GraphicsEngine >()->GetStateManager()->SetRenderState( RenderState() );
		rt->Clear();

		_imageGen->SetArg( "globalTime",	globalTime );
		_imageGen->SetArg( "outImage",		_image );
		_imageGen->SetArg( "cellSize",		_cellSize );
		_imageGen->SetArg( "subCellSize",	_subCellSize );
		
		_imageGen->Render();

		SubSystems()->Get< ShaderEditorCore >()->GetRenderer()->
			DrawImage( _controller, _image, EImageViewMode::Color );
	}
	
/*
=================================================
	Update
=================================================
*/
	void ShadertoySamples::Update (Time<double> dt)
	{
		_controller.Update( dt );
	}

/*
=================================================
	New
=================================================
*/
	ISamplePtr  ShadertoySamples::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new ShadertoySamples( ss ) );
	}


}	// ShaderEditor
