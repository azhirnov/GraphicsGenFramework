// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "CL2ComputeEngine.h"

#if defined( COMPUTE_API_OPENCL )

// Windows & OpenGL
# if defined( PLATFORM_WINDOWS ) and defined( GRAPHICS_API_OPENGL )

#	include <Windows.h>
#	include "External/opengl/wglext.h"

// Linux & OpenGL
# elif defined( PLATFORM_BASE_POSIX ) and defined( GRAPHICS_API_OPENGL )

#	include "External/opengl/glxext.h"

# endif

namespace Engine
{
namespace Compute
{
	
/*
=================================================
	_CreateContext
=================================================
*/
# if defined( PLATFORM_WINDOWS ) and defined( GRAPHICS_API_OPENGL )

	bool CL2ComputeEngine::_CreateContext ()
	{
		using namespace cl;

		HDC		dc	= wglGetCurrentDC();
		HGLRC	rc	= wglGetCurrentContext();

		cl_context_properties properties[] =
		{
			CL_CONTEXT_PLATFORM,	(cl_context_properties) _platform,
			CL_GL_CONTEXT_KHR,		(cl_context_properties) rc,
			CL_WGL_HDC_KHR,			(cl_context_properties) dc,
			0,	0
		};

		if ( rc == null or dc == null )
			properties[2] = properties[3] = 0;
		
		cl_int	cl_err = 0;
		CL_CALL( (_context = clCreateContext( properties, 1, &_device, null, null, &cl_err )), cl_err );
		CHECK_ERR( _context != null );

		_isShared = (rc != null and dc != null);
		return true;
	}

# elif defined( PLATFORM_BASE_POSIX ) and defined( GRAPHICS_API_OPENGL )

	bool CL2ComputeEngine::_CreateContext ()
	{
		using namespace cl;

		Display *	display	= glXGetCurrentDisplay();
		GLXContext	context	= glXGetCurrentContext();

		cl_context_properties properties[] =
		{
			CL_CONTEXT_PLATFORM,	(cl_context_properties) _platform,
			CL_GL_CONTEXT_KHR,		(cl_context_properties) context,
			CL_GLX_DISPLAY_KHR,		(cl_context_properties) display,
			0,	0
		};

		if ( rc == null or dc == null )
			properties[2] = properties[3] = 0;
		
		cl_int	cl_err = 0;
		CL_CALL( (_context = clCreateContext( properties, 1, &_device, null, null, &cl_err )), cl_err );
		CHECK_ERR( _context != null );

		_isShared = (rc != null and dc != null);
		return true;
	}

# else

	// without sharing
	bool CL2ComputeEngine::_CreateContext ()
	{
		using namespace cl;

		cl_context_properties properties[] =
		{
			CL_CONTEXT_PLATFORM,	(cl_context_properties) _platform,
			0,	0
		};
		
		cl_int	cl_err = 0;
		CL_CALL( (_context = clCreateContext( properties, 1, &_device, null, null, &cl_err )), cl_err );
		CHECK_ERR( _context != null );

		_isShared = false;
		return true;
	}

# endif
	
/*
=================================================
	_WriteInfo
=================================================
*/
	void CL2ComputeEngine::_WriteInfo ()
	{
		using namespace cl;

		String		log;
		String		info;
		cl_bool		valueb   = false;
		cl_uint		value32  = 0;
		cl_ulong	value64	 = 0;
		size_t		size	 = 0;
		usize3		size3;

		info.Reserve( 1 << 9 );

		log << "OpenCL info\n---------------";

		clGetPlatformInfo( _platform, CL_PLATFORM_NAME, info.Capacity(), info.ptr(), null );
		log << "\nPlatform:         " << info.cstr();
		
		clGetPlatformInfo( _platform, CL_PLATFORM_VERSION, info.Capacity(), info.ptr(), null );
		log << "\nVersion:          " << info.cstr();
		
		clGetDeviceInfo( _device, CL_DEVICE_NAME, info.Capacity(), info.ptr(), null );
		log << "\nDevice:           " << info.cstr();
		
		clGetDeviceInfo( _device, CL_DRIVER_VERSION, info.Capacity(), info.ptr(), null );
		log << "\nDriver:           " << info.cstr();
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(value32), &value32, null ) );
		log << "\nFrequency:        " << value32 << " MHz";

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(value32), &value32, null ) );
		log << "\nCompute Units:    " << value32;
		_maxComputeUnits = value32;

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(value64), &value64, null ) );
		log << "\nLocal Mem:        " << ToString( Bytes<ulong>::FromBytes( value64 ) );

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size), &size, null ) );
		log << "\nMax invocations:  " << size;
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size3), size3.ptr(), null ) );
		log << "\nMax local groups: " << ToString( size3 );

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_ADDRESS_BITS, sizeof(value32), &value32, null ) );
		log << "\nAddress bits:     " << value32;

		_maxInvocations		= ulong( size );
		_maxLocalGroupSize	= ulong3( size3 );
		_maxThreads			= ToMask< ulong3 >( BitsU(value32) );		// TODO: _maxThreads.Volume() = MaxValue<usize>();

		//log << "\nMax groups:       " << ToString( _maxThreads / _maxLocalGroupSize );

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(value64), &value64, null ) );
		log << "\nGlobal Cache:     " << ToString( Bytes<ulong>::FromBytes( value64 ) );		// L1 cache

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(value64), &value64, null ) );
		log << "\nGlobal Mem:       " << ToString( Bytes<ulong>::FromBytes( value64 ) );
		_totalMemory.SetBytes( value64 );
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(value64), &value64, null ) );
		log << "\nConst Buffer:     " << ToString( Bytes<ulong>::FromBytes( value64 ) );

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(value64), &value64, null ) );
		log << "\nMax alloc size:   " << ToString( Bytes<ulong>::FromBytes( value64 ) );
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(value32), &value32, null ) );
		log << "\nConst args:       " << value32;
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(size), &size, null ) );
		log << "\nParam size:       " << ToString( BytesU( size ) );

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_ENDIAN_LITTLE, sizeof(valueb), &valueb, null ) );
		log << "\nEncoding:         " << ( valueb ? "Little endian" : "Big endian");
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MEM_BASE_ADDR_ALIGN, sizeof(value32), &value32, null ) );
		log << "\nMem align:        " << ToString( Bits<uint>( value32 ) );

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(size), &size, null ) );
		log << "\nImage2D width:    " << size << " px";
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(size), &size, null ) );
		log << "\nImage2D height:   " << size << " px";
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(size), &size, null ) );
		log << "\nImage3D width:    " << size << " px";
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(size), &size, null ) );
		log << "\nImage3D height:   " << size << " px";
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(size), &size, null ) );
		log << "\nImage3D depth:    " << size << " px";
		
#		if COMPUTE_API_OPENCL >= 120
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_IMAGE_MAX_BUFFER_SIZE, sizeof(size), &size, null ) );
		log << "\nBuffer size:      " << size << " px";
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_IMAGE_MAX_ARRAY_SIZE, sizeof(size), &size, null ) );
		log << "\nImageArray:       " << size << " layers";

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_BUILT_IN_KERNELS, info.Capacity(), info.ptr(), null ) );
		log << "\nKernels:          " << info.cstr();
#		endif

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_EXTENSIONS, 0, null, &size ) );
		info.Reserve( size );

		LOG( log.cstr(), ELog::Debug | ELog::SpoilerFlag );

		// write extensions
		log.Clear();
		log << "OpenCL Extensions\n---------------\n";

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_EXTENSIONS, info.Capacity(), info.ptr(), null ) );
		info.CalculateLength();

		const usize	maxLineLength	= 100;
		usize		pos				= 0;
		usize		last_pos		= 0;

		for (; info.Find( ' ', OUT pos, pos+1 ); )
		{
			if ( pos - last_pos > maxLineLength )
			{
				log << info.SubString( last_pos, pos - last_pos ) << '\n';

				last_pos = pos+1;
			}
		}

		log << info.SubString( pos+1 );
		
		LOG( log.cstr(), ELog::Debug | ELog::SpoilerFlag );
	}

}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENCL
