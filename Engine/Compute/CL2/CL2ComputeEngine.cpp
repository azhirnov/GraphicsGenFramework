// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

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
		_platform( null ),		_device( null ),
		_context( null ),		_queue( null ),
		_version( 0 ),			_totalMemory( 0 ),
		_isShared( false ),		_maxComputeUnits( 0 ),
		_maxInvocations( 0 )
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

		_version = (uint) RoundToInt( StringUtils::ToDouble( StringCRef( buf ).SubString( /*OpenCL*/6 ) ) * 100.0 );

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
		
		CL_CALL( clEnqueueBarrierWithWaitList( _queue, 0, null, null ) );
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
			_buildOptions << " -cl-kernel-arg-info " );
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
