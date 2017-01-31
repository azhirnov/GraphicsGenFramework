// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "gl4.h"

#if defined( GRAPHICS_API_OPENGL )

#include "Engine/Graphics/Types/Enums.h"
#include "Engine/Graphics/VertexAttribs/VertexAttribsDesc.h"
#include "Engine/Graphics/RenderTarget/FragmentOutputState.h"

namespace Engine
{
namespace Graphics
{
	struct  GL4SamplerID;
	typedef GL4SamplerID			SamplerID;
	
	struct  GL4TextureID;
	typedef GL4TextureID			TextureID;

	struct  GL4VertexAttribsID;
	typedef GL4VertexAttribsID		VertexAttribsID;
	
	struct  GL4BufferID;
	typedef GL4BufferID				BufferID;

	struct  GL4ProgramID;
	typedef GL4ProgramID			ProgramID;

	struct  GL4ShaderID;
	typedef GL4ShaderID				ShaderID;

	struct  GL4RenderTargetID;
	typedef GL4RenderTargetID		RenderTargetID;

	struct  GL4UniformLocationID;
	typedef GL4UniformLocationID	UniformLocationID;

	struct  GL4SyncEventID;
	typedef GL4SyncEventID			SyncEventID;

	struct  GL4QueryID;
	typedef GL4QueryID				QueryID;



	//
	// Sampler ID
	//

	struct GL4SamplerID : public CompileTime::PODStruct
	{
		friend class GL4Context;
		friend class GL4StateManager;

	// variables
	private:
		mutable gl::GLuint		_id;

	// methods
	public:
		explicit GL4SamplerID (gl::GLuint id = 0) : _id(id) {}
		
		bool IsValid ()	const									{ return _id != 0; }

		bool operator == (const GL4SamplerID &right) const		{ return this->_id == right._id; }
	};
	


	//
	// Texture ID
	//

	struct GL4TextureID : public CompileTime::PODStruct
	{
		friend class GL4Context;
		friend class GL4StateManager;

	// variables
	private:
		gl::GLuint				_id;
		gl::GLenum				_target;
			
	// methods
	public:
		GL4TextureID () : _id(0), _target(0) {}
		
		gl::GLuint	Id ()		const	{ return _id; }
		gl::GLenum	Target ()	const	{ return _target; }
		bool		IsValid ()	const	{ return _id != 0; }

		bool operator == (const GL4TextureID &right) const		{ return this->_id == right._id and this->_target == right._target; }
		bool operator != (const GL4TextureID &right) const		{ return not (*this == right); }
	};



	//
	// Vertex Attribs ID
	//

	struct GL4VertexAttribsID : public CompileTime::PODStruct
	{
		friend class GL4Context;
		friend class GL4StateManager;

	// types
	private:
		struct _VertBuf
		{
			gl::GLuint	id		= 0;
			uint		offset	= 0;
			uint		stride	= 0;
			uint		divisor	= 0;
		};

		typedef StaticArray< _VertBuf, 1 >	_VertBufs_t;

	// variables
	private:
		mutable gl::GLuint		_id;
		mutable gl::GLuint		_indexBuf;
		mutable _VertBufs_t		_vb;

	// methods
	public:
		GL4VertexAttribsID () : _id(0), _indexBuf(0) {}
		explicit GL4VertexAttribsID (gl::GLuint id) : _id(id), _indexBuf(0) {}

		gl::GLuint	Id ()		const	{ return _id; }
		bool		IsValid ()	const	{ return _id != 0; }

		bool operator == (const GL4VertexAttribsID &right) const		{ return this->_id == right._id; }
	};



	//
	// Buffer ID
	//

	struct GL4BufferID : public CompileTime::PODStruct
	{
		friend class GL4Context;
		friend class GL4StateManager;

	// variables
	private:
		mutable gl::GLuint		_id;

	// methods
	public:
		explicit GL4BufferID (gl::GLuint id = 0) : _id(id) {}

		gl::GLuint	Id ()		const	{ return _id; }
		bool		IsValid ()	const	{ return _id != 0; }

		bool operator == (const GL4BufferID &right) const		{ return this->_id == right._id; }
		bool operator != (const GL4BufferID &right) const		{ return not (*this == right); }
		bool operator >  (const GL4BufferID &right) const		{ return this->_id > right._id; }
		bool operator <  (const GL4BufferID &right) const		{ return this->_id < right._id; }
	};



	//
	// Program ID
	//

	struct GL4ProgramID : public CompileTime::PODStruct
	{
		friend class GL4Context;
		friend class GL4StateManager;

	// variables
	private:
		gl::GLuint				_id;
		VertexAttribsState		_input;
		FragmentOutputState		_output;
		uint					_patchSize;


	// methods
	public:
		explicit GL4ProgramID (gl::GLuint id = 0) : _id(id), _patchSize(0) {}
	
		GL4ProgramID (gl::GLuint id, uint patchSize,
					  const VertexAttribsState &input,
					  const FragmentOutputState &output) :
			_id(id), _patchSize(patchSize), _input(input), _output(output)
		{}

		gl::GLuint					Id ()					const	{ return _id; }
		bool						IsValid ()				const	{ return _id != 0; }
		uint						GetTessellationPatch ()	const	{ return _patchSize; }
		VertexAttribsState const&	GetVertexAttribs ()		const	{ return _input; }
		FragmentOutputState const&	GetFragmentOutput ()	const	{ return _output; }

		bool operator == (const GL4ProgramID &right)		const	{ return this->_id == right._id; }
		bool operator != (const GL4ProgramID &right)		const	{ return not (*this == right); }
		bool operator >  (const GL4ProgramID &right)		const	{ return this->_id > right._id; }
		bool operator <  (const GL4ProgramID &right)		const	{ return this->_id < right._id; }
	};



	//
	// Shader ID
	//

	struct GL4ShaderID : public CompileTime::PODStruct
	{
		friend class GL4Context;
		friend class GL4StateManager;


	// variables
	private:
		gl::GLuint			_id;
		EShader::type		_type;


	// methods
	public:
		GL4ShaderID () : _id(0), _type( EShader::Unknown ) {}
		GL4ShaderID (gl::GLuint id, EShader::type type) : _id(id), _type(type) {}

		gl::GLuint		Id ()		const	{ return _id; }
		bool			IsValid ()	const	{ return _id != 0; }
		EShader::type	GetType ()	const	{ return _type; }
	};



	//
	// Render Target ID
	//

	struct GL4RenderTargetID : public CompileTime::PODStruct
	{
		friend class GL4Context;
		friend class GL4StateManager;

	// variables
	private:
		gl::GLuint		_id;

	// methods
	public:
		explicit GL4RenderTargetID (gl::GLuint id = 0) : _id(id) {}

		gl::GLuint	Id ()		const	{ return _id; }
		bool		IsValid ()	const	{ return _id != 0; }

		bool operator == (const GL4RenderTargetID &right) const		{ return this->_id == right._id; }
	};



	//
	// Uniform Location ID
	//

	struct GL4UniformLocationID : public CompileTime::PODStruct
	{
		friend class GL4Context;

	// variables
	private:
		gl::GLuint		_progID;
		gl::GLint		_location;

	// methods
	public:
		explicit
		GL4UniformLocationID (gl::GLuint progID = 0, gl::GLint location = -1) :
			_progID(progID), _location(location)
		{}

		gl::GLuint	Program ()		const	{ return _progID; }
		gl::GLint	Location ()		const	{ return _location; }
		bool		IsValid ()		const	{ return _location != -1; }

		bool operator == (const GL4UniformLocationID &right) const
		{
			return this->_progID == right._progID and this->_location == right._location;
		}
	};



	//
	// Sync Event ID
	//

	struct GL4SyncEventID : public CompileTime::PODStruct
	{
		friend class GL4Context;

	// variables
	private:
		gl::GLsync		_id;

	// methods
	public:
		explicit GL4SyncEventID (gl::GLsync id = 0) : _id( id ) {}

		gl::GLsync	Id ()		const	{ return _id; }
		bool		IsValid ()	const	{ return _id != 0; }

		bool operator == (const GL4SyncEventID &right) const	{ return this->_id == right._id; }
	};



	//
	// Query ID
	//

	struct GL4QueryID : public CompileTime::PODStruct
	{
		friend class GL4Context;

	// variables
	private:
		gl::GLuint		_id;

	// methods
	public:
		explicit GL4QueryID (gl::GLuint id = 0) : _id( id ) {}

		gl::GLuint	Id ()		const	{ return _id; }
		bool		IsValid ()	const	{ return _id != 0; }

		bool operator == (const GL4QueryID &right)	const	{ return this->_id == right._id; }
	};


}	// Graphics
}	// Engine

#endif	// GRAPHICS_API_OPENGL
