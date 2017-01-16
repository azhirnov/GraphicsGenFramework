// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Graphics/Texture/Texture.h"
#include "Engine/Graphics/RenderTarget/FragmentOutputState.h"

namespace Engine
{
namespace Graphics
{

	//
	// Render Target
	//

	class RenderTarget : public BaseObject
	{
	// types
	private:
		typedef Union< float4, int4, uint4, float, int >	ClearValue_t;

		struct _Target
		{
		// variables
			TexturePtr		texture;
			ClearValue_t	clearValue;
			bool			enabled;

		// methods
			_Target () : clearValue( float4() ), enabled(false) {}

			bool IsEnabled ()			const	{ return enabled; }
			void Attach (const TexturePtr &tex)	{ texture = tex;   enabled = true; }
			void Detach ()						{ texture = null;  enabled = false; }
		};

		typedef StaticArray< _Target, ERenderTarget::_COUNT >				Targets_t;
		typedef FixedSizeArray< RectI, GlobalConst::Graphics_MaxViewports >	Viewports_t;


	// variables
	private:
		RenderTargetID			_rt;
		Targets_t				_targets;
		Viewports_t				_viewports;
		FragmentOutputState		_fragmentOut;
		bool					_changed;
		bool					_isSRGB;
		bool const				_isSystemRT;


	// methods
	protected:
		RenderTarget (bool isSystemRT, const SubSystemsRef ss);
		~RenderTarget ();

	public:
		bool Create ();
		void Destroy ();

		void Bind ();
		void Unbind ();
		bool IsBinded () const;

		bool Attach (const TexturePtr &texture, ERenderTarget::type target, uint level);
		bool AttachLayer (const TexturePtr &texture, ERenderTarget::type target, uint layer, uint level);

		void Detach (const TexturePtr &texture);
		void Detach (ERenderTarget::type target);
		void DetachAll ();

		void SetClearColor (ERenderTarget::type target, color4u color);
		void SetClearColor (ERenderTarget::type target, const color4f &color);
		void SetClearColor (ERenderTarget::type target, const int4 &color);
		void SetClearColor (ERenderTarget::type target, const uint4 &color);
		void SetClearColor (ERenderTarget::type target, const float4 &color);
		void SetClearDepth (float depth);
		void SetClearStencil (int stencil);

		void Clear ();
		void ClearOnly (ERenderTarget::type target);

		void SetViewport (const int2 &size);
		void SetViewport (const RectI &viewport);

		void SetViewports (Buffer<const int2> sizes);
		void SetViewports (Buffer<const RectI> viewports);

		void ComputeViewport ();

		bool HasTarget (ERenderTarget::type target) const;
		bool HasDepth () const;
		bool HasStencil () const;
		bool HasColor () const;

		bool GetImage (OUT Buffer<ubyte> &data, EPixelFormat::type format, const RectI &region,
						ERenderTarget::type target = ERenderTarget::Color0,
						Bytes<usize> align = Bytes<usize>(1)) const;

		FragmentOutputState const&	GetFragmentOutput ()					const	{ return _fragmentOut; }
		RectI						GetViewport (usize index = 0)			const	{ return _viewports[ index ]; }
		Buffer< const RectI >		GetViewports ()							const	{ return _viewports; }
		TexturePtr const &			GetTexture (ERenderTarget::type target)	const	{ return _targets[ target ].texture; }
		EPixelFormat::type			GetFormat (ERenderTarget::type target)	const;
		bool						IsSystemRenderTarget ()					const	{ return _isSystemRT; }
		bool						IsSRGB ()								const	{ return _isSRGB; }

		static RenderTargetPtr		New (const SubSystemsRef ss);
		static RenderTargetPtr		NewSystemRT (const SubSystemsRef ss);
		

	// api specific methods
	private:
		void _ClearTarget (ERenderTarget::type target);

		void _BeforeAttach (ERenderTarget::type target);
		void _OnAttached (ERenderTarget::type target);
		void _OnDetached (ERenderTarget::type target);

		bool _CheckStatus ();
		void _UpdateDrawBuffers ();

		void _CheckViewports () const;

		void _InitSystemRT ();
	};


}	// Graphics
}	// Engine
