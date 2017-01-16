// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "CudaComputeBuffer.h"
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
	CudaComputeBuffer::CudaComputeBuffer (const SubSystemsRef ss) :
		BaseObject( ss ), _devicePtr( 0 ), _dataSize( 0 )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	CudaComputeBuffer::~CudaComputeBuffer ()
	{
		_Destroy();
	}
	
/*
=================================================
	Create
=================================================
*/
	bool CudaComputeBuffer::Create (usize size)
	{
		using namespace cuda;

		_Destroy();

		CU_CHECK( cuMemAlloc( &_devicePtr, size ) );

		_dataSize = size;
		return true;
	}
	
/*
=================================================
	_Destroy
=================================================
*/
	void CudaComputeBuffer::_Destroy ()
	{
		using namespace cuda;

		if ( _devicePtr != 0 )
		{
			CU_CALL( cuMemFree( _devicePtr ) );
			_devicePtr = 0;
		}

		_dataSize = 0;
	}

/*
=================================================
	Write
=================================================
*/
	bool CudaComputeBuffer::Write (BinaryBuffer data, usize offset)
	{
		using namespace cuda;

		CU_CHECK( cuMemcpyHtoD( _devicePtr + offset, data.ptr(), data.Size() ) );
		return true;
	}
	
/*
=================================================
	Read
=================================================
*/
	bool CudaComputeBuffer::Read (OUT Buffer<ubyte> &data, usize offset)
	{
		using namespace cuda;

		CU_CHECK( cuMemcpyDtoH( data.ptr(), _devicePtr + offset, data.Size() ) );
		return true;
	}
	
/*
=================================================
	Copy
=================================================
*/
	bool CudaComputeBuffer::Copy (const ComputeBufferPtr &fromBuffer, usize size, usize srcOffset, usize dstOffset)
	{
		using namespace cuda;

		if ( size == 0 )
			size = Min( _dataSize, fromBuffer->_dataSize );

		//CU_CHECK( cuMemcpy( _devicePtr, fromBuffer->Id(), size ) );
		CU_CHECK( cuMemcpyAsync( _devicePtr + dstOffset, fromBuffer->Id() + srcOffset, size,
								 SubSystems()->Get< ComputeEngine >()->GetStream() ) );
		return true;
	}
	
/*
=================================================
	Copy
=================================================
*/
	bool CudaComputeBuffer::Copy (const ComputeImagePtr &fromImage, usize size, usize srcOffset, usize dstOffset)
	{
		using namespace cuda;

		CU_CHECK( cuMemcpyAtoD( _devicePtr + dstOffset, fromImage->Id(), srcOffset, size ) );
		return true;
	}
	
/*
=================================================
	_Clear8
=================================================
*/
	void CudaComputeBuffer::_Clear8 (ubyte value)
	{
		using namespace cuda;

		CU_CALL( cuMemsetD8Async( _devicePtr, value, _dataSize / sizeof(value),
									SubSystems()->Get< ComputeEngine >()->GetStream() ) );
	}
	
/*
=================================================
	_Clear16
=================================================
*/
	void CudaComputeBuffer::_Clear16 (ushort value)
	{
		using namespace cuda;

		CU_CALL( cuMemsetD16Async( _devicePtr, value, _dataSize / sizeof(value),
									SubSystems()->Get< ComputeEngine >()->GetStream() ) );
	}
	
/*
=================================================
	_Clear32
=================================================
*/
	void CudaComputeBuffer::_Clear32 (uint value)
	{
		using namespace cuda;

		CU_CALL( cuMemsetD32Async( _devicePtr, value, _dataSize / sizeof(value),
									SubSystems()->Get< ComputeEngine >()->GetStream() ) );
	}

/*
=================================================
	New
=================================================
*/
	ComputeBufferPtr  CudaComputeBuffer::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new CudaComputeBuffer( ss ) );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_CUDA
