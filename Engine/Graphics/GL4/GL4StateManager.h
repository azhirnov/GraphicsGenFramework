// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "gl4.h"

#if defined( GRAPHICS_API_OPENGL )

#include "GL4Enums.h"
#include "GL4ObjectIDs.h"
#include "Engine/Graphics/Types/IStateActuator.h"
#include "Engine/Graphics/Types/RenderState.h"
#include "Engine/Graphics/Types/MipmapLevel.h"
#include "Engine/Graphics/Types/TexArrLayer.h"
#include "Engine/Graphics/Types/MemoryBarrierObj.h"
#include "Engine/Graphics/VertexAttribs/VertexAttribsDesc.h"

namespace Engine
{
namespace Graphics
{
	
	class   GL4StateManager;
	typedef GL4StateManager		StateManager;
	
	class   GL4Context;
	typedef GL4Context		GraphicsContext;



	//
	// State Manager
	//

	class GL4StateManager
	{
		friend class GL4Context;

	// types
	private:

		struct _TextureUnit : public CompileTime::PODStruct
		{
		// variables
			TextureID		binded;
			SamplerID		sampler;

		// methods
			_TextureUnit () {}
		};

		struct _ImageUnit : public CompileTime::PODStruct
		{
		// variables
			TextureID		binded;
			gl::GLint		level;
			gl::GLint		layer;
			gl::GLenum		access;
			gl::GLenum		format;

		// methods
			_ImageUnit () : level(0), layer(-1), access(0), format(0) {}
		};

		struct _Buffer : public CompileTime::PODStruct
		{
		// variables
			BufferID		id;
			usize			offset;
			usize			size;

		// methods
			_Buffer () : offset(0), size(0) {}
			_Buffer (BufferID id, usize offset, usize size) : id(id), offset(offset), size(size) {}

			bool operator == (const _Buffer &right) const	{ return id == right.id and offset == right.offset and size == right.size; }
		};

		typedef Array< _TextureUnit >				TextureUnits_t;
		typedef Array< _ImageUnit >					ImageUnits_t;
		typedef Array< _Buffer >					BufferArray_t;
		typedef FixedSizeStack< RectI, 16 >			ViewportStack_t;
		typedef StaticArray< uint, EQuery::_COUNT >	QueryMaxIndices_t;

		typedef StaticArray< BufferArray_t, EBufferTarget::_COUNT >	Buffers_t;

		typedef StaticArray< uint, CompileTime::IntLog2< uint, EMemoryBarrier::_Last >+1 >	BarrierLastTickArray_t;

		static const uint	_BARRIER_TICKS_MASK = MemoryBarrierObj::TicksInit()-1;


	// variables
	private:
		// texture
		TextureUnits_t			_textureUnits;
		uint					_activeUnit;
		uint					_maxAnisotropy;

		// image
		ImageUnits_t			_imageUnits;

		// vertex attribs
		gl::GLuint				_currentVA;
		VertexAttribsState		_attribsState;

		// program
		ProgramID				_currentProgram;
		Buffers_t				_buffers;

		// framebuffers
		gl::GLuint				_currentRenderTarget;
		uint					_maxColorAttachments;

		// render states
		RenderState				_renderState;
		uint					_tessellationPatch;
		float2					_pointSizeRange;
		bool					_framebufferSRGB;

		// viewport
		ViewportStack_t			_viewportStack;

		// query
		QueryMaxIndices_t		_queryMaxIndices;

		// memory barrier
		EMemoryBarrier::type	_memBarrierBeforeDraw;
		BarrierLastTickArray_t	_memBarrierLastTicks;
		uint					_barrierTicks;


	// methods
	public:
		GL4StateManager ();
		~GL4StateManager ();
		
		bool Initialize ();
		void Deinitialize ();


		// sync
		void TextureBarrier ();
		void CreateBarrier (OUT MemoryBarrierObj &obj);
		//void UpdateBarrier (INOUT MemoryBarrierObj &obj, EMemoryBarrier::type flags);
		void MemoryBarrier (INOUT MemoryBarrierObj &obj, EMemoryBarrier::type flags);
		void MemoryBarrierBeforeDraw (INOUT MemoryBarrierObj &obj, EMemoryBarrier::type flags);
		void ClientSynchronize ();	// CPU side sync
		void Synchronize ();		// GPU side sync


		// texture
		void ActiveTexture (const TextureID &id);

		void BindTexture (const TextureID &id, uint unit);
		void UnbindTexture (const TextureID &id);

		bool SupportAnisotropyFiltering ()	const					{ return _maxAnisotropy > 1; }
		uint GetMaxAnisotropyLevel ()		const					{ return _maxAnisotropy; }
		uint GetMaxTextureUnits ()			const					{ return (uint) _textureUnits.Count(); }


		// image
		void UnbindImage (const TextureID &id);
		void BindImage (const TextureID &id, uint unit, MipmapLevel level, TexArrLayer layer,
						EMemoryAccess::type access, EPixelFormat::type format);

		uint GetMaxImageUnits ()			const					{ return (uint) _imageUnits.Count(); }


		// sampler
		void BindSampler (const SamplerID &sampler, uint unit);
		void UnbindSampler (const SamplerID &sampler);
		

		// vertex attribs
		void BindAttribs (const VertexAttribsPtr &attribs, const BufferID &vb, uint offset, uint stride, uint divisor = 0);
		void BindAttribs (const VertexAttribsPtr &attribs, const BufferID &ib);
		void UnbindAttribs (const VertexAttribsPtr &attribs);
		void UnbindAttribs ();
		bool IsCurrentAttribs (const VertexAttribsPtr &attribs)	const;


		// program
		void BindProgram (const ProgramID &id);
		
		void BindBufferBase (EBufferTarget::type target, const BufferID &id, uint index, usize offset = 0, usize size = -1);
		void UnbindBufferBase (EBufferTarget::type target, const BufferID &id);
		void UnbindBufferBase (const BufferID &id);

		uint GetShaderBuffers (EBufferTarget::type target)	const	{ return _buffers[target].Count(); }
		bool IsProgramBinded (const ProgramID &prog)		const	{ return prog.Id() == _currentProgram.Id(); }


		// framebuffers
		void BindRenderTarget (const RenderTargetID &rt);
		bool IsRenderTargetBinded (const RenderTargetID &rt) const	{ return rt._id == _currentRenderTarget; }

		RenderTargetID _CurrentRenderTarget () const				{ return RenderTargetID( _currentRenderTarget ); }

		uint GetMaxColorAttachments () const						{ return _maxColorAttachments; }


		// states
		void SetRenderState (const RenderState &state);

		bool IsTessellationEnabled ()				const		{ return _currentProgram._patchSize != 0; }

		void EnableFramebufferSRGB (bool enabled);
		bool IsFramebufferSRGB ()					const		{ return _framebufferSRGB; }

		float2 const &		GetPointSizeRange ()	const		{ return _pointSizeRange; }

		RenderState const &	GetRenderState ()		const		{ return _renderState; }


		// viewport
		void PushViewport ();
		void PopViewport ();
		void Viewport (const int2 &offset, const int2 &size)	{ Viewport( RectI( offset.x, offset.y, size.x, size.y ) ); }
		void Viewport (const RectI &value);
		void Viewports (Buffer<const RectI> values);
		void _ResetViewportStack ();
		void _OnResize (const int2 &newSize);


		// drawing
		void DrawArrays (EPrimitive::type primitive, usize count, usize offset);

		void DrawElements (EPrimitive::type primitive, EIndex::type indexType, usize count, usize offset);
		void DrawElementsBaseVertex (EPrimitive::type primitive, EIndex::type indexType, usize count, usize offset, uint baseVertex);

		void MultiDrawElements (EPrimitive::type primitive, EIndex::type indexType, const usize *count, const usize *offset, uint drawcount);
		void MultiDrawElementsBaseVertex (EPrimitive::type primitive, EIndex::type indexType, const usize *count, const usize *offset, const uint *baseVertex, uint drawcount);


		// dispatch compute
		void DispatchCompute (const uint3 &groupSize);
		void DispatchCompute (const uint3 &groupSize, const uint3 &localSize);


		// query
		void BeginQuery (const GL4QueryID &query, EQuery::type target, uint index);
		void EndQuery (const GL4QueryID &query, EQuery::type target, uint index);


	private:
		void SetColorBufferState (uint index, const RenderState::ColorBufferState &state);
		void SetDepthState (const RenderState::DepthBufferState &state);
		void SetStencilState (const RenderState::StencilBufferState &state);
		void SetCullMode (const RenderState::CullMode &mode);
		void SetPolygonState (const RenderState::PolygonStates &state);
		void SetPointState (const RenderState::PointState &state);

		void BlendEq (uint index, EBlendEq::type modeRGB, EBlendEq::type modeA);
		void BlendFunc (uint index, EBlendFunc::type srcRGB, EBlendFunc::type srcA, EBlendFunc::type dstRGB, EBlendFunc::type dstA);
		void ColorMask (uint index, bool4 mask);
		void EnableBlend (uint index, bool enabled);
		void BlendColor (const color4f &color);

		void DepthTest (bool enabled);
		void DepthWrite (bool enabled);
		void DepthClamp (bool enabled);
		void DepthFunc (ECompareFunc::type func);
		void DepthRange (const float2 &range);

		void StencilFunc (ECompareFunc::type func, ubyte ref, ubyte mask);
		void StencilOp (EStencilOp::type sfail, EStencilOp::type dfail, EStencilOp::type dppass);
		void StencilMask (ubyte mask);
		void StencilTest (bool enabled);

		void PolygonMode (EPolygonMode::type mode);
		void PolygonOffset (bool enabled, float factor, float units);

		void ProgramPointSize (bool enabled);
		void PointSize (float value);
		void PointFadeThresholdSize (float value);
		void PointSpriteCoordOrigin (EPointSpriteCoordOrigin::type value);
		
	private:
		void _UpdateCurrentViewport ();
		
		void _BindVertexArray (const VertexAttribsID &va, const VertexAttribsState &state);
		void _BindAttribs (const VertexAttribsPtr &attribs);

		void _SetTessellationPatch (uint size);

		void _CheckProgramInOutStates () const;

		void _SetMemoryBarrierBeforeDraw ();
		void _IncBarrierTicks ();
		void _MemoryBarrier (EMemoryBarrier::type flags);
		void _FilterMemoryBarrier (uint ticks, INOUT EMemoryBarrier::type &flags);
	};


}	// Graphics
}	// Engine

#endif	// GRAPHICS_API_OPENGL
