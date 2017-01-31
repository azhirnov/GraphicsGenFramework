// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "GL4ComputeBuffer.h"

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
	GL4ComputeBuffer::GL4ComputeBuffer (const SubSystemsRef ss) :
		BaseObject( ss ),
		_flags( EMemoryAccess::Unknown )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	GL4ComputeBuffer::~GL4ComputeBuffer ()
	{
		_Destroy();
	}
	
/*
=================================================
	Bind
=================================================
*/
	void GL4ComputeBuffer::Bind (uint bindingIndex, EMemoryAccess::type access) const
	{
		CHECK( IsCreated() );
		CHECK( EnumEqMask( access, _flags & access, EMemoryAccess::Write ) );

		_shared->BindBase( EBufferTarget::ShaderStorage, bindingIndex, access );
	}

/*
=================================================
	Create
=================================================
*/
	bool GL4ComputeBuffer::Create (BytesU size, EMemoryAccess::type flags)
	{
		_Destroy();
		
		CHECK_ERR( size > 0 );

		_flags	= flags;
		_shared	= MemoryBuffer::New( SubSystems() );

		CHECK_ERR( _shared->Create() );
		CHECK_ERR( _shared->Allocate( size, EBufferUsage::Dynamic ) );

		return true;
	}
	
/*
=================================================
	Create
=================================================
*/
	bool GL4ComputeBuffer::Create (BinaryBuffer data, EMemoryAccess::type flags)
	{
		_Destroy();
		
		CHECK_ERR( not data.Empty() );
		
		_flags	= flags;
		_shared	= MemoryBuffer::New( SubSystems() );

		CHECK_ERR( _shared->Create() );
		CHECK_ERR( _shared->SetData( data, EBufferUsage::Dynamic ) );

		return true;
	}
	
/*
=================================================
	Create
=================================================
*/
	bool GL4ComputeBuffer::Create (const MemoryBufferPtr &shared, EMemoryAccess::type flags)
	{
		_Destroy();

		CHECK_ERR( shared and shared->IsValid() );

		_shared	= shared;
		_flags	= flags;

		return true;
	}
	
/*
=================================================
	Read
=================================================
*/
	bool GL4ComputeBuffer::Read (OUT Buffer<ubyte> data, BytesU offset) const
	{
		CHECK_ERR( IsCreated() );

		CHECK_ERR( _shared->GetData( data, offset ) );
		return true;
	}
	
/*
=================================================
	Write
=================================================
*/
	bool GL4ComputeBuffer::Write (BinaryBuffer data, BytesU offset)
	{
		CHECK_ERR( IsCreated() );

		CHECK_ERR( _shared->SubData( data, offset ) );
		return true;
	}
	
/*
=================================================
	Copy
=================================================
*/
	bool GL4ComputeBuffer::Copy (const ComputeImagePtr &src, const uint4 &srcOffset, BytesU dstOffset, const uint4 &size)
	{
		return src->CopyTo( this, srcOffset, dstOffset, size );
	}
	
/*
=================================================
	Copy
=================================================
*/
	bool GL4ComputeBuffer::Copy (const ComputeBufferPtr &src, BytesU srcOffset, BytesU dstOffset, BytesU size)
	{
		CHECK_ERR( src );
		CHECK_ERR( this->IsCreated() and src->IsCreated() );

		return _shared->Copy( src, srcOffset, dstOffset, size );
	}
	
/*
=================================================
	New
=================================================
*/
	ComputeBufferPtr  GL4ComputeBuffer::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new GL4ComputeBuffer( ss ) );
	}
	
/*
=================================================
	New
=================================================
*/
	ComputeBufferPtr  GL4ComputeBuffer::New (const MemoryBufferPtr &shared, EMemoryAccess::type flags)
	{
		ComputeBufferPtr	buf = New( shared->SubSystems() );

		CHECK_ERR( buf->Create( shared, flags ) );
		return buf;
	}
	
/*
=================================================
	_Destroy
=================================================
*/
	void GL4ComputeBuffer::_Destroy ()
	{
		_shared	= null;
		_flags	= EMemoryAccess::Unknown;
	}
		

}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
