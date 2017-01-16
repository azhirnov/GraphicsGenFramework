// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/Common/Common.h"

#if defined( COMPUTE_API_CUDA )

# if defined(GRAPHICS_API_OPENGL)
#	include "Engine/Graphics/GL4/gl4.h"
# endif

namespace cuda
{
	
#	define _INC_STDLIB
#	include "External/cuda/include/cuda.h"
	
#	if defined(GRAPHICS_API_OPENGL)
	 using namespace gl;
#	 include "External/cuda/include/cudaGL.h"
#	endif


	bool Cuda_CheckErrors (CUresult errorCode, const char *cudaCall, const char *func, const char *file, int line);


#	define CU_CALL( ... ) \
	{ \
		const ::cuda::CUresult __cu_err__ =  (__VA_ARGS__); \
		::cuda::Cuda_CheckErrors( __cu_err__, TOSTRING( __VA_ARGS__ ), GX_FUNCTION_NAME, __FILE__, __LINE__ ); \
	}
	
#	define __CU_CALL_R( _func_, _ret_, ... ) \
	{ \
		const ::cuda::CUresult __cu_err__ =  (_func_); \
		if ( not ::cuda::Cuda_CheckErrors( __cu_err__, TOSTRING( _func_ ), GX_FUNCTION_NAME, __FILE__, __LINE__ ) ) \
			return _ret_; \
	}

#	define CU_CHECK( ... ) \
		__CU_CALL_R( DEFAUX_GET_FIRST( __VA_ARGS__ ), DEFAUX_GET_SECOND( __VA_ARGS__, false ) )


}	// cuda

#endif	// COMPUTE_API_CUDA
