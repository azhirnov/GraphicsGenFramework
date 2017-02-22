// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Graphics/VertexAttribs/VertexAttribs.h"

namespace Engine
{
namespace Graphics
{

	class MemoryBuffer;
	SHARED_POINTER( MemoryBuffer );
	
	class IndexBuffer;
	SHARED_POINTER( IndexBuffer );

	class VertexBuffer;
	SHARED_POINTER( VertexBuffer );



	//
	// Memory Buffer
	//

	class MemoryBuffer : public Resource
	{
	// types
	public:
		typedef Bitfield< EBufferTarget::_Count >	BufferTargetBits_t;


	// variables
	protected:
		BufferID				_id;
		BytesU					_size;
		EBufferUsage::type		_usage;
		MemoryBarrierObj		_pendingBarrier;


	// methods
	public:
		explicit
		MemoryBuffer (const SubSystemsRef ss);
		~MemoryBuffer ();

		bool Create ();
		virtual void Destroy ();

		bool Allocate (BytesU size, EBufferUsage::type usage);

		template <typename T> bool SetData (Buffer<const T> data, EBufferUsage::type usage = EBufferUsage::Static);
		template <typename T> bool SetData (Buffer<T> data, EBufferUsage::type usage = EBufferUsage::Static);

		template <typename T> bool SubData (Buffer<const T> data, BytesU offset = Uninitialized);
		template <typename T> bool SubData (Buffer<T> data, BytesU offset = Uninitialized);

		template <typename T> bool GetData (OUT Buffer<T> data, BytesU offset = Uninitialized);

		template <typename T> bool ClearSubData (const T &value, BytesU offset, BytesU size);
		template <typename T> bool Clear (const T &value);

		bool ClearSubData (BytesU offset, BytesU size);
		bool Clear ();
		
		bool BindBase (EBufferTarget::type target, uint bindingIndex, EMemoryAccess::type access = EMemoryAccess::Read);
		bool BindBase (EBufferTarget::type target, uint bindingIndex, EMemoryAccess::type access, BytesU offset, BytesU size);
		
		bool Copy (const MemoryBufferPtr &src, BytesU srcOffset, BytesU dstOffset, BytesU size);

		void Barrier (EMemoryBarrier::type flags = EMemoryBarrier::Unknown);

		BufferID const &	GetBufferID ()	const	{ return _id; }
		BytesU				Size ()			const	{ return _size; }
		EBufferUsage::type	Usage ()		const	{ return _usage; }


	// Resource
	public:
		bool SwapData (const ResourcePtr &other) override;

		bool IsValid () const override;

		
	private:
		bool _SetData (BinaryCBuffer data, EBufferUsage::type usage);
		bool _SubData (BinaryCBuffer data, BytesU offset);
		bool _GetData (OUT BinaryBuffer data, BytesU offset);
		void _BarrierNow (EMemoryBarrier::type flags = EMemoryBarrier::Unknown);


	// api specific methods
	private:
		static bool _BufferData (const BufferID &id, const void *data, usize dataSize, EBufferUsage::type usage);
		static bool _BufferSubData (const BufferID &id, const void *data, usize dataSize, usize offset);
		static bool _GetBufferSubData (const BufferID &id, void *data, usize dataSize, usize offset);
		static bool _ClearSubData (const BufferID &id, usize offset, usize size, EPixelFormat::type fmt, BinaryCBuffer pattern);
		static bool _CopyBufferSubData (const BufferID &src, const BufferID &dst, usize readOffset, usize writeOffset, usize size);
	};



	//
	// Index Buffer
	//

	class IndexBuffer : public MemoryBuffer
	{
	// variables
	private:
		EIndex::type	_indexType;


	// methods
	public:
		explicit
		IndexBuffer (const SubSystemsRef ss);

		void Destroy () override;

		bool SetIndexType (EIndex::type indexType);
		
		usize			Count ()		const	{ return (usize) SafeDiv( _size, EIndex::SizeOf( _indexType ), BytesU() ); }
		EIndex::type	IndexType ()	const	{ return _indexType; }


	// Resource
	public:
		bool SwapData (const ResourcePtr &other) override;
		bool IsValid () const override;
	};



	//
	// Vertex Buffer
	//

	class VertexBuffer : public MemoryBuffer
	{
	// variables
	private:
		VertexAttribsPtr	_attribs;
		EPrimitive::type	_primitive;
		uint				_divisor;
		BytesU				_stride;


	// methods
	public:
		explicit
		VertexBuffer (const SubSystemsRef ss);

		void Destroy () override;

		bool SetAttribs (const VertexAttribsPtr &attribs, BytesU vertexStride);
		bool SetAttribs (const VertexAttribsDesc &attribDesc, BytesU vertexStride);

		bool SetPrimitive (EPrimitive::type primitve);
		bool SetDivisor (uint divisor);

		bool Draw ();
		bool Draw (usize count, usize offset = 0);

		bool DrawIndexed (const IndexBufferPtr &indexBuffer);
		bool DrawIndexed (const IndexBufferPtr &indexBuffer, usize count, BytesU offset = BytesU());

		VertexAttribsPtr const&	Attribs ()		const	{ return _attribs; }
		EPrimitive::type		Primitive ()	const	{ return _primitive; }
		BytesU					Stride ()		const	{ return _stride; }
		usize					Count ()		const	{ return (usize) SafeDiv( _size, _stride, BytesU() ); }


	// Resource
	public:
		bool SwapData (const ResourcePtr &other) override;
		bool IsValid () const override;
	};


	
/*
=================================================
	SetData
=================================================
*/
	template <typename T>
	inline bool MemoryBuffer::SetData (Buffer<const T> data, EBufferUsage::type usage)
	{
		return _SetData( BinaryCBuffer::From( data ), usage );
	}

	template <typename T>
	inline bool MemoryBuffer::SetData (Buffer<T> data, EBufferUsage::type usage)
	{
		return _SetData( BinaryCBuffer::From( data ), usage );
	}

/*
=================================================
	SubData
=================================================
*/
	template <typename T>
	inline bool MemoryBuffer::SubData (Buffer<const T> data, BytesU offset)
	{
		return _SubData( BinaryCBuffer::From( data ), offset );
	}

	template <typename T>
	inline bool MemoryBuffer::SubData (Buffer<T> data, BytesU offset)
	{
		return _SubData( BinaryCBuffer::From( data ), offset );
	}

/*
=================================================
	GetData
=================================================
*/
	template <typename T>
	inline bool MemoryBuffer::GetData (OUT Buffer<T> data, BytesU offset)
	{
		BinaryBuffer	tmp_data = BinaryBuffer::From( data );
		return _GetData( tmp_data, offset );
	}
	
/*
=================================================
	ClearSubData
=================================================
*/
	template <typename T>
	inline bool MemoryBuffer::ClearSubData (const T &value, BytesU offset, BytesU size)
	{
		CHECK_ERR( IsValid() );
		CHECK_ERR( offset + size <= Size() );

		return _ClearSubData( GetBufferID(), (usize)offset, (usize)size, EPixelFormat::FromType( value ), BinaryCBuffer::FromValue( value ) );
	}

	inline bool MemoryBuffer::ClearSubData (BytesU offset, BytesU size)
	{
		return ClearSubData( ubyte(0), offset, size );
	}
	
/*
=================================================
	Clear
=================================================
*/
	template <typename T>
	inline bool MemoryBuffer::Clear (const T &value)
	{
		CHECK_ERR( IsValid() );

		return _ClearSubData( GetBufferID(), 0, (usize)Size(), EPixelFormat::FromType( value ), BinaryCBuffer::FromValue( value ) );
	}
	
	inline bool MemoryBuffer::Clear ()
	{
		return Clear( ubyte(0) );
	}


}	// Graphics
}	// Engine
