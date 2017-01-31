// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "GL4ComputeProgram.h"
#include "GL4ComputeImage.h"
#include "GL4ComputeBuffer.h"

#if defined( COMPUTE_API_OPENGL )

namespace Engine
{
namespace Compute
{

	//
	// Compute Function
	//

	class GL4ComputeFunction : public Noncopyable
	{
	// types
	public:
		typedef GL4ComputeFunction		Self;

	private:
		typedef Union<	int, int2, int3, int4,
						uint, uint2, uint3, uint4,
						ilong, ilong2, ilong3, ilong4,
						ulong, ulong2, ulong3, ulong4,
						float, float2, float3, float4,
						float2x2, float3x3, float4x4,
						double, double2, double3, double4,
						double2x2, double3x3, double4x4,
						ComputeBufferPtr, ComputeImagePtr,
						TexturePtr, MemoryBufferPtr,
						VertexBufferPtr, IndexBufferPtr >		ArgValue_t;

		struct _SetArgFunc;

		struct Arg
		{
		// types
			enum EType
			{
				Unknown = 0,
				Uniform,
				Image,
				Texture,
				AtomicCounterBuffer,
				StorageBuffer,
				UniformBuffer,
				_Count
			};

		// variables
			ArgValue_t		value;
			uint			index;
			EType			type;
			bool			writeAccess;

		// methods
			Arg () : index(0), type(Unknown), writeAccess(false)
			{}

			Arg (uint index, EType type, bool writeAccess = false) : index(index), type(type), writeAccess(writeAccess)
			{}
		};

		typedef StaticString< 64 >				StString_t;
		typedef HashMap< StString_t, Arg >		Arguments_t;


	// variables
	private:
		ComputeProgramPtr	_program;
		Arguments_t			_args;
		ProgramID			_progID;
		uint3				_fixedGroupSize;


	// methods
	public:
		GL4ComputeFunction ();
		~GL4ComputeFunction ();

		// for OpenCL compatibility
		bool Create (const ComputeProgramPtr &program, StringCRef name);
		void Destroy ();

		bool Load (const SubSystemsRef ss, StringCRef filename, EShaderCompilationFlags::type flags);

		void Run (const uint3 &size, const uint3 &localSize = Uninitialized);


		template <typename T>
		Self& SetArg (StringCRef name, const T& value);
		
		template <typename T>
		Self& SetArg (StringCRef name, const Quaternion<T> &value);

		template <typename T>
		Self& SetArg (StringCRef name, const Radians<T> &value);

		bool HasArg (StringCRef name) const;
		
		void GetArgNames (OUT Array<StringCRef> &names) const;

		void ResetArgs ();


		bool	IsCreated ()	const	{ return _program and _progID.IsValid(); }
		usize	NumArgs ()		const	{ return _args.Count(); }


	private:
		void _RunFixedGroupSize (const uint3 &size);
		void _RunVariableGroupSize (const uint3 &size, const uint3 &localSize);

		void _InitArgs ();
		void _SetArgs ();
		void _CheckArgs ();
	};
	
	
/*
=================================================
	HasArg
=================================================
*/
	inline bool GL4ComputeFunction::HasArg (StringCRef name) const
	{
		return _args.IsExist( name );
	}
	
/*
=================================================
	GetArgNames
=================================================
*/
	inline void GL4ComputeFunction::GetArgNames (OUT Array<StringCRef> &names) const
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
	inline GL4ComputeFunction &  GL4ComputeFunction::SetArg (StringCRef name, const T& value)
	{
		Arguments_t::iterator	result;

		CHECK_ERR( _args.Find( name, OUT result ), *this );

		result->second.value.Create( value );
		return *this;
	}

	template <typename T>
	inline GL4ComputeFunction & GL4ComputeFunction::SetArg (StringCRef name, const Quaternion<T> &value)
	{
		return SetArg( name, value.xyzw() );
	}
	
	template <typename T>
	inline GL4ComputeFunction & GL4ComputeFunction::SetArg (StringCRef name, const Radians<T> &value)
	{
		return SetArg( name, (T)value );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
