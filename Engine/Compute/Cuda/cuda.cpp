// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "cuda.h"

#if defined( COMPUTE_API_CUDA )

namespace cuda
{
	
/*
=================================================
	Cuda_CheckErrors
=================================================
*/
	bool Cuda_CheckErrors (CUresult errorCode, const char *cudaCall, const char *func, const char *file, int line)
	{
		using namespace Engine;

		if ( errorCode == CUDA_SUCCESS )
			return true;

		String			msg( "CUDA error: " );
		const char *	str = null;

		cuGetErrorName( errorCode, &str );
		msg << str;

		cuGetErrorString( errorCode, &str );
		msg << ", message: \"" << str;

		msg << "\", in " << cudaCall;
		msg << ", function: " << func;
		msg << ", file: \"" << file << "\", line: " << String().FormatI(line, 10);

		LOG( msg.cstr(), ELog::ERROR );
		return false;
	}


}	// cuda

#endif	// COMPUTE_API_CUDA
