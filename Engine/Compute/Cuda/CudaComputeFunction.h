// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CudaComputeProgram.h"
#include "CudaComputeBuffer.h"
#include "CudaComputeTexture.h"
#include "CudaComputeImage.h"

#if defined( COMPUTE_API_CUDA )

namespace Engine
{
namespace Compute
{
	
	class	CudaComputeFunction;
	typedef CudaComputeFunction		ComputeFunction;


	//
	// Compute Function
	//

	class CudaComputeFunction
	{
	// types
	public:
		typedef CudaComputeFunction		Self;

		typedef FixedSizeArray< void*, 128 >		KernelArgs_t;

	private:

		typedef Union<	int, int2, int3, int4,
						uint, uint2, uint3, uint4,
						ilong, ilong2, ilong3, ilong4,
						ulong, ulong2, ulong3, ulong4,
						float, float2, float3, float4,
						float2x2, float3x3, float4x4,
						double, double2, double3, double4,
						double2x2, double3x3, double4x4,
						ComputeBufferPtr, ComputeImagePtr >		ArgValue_t;

		typedef StaticString< char, 64 >						StString_t;
		typedef Map< StString_t, ArgValue_t >					Arguments_t;


	// variables
	private:
		ComputeProgramPtr	_program;
		Arguments_t			_args;
		StString_t			_name;
		cuda::CUfunction	_id;


	// methods
	public:
		CudaComputeFunction ();
		~CudaComputeFunction ();

		template <typename T>
		Self& SetArg (StringCRef name, const T& value);
		
		bool Create (const ComputeProgramPtr &program, StringCRef name);

		void Run (const uint3 &size, const uint3 &localSize);
		

		bool	IsCreated ()	const	{ return _program.IsNotNull() and _id != null; }
		usize	NumArgs ()		const	{ return _args.Count(); }


	private:
		bool _Create (const ComputeProgramPtr &program, StringCRef name);
		void _Delete ();
		void _SetArgs (OUT KernelArgs_t &kernelArgs);

		void _AcquireObjects (cuda::CUstream stream);
		void _ReleaseObjects (cuda::CUstream stream);
	};
	


/*
=================================================
	SetArg
=================================================
*/
	template <typename T>
	inline CudaComputeFunction &  CudaComputeFunction::SetArg (StringCRef name, const T& value)
	{
		_args.AddOrReplace( name, ArgValue_t( value ) );
		return *this;
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_CUDA
