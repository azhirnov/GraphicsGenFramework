// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

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
		_size(0), _usage(EBufferUsage::type(-1)), _pendingBarrier()
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	MemoryBuffer::~MemoryBuffer ()
	{
		SubSystems()->Get<GraphicsEngine>()->GetContext()->DeleteBuffer( _id );
	}
	
/*
=================================================
	Create
=================================================
*/
	bool MemoryBuffer::Create ()
	{
		Destroy();

		CHECK_ERR( SubSystems()->Get<GraphicsEngine>()->GetContext()->CreateBuffer( OUT _id ) );

		_SetLoadState( ELoadState::Created );
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void MemoryBuffer::Destroy ()
	{
		SubSystems()->Get<GraphicsEngine>()->GetStateManager()->UnbindBufferBase( _id );

		SubSystems()->Get<GraphicsEngine>()->GetContext()->DeleteBuffer( _id );

		_size	= Bytes<usize>( 0 );
		_usage	= EBufferUsage::type(-1);
		
		_pendingBarrier.Reset();

		_SetLoadState( ELoadState::None );
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
	bool MemoryBuffer::Allocate (Bytes<usize> size, EBufferUsage::type usage)
	{
		CHECK_ERR( GetBufferID().IsValid() );
		CHECK_ERR( size > 0 );

		// barrier not needed

		CHECK_ERR( _BufferData( GetBufferID(), null, size, usage ) );
		
		_pendingBarrier.Reset();

		_size	= size;
		_usage	= usage;

		_SetLoadState( ELoadState::Loaded );
		return true;
	}

/*
=================================================
	_SetData
=================================================
*/
	bool MemoryBuffer::_SetData (Buffer<const ubyte> data, EBufferUsage::type usage)
	{
		CHECK_ERR( GetBufferID().IsValid() );
		CHECK_ERR( not data.Empty() );
		
		// barrier not needed

		CHECK_ERR( _BufferData( GetBufferID(), data.ptr(), data.Size(), usage ) );
		
		_pendingBarrier.Reset();

		_size	= data.Size();
		_usage	= usage;

		_SetLoadState( ELoadState::Loaded );
		return true;
	}

/*
=================================================
	_SubData
=================================================
*/
	bool MemoryBuffer::_SubData (Buffer<const ubyte> data, Bytes<usize> offset)
	{
		CHECK_ERR( GetBufferID().IsValid() );
		CHECK_ERR( not data.Empty() );
		CHECK_ERR( data.Size() + offset <= _size );

		_BarrierNow( EMemoryBarrier::BufferUpdate );

		CHECK_ERR( _BufferSubData( GetBufferID(), data.ptr(), data.Size(), offset ) );
		return true;
	}
	
/*
=================================================
	_GetData
=================================================
*/
	bool MemoryBuffer::_GetData (OUT Buffer<ubyte> &data, Bytes<usize> offset)
	{
		CHECK_ERR( GetBufferID().IsValid() );
		
		_BarrierNow( EMemoryBarrier::BufferUpdate );

		CHECK_ERR( _GetBufferSubData( GetBufferID(), data.ptr(), data.Size(), offset ) );
		return true;
	}
	
/*
=================================================
	BindBase
=================================================
*/
	void MemoryBuffer::BindBase (EBufferTarget::type target, uint bindingIndex, EMemoryAccess::type access)
	{
		return BindBase( target, bindingIndex, access, Bytes<usize>(0), Size() );
	}
	
/*
=================================================
	BindBase
=================================================
*/
	void MemoryBuffer::BindBase (EBufferTarget::type target, uint bindingIndex, EMemoryAccess::type access,
								 Bytes<usize> offset, Bytes<usize> size)
	{
		CHECK_ERR( GetBufferID().IsValid(), void() );
		CHECK_ERR( offset + size <= Size(), void() );
		// TODO: check write access to unwritable targets

		Ptr< StateManager >		sm = SubSystems()->Get< GraphicsEngine >()->GetStateManager();

		EMemoryBarrier::type	target_barrier = EMemoryBarrier::type(0);

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
	}
	
/*
=================================================
	Copy
=================================================
*/
	bool MemoryBuffer::Copy (const MemoryBufferPtr &src, Bytes<usize> srcOffset, Bytes<usize> dstOffset, Bytes<usize> size)
	{
		CHECK_ERR( src.IsNotNull() );
		CHECK_ERR( srcOffset + size <= src->Size() );
		CHECK_ERR( dstOffset + size <= Size() );
		
		_BarrierNow( EMemoryBarrier::BufferUpdate );

		return _CopyBufferSubData( src->GetBufferID(), GetBufferID(), srcOffset, dstOffset, size );
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
		return IsResourceLoaded() and GetBufferID().IsValid();
	}

/*
=================================================
	New
=================================================
*/
	MemoryBufferPtr  MemoryBuffer::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new MemoryBuffer( ss ) );
	}
	
	

//------------------------------ IndexBuffer --------------------------------//
/*
=================================================
	constructor
=================================================
*/
	IndexBuffer::IndexBuffer (const SubSystemsRef ss) :
		MemoryBuffer( ss ), _indexType( EIndex::type(-1) )
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

		_indexType = EIndex::type(-1);
	}
	
/*
=================================================
	SetIndexType
=================================================
*/
	void IndexBuffer::SetIndexType (EIndex::type indexType)
	{
		_indexType = indexType;
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
	New
=================================================
*/
	IndexBufferPtr  IndexBuffer::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new IndexBuffer( ss ) );
	}



//------------------------------ VertexBuffer -------------------------------//
/*
=================================================
	constructor
=================================================
*/
	VertexBuffer::VertexBuffer (const SubSystemsRef ss) :
		MemoryBuffer( ss ), _primitive( EPrimitive::type(-1) ),
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

		_primitive	= EPrimitive::type(-1);
		_divisor	= 0;
		_stride		= Bytes<ubyte>( 0 );
	}

/*
=================================================
	SetAttribs
=================================================
*/
	void VertexBuffer::SetAttribs (const VertexAttribsPtr &attribs, Bytes<uint> vertexStride)
	{
		_attribs	= attribs;
		_stride		= vertexStride.To<ubyte>();
	}
	
/*
=================================================
	SetAttribs
=================================================
*/
	void VertexBuffer::SetAttribs (const VertexAttribsDesc &attribDesc, Bytes<uint> vertexStride)
	{
		CHECK( SubSystems()->Get<GraphicsEngine>()->GetContext()
			->CreateVertexAttribs( attribDesc, OUT _attribs ) );
		
		_stride = vertexStride.To<ubyte>();
	}

/*
=================================================
	SetPrimitive
=================================================
*/
	void VertexBuffer::SetPrimitive (EPrimitive::type primitve)
	{
		_primitive = primitve;
	}
	
/*
=================================================
	SetDivisor
=================================================
*/
	void VertexBuffer::SetDivisor (uint divisor)
	{
		_divisor = (ushort) divisor;
	}

/*
=================================================
	Draw
=================================================
*/
	void VertexBuffer::Draw ()
	{
		return Draw( Count() );
	}

	void VertexBuffer::Draw (uint count, uint offset)
	{
		CHECK_ERR( GetBufferID().IsValid(), void() );
		CHECK( count + offset <= Count() );

		Ptr<StateManager>	sm = SubSystems()->Get<GraphicsEngine>()->GetStateManager();
		
		Barrier( EMemoryBarrier::VertexAttribArray );

		sm->BindAttribs( _attribs, _id, 0, _stride, _divisor );
		sm->BindAttribs( _attribs, BufferID() );
		sm->DrawArrays( _primitive, count, offset );
	}
	
/*
=================================================
	DrawIndexed
=================================================
*/
	void VertexBuffer::DrawIndexed (const IndexBufferPtr &indexBuffer)
	{
		return DrawIndexed( indexBuffer, indexBuffer->Count(), Bytes<usize>() );
	}

	void VertexBuffer::DrawIndexed (const IndexBufferPtr &indexBuffer, uint count, Bytes<usize> offset)
	{
		CHECK_ERR( GetBufferID().IsValid(), void() );
		CHECK_ERR( indexBuffer.IsNotNull() and indexBuffer->GetBufferID().IsValid(), void() );
		CHECK_ERR( count * EIndex::SizeOf( indexBuffer->IndexType() ) + offset <= indexBuffer->Size(), void() );

		Ptr<StateManager>	sm = SubSystems()->Get<GraphicsEngine>()->GetStateManager();
		
		Barrier( EMemoryBarrier::VertexAttribArray );
		indexBuffer->Barrier( EMemoryBarrier::ElementArray );

		sm->BindAttribs( _attribs, GetBufferID(), 0, _stride, _divisor );
		sm->BindAttribs( _attribs, indexBuffer->GetBufferID() );
		sm->DrawElements( _primitive, indexBuffer->IndexType(), count, offset );
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
	New
=================================================
*/
	VertexBufferPtr  VertexBuffer::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new VertexBuffer( ss ) );
	}


}	// Graphics
}	// Engine
