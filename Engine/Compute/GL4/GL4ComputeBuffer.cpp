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
		_flags( EMemoryAccess::Unknown ),
		_target( EBufferTarget::Unknown )
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

		_shared->BindBase( _target, bindingIndex, access );
	}

/*
=================================================
	Create
=================================================
*/
	bool GL4ComputeBuffer::Create (BytesU size, EMemoryAccess::type flags, EBufferTarget::type target)
	{
		_Destroy();
		
		CHECK_ERR( size > 0 );

		_flags	= flags;
		_target	= target;
		_shared	= GXTypes::New<MemoryBuffer>( SubSystems() );

		CHECK_ERR( _shared->Create() );
		CHECK_ERR( _shared->Allocate( size, EBufferUsage::Dynamic ) );

		return true;
	}
	
/*
=================================================
	Create
=================================================
*/
	bool GL4ComputeBuffer::Create (BinaryCBuffer data, EMemoryAccess::type flags, EBufferTarget::type target)
	{
		_Destroy();
		
		CHECK_ERR( not data.Empty() );
		
		_flags	= flags;
		_target	= target;
		_shared	= GXTypes::New<MemoryBuffer>( SubSystems() );

		CHECK_ERR( _shared->Create() );
		CHECK_ERR( _shared->SetData( data, EBufferUsage::Dynamic ) );

		return true;
	}
	
/*
=================================================
	Create
=================================================
*/
	bool GL4ComputeBuffer::Create (const MemoryBufferPtr &shared, EMemoryAccess::type flags, EBufferTarget::type target)
	{
		_Destroy();

		CHECK_ERR( shared and shared->IsValid() );

		_shared	= shared;
		_flags	= flags;
		_target	= target;

		return true;
	}
	
/*
=================================================
	Read
=================================================
*/
	bool GL4ComputeBuffer::Read (OUT BinaryBuffer data, BytesU offset) const
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
	bool GL4ComputeBuffer::Write (BinaryCBuffer data, BytesU offset)
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
	ComputeBufferPtr  GL4ComputeBuffer::New (const MemoryBufferPtr &shared, EMemoryAccess::type flags)
	{
		CHECK_ERR( shared );

		ComputeBufferPtr	buf = GXTypes::New<GL4ComputeBuffer>( shared->SubSystems() );

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
		_target	= EBufferTarget::Unknown;
	}
		

}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
