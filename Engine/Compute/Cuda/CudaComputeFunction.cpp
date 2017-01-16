// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "CudaComputeFunction.h"
#include "../Engine/ComputeEngine.h"

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
	CudaComputeFunction::CudaComputeFunction () :
		_id( null )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	CudaComputeFunction::~CudaComputeFunction ()
	{
		_Delete();
	}
	
/*
=================================================
	Create
=================================================
*/
	bool CudaComputeFunction::Create (const ComputeProgramPtr &program, StringCRef name)
	{
		CHECK_ERR( _Create( program, name ) );
		return true;
	}
	
/*
=================================================
	Run
=================================================
*/
	void CudaComputeFunction::Run (const uint3 &size, const uint3 &localSize)
	{
		using namespace cuda;

		CHECK( IsCreated() );
		
		Ptr< ComputeEngine >	engine = _program->SubSystems()->Get< ComputeEngine >();

		KernelArgs_t	args;
		uint3			block_dim	= Max( localSize, uint3(1) );
		uint3			grid_dim	= Max( size, uint3(1) ) / block_dim;

		CHECK( All( grid_dim * block_dim == size ) );

		_SetArgs( args );
		_AcquireObjects( engine->GetStream() );

		CU_CALL( cuLaunchKernel(
			_id,
			grid_dim.x,  grid_dim.y,  grid_dim.z,
			block_dim.x, block_dim.y, block_dim.y,
			0,
			engine->GetStream(),
			args.ptr(),
			null ) );
		
		_ReleaseObjects( engine->GetStream() );

		engine->Barrier();
	}
	
/*
=================================================
	_Create
=================================================
*/
	bool CudaComputeFunction::_Create (const ComputeProgramPtr &program, StringCRef name)
	{
		using namespace cuda;
		
		_Delete();

		CHECK_ERR( program.IsNotNull() );

		CU_CHECK( cuModuleGetFunction( &_id, program->Id(), name.cstr() ) );

		_name		= name;
		_program	= program;
		return true;
	}
	
/*
=================================================
	_Delete
=================================================
*/
	void CudaComputeFunction::_Delete ()
	{
		_name.Clear();

		_id			= null;
		_program	= null;
	}
	
/*
=================================================
	_InitArgs
=================================================
*
	void CudaComputeFunction::_InitArgs (StringCRef kernelName)
	{
		StString_t	arg_name;
		
		arg_name << "in_" << kernelName;

		const usize	len	= arg_name.Length();


	}
	
/*
=================================================
	CudaComputeFunction_SetArg
=================================================
*/
	struct CudaComputeFunction_SetArg
	{
		const StringCRef					argName;
		const cuda::CUmodule				module;
		const cuda::CUfunction				func;
		CudaComputeFunction::KernelArgs_t&	kernelArgs;
		const uint							index;
		const bool							isArg;

		CudaComputeFunction_SetArg (cuda::CUmodule module, cuda::CUfunction func, StringCRef name,
									CudaComputeFunction::KernelArgs_t& kernelArgs, uint index) :
			argName(name), module(module), func(func), index(index),
			isArg(index != -1), kernelArgs(kernelArgs)
		{}

		template <typename T>
		void operator () (const T& value) const
		{
			CHECK_ERR( isArg, void() );

			kernelArgs[ index ] = (void *) &value;
		}

		void operator () (const ComputeBufferPtr &value) const
		{
			using namespace cuda;

			CHECK( not isArg );
			
			CUdeviceptr	dev_ptr = 0;
			usize		size_of	= 0;

			CU_CALL( cuModuleGetGlobal( &dev_ptr, &size_of, module, argName.cstr() ) );

			value->Bind( dev_ptr, size_of );
		}

		void operator () (const ComputeTexturePtr &value) const
		{
			using namespace cuda;

			CHECK_ERR( not isArg, void() );

			CUtexref	texref = null;
			CU_CALL( cuModuleGetTexRef( &texref, module, argName.cstr() ) );

			value->Bind( texref );
			
			CU_CALL( cuParamSetTexRef( func, CU_PARAM_TR_DEFAULT, texref ) );	// TODO: this func is deprecated
		}

		void operator () (const ComputeImagePtr &value) const
		{
			using namespace cuda;

			CHECK_ERR( not isArg, void() );

			CUtexref	texref = null;
			CU_CALL( cuModuleGetTexRef( &texref, module, argName.cstr() ) );

			value->Bind( texref );
			
			CU_CALL( cuParamSetTexRef( func, CU_PARAM_TR_DEFAULT, texref ) );	// TODO: this func is deprecated
		}
	};
	
/*
=================================================
	_SetArgs
=================================================
*/
	void CudaComputeFunction::_SetArgs (OUT KernelArgs_t &kernelArgs)
	{
		Ptr< const CudaComputeProgram::KernelInfo >		kernel;

		uint	num_args = 0;

		CHECK( _program->_GetKernel( _name, kernel, num_args ) );

		kernelArgs.Resize( num_args, false );

		FOR( i, _args )
		{
			Arguments_t::cpair_t const &	arg		= _args[i];
			uint							index	= 0;
			
			bool	is_arg		= _program->_HasKernelArg( kernel, arg.first, index );
			bool	is_global	= not is_arg and _program->_HasGlobal( arg.first );

			CHECK( is_arg or is_global );

			CudaComputeFunction_SetArg	func( _program->Id(), _id, arg.first, kernelArgs, index );
			
			arg.second.Apply( func );
		}
	}

# if defined(GRAPHICS_API_OPENGL)
/*
=================================================
	_AcquireObjects
=================================================
*/
	void CudaComputeFunction::_AcquireObjects (cuda::CUstream stream)
	{
		using namespace cuda;
	}
	
/*
=================================================
	_ReleaseObjects
=================================================
*/
	void CudaComputeFunction::_ReleaseObjects (cuda::CUstream stream)
	{
		using namespace cuda;
	}

# endif	// GRAPHICS_API_OPENGL


}	// Compute
}	// Engine

#endif	// COMPUTE_API_CUDA
