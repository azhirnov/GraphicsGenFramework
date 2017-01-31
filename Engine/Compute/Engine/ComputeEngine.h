// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/Common.h"

// OpenCL //
#if defined( COMPUTE_API_OPENCL )
#	include "Engine/Compute/CL2/CL2ComputeEngine.h"
#	include "Engine/Compute/CL2/CL2ComputeFunction.h"
#	include "Engine/Compute./CL2/CL2ComputeProgram.h"
#	include "Engine/Compute/CL2/CL2ComputeSyncEvent.h"
#	include "Engine/Compute/CL2/CL2ComputeImage.h"
#	include "Engine/Compute/CL2/CL2ComputeBuffer.h"
#endif

// CUDA //
#if defined( COMPUTE_API_CUDA )
#	include "Engine/Compute/Cuda/CudaComputeEngine.h"
#	include "Engine/Compute/Cuda/CudaComputeProgram.h"
#	include "Engine/Compute/Cuda/CudaComputeFunction.h"
#	include "Engine/Compute/Cuda/CudaComputeBuffer.h"
#	include "Engine/Compute/Cuda/CudaComputeTexture.h"
#	include "Engine/Compute/Cuda/CudaComputeImage.h"
#	include "Engine/Compute/Cuda/CudaComputeSyncEvent.h"
#endif

// OpenGL //
#if defined( COMPUTE_API_OPENGL )
#	include "Engine/Compute/GL4/GL4ComputeEngine.h"
#	include "Engine/Compute/GL4/GL4ComputeBuffer.h"
#	include "Engine/Compute/GL4/GL4ComputeImage.h"
#	include "Engine/Compute/GL4/GL4ComputeProgram.h"
#	include "Engine/Compute/GL4/GL4ComputeFunction.h"
#endif


namespace Engine
{
namespace Compute
{

	//
	// Compute Engine
	//

	class ComputeEngine : public BaseComputeEngine
	{
	// methods
	public:
		explicit
		ComputeEngine (const SubSystemsRef ss);
		~ComputeEngine ();
	};


}	// Compute
}	// Engine
