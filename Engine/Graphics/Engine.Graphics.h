// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.
/*
	This is wrapper for low level graphics API such as OpenGL, DirectX, Vulkan, ...
	This project provides middle level abstraction:
		- Texture.
		- Sampler.
		- Memory buffer (+ Vertex, Indices buffers).
		- Shader program.
		- Render target.
		- Uniform block, texture and image uniforms.
		- Render state.
		- Query.

	TODO:
		- Font
		- Render queue (Render pass) (or it is high level?)
*/

#pragma once


#include "Engine/Graphics/Engine/GraphicsEngine.h"
#include "Engine/Graphics/Shader/UniformBlock.h"
#include "Engine/Graphics/Shader/TextureUniform.h"
#include "Engine/Graphics/Shader/ImageUniform.h"
#include "Engine/Graphics/Buffer/MeshGenerator.h"
#include "Engine/Graphics/Query/GraphicsQuery.h"
#include "Engine/Graphics/Profiling/FunctionGpuProfiler.h"
