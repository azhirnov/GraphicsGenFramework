// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "RayTracingSamples.h"
#include "Projects/ShaderEditor/Core/ShaderEditor.h"

namespace ShaderEditor
{
	
/*
=================================================
	constructor
=================================================
*/
	RayTracingSamples::RayTracingSamples (const SubSystemsRef ss) :
		ISample( ss ), _controller( SubSystems() ),
		_curShader( 0 )
	{
	}
		
/*
=================================================
	Init
=================================================
*/
	void RayTracingSamples::Init ()
	{
		_imageGen	= _ChooseShader( _curShader );
		_image		= ComputeImage::New( SubSystems() );

		CHECK( _image->Create( uint2(1 << 12).xyoo(), ETexture::Tex2D, EPixelFormat::RGBA8_UNorm ) );

		CHECK( _imageGen->Compile() );
	}
	
/*
=================================================
	Release
=================================================
*/
	void RayTracingSamples::Release ()
	{
		_imageGen	= null;
		_image		= null;
	}
	
/*
=================================================
	Reload
=================================================
*/
	void RayTracingSamples::Reload ()
	{
		IGeneratorPtr	new_generator = _ChooseShader( _curShader );

		if ( new_generator and new_generator->Compile() )
		{
			_imageGen = new_generator;
		}
	}
	
/*
=================================================
	_ChooseShader
=================================================
*/
	IGeneratorPtr RayTracingSamples::_ChooseShader (uint index) const
	{
		IGeneratorPtr	ptr = null;

		switch ( index )
		{
			case 0 :
			{
				uint2	cellSize( 10 );
				uint2	subCellSize( 4 );

				ptr = IGenerator::Create_Microscheme( SubSystems() );

				ptr->SetArg( "cellSize",	cellSize );
				ptr->SetArg( "subCellSize",	subCellSize );
				break;
			}
			case 1 :
			{
				ptr = IGenerator::Create_VoronoiRecursion( SubSystems() );
				break;
			}
			case 2 :
			{
				ptr = IGenerator::Create_ThousandsOfStars( SubSystems() );
				break;
			}
		}
		return ptr;
	}

/*
=================================================
	Draw
=================================================
*/
	void RayTracingSamples::Draw (const RenderTargetPtr &rt, TimeD globalTime)
	{
		// resize target
		/*if ( Any( uint2(rt->GetViewport().Size()) != _image->Dimension().xy() ) )
		{
			CHECK( _image->Create( uint2(rt->GetViewport().Size()).xyoo(), ETexture::Tex2D, EPixelFormat::RGBA8_UNorm ) );
		}*/

		SubSystems()->Get< GraphicsEngine >()->GetStateManager()->SetRenderState( RenderState() );

		rt->SetClearColor( ERenderTarget::Color0, float4() );
		rt->Clear();

		_imageGen->SetArg( "globalTime",	globalTime );
		_imageGen->SetArg( "outImage",		_image );
		
		_imageGen->Render();

		SubSystems()->Get< ShaderEditorCore >()->GetRenderer()->
			DrawImage( _controller, _image, EImageViewMode::Color );
	}
	
/*
=================================================
	Update
=================================================
*/
	void RayTracingSamples::Update (TimeD dt)
	{
		_controller.Update( dt );
	}
	
/*
=================================================
	Next
=================================================
*/
	bool RayTracingSamples::Next ()
	{
		IGeneratorPtr	new_generator = _ChooseShader( ++_curShader );
		
		if ( new_generator and new_generator->Compile() )
		{
			_imageGen = new_generator;
			return true;
		}

		_curShader = 0;
		return false;
	}

/*
=================================================
	New
=================================================
*/
	ISamplePtr  RayTracingSamples::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new RayTracingSamples( ss ) );
	}


}	// ShaderEditor
