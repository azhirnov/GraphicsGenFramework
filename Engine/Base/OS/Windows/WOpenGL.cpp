// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "WOpenGL.h"

#if (defined( PLATFORM_WINDOWS ) and not defined( PLATFORM_SDL )) \
	 and (defined( GRAPHICS_API_OPENGL ) or defined( GRAPHICS_API_OPENGLES ))

namespace Engine
{
namespace WinPlatform
{
	
/*
=================================================
	constructor
=================================================
*/
	WindowsOpenGLContext::WindowsOpenGLContext () :
		wglSwapInterval(null),		wglGetSwapInterval(null),
		wglChoosePixelFormat(null),	wglCreateContextAttribs(null),
		swapControlSupported(false),
		_deviceContext(null),		_renderContext(null)
	{
	}
	
/*
=================================================
	Init
=================================================
*/
	bool WindowsOpenGLContext::Init (HDC dc, INOUT VideoSettings &vs)
	{
		Destroy();

		_deviceContext	= dc;

		CHECK_ERR( _InitOpenGL( vs ) );

		return true;
	}
	
/*
=================================================
	Destroy
=================================================
*/
	void WindowsOpenGLContext::Destroy ()
	{
		if ( _deviceContext != null ) {
			CHECK( wglMakeCurrent( _deviceContext, null ) == TRUE );
		}

		if ( _renderContext != null ) {
			CHECK( wglDeleteContext( _renderContext ) == TRUE );
		}

		wglSwapInterval			= null;
		wglGetSwapInterval		= null;
		wglChoosePixelFormat	= null;
		wglCreateContextAttribs	= null;
		swapControlSupported	= false;
		_deviceContext			= null;
		_renderContext			= null;
	}
	
/*
=================================================
	_InitOpenGL2
=================================================
*/
	bool WindowsOpenGLContext::_InitOpenGL2 ()
	{
		PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
									  32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };

		const uint	pixformat	= ChoosePixelFormat( _deviceContext, &pfd );
		HGLRC		rc			= null;
		bool		res			= false;

		if ( pixformat != 0 )
		{
			CHECK( SetPixelFormat( _deviceContext, pixformat, &pfd ) == TRUE );
			rc = wglCreateContext( _deviceContext );

			if ( rc != null and wglMakeCurrent( _deviceContext, rc ) == TRUE )
			{
				wglChoosePixelFormat	= (PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");
				wglCreateContextAttribs	= (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
		
				wglSwapInterval			= (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
				wglGetSwapInterval		= (PFNWGLGETSWAPINTERVALEXTPROC) wglGetProcAddress("wglGetSwapIntervalEXT");

				swapControlSupported	= (wglSwapInterval != null) and (wglGetSwapInterval != null);

				res = true;
			}
			CHECK( wglMakeCurrent( _deviceContext, null ) == TRUE );

			CHECK( wglDeleteContext( rc ) == TRUE );
		}
		
		return res;
	}

/*
=================================================
	_InitOpenGL
=================================================
*/
	bool WindowsOpenGLContext::_InitOpenGL (INOUT VideoSettings &vs)
	{
		if ( vs.IsAutoRenderer() )
			vs.version = VideoSettings::OPENGL_AUTO;

		CHECK_ERR( vs.IsOpenGL() or vs.IsOpenGLES() );

		if ( _deviceContext == null )
			RETURN_ERR( "Window must be created before init OpenGL!" );

		CHECK_ERR( _InitOpenGL2() );

		CHECK_ERR( wglChoosePixelFormat != null );
		CHECK_ERR( wglCreateContextAttribs != null );
		

		// Chose Pixel Format //
		PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
									  32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };

		uint	pixformat		= 0;

		float	ctx_float_attribs[]	= {0.0, 0.0};

		int		ctx_int_attribs[]	=
		{
			WGL_DRAW_TO_WINDOW_ARB,		true,
			WGL_SUPPORT_OPENGL_ARB,		true,
			WGL_ACCELERATION_ARB,		WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB,			vs.AlphaBits() == 0 ? 24 : 32,
			WGL_ALPHA_BITS_ARB,			vs.AlphaBits() == 0 ? 1 : vs.AlphaBits(),
			WGL_DEPTH_BITS_ARB,			vs.depthBits,
			WGL_STENCIL_BITS_ARB,		vs.stencilBits,
			WGL_DOUBLE_BUFFER_ARB,		true,
			WGL_STEREO_ARB,				vs.stereo,
			WGL_PIXEL_TYPE_ARB,			WGL_TYPE_RGBA_ARB,
			WGL_SAMPLE_BUFFERS_ARB,		vs.multiSamples > 1 ? true : false,
			WGL_SAMPLES_ARB,			vs.multiSamples,
			0,0
		};

		int rv = 0;

		// validate pixel format
		while ( rv == 0 )
		{
			int		pixelFormat	= 0;
			uint	numFormats	= 0;
			int		valid		= wglChoosePixelFormat( _deviceContext, ctx_int_attribs, ctx_float_attribs, 1, &pixelFormat, &numFormats );

			if ( valid != 0 and numFormats > 0 )
				rv = pixelFormat;
			else
			{
				if ( ctx_int_attribs[21] != 0 )		// msaa
					ctx_int_attribs[21] >>= 1;
				else
				if ( ctx_int_attribs[13] != 0 )		// stencil buffer
					ctx_int_attribs[13] = 0;
				else
				if ( ctx_int_attribs[11] > 24 )		// depth buffer
					ctx_int_attribs[11] = 24;
				else
				if ( ctx_int_attribs[11] != 16 )	// depth buffer
					ctx_int_attribs[11] = 16;
				else
					break;
			}
		}

		if ( rv != 0 )
		{
			pixformat		= rv;
			vs.depthBits	= (VideoSettings::EDepthFromat) ctx_int_attribs[11];
			vs.stencilBits	= (VideoSettings::EStencilFormat) ctx_int_attribs[13];
			vs.multiSamples	= (ubyte) ctx_int_attribs[21];
			
			if ( not DescribePixelFormat( _deviceContext, pixformat, sizeof(pfd), &pfd ) )
				LOG( "Can't describe pixel format", ELog::Warning );
		}

		const bool	is_core_version =	( vs.IsOpenGL() and vs.version >= VideoSettings::OPENGL_3_3 ) or
										( vs.IsOpenGLES() and vs.version >= VideoSettings::OPENGLES_3_0 );

		const uint	CONTEXT_FLAG_NO_ERROR_BIT_KHR = 0x00000008;

		// Create OpenGL Context //
		int	context_attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, vs.MajorVersion(),
			WGL_CONTEXT_MINOR_VERSION_ARB, vs.MinorVersion(),
			WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB |
											(vs.IsDebugContext() ? WGL_CONTEXT_DEBUG_BIT_ARB : 0) |
											(vs.IsNoErrorContext() ? CONTEXT_FLAG_NO_ERROR_BIT_KHR : 0),
			WGL_CONTEXT_PROFILE_MASK_ARB,  is_core_version ?
												WGL_CONTEXT_CORE_PROFILE_BIT_ARB :
												WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			0
		};

		// search for valid context attribs
		for (;;)
		{
			_renderContext = wglCreateContextAttribs( _deviceContext, null, context_attribs );

			if ( _renderContext != null or context_attribs[1] == 2 )
				break;
				
			// for opengl 4.x
			if ( context_attribs[1] < 4 )
			{
				if ( EnumEq( context_attribs[5], CONTEXT_FLAG_NO_ERROR_BIT_KHR ) )
					context_attribs[5] = context_attribs[5] & ~CONTEXT_FLAG_NO_ERROR_BIT_KHR;
				else
				if ( EnumEq( context_attribs[5], WGL_CONTEXT_DEBUG_BIT_ARB ) )
					context_attribs[5] = context_attribs[5] & ~WGL_CONTEXT_DEBUG_BIT_ARB;
			}

			// decrease version number
			if ( context_attribs[3] == 0 )
			{
				context_attribs[1]--;
				context_attribs[3] = 9;
			}
			else
				context_attribs[3]--;
		}

		// create compatibility profile context
		if ( _renderContext == null )
		{
			_renderContext = wglCreateContext( _deviceContext );
		}

		CHECK_ERR( _renderContext != null );
		CHECK_ERR( wglMakeCurrent( _deviceContext, _renderContext ) == TRUE );

		// set vertical synchronization
		if ( swapControlSupported )
		{
			CHECK( wglSwapInterval( int(vs.vsync) ) == TRUE );

			vs.vsync = ( wglGetSwapInterval() != 0 );
		}
		
		wglMakeCurrent( _deviceContext, null );
		return true;
	}


}	// WinPlatform
}	// Engine

#endif	// PLATFORM_WINDOWS