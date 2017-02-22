// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "ShaderEditor.h"

namespace ShaderEditor
{

/*
=================================================
	constructor
=================================================
*/
	ShaderEditorCore::ShaderEditorCore (const IParallelThreadPtr &thread) :
		GameApplication( thread ),
		_graphicsEngine( SubSystems() ),	_computeEngine( SubSystems() ),
		_renderer( SubSystems() ),			_tilesManager( SubSystems() ),
		_activeSample( -1 ),				_timePaused( false ),
		_initialized( false ),				_makeScreenShot( false )
	{
		SubSystems()->GetSetter< ShaderEditorCore >().Set( this );

		CHECK( SubSystems()->Get< FileManager >()->FindAndSetCurrentDir( "Projects/ShaderEditor" ) );

		CHECK( Logger::GetInstance()->Open( "log", false ) );
	}
	
/*
=================================================
	destructor
=================================================
*/
	ShaderEditorCore::~ShaderEditorCore ()
	{
		SubSystems()->GetSetter< ShaderEditorCore >().Set( null );
	}
	
/*
=================================================
	AddSample
=================================================
*/
	void ShaderEditorCore::AddSample (const ISamplePtr &sample)
	{
		_samples.PushBack( sample );

		if ( _activeSample == -1 )
		{
			_ActiveNextSample();
		}
	}

/*
=================================================
	_ActiveNextSample
=================================================
*/
	void ShaderEditorCore::_ActiveNextSample ()
	{
		_time.SetSeconds( 0.0 );

		if ( _activeSample < _samples.Count() )
		{
			if ( _samples[ _activeSample ]->Next() )
				return;

			_samples[ _activeSample ]->Release();
		}

		usize	index = Wrap( _activeSample + 1, 0u, _samples.LastIndex() );

		_activeSample = usize(-1);

		if ( index < _samples.Count() )
		{
			_activeSample = index;

			if ( _initialized )
				_samples[ _activeSample ]->Init();
		}
	}

/*
=================================================
	_Draw
=================================================
*/
	void ShaderEditorCore::_Draw ()
	{
		RenderTargetPtr const&	rt = _graphicsEngine.GetRenderTargetManager()->GetCurrent();
		
		rt->Bind();

		if ( _activeSample < _samples.Count() )
		{
			_samples[ _activeSample ]->Draw( rt, _time );
		}
		else
		{
			rt->SetClearColor( ERenderTarget::Color0, float4( 1.0f, 0.0f, 0.0f, 1.0f ) );
			rt->Clear();
		}
		
		if ( _makeScreenShot )
		{
			_makeScreenShot = false;

			CHECK( SubSystems()->Get< GraphicsEngine >()->GetRenderTargetManager()->
				MakeScreenShot( "images/screenshot.png", ESaveImageFormat::PNG_Flipped, true ) );
		}

		SubSystems()->Get< GraphicsEngine >()->FlushFrame();
	}

/*
=================================================
	_Update
=================================================
*/
	void ShaderEditorCore::_Update (TimeD dt, bool)
	{
		if ( not _initialized )
			return;

		Ptr< Input >	input = SubSystems()->Get< Input >();

		if ( input->IsKeyClicked( EKey::ESCAPE ) )
		{
			SubSystems()->Get< Platform >()->Exit();
			return;
		}

		if ( input->IsKeyClicked( EKey::R ) )
			_Reload();
		
		// reset timer
		if ( input->IsKeyClicked( EKey::T ) )
			_time.SetSeconds( 0.0 );

		// pause time 
		if ( input->IsKeyClicked( EKey::P ) )
			_timePaused = not _timePaused;

		if ( not _timePaused )
			_time += dt;

		// screenshot
		if ( input->IsKeyClicked( EKey::PRINTSCREEN ) )
			_makeScreenShot = true;

		if ( input->IsKeyClicked( EKey::RETURN ) )
			_ActiveNextSample();
		
		if ( _activeSample < _samples.Count() )
			_samples[ _activeSample ]->Update( dt );

		_Draw();
	}
	
/*
=================================================
	_OnInit
=================================================
*/
	void ShaderEditorCore::_OnInit ()
	{
		CHECK( SubSystems()->Get< Platform >()->
			InitWindow( Platform::WindowDesc( "ShaderEditor", uint2(800, 600), MinValue<int2>(), false, true ) ) );
		
		CHECK( SubSystems()->Get< Platform >()->
			InitRender( VideoSettings( VideoSettings::RGBA8, VideoSettings::DEPTH_24, VideoSettings::NO_STENCIL,
										0, VideoSettings::AUTO, VideoSettings::CONTEXT_DEBUG, true, false ) ) );
	}
	
/*
=================================================
	_OnSurfaceCreated
=================================================
*/
	extern void InitializeShaderSamples (Ptr< ShaderEditorCore > core);

	void ShaderEditorCore::_OnSurfaceCreated ()
	{
		CHECK( SubSystems()->Get< GraphicsEngine >()->Initialize() );
		CHECK( SubSystems()->Get< ComputeEngine >()->Initialize() );

		// setup shader manager
		{
			Ptr< ShaderManager >	sm = SubSystems()->Get< GraphicsEngine >()->GetShaderManager();

			sm->AddIncludeDirectory( "gl" );
			sm->AddImportDirectory( "gl" );
			sm->SetDebugOutputFolder( "gl/_DebugOutput" );
		}

		CHECK( _renderer.Initialize() );
		CHECK( _tilesManager.Initialize() );
		
		InitializeShaderSamples( this );
		_time.SetSeconds( 0.0 );
		
		if ( _activeSample < _samples.Count() )
			_samples[ _activeSample ]->Init();

		_initialized = true;
	}
	
/*
=================================================
	_OnExit
=================================================
*/
	void ShaderEditorCore::_OnExit ()
	{
		if ( _activeSample < _samples.Count() )
			_samples[ _activeSample ]->Release();

		_activeSample = -1;
		_samples.Clear();

		_tilesManager.Deinitialize();
		_renderer.Deinitialize();

		SubSystems()->Get< ComputeEngine >()->Deinitialize();
		SubSystems()->Get< GraphicsEngine >()->Deinitialize();
	}
	
/*
=================================================
	_OnResized
=================================================
*/
	void ShaderEditorCore::_OnResized ()
	{
	}
	
/*
=================================================
	_Reload
=================================================
*/
	void ShaderEditorCore::_Reload ()
	{
		//SubSystems()->Get< GraphicsEngine >()->GetStateManager()->Reset();
		SubSystems()->Get< GraphicsEngine >()->GetShaderManager()->ClearShaderCache();

		CHECK( _renderer.Reload() );
		CHECK( _tilesManager.Reload() );

		if ( _activeSample < _samples.Count() )
			_samples[ _activeSample ]->Reload();
	}

}	// ShaderEditor

MAIN_APPLICATION_ENTRY( ShaderEditor::ShaderEditorCore );
