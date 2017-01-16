// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "GraphicsEngine.h"
#include "Engine/Graphics/Texture/TextureLoader.h"

namespace Engine
{
namespace Graphics
{
	
/*
=================================================
	constructor
=================================================
*/
	GraphicsEngine::GraphicsEngine (const SubSystemsRef ss) :
		BaseObject( ss ),
		_stateMngr(),
		_context( &_stateMngr ),
		_renderTargetMngr( ss ),
		_shaderMngr( ss ),
		_vendor( EVideoCardVendor::Unknown ),
		_initialized( false )
	{
		SubSystems()->GetSetter< GraphicsEngine >().Set( this );

		SubSystems()->Get< Application >()->GetEventSystem()->
			Subscribe( DelegateBuilder::Create( this, &GraphicsEngine::_OnResize ) );
	}
	
/*
=================================================
	destructor
=================================================
*/
	GraphicsEngine::~GraphicsEngine ()
	{
		SubSystems()->Get< Application >()->GetEventSystem()->
			Unsubscribe( DelegateBuilder::Create( this, &GraphicsEngine::_OnResize ) );

		SubSystems()->GetSetter< GraphicsEngine >().Set( null );
	}
	
/*
=================================================
	Initialize
=================================================
*/
	bool GraphicsEngine::Initialize ()
	{
		CHECK( not _initialized );

		CHECK_ERR( _context.Initialize() );
		CHECK_ERR( _stateMngr.Initialize() );
		CHECK_ERR( _renderTargetMngr.Initialize() );

		_vendor = _context.GetVideoCardVendor();

		Ptr< ResourceManager >	rm = SubSystems()->Get< ResourceManager >();

		rm->RegisterLoader( TextureLoader::New( SubSystems() ) );

		_initialized = true;
		return true;
	}
	
/*
=================================================
	Deinitialize
=================================================
*/
	void GraphicsEngine::Deinitialize ()
	{
		_shaderMngr.Clear();
		_renderTargetMngr.Deinitialize();
		_stateMngr.Deinitialize();
		_context.Deinitialize();

		_initialized = false;
	}
	
/*
=================================================
	FlushFrame
=================================================
*/
	void GraphicsEngine::FlushFrame ()
	{
		GetStateManager()->_ResetViewportStack();

		SubSystems()->Get< Platform >()->SwapBuffers();
	}

/*
=================================================
	_OnResize
=================================================
*/
	void GraphicsEngine::_OnResize (const SysEvent::Window &ev)
	{
		if ( not _initialized or ev.type != SysEvent::Window::RESIZED)
			return;

		Platform::WindowDesc	wnd;
		SubSystems()->Get< Platform >()->GetWindowDesc( wnd );

		_stateMngr._OnResize( wnd.size );
		_renderTargetMngr._OnResize( wnd.size );
	}


}	// Graphics
}	// Engine
