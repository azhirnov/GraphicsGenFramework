// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "RenderTarget.h"

namespace Engine
{
namespace Graphics
{

	//
	// Render Target Manager
	//

	class RenderTargetManager : public BaseObject
	{
	// types
	private:
		typedef Array< RenderTargetPtr >	RenderTargetsStack_t;


	// variables
	private:
		RenderTargetPtr			_defaultRT;
		RenderTargetsStack_t	_rtStack;


	// methods
	public:
		explicit
		RenderTargetManager (const SubSystemsRef ss);

		bool Initialize ();
		void Deinitialize ();
		
		bool ScreenShot (StringCRef filename, ESaveImageFormat::type format = ESaveImageFormat::PNG, bool makeUnique = true);

		bool SaveRenderTargetImage (const RenderTargetPtr &rt, StringCRef filename, ESaveImageFormat::type format, bool makeUnique = true);

		// render targets stack
		bool Push (const RenderTargetPtr &rt);
		bool Pop (const RenderTargetPtr &rt);
		bool Remove (const RenderTargetPtr &rt);

		bool IsCurrent (const RenderTargetPtr &rt) const;

		RenderTargetPtr const&	GetCurrent ()				const	{ return _rtStack.Back(); }
		RenderTargetPtr const&	GetSystemRenderTarget ()	const	{ return _defaultRT; }

		void _OnResize (const uint2 &newSize);


	private:
		bool _SaveRenderTargetImage_PNG (const RenderTargetPtr &rt, StringCRef filename, bool flipY);
		bool _SaveRenderTargetImage_RAW (const RenderTargetPtr &rt, StringCRef filename);

	};


}	// Graphics
}	// Engine
