// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "GL4Context.h"

#if defined( GRAPHICS_API_OPENGL )

#include "Engine/Graphics/Texture/Sampler.h"
#include "Engine/Graphics/VertexAttribs/VertexAttribs.h"

namespace Engine
{
namespace Graphics
{
	using namespace gl;
	
/*
=================================================
	constructor
=================================================
*/
	GL4Context::GL4Context (GL4StateManager *sm) :
		_stateManager( sm ),
		_numExtensions( 0 )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	GL4Context::~GL4Context ()
	{
	}
	
/*
=================================================
	Initialize
=================================================
*/
	bool GL4Context::Initialize ()
	{
		CHECK( GL4_Init() );

		GL4_WriteInfo();

		CHECK_ERR( GL4_GetVersion() >= MIN_SUPPORTED_GL_VERSION );

		_vendor = _GetVideoCardVendor();

		GL_CALL( glGetIntegerv( GL_NUM_EXTENSIONS, (GLint *) &_numExtensions ) );

		DEBUG_ONLY(
			GL_CALL( glDebugMessageCallback( &_StaticDebugCallback, this ) );

			// disable notifications
			GL_CALL( glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION,
											0, null, GL_FALSE ) );
		)

		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void GL4Context::Deinitialize ()
	{
		DEBUG_ONLY( GL_CALL( glDebugMessageCallback( null, null ) ) );

		// delete samplers
		FOR( i, _samplerCache )
		{
			GLuint &	id = _samplerCache[i].second->GetSamplerID()._id;

			// not needed here
			//_stateManager->UnbindSampler( _samplerCache[i].second->GetSamplerID() );

			GL_CALL( glDeleteSamplers( 1, &id ) );
			id = 0;
		}
		_samplerCache.Clear();
		

		// delete vertex attribs
		FOR( i, _attribsCache )
		{
			GLuint &	id = _attribsCache[i]->GetID()._id;
			
			// not needed here
			//_stateManager->UnbindAttribs( _attribsCache[i]->GetID() );

			GL_CALL( glDeleteVertexArrays( 1, &id ) );
			id = 0;
		}
		_attribsCache.Clear();


		GL4_Delete();
	}
	
/*
=================================================
	GetVideoCardVendor
=================================================
*/
	EVideoCardVendor::type GL4Context::_GetVideoCardVendor () const
	{
		StringCRef	gpu_vendor;
		
		GL_CALL( gpu_vendor = (const char*) glGetString( GL_VENDOR ) );
		
		if ( gpu_vendor.HasSubString( "AMD" ) or gpu_vendor.HasSubString( "ATI" ) )
			return EVideoCardVendor::AMD;

		if ( gpu_vendor.HasSubStringIC( "NVIDIA" ) )
			return EVideoCardVendor::NVidia;

		if ( gpu_vendor.HasSubStringIC( "INTEL" ) )	
			return EVideoCardVendor::Intel;

		return EVideoCardVendor::Unknown;
	}
	
/*
=================================================
	IsExtensionSupported
=================================================
*/
	bool GL4Context::IsExtensionSupported (StringCRef extName)
	{
		const char *	ext = null;

		for (uint i = 0; i < _numExtensions; ++i)
		{
			GL_CALL( ext = (const char *) glGetStringi( GL_EXTENSIONS, i ) );

			if ( extName.EqualsIC( ext ) )
				return true;
		}
		return false;
	}
	
/*
=================================================
	CreateTexture
=================================================
*/
	bool GL4Context::CreateTexture (ETexture::type target, OUT TextureID &tex)
	{
		ASSERT( not tex.IsValid() );

		tex._target = GL4Enum( target );

		GL_CHECK( glGenTextures( 1, &tex._id ) );
		CHECK_ERR( tex.IsValid() );

		//_stateManager->ActiveTexture( tex );
		return true;
	}
	
/*
=================================================
	DeleteTexture
=================================================
*/
	void GL4Context::DeleteTexture (INOUT TextureID &tex)
	{
		if ( tex._id != 0 )
		{
			_stateManager->UnbindTexture( tex );
			_stateManager->UnbindImage( tex );

			GL_CALL( glDeleteTextures( 1, &tex._id ) );
		}
		tex = TextureID();
	}

/*
=================================================
	CreateSampler
=================================================
*/
	bool GL4Context::CreateSampler (const SamplerState &state, OUT SamplerPtr &sampler)
	{
		sampler = null;

		SamplerState	tmp = state;
		GLuint			id	= 0;

		// validate anisotropy level
		if ( EnumEq( tmp.filter, EFilter::_ANISOTROPIC ) )
		{
			tmp.filter = EFilter::type( (tmp.filter ^ EFilter::_A_FACTOR_MASK) |
										Clamp(	uint(tmp.filter & EFilter::_A_FACTOR_MASK),
												uint(_stateManager->SupportAnisotropyFiltering()) * 2,
												_stateManager->GetMaxAnisotropyLevel() ) );
		}

		// search in cache
		samplers_t::const_iterator	iter;

		if ( _samplerCache.Find( tmp, iter ) )
		{
			sampler = iter->second;
			return true;
		}

		// create sampler
		GL_CHECK( glGenSamplers( 1, &id ) );

		sampler = Sampler::New( GL4SamplerID( id ), tmp );
		
		// set sampler states

		// wrap
		GL_CALL( glSamplerParameteri( id, GL_TEXTURE_WRAP_S, GL4Enum( tmp.wrapMode.x ) ) );
		GL_CALL( glSamplerParameteri( id, GL_TEXTURE_WRAP_T, GL4Enum( tmp.wrapMode.y ) ) );
		GL_CALL( glSamplerParameteri( id, GL_TEXTURE_WRAP_R, GL4Enum( tmp.wrapMode.z ) ) );
		
		// filtering
		GL4MinFilter		min_filter;
		GL4MagFilter		mag_filter;
		GL4AnisotropyFilter aniso;

		CHECK_ERR( GL4Enum( tmp.filter, min_filter, mag_filter, aniso ) );

		GL_CALL( glSamplerParameteri( id, GL_TEXTURE_MIN_FILTER, min_filter ) );
		GL_CALL( glSamplerParameteri( id, GL_TEXTURE_MAG_FILTER, mag_filter ) );

		// anisotropy
		if ( aniso > 0 and _stateManager->SupportAnisotropyFiltering() )
		{
			GL_CALL( glSamplerParameteri( id, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso ) );
		}

		// compare func
		if ( tmp.compareRefToTexture != ECompareFunc::None )
		{
			GL_CALL( glSamplerParameteri( id, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE ) );
			GL_CALL( glSamplerParameteri( id, GL_TEXTURE_COMPARE_FUNC, GL4Enum( tmp.compareRefToTexture ) ) );
		}
		else
		{
			GL_CALL( glSamplerParameteri( id, GL_TEXTURE_COMPARE_MODE, GL_NONE ) );
		}

		_samplerCache.Add( tmp, sampler );
		return true;
	}
	
/*
=================================================
	CreateVertexAttribs
=================================================
*/
	bool GL4Context::CreateVertexAttribs (const VertexAttribsDesc &desc, OUT VertexAttribsPtr &attribs)
	{
		attribs = null;

		// search in cache
		FOR( i, _attribsCache )
		{
			if ( _attribsCache[i]->GetDescriptor() == desc )
			{
				attribs = _attribsCache[i];
				return true;
			}
		}

		// create new vertex attribs
		GLuint	id;

		GL_CHECK( glGenVertexArrays( 1, &id ) );

		attribs = VertexAttribs::New( VertexAttribsID( id ), desc );

		_stateManager->_BindAttribs( attribs );

		// set attribs
		FOR( i, desc )
		{
			if ( not desc[i].IsEnabled() )
			{
				GL_CALL( glDisableVertexArrayAttrib( id, GLuint(i) ) );
				continue;
			}

			const uint	index = desc[i].index;
			ASSERT( index == i );
			
			GL_CALL( glEnableVertexArrayAttrib( id, index ) );
			GL_CALL( glVertexArrayAttribBinding( id, index, desc[i].bufferIndex ) );

			GL4VertexAttribType	type;
			uint				size;
			bool				norm;

			CHECK_ERR( GL4Enum( desc[i].type, OUT type, OUT size, OUT norm ) );

			if ( EAttribute::ValueType::IsDouble( desc[i].ToVSInputType() ) )
			{
				GL_CALL( glVertexArrayAttribLFormat( id, index, size, type, GLsizei(desc[i].offset) ) );
			}
			else
			if ( EAttribute::ValueType::IsInteger( desc[i].ToVSInputType() ) )
			{
				GL_CALL( glVertexArrayAttribIFormat( id, index, size, type, GLsizei(desc[i].offset) ) );
			}
			else
			{
				CHECK( EAttribute::ValueType::IsFloat( desc[i].ToVSInputType() ) );
				GL_CALL( glVertexArrayAttribFormat( id, index, size, type, norm, GLsizei(desc[i].offset) ) );
			}
		}

		_attribsCache.PushBack( attribs );
		return true;
	}
	
/*
=================================================
	DeleteVertexAttribs
=================================================
*/
	void GL4Context::DeleteVertexAttribs (INOUT VertexAttribsPtr &attribs)
	{
		attribs = null;

		// _stateManager->UnbindAttribs();
	}
	
/*
=================================================
	CreateProgram
=================================================
*/
	bool GL4Context::CreateProgram (OUT ProgramID &prog)
	{
		ASSERT( not prog.IsValid() );

		GL_CHECK( prog._id = glCreateProgram() );
		CHECK_ERR( prog.IsValid() );

		return true;
	}

/*
=================================================
	DeleteProgram
=================================================
*/
	void GL4Context::DeleteProgram (INOUT ProgramID &prog)
	{
		if ( prog._id != 0 )
		{
			if ( _stateManager->IsProgramBinded( prog ) )
				_stateManager->BindProgram( ProgramID() );

			GL_CALL( glDeleteProgram( prog._id ) );
		}
		prog = ProgramID();
	}
	
/*
=================================================
	CreateUniform
=================================================
*/
	bool GL4Context::CreateUniform (const ProgramID &prog, StringCRef name, OUT UniformLocationID &uniform)
	{
		ASSERT( not uniform.IsValid() );

		GLint	loc = -1;
		GL_CHECK( loc = glGetUniformLocation( prog.Id(), name.cstr() ) );
		CHECK_ERR( loc >= 0 );
		
		uniform = UniformLocationID( prog.Id(), loc );
		return true;
	}
	
/*
=================================================
	DeleteUniform
=================================================
*/
	void GL4Context::DeleteUniform (INOUT UniformLocationID &uniform)
	{
		uniform = UniformLocationID();
	}
	
/*
=================================================
	CreateShader
=================================================
*/
	bool GL4Context::CreateShader (OUT ShaderID &shader, EShader::type type)
	{
		ASSERT( not shader.IsValid() );

		GLuint	id = 0;
		GL_CHECK( id = glCreateShader( GL4Enum( type ) ) );
		CHECK_ERR( id > 0 );

		shader = ShaderID( id, type );
		return true;
	}
	
/*
=================================================
	DeleteShader
=================================================
*/
	void GL4Context::DeleteShader (INOUT ShaderID &shader)
	{
		if ( shader.Id() != 0 )
		{
			GL_CALL( glDeleteShader( shader._id ) );
		}

		shader = ShaderID();
	}

/*
=================================================
	CreateRenderTarget
=================================================
*/
	bool GL4Context::CreateRenderTarget (OUT RenderTargetID &rt)
	{
		ASSERT( not rt.IsValid() );

		GL_CHECK( glGenFramebuffers( 1, &rt._id ) );
		CHECK_ERR( rt.IsValid() );
		
		RenderTargetID	prev_rt = _stateManager->_CurrentRenderTarget();

		_stateManager->BindRenderTarget( rt );
		_stateManager->BindRenderTarget( prev_rt );

		return true;
	}
	
/*
=================================================
	CreateSystemRenderTarget
=================================================
*/
	bool GL4Context::CreateSystemRenderTarget (OUT RenderTargetID &rt)
	{
		rt = RenderTargetID( 0 );
		return true;
	}

/*
=================================================
	DeleteRenderTarget
=================================================
*/
	void GL4Context::DeleteRenderTarget (INOUT RenderTargetID &rt)
	{
		if ( rt._id != 0 )
		{
			if ( _stateManager->IsRenderTargetBinded( rt ) )
				_stateManager->BindRenderTarget( RenderTargetID() );

			GL_CALL( glDeleteFramebuffers( 1, &rt._id ) );
		}
		rt = RenderTargetID();
	}
	
/*
=================================================
	CreateBuffer
=================================================
*/
	bool GL4Context::CreateBuffer (OUT BufferID &buf)
	{
		ASSERT( not buf.IsValid() );

		GL_CHECK( glGenBuffers( 1, &buf._id ) );
		GL_CHECK( glBindBuffer( GL_COPY_READ_BUFFER, buf._id ) );
		GL_CHECK( glBindBuffer( GL_COPY_READ_BUFFER, 0 ) );

		CHECK_ERR( buf.IsValid() );
		return true;
	}

/*
=================================================
	DeleteBuffer
=================================================
*/
	void GL4Context::DeleteBuffer (INOUT BufferID &buf)
	{
		if ( buf._id != 0 )
		{
			GL_CALL( glDeleteBuffers( 1, &buf._id ) );
		}
		buf = BufferID();
	}
	
/*
=================================================
	FenceSync
=================================================
*/
	bool GL4Context::FenceSync (OUT SyncEventID &sync)
	{
		ASSERT( not sync.IsValid() );

		GL_CHECK( sync._id = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 ) );
		CHECK_ERR( sync.IsValid() );

		return true;
	}
	
/*
=================================================
	DeleteSync
=================================================
*/
	void GL4Context::DeleteSync (INOUT SyncEventID &sync)
	{
		if ( sync._id != null )
		{
			GL_CALL( glDeleteSync( sync._id ) );
			sync._id = null;
		}
	}
	
/*
=================================================
	CreateQuery
=================================================
*/
	bool GL4Context::CreateQuery (OUT QueryID &query)
	{
		ASSERT( not query.IsValid() );

		GL_CHECK( glGenQueries( 1, &query._id ) );
		CHECK_ERR( query.IsValid() );

		return true;
	}
	
/*
=================================================
	DeleteQuery
=================================================
*/
	void GL4Context::DeleteQuery (INOUT QueryID &query)
	{
		if ( query._id != 0 )
		{
			GL_CALL( glDeleteQueries( 1, &query._id ) );
			query._id = 0;
		}
	}

/*
=================================================
	_StaticDebugCallback
=================================================
*/
	void GL4Context::_StaticDebugCallback (GLenum source, GLenum type, GLuint id, GLenum severity,
										   GLsizei length, const GLchar* message, const void* userParam)
	{
		((GL4Context*) userParam)->_DebugCallback( source, type, id, severity, length, message );
	}
	
/*
=================================================
	_DebugCallback
=================================================
*/
	void GL4Context::_DebugCallback (GLenum source, GLenum type, GLuint id, GLenum severity,
									 GLsizei length, const GLchar* message) const
	{
		String	str;

		switch ( severity )
		{
			case GL_DEBUG_SEVERITY_HIGH 			: str << "[High]";			break;
			case GL_DEBUG_SEVERITY_MEDIUM 			: str << "[Medium]";		break;
			case GL_DEBUG_SEVERITY_LOW 				: str << "[Low]";			break;
			case GL_DEBUG_SEVERITY_NOTIFICATION		: str << "[Notification]";	break;
			default									: str << "[unknown]";		WARNING( "unknown severity" );
		}

		str << " src: ";

		switch ( source )
		{
			case GL_DEBUG_SOURCE_API				: str << "OpenGL";			break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM		: str << "OS";				break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER	: str << "GL_Compiler";		break;
			case GL_DEBUG_SOURCE_THIRD_PARTY		: str << "Thirf_Party";		break;
			case GL_DEBUG_SOURCE_APPLICATION 		: str << "Application";		break;
			case GL_DEBUG_SOURCE_OTHER 				: str << "Other";			break;
			default									: str << "Unknown";			WARNING( "unknown source" );
		}

		str << ", type: ";

		switch ( type )
		{
			case GL_DEBUG_TYPE_ERROR 				: str << "Error";				break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR	: str << "Deprecated";			break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR	: str << "Undefined_Behavior";	break;
			case GL_DEBUG_TYPE_PORTABILITY 			: str << "Portability";			break;
			case GL_DEBUG_TYPE_PERFORMANCE 			: str << "Perfomance";			break;
			case GL_DEBUG_TYPE_MARKER 				: str << "Marker";				break;
			case GL_DEBUG_TYPE_PUSH_GROUP 			: str << "Push_Group";			break;
			case GL_DEBUG_TYPE_POP_GROUP 			: str << "Pop_Group";			break;
			case GL_DEBUG_TYPE_OTHER 				: str << "Other";				break;
			default									: str << "Unknown";				WARNING( "unknown type" );
		}

		str << ", message:\n";

		if ( message != null and length > 0 )
			str += message;

		LOG( str.cstr(), ELog::Debug );
	}
	

}	// Graphics
}	// Engine

#endif	// GRAPHICS_API_OPENGL
