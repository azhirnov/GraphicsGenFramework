// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CL2Emulator.h"
#include "Engine/Compute/Types/Enums.h"

#if defined( COMPUTE_API_OPENCL )

namespace Engine
{
namespace Compute
{

	//
	// Compute Utils
	//

	class CL2ComputeUtils
	{
	// methods
	public:
		template <typename T0>
		static void AcquireObjects (cl::cl_command_queue cmdQueue, const T0 &obj0);

		template <typename T0, typename T1>
		static void AcquireObjects (cl::cl_command_queue cmdQueue, const T0 &obj0, const T1 &obj1);
		
		template <typename T0>
		static void ReleaseObjects (cl::cl_command_queue cmdQueue, const T0 &obj0);

		template <typename T0, typename T1>
		static void ReleaseObjects (cl::cl_command_queue cmdQueue, const T0 &obj0, const T1 &obj1);

	private:
		static void _AcquireObjects (cl::cl_command_queue cmdQueue, Buffer< cl::cl_mem > objs);
		static void _ReleaseObjects (cl::cl_command_queue cmdQueue, Buffer< cl::cl_mem > objs);
	};
	
	
/*
=================================================
	AcquireObjects
=================================================
*/
	template <typename T0>
	inline void CL2ComputeUtils::AcquireObjects (cl::cl_command_queue cmdQueue, const T0 &obj0)
	{
		FixedSizeArray< cl::cl_mem, 2 >	arr;

		if ( obj0 != null and obj0->IsShared() )	arr.PushBack( obj0->Id() );

		_AcquireObjects( cmdQueue, arr );
	}

	template <typename T0, typename T1>
	inline void CL2ComputeUtils::AcquireObjects (cl::cl_command_queue cmdQueue, const T0 &obj0, const T1 &obj1)
	{
		FixedSizeArray< cl::cl_mem, 2 >	arr;

		if ( obj0 != null and obj0->IsShared() )	arr.PushBack( obj0->Id() );
		if ( obj1 != null and obj1->IsShared() )	arr.PushBack( obj1->Id() );

		_AcquireObjects( cmdQueue, arr );
	}
		
/*
=================================================
	ReleaseObjects
=================================================
*/
	template <typename T0>
	inline void CL2ComputeUtils::ReleaseObjects (cl::cl_command_queue cmdQueue, const T0 &obj0)
	{
		FixedSizeArray< cl::cl_mem, 2 >	arr;

		if ( obj0 != null and obj0->IsShared() )	arr.PushBack( obj0->Id() );

		_ReleaseObjects( cmdQueue, arr );
	}

	template <typename T0, typename T1>
	inline void CL2ComputeUtils::ReleaseObjects (cl::cl_command_queue cmdQueue, const T0 &obj0, const T1 &obj1)
	{
		FixedSizeArray< cl::cl_mem, 2 >	arr;

		if ( obj0 != null and obj0->IsShared() )	arr.PushBack( obj0->Id() );
		if ( obj1 != null and obj1->IsShared() )	arr.PushBack( obj1->Id() );

		_ReleaseObjects( cmdQueue, arr );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENCL
