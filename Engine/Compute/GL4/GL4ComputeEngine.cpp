// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "GL4ComputeEngine.h"

#if defined( COMPUTE_API_OPENGL )

namespace Engine
{
namespace Compute
{
	
/*
=================================================
	constructor
=================================================
*/
	GL4ComputeEngine::GL4ComputeEngine (const SubSystemsRef ss) :
		BaseObject( ss ),
		_maxInvocations( 0 ),
		_variableGroupSizeSupported( 0 )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	GL4ComputeEngine::~GL4ComputeEngine ()
	{
	}
	
/*
=================================================
	Initialize
=================================================
*/
	bool GL4ComputeEngine::Initialize (StringCRef deviceName)
	{
		using namespace gl;

		CHECK_ERR( deviceName.Empty() );	// device choosing not supported
		
		CHECK_ERR( GL4_GetVersion() >= COMPUTE_API_OPENGL );

		_CalcTotalMemory();
		_GetMaxWorkGroups();
		_GetLocalGroupSize();
		_CheckIsVariableGroupSizeSupported();
		
		_PrintInfo ();
		return true;
	}
	
/*
=================================================
	_PrintInfo
=================================================
*/
	void GL4ComputeEngine::_PrintInfo ()
	{
		String log;

		log	<< "OpenGL Compute info\n---------------"
			<< "\nTotal memory:     " << ToString( _totalMemory )
			<< "\nFree memory:      " << ToString( GetAvailableMemory() )
			<< "\nMax groups:       " << ToString( _maxWorkGroupCount )
			<< "\nMax local groups: " << ToString( _maxLocalGroupSize )
			<< "\nMax invocations:  " << _maxInvocations;
			
		LOG( log.cstr(), ELog::Debug | ELog::SpoilerFlag );
	}

/*
=================================================
	_CalcTotalMemory
=================================================
*/
	void GL4ComputeEngine::_CalcTotalMemory ()
	{
		using namespace gl;

		switch ( SubSystems()->Get< GraphicsEngine >()->VideoCardVendor() )
		{
			case EVideoCardVendor::NVidia :
			{
				const int GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX = 0x9048;
				
				GLint total_mem_kb = 0;
				GL_CALL( glGetIntegerv( GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb ) );

				_totalMemory.SetKb( total_mem_kb );
				break;
			}

#			if 0 //defined(PLATFORM_WINDOWS) and not defined(PLATFORM_SDL)
			case EVideoCardVendor::AMD :
			{
				using namespace WinPlatform;

				PFNWGLGETGPUIDSAMDPROC	wglGetGPUIDsAMD  = (PFNWGLGETGPUIDSAMDPROC) wglGetProcAddress("wglGetGPUIDsAMD");
				PFNWGLGETGPUINFOAMDPROC	wglGetGPUInfoAMD = (PFNWGLGETGPUINFOAMDPROC) wglGetProcAddress("wglGetGPUInfoAMD");

				if ( wglGetGPUIDsAMD != null and wglGetGPUInfoAMD != null )
				{
					uint	ids[1]			= {};
					//uint	n				= wglGetGPUIDsAMD(0, 0);
					size_t	total_mem_mb	= 0;

					wglGetGPUIDsAMD( 1, ids );
					wglGetGPUInfoAMD( ids[0], WGL_GPU_RAM_AMD, GL_UNSIGNED_INT, sizeof(size_t), &total_mem_mb );

					_totalMemory.SetMb( total_mem_mb );
					break;
				}
			}
#			endif	// PLATFORM_WINDOWS and not PLATFORM_SDL

			default:
			{
				LOG( "Total GPU memory not available for this GPU type", ELog::Debug );
				_totalMemory = Bytes<ulong>();
			}
		}
	}
	
/*
=================================================
	_GetLocalGroupSize
=================================================
*/
	void GL4ComputeEngine::_GetLocalGroupSize ()
	{
		using namespace gl;

		GLint	group_size[3]	= {0};
		GLint	invocations		= 0;

		GL_CALL( glGetIntegeri_v( GL_MAX_COMPUTE_VARIABLE_GROUP_SIZE_ARB, 0, &group_size[0] ) );
		GL_CALL( glGetIntegeri_v( GL_MAX_COMPUTE_VARIABLE_GROUP_SIZE_ARB, 1, &group_size[1] ) );
		GL_CALL( glGetIntegeri_v( GL_MAX_COMPUTE_VARIABLE_GROUP_SIZE_ARB, 2, &group_size[2] ) );
		GL_CALL( glGetIntegerv( GL_MAX_COMPUTE_VARIABLE_GROUP_INVOCATIONS_ARB, &invocations ) );

		_maxLocalGroupSize	= ulong3( group_size[0], group_size[1], group_size[2] );
		_maxInvocations		= invocations;
	}
	
/*
=================================================
	_GetMaxWorkGroups
=================================================
*/
	void GL4ComputeEngine::_GetMaxWorkGroups ()
	{
		using namespace gl;

		GLint	max_groups[3]	= {0};

		GL_CALL( glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_groups[0] ) );
		GL_CALL( glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &max_groups[1] ) );
		GL_CALL( glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &max_groups[2] ) );

		_maxWorkGroupCount	= ulong3( max_groups[0], max_groups[1], max_groups[2] );
	}
	
/*
=================================================
	_CheckIsVariableGroupSizeSupported
=================================================
*/
	void GL4ComputeEngine::_CheckIsVariableGroupSizeSupported ()
	{
		_variableGroupSizeSupported = SubSystems()->Get< GraphicsEngine >()->
										GetContext()->IsExtensionSupported( "GL_ARB_compute_variable_group_size" );
	}

/*
=================================================
	GetAvailableMemory
=================================================
*/
	Bytes<ulong> GL4ComputeEngine::GetAvailableMemory () const
	{
		using namespace gl;

		switch ( SubSystems()->Get< GraphicsEngine >()->VideoCardVendor() )
		{
			case EVideoCardVendor::NVidia :
			{
				static const int GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX = 0x9049;
				
				GLint cur_avail_mem_kb = 0;
				GL_CALL( glGetIntegerv( GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb ) );

				return Bytes<ulong>::FromKb( cur_avail_mem_kb );
			}

			case EVideoCardVendor::AMD :
			{
				static const int GL_TEXTURE_FREE_MEMORY_ATI = 0x87FC;
				
				// param[0] - total memory free in the pool
				// param[1] - largest available free block in the pool
				// param[2] - total auxiliary memory free
				// param[3] - largest auxiliary free block
				GLint	free_mem[4] = {0};
				GL_CALL( glGetIntegerv( GL_TEXTURE_FREE_MEMORY_ATI, free_mem ) );
			
				return Bytes<ulong>::FromKb( free_mem[0] );
			}
		}
		return Bytes<ulong>();
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void GL4ComputeEngine::Deinitialize ()
	{
	}
	
/*
=================================================
	Synchronize
=================================================
*/
	void GL4ComputeEngine::Synchronize ()
	{
		SubSystems()->Get< GraphicsEngine >()->GetStateManager()->Synchronize();
	}
	
/*
=================================================
	Barrier
=================================================
*/
	void GL4ComputeEngine::Barrier ()
	{
		Ptr<StateManager>	sm = SubSystems()->Get< GraphicsEngine >()->GetStateManager();
		MemoryBarrierObj	barrier;

		sm->CreateBarrier( OUT barrier );
		sm->MemoryBarrier( barrier, EMemoryBarrier::All );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
