// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "gl4.h"
#include "Engine/Base/Engine.Base.h"

#if defined( GRAPHICS_API_OPENGL )

namespace gl
{
	
#	pragma warning (push)
#	pragma warning (disable : 4100)	// 'identifier' : unreferenced formal parameter

	// get function address from driver //
#	define OPENGL4_GET_PROC( _p_, _n_, _d_ ) \
		++counter; \
		if ( (_n_ = (_p_)GL4_GetProcAddress( TOSTRING( _n_ ) )) == null ) \
		{ \
			_n_ = _d_; \
			missing++; \
		}

	// Default Functions //
#	undef  GL4_BUILD_DEFFUNC
#	define GL4_BUILD_DEFFUNC( _retType_, _funcName_, _funcParams_, _retValue_ ) \
		_retType_ GL4_APIENTRY glDefault##_funcName_ _funcParams_ \
		{ \
			using namespace Engine; \
			LOG( "Used dummy function gl" TOSTRING( _funcName_ ), ELog::Debug ); \
			return _retValue_; \
		}
	
	// pointers to functions //
#	undef  GL4_BUILDFUNC
#	define GL4_BUILDFUNC( _retType_, _funcName_, _funcParams_, _retValue_ ) \
		PFNGL##_funcName_##PROC		gl##_funcName_ = &glDefault##_funcName_;
	

	GL4_CORE_FUNCTIONS( GL4_BUILD_DEFFUNC )
	GL4_CORE_FUNCTIONS( GL4_BUILDFUNC )

	static int	gl4Version = 0;
	
#	pragma warning (pop)

/*
=================================================
	GL4_GetProcAddress
=================================================
*/
	void * GL4_GetProcAddress (const char *name)
	{
		using namespace Engine;
		void * ptr = SingletonSingleThread::Instance< Base::LibOpenGL >()->GetProc( name );
		
		DEBUG_ONLY(
		if ( ptr == null ) {
			LOG( (String("Can't get address of ") << name).cstr(), ELog::Debug );
		})
		return ptr;
	}

/*
=================================================
	GL4_Init
=================================================
*/
	bool GL4_Init ()
	{
		using namespace Engine;

		int	missing = 0;	// number of missing function addresses
		int counter = 0;	// number of all functions

		CHECK_ERR( SingletonSingleThread::Instance< Base::LibOpenGL >()->Load() );
		
#		define GL4_GETPROC( _retType_, _funcName_, _funcParams_, _retValue_ ) \
			OPENGL4_GET_PROC( PFNGL##_funcName_##PROC, gl##_funcName_, &glDefault##_funcName_ );
		
		GL4_CORE_FUNCTIONS( GL4_GETPROC )

#		undef GL4_GETPROC
		
		{
			int	major = 0;
			int minor = 0;
			GL_CALL( glGetIntegerv( GL_MAJOR_VERSION, &major ) );
			GL_CALL( glGetIntegerv( GL_MINOR_VERSION, &minor ) );
			gl4Version = major * 100 + minor * 10;
		}

		if ( missing > 0 )
		{
			LOG( (String("While get address of OpenGL functions: ") <<
				 missing << " missing, " << (counter - missing) << " successfully.").cstr(),
				 ELog::Debug );
		}
		
		return true;
	}
	
/*
=================================================
	GL4_Delete
=================================================
*/
	void GL4_Delete ()
	{
		using namespace Engine;

		#define GL4_ZEROFUNC( _retType_, _funcName_, _funcParams_, _retValue_ ) \
			gl##_funcName_ = &glDefault##_funcName_;

		GL4_CORE_FUNCTIONS( GL4_ZEROFUNC )
			
		SingletonSingleThread::Instance< Base::LibOpenGL >()->Unload();
	}
	
/*
=================================================
	GL4_WriteInfo
=================================================
*/
	void GL4_WriteInfo ()
	{
		using namespace Engine;

		String	log;
		GLint	data[4] = {};

		log	<< "OpenGL info\n---------------"
			<< "\nRenderer:              " << (const char*)glGetString( GL_RENDERER )
			<< "\nVersion:               " << (const char*)glGetString( GL_VERSION )
			<< "\nVendor:                " << (const char*)glGetString( GL_VENDOR )
			<< "\nGLSL:                  " << (const char*)glGetString( GL_SHADING_LANGUAGE_VERSION );

		GL_CALL( glGetIntegerv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, data ) );
		log	<< "\nMax anisotropy:        " << data[0];
		
		GL_CALL( glGetIntegerv( GL_MAX_VERTEX_ATTRIBS, data ) );
		log << "\nMax attribs:           " << data[0];
			
		GL_CALL( glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, data ) );
		log << "\nMax texture units:     " << data[0];
		
		GL_CALL( glGetIntegerv( GL_MAX_IMAGE_UNITS, data ) );
		log << "\nMax image units:       " << data[0];
		
		GL_CALL( glGetIntegerv( GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS, data ) );
		log << "\nMax image and output:  " << data[0];
		
		GL_CALL( glGetIntegerv( GL_MAX_TEXTURE_SIZE, data ) );
		log << "\nMax texture size:      " << data[0] << " px";
		
		GL_CALL( glGetIntegerv( GL_MAX_3D_TEXTURE_SIZE, data ) );
		log << "\nMax texture 3d size:   " << data[0] << " px";
		
		GL_CALL( glGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS, data ) );
		log << "\nMax texture layers:    " << data[0];
		
		GL_CALL( glGetIntegerv( GL_MAX_CUBE_MAP_TEXTURE_SIZE, data ) );
		log << "\nMax cube map size:     " << data[0] << " px";
		
		GL_CALL( glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE, data ) );
		log << "\nMax render buf size:   " << data[0] << " px";
		
		GL_CALL( glGetIntegerv( GL_MAX_TEXTURE_BUFFER_SIZE, data ) );
		log << "\nMax texture buf size:  " << ToString( BytesU( data[0] ) );
		
		GL_CALL( glGetIntegerv( GL_MAX_VIEWPORT_DIMS, data ) );
		log << "\nMax viewport dim:      " << data[0] << " px";
		
		GL_CALL( glGetIntegerv( GL_MAX_VIEWPORTS, data ) );
		log << "\nMax viewports:         " << data[0];
		
		GL_CALL( glGetIntegerv( GL_MAX_COLOR_TEXTURE_SAMPLES, data ) );
		log << "\nMax color tex samples: " << data[0];
		
		GL_CALL( glGetIntegerv( GL_MAX_DEPTH_TEXTURE_SAMPLES, data ) );
		log << "\nMax depth tex samples: " << data[0];
		
		GL_CALL( glGetIntegerv( GL_MAX_INTEGER_SAMPLES, data ) );
		log << "\nMax integer samples:   " << data[0];
		
		GL_CALL( glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, data ) );
		log << "\nMax color attachments: " << data[0];
		
		GL_CALL( glGetIntegerv( GL_MAX_DRAW_BUFFERS, data ) );
		log << "\nMax draw buffers:      " << data[0];
		
		GL_CALL( glGetIntegerv( GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, data ) );
		log << "\nMax dual draw buffers: " << data[0];

		GL_CALL( glGetIntegerv( GL_MAX_UNIFORM_BUFFER_BINDINGS, data ) );
		log << "\nMax UBO bindings:      " << data[0];
		
		GL_CALL( glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, data ) );
		log << "\nMax UBO size:          " << ToString( BytesU( data[0] ) );
		
		GL_CALL( glGetIntegerv( GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, data ) );
		log << "\nMax SSBO bindings:     " << data[0];
			
		GL_CALL( glGetIntegerv( GL_MAX_SHADER_STORAGE_BLOCK_SIZE, data ) );
		log << "\nMax SSBO size:         " << ToString( BytesU( data[0] ) );
		
		GL_CALL( glGetIntegerv( GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, data ) );
		log << "\nMax SSBO blocks:       " << data[0];

		GL_CALL( glGetIntegerv( GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES, data ) );
		log << "\nMax SSBO out blocks:   " << data[0];

		GL_CALL( glGetIntegerv( GL_MAX_TESS_GEN_LEVEL, data ) );
		log << "\nMax tess gen level:    " << data[0];

		// TODO: GL_MAX_FRAMEBUFFER_WIDTH, GL_MAX_FRAMEBUFFER_HEIGHT,
		//		GL_MAX_FRAMEBUFFER_LAYERS, GL_MAX_FRAMEBUFFER_SAMPLES,
		//		GL_MAX_SERVER_WAIT_TIMEOUT

		LOG( log.cstr(), ELog::Debug | ELog::SpoilerFlag );

		// write all extensions
		#if 1
			log.Clear();
			log << "OpenGL Extensions\n---------------\n";

			GLint			num_ext;
			const char *	ext_str;
			
			GL_CALL( glGetIntegerv( GL_NUM_EXTENSIONS, &num_ext ) );
			
			for (GLint i = 0; i < num_ext; ++i)
			{
				GL_CALL( ext_str = (const char *) glGetStringi( GL_EXTENSIONS, i ) );

				log << (i == 0 ? "" : ((i & 3) == 0 ? ",\n" : ", ")) << ext_str;
			}
		#endif

		LOG( log.cstr(), ELog::Debug | ELog::SpoilerFlag );
	}

/*
=================================================
	GL4_GetVersion
=================================================
*/
	int  GL4_GetVersion ()
	{
		return gl4Version;
	}
	
/*
=================================================
	GL4_GetShaderVersion
=================================================
*/
	int  GL4_GetShaderVersion ()
	{
		if ( gl4Version >= 330 )
			return gl4Version;

		switch ( gl4Version )
		{
			case 320 :	return 150;
			case 310 :	return 140;
			case 300 :	return 130;
			case 210 :	return 120;
			case 200 :	return 110;
		}

		return 0;
	}

/*
=================================================
	GL4_ClearErrors
=================================================
*/
	void GL4_ClearErrors (int maxErrors)
	{
		for (int i = 0; i < maxErrors and glGetError() != GL_NO_ERROR; ++i) {}
	}
	
/*
=================================================
	GL4_CheckErrors
=================================================
*/
	bool GL4_CheckErrors (const char *glcall, const char *func, const char *file, int line)
	{
		using namespace Engine;
		
		String	msg;
		GLenum	err = glGetError();

		if ( err == GL_NO_ERROR )
			return true;

		msg << "OpenGL error: ";

		switch ( err )
		{
			case GL_INVALID_ENUM:
				msg << "GL_INVALID_ENUM";
				break;

			case GL_INVALID_VALUE:
				msg << "GL_INVALID_VALUE";
				break;

			case GL_INVALID_OPERATION:
				msg << "GL_INVALID_OPERATION";
				break;

			case GL_INVALID_FRAMEBUFFER_OPERATION:
				msg << "GL_INVALID_FRAMEBUFFER_OPERATION";
				break;

			case GL_OUT_OF_MEMORY:
				msg << "GL_OUT_OF_MEMORY";
				break;

			default:
				msg << "code 0x" << String().FormatI( err, 16 );
				break;
		}
		
		msg << ", in " << glcall << ", function " << func;

		LOG( msg.cstr(), ELog::Error, file, line );
		return false;
	}


}	// gl

#endif	// GRAPHICS_API_OPENGL