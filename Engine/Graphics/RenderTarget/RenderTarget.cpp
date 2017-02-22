// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "RenderTarget.h"
#include "Engine/Graphics/Engine/GraphicsEngine.h"

namespace Engine
{
namespace Graphics
{
	
/*
=================================================
	constructor
=================================================
*/
	RenderTarget::RenderTarget (bool isSystemRT, const SubSystemsRef ss) :
		BaseObject( ss ),
		_changed( true ), _isSRGB( false ), _isSystemRT( isSystemRT )
	{
	}

/*
=================================================
	destructor
=================================================
*/
	RenderTarget::~RenderTarget ()
	{
		Destroy();
	}
	
/*
=================================================
	Create
=================================================
*/
	bool RenderTarget::Create ()
	{
		Destroy();

		Ptr< GraphicsContext >	gc = SubSystems()->Get< GraphicsEngine >()->GetContext();

		if ( _isSystemRT )
		{
			CHECK_ERR( gc->CreateSystemRenderTarget( OUT _rt ) );
			_InitSystemRT();
		}
		else
		{
			CHECK_ERR( gc->CreateRenderTarget( OUT _rt ) );
		}
		return true;
	}
	
/*
=================================================
	_InitSystemRT
=================================================
*/
	void RenderTarget::_InitSystemRT ()
	{
		VideoSettings	vs;
		SubSystems()->Get< Platform >()->GetVideoSettings( vs );

		_targets[ ERenderTarget::Color0 ].enabled = true;

		if ( vs.depthBits != VideoSettings::NO_DEPTH )
			_targets[ ERenderTarget::Depth ].enabled = true;

		if ( vs.stencilBits != VideoSettings::NO_STENCIL )
			_targets[ ERenderTarget::Stencil ].enabled = true;

		_viewports.Resize( 1 );

		_changed		= false;
		_isSRGB			= false;
		_fragmentOut	= FragmentOutputState().Set( EFragOutput::Float4, ERenderTarget::Color0 );
	}

/*
=================================================
	Destroy
=================================================
*/
	void RenderTarget::Destroy ()
	{
		if ( not _isSystemRT and SubSystems()->Get< GraphicsEngine >() )
		{
			// unbind and delete
			SubSystems()->Get< GraphicsEngine >()->GetContext()->DeleteRenderTarget( _rt );
		}

		_targets.Clear();
		_viewports.Clear();

		_changed		= true;
		_isSRGB			= false;
		_fragmentOut	= FragmentOutputState();
	}
	
/*
=================================================
	Bind
=================================================
*/
	void RenderTarget::Bind ()
	{
		Ptr< StateManager >	sm = SubSystems()->Get< GraphicsEngine >()->GetStateManager();
		
		_UpdateDrawBuffers();

		sm->BindRenderTarget( _rt );
		sm->EnableFramebufferSRGB( _isSRGB );

		//sm->PushViewport();

		DEBUG_ONLY( _CheckViewports(); )

		if ( _viewports.Count() == 1 )
			sm->Viewport( _viewports.Front() );
		else
			sm->Viewports( _viewports );

		DBG_CHECK( _CheckStatus() );
	}
	
/*
=================================================
	Unbind
=================================================
*/
	void RenderTarget::Unbind ()
	{
		Ptr< StateManager >	sm = SubSystems()->Get< GraphicsEngine >()->GetStateManager();

		//sm->BindRenderTarget( RenderTargetID() );
		//sm->PopViewport();
	}

/*
=================================================
	IsBinded
=================================================
*/
	bool RenderTarget::IsBinded () const
	{
		return SubSystems()->Get< GraphicsEngine >()->GetStateManager()->
			IsRenderTargetBinded( _rt );
	}
	
/*
=================================================
	DetachAll
=================================================
*/
	void RenderTarget::DetachAll ()
	{
		FOR( i, _targets ) {
			Detach( ERenderTarget::type(i) );
		}
	}
	
/*
=================================================
	Detach
=================================================
*/
	bool RenderTarget::Detach (const TexturePtr &texture)
	{
		CHECK_ERR( not _isSystemRT );
		CHECK_ERR( texture );
		
		_changed = true;

		FOR( i, _targets )
		{
			if ( texture == _targets[i].texture )
			{
				Detach( ERenderTarget::type(i) );
				break;
			}
		}
		return true;
	}
	
/*
=================================================
	_BeforeAttach
=================================================
*/
	void RenderTarget::_BeforeAttach (ERenderTarget::type target)
	{
		if ( target == ERenderTarget::Depth )
			_targets[ ERenderTarget::DepthStencil ].Detach();
		
		if ( target == ERenderTarget::Stencil )
			_targets[ ERenderTarget::DepthStencil ].Detach();
		
		if ( target == ERenderTarget::DepthStencil )
		{
			_targets[ ERenderTarget::Depth ].Detach();
			_targets[ ERenderTarget::Stencil ].Detach();
		}
	}
	
/*
=================================================
	_OnAttached
=================================================
*/
	void RenderTarget::_OnAttached (ERenderTarget::type target)
	{
		if ( target >= ERenderTarget::Color0	and
			 _targets[ target ].IsEnabled()		and
			 _targets[ target ].texture )
		{
			_fragmentOut.Set(
				EFragOutput::From( _targets[ target ].texture->PixelFormat() ),
				target
			);
		}
	}
	
/*
=================================================
	_OnDetached
=================================================
*/
	void RenderTarget::_OnDetached (ERenderTarget::type target)
	{
		if ( target >= ERenderTarget::Color0 )
		{
			_fragmentOut.Set( EFragOutput::Unknown, target );
		}
	}

/*
=================================================
	SetClearColor
=================================================
*/
	void RenderTarget::SetClearColor (ERenderTarget::type target, color4u color)
	{
		SetClearColor( target, color.To<color4f>() );
	}

	void RenderTarget::SetClearColor (ERenderTarget::type target, const color4f &color)
	{
		SetClearColor( target, color.To<float4>() );
	}

	void RenderTarget::SetClearColor (ERenderTarget::type target, const int4 &color)
	{
		//CHECK_ERR( target >= ERenderTarget::Color0 and target < (isize)_targets.Count(), void() );

		_targets[ target ].clearValue.Create( color );
	}

	void RenderTarget::SetClearColor (ERenderTarget::type target, const uint4 &color)
	{
		//CHECK_ERR( target >= ERenderTarget::Color0 and target < (isize)_targets.Count(), void() );

		_targets[ target ].clearValue.Create( color );
	}

	void RenderTarget::SetClearColor (ERenderTarget::type target, const float4 &color)
	{
		//CHECK_ERR( target >= ERenderTarget::Color0 and target < (isize)_targets.Count(), void() );

		_targets[ target ].clearValue.Create( color );
	}
	
/*
=================================================
	SetClearDepth
=================================================
*/
	void RenderTarget::SetClearDepth (float depth)
	{
		_targets[ ERenderTarget::Depth ].clearValue.Create( depth );
	}
	
/*
=================================================
	SetClearStencil
=================================================
*/
	void RenderTarget::SetClearStencil (int stencil)
	{
		_targets[ ERenderTarget::Stencil ].clearValue.Create( stencil );
	}
	
/*
=================================================
	ClearOnly
=================================================
*/
	void RenderTarget::ClearOnly (ERenderTarget::type target)
	{
		CHECK( IsBinded() );

		_ClearTarget( target );
	}

/*
=================================================
	HasTarget
=================================================
*/
	bool RenderTarget::HasTarget (ERenderTarget::type target) const
	{
		if ( target == ERenderTarget::Depth )
			return _targets[ ERenderTarget::Depth ].IsEnabled() or _targets[ ERenderTarget::DepthStencil ].IsEnabled();

		if ( target == ERenderTarget::Stencil )
			return _targets[ ERenderTarget::Stencil ].IsEnabled() or _targets[ ERenderTarget::DepthStencil ].IsEnabled();

		if ( target == ERenderTarget::DepthStencil )
			return (_targets[ ERenderTarget::Depth ].IsEnabled() and _targets[ ERenderTarget::Stencil ].IsEnabled()) or
					_targets[ ERenderTarget::DepthStencil ].IsEnabled();

		if ( target >= ERenderTarget::Color0 and target < (isize)_targets.Count() )
			return _targets[ target ].IsEnabled();

		return false;
	}

/*
=================================================
	HasDepth
=================================================
*/
	bool RenderTarget::HasDepth () const
	{
		return HasTarget( ERenderTarget::Depth );
	}
	
/*
=================================================
	HasStencil
=================================================
*/
	bool RenderTarget::HasStencil () const
	{
		return HasTarget( ERenderTarget::Stencil );
	}
	
/*
=================================================
	HasColor
=================================================
*/
	bool RenderTarget::HasColor () const
	{
		FORv( i, ERenderTarget::Color0, _targets )
		{
			if ( _targets[ i ].IsEnabled() )
				return true;
		}
		return false;
	}

/*
=================================================
	SetViewport
=================================================
*/
	void RenderTarget::SetViewport (const uint2 &size)
	{
		SetViewport( RectU( 0, 0, size.x, size.y ) );
	}

