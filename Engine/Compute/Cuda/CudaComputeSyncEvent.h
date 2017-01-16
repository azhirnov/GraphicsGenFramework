// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CudaEnums.h"

#if defined( COMPUTE_API_CUDA )

namespace Engine
{
namespace Compute
{
	
	class   CudaComputeSyncEvent;
	typedef CudaComputeSyncEvent		ComputeSyncEvent;

	SHARED_POINTER( ComputeSyncEvent );



	//
	// Sync Event
	//

	class CudaComputeSyncEvent : public BaseObject
	{
	// variables
	private:
		cuda::CUevent	_id;
		uint			_flags;


	// methods
	protected:
		explicit
		CudaComputeSyncEvent (const SubSystemsRef ss);
		~CudaComputeSyncEvent ();

	public:
		bool Create (EEventFlags::type flags);

		bool EnqueueMarker ();
		bool EnqueueWait ();
		bool Wait ();

		Time<double>	ElapsedTime (const ComputeSyncEventPtr &end);

		bool			IsCreated ()	const	{ return _id != null; }
		cuda::CUevent	Id ()			const	{ return _id; }

		static ComputeSyncEventPtr  New (const SubSystemsRef ss);

	private:
		void _Delete ();
	};


}	// Compute
}	// Engine

#endif	// COMPUTE_API_CUDA
