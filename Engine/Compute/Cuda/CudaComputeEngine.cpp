// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "CudaComputeEngine.h"

#if defined( COMPUTE_API_CUDA )

namespace Engine
{
namespace Compute
{
	
/*
=================================================
	constructor
=================================================
*/
	CudaComputeEngine::CudaComputeEngine (const SubSystemsRef ss) :
		BaseObject( ss ),
		_device( -1 ), _context( null ), _stream( null ),
		_totalMemory( 0 ), _version( 0 ), _isShared( false )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	CudaComputeEngine::~CudaComputeEngine ()
	{
	}
	
/*
=================================================
	Initialize
=================================================
*/
	bool CudaComputeEngine::Initialize (StringCRef deviceName)
	{
		Deinitialize();

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
	void CudaComputeEngine::Deinitialize ()
	{
		using namespace cuda;
		
		if ( _stream != null )
		{
			CU_CALL( cuStreamDestroy( _stream ) );
			_stream = null;
		}

		if ( _context != null )
		{
			CU_CALL( cuCtxDestroy( _context ) );
			_context = null;
		}

		_device = -1;
	}
	
/*
=================================================
	_ChooseDevice
=================================================
*/
	bool CudaComputeEngine::_ChooseDevice (StringCRef deviceName)
	{
		using namespace cuda;

		static const int	INVALID_CU_DEVICE = -1;

		CU_CHECK( cuInit( 0 ) );

		_device = INVALID_CU_DEVICE;

		int			dev_count	= 0;
		CUdevice	hiprio_dev	= INVALID_CU_DEVICE;
		float		priority	= 0.0f;
		char		name[ 128 ]	= {};

		CU_CALL( cuDeviceGetCount( &dev_count ) );
		CHECK_ERR( dev_count > 0 );

		for (int i = 0; i < dev_count; ++i)
		{
			CUdevice	dev			= INVALID_CU_DEVICE;
			int			param		= 0;
			float		prio		= 0.0f;

			int			clock_rate	= 0;
			int			processors	= 0;
			int			proc_scale	= 0;
			int			mem_clock	= 0;
			int			mem_bus		= 0;
			int			cap_major	= 0;
			int			cap_minor	= 0;

			CU_CALL( cuDeviceGet( &dev, i ) );
			CU_CALL( cuDeviceGetName( name, CountOf( name ), dev ) );

			if ( _device == INVALID_CU_DEVICE and
				 StringCRef( name ).HasSubStringIC( deviceName ) )
			{
				_device = dev;
			}

			CU_CALL( cuDeviceGetAttribute( &clock_rate, CU_DEVICE_ATTRIBUTE_CLOCK_RATE, dev ) );
			CU_CALL( cuDeviceGetAttribute( &processors, CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, dev ) );
			
			CU_CALL( cuDeviceGetAttribute( &mem_clock, CU_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE, dev ) );
			CU_CALL( cuDeviceGetAttribute( &mem_bus, CU_DEVICE_ATTRIBUTE_GLOBAL_MEMORY_BUS_WIDTH, dev ) );
			
			CU_CALL( cuDeviceGetAttribute( &cap_major, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR, dev ) );
			CU_CALL( cuDeviceGetAttribute( &cap_minor, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR, dev ) );
			
			switch ( cap_major * 10 + cap_minor )
			{
				case 10 :
				case 11 :
				case 12 :
				case 13 :	proc_scale = 8;		break;

				case 20 :	proc_scale = 32;	break;
				case 21 :	proc_scale = 48;	break;

				case 30 :
				case 35 :
				case 37 :	proc_scale = 192;	break;

				case 50 :
				case 52 :	proc_scale = 128;	break;

				default :
					WARNING( "unknown compute capability version!" );
					proc_scale = 1;
			}

			prio += (clock_rate * 0.001f * processors * proc_scale);
			prio += (mem_clock * 0.001f * mem_bus) * 0.01f;

			if ( prio > priority )
			{
				hiprio_dev	= dev;
				priority	= prio;
			}
		}

		if ( _device == INVALID_CU_DEVICE and hiprio_dev != INVALID_CU_DEVICE )
			_device = hiprio_dev;

		if ( _device == INVALID_CU_DEVICE )
			cuDeviceGet( &_device, 0 );

		size_t	total_mem = 0;
		CU_CALL( cuDeviceTotalMem( &total_mem, _device ) );

		_totalMemory = total_mem;
		return true;
	}
	
/*
=================================================
	_CreateContext
=================================================
*/
	bool CudaComputeEngine::_CreateContext ()
	{
		using namespace cuda;

		CU_CHECK( cuCtxCreate( &_context, 0, _device ) );

		_CheckSharedGraphicsDevices();
		return true;
	}

# if defined(GRAPHICS_API_OPENGL)
/*
=================================================
	_CheckSharedGraphicsDevices
=================================================
*/
	void CudaComputeEngine::_CheckSharedGraphicsDevices ()
	{
		using namespace cuda;

		uint		dev_count		= 0;
		CUdevice	devices[128]	= {};

		CU_CALL( cuGLGetDevices( &dev_count, devices, CountOf( devices ), CU_GL_DEVICE_LIST_ALL ) );

		_isShared = dev_count > 0;
	}
# endif

/*
=================================================
	_WriteInfo
=================================================
*/
	void CudaComputeEngine::_WriteInfo ()
	{
		using namespace cuda;
		

		String	log;
		char	name[ 128 ]	= {};
		
		int		clock_rate;
		int		processors;
		int		mem_clock;
		int		mem_bus;
		int		cap_major;
		int		cap_minor;
		int		l2_cache;
		int		total_cmem;
		int		max_grid[3];
		int		smem_per_block;
		int		reg_per_block;
		int		threads_pb;
		int		block_dim[3];
		int		threads_pmp;
		int		smem_pmp;
		int		reg_pmp;

		CU_CALL( cuDeviceGetName( name, CountOf( name ), _device ) );

		CU_CALL( cuDeviceGetAttribute( &clock_rate,		CU_DEVICE_ATTRIBUTE_CLOCK_RATE,								_device ) );
		CU_CALL( cuDeviceGetAttribute( &processors,		CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT,					_device ) );
		CU_CALL( cuDeviceGetAttribute( &mem_clock,		CU_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE,						_device ) );
		CU_CALL( cuDeviceGetAttribute( &mem_bus,		CU_DEVICE_ATTRIBUTE_GLOBAL_MEMORY_BUS_WIDTH,				_device ) );
		CU_CALL( cuDeviceGetAttribute( &cap_major,		CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR,				_device ) );
		CU_CALL( cuDeviceGetAttribute( &cap_minor,		CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR,				_device ) );
		CU_CALL( cuDeviceGetAttribute( &l2_cache,		CU_DEVICE_ATTRIBUTE_L2_CACHE_SIZE,							_device ) );
		CU_CALL( cuDeviceGetAttribute( &total_cmem,		CU_DEVICE_ATTRIBUTE_TOTAL_CONSTANT_MEMORY,					_device ) );
		CU_CALL( cuDeviceGetAttribute( &max_grid[0],	CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_X,							_device ) );
		CU_CALL( cuDeviceGetAttribute( &max_grid[1],	CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Y,							_device ) );
		CU_CALL( cuDeviceGetAttribute( &max_grid[2],	CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Z,							_device ) );
		CU_CALL( cuDeviceGetAttribute( &smem_per_block,	CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK,			_device ) );
		CU_CALL( cuDeviceGetAttribute( &reg_per_block,	CU_DEVICE_ATTRIBUTE_MAX_REGISTERS_PER_BLOCK,				_device ) );
		CU_CALL( cuDeviceGetAttribute( &threads_pb,		CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK,					_device ) );
		CU_CALL( cuDeviceGetAttribute( &block_dim[0],	CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_X,						_device ) );
		CU_CALL( cuDeviceGetAttribute( &block_dim[1],	CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Y,						_device ) );
		CU_CALL( cuDeviceGetAttribute( &block_dim[2],	CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Z,						_device ) );
		CU_CALL( cuDeviceGetAttribute( &threads_pmp,	CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_MULTIPROCESSOR,			_device ) );
		CU_CALL( cuDeviceGetAttribute( &smem_pmp,		CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_MULTIPROCESSOR,	_device ) );
		CU_CALL( cuDeviceGetAttribute( &reg_pmp,		CU_DEVICE_ATTRIBUTE_MAX_REGISTERS_PER_MULTIPROCESSOR,		_device ) );

		log << "\n---------------\nCUDA\n---------------"
			<< "\nName:         " << name
			<< "\nVersion:      " << cap_major << '.' << cap_minor
			<< "\nProcessors:   " << processors
			<< "\nFrequency:    " << (clock_rate >> 10) << " MHz"
			<< "\nMem freq:     " << (mem_clock >> 10) << " MHz"
			<< "\nMem bus:      " << Bits<usize>( mem_bus ).ToString()
			<< "\nL2 cache:     " << Bytes<usize>( l2_cache ).ToString()
			<< "\nConst mem:    " << Bytes<usize>( total_cmem ).ToString()
			<< "\nGlobal mem:   " << Bytes<ulong>( _totalMemory ).ToString()
			<< "\nMax grid:     " << max_grid[0] << ", " << max_grid[1] << ", " << max_grid[2]
			<< "\nPer block:"
			<< "\n-Shared mem:  " << Bytes<usize>( smem_per_block ).ToString()
			<< "\n-Threads:     " << threads_pb
			<< "\n-Thread dim:  " << block_dim[0] << ", " << block_dim[1] << ", " << block_dim[2]
			<< "\n-Registers:   " << reg_per_block
			<< "\nPer processor:"
			<< "\n-Shared mem:  " << Bytes<usize>( smem_pmp ).ToString()
			<< "\n-Threads:     " << threads_pmp
			<< "\n-Registers:   " << reg_pmp
			<< "\n---------------\n";

		LOG( log.cstr(), ELog::DEBUG );
	}
	
/*
=================================================
	_CreateQueue
=================================================
*/
	bool CudaComputeEngine::_CreateQueue ()
	{
		using namespace cuda;

		CU_CHECK( cuStreamCreate ( &_stream, 0 ) );
		return true;
	}
	
/*
=================================================
	_CheckVersion
=================================================
*/
	bool CudaComputeEngine::_CheckVersion ()
	{
		using namespace cuda;

		int	version = 0;

		CU_CALL( cuDriverGetVersion( &version ) );
		CHECK_ERR( version >= MIN_VERSION );

		CU_CALL( cuCtxGetApiVersion( _context, (uint *)&version ) );
		CHECK_ERR( version >= MIN_VERSION );

		_version = version;
		return true;
	}
	
/*
=================================================
	GetFreeMemory
=================================================
*/
	Bytes<ulong>  CudaComputeEngine::GetFreeMemory () const
	{
		using namespace cuda;

		usize	free_mem_size = 0;

		CU_CALL( cuMemGetInfo( &free_mem_size, null ) );

		return Bytes<ulong>( free_mem_size );
	}
	
/*
=================================================
	Finish
=================================================
*/
	void CudaComputeEngine::Finish ()
	{
		using namespace cuda;

		CU_CALL( cuCtxSynchronize() );
	}
	
/*
=================================================
	Barrier
=================================================
*/
	void CudaComputeEngine::Barrier ()
	{
		using namespace cuda;

		CU_CALL( cuStreamSynchronize( _stream ) );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_CUDA
