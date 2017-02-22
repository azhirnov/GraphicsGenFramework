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
	
	class   GL4ComputeFunction;
	typedef GL4ComputeFunction		ComputeFunction;

	SHARED_POINTER( ComputeFunction );



	//
	// Compute Function
	//

	class GL4ComputeFunction : public BaseObject
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
				//AtomicCounterBuffer,
				StorageBuffer,
				UniformBuffer,
				_Count
			};

		// variables
			ArgValue_t		value;
			uint			index;
			EType			type;
			uint			dataSize;
			bool			writeAccess;

		// methods
			Arg () :
				index(0), type(Unknown), dataSize(0), writeAccess(false)
			{}

			Arg (uint index, EType type, uint dataSize = 0, bool writeAccess = false) :
				index(index), type(type), dataSize(dataSize), writeAccess(writeAccess)
			{}
		};

		typedef StaticString< 64 >				StString_t;
		typedef HashMap< StString_t, Arg >		Arguments_t;


	// variables
	private:
		//ComputeProgramPtr	_program;
		Arguments_t						_args;
		ProgramID						_progID;
		uint3							_fixedGroupSize;
		EShaderCompilationFlags::type	_flags;


	// methods
	public:
		explicit
		GL4ComputeFunction (const SubSystemsRef ss);
		~GL4ComputeFunction ();

		// for OpenCL compatibility
		bool Create (const ComputeProgramPtr &program, StringCRef name);
		void Destroy ();

		bool Create (StringCRef source, EShaderCompilationFlags::type flags);
		bool Load (StringCRef filename, EShaderCompilationFlags::type flags);

		void Run (const uint3 &size, const uint3 &localSize = Uninitialized);
		void Run (const ulong3 &size, const ulong3 &localSize = Uninitialized);


		template <typename T>
		Self& SetArg (StringCRef name, const T& value);
		
		template <typename T>
		Self& SetArg (StringCRef name, const Quaternion<T> &value);

		template <typename T>
		Self& SetArg (StringCRef name, const Radians<T> &value);
		
		template <typename T>
		bool TrySetArg (StringCRef name, const T& value);

		bool HasArg (StringCRef name) const;
		
		void GetArgNames (OUT Array<StringCRef> &names) const;

		void ResetArgs ();


		bool	IsCreated ()	const	{ return _progID.IsValid(); }
		usize	NumArgs ()		const	{ return _args.Count(); }


	private:
		void _RunFixedGroupSize (const ulong3 &size);
		void _RunVariableGroupSize (const ulong3 &size, const ulong3 &localSize);

		void _GetFixedGroupSize ();

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
	
/*
=================================================
	TrySetArg
=================================================
*/
	template <typename T>
	bool GL4ComputeFunction::TrySetArg (StringCRef name, const T& value)
	{
		if ( HasArg( name ) )
		{
			SetArg( name, value );
			return true;
		}
		return false;
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
