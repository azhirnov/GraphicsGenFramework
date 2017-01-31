// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "gl4.h"

#if defined( GRAPHICS_API_OPENGL )

#include "Engine/Graphics/RenderTarget/RenderTarget.h"
#include "Engine/Graphics/Engine/GraphicsEngine.h"

namespace Engine
{
namespace Graphics
{
	
/*
=================================================
	Clear
=================================================
*/
	void RenderTarget::Clear ()
	{
		using namespace gl;

		CHECK( IsBinded() );

		FOR( i, _targets )
		{
			_ClearTarget( ERenderTarget::type( i ) );
		}
	}
	
/*
=================================================
	_ClearTarget
=================================================
*/
	void RenderTarget::_ClearTarget (ERenderTarget::type target)
	{
		using namespace gl;

		if ( not _targets[ target ].IsEnabled() )
			return;
		
		Ptr< StateManager >		sm = SubSystems()->Get< GraphicsEngine >()->GetStateManager();

		const ClearValue_t&	cv  = _targets[ target ].clearValue;

		if ( target == ERenderTarget::Depth )
		{
			// is enabled writting to depth?
			ASSERT( sm->GetRenderState().depth.write );

			GL_CALL( glClearBufferfv( GL_DEPTH, 0, float4( cv.Get<float>( 1.0f ) ).ptr() ) );
		}
		else
		if ( target == ERenderTarget::Stencil )
		{
			// is enabled writting to stencil?
			ASSERT( sm->GetRenderState().stencil.mask != 0 );

			GL_CALL( glClearBufferiv( GL_STENCIL, 0, int4( cv.Get<int>( 0xFF ) ).ptr() ) );
		}
		else
		if ( target == ERenderTarget::DepthStencil )
		{
			// is enabled writting to depth?
			ASSERT( sm->GetRenderState().depth.write );
			
			// is enabled writting to stencil?
			ASSERT( sm->GetRenderState().stencil.mask != 0 );

			GL_CALL( glClearBufferfi( GL_DEPTH_STENCIL, 0, cv.Get<float>( 1.0f ), cv.Get<int>( 0xFF ) ) );
		}
		else
		{
			const uint	buf = target - ERenderTarget::Color0;

			// is enabled writting to color?
			ASSERT( Any( sm->GetRenderState().colors[buf].colorMask ) );

			if ( cv.Is<int4>() )
			{
				GL_CALL( glClearBufferiv( GL_COLOR, buf, cv.Get<int4>().ptr() ) );
			}
			else
			if ( cv.Is<uint4>() )
			{
				GL_CALL( glClearBufferuiv( GL_COLOR, buf, cv.Get<uint4>().ptr() ) );
			}
			else
			{
				GL_CALL( glClearBufferfv( GL_COLOR, buf, cv.Get<float4>( float4() ).ptr() ) );
			}
		}
	}

/*
=================================================
	Attach
=================================================
*/
	bool RenderTarget::Attach (const TexturePtr &texture, ERenderTarget::type target, uint level)
	{
		using namespace gl;

		CHECK_ERR( not _isSystemRT );
		CHECK_ERR( texture );
		CHECK_ERR( target < (isize)_targets.Count() );

		_BeforeAttach( target );

		_changed = true;

		GL_CHECK( glNamedFramebufferTexture( _rt.Id(), GL4Enum( target, texture->PixelFormat() ),
											 texture->GetTextureID().Id(), level ) );
		_targets[ target ].Attach( texture );
		
		_OnAttached( target );

		DBG_CHECK( _CheckStatus() );
		return true;
	}
	
/*
=================================================
	AttachLayer
=================================================
*/
	bool RenderTarget::AttachLayer (const TexturePtr &texture, ERenderTarget::type target, uint layer, uint level)
	{
		using namespace gl;

		CHECK_ERR( not _isSystemRT );
		CHECK_ERR( texture );
		CHECK_ERR( target < (isize)_targets.Count() );
		
		_BeforeAttach( target );
		
		_changed = true;

		GL_CHECK( glNamedFramebufferTextureLayer( _rt.Id(), GL4Enum( target, texture->PixelFormat() ),
												  texture->GetTextureID().Id(), layer, level ) );
		_targets[ target ].Attach( texture );
		
		_OnAttached( target );
		
		DBG_CHECK( _CheckStatus() );
		return true;
	}
	
/*
=================================================
	Detach
=================================================
*/
	void RenderTarget::Detach (ERenderTarget::type target)
	{
		using namespace gl;
		
		if ( not _targets[ target ].IsEnabled() or _isSystemRT )
			return;
		
		_changed = true;

		GL_CALL( glNamedFramebufferTexture( _rt.Id(), GL4Enum( target ), 0, 0 ) );
		_targets[ target ].Detach();

		_OnDetached( target );
	}
	
/*
=================================================
	_CheckStatus
=================================================
*/
	bool RenderTarget::_CheckStatus ()
	{
		using namespace gl;

		if ( _isSystemRT )
			return true;

		GLenum status = 0;

		GL_CALL( status = glCheckNamedFramebufferStatus( _rt.Id(), GL_DRAW_FRAMEBUFFER ) );

		if ( status == GL_FRAMEBUFFER_COMPLETE )
			return true;

		String	msg( "FrameBuffer status: " );

		switch ( status )
		{
			case GL_FRAMEBUFFER_UNDEFINED :						msg << "GL_FRAMEBUFFER_UNDEFINED";						break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT :			msg << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";			break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT :	msg << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";	break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER :		msg << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";			break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER :		msg << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";			break;
			case GL_FRAMEBUFFER_UNSUPPORTED :					msg << "GL_FRAMEBUFFER_UNSUPPORTED";					break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE :		msg << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";			break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS :		msg << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";		break;
			default :											msg << "code 0x" << String().FormatI( status, 16 );		break;
		}

		LOG( msg.cstr(), ELog::Debug );
		return false;
	}
	
/*
=================================================
	_UpdateDrawBuffers
=================================================
*/
	void RenderTarget::_UpdateDrawBuffers ()
	{
		using namespace gl;

		if ( not _changed or _isSystemRT )
			return;

		_changed = false;
		
		StaticArray< GLenum, GlobalConst::Graphics_MaxColorBuffers >	draw_buffers( GL_NONE );

		bool	any_srgb = false;
		bool	all_srgb = false;

		FOR( i, _targets )
		{
			const _Target&	target		= _targets[i];
			const bool		is_color	= i >= ERenderTarget::Color0;
			
			if ( target.IsEnabled() )
			{
				target.texture->Barrier( EMemoryBarrier::Framebuffer );
			}

			if ( target.IsEnabled() and is_color )
			{
				usize	idx		= i - ERenderTarget::Color0;
				bool	is_srgb	= EPixelFormat::ColorSpace::IsNonLinear( target.texture->PixelFormat() );

				draw_buffers[idx] = GL_COLOR_ATTACHMENT0 + GLenum(idx);

				any_srgb |= is_srgb;
				all_srgb &= is_srgb;
			}
		}

		GL_CALL( glNamedFramebufferDrawBuffers( _rt.Id(), GLsizei(draw_buffers.Count()), draw_buffers.ptr() ) );

		CHECK( any_srgb == all_srgb );

		_isSRGB = all_srgb;
	}
	
/*
=================================================
	GetImage
=================================================
*/
	bool RenderTarget::GetImage (OUT Buffer<ubyte> &data, EPixelFormat::type format, const RectU &region,
								 ERenderTarget::type target, BytesU align) const
	{
		using namespace gl;
		
		CHECK_ERR( HasTarget( target ) and ERenderTarget::IsColor( target ) );

		GL4InternalPixelFormat	ifmt;
		GL4PixelFormat			fmt;
		GL4PixelType			type;

		CHECK_ERR( GL4Enum( format, OUT ifmt, OUT fmt, OUT type ) );

		if ( not IsSystemRenderTarget() )
		{
			const GLenum	read_target = GL_COLOR_ATTACHMENT0 + target - ERenderTarget::Color0;

			GL_CALL( glNamedFramebufferReadBuffer( _rt.Id(), read_target ) );
		}

		// TODO: barrier?

		GL_CALL( glBindFramebuffer( GL_READ_FRAMEBUFFER, _rt.Id() ) );

		GL_CALL( glPixelStorei( GL_PACK_ALIGNMENT, GLint(align) ) );
		GL_CALL( glReadnPixels( region.left, region.bottom, region.Width(), region.Height(),
								fmt, type, GLsizei(data.Size()), data.ptr() ) );
		
		GL_CALL( glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 ) );

		return true;
	}


}	// Graphics
}	// Engine

#endif	// GRAPHICS_API_OPENGL
