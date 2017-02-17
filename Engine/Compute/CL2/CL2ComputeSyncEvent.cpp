// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "CL2ComputeSyncEvent.h"

#if defined( COMPUTE_API_OPENCL )

#include "Engine/Compute/Engine/ComputeEngine.h"

namespace Engine
{
namespace Compute
{
	
/*
=================================================
	constructor
=================================================
*/
	CL2ComputeSyncEvent::CL2ComputeSyncEvent (const SubSystemsRef ss) :
		BaseObject( ss ), _id( null )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	CL2ComputeSyncEvent::~CL2ComputeSyncEvent ()
	{
		_Delete();
	}
	
/*
=================================================
	EnqueueMarker
=================================================
*/
	bool CL2ComputeSyncEvent::EnqueueMarker ()
	{
		using namespace cl;

		_Delete();

		CL_CHECK( clEnqueueMarkerWithWaitList(
					SubSystems()->Get< ComputeEngine >()->GetCommandQueue(),
					0, null,
					&_id ) );

		CHECK_ERR( IsCreated() );
		return true;
	}
		
/*
=================================================
	Wait
=================================================
*/
	bool CL2ComputeSyncEvent::Wait ()
	{
		using namespace cl;
		
		CHECK_ERR( IsCreated() );

		CL_CHECK( clWaitForEvents( 1, &_id ) );
		return true;
	}
	
/*
=================================================
	EnqueueWait
=================================================
*/
	bool CL2ComputeSyncEvent::EnqueueWait ()
	{
		using namespace cl;
		
		CHECK_ERR( IsCreated() );

		CL_CHECK( clEnqueueBarrierWithWaitList(
					SubSystems()->Get< ComputeEngine >()->GetCommandQueue(),
					1, &_id,
					null ) );
		return true;
	}
	
/*
=================================================
	_Delete
=================================================
*/
	void CL2ComputeSyncEvent::_Delete ()
	{
		using namespace cl;

		if ( _id == null )
			return;

		CL_CALL( clReleaseEvent( _id ) );
		_id = null;
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENCL
