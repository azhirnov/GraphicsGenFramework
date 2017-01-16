// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "CudaComputeSyncEvent.h"
#include "../Engine/ComputeEngine.h"

#if defined( COMPUTE_API_CUDA )

namespace Engine
{
namespace Compute
{
	
/*
=================================================
	constructor
=================================================
*/
	CudaComputeSyncEvent::CudaComputeSyncEvent (const SubSystemsRef ss) :
		BaseObject( ss ), _id( null ), _flags( 0 )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	CudaComputeSyncEvent::~CudaComputeSyncEvent ()
	{
		_Delete();
	}
	
/*
=================================================
	Create
=================================================
*/
	bool CudaComputeSyncEvent::Create (EEventFlags::type flags)
	{
		using namespace cuda;

		_Delete();

		_flags = CudaEnum( flags );

		CU_CHECK( cuEventCreate( &_id, _flags ) );
		return true;
	}
	
/*
=================================================
	_Delete
=================================================
*/
	void CudaComputeSyncEvent::_Delete ()
	{
		using namespace cuda;

		if ( _id != null )
		{
			CU_CALL( cuEventDestroy( _id ) );
			_id = null;
		}
		_flags = 0;
	}
	
/*
=================================================
	EnqueueMarker
=================================================
*/
	bool CudaComputeSyncEvent::EnqueueMarker ()
	{
		using namespace cuda;

		CU_CHECK( cuEventRecord( _id, SubSystems()->Get< ComputeEngine >()->GetStream() ) );
		return true;
	}
	
/*
=================================================
	Wait
=================================================
*/
	bool CudaComputeSyncEvent::Wait ()
	{
		using namespace cuda;

		CU_CHECK( cuEventSynchronize( _id ) );
		return true;
	}
	
/*
=================================================
	EnqueueWait
=================================================
*/
	bool CudaComputeSyncEvent::EnqueueWait ()
	{
		using namespace cuda;

		CU_CHECK( cuStreamWaitEvent( SubSystems()->Get< ComputeEngine >()->GetStream(), _id, 0 ) );
		return true;
	}
	
/*
=================================================
	ElapsedTime
=================================================
*/
	Time<double> CudaComputeSyncEvent::ElapsedTime (const ComputeSyncEventPtr &end)
	{
		using namespace cuda;

		float	time = 0.0f;

		CU_CALL( cuEventElapsedTime( &time, _id, end->Id() ) );
		return Time<double>().FromMilliSec( time );
	}

/*
=================================================
	New
=================================================
*/
	ComputeSyncEventPtr  CudaComputeSyncEvent::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new CudaComputeSyncEvent( ss ) );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_CUDA
