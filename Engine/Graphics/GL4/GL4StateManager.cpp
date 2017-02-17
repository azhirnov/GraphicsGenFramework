// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "GL4StateManager.h"

#if defined( GRAPHICS_API_OPENGL )

#include "Engine/Graphics/Texture/Sampler.h"
#include "Engine/Graphics/VertexAttribs/VertexAttribs.h"

namespace Engine
{
namespace Graphics
{
	using namespace gl;
	
/*
=================================================
	constructor
=================================================
*/
	GL4StateManager::GL4StateManager () :
		_activeUnit(0), _maxAnisotropy(0), _currentVA(0), _currentProgram(0), _currentRenderTarget(0),
		_tessellationPatch(0), _maxColorAttachments(0), _framebufferSRGB(false), 
		_memBarrierBeforeDraw( EMemoryBarrier::Unknown ), _barrierTicks( 1 ),
		_memBarrierLastTicks( 0 )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	GL4StateManager::~GL4StateManager ()
	{
	}

/*
=================================================
	Initialize
=================================================
*/
	bool GL4StateManager::Initialize ()
	{
		// texture units
		{
			GLint	max_units = 0;

			GL_CALL( glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_units ) );
			_textureUnits.Resize( Max( max_units, 16/*from specs*/ ), false );

			//const uint	max_textures = GlobalConst::Graphics_MaxMaterialTextures + GlobalConst::Graphics_MaxRenderPassTextures + 1;
			//CHECK( max_textures < max_units );

			GL_CALL( glGetIntegerv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLint*) &_maxAnisotropy ) );
		}

		// image units
		{
			GLint	max_units = 0;

			GL_CALL( glGetIntegerv( GL_MAX_IMAGE_UNITS, &max_units ) );

			_imageUnits.Resize( Max( max_units, 8/*from specs*/ ), false );
		}

		// uniform buffer
		{
			GLint	max_uniform_buffers = 0;
			GL_CALL( glGetIntegerv( GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_uniform_buffers ) );

			_buffers[ EBufferTarget::Uniform ].Resize( max_uniform_buffers, false );
		}
		
		// shader storage buffer
		{
			static const GLenum storageBlockNames[] = {
				GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS,
				GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,
				GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS,
				GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,
				GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,
				GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS
			};

			int max_ssb = 0;
		
			for (usize i = 0; i < CountOf(storageBlockNames); ++i)
			{
				GLint value = 0;
				glGetIntegerv( storageBlockNames[i], &value );

				max_ssb = Max( max_ssb, value );
			}

			_buffers[ EBufferTarget::ShaderStorage ].Resize( max_ssb, false );
		}

		// atomic counter buffer
		{
			GLint	max_atomic_buffers = 0;
			glGetIntegerv( GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, &max_atomic_buffers );

			_buffers[ EBufferTarget::AtomicCounter ].Resize( max_atomic_buffers, false );
		}

		// transform feedback buffer
		{
			_buffers[ EBufferTarget::TransformFeedback ].Resize( 1, false );
		}

		// query
		{
			FOR( i, _queryMaxIndices )
			{
				glGetQueryiv( GL4Enum( EQuery::type(i) ), GL_QUERY_COUNTER_BITS, (GLint *) &_queryMaxIndices[i] );
			}
		}

		GL_CALL( glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, (GLint*) &_maxColorAttachments ) );

		GL_CALL( glEnable( GL_DITHER ) );
		GL_CALL( glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS ) );

		int	viewport[4] = {};
		GL_CALL( glGetIntegerv( GL_VIEWPORT, viewport ) );

		_viewportStack.SetDefault( RectU( viewport[0], viewport[1], viewport[2] + viewport[0], viewport[3] + viewport[1] ) );

		GL_CALL( glGetFloatv( GL_POINT_SIZE_RANGE, _pointSizeRange.ptr() ) );

		return true;
	}
	
/*
=================================================
	Deinitialize
=================================================
*/
	void GL4StateManager::Deinitialize ()
	{
		_buffers.Clear();
		_textureUnits.Clear();
		_viewportStack.Clear();

		_activeUnit				= 0;
		_maxAnisotropy			= 0;
		_currentVA				= 0;
		_attribsState			= VertexAttribsState();
		_tessellationPatch		= 0;
		_framebufferSRGB		= false;
		_renderState			= RenderState();
		_currentProgram			= ProgramID();
		_currentRenderTarget	= 0;
		_maxColorAttachments	= 0;
		_memBarrierBeforeDraw	= EMemoryBarrier::Unknown;
		_barrierTicks			= 0;
	}
	
/*
=================================================
	TextureBarrier
=================================================
*/
	void GL4StateManager::TextureBarrier ()
	{
		GL_CALL( glTextureBarrier() );
	}

/*
=================================================
	MemoryBarrier
=================================================
*/
	void GL4StateManager::MemoryBarrier (INOUT MemoryBarrierObj &obj, EMemoryBarrier::type flags)
	{
		obj.Filter( INOUT flags );

		_FilterMemoryBarrier( obj.Ticks(), INOUT flags );

		if ( flags == EMemoryBarrier::Unknown )
		{
			obj.Reset();
			return;
		}
		
		_memBarrierBeforeDraw = _memBarrierBeforeDraw ^ (flags & _memBarrierBeforeDraw);

		_MemoryBarrier( flags );

		_IncBarrierTicks();
	}
	
/*
=================================================
	MemoryBarrierBeforeDraw
=================================================
*/
	void GL4StateManager::MemoryBarrierBeforeDraw (INOUT MemoryBarrierObj &obj, EMemoryBarrier::type flags)
	{
		obj.Filter( INOUT flags );

		_FilterMemoryBarrier( obj.Ticks(), INOUT flags );
		
		if ( flags == EMemoryBarrier::Unknown )
		{
			obj.Reset();
			return;
		}
		
		_memBarrierBeforeDraw |= flags;
	}
	
/*
=================================================
	CreateBarrier
=================================================
*/
	void GL4StateManager::CreateBarrier (OUT MemoryBarrierObj &obj)
	{
		obj = MemoryBarrierObj( (_barrierTicks + 1) & _BARRIER_TICKS_MASK );
	}
	
/*
=================================================
	_SetMemoryBarrierBeforeDraw
=================================================
*/
	void GL4StateManager::_SetMemoryBarrierBeforeDraw ()
	{
		if ( _memBarrierBeforeDraw != EMemoryBarrier::Unknown )
		{
			_MemoryBarrier( _memBarrierBeforeDraw );
		}

		_IncBarrierTicks();

		_memBarrierBeforeDraw = EMemoryBarrier::Unknown;
	}
	
/*
=================================================
	_IncBarrierTicks
=================================================
*/
	void GL4StateManager::_IncBarrierTicks ()
	{
		_barrierTicks = (_barrierTicks + 1) & _BARRIER_TICKS_MASK;
	}
	
/*
=================================================
	_MemoryBarrier
=================================================
*/
	void GL4StateManager::_MemoryBarrier (EMemoryBarrier::type flags)
	{
		FOR( i, _memBarrierLastTicks )
		{
			EMemoryBarrier::type	f = EMemoryBarrier::type(1 << i);

			if ( EnumEq( flags, f ) )
			{
				_memBarrierLastTicks[i] = _barrierTicks;
			}
		}

		GL_CALL( glMemoryBarrier( GL4Enum( flags ) ) );
	}
	
/*
=================================================
	_FilterMemoryBarrier
=================================================
*/
	void GL4StateManager::_FilterMemoryBarrier (uint ticks, INOUT EMemoryBarrier::type &flags)
	{
		EMemoryBarrier::type	filtered = EMemoryBarrier::Unknown;

		if ( ticks == MemoryBarrierObj::TicksInit() or
			 flags == EMemoryBarrier::Unknown )
		{
			flags = filtered;
			return;
		}
		
		const uint	c0	= ticks;

		FOR( i, _memBarrierLastTicks )
		{
			EMemoryBarrier::type	f = EMemoryBarrier::type(1 << i);
			
			uint	t0	= _memBarrierLastTicks[i] & _BARRIER_TICKS_MASK;

			bool	intersects	= c0 > t0;	// TODO: check overflow

			if ( EnumEq( flags, f ) and intersects )
			{
				filtered |= f;
			}
		}

		flags = filtered;
	}

/*
=================================================
	ClientSynchronize
=================================================
*/
	void GL4StateManager::ClientSynchronize ()
	{
		GL_CALL( glFinish() );

		_barrierTicks += 100;
		_IncBarrierTicks();
	}
	
/*
=================================================
	Synchronize
=================================================
*/
	void GL4StateManager::Synchronize ()
	{
		GL_CALL( glFlush() );

		//_IncBarrierTicks();
	}

/*
=================================================
	ActiveTexture
=================================================
*/
	void GL4StateManager::ActiveTexture (const TextureID &tex)
	{
		const uint	unit = GetMaxTextureUnits()-1;
		
		if ( _activeUnit != unit )
		{
			_activeUnit = unit;
			GL_CALL( glActiveTexture( GL_TEXTURE0 + _activeUnit ) );
		}
		
		if ( _textureUnits[ _activeUnit ].binded != tex )
		{
			GL_CALL( glBindTexture( tex.Target(), tex.Id() ) );
			_textureUnits[ _activeUnit ].binded = tex;
		}
	}

/*
=================================================
	BindTexture
=================================================
*/
	void GL4StateManager::BindTexture (const TextureID &id, uint unit)
	{
		if ( _textureUnits[ unit ].binded == id )
			return;
		
		GL_CALL( glBindMultiTexture( unit, id.Target(), id.Id() ) );
		_textureUnits[ unit ].binded = id;
	}
	
/*
=================================================
	UnbindTexture
=================================================
*/
	void GL4StateManager::UnbindTexture (const TextureID &id)
	{
		FOR( i, _textureUnits )
		{
			if ( _textureUnits[i].binded == id )
			{
				GL_CALL( glBindMultiTexture( uint(i), id.Target(), 0 ) );

				_textureUnits[i].binded = TextureID();
			}
		}
	}
	
/*
=================================================
	BindImage
=================================================
*/
	void GL4StateManager::BindImage (const TextureID &id, uint unit, MipmapLevel level, TexArrLayer layer,
									 EMemoryAccess::type access, EPixelFormat::type format)
	{
		_ImageUnit &	img = _imageUnits[ unit ];

		if ( img.binded == id )
			return;

		CHECK( EPixelFormat::IsSupportedForImage( format ) );
		
		const bool	layered = layer.IsDefined();

		GL_CALL( glBindImageTexture( unit, id.Id(), level.Get(), layered, layer.Get(),
									 GL4Enum( access ), GL4Enum( format ) ) );

		img.access	= access;
		img.binded	= id;
		img.format	= format;
		img.layer	= layer.Get();
		img.level	= level.Get();
	}
	
/*
=================================================
	UnbindImage
=================================================
*/
	void GL4StateManager::UnbindImage (const TextureID &id)
	{
		FOR( i, _imageUnits )
		{
			if ( _imageUnits[i].binded == id )
			{
				GL_CALL( glBindImageTexture( (GLuint)i, 0, 0, false, 0, GL_READ_ONLY, GL_RGBA8 ) );

				_imageUnits[ i ] = _ImageUnit();
			}
		}
	}

/*
=================================================
	BindSampler
=================================================
*/
	void GL4StateManager::BindSampler (const SamplerID &sampler, uint unit)
	{
		SamplerID&	curr_samp	= _textureUnits[ unit ].sampler;
		SamplerID	new_samp	= sampler;

		if ( curr_samp == new_samp )
			return;

		curr_samp = new_samp;
		GL_CALL( glBindSampler( unit, new_samp._id ) );
	}
	
/*
=================================================
	BindSampler
=================================================
*/
	void GL4StateManager::UnbindSampler (const SamplerID &id)
	{
		FOR( i, _textureUnits )
		{
			if ( _textureUnits[i].sampler == id )
			{
				_textureUnits[i].sampler = SamplerID();

				GL_CALL( glBindSampler( (GLuint)i, 0 ) );
			}
		}
	}

/*
=================================================
	BindAttribs
=================================================
*/
	void GL4StateManager::BindAttribs (const VertexAttribsPtr &attribs, const BufferID &vb, BytesU offset, BytesU stride, uint divisor)
	{
		VertexAttribsID const &	va			= attribs->GetID();
		const uint				bind_idx	= 0;	// TODO

		_BindVertexArray( va, attribs->GetAttribsState() );

		GL4VertexAttribsID::_VertBuf &	buf = va._vb[ bind_idx ];

		if ( buf.id      == vb.Id() and
			 buf.offset  == offset  and
			 buf.stride  == stride  and
			 buf.divisor == divisor )
			return;

		buf.id      = vb.Id();
		buf.offset  = (uint) usize(offset);
		buf.stride  = (uint) usize(stride);
		buf.divisor = divisor;

		GL_CALL( glVertexArrayVertexBuffer( va._id, bind_idx, buf.id, buf.offset, buf.stride ) );
		GL_CALL( glVertexArrayBindingDivisor( va._id, bind_idx, buf.divisor ) );
	}
	
/*
=================================================
	BindAttribs
=================================================
*/
	void GL4StateManager::BindAttribs (const VertexAttribsPtr &attribs, const BufferID &ib)
	{
		VertexAttribsID const &	va = attribs->GetID();
		
		_BindVertexArray( va, attribs->GetAttribsState() );

		if ( va._indexBuf == ib._id )
			return;

		va._indexBuf = ib._id;
		GL_CALL( glVertexArrayElementBuffer( va._id, va._indexBuf ) );
	}
	
/*
=================================================
	_BindAttribs
=================================================
*/
	void GL4StateManager::_BindAttribs (const VertexAttribsPtr &attribs)
	{
		_BindVertexArray( attribs->GetID(), attribs->GetAttribsState() );
	}

/*
=================================================
	UnbindAttribs
=================================================
*/
	void GL4StateManager::UnbindAttribs (const VertexAttribsPtr &attribs)
	{
		if ( IsCurrentAttribs( attribs ) )
			UnbindAttribs();
	}

	void GL4StateManager::UnbindAttribs ()
	{
		_BindVertexArray( VertexAttribsID(), VertexAttribsState() );
	}

/*
=================================================
	IsCurrentAttribs
=================================================
*/
	bool GL4StateManager::IsCurrentAttribs (const VertexAttribsPtr &attribs) const
	{
		return attribs->GetID()._id == _currentVA;
	}

/*
=================================================
	_BindVertexArray
=================================================
*/
	void GL4StateManager::_BindVertexArray (const VertexAttribsID &va, const VertexAttribsState &state)
	{
		if ( _currentVA == va._id )
			return;

		_attribsState	= state;
		_currentVA		= va._id;

		GL_CALL( glBindVertexArray( _currentVA ) );
	}
	
/*
=================================================
	SetRenderState
=================================================
*/
	void GL4StateManager::SetRenderState (const RenderState &state)
	{
		FOR( i, state.colors ) {
			SetColorBufferState( uint(i), state.colors[i] );
		}

		BlendColor( state.blendColor );
		SetDepthState( state.depth );
		SetStencilState( state.stencil );
		SetCullMode( state.cullMode );
		SetPolygonState( state.polygon );
		SetPointState( state.point );
	}
	
/*
=================================================
	SetColorBufferState
=================================================
*/
	void GL4StateManager::SetColorBufferState (uint index, const RenderState::ColorBufferState &state)
	{
		EnableBlend( index, state.blend );

		if ( state.blend )
		{
			BlendEq( index, state.blendModeRGB, state.blendModeA );
			ColorMask( index, state.colorMask );
			BlendFunc( index, state.blendFuncSrcRGB, state.blendFuncSrcA, state.blendFuncDstRGB, state.blendFuncDstA );
		}
	}
	
/*
=================================================
	BlendFunc
=================================================
*/
	void GL4StateManager::BlendFunc (uint index, EBlendFunc::type srcRGB, EBlendFunc::type srcA, EBlendFunc::type dstRGB, EBlendFunc::type dstA)
	{
		RenderState::ColorBufferState &	cb = _renderState.colors[ index ];

		if ( cb.blendFuncSrcRGB == srcRGB and cb.blendFuncSrcA == srcA and
			 cb.blendFuncDstRGB == dstRGB and cb.blendFuncDstA )
		{
			return;
		}

		cb.blendFuncSrcRGB	= srcRGB;
		cb.blendFuncSrcA	= srcA;
		cb.blendFuncDstRGB	= dstRGB;
		cb.blendFuncDstA	= dstA;

		GL_CALL( glBlendFuncSeparatei( index, GL4Enum( srcRGB ), GL4Enum( dstRGB ), GL4Enum( srcA ), GL4Enum( dstA ) ) );
	}
	
/*
=================================================
	BlendEq
=================================================
*/
	void GL4StateManager::BlendEq (uint index, EBlendEq::type modeRGB, EBlendEq::type modeA)
	{
		RenderState::ColorBufferState &	cb = _renderState.colors[ index ];

		if ( cb.blendModeRGB == modeRGB and cb.blendModeA == modeA )
			return;

		cb.blendModeRGB	= modeRGB;
		cb.blendModeA	= modeA;

		GL_CALL( glBlendEquationSeparatei( index, GL4Enum( modeRGB ), GL4Enum( modeA ) ) );
	}
	
/*
=================================================
	ColorMask
=================================================
*/
	void GL4StateManager::ColorMask (uint index, bool4 mask)
	{
		RenderState::ColorBufferState &	cb = _renderState.colors[ index ];

		if ( All( cb.colorMask == mask ) )
			return;

		cb.colorMask = mask;

		GL_CALL( glColorMaski( index, mask.x, mask.y, mask.z, mask.w ) );
	}
	
/*
=================================================
	BlendColor
=================================================
*/
	void GL4StateManager::BlendColor (const color4f &color)
	{
		if ( All( _renderState.blendColor == color ) )
			return;

		_renderState.blendColor = color;

		GL_CALL( glBlendColor( color.R(), color.G(), color.B(), color.A() ) );
	}

/*
=================================================
	EnableBlend
=================================================
*/
	void GL4StateManager::EnableBlend (uint index, bool enabled)
	{
		RenderState::ColorBufferState &	cb = _renderState.colors[ index ];

		if ( cb.blend == enabled )
			return;

		cb.blend = enabled;

		if ( enabled ) {
			GL_CALL( glEnablei( GL_BLEND, index ) );
		}
		else {
			GL_CALL( glDisablei( GL_BLEND, index ) );
		}
	}

/*
=================================================
	SetDepthState
=================================================
*/
	void GL4StateManager::SetDepthState (const RenderState::DepthBufferState &state)
	{
		DepthTest( state.test );
		DepthWrite( state.write );
		DepthClamp( state.clamp );
		DepthRange( state.range );
		DepthFunc( state.func );
	}
	
/*
=================================================
	DepthTest
=================================================
*/
	void GL4StateManager::DepthTest (bool enabled)
	{
		RenderState::DepthBufferState &	db = _renderState.depth;

		if ( db.test == enabled )
			return;

		db.test = enabled;

		if ( enabled ) {
			GL_CALL( glEnable( GL_DEPTH_TEST ) );
		}
		else {
			GL_CALL( glDisable( GL_DEPTH_TEST ) );
		}
	}
	
/*
=================================================
	DepthWrite
=================================================
*/
	void GL4StateManager::DepthWrite (bool enabled)
	{
		RenderState::DepthBufferState &	db = _renderState.depth;

		if ( db.write == enabled )
			return;

		db.write = enabled;

		GL_CALL( glDepthMask( enabled ) );
	}
	
/*
=================================================
	DepthClamp
=================================================
*/
	void GL4StateManager::DepthClamp (bool enabled)
	{
		RenderState::DepthBufferState &	db = _renderState.depth;

		if ( db.clamp == enabled )
			return;

		db.clamp = enabled;

		if ( enabled ) {
			GL_CALL( glEnable( GL_DEPTH_CLAMP ) );
		}
		else {
			GL_CALL( glDisable( GL_DEPTH_CLAMP ) );
		}
	}

/*
=================================================
	DepthFunc
=================================================
*/
	void GL4StateManager::DepthFunc (ECompareFunc::type func)
	{
		RenderState::DepthBufferState &	db = _renderState.depth;

		if ( db.func == func )
			return;

		db.func = func;

		GL_CALL( glDepthFunc( GL4Enum( func ) ) );
	}
	
/*
=================================================
	DepthRange
=================================================
*/
	void GL4StateManager::DepthRange (const float2 &range)
	{
		RenderState::DepthBufferState &	db = _renderState.depth;

		if ( All( db.range == range ) )
			return;

		db.range = range;

		GL_CALL( glDepthRangef( range.x, range.y ) );
	}

/*
=================================================
	SetStencilState
=================================================
*/
	void GL4StateManager::SetStencilState (const RenderState::StencilBufferState &state)
	{
		StencilOp( state.sfail, state.dfail, state.dppass );
		StencilFunc( state.func, state.funcRef, state.funcMask );
		StencilMask( state.mask );
		StencilTest( state.test );
	}
	
/*
=================================================
	StencilFunc
=================================================
*/
	void GL4StateManager::StencilFunc (ECompareFunc::type func, ubyte ref, ubyte mask)
	{
		RenderState::StencilBufferState &	sb = _renderState.stencil;

		if ( sb.func == func and sb.funcRef == ref and sb.funcMask == mask )
			return;

		sb.func		= func;
		sb.funcRef	= ref;
		sb.funcMask	= mask;

		GL_CALL( glStencilFunc( GL4Enum( func ), ref, mask ) );
	}
	
/*
=================================================
	StencilOp
=================================================
*/
	void GL4StateManager::StencilOp (EStencilOp::type sfail, EStencilOp::type dfail, EStencilOp::type dppass)
	{
		RenderState::StencilBufferState &	sb = _renderState.stencil;

		if ( sb.sfail == sfail and sb.dfail == dfail and sb.dppass == dppass )
			return;

		sb.sfail	= sfail;
		sb.dfail	= dfail;
		sb.dppass	= dppass;

		GL_CALL( glStencilOp( GL4Enum( sfail ), GL4Enum( dfail ), GL4Enum( dppass ) ) );
	}
	
/*
=================================================
	StencilMask
=================================================
*/
	void GL4StateManager::StencilMask (ubyte mask)
	{
		RenderState::StencilBufferState &	sb = _renderState.stencil;

		if ( sb.mask == mask )
			return;

		sb.mask = mask;

		GL_CALL( glStencilMask( mask ) );
	}
	
/*
=================================================
	StencilTest
=================================================
*/
	void GL4StateManager::StencilTest (bool enabled)
	{
		RenderState::StencilBufferState &	sb = _renderState.stencil;

		if ( sb.test == enabled )
			return;

		sb.test = enabled;

		if ( enabled ) {
			GL_CALL( glEnable( GL_STENCIL_TEST ) );
		}
		else {
			GL_CALL( glDisable( GL_STENCIL_TEST ) );
		}
	}

/*
=================================================
	SetCullMode
=================================================
*/
	void GL4StateManager::SetCullMode (const RenderState::CullMode &mode)
	{
		RenderState::CullMode &	cm = _renderState.cullMode;

		if ( cm.enabled != mode.enabled )
		{
			cm.enabled = mode.enabled;

			if ( mode.enabled ) {
				GL_CALL( glEnable( GL_CULL_FACE ) );
			}
			else {
				GL_CALL( glDisable( GL_CULL_FACE ) );
				return;
			}
		}

		if ( cm.cullBackFace != mode.cullBackFace )
		{
			GL_CALL( glCullFace( mode.cullBackFace ? GL_BACK : GL_FRONT ) );
		}

		if ( cm.frontFaceCCW != mode.frontFaceCCW )
		{
			GL_CALL( glFrontFace( mode.frontFaceCCW ? GL_CCW : GL_CW ) );
		}

		cm = mode;
	}
	
/*
=================================================
	SetPolygonState
=================================================
*/
	void GL4StateManager::SetPolygonState (const RenderState::PolygonStates &state)
	{
		PolygonMode( state.mode );
		PolygonOffset( state.offsetEnabled, state.offsetFactor, state.offsetUnits );
	}

/*
=================================================
	SetPolygonOffset
=================================================
*/
	void GL4StateManager::PolygonMode (EPolygonMode::type mode)
	{
		if ( _renderState.polygon.mode == mode )
			return;
		
		_renderState.polygon.mode = mode;

		GL_CALL( glPolygonMode( GL_FRONT_AND_BACK, GL4Enum( mode ) ) );
	}
	
/*
=================================================
	SetPolygonOffset
=================================================
*/
	void GL4StateManager::PolygonOffset (bool enabled, float factor, float units)
	{
		RenderState::PolygonStates &	ps = _renderState.polygon;

		if ( ps.offsetEnabled != enabled )
		{
			ps.offsetEnabled = enabled;

			if ( enabled )
			{
				GL_CALL( glEnable( GL_POLYGON_OFFSET_FILL ) );
				GL_CALL( glEnable( GL_POLYGON_OFFSET_LINE ) );
				GL_CALL( glEnable( GL_POLYGON_OFFSET_POINT ) );
			}
			else
			{
				GL_CALL( glDisable( GL_POLYGON_OFFSET_FILL ) );
				GL_CALL( glDisable( GL_POLYGON_OFFSET_LINE ) );
				GL_CALL( glDisable( GL_POLYGON_OFFSET_POINT ) );
				return;
			}
		}

		if ( Equals( ps.offsetFactor, factor ) and Equals( ps.offsetUnits, units ) )
			return;
		
		ps.offsetFactor	= factor;
		ps.offsetUnits	= units;

		GL_CALL( glPolygonOffset( factor, units ) );
	}
	
/*
=================================================
	SetPointState
=================================================
*/
	void GL4StateManager::SetPointState (const RenderState::PointState &state)
	{
		ProgramPointSize( state.programPointSize );
		
		if ( not state.programPointSize )
			PointSize( Clamp( state.pointSize, GetPointSizeRange().x, GetPointSizeRange().y ) );

		PointFadeThresholdSize( state.fadeThresholdSize );
		PointSpriteCoordOrigin( state.spriteCoordOrigin );
	}
	
/*
=================================================
	PointSize
=================================================
*/
	void GL4StateManager::PointSize (float value)
	{
		RenderState::PointState &	ps = _renderState.point;

		if ( Equals( ps.pointSize, value ) )
			return;

		GL_CALL( glPointSize( value ) );
		ps.pointSize = value;
	}

/*
=================================================
	ProgramPointSize
=================================================
*/
	void GL4StateManager::ProgramPointSize (bool enabled)
	{
		RenderState::PointState &	ps = _renderState.point;

		if ( ps.programPointSize == enabled )
			return;
		
		if ( enabled ) {
			GL_CALL( glEnable( GL_PROGRAM_POINT_SIZE ) );
		} else {
			GL_CALL( glDisable( GL_PROGRAM_POINT_SIZE ) );
		}

		ps.programPointSize = enabled;
	}
	
/*
=================================================
	PointFadeThresholdSize
=================================================
*/
	void GL4StateManager::PointFadeThresholdSize (float value)
	{
		RenderState::PointState &	ps = _renderState.point;

		if ( Equals( ps.fadeThresholdSize, value ) )
			return;

		GL_CALL( glPointParameterf( GL_POINT_FADE_THRESHOLD_SIZE, value ) );
		ps.fadeThresholdSize = value;
	}
	
/*
=================================================
	PointSpriteCoordOrigin
=================================================
*/
	void GL4StateManager::PointSpriteCoordOrigin (EPointSpriteCoordOrigin::type value)
	{
		RenderState::PointState &	ps = _renderState.point;

		if ( ps.spriteCoordOrigin == value )
			return;
		
		GL_CALL( glPointParameteri( GL_POINT_SPRITE_COORD_ORIGIN, GL4Enum( value ) ) );
		ps.spriteCoordOrigin = value;
	}

/*
=================================================
	SetTextureStates
=================================================
*/
	void GL4StateManager::EnableFramebufferSRGB (bool enabled)
	{
		if ( _framebufferSRGB == enabled )
			return;

		_framebufferSRGB = enabled;

		if ( _framebufferSRGB ) {
			GL_CALL( glEnable( GL_FRAMEBUFFER_SRGB ) );
		}
		else {
			GL_CALL( glDisable( GL_FRAMEBUFFER_SRGB ) );
		}
	}
	
/*
=================================================
	_SetTessellationPatch
=================================================
*/
	void GL4StateManager::_SetTessellationPatch (uint size)
	{
		if ( _tessellationPatch == size or size == 0 )
			return;

		_tessellationPatch = size;
		GL_CALL( glPatchParameteri( GL_PATCH_VERTICES, _tessellationPatch ) );
	}
	
/*
=================================================
	BindProgram
=================================================
*/
	void GL4StateManager::BindProgram (const ProgramID &id)
	{
		if ( id._id != _currentProgram._id )
		{
			GL_CALL( glUseProgram( id._id ) );
		}

		_currentProgram = id;

		_SetTessellationPatch( id._patchSize );
	}
	
/*
=================================================
	BindBuffer
=================================================
*/
	bool GL4StateManager::BindBufferBase (EBufferTarget::type target, const BufferID &id, uint index, BytesU offset, BytesU size)
	{
		CHECK_ERR( target < _buffers.Count() );

		BufferArray_t &		buffers = _buffers[ target ];

		CHECK_ERR( index < buffers.Count() );

		const _Buffer		buf( id, (usize)offset, (usize)size );
		const GLenum		targ = GL4Enum( target );

		if ( buffers[ index ] == buf )
			return true;
		
		if ( offset == 0 and size == BytesU(-1) ) {
			GL_CALL( glBindBufferBase( targ, index, id.Id() ) );
		}
		else {
			GL_CALL( glBindBufferRange( targ, index, id.Id(), GLintptr(offset), GLsizeiptr(size) ) );
		}

		buffers[ index ] = buf;
		return true;
	}
	
/*
=================================================
	UnbindBuffer
=================================================
*/
	bool GL4StateManager::UnbindBufferBase (EBufferTarget::type target, const BufferID &id)
	{
		if ( not id.IsValid() )
			return true;

		CHECK_ERR( target < _buffers.Count() );

		BufferArray_t &		buffers	= _buffers[ target ];
		const GLenum		targ	= GL4Enum( target );

		FOR( i, buffers )
		{
			if ( buffers[i].id == id )
			{
				GL_CALL( glBindBufferBase( targ, GLuint(i), 0 ) );

				buffers[i] = _Buffer();
			}
		}
		return true;
	}
	
/*
=================================================
	UnbindBuffer
=================================================
*/
	void GL4StateManager::UnbindBufferBase (const BufferID &id)
	{
		if ( not id.IsValid() )
			return;

		FOR( i, _buffers )
		{
			BufferArray_t &		buf		= _buffers[i];
			const GLenum		targ	= GL4Enum( EBufferTarget::type(i) );

			FOR( j, buf )
			{
				if ( buf[j].id == id )
				{
					GL_CALL( glBindBufferBase( targ, GLuint(j), 0 ) );

					buf[j] = _Buffer();
				}
			}
		}
	}

/*
=================================================
	BindRenderTarget
=================================================
*/
	void GL4StateManager::BindRenderTarget (const RenderTargetID &rt)
	{
		if ( rt._id == _currentRenderTarget )
			return;

		_currentRenderTarget = rt._id;
		GL_CALL( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, _currentRenderTarget ) );
	}
	
/*
=================================================
	PushViewport
=================================================
*/
	void GL4StateManager::PushViewport ()
	{
		_viewportStack.Push();
	}
	
/*
=================================================
	PopViewport
=================================================
*/
	void GL4StateManager::PopViewport ()
	{
		_viewportStack.Pop();
		
		RectU const &	value = _viewportStack.Get();

		GL_CALL( glViewport( value.left, value.bottom, value.Width(), value.Height() ) );
	}
	
/*
=================================================
	Viewport
=================================================
*/
	void GL4StateManager::Viewport (const uint2 &offset, const uint2 &size)
	{
		Viewport( RectU( offset.x, offset.y, offset.x + size.x, offset.y + size.y ) );
	}

	void GL4StateManager::Viewport (const RectU &value)
	{
		if ( value != _viewportStack.Get() )
		{
			_viewportStack.Set( value );

			GL_CALL( glViewport( value.left, value.bottom, value.Width(), value.Height() ) );
		}
	}
	
/*
=================================================
	Viewports
=================================================
*/
	void GL4StateManager::Viewports (Buffer<const RectU> values)
	{
		StaticArray< float4, GlobalConst::Graphics_MaxViewports >	fvps;

		FOR( i, fvps ) {
			fvps[i] = uint4( values[i].left, values[i].bottom, values[i].Width(), values[i].Height() ).To<float4>();
		}

		GL_CALL( glViewportArrayv( 0, GLsizei(values.Count()), (const GLfloat *) fvps.ptr() ) );

		_viewportStack.Set( RectU() );	// to disable state caching
	}
	
/*
=================================================
	_OnResize
=================================================
*/
	void GL4StateManager::_OnResize (const uint2 &newSize)
	{
		_viewportStack.Clear();
		_viewportStack.SetDefault( RectU( 0, 0, newSize.x, newSize.y ) );
		
		_UpdateCurrentViewport();
	}
	
/*
=================================================
	_ResetViewportStack
=================================================
*/
	void GL4StateManager::_ResetViewportStack ()
	{
		_viewportStack.Clear();
		_UpdateCurrentViewport();
	}
	
/*
=================================================
	_UpdateCurrentViewport
=================================================
*/
	void GL4StateManager::_UpdateCurrentViewport ()
	{
		RectU const &	value = _viewportStack.Get();

		GL_CALL( glViewport( value.left, value.bottom, value.Width(), value.Height() ) );
	}

/*
=================================================
	_CheckProgramInOutStates
=================================================
*/
	void GL4StateManager::_CheckProgramInOutStates () const
	{
		DEBUG_ONLY(
		if ( _currentProgram._input != _attribsState )
		{
			String	s;
			s	<< "Warning: program attribs not compatible with binded VAO!\n"
				<< "program attribs:\n" << _currentProgram._input.ToString()
				<< "-----\ncurrent attribs:\n" << _attribsState.ToString();
			LOG( s.cstr(), ELog::Warning );
		})
	}

/*
=================================================
	DrawArrays
=================================================
*/
	void GL4StateManager::DrawArrays (EPrimitive::type primitive, usize count, usize offset)
	{
		_CheckProgramInOutStates();
		
		const GLenum mode = IsTessellationEnabled() ? GL_PATCHES : GL4Enum( primitive );
		
		_SetMemoryBarrierBeforeDraw();

		GL_CALL( glDrawArrays( mode, GLint(offset), GLsizei(count) ) );
	}

/*
=================================================
	DrawElements
=================================================
*/
	void GL4StateManager::DrawElements (EPrimitive::type primitive, EIndex::type indexType, usize count, BytesU offset)
	{
		_CheckProgramInOutStates();
		
		const GLenum mode = IsTessellationEnabled() ? GL_PATCHES : GL4Enum( primitive );
		
		_SetMemoryBarrierBeforeDraw();

		GL_CALL( glDrawElements( mode, GLsizei(count), GL4Enum( indexType ), (const void *) (usize)offset ) );
	}
	
/*
=================================================
	DrawElementsBaseVertex
=================================================
*/
	void GL4StateManager::DrawElementsBaseVertex (EPrimitive::type primitive, EIndex::type indexType, usize count, BytesU offset, uint baseVertex)
	{
		_CheckProgramInOutStates();
		
		const GLenum mode = IsTessellationEnabled() ? GL_PATCHES : GL4Enum( primitive );
		
		_SetMemoryBarrierBeforeDraw();

		GL_CALL( glDrawElementsBaseVertex( mode, GLsizei(count), GL4Enum( indexType ), (const void *) (usize)offset, GLint(baseVertex) ) );
	}
	
/*
=================================================
	MultiDrawElements
=================================================
*/
	void GL4StateManager::MultiDrawElements (EPrimitive::type primitive, EIndex::type indexType, const usize *count, const BytesU *offsets, uint drawcount)
	{
		_CheckProgramInOutStates();
		
		const GLenum mode = IsTessellationEnabled() ? GL_PATCHES : GL4Enum( primitive );
		
		_SetMemoryBarrierBeforeDraw();

		STATIC_ASSERT( sizeof(offsets[0]) == sizeof(void *) );

		GL_CALL( glMultiDrawElements( mode, (const GLsizei *) count, GL4Enum( indexType ),
									  (const void *const*) offsets, drawcount ) );
	}
	
/*
=================================================
	MultiDrawElementsBaseVertex
=================================================
*/
	void GL4StateManager::MultiDrawElementsBaseVertex (EPrimitive::type primitive, EIndex::type indexType, const usize *count, const BytesU *offsets, const uint *baseVertex, uint drawcount)
	{
		_CheckProgramInOutStates();

		const GLenum mode = IsTessellationEnabled() ? GL_PATCHES : GL4Enum( primitive );
		
		_SetMemoryBarrierBeforeDraw();
		
		STATIC_ASSERT( sizeof(offsets[0]) == sizeof(void *) );

		GL_CALL( glMultiDrawElementsBaseVertex( mode, (const GLsizei *) count, GL4Enum( indexType ),
												(const void *const*) offsets, drawcount, (const GLint *) baseVertex ) );
	}
	
/*
=================================================
	BeginQuery
=================================================
*/
	void GL4StateManager::BeginQuery (const GL4QueryID &query, EQuery::type target, uint index)
	{
		CHECK( index < _queryMaxIndices[target] );

		GL_CALL( glBeginQueryIndexed( GL4Enum( target ), index, query.Id() ) );

		// TODO: save binded query
	}
	
/*
=================================================
	EndQuery
=================================================
*/
	void GL4StateManager::EndQuery (const GL4QueryID &query, EQuery::type target, uint index)
	{
		CHECK( index < _queryMaxIndices[target] );

		GL_CALL( glEndQueryIndexed( GL4Enum( target ), index ) );
	}
	
/*
=================================================
	DispatchCompute
=================================================
*/
	void GL4StateManager::DispatchCompute (const ulong3 &groupSize)
	{
		_SetMemoryBarrierBeforeDraw();
		
		ASSERT( All( groupSize < ulong3(MaxValue<uint>()) ) );

		GL_CALL( glDispatchCompute( (GLuint)groupSize.x, (GLuint)groupSize.y, (GLuint)groupSize.z ) );
	}
	
/*
=================================================
	DispatchCompute
=================================================
*/
	void GL4StateManager::DispatchCompute (const ulong3 &groupSize, const ulong3 &localSize)
	{
		_SetMemoryBarrierBeforeDraw();

		ASSERT( All( groupSize < ulong3(MaxValue<uint>()) ) );
		ASSERT( All( localSize < ulong3(MaxValue<uint>()) ) );

		GL_CALL( glDispatchComputeGroupSizeARB( (GLuint)groupSize.x, (GLuint)groupSize.y, (GLuint)groupSize.z,
												  (GLuint)localSize.x, (GLuint)localSize.y, (GLuint)localSize.z ) );
	}


}	// Graphics
}	// Engine

#endif	// GRAPHICS_API_OPENGL
