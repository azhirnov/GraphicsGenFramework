// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/OS/Shared/Application.h"

#if (defined( PLATFORM_WINDOWS ) and not defined( PLATFORM_SDL )) \
	and (defined( GRAPHICS_API_OPENGL ) or defined( GRAPHICS_API_OPENGLES ))

namespace Engine
{
namespace WinPlatform
{
	using namespace Base;
	using namespace GX_STL::winapi;

#	include "External/opengl/wglext.h"



	//
	// Windows OpenGL Library
	//

	struct WindowsLibOpenGL
	{
	// types
	private:
		typedef PROC (CALLBACK * wglGetProcAddressProc_t) (LPCSTR lpszProc);


	// variables
	private:
		Library						_lib;
		wglGetProcAddressProc_t		_getProc;


	// methods
	public:
		WindowsLibOpenGL () : _getProc(null)
		{}

		bool Load (StringCRef name = StringCRef())
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

		void Unload ()
		{
			_getProc = null;
			_lib.Unload();
		}

		void * GetProc (StringCRef address) const
		{
			void * res = null;

			if ( _getProc != null and (res = _getProc( address.cstr() )) != null )
				return res;
			
			if ( (res = _lib.GetProc( address )) != null )
				return res;

			return null;
		}

		static StringCRef  GetDefaultName ()
		{
			return "opengl32.dll";
		}


	private:
		
		bool _OnInit ()
		{
			_getProc = (wglGetProcAddressProc_t) _lib.GetProc( "wglGetProcAddress" );
			return _getProc != null;
		}
	};



	//
	// Windows OpenGL Context
	//

	class WindowsOpenGLContext : public Noncopyable
	{
	// variables
	private:
		PFNWGLSWAPINTERVALEXTPROC			wglSwapInterval;
		PFNWGLGETSWAPINTERVALEXTPROC		wglGetSwapInterval;
		PFNWGLCHOOSEPIXELFORMATARBPROC		wglChoosePixelFormat;
		PFNWGLCREATECONTEXTATTRIBSARBPROC	wglCreateContextAttribs;
		bool								swapControlSupported;

		HDC									_deviceContext;
		HGLRC								_renderContext;


	// methods
	public:
		WindowsOpenGLContext ();

		bool Init (HDC dc, INOUT VideoSettings &vs);

		void Destroy ();

		void MakeCurrent ()
		{
			CHECK( wglMakeCurrent( _deviceContext, _renderContext ) == TRUE );
		}

		void ResetCurrent ()
		{
			CHECK( wglMakeCurrent( _deviceContext, null ) == TRUE );
		}

		bool IsCreated () const
		{
			return _deviceContext != null and _renderContext != null;
		}

		bool IsCurrent ()
		{
			return IsCreated() and wglGetCurrentContext() == _renderContext;
		}

		void SwapBuffers ()
		{
			if ( IsCreated() )
			{
				winapi::SwapBuffers( _deviceContext );
			}
		}


	private:
		bool _InitOpenGL (INOUT VideoSettings &vs);
		bool _InitOpenGL2 ();
	};


}	// WinPlatform
}	// Engine

#endif	// PLATFORM_WINDOWS