// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "CL2ComputeBuffer.h"

#if defined( COMPUTE_API_OPENCL )

#include "Engine/Compute/Engine/ComputeEngine.h"
#include "CL2ComputeUtils.h"

namespace Engine
{
namespace Compute
{
	
/*
=================================================
	constructor
=================================================
*/
	CL2ComputeBuffer::CL2ComputeBuffer (const SubSystemsRef ss) :
		BaseObject( ss ),
		_shared( null ), _id( null ), _size( 0 ), _flags( EMemoryAccess::Unknown )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	CL2ComputeBuffer::~CL2ComputeBuffer ()
	{
		_Destroy();
	}
	
/*
=================================================
	Create
=================================================
*/
	bool CL2ComputeBuffer::Create (BytesU size, EMemoryAccess::type flags)
	{
		using namespace cl;

		_Destroy();

		CHECK_ERR( size > 0 );
		
		cl_int	cl_err = 0;

		CL_CHECK( ((_id = clCreateBuffer(
							SubSystems()->Get< ComputeEngine >()->GetContext(),
							CL2Enum( flags ),
							(size_t)size,
							null,
							&cl_err )), cl_err) );

		_size	= size;
		_flags	= flags;
		return true;
	}
	
/*
=================================================
	Create
=================================================
*/
	bool CL2ComputeBuffer::Create (BinaryBuffer data, EMemoryAccess::type flags)
	{
		using namespace cl;

		_Destroy();

		CHECK_ERR( not data.Empty() );
		
		cl_int	cl_err = 0;
		
		CL_CHECK( ((_id = clCreateBuffer(
							SubSystems()->Get< ComputeEngine >()->GetContext(),
							CL2Enum( flags ) | CL_MEM_COPY_HOST_PTR,
							(size_t)data.Size(),
							(void *) data.ptr(),
							&cl_err )), cl_err) );
		
		_size	= data.Size();
		_flags	= flags;
		return true;
	}
		
# if defined( GRAPHICS_API_OPENGL )
/*
=================================================
	Create
=================================================
*/	
	bool CL2ComputeBuffer::Create (const MemoryBufferPtr &shared, EMemoryAccess::type flags)
	{
		using namespace cl;
		
		_Destroy();
		
		CHECK_ERR( shared and shared->IsValid() );

		cl_int	cl_err = 0;

		CL_CHECK( ((_id = clCreateFromGLBuffer(
							SubSystems()->Get< ComputeEngine >()->GetContext(),
							CL2Enum( flags ),
							shared->GetBufferID().Id(),
							&cl_err )), cl_err ) );

		//CL_CALL( clGetMemObjectInfo( _id, CL_MEM_SIZE, sizeof(_size), &_size, null ) );

		_size	= shared->Size();
		_shared	= shared;
		_flags	= flags;
		return true;
	}

#endif	// GRAPHICS_API_OPENGL

/*
=================================================
	_Destroy
=================================================
*/
	void CL2ComputeBuffer::_Destroy ()
	{
		using namespace cl;

		_shared = null;
		_size	= BytesU();
		_flags	= EMemoryAccess::Unknown;

		if ( _id == null )
			return;

		CL_CALL( clReleaseMemObject( _id ) );
		_id = null;
	}

/*
=================================================
	Read
=================================================
*/
	bool CL2ComputeBuffer::Read (Buffer<ubyte> data, BytesU offset) const
	{
		using namespace cl;
		
		CHECK_ERR( IsCreated() );
		CHECK_ERR( offset + data.Size() <= _size );
		
		cl_command_queue	cmd_queue = SubSystems()->Get< ComputeEngine >()->GetCommandQueue();

		CL2ComputeUtils::AcquireObjects( cmd_queue, this );

		CL_CHECK( clEnqueueReadBuffer(
					cmd_queue,
					_id,
					CL_TRUE,
					(size_t)offset,
					(size_t)data.Size(),
					data.ptr(),
					0, null,
					null ) );

		CL2ComputeUtils::ReleaseObjects( cmd_queue, this );
		return true;
	}
	
/*
=================================================
	Write
=================================================
*/
	bool CL2ComputeBuffer::Write (BinaryBuffer data, BytesU offset)
	{
		using namespace cl;
		
		CHECK_ERR( IsCreated() );
		CHECK_ERR( offset + data.Size() <= _size );
		
		cl_command_queue	cmd_queue = SubSystems()->Get< ComputeEngine >()->GetCommandQueue();

		CL2ComputeUtils::AcquireObjects( cmd_queue, this );

		CL_CHECK( clEnqueueWriteBuffer(
					cmd_queue,
					_id,
					CL_TRUE,
					(size_t)offset,
					(size_t)data.Size(),
					data.ptr(),
					0, null,
					null ) );

		CL2ComputeUtils::ReleaseObjects( cmd_queue, this );
		return true;
	}
	
/*
=================================================
	Copy
=================================================
*/
	bool CL2ComputeBuffer::Copy (const ComputeImagePtr &src, const uint4 &srcOffset, BytesU dstOffset, const uint4 &size)
	{
		return src->CopyTo( this, srcOffset, dstOffset, size );
	}
	
/*
=================================================
	Copy
=================================================
*/
	bool CL2ComputeBuffer::Copy (const ComputeBufferPtr &src, BytesU srcOffset, BytesU dstOffset, BytesU size)
	{
		using namespace cl;
		
		CHECK_ERR( IsCreated() );
		CHECK_ERR( src and src->IsCreated() );
		CHECK_ERR( srcOffset + size <= src->Size() );
		CHECK_ERR( dstOffset + size <= this->Size() );
		
		cl_command_queue	cmd_queue = SubSystems()->Get< ComputeEngine >()->GetCommandQueue();

		CL2ComputeUtils::AcquireObjects( cmd_queue, this, src );

		CL_CHECK( clEnqueueCopyBuffer(
					cmd_queue,
					src->_id,
					this->Id(),
					(size_t)srcOffset,
					(size_t)dstOffset,
					(size_t)size,
					0, null,
					null ) );

		CL2ComputeUtils::ReleaseObjects( cmd_queue, this, src );
		return true;
	}
		
/*
=================================================
	_FillBuffer
=================================================
*/
	bool CL2ComputeBuffer::_FillBuffer (usize offset, usize size, BinaryBuffer pattern)
	{
		using namespace cl;
		
		CHECK_ERR( IsCreated() );

		cl_command_queue	cmd_queue = SubSystems()->Get< ComputeEngine >()->GetCommandQueue();

		CL2ComputeUtils::AcquireObjects( cmd_queue, this );

		CL_CHECK( clEnqueueFillBuffer(
					SubSystems()->Get< ComputeEngine >()->GetCommandQueue(),
					_id,
					pattern.ptr(),
					(size_t)pattern.Size(),
					offset,
					size,
					0, null,
					null ) );

		CL2ComputeUtils::ReleaseObjects( cmd_queue, this );
		return true;
	}
		
/*
=================================================
	New
=================================================
*/
	ComputeBufferPtr  CL2ComputeBuffer::New (const MemoryBufferPtr &shared, EMemoryAccess::type flags)
	{
		CHECK_ERR( shared );

		ComputeBufferPtr	ptr = GXTypes::New<CL2ComputeBuffer>( shared->SubSystems() );

		CHECK_ERR( ptr->Create( shared, flags ) );
		return ptr;
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENCL
