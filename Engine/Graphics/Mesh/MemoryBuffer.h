// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

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
	
	class ShaderStorageBuffer;
	SHARED_POINTER( ShaderStorageBuffer );



	//
	// Memory Buffer
	//

	class MemoryBuffer : public Resource
	{
	// types
	public:
		typedef Bitfield< EBufferTarget::_COUNT >	BufferTargetBits_t;


	// variables
	protected:
		BufferID				_id;
		Bytes< usize >			_size;
		EBufferUsage::type		_usage;
		MemoryBarrierObj		_pendingBarrier;


	// methods
	protected:
		explicit
		MemoryBuffer (const SubSystemsRef ss);
		~MemoryBuffer ();

	public:
		bool Create ();
		virtual void Destroy ();

		bool Allocate (Bytes<usize> size, EBufferUsage::type usage);

		template <typename T> bool SetData (Buffer<const T> data, EBufferUsage::type usage = EBufferUsage::Static);
		template <typename T> bool SetData (Buffer<T> data, EBufferUsage::type usage = EBufferUsage::Static);

		template <typename T> bool SubData (Buffer<const T> data, Bytes<usize> offset = Uninitialized());
		template <typename T> bool SubData (Buffer<T> data, Bytes<usize> offset = Uninitialized());

		template <typename T> bool GetData (OUT Buffer<T> &data, Bytes<usize> offset = Uninitialized());

		template <typename T> bool ClearSubData (const T &value, Bytes<usize> offset, Bytes<usize> size);
		template <typename T> bool Clear (const T &value);

		bool ClearSubData (Bytes<usize> offset, Bytes<usize> size);
		bool Clear ();
		
		void BindBase (EBufferTarget::type target, uint bindingIndex, EMemoryAccess::type access = EMemoryAccess::Read);

		void BindBase (EBufferTarget::type target, uint bindingIndex, EMemoryAccess::type access,
					   Bytes<usize> offset, Bytes<usize> size);
		
		bool Copy (const MemoryBufferPtr &src, Bytes<usize> srcOffset, Bytes<usize> dstOffset, Bytes<usize> size);

		void Barrier (EMemoryBarrier::type flags = EMemoryBarrier::type(0));

		BufferID const &	GetBufferID ()	const	{ return _id; }
		Bytes<usize>		Size ()			const	{ return _size; }
		EBufferUsage::type	Usage ()		const	{ return _usage; }

		static MemoryBufferPtr  New (const SubSystemsRef ss);


	// Resource
	public:
		bool SwapData (const ResourcePtr &other) override;

		bool IsValid () const override;

		
	private:
		bool _SetData (Buffer<const ubyte> data, EBufferUsage::type usage);
		bool _SubData (Buffer<const ubyte> data, Bytes<usize> offset);
		bool _GetData (OUT Buffer<ubyte> &data, Bytes<usize> offset);
		void _BarrierNow (EMemoryBarrier::type flags = EMemoryBarrier::type(0));


	// api specific methods
	private:
		static bool _BufferData (const BufferID &id, const void *data, usize dataSize, EBufferUsage::type usage);
		static bool _BufferSubData (const BufferID &id, const void *data, usize dataSize, usize offset);
		static bool _GetBufferSubData (const BufferID &id, void *data, usize dataSize, usize offset);
		static bool _ClearSubData (const BufferID &id, usize offset, usize size, EPixelFormat::type fmt, BinaryBuffer pattern);
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
	private:
		explicit
		IndexBuffer (const SubSystemsRef ss);

	public:
		void Destroy () override;

		void SetIndexType (EIndex::type indexType);
		
		uint			Count ()		const	{ return SafeDiv<uint>( _size, EIndex::SizeOf( _indexType ), 0 ); }
		EIndex::type	IndexType ()	const	{ return _indexType; }

		static IndexBufferPtr  New (const SubSystemsRef ss);

	// Resource
	public:
		bool SwapData (const ResourcePtr &other) override;
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
		ushort				_divisor;
		Bytes<ubyte>		_stride;


	// methods
	private:
		explicit
		VertexBuffer (const SubSystemsRef ss);

	public:
		void Destroy () override;

		void SetAttribs (const VertexAttribsPtr &attribs, Bytes<uint> vertexStride);
		void SetAttribs (const VertexAttribsDesc &attribDesc, Bytes<uint> vertexStride);

		void SetPrimitive (EPrimitive::type primitve);
		void SetDivisor (uint divisor);

		void Draw ();
		void Draw (uint count, uint offset = 0);

		void DrawIndexed (const IndexBufferPtr &indexBuffer);
		void DrawIndexed (const IndexBufferPtr &indexBuffer, uint count, Bytes<usize> offset = Bytes<usize>());

		VertexAttribsPtr const&	Attribs ()		const	{ return _attribs; }
		EPrimitive::type		Primitive ()	const	{ return _primitive; }
		Bytes<usize>			Stride ()		const	{ return Bytes<usize>( _stride ); }
		uint					Count ()		const	{ return SafeDiv<uint>( _size, _stride, 0 ); }

		static VertexBufferPtr  New (const SubSystemsRef ss);

	// Resource
	public:
		bool SwapData (const ResourcePtr &other) override;
	};


	
/*
=================================================
	SetData
=================================================
*/
	template <typename T>
	inline bool MemoryBuffer::SetData (Buffer<const T> data, EBufferUsage::type usage)
	{
		return _SetData( Buffer<const ubyte>::From( data ), usage );
	}

	template <typename T>
	inline bool MemoryBuffer::SetData (Buffer<T> data, EBufferUsage::type usage)
	{
		return _SetData( Buffer<const ubyte>::From( data ), usage );
	}

/*
=================================================
	SubData
=================================================
*/
	template <typename T>
	inline bool MemoryBuffer::SubData (Buffer<const T> data, Bytes<usize> offset)
	{
		return _SubData( Buffer<const ubyte>::From( data ), offset );
	}

	template <typename T>
	inline bool MemoryBuffer::SubData (Buffer<T> data, Bytes<usize> offset)
	{
		return _SubData( Buffer<const ubyte>::From( data ), offset );
	}

/*
=================================================
	GetData
=================================================
*/
	template <typename T>
	inline bool MemoryBuffer::GetData (OUT Buffer<T> &data, Bytes<usize> offset)
	{
		Buffer<ubyte> tmp_data = Buffer<ubyte>::From( data );
		return _GetData( tmp_data, offset );
	}
	
/*
=================================================
	ClearSubData
=================================================
*/
	template <typename T>
	inline bool MemoryBuffer::ClearSubData (const T &value, Bytes<usize> offset, Bytes<usize> size)
	{
		CHECK_ERR( IsValid() );
		CHECK_ERR( offset + size <= Size() );

		return _ClearSubData( GetBufferID(), offset, size, EPixelFormat::FromType( value ), BinaryBuffer::From( value ) );
	}

	inline bool MemoryBuffer::ClearSubData (Bytes<usize> offset, Bytes<usize> size)
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

		return _ClearSubData( GetBufferID(), 0, Size(), EPixelFormat::FromType( value ), BinaryBuffer::From( value ) );
	}
	
	inline bool MemoryBuffer::Clear ()
	{
		return Clear( ubyte(0) );
	}


}	// Graphics
}	// Engine
