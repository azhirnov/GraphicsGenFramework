// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CudaComputeImage.h"

#if defined( COMPUTE_API_CUDA )

namespace Engine
{
namespace Compute
{

	//
	// Compute Buffer
	//

	class CudaComputeBuffer : public BaseObject
	{
	// variables
	private:
		//BaseObjectPtr		_shared;

		cuda::CUdeviceptr	_devicePtr;
		usize				_dataSize;


	// methods
	public:
		explicit
		CudaComputeBuffer (const SubSystemsRef ss);
		~CudaComputeBuffer ();

		bool Create (usize size);

		bool Write (BinaryBuffer data, usize offset = 0);
		bool Read (OUT Buffer<ubyte> &data, usize offset = 0);

		bool Copy (const ComputeBufferPtr &fromBuffer, usize size, usize srcOffset = 0, usize dstOffset = 0);
		bool Copy (const ComputeImagePtr &fromImage, usize size, usize srcOffset = 0, usize dstOffset = 0);

		template <typename T>
		void Clear (T value);
		void Clear ();
		
		void Bind (cuda::CUdeviceptr devPtr, usize sizeOf) {}	// TODO: ???
		
		//bool					IsShared ()		const	{ return _shared.IsNotNull(); }
		//BaseObjectPtr const&	GetObject ()	const	{ return _shared; }

		bool					IsCreated ()	const	{ return _devicePtr != 0; }
		cuda::CUdeviceptr		Id ()			const	{ return _devicePtr; }
		Bytes<usize>					Size ()			const	{ return Bytes<usize>( _dataSize ); }

		static ComputeBufferPtr  New (const SubSystemsRef ss);


	private:
		void _Destroy ();

		void _Clear8 (ubyte value);
		void _Clear16 (ushort value);
		void _Clear32 (uint value);
	};

	
/*
=================================================
	Clear
=================================================
*/
	template <> inline void CudaComputeBuffer::Clear (byte value)	{ return _Clear8( ReferenceCast<ubyte>( value ) ); }
	template <> inline void CudaComputeBuffer::Clear (ubyte value)	{ return _Clear8( value ); }

	template <> inline void CudaComputeBuffer::Clear (short value)	{ return _Clear16( ReferenceCast<ushort>( value ) ); }
	template <> inline void CudaComputeBuffer::Clear (ushort value)	{ return _Clear16( value ); }
	template <> inline void CudaComputeBuffer::Clear (half value)	{ return _Clear16( ReferenceCast<ushort>( value ) ); }

	template <> inline void CudaComputeBuffer::Clear (int value)	{ return _Clear32( ReferenceCast<uint>( value ) ); }
	template <> inline void CudaComputeBuffer::Clear (uint value)	{ return _Clear32( value ); }
	template <> inline void CudaComputeBuffer::Clear (float value)	{ return _Clear32( ReferenceCast<uint>( value ) ); }
	
	inline void CudaComputeBuffer::Clear ()
	{
		_Clear8( 0 );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_CUDA
