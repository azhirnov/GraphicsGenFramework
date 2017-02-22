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
		ISample( ss ),				_controller( SubSystems() ),
		_currentGenerator( 0 ),		_currentShader( 0 ),
		_regenerate( true )
	{
	}
		
/*
=================================================
	Init
=================================================
*/
	void RayTracingSamples::Init ()
	{
		_imageGen	= _ChooseShader( _currentGenerator, _currentShader );
		_image		= New<ComputeImage>( SubSystems() );

		CHECK( _image->Create( uint2(1<<12, 1<<11).xyoo(), ETexture::Tex2D, EPixelFormat::RGBA8_UNorm ) );

		CHECK( _imageGen->Compile() );
			

		SamplerPtr	sampler;

		CHECK( SubSystems()->Get< GraphicsEngine >()->GetContext()->CreateSampler(
			SamplerState( EWrapMode::Clamp, EFilter::MinMagMipLinear ), OUT sampler ) );

		_image->GetSharedObject()->SetSampler( sampler );


		_regenerate	= true;
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
		IGeneratorPtr	new_generator = _ChooseShader( _currentGenerator, _currentShader );

		if ( new_generator and new_generator->Compile() )
		{
			_regenerate	= true;
			_imageGen	= new_generator;
		}
	}
	
/*
=================================================
	_ChooseShader
=================================================
*/
	IGeneratorPtr RayTracingSamples::_ChooseShader (uint &genIndex, uint &shIndex) const
	{
		IGeneratorPtr	ptr = null;

		switch ( genIndex )
		{
			case 0 :
			{
				if ( shIndex == 0 )
				{
					uint2	cellSize( 10 );
					uint2	subCellSize( 4 );

					ptr = IGenerator::Create_Microscheme( SubSystems() );
					ptr->SetArg( "cellSize",	cellSize );
					ptr->SetArg( "subCellSize",	subCellSize );
					break;
				}
				genIndex ++;
				shIndex  = 0;
			}
			case 1 :
			{
				if ( shIndex == 0 )
				{
					ptr = IGenerator::Create_ThousandsOfStars( SubSystems() );
					break;
				}
				genIndex ++;
				shIndex  = 0;
			}
			case 2 :
			{
				ptr = IGenerator::Create_SinglePassRayTracing2D( SubSystems() );

				if ( ptr->SetArg( "shader_index", shIndex ) )
					break;
				
				genIndex ++;
				shIndex  = 0;
			}
			case 3 :
			{
				ptr = IGenerator::Create_SinglePassRayTracing3D( SubSystems() );

				if ( ptr->SetArg( "shader_index", shIndex ) )
					break;
				
				genIndex ++;
				shIndex  = 0;
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
		// update image
		TimeD	dt = globalTime - _prevGlobalTime;

		_prevGlobalTime = globalTime;

		if ( not dt.IsZero() or _regenerate )
		{
			_regenerate = false;

			_imageGen->SetArg( "globalTime",	globalTime );
			_imageGen->SetArg( "timeDelta",		dt );
			_imageGen->SetArg( "outImage",		_image );
		
			_imageGen->Render();
		}

		// draw
		SubSystems()->Get< GraphicsEngine >()->GetStateManager()->SetRenderState( RenderState() );

		rt->SetClearColor( ERenderTarget::Color0, float4() );
		rt->Clear();

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
		IGeneratorPtr	new_generator = _ChooseShader( _currentGenerator, ++_currentShader );
		
		if ( new_generator and new_generator->Compile() )
		{
			_regenerate	= true;
			_imageGen	= new_generator;
			return true;
		}

		_currentGenerator	= 0;
		_currentShader		= 0;
		return false;
	}


}	// ShaderEditor
