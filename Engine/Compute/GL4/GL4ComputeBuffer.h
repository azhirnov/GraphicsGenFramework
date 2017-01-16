// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

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
		MemoryBufferPtr		_shared;
		EMemoryAccess::type	_flags;


	// methods
	protected:
		explicit GL4ComputeBuffer (const SubSystemsRef ss);
		~GL4ComputeBuffer ();

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

		void Bind (uint bindingIndex, EMemoryAccess::type access) const;
		
		bool					IsShared ()			const	{ return _shared.IsNotNull(); }
		BaseObjectPtr const&	GetSharedObject ()	const	{ return _shared; }
		
		EMemoryAccess::type		MemoryAccess ()		const	{ return _flags; }

		Bytes<usize>			Size ()				const	{ return Bytes<usize>( _shared.IsNull() ? 0 : _shared->Size() ); }
		bool					IsCreated ()		const	{ return Id().IsValid(); }
		BufferID				Id ()				const	{ return _shared.IsNull() ? BufferID() : _shared->GetBufferID(); }

		static ComputeBufferPtr  New (const SubSystemsRef ss);
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
		_shared->ClearSubData( value, Bytes<usize>(0), Size() );
	}
	
	inline void GL4ComputeBuffer::Clear ()
	{
		return Clear( ubyte(0) );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
