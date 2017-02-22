// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "gl4.h"

#if defined( GRAPHICS_API_OPENGL )

#include "Engine/Graphics/Buffer/MemoryBuffer.h"
#include "Engine/Graphics/Engine/GraphicsEngine.h"

namespace Engine
{
namespace Graphics
{
	
/*
=================================================
	_BufferData
=================================================
*/
	bool MemoryBuffer::_BufferData (const BufferID &buf, const void *data, usize dataSize, EBufferUsage::type usage)
	{
		using namespace gl;

		GL_CHECK( glNamedBufferData( buf.Id(), GLsizei(dataSize), data, GL4Enum( usage ) ) );
		return true;
	}
	
/*
=================================================
	_BufferSubData
=================================================
*/
	bool MemoryBuffer::_BufferSubData (const BufferID &buf, const void *data, usize dataSize, usize offset)
	{
		using namespace gl;

		GL_CHECK( glNamedBufferSubData( buf.Id(), GLintptr(offset), GLsizei(dataSize), data ) );
		return true;
	}
	
/*
=================================================
	_GetBufferSubData
=================================================
*/
	bool MemoryBuffer::_GetBufferSubData (const BufferID &buf, void *data, usize dataSize, usize offset)
	{
		using namespace gl;

		GL_CHECK( glGetNamedBufferSubData( buf.Id(), GLintptr(offset), GLsizei(dataSize), data ) );
		return true;
	}
	
/*
=================================================
	_ClearSubData
=================================================
*/
	bool MemoryBuffer::_ClearSubData (const BufferID &buf, usize offset, usize size, EPixelFormat::type format, BinaryCBuffer pattern)
	{
		using namespace gl;

		CHECK_ERR( EPixelFormat::IsSupportedForBuffer( format ) );
		
		GL4InternalPixelFormat	ifmt;
		GL4PixelFormat			fmt;
		GL4PixelType			type;

		CHECK( GL4Enum( format, OUT ifmt, OUT fmt, OUT type ) );

		GL_CALL( glClearNamedBufferSubData( buf.Id(), ifmt, offset, (GLsizei)size, fmt, type, pattern.ptr() ) );
		return true;
	}
	
/*
=================================================
	_ClearSubData
=================================================
*/
	bool MemoryBuffer::_CopyBufferSubData (const BufferID &src, const BufferID &dst, usize readOffset, usize writeOffset, usize size)
	{
		using namespace gl;

		GL_CALL( glCopyNamedBufferSubData( src.Id(), dst.Id(), (GLsizei)readOffset, (GLsizei)writeOffset, (GLsizei)size ) );
		return true;
	}


}	// Graphics
}	// Engine

#endif	// GRAPHICS_API_OPENGL
