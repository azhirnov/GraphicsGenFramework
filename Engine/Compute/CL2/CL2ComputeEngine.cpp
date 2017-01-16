// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "CL2ComputeEngine.h"

#if defined( COMPUTE_API_OPENCL )

namespace Engine
{
namespace Compute
{
	
/*
=================================================
	constructor
=================================================
*/
	CL2ComputeEngine::CL2ComputeEngine (const SubSystemsRef ss) :
		BaseObject( ss ),
		_platform( null ), _device( null ), _context( null ), _queue( null ),
		_version( 0 ), _totalMemory( 0 ), _isShared( false ), _maxComputeUnits( 0 )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	CL2ComputeEngine::~CL2ComputeEngine ()
	{
		Deinitialize();
	}
	
/*
=================================================
	Initialize
=================================================
*/
	bool CL2ComputeEngine::Initialize (StringCRef deviceName)
	{
		using namespace cl;

		Deinitialize();

		CHECK_ERR( CL2_Init() );

		CHECK_ERR( _ChooseDevice( deviceName ) );
		CHECK_ERR( _CreateContext() );
		CHECK_ERR( _CreateQueue() );

		_WriteInfo();
		CHECK_ERR( _CheckVersion() );

		return true;
	}
	
/*
=================================================
	Deinitialize
=================================================
*/
	void CL2ComputeEngine::Deinitialize ()
	{
		using namespace cl;

		_DeleteQueue();

		if ( _context != null )
		{
			CL_CALL( clReleaseContext( _context ) );
			_context = null;
		}

		_platform		= null;
		_device			= null;
		_version		= 0;
		_buildOptions	= " ";

		CL2_Delete();
	}
	
/*
=================================================
	_ChooseDevice
=================================================
*/
	bool CL2ComputeEngine::_ChooseDevice (StringCRef deviceName)
	{
		using namespace cl;

		Array< cl_platform_id >		platforms;
		Array< cl_device_id >		devices;


		// Choose Platform //
		{
			cl_uint	num_platforms = 0;

			CL_CHECK( clGetPlatformIDs( 0, null, &num_platforms ) );
			CHECK_ERR( num_platforms > 0 );

			platforms.Resize( num_platforms );

			CL_CHECK( clGetPlatformIDs( num_platforms, platforms.ptr(), null ) );
			_platform = platforms[0];
		}


		// Choose Device //
		{
			cl_uint	num_devices = 0;
	
			CL_CHECK( clGetDeviceIDs( _platform, CL_DEVICE_TYPE_ALL, 0, null, &num_devices ) );
			CHECK_ERR( num_devices != 0 );
		
			devices.Resize( num_devices );

			CL_CHECK( clGetDeviceIDs( _platform, CL_DEVICE_TYPE_ALL, num_devices, devices.ptr(), null ) );
		}

		// search device for name and type
		String	info;
		info.Reserve( 1 << 9 );

		cl_device_id	gpu_device	 = null;
		//cl_device_id	accel_device = null;

		FOR( i, devices )
		{
			cl_device_type	dev_type = 0;

			CL_CALL( clGetDeviceInfo( devices[i], CL_DEVICE_TYPE, sizeof(dev_type), &dev_type, null ) );

			if ( gpu_device == null and EnumEq( dev_type, CL_DEVICE_TYPE_GPU ) )
				gpu_device = devices[i];

			//if ( accel_device == null and EnumEq( dev_type, CL_DEVICE_TYPE_ACCELERATOR ) )
			//	accel_device = devices[i];

			if ( not EnumEq( dev_type, CL_DEVICE_TYPE_GPU ) or deviceName.Empty() )
				continue;
				
			CL_CALL( clGetDeviceInfo( devices[i], CL_DEVICE_NAME, info.Capacity(), info.ptr(), null ) );
			info.CalculateLength();

			if ( info.HasSubStringIC( deviceName ) )
			{
				_device = devices[i];
				break;
			}
				
			CL_CALL( clGetDeviceInfo( devices[i], CL_DEVICE_VENDOR, info.Capacity(), info.ptr(), null ) );
			info.CalculateLength();
				
			if ( info.HasSubStringIC( deviceName ) )
			{
				_device = devices[i];
				break;
			}
		}

		if ( _device == null )
			_device = gpu_device;
			
		if ( _device == null )
			_device = devices.Front();

		CHECK_ERR( _device != null );
		return true;
	}
	
/*
=================================================
	_CreateContext
=================================================
*/
# if defined( PLATFORM_WINDOWS ) and defined( GRAPHICS_API_OPENGL )
	
	using namespace GX_STL::winapi;
#	include "External/opengl/wglext.h"

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
	
	using namespace GX_STL::posix;
#	include "External/opengl/glxext.h"

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

		info.Reserve( 1 << 9 );

		log << "OpenCL info\n---------------";

		clGetPlatformInfo( _platform, CL_PLATFORM_NAME, info.Capacity(), info.ptr(), null );
		log << "\nPlatform:       " << info.cstr();
		
		clGetPlatformInfo( _platform, CL_PLATFORM_VERSION, info.Capacity(), info.ptr(), null );
		log << "\nVersion:        " << info.cstr();
		
		clGetDeviceInfo( _device, CL_DEVICE_NAME, info.Capacity(), info.ptr(), null );
		log << "\nDevice:         " << info.cstr();
		
		clGetDeviceInfo( _device, CL_DRIVER_VERSION, info.Capacity(), info.ptr(), null );
		log << "\nDriver:         " << info.cstr();
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(value32), &value32, null ) );
		log << "\nFrequency:      " << value32 << " MHz";

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(value32), &value32, null ) );
		log << "\nCompute Units:  " << value32;
		_maxComputeUnits = value32;

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(value64), &value64, null ) );
		log << "\nLocal Mem:      " << StringUtils::BytesToString( Bytes<ulong>().FromBytes( value64 ) );

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size), &size, null ) );
		log << "\nWork Group:     " << size;
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(_maxWorkGroupSize), _maxWorkGroupSize.ptr(), null ) );
		log << "\nWork items:     ( " << _maxWorkGroupSize.ToString() << " )";

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(value64), &value64, null ) );
		log << "\nGlobal Cache:   " << StringUtils::BytesToString( Bytes<ulong>().FromBytes( value64 ) );		// L1 cache

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(value64), &value64, null ) );
		log << "\nGlobal Mem:     " << StringUtils::BytesToString( Bytes<ulong>().FromBytes( value64 ) );
		_totalMemory.FromBytes( value64 );
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(value64), &value64, null ) );
		log << "\nConst Buffer:   " << StringUtils::BytesToString( Bytes<ulong>().FromBytes( value64 ) );
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_ADDRESS_BITS, sizeof(value32), &value32, null ) );
		log << "\nAddress bits:   " << value32;

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(value64), &value64, null ) );
		log << "\nMax alloc size: " << StringUtils::BytesToString( Bytes<ulong>().FromBytes( value64 ) );
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(value32), &value32, null ) );
		log << "\nConst args:     " << value32;

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_ENDIAN_LITTLE, sizeof(valueb), &valueb, null ) );
		log << "\nEncoding:       " << ( valueb ? "Little endian" : "Big endian");
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_MEM_BASE_ADDR_ALIGN, sizeof(value32), &value32, null ) );
		log << "\nMem align:      " << StringUtils::BitsToString( Bits<uint>( value32 ) );

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(size), &size, null ) );
		log << "\nImage2D width:  " << size << " px";
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(size), &size, null ) );
		log << "\nImage2D height: " << size << " px";
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(size), &size, null ) );
		log << "\nImage3D width:  " << size << " px";
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(size), &size, null ) );
		log << "\nImage3D height: " << size << " px";
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(size), &size, null ) );
		log << "\nImage3D depth:  " << size << " px";
		
#		if COMPUTE_API_OPENCL >= 120
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_IMAGE_MAX_BUFFER_SIZE, sizeof(size), &size, null ) );
		log << "\nBuffer size:    " << size << " px";
		
		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_IMAGE_MAX_ARRAY_SIZE, sizeof(size), &size, null ) );
		log << "\nImageArray size:" << size;

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_BUILT_IN_KERNELS, info.Capacity(), info.ptr(), null ) );
		log << "\nKernels:        " << info.cstr();
#		endif

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_EXTENSIONS, 0, null, &size ) );
		info.Reserve( size );

		CL_CALL( clGetDeviceInfo( _device, CL_DEVICE_EXTENSIONS, info.Capacity(), info.ptr(), null ) );
		log << "\nExtensions:     " << info.cstr();
		
		LOG( log.cstr(), ELog::Debug | ELog::OpenSpoilerFlag );
	}

/*
=================================================
	_CreateQueue
=================================================
*/
	bool CL2ComputeEngine::_CreateQueue ()
	{
		using namespace cl;

		_DeleteQueue();

		cl_int	cl_err = 0;
		CL_CALL( (_queue = clCreateCommandQueueWithProperties( _context, _device, 0, &cl_err )), cl_err );
		CHECK_ERR( _queue != null );

		return true;
	}
	
/*
=================================================
	_DeleteQueue
=================================================
*/
	void CL2ComputeEngine::_DeleteQueue ()
	{
		using namespace cl;

		if ( _queue == null )
			return;

		CL_CALL( clReleaseCommandQueue( _queue ) );
		_queue = null;
	}
	
/*
=================================================
	_CheckVersion
=================================================
*/
	bool CL2ComputeEngine::_CheckVersion ()
	{
		using namespace cl;

		char	buf[256] = {};

		CL_CALL( clGetPlatformInfo( _platform, CL_PLATFORM_VERSION, CountOf(buf), buf, null ) );

		_version = (uint) RoundToInt( StringUtils::FromStrToFloat( StringCRef( buf ).SubString( /*OpenCL*/6 ) ) * 100.0 );

		CHECK_ERR( _version >= MIN_VERSION );
		return true;
	}
	
/*
=================================================
	Synchronize
=================================================
*/
	void CL2ComputeEngine::Synchronize ()
	{
		using namespace cl;

		CL_CALL( clFinish( _queue ) );
	}
	
/*
=================================================
	Barrier
=================================================
*/
	void CL2ComputeEngine::Barrier ()
	{
		using namespace cl;
		
		Synchronize();
		//CL_CALL( clEnqueueBarrierWithWaitList( _queue, 0, null, null ) );
	}
	
/*
=================================================
	SetProgramBuildOptions
=================================================
*/
	void CL2ComputeEngine::SetProgramBuildOptions (StringCRef options)
	{
		_buildOptions.Clear();

#		if COMPUTE_API_OPENCL >= 200
			_buildOptions << " -cl-kernel-arg-info" );
#		endif

		_buildOptions << options;
	}
	
/*
=================================================
	GetAvailableMemory
=================================================
*/
	Bytes<ulong>  CL2ComputeEngine::GetAvailableMemory () const
	{
		// not supported
		return Bytes<ulong>();
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENCL
