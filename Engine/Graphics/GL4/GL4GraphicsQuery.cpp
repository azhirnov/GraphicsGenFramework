// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "gl4.h"

#if defined( GRAPHICS_API_OPENGL )

#include "Engine/Graphics/Query/GraphicsQuery.h"

namespace Engine
{
namespace Graphics
{
	
/*
=================================================
	_IsResultAvailable
=================================================
*/
	bool GraphicsQuery::_IsResultAvailable () const
	{
		using namespace gl;

		GLint	ready = 0;
		GL_CALL( glGetQueryObjectiv( _id.Id(), GL_QUERY_RESULT_AVAILABLE, &ready ) );
		return ready == GL_TRUE;
	}
	
/*
=================================================
	_SyncResult
----
	lock thread until result not available
=================================================
*/
	ulong GraphicsQuery::_SyncResult () const
	{
		using namespace gl;

		GLuint64	result = 0;
		GL_CALL( glGetQueryObjectui64v( _id.Id(), GL_QUERY_RESULT, &result ) );
		return result;
	}
	
/*
=================================================
	_AsyncResult
----
	!!! Only for query buffer !!!
	return result if available
=================================================
*
	ulong GraphicsQuery::_AsyncResult () const
	{
		using namespace gl;

		GLuint64	result = 0;
		GL_CALL( glGetQueryObjectui64v( _id.Id(), GL_QUERY_RESULT_NO_WAIT, &result ) );
		return result;
	}
*/

}	// Graphics
}	// Engine

#endif	// GRAPHICS_API_OPENGL
