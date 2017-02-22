// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

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

		typedef StaticArray< _Target, ERenderTarget::_Count >				Targets_t;
		typedef FixedSizeArray< RectU, GlobalConst::Graphics_MaxViewports >	Viewports_t;


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

	public:
		explicit
		RenderTarget (const SubSystemsRef ss) : RenderTarget(false, ss) {}
		~RenderTarget ();

		bool Create ();
		void Destroy ();

		void Bind ();
		void Unbind ();
		bool IsBinded () const;

		bool Attach (const TexturePtr &texture, ERenderTarget::type target, uint level);
		bool AttachLayer (const TexturePtr &texture, ERenderTarget::type target, uint layer, uint level);

		bool Detach (const TexturePtr &texture);
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

		void SetViewport (const uint2 &size);
		void SetViewport (const RectU &viewport);

		void SetViewports (Buffer<const uint2> sizes);
		void SetViewports (Buffer<const RectU> viewports);

		void ComputeViewport ();

		bool HasTarget (ERenderTarget::type target) const;
		bool HasDepth () const;
		bool HasStencil () const;
		bool HasColor () const;

		bool GetImage (OUT BinaryBuffer data, EPixelFormat::type format, const RectU &region,
						ERenderTarget::type target = ERenderTarget::Color0,
						BytesU align = 1_b) const;

		FragmentOutputState const&	GetFragmentOutput ()					const	{ return _fragmentOut; }
		RectU						GetViewport (usize index = 0)			const	{ return _viewports[ index ]; }
		Buffer< const RectU >		GetViewports ()							const	{ return _viewports; }
		TexturePtr const &			GetTexture (ERenderTarget::type target)	const	{ return _targets[ target ].texture; }
		EPixelFormat::type			GetFormat (ERenderTarget::type target)	const;
		bool						IsSystemRenderTarget ()					const	{ return _isSystemRT; }
		bool						IsSRGB ()								const	{ return _isSRGB; }

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
