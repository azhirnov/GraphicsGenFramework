// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CL2ComputeProgram.h"

#if defined( COMPUTE_API_OPENCL )

namespace Engine
{
namespace Compute
{
	
	class   CL2ComputeSyncEvent;
	typedef CL2ComputeSyncEvent		ComputeSyncEvent;
	SHARED_POINTER( ComputeSyncEvent );



	//
	// Sync Event
	//

	class CL2ComputeSyncEvent : public BaseObject
	{
	// variables
	private:
		cl::cl_event	_id;


	// methods
	private:
		explicit
		CL2ComputeSyncEvent (const SubSystemsRef ss);
		~CL2ComputeSyncEvent ();

	public:
		bool IsCreated ()	const	{ return _id != null; }

		bool EnqueueMarker ();
		
		// CPU-side wait
		bool Wait ();
		
		// GPU-side wait
		bool EnqueueWait ();

		static ComputeSyncEventPtr  New (const SubSystemsRef ss);

	private:
		void _Delete ();
	};


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENCL
