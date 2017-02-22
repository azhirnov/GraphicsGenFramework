// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "MemoryBuffer.h"
#include "Engine/Graphics/Engine/GraphicsEngine.h"

namespace Engine
{
namespace Graphics
{
	
//------------------------------ MemoryBuffer -------------------------------//
/*
=================================================
	constructor
=================================================
*/
	MemoryBuffer::MemoryBuffer (const SubSystemsRef ss) :
		Resource( PackFileID(), EResource::MemoryBuffer, ss ),
		_size(0), _usage( EBufferUsage::Unknown ), _pendingBarrier()
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	MemoryBuffer::~MemoryBuffer ()
	{
		Destroy();
	}
	
/*
=================================================
	Create
=================================================
*/
	bool MemoryBuffer::Create ()
	{
		CHECK_ERR( not _IsResourceLocked() );

		Destroy();

		CHECK_ERR( SubSystems()->Get<GraphicsEngine>()->GetContext()->CreateBuffer( OUT _id ) );

		_SetResourceStatus( EResourceStatus::Created );
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void MemoryBuffer::Destroy ()
	{
		Resource::_Destroy();

		if ( SubSystems()->Get< GraphicsEngine >() )
		{
			// unbind and delete
			SubSystems()->Get<GraphicsEngine>()->GetContext()->DeleteBuffer( _id );
		}

		_size	= 0_b;
		_usage	= EBufferUsage::Unknown;
		
		_pendingBarrier.Reset();
	}
	
/*
=================================================
	_BarrierNow
=================================================
*/
	void MemoryBuffer::_BarrierNow (EMemoryBarrier::type flags)
	{
		SubSystems()->Get<GraphicsEngine>()->GetStateManager()->
			MemoryBarrier( INOUT _pendingBarrier, flags );
	}
	
/*
=================================================
	Barrier
=================================================
*/
	void MemoryBuffer::Barrier (EMemoryBarrier::type flags)
	{
		SubSystems()->Get<GraphicsEngine>()->GetStateManager()->
			MemoryBarrierBeforeDraw( INOUT _pendingBarrier, flags );
	}

/*
=================================================
	SetBarrier
=================================================
*
	void MemoryBuffer::SetBarrier (EMemoryBarrier::type flags)
	{
		SubSystems()->Get<GraphicsEngine>()->GetStateManager()->UpdateBarrier( INOUT _pendingBarrier, flags );
	}

/*
=================================================
	Allocate
=================================================
*/
	bool MemoryBuffer::Allocate (BytesU size, EBufferUsage::type usage)
	{
		CHECK_ERR( GetBufferID().IsValid() );
		CHECK_ERR( size > 0 );
		CHECK_ERR( not _IsResourceLocked() );

		// barrier not needed

		CHECK_ERR( _BufferData( GetBufferID(), null, (usize)size, usage ) );
		
		_EditMemStat().Set( BytesU(), size );

		_pendingBarrier.Reset();

		_size	= size;
		_usage	= usage;

		_SetResourceStatus( EResourceStatus::Loaded );
		return true;
	}

/*
=================================================
	_SetData
=================================================
*/
	bool MemoryBuffer::_SetData (BinaryCBuffer data, EBufferUsage::type usage)
	{
		CHECK_ERR( GetBufferID().IsValid() );
		CHECK_ERR( not data.Empty() );
		CHECK_ERR( not _IsResourceLocked() );
		
		// barrier not needed

		CHECK_ERR( _BufferData( GetBufferID(), data.ptr(), (usize)data.Size(), usage ) );
		
		_EditMemStat().Set( BytesU(), data.Size() );

		_pendingBarrier.Reset();

		_size	= data.Size();
		_usage	= usage;

		_SetResourceStatus( EResourceStatus::Loaded );
		return true;
	}

/*
=================================================
	_SubData
=================================================
*/
	bool MemoryBuffer::_SubData (BinaryCBuffer data, BytesU offset)
	{
		CHECK_ERR( GetBufferID().IsValid() );
		CHECK_ERR( not data.Empty() );
		CHECK_ERR( data.Size() + offset <= _size );
		CHECK_ERR( not _IsResourceLocked() );

		_BarrierNow( EMemoryBarrier::BufferUpdate );

		CHECK_ERR( _BufferSubData( GetBufferID(), data.ptr(), (usize)data.Size(), (usize)offset ) );
		return true;
	}
	
/*
=================================================
	_GetData
=================================================
*/
	bool MemoryBuffer::_GetData (OUT BinaryBuffer data, BytesU offset)
	{
		CHECK_ERR( GetBufferID().IsValid() );
		
		_BarrierNow( EMemoryBarrier::BufferUpdate );

		CHECK_ERR( _GetBufferSubData( GetBufferID(), data.ptr(), (usize)data.Size(), (usize)offset ) );
		return true;
	}
	
/*
=================================================
	BindBase
=================================================
*/
	bool MemoryBuffer::BindBase (EBufferTarget::type target, uint bindingIndex, EMemoryAccess::type access)
	{
		return BindBase( target, bindingIndex, access, 0_b, Size() );
	}
	
/*
=================================================
	BindBase
=================================================
*/
	bool MemoryBuffer::BindBase (EBufferTarget::type target, uint bindingIndex, EMemoryAccess::type access,
								 BytesU offset, BytesU size)
	{
		CHECK_ERR( GetBufferID().IsValid() );
		CHECK_ERR( offset + size <= Size() );
		CHECK_ERR( not (EnumEq( access, EMemoryAccess::Write ) and _IsResourceLocked()) );
		// TODO: check write access to unwritable targets

		Ptr< StateManager >		sm = SubSystems()->Get< GraphicsEngine >()->GetStateManager();

		EMemoryBarrier::type	target_barrier = EMemoryBarrier::Unknown;

		switch ( target )
		{
			case EBufferTarget::ShaderStorage :		target_barrier = EMemoryBarrier::ShaderStorage;		break;
			case EBufferTarget::Uniform :			target_barrier = EMemoryBarrier::Uniform;			break;
			case EBufferTarget::AtomicCounter :		target_barrier = EMemoryBarrier::AtomicCounter;		break;
			case EBufferTarget::TransformFeedback :	target_barrier = EMemoryBarrier::TransformFeedback;	break;
		}

		Barrier( target_barrier );
		
		sm->BindBufferBase( target, GetBufferID(), bindingIndex, offset, size );
		
		// add barrier if buffer may be changed
		if ( EnumEq( access, EMemoryAccess::Write ) and 
			(target == EBufferTarget::ShaderStorage or
			 target == EBufferTarget::AtomicCounter) )
		{
			sm->CreateBarrier( OUT _pendingBarrier );
		}
		return true;
	}
	
/*
=================================================
	Copy
=================================================
*/
	bool MemoryBuffer::Copy (const MemoryBufferPtr &src, BytesU srcOffset, BytesU dstOffset, BytesU size)
	{
		CHECK_ERR( GetBufferID().IsValid() );
		CHECK_ERR( src and src->GetBufferID().IsValid() );
		CHECK_ERR( srcOffset + size <= src->Size() );
		CHECK_ERR( dstOffset + size <= Size() );
		CHECK_ERR( not _IsResourceLocked() );
		
		_BarrierNow( EMemoryBarrier::BufferUpdate );

		return _CopyBufferSubData( src->GetBufferID(), GetBufferID(), (usize)srcOffset, (usize)dstOffset, (usize)size );
	}
	
/*
=================================================
	SwapData
=================================================
*/
	bool MemoryBuffer::SwapData (const ResourcePtr &other)
	{
		CHECK_ERR( Resource::SwapData( other ) );

		MemoryBuffer *	mbuf = other.ToPtr<MemoryBuffer>();

		SwapMembers( this, mbuf, &MemoryBuffer::_id );
		SwapMembers( this, mbuf, &MemoryBuffer::_size );
		SwapMembers( this, mbuf, &MemoryBuffer::_usage );
		SwapMembers( this, mbuf, &MemoryBuffer::_pendingBarrier );

		return true;
	}
	
/*
=================================================
	IsValid
=================================================
*/
	bool MemoryBuffer::IsValid () const
	{
		return _IsResourceLoaded() and GetBufferID().IsValid();
	}
	
	

//------------------------------ IndexBuffer --------------------------------//
/*
=================================================
	constructor
=================================================
*/
	IndexBuffer::IndexBuffer (const SubSystemsRef ss) :
		MemoryBuffer( ss ), _indexType( EIndex::Unknown )
	{
	}
	
/*
=================================================
	Destroy
=================================================
*/
	void IndexBuffer::Destroy ()
	{
		MemoryBuffer::Destroy();

		_indexType = EIndex::Unknown;
	}
	
/*
=================================================
	SetIndexType
=================================================
*/
	bool IndexBuffer::SetIndexType (EIndex::type indexType)
	{
		CHECK_ERR( not _IsResourceLocked() );

		_indexType = indexType;
		return true;
	}
	
/*
=================================================
	SwapData
=================================================
*/
	bool IndexBuffer::SwapData (const ResourcePtr &other)
	{
		CHECK_ERR( MemoryBuffer::SwapData( other ) );

		IndexBuffer *	ibuf = other.ToPtr<IndexBuffer>();

		SwapMembers( this, ibuf, &IndexBuffer::_indexType );

		return true;
	}
	
/*
=================================================
	IsValid
=================================================
*/
	bool IndexBuffer::IsValid () const
	{
		return MemoryBuffer::IsValid() and _indexType != EIndex::Unknown;
	}



//------------------------------ VertexBuffer -------------------------------//
/*
=================================================
	constructor
=================================================
*/
	VertexBuffer::VertexBuffer (const SubSystemsRef ss) :
		MemoryBuffer( ss ), _primitive( EPrimitive::Unknown ),
		_divisor(0), _stride(0)
	{
	}
	
/*
=================================================
	Destroy
=================================================
*/
	void VertexBuffer::Destroy ()
	{
		MemoryBuffer::Destroy();

		_primitive	= EPrimitive::Unknown;
		_divisor	= 0;
		_stride		= BytesU();
	}

/*
=================================================
	SetAttribs
=================================================
*/
	bool VertexBuffer::SetAttribs (const VertexAttribsPtr &attribs, BytesU vertexStride)
	{
		CHECK_ERR( not _IsResourceLocked() );
		ASSERT( vertexStride < MaxValue<ubyte>() );

		_attribs	= attribs;
		_stride		= vertexStride;

		return true;
	}
	
/*
=================================================
	SetAttribs
=================================================
*/
	bool VertexBuffer::SetAttribs (const VertexAttribsDesc &attribDesc, BytesU vertexStride)
	{
		CHECK_ERR( not _IsResourceLocked() );
		ASSERT( vertexStride < MaxValue<ubyte>() );

		CHECK( SubSystems()->Get<GraphicsEngine>()->GetContext()
			->CreateVertexAttribs( attribDesc, OUT _attribs ) );
		
		_stride = vertexStride;
		return true;
	}

/*
=================================================
	SetPrimitive
=================================================
*/
	bool VertexBuffer::SetPrimitive (EPrimitive::type primitve)
	{
		CHECK_ERR( not _IsResourceLocked() );

		_primitive = primitve;
		return true;
	}
	
/*
=================================================
	SetDivisor
=================================================
*/
	bool VertexBuffer::SetDivisor (uint divisor)
	{
		CHECK_ERR( not _IsResourceLocked() );

		_divisor = divisor;
		return true;
	}

/*
=================================================
	Draw
=================================================
*/
	bool VertexBuffer::Draw ()
	{
		return Draw( Count() );
	}

	bool VertexBuffer::Draw (usize count, usize offset)
	{
		CHECK_ERR( GetBufferID().IsValid() );
		CHECK( count + offset <= Count() );

		Ptr<StateManager>	sm = SubSystems()->Get<GraphicsEngine>()->GetStateManager();
		
		Barrier( EMemoryBarrier::VertexAttribArray );

		sm->BindAttribs( _attribs, _id, BytesU(), Stride(), _divisor );
		sm->BindAttribs( _attribs, BufferID() );
		sm->DrawArrays( _primitive, count, offset );

		return true;
	}
	
/*
=================================================
	DrawIndexed
=================================================
*/
	bool VertexBuffer::DrawIndexed (const IndexBufferPtr &indexBuffer)
	{
		return DrawIndexed( indexBuffer, indexBuffer->Count(), BytesU() );
	}

	bool VertexBuffer::DrawIndexed (const IndexBufferPtr &indexBuffer, usize count, BytesU offset)
	{
		CHECK_ERR( GetBufferID().IsValid() );
		CHECK_ERR( indexBuffer and indexBuffer->GetBufferID().IsValid() );
		CHECK_ERR( count * EIndex::SizeOf( indexBuffer->IndexType() ) + offset <= indexBuffer->Size() );

		Ptr<StateManager>	sm = SubSystems()->Get<GraphicsEngine>()->GetStateManager();
		
		Barrier( EMemoryBarrier::VertexAttribArray );
		indexBuffer->Barrier( EMemoryBarrier::ElementArray );

		sm->BindAttribs( _attribs, GetBufferID(), BytesU(), Stride(), _divisor );
		sm->BindAttribs( _attribs, indexBuffer->GetBufferID() );
		sm->DrawElements( _primitive, indexBuffer->IndexType(), count, offset );

		return true;
	}
	
/*
=================================================
	SwapData
=================================================
*/
	bool VertexBuffer::SwapData (const ResourcePtr &other)
	{
		CHECK_ERR( MemoryBuffer::SwapData( other ) );

		VertexBuffer *	vbuf = other.ToPtr<VertexBuffer>();

		SwapMembers( this, vbuf, &VertexBuffer::_attribs );
		SwapMembers( this, vbuf, &VertexBuffer::_primitive );
		SwapMembers( this, vbuf, &VertexBuffer::_divisor );
		SwapMembers( this, vbuf, &VertexBuffer::_stride );

		return true;
	}
	
/*
=================================================
	IsValid
=================================================
*/
	bool VertexBuffer::IsValid () const
	{
		return MemoryBuffer::IsValid() and _attribs != null and _primitive != EPrimitive::Unknown;
	}


}	// Graphics
}	// Engine