	void RenderTarget::SetViewport (const RectU &viewport)
	{
		_viewports.Resize( 1 );
		_viewports[0] = viewport;
	}

/*
=================================================
	SetViewports
=================================================
*/
	void RenderTarget::SetViewports (Buffer<const uint2> sizes)
	{
		CHECK( _viewports.MaxCapacity() >= sizes.Count() );

		_viewports.Resize( Min( _viewports.MaxCapacity(), sizes.Count() ) );

		FOR( i, _viewports ) {
			_viewports[i] = RectU( 0, 0, sizes[i].x, sizes[i].y );
		}
	}

	void RenderTarget::SetViewports (Buffer<const RectU> viewports)
	{
		CHECK( _viewports.MaxCapacity() >= viewports.Count() );

		_viewports.Resize( Min( _viewports.MaxCapacity(), viewports.Count() ) );
		
		FOR( i, _viewports ) {
			_viewports[i] = viewports[i];
		}
	}
	
/*
=================================================
	ComputeViewport
=================================================
*/
	void RenderTarget::ComputeViewport ()
	{
		uint2	size		= uint2( GlobalConst::Graphics_MaxTextureSize );
		uint	num_active	= 0;

		FOR( i, _targets )
		{
			if ( _targets[i].IsEnabled() and _targets[i].texture )
			{
				size = Min( size, _targets[i].texture->Dimension().xy() );
				++num_active;
			}
		}

		if ( num_active > 0 )
			SetViewport( size );
	}

/*
=================================================
	GetFormat
=================================================
*/
	EPixelFormat::type RenderTarget::GetFormat (ERenderTarget::type target) const
	{
		CHECK_ERR( HasTarget( target ) );

		TexturePtr const &	tex = GetTexture( target );

		if ( tex )
			return tex->PixelFormat();

		if ( IsSystemRenderTarget() )
		{
			VideoSettings	vs;
			SubSystems()->Get< Platform >()->GetVideoSettings( vs );

			if ( target == ERenderTarget::Color0 )
			{
				switch ( vs.colorFormat )
				{
					case VideoSettings::R8_G8_B8_A8 :	return EPixelFormat::RGBA8_UNorm;
					case VideoSettings::R4_G4_B4_A4 :	return EPixelFormat::RGBA4_UNorm;
					case VideoSettings::R5_G6_B5 :		return EPixelFormat::RGB_5_6_5_UNorm;
				}
			}

			if ( target == ERenderTarget::Depth )
			{
				switch ( vs.depthBits )
				{
					case VideoSettings::DEPTH_16 :	return EPixelFormat::Depth16;
					case VideoSettings::DEPTH_24 :	return EPixelFormat::Depth24;
					case VideoSettings::DEPTH_32 :	return EPixelFormat::Depth32;
				}
			}

			if ( target == ERenderTarget::Stencil )
			{
				switch ( vs.stencilBits )
				{
					case VideoSettings::STENCIL_8 :	return EPixelFormat::Depth24_Stencil8;	// TODO
				}
			}
		}

		RETURN_ERR( "no pixel format for target!" );
	}
	
/*
=================================================
	_CheckViewports
----
	debug check
=================================================
*/
	void RenderTarget::_CheckViewports () const
	{
		// all active viewports must not be zero size!

		Buffer< const RectU >	vp = _viewports;

		FOR( i, vp ) {
			ASSERT( All( vp[i].Size() > uint2( 0 ) ) );
		}
	}

/*
=================================================
	NewSystemRT
=================================================
*/
	RenderTargetPtr  RenderTarget::NewSystemRT (const SubSystemsRef ss)
	{
		return New<RenderTarget>( RenderTarget( true, ss ) );
	}


}	// Graphics
}	// Engine
