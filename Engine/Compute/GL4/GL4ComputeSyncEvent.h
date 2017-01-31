// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "GL4ComputeProgram.h"

#if defined( COMPUTE_API_OPENGL )

namespace Engine
{
namespace Compute
{

	class   GL4ComputeSyncEvent;
	typedef GL4ComputeSyncEvent		ComputeSyncEvent;
	SHARED_POINTER( ComputeSyncEvent );


	//
	// Sync Event
	//

	class GL4ComputeSyncEvent : public BaseObject
	{
	// variables
	private:
		SyncEventID		_syncID;


	// methods
	private:
		explicit
		GL4ComputeSyncEvent (const SubSystemsRef ss);
		~GL4ComputeSyncEvent ();

	public:
		bool IsCreated ()	const	{ return _syncID.IsValid(); }

		bool Create ();
		
		// CPU-side wait
		bool ClientWait (TimeL timeout);

		// GPU-side wait
		bool Wait ();

		bool IsSignaled () const;

		TimeL MaxTimeout () const;

		static ComputeSyncEventPtr  New (const SubSystemsRef ss);

	private:
		void _Delete ();
	};


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
