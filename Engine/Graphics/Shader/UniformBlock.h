// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "ShaderProgram.h"
#include "Engine/Graphics/Buffer/MemoryBuffer.h"
#include "Engine/Graphics/Engine/GraphicsEngine.h"

namespace Engine
{
namespace Graphics
{

	//
	// Uniform Block
	//

	// TODO: refactoring required
	template <typename T>
	struct UniformBlock
	{
	// types
	private:
		static const uint	ALIGN		= sizeof(float) * 4;
		static const uint	BUFFER_SIZE = CompileTime::AlignToLarge< uint, sizeof(T), ALIGN >;

		typedef UniformBlock<T>						Self;
		typedef T									Struct_t;
		typedef StaticArray< ubyte, BUFFER_SIZE >	Buffer_t;
		typedef StaticArray< MemoryBufferPtr, /*3*/1 >	GraphicsBuffers_t;


	// variables
	private:
		ShaderProgramPtr	_program;
		Buffer_t			_bufferData;
		GraphicsBuffers_t	_graphicsBuffers;	// triple buffer (TODO)
		uint				_location;
		uint				_bindingIndex;
		uint				_currentGraphicsBuffer;
		bool				_changed;


	// methods
	public:
		UniformBlock ();
		~UniformBlock ();

		T &			Get ();
		T const &	Get () const;

		void Write (BinaryBuffer data, BytesU offset);

		bool _Create (const ShaderProgramPtr &program, StringCRef name, uint index);
		void Destroy ();

		bool IsCreated () const;

		void Update ();
	};
	
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	inline UniformBlock<T>::UniformBlock () :
		_currentGraphicsBuffer(0), _location(-1), _bindingIndex(-1), _changed(false)
	{
		//ReferenceCast< T >( _bufferData ) = T();
		(*(T *)_bufferData.ptr()) = T();
	}

/*
=================================================
	destructor
=================================================
*/
	template <typename T>
	inline UniformBlock<T>::~UniformBlock ()
	{
		Destroy();
	}
	
/*
=================================================
	Get
=================================================
*/
	template <typename T>
	inline T &  UniformBlock<T>::Get ()
	{
		ASSERT( IsCreated() );
		_changed = true;
		return *(T *) _bufferData.ptr();
	}
	
	template <typename T>
	inline T const &  UniformBlock<T>::Get () const
	{
		ASSERT( IsCreated() );
		return *(T const *)  _bufferData.ptr();
	}
	
/*
=================================================
	Write
=================================================
*/
	template <typename T>
	inline void UniformBlock<T>::Write (BinaryBuffer data, BytesU offset)
	{
		CHECK( data.Count() + offset <= _bufferData.Count() );

		UnsafeMem::MemCopy( _bufferData.SubArray( offset, data.Count() ), data );
	}

/*
=================================================
	IsCreated
=================================================
*/
	template <typename T>
	bool UniformBlock<T>::IsCreated () const
	{
		return _graphicsBuffers.Front() and _location != -1;
	}

/*
=================================================
	Create
=================================================
*/
	template <typename T>
	inline bool UniformBlock<T>::_Create (const ShaderProgramPtr &program, StringCRef name, uint index)
	{
		CHECK_ERR( program );

		_program = program;

		FOR( i, _graphicsBuffers )
		{
			_graphicsBuffers[i] = New<MemoryBuffer>( program->SubSystems() );

			CHECK_ERR( _graphicsBuffers[i]->Create() );
		}

		uint	size = 0;

		program->_CreateUniformBuffer( OUT _location, OUT size, name );
		_bindingIndex = index;

		CHECK( AlignToLarge( size, BytesU(ALIGN) ) == _bufferData.Size() );
		return true;
	}
	
/*
=================================================
	Destroy
=================================================
*/
	template <typename T>
	inline void UniformBlock<T>::Destroy ()
	{
		_graphicsBuffers.Clear();

		//_currentGraphicsBuffer	= 0;
		_location				= -1;
		_bindingIndex			= -1;
		_changed				= false;
	}

/*
=================================================
	Update
=================================================
*/
	template <typename T>
	inline void UniformBlock<T>::Update ()
	{
		/*if ( ++_currentGraphicsBuffer >= _graphicsBuffers.Count() )
		{
			_currentGraphicsBuffer = 0;
		}
		*/

		MemoryBufferPtr const&	buf = _graphicsBuffers[ _currentGraphicsBuffer ];

		if ( _changed )
		{
			buf->SetData( BinaryBuffer( _bufferData ), EBufferUsage::Dynamic );
			_changed = false;
		}

		buf->BindBase( EBufferTarget::Uniform, _bindingIndex );
	}

/*
=================================================
	CreateUniformBlock
=================================================
*/
	template <typename T>
	inline bool ShaderProgram::CreateUniformBlock (OUT UniformBlock<T> &ub, StringCRef name)
	{
		_onBindEvent.Add( &ub, &UniformBlock<T>::Update );
		return ub._Create( this, name, _numUniformBlocks++ );
	}


}	// Graphics
}	// Engine
