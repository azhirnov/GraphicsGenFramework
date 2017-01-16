// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "GL4ComputeSyncEvent.h"

#if defined( COMPUTE_API_OPENGL )

namespace Engine
{
namespace Compute
{
	
/*
=================================================
	constructor
=================================================
*/
	GL4ComputeSyncEvent::GL4ComputeSyncEvent (const SubSystemsRef ss) :
		BaseObject( ss )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	GL4ComputeSyncEvent::~GL4ComputeSyncEvent ()
	{
		_Delete();
	}
	
/*
=================================================
	Create
=================================================
*/
	bool GL4ComputeSyncEvent::Create ()
	{
		using namespace gl;

		_Delete();

		CHECK_ERR( SubSystems()->Get< GraphicsEngine >()->GetContext()->FenceSync( _syncID ) );
		return true;
	}
	
/*
=================================================
	_Delete
=================================================
*/
	void GL4ComputeSyncEvent::_Delete ()
	{
		using namespace gl;

		SubSystems()->Get< GraphicsEngine >()->GetContext()->DeleteSync( _syncID );
	}
		
/*
=================================================
	ClientWait
=================================================
*/
	bool GL4ComputeSyncEvent::ClientWait (Time<ulong> timeout)
	{
		using namespace gl;

		CHECK_ERR( IsCreated() );

		GLenum	res = 0;
		GL_CALL( res = glClientWaitSync( _syncID.Id(), GL_SYNC_FLUSH_COMMANDS_BIT, timeout.NanoSeconds() ) );
		/*
		GL_ALREADY_SIGNALED		indicates that sync was signaled at the time that glClientWaitSync was called. 
		GL_TIMEOUT_EXPIRED		indicates that at least timeout nanoseconds passed and sync did not become signaled. 
		GL_CONDITION_SATISFIED	indicates that sync was signaled before the timeout expired. 
		GL_WAIT_FAILED			indicates that an error occurred. Additionally, an OpenGL error will be generated.
		*/
		return ( res == GL_ALREADY_SIGNALED or res == GL_CONDITION_SATISFIED );
	}
	
/*
=================================================
	Wait
=================================================
*/
	bool GL4ComputeSyncEvent::Wait ()
	{
		using namespace gl;

		CHECK_ERR( IsCreated() );

		GL_CALL( glWaitSync( _syncID.Id(), 0, GL_TIMEOUT_IGNORED ) );
		return true;
	}
	
/*
=================================================
	IsSignaled
=================================================
*/
	bool GL4ComputeSyncEvent::IsSignaled () const
	{
		using namespace gl;

		CHECK_ERR( IsCreated() );

		GLint	val = 0;
		GL_CALL( glGetSynciv( _syncID.Id(), GL_SYNC_STATUS, 1, null, &val ) );

		return val == GL_SIGNALED;
	}
	
/*
=================================================
	MaxTimeout
=================================================
*/
	Time<ulong> GL4ComputeSyncEvent::MaxTimeout () const
	{
		using namespace gl;

		GLint64	time = 0;
		GL_CALL( glGetInteger64v( GL_MAX_SERVER_WAIT_TIMEOUT, &time ) );

		return Time<ulong>().FromNanoSeconds( time );;
	}

/*
=================================================
	New
=================================================
*/
	ComputeSyncEventPtr  GL4ComputeSyncEvent::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new GL4ComputeSyncEvent( ss ) );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
