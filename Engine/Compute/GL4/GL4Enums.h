// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Compute/Types/Enums.h"

#if defined( COMPUTE_API_OPENGL )

#if not defined( GRAPHICS_API_OPENGL )
#	error OpenGL Compute API need to use with OpenGL Graphics API
#endif

namespace Engine
{
namespace Compute
{
	using namespace Engine::Graphics;
	

//-----------------------------------------------------------------------------//
// EProgramUnitFlags

	inline bool EProgramUnitFlags::IsCompatible (type left, type right)
	{
		return EShaderCompilationFlags::IsCompatible( EShaderCompilationFlags::type( left ),
													  EShaderCompilationFlags::type( right ) );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
