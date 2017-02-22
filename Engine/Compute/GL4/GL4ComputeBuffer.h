// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "GL4ComputeImage.h"

#if defined( COMPUTE_API_OPENGL )

namespace Engine
{
namespace Compute
{

	//
	// Compute Buffer
	//
	 
	class GL4ComputeBuffer : public BaseObject
	{
	// variables
	private:
		MemoryBufferPtr			_shared;
		EMemoryAccess::type		_flags;
		EBufferTarget::type		_target;


	// methods
	public:
		explicit
		GL4ComputeBuffer (const SubSystemsRef ss);
		~GL4ComputeBuffer ();

		bool Create (BytesU size, EMemoryAccess::type flags = EMemoryAccess::ReadWrite, EBufferTarget::type target = EBufferTarget::ShaderStorage);
		bool Create (BinaryCBuffer data, EMemoryAccess::type flags = EMemoryAccess::ReadWrite, EBufferTarget::type target = EBufferTarget::ShaderStorage);
		bool Create (const MemoryBufferPtr &shared, EMemoryAccess::type flags = EMemoryAccess::ReadWrite, EBufferTarget::type target = EBufferTarget::ShaderStorage);
		
		bool Read (OUT BinaryBuffer data, BytesU offset = Uninitialized) const;
		bool Write (BinaryCBuffer data, BytesU offset = Uninitialized);

		bool Copy (const ComputeImagePtr &src, const uint4 &srcOffset, BytesU dstOffset, const uint4 &size);
		bool Copy (const ComputeBufferPtr &src, BytesU srcOffset, BytesU dstOffset, BytesU size);

		template <typename T>
		void Clear (const T &value);
		void Clear ();

		void Bind (uint bindingIndex, EMemoryAccess::type access) const;
		
		bool					IsShared ()			const	{ return _shared.IsNotNull(); }
		BaseObjectPtr const&	GetSharedObject ()	const	{ return _shared; }
		
		EMemoryAccess::type		MemoryAccess ()		const	{ return _flags; }
		EBufferTarget::type		Target ()			const	{ return _target; }

		BytesU					Size ()				const	{ return _shared ? _shared->Size() : 0_b; }
		bool					IsCreated ()		const	{ return Id().IsValid(); }
		BufferID				Id ()				const	{ return _shared ? _shared->GetBufferID() : BufferID(); }

		static ComputeBufferPtr  New (const MemoryBufferPtr &shared, EMemoryAccess::type flags = EMemoryAccess::ReadWrite);


	private:
		void _Destroy ();
	};

	
/*
=================================================
	Clear
=================================================
*/
	template <typename T>
	inline void GL4ComputeBuffer::Clear (const T &value)
	{
		_shared->ClearSubData( value, 0_b, Size() );
	}
	
	inline void GL4ComputeBuffer::Clear ()
	{
		return Clear( ubyte(0) );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
