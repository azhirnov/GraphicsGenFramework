// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CL2ComputeImage.h"

#if defined( COMPUTE_API_OPENCL )

namespace Engine
{
namespace Compute
{

	//
	// Compute Buffer
	//
	 
	class CL2ComputeBuffer : public BaseObject
	{
	// variables
	private:
		BaseObjectPtr		_shared;
		cl::cl_mem			_id;
		usize				_size;
		EMemoryAccess::type	_flags;


	// methods
	protected:
		explicit CL2ComputeBuffer (const SubSystemsRef ss);
		~CL2ComputeBuffer ();

	public:
		bool Create (Bytes<usize> size, EMemoryAccess::type flags = EMemoryAccess::ReadWrite);
		bool Create (BinaryBuffer data, EMemoryAccess::type flags = EMemoryAccess::ReadWrite);
		bool Create (const MemoryBufferPtr &shared, EMemoryAccess::type flags = EMemoryAccess::ReadWrite);
		
		bool Read (Buffer<ubyte> data, Bytes<usize> offset = Uninitialized()) const;
		bool Write (BinaryBuffer data, Bytes<usize> offset = Uninitialized());

		bool Copy (const ComputeImagePtr &src, const uint4 &srcOffset, Bytes<usize> dstOffset, const uint4 &size);
		bool Copy (const ComputeBufferPtr &src, Bytes<usize> srcOffset, Bytes<usize> dstOffset, Bytes<usize> size);

		template <typename T>
		void Clear (const T &value);
		void Clear ();
		
		bool					IsShared ()			const	{ return _shared.IsNotNull(); }
		BaseObjectPtr const&	GetSharedObject ()	const	{ return _shared; }
		
		EMemoryAccess::type		MemoryAccess ()		const	{ return _flags; }

		Bytes<usize>			Size ()				const	{ return Bytes<usize>( _size ); }
		bool					IsCreated ()		const	{ return _id != null; }
		cl::cl_mem				Id ()				const	{ return _id; }

		static ComputeBufferPtr  New (const SubSystemsRef ss);


	private:
		void _FillBuffer (usize offset, usize size, BinaryBuffer pattern);

		void _Destroy ();
	};

	
/*
=================================================
	Clear
=================================================
*/
	template <typename T>
	inline void CL2ComputeBuffer::Clear (const T &value)
	{
		return _FillBuffer( 0, _size, BinaryBuffer::FromType( value ) );
	}
	
	inline void CL2ComputeBuffer::Clear ()
	{
		return Clear( ubyte(0) );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENCL
