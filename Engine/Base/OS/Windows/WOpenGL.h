// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Engine/Base/OS/Shared/Application.h"

#if (defined( PLATFORM_WINDOWS ) and not defined( PLATFORM_SDL )) \
	and (defined( GRAPHICS_API_OPENGL ) or defined( GRAPHICS_API_OPENGLES ))


namespace Engine
{
namespace WinPlatform
{
	using namespace Base;


	//
	// Windows OpenGL Library
	//

	struct WindowsLibOpenGL
	{
	// types
	private:
		typedef OS::HiddenOSTypeFrom<void*>		FuncPtr_t;


	// variables
	private:
		OS::Library		_lib;
		FuncPtr_t		_getProc;	// wglGetProcAddress*


	// methods
	public:
		WindowsLibOpenGL ();

		bool Load (StringCRef name = StringCRef());

		void Unload ();

		void * GetProc (StringCRef address) const;

		static StringCRef  GetDefaultName ()
		{
			return "opengl32.dll";
		}

	private:
		bool _OnInit ();
	};



	//
	// Windows OpenGL Context
	//

	class WindowsOpenGLContext : public Noncopyable
	{
	// types
	private:
		typedef OS::HiddenOSTypeFrom<void*>		FuncPtr_t;
		typedef OS::HiddenOSTypeFrom<void*>		Handle_t;


	// variables
	private:
		FuncPtr_t		wglSwapInterval;			// PFNWGLSWAPINTERVALEXTPROC
		FuncPtr_t		wglGetSwapInterval;			// PFNWGLGETSWAPINTERVALEXTPROC
		FuncPtr_t		wglChoosePixelFormat;		// PFNWGLCHOOSEPIXELFORMATARBPROC
		FuncPtr_t		wglCreateContextAttribs;	// PFNWGLCREATECONTEXTATTRIBSARBPROC
		bool			swapControlSupported;

		Handle_t		_deviceContext;				// HDC
		Handle_t		_renderContext;				// HGLRC


	// methods
	public:
		WindowsOpenGLContext ();

		bool Init (const Handle_t &dc, INOUT VideoSettings &vs);
		void Destroy ();

		void MakeCurrent ();
		void ResetCurrent ();

		bool IsCreated () const;
		bool IsCurrent ();

		void SwapBuffers ();

	private:
		bool _InitOpenGL (INOUT VideoSettings &vs);
		bool _InitOpenGL2 ();
	};


}	// WinPlatform
}	// Engine

#endif	// PLATFORM_WINDOWS