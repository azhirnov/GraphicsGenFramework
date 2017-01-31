// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "CL2ComputeUtils.h"

#if defined( COMPUTE_API_OPENCL )

namespace Engine
{
namespace Compute
{
	
/*
=================================================
	_AcquireObjects
=================================================
*/
	void CL2ComputeUtils::_AcquireObjects (cl::cl_command_queue cmdQueue, Buffer< cl::cl_mem > objs)
	{
		using namespace cl;

		if ( objs.Empty() )
			return;

		CL_CALL( clEnqueueAcquireGLObjects( cmdQueue, (cl_uint) objs.Count(), objs.ptr(), 0, null, null ) );
	}
	
/*
=================================================
	_ReleaseObjects
=================================================
*/
	void CL2ComputeUtils::_ReleaseObjects (cl::cl_command_queue cmdQueue, Buffer< cl::cl_mem > objs)
	{
		using namespace cl;

		if ( objs.Empty() )
			return;
		
		CL_CALL( clEnqueueReleaseGLObjects( cmdQueue, (cl_uint) objs.Count(), objs.ptr(), 0, null, null ) );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENCL