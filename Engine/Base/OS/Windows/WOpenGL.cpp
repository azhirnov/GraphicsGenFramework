// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "WOpenGL.h"

#if (defined( PLATFORM_WINDOWS ) and not defined( PLATFORM_SDL )) \
	 and (defined( GRAPHICS_API_OPENGL ) or defined( GRAPHICS_API_OPENGLES ))

#include <Windows.h>
#include "External/opengl/wglext.h"

namespace Engine
{
namespace WinPlatform
{

	typedef decltype(&wglGetProcAddress)		wglGetProcAddressProc_t;
	
/*
=================================================
	constructor
=================================================
*/
	WindowsLibOpenGL::WindowsLibOpenGL () :
		_getProc( UninitializedT<wglGetProcAddressProc_t>() )
	{}
	
/*
=================================================
	Load
=================================================
*/
	bool WindowsLibOpenGL::Load (StringCRef name)
	{
		Unload();

		if ( not name.Empty() )
		{
			if ( _lib.Load( name, true ) )
				return _OnInit();
		}

		if ( _lib.Load( GetDefaultName(), true ) )
			return _OnInit();

		return false;
	}
	
/*
=================================================
	Unload
=================================================
*/
	void WindowsLibOpenGL::Unload ()
	{
		_getProc = null;
		_lib.Unload();
	}
	
/*
=================================================
	GetProc
=================================================
*/
	void * WindowsLibOpenGL::GetProc (StringCRef address) const
	{
		void * res = null;

		if ( _getProc.IsNotNull<wglGetProcAddressProc_t>() and
			 (res = _getProc.Get<wglGetProcAddressProc_t>()( address.cstr() )) != null )
		{
			return res;
		}
			
		if ( (res = _lib.GetProc( address )) != null )
			return res;

		return null;
	}
		
/*
=================================================
	_OnInit
=================================================
*/
	bool WindowsLibOpenGL::_OnInit ()
	{
		_getProc = (wglGetProcAddressProc_t) _lib.GetProc( "wglGetProcAddress" );
		return _getProc.IsNotNull<wglGetProcAddressProc_t>();
	}

	
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	WindowsOpenGLContext::WindowsOpenGLContext () :
		wglSwapInterval( UninitializedT< PFNWGLSWAPINTERVALEXTPROC >() ),
		wglGetSwapInterval( UninitializedT< PFNWGLGETSWAPINTERVALEXTPROC >() ),
		wglChoosePixelFormat( UninitializedT< PFNWGLCHOOSEPIXELFORMATARBPROC >() ),
		wglCreateContextAttribs( UninitializedT< PFNWGLCREATECONTEXTATTRIBSARBPROC >() ),
		swapControlSupported(false),
		_deviceContext( UninitializedT< HDC >() ),
		_renderContext( UninitializedT< HGLRC >() )
	{
	}
	
/*
=================================================
	Init
=================================================
*/
	bool WindowsOpenGLContext::Init (const Handle_t &dc, INOUT VideoSettings &vs)
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
		if ( _deviceContext.IsNotNull<HDC>() ) {
			CHECK( wglMakeCurrent( _deviceContext.Get<HDC>(), null ) == TRUE );
		}

		if ( _renderContext.IsNotNull<HGLRC>() ) {
			CHECK( wglDeleteContext( _renderContext.Get<HGLRC>() ) == TRUE );
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

		const HDC	dc			= _deviceContext.Get<HDC>();
		HGLRC		rc			= null;
		const uint	pixformat	= ChoosePixelFormat( dc, &pfd );
		bool		res			= false;

		if ( pixformat != 0 )
		{
			CHECK( SetPixelFormat( dc, pixformat, &pfd ) == TRUE );
			rc = wglCreateContext( dc );

			if ( rc != null and wglMakeCurrent( dc, rc ) == TRUE )
			{
				wglChoosePixelFormat	= (PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");
				wglCreateContextAttribs	= (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
		
				wglSwapInterval			= (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
				wglGetSwapInterval		= (PFNWGLGETSWAPINTERVALEXTPROC) wglGetProcAddress("wglGetSwapIntervalEXT");

				swapControlSupported	= wglSwapInterval.IsNotNull<PFNWGLSWAPINTERVALEXTPROC>() and
										  wglGetSwapInterval.IsNotNull<PFNWGLGETSWAPINTERVALEXTPROC>();

				res = true;
			}
			CHECK( wglMakeCurrent( dc, null ) == TRUE );

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

		const HDC	dc	= _deviceContext.Get<HDC>();
		HGLRC		rc	= null;

		if ( dc == null )
			RETURN_ERR( "Window must be created before init OpenGL!" );

		CHECK_ERR( _InitOpenGL2() );

		CHECK_ERR( wglChoosePixelFormat.IsNotNull<PFNWGLCHOOSEPIXELFORMATARBPROC>() );
		CHECK_ERR( wglCreateContextAttribs.IsNotNull<PFNWGLCREATECONTEXTATTRIBSARBPROC>() );
		

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
			int		valid		= wglChoosePixelFormat.Get<PFNWGLCHOOSEPIXELFORMATARBPROC>()
										( dc, ctx_int_attribs, ctx_float_attribs, 1, &pixelFormat, &numFormats );

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
			
			if ( not DescribePixelFormat( dc, pixformat, sizeof(pfd), &pfd ) )
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
			rc = wglCreateContextAttribs.Get<PFNWGLCREATECONTEXTATTRIBSARBPROC>()( dc, null, context_attribs );

			if ( rc != null or context_attribs[1] == 2 )
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
		if ( rc == null )
		{
			rc = wglCreateContext( dc );
		}

		CHECK_ERR( rc != null );
		_renderContext = rc;

		CHECK_ERR( wglMakeCurrent( dc, rc ) == TRUE );

		// set vertical synchronization
		if ( swapControlSupported )
		{
			CHECK( wglSwapInterval.Get<PFNWGLSWAPINTERVALEXTPROC>()( int(vs.vsync) ) == TRUE );

			vs.vsync = ( wglGetSwapInterval.Get<PFNWGLGETSWAPINTERVALEXTPROC>()() != 0 );
		}
		
		wglMakeCurrent( dc, null );
		return true;
	}
	
/*
=================================================
	MakeCurrent
=================================================
*/
	void WindowsOpenGLContext::MakeCurrent ()
	{
		ASSERT( IsCreated() );
		CHECK( wglMakeCurrent( _deviceContext.Get<HDC>(), _renderContext.Get<HGLRC>() ) == TRUE );
	}
	
/*
=================================================
	ResetCurrent
=================================================
*/
	void WindowsOpenGLContext::ResetCurrent ()
	{
		ASSERT( IsCreated() );
		CHECK( wglMakeCurrent( _deviceContext.Get<HDC>(), null ) == TRUE );
	}
	
/*
=================================================
	IsCreated
=================================================
*/
	bool WindowsOpenGLContext::IsCreated () const
	{
		return _deviceContext.IsNotNull<HDC>() and _renderContext.IsNotNull<HGLRC>();
	}
	
/*
=================================================
	IsCurrent
=================================================
*/
	bool WindowsOpenGLContext::IsCurrent ()
	{
		return IsCreated() and wglGetCurrentContext() == _renderContext.Get<HGLRC>();
	}
	
/*
=================================================
	SwapBuffers
=================================================
*/
	void WindowsOpenGLContext::SwapBuffers ()
	{
		ASSERT( IsCreated() );
		if ( IsCreated() )
		{
			::SwapBuffers( _deviceContext.Get<HDC>() );
		}
	}


}	// WinPlatform
}	// Engine

#endif	// PLATFORM_WINDOWS