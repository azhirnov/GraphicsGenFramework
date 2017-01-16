// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Graphics/Engine.Graphics.h"

namespace Engine
{
namespace Compute
{
	using namespace Engine::Graphics;


	/*
	struct EMemoryObject
	{
		enum type
		{
			Buffer = 0,
			Image1D,
			Image1DArray,
			ImageBuffer,
			Image2D,
			Image2DArray,
			Image3D,
			PIPE,
		};

		inline static type From (ETexture::type value);

		inline static bool IsImage (type value);
		inline static bool IsBuffer (type value);
		inline static bool IsPipe (type value);
	};


	/*struct EEventFlags
	{
		enum type
		{
			BLOCKING_SYNC	= 1 << 0,
			DISABLE_TIMING	= 1 << 1,
			INTERPROCESS	= 1 << 2,
		};
	};*/


	struct EProgramUnitFlags
	{
		enum type
		{
			None		= EShaderCompilationFlags::None,

			FP_32		= EShaderCompilationFlags::FP_32,
			FP_64		= EShaderCompilationFlags::FP_64,
			
			Default		= FP_32,
		};

		inline static bool IsCompatible (type left, type right);

		GX_ENUM_BIT_OPERATIONS( type );
	};


}	// Compute
}	// Engine

GX_SUBSYSTEM_DECL( GraphicsSubSystemsTypeList_t, Compute::ComputeEngine );
