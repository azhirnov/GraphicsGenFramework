// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CL2ComputeProgram.h"
#include "CL2ComputeImage.h"
#include "CL2ComputeBuffer.h"

#if defined( COMPUTE_API_OPENCL )

namespace Engine
{
namespace Compute
{

	class   CL2ComputeFunction;
	typedef CL2ComputeFunction		ComputeFunction;



	//
	// Compute Function
	//

	class CL2ComputeFunction : public Noncopyable
	{
	// types
	public:
		typedef CL2ComputeFunction		Self;

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

		typedef StaticString< 64 >								StString_t;

		struct Arg
		{
		// variables
			ArgValue_t	value;
			uint		index;

		// methods
			Arg (uint index = -1) : index(index) {}
		};

		typedef HashMap< StString_t, Arg >		Arguments_t;

		typedef Array< cl::cl_mem >				GraphicsObjects_t;


	// variables
	private:
		ComputeProgramPtr	_program;
		Arguments_t			_args;
		GraphicsObjects_t	_objs;
		cl::cl_kernel		_id;
		usize				_preferredMultipleOfWorkGroupSize;


	// methods
	public:
		CL2ComputeFunction ();
		~CL2ComputeFunction ();


		bool Create (const ComputeProgramPtr &program, StringCRef name);
		void Destroy ();

		void Run (const uint3 &size, const uint3 &localSize = Uninitialized());


		template <typename T>
		Self& SetArg (StringCRef name, const T& value);
		
		template <typename T>
		Self& SetArg (StringCRef name, const Vec<T,3> &value);

		template <typename T>
		Self& SetArg (StringCRef name, const Quaternion<T> &value);

		template <typename T>
		Self& SetArg (StringCRef name, const Radians<T> &value);

		bool HasArg (StringCRef name) const;

		void GetArgNames (OUT Array<StringCRef> &names) const;


		bool	IsCreated ()	const	{ return _program.IsNotNull() and _id != null; }
		usize	NumArgs ()		const	{ return _args.Count(); }


	private:
		bool _Create (const ComputeProgramPtr &program, StringCRef name);
		void _Delete ();
		void _PrintInfo (StringCRef name);
		void _InitArgs ();
		void _SetArgs ();
		void _CheckArgs ();
		bool _ParseKernelArgs (StringCRef source, usize pos);

		//uint3 _CalcOptimalLocalSize (const uint3 &size) const;

		void _AcquireObjects (cl::cl_command_queue cmdQueue);
		bool _ReleaseObjects (cl::cl_command_queue cmdQueue);
	};
	
	
	
/*
=================================================
	HasArg
=================================================
*/
	inline bool CL2ComputeFunction::HasArg (StringCRef name) const
	{
		return _args.IsExist( name );
	}
	
/*
=================================================
	GetArgNames
=================================================
*/
	inline void CL2ComputeFunction::GetArgNames (OUT Array<StringCRef> &names) const
	{
		names.Clear();
		names.Reserve( _args.Count() );

		FOR( i, _args )
		{
			names.PushBack( _args[i].first );
		}
	}

/*
=================================================
	SetArg
=================================================
*/
	template <typename T>
	inline CL2ComputeFunction &  CL2ComputeFunction::SetArg (StringCRef name, const T& value)
	{
		Arguments_t::iterator	result;

		CHECK_ERR( _args.Find( name, result ), *this );

		result->second.value.Create( value );
		return *this;
	}

	template <typename T>
	inline CL2ComputeFunction &  CL2ComputeFunction::SetArg (StringCRef name, const Vec<T,3> &value)
	{
		return SetArg( name, Vec<T,4>( value ) );
	}
	
	template <typename T>
	inline CL2ComputeFunction &  CL2ComputeFunction::SetArg (StringCRef name, const Quaternion<T> &value)
	{
		return SetArg( name, value.ToVec() );
	}
	
	template <typename T>
	inline CL2ComputeFunction &  CL2ComputeFunction::SetArg (StringCRef name, const Radians<T> &value)
	{
		return SetArg( name, (T)value );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENCL
