// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "gl4.h"

#if defined( GRAPHICS_API_OPENGL )

#include "GL4StateManager.h"

namespace Engine
{
namespace Graphics
{

	//
	// Context (Object Manager)
	//

	class GL4Context
	{
	// types
	private:
		typedef Array< VertexAttribsPtr >			vertex_attribs_t;
		typedef Map< SamplerState, SamplerPtr >		samplers_t;			// TODO: HashMap

		static const uint	MIN_SUPPORTED_GL_VERSION = GRAPHICS_API_OPENGL;


	// variables
	private:
		// samplers
		samplers_t				_samplerCache;

		// vertex attribs
		vertex_attribs_t		_attribsCache;

		// other
		Ptr< GL4StateManager >	_stateManager;

		EVideoCardVendor::type	_vendor;

		uint					_numExtensions;


	// methods
	public:
		GL4Context (GL4StateManager *sm);
		~GL4Context ();
		
		bool Initialize ();
		void Deinitialize ();

		EVideoCardVendor::type	GetVideoCardVendor () const			{ return _vendor; }

		bool IsExtensionSupported (StringCRef extName);


		// textures
		bool CreateTexture (ETexture::type target, OUT TextureID &tex);
		void DeleteTexture (INOUT TextureID &tex);
		

		// samplers
		bool CreateSampler (const SamplerState &state, OUT SamplerPtr &sampler);
		

		// programs
		bool CreateProgram (OUT ProgramID &prog);
		void DeleteProgram (INOUT ProgramID &prog);

		bool CreateUniform (const ProgramID &prog, StringCRef name, OUT UniformLocationID &uniform);
		void DeleteUniform (INOUT UniformLocationID &uniform);

		bool CreateShader (OUT ShaderID &shader, EShader::type type);
		void DeleteShader (INOUT ShaderID &shader);
		

		// vertex attribs
		bool CreateVertexAttribs (const VertexAttribsDesc &desc, OUT VertexAttribsPtr &attribs);
		void DeleteVertexAttribs (INOUT VertexAttribsPtr &attribs);


		// buffers
		bool CreateBuffer (OUT BufferID &buf);
		void DeleteBuffer (INOUT BufferID &buf);


		// render targets
		bool CreateRenderTarget (OUT RenderTargetID &rt);
		bool CreateSystemRenderTarget (OUT RenderTargetID &rt);
		void DeleteRenderTarget (INOUT RenderTargetID &rt);


		// event
		bool FenceSync (OUT SyncEventID &sync);
		void DeleteSync (INOUT SyncEventID &sync);


		// query
		bool CreateQuery (OUT QueryID &query);
		void DeleteQuery (INOUT QueryID &query);


	private:
		EVideoCardVendor::type	_GetVideoCardVendor () const;

		void _DebugCallback (gl::GLenum source, gl::GLenum type, gl::GLuint id,  gl::GLenum severity,
							 gl::GLsizei length, const gl::GLchar* message) const;

		static void GL4_APIENTRY _StaticDebugCallback (gl::GLenum source, gl::GLenum type, gl::GLuint id,
													   gl::GLenum severity, gl::GLsizei length,
													   const gl::GLchar* message, const void* userParam);
	};


	
}	// Graphics
}	// Engine

#endif	// GRAPHICS_API_OPENGL
