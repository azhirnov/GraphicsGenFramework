// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Graphics/Types/Common.h"
#include "Engine/Graphics/RenderTarget/RenderTargetManager.h"
#include "Engine/Graphics/Shader/ShaderManager.h"

namespace Engine
{
namespace Graphics
{

	//
	// Graphics Engine
	//

	class GraphicsEngine : public BaseObject
	{
	// variables
	private:
		StateManager			_stateMngr;
		GraphicsContext			_context;
		RenderTargetManager		_renderTargetMngr;
		ShaderManager			_shaderMngr;

		EVideoCardVendor::type	_vendor;
		bool					_initialized;


	// methods
	public:
		explicit
		GraphicsEngine (const SubSystemsRef ss);
		~GraphicsEngine ();

		bool Initialize ();
		void Deinitialize ();

		void FlushFrame ();

		EVideoCardVendor::type		VideoCardVendor ()	const	{ return _vendor; }

		Ptr< StateManager >			GetStateManager ()			{ return &_stateMngr; }
		Ptr< GraphicsContext >		GetContext ()				{ return &_context; }
		Ptr< RenderTargetManager >	GetRenderTargetManager ()	{ return &_renderTargetMngr; }
		Ptr< ShaderManager >		GetShaderManager ()			{ return &_shaderMngr; }

	private:
		void _OnResize (const SysEvent::Window &ev);
	};


}	// Graphics
}	// Engine
