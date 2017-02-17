// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "CL2ComputeFunction.h"

#if defined( COMPUTE_API_OPENCL )

#include "Engine/Compute/Engine/ComputeEngine.h"

namespace Engine
{
namespace Compute
{
	
/*
=================================================
	constructor
=================================================
*/
	CL2ComputeFunction::CL2ComputeFunction (const SubSystemsRef ss) :
		BaseObject( ss ),
		_id( null ), _preferredMultipleOfWorkGroupSize( 1 )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	CL2ComputeFunction::~CL2ComputeFunction ()
	{
		_Delete();
	}
	
/*
=================================================
	Create
=================================================
*/
	bool CL2ComputeFunction::Create (const ComputeProgramPtr &program, StringCRef name)
	{
		CHECK_ERR( _Create( program, name ) );
		_InitArgs();
		_PrintInfo( name );
		return true;
	}
		
/*
=================================================
	Load
=================================================
*/
	bool CL2ComputeFunction::Load (StringCRef filename, EProgramUnitFlags::type flags)
	{
		ComputeProgramPtr	prog = New<ComputeProgram>( SubSystems() );

		CHECK_ERR( prog->Create( filename, flags ) );
		CHECK_ERR( Create( prog, "main" ) );

		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void CL2ComputeFunction::Destroy ()
	{
		using namespace cl;

		_preferredMultipleOfWorkGroupSize = 0;

		_program = null;

		_args.Clear();
		_objs.Clear();

		if ( _id == null )
			return;

		CL_CALL( clReleaseKernel( _id ) );
		_id = null;
	}

/*
=================================================
	Run
=================================================
*/
	void CL2ComputeFunction::Run (const ulong3 &size, const ulong3 &localSize)
	{
		using namespace cl;
		
		CHECK( IsCreated() );

		Ptr< ComputeEngine >	comp_eng	= SubSystems()->Get< ComputeEngine >();

		CHECK( All( localSize <= comp_eng->GetMaxLocalGroupSize() ) );
		CHECK( All( size <= comp_eng->GetMaxThreads() ) );

		const cl_uint	work_dim	= Max( (size[0] > 1 ? 1 : 0), (size[1] > 1 ? 2 : 0), (size[2] > 1 ? 3 : 0), 1 );

		const usize3	global_size	= Max( size.To<usize3>(), usize3(1) );
		const usize3	local_size	= Max( localSize.To<usize3>(), usize3(1) );

		Ptr< ComputeEngine >	engine = SubSystems()->Get< ComputeEngine >();

		CHECK( All( global_size % local_size == usize3(0) ) );

		DEBUG_ONLY( _CheckArgs() );

		_AcquireObjects( engine->GetCommandQueue() );
		_SetArgs();

		CL_CALL( clEnqueueNDRangeKernel(
					engine->GetCommandQueue(),
					_id,
					work_dim,
					null,
					global_size.ptr(),
					local_size.ptr(),
					0,
					null,
					null ) );

		if ( not _ReleaseObjects( engine->GetCommandQueue() ) )
			engine->Barrier();
	}

	void CL2ComputeFunction::Run (const uint3 &size, const uint3 &localSize)
	{
		return Run( ulong3(size), ulong3(localSize) );
	}

/*
=================================================
	_Create
=================================================
*/
	bool CL2ComputeFunction::_Create (const ComputeProgramPtr &program, StringCRef name)
	{
		using namespace cl;

		_Delete();

		CHECK_ERR( program );
		
		cl_int	cl_err = 0;
		CL_CHECK( ((_id = clCreateKernel( program->Id(), name.cstr(), &cl_err )), cl_err) );

		_program = program;
		return true;
	}
	
/*
=================================================
	_Delete
=================================================
*/
	void CL2ComputeFunction::_Delete ()
	{
		using namespace cl;

		_args.Clear();

		if ( _id == null )
			return;

		CL_CALL( clReleaseKernel( _id ) );
		_id = null;
	}

/*
=================================================
	_CalcOptimalLocalSize
=================================================
*
	uint3 CL2ComputeFunction::_CalcOptimalLocalSize (const uint3 &size) const
	{

	}

/*
=================================================
	_PrintInfo
=================================================
*/
	void CL2ComputeFunction::_PrintInfo (StringCRef name)
	{
		using namespace cl;
		
		Ptr< ComputeEngine >	engine = SubSystems()->Get< ComputeEngine >();

		usize3		group_size;
		cl_ulong	local_mem	= 0;
		cl_ulong	priv_mem	= 0;
		size_t		pref_size	= 0;

		CL_CALL( clGetKernelWorkGroupInfo(	_id,
											engine->GetDevice(),
											CL_KERNEL_COMPILE_WORK_GROUP_SIZE,
											sizeof(group_size),
											group_size.ptr(),
											null ) );
		
		CL_CALL( clGetKernelWorkGroupInfo(	_id,
											engine->GetDevice(),
											CL_KERNEL_LOCAL_MEM_SIZE,
											sizeof(local_mem),
											&local_mem,
											null ) );
		
		CL_CALL( clGetKernelWorkGroupInfo(	_id,
											engine->GetDevice(),
											CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
											sizeof(pref_size),
											&pref_size,
											null ) );

		CL_CALL( clGetKernelWorkGroupInfo(	_id,
											engine->GetDevice(),
											CL_KERNEL_PRIVATE_MEM_SIZE,
											sizeof(priv_mem),
											&priv_mem,
											null ) );

		String	log;

		log << "Kernel \"" << name << "\" info\n---------------"
			<< "\nKernel work group:    " << ToString( group_size )
			<< "\nPreferred work group: " << pref_size
			<< "\nKernel local mem:     " << ToString( Bytes<ulong>( local_mem ) )
			<< "\nKernel private mem:   " << ToString( Bytes<ulong>( priv_mem ) );

		LOG( log.cstr(), ELog::Debug | ELog::SpoilerFlag );

		_preferredMultipleOfWorkGroupSize = pref_size;
	}

/*
=================================================
	_InitArgs
----
	for OpenCL 2.0 and highter
=================================================
*/
# if COMPUTE_API_OPENCL >= 200

	void CL2ComputeFunction::_InitArgs ()
	{
		using namespace cl;
		
		if ( _id == null )
			return;

		cl_uint	num_args = 0;
		char	buf[ 128 ] = {};

		CL_CALL( clGetKernelInfo( _id, CL_KERNEL_NUM_ARGS, sizeof(num_args), &num_args, null ) );
		
		_args.Reserve( num_args );
		_objs.Reserve( num_args );

		for (uint i = 0; i < num_args; ++i)
		{
			CL_CALL( clGetKernelArgInfo( _id, i, CL_KERNEL_ARG_NAME, CountOf(buf), buf, null ) );

			_args.Add( StringCRef( buf ), Arg( i ) );
		}
	}

# endif
	
/*
=================================================
	_InitArgs
----
	for OpenCL 1.1 and 1.2
	TODO: add support for / * * /
=================================================
*/
# if COMPUTE_API_OPENCL < 200

	bool CL2ComputeFunction::_ParseKernelArgs (StringCRef source, usize pos)
	{
		Array< StringCRef >	tokens;
		StringCRef			line;
		
		StringParser::ReadLineToEnd( source, pos, line );
		CHECK_ERR( StringParser::DivideString_CPP( line, tokens ) );

		// function params
		usize	j			= 0;
		int		brackets	= 0;
		uint	arg_counter	= 0;

		for (; j < tokens.Count(); ++j) {
			if ( tokens[j] == "(" )
				break;
		}
		
		CHECK_ERR( j > 0 );

		for (; j < tokens.Count(); ++j)
		{
			StringCRef	token		= tokens[j];
			StringCRef	next_token	= j+1 < tokens.Count() ? tokens[j+1] : StringCRef();

			if ( token == "(" )
			{
				++brackets;
				continue;
			}

			if ( next_token == "," or (next_token == ")" and brackets == 1) )
			{
				_args.Add( token, Arg( arg_counter ) );
				++arg_counter;
			}

			if ( token == ")" )
			{
				if ( (--brackets) == 0 )
					break;

				continue;
			}

			if ( token == "," and j+1 == tokens.Count() )
			{
				j = -1;

				StringParser::ReadLineToEnd( source, pos, line );
				CHECK_ERR( StringParser::DivideString_CPP( line, tokens ) );
				continue;
			}
		}

		return true;
	}

	bool CL2ComputeFunction::_InitArgs ()
	{
		using namespace cl;
		
		if ( _id == null )
			return true;

		cl_uint	num_args	= 0;
		char	name[ 128 ]	= "";

		CL_CALL( clGetKernelInfo( _id, CL_KERNEL_NUM_ARGS, sizeof(num_args), &num_args, null ) );
		CL_CALL( clGetKernelInfo( _id, CL_KERNEL_FUNCTION_NAME, sizeof(name), name, null ) );

		_args.Reserve( num_args );
		_objs.Reserve( num_args );


		// get program source
		String	source;
		usize	src_size = 0;

		CL_CALL( clGetProgramInfo( _program->Id(), CL_PROGRAM_SOURCE, 0, null, &src_size ) );
		
		source.Reserve( src_size + 2 );
		CL_CALL( clGetProgramInfo( _program->Id(), CL_PROGRAM_SOURCE, source.Capacity(), source.ptr(), null ) );

		source.SetLength( src_size );


		// find kernel
		StringCRef			kernel( "kernel" );
		Array< StringCRef >	tokens;

		for (usize pos = 0; source.Find( kernel, pos, pos ); pos += kernel.Length())
		{
			usize	name_pos = 0;
			usize	line_end = pos;

			StringParser::ToEndOfLine( source, line_end );
			
			CHECK( StringParser::DivideString_CPP( source.SubString( pos, line_end-pos ), tokens ) );

			if ( tokens.Find( name_pos, StringCRef( name ) ) )
			{
				CHECK_ERR( _ParseKernelArgs( source, pos + name_pos ) );
				return true;
			}
		}

		RETURN_ERR( "kernel not found!" );
	}

# endif

/*
=================================================
	_SetArgs
=================================================
*/
	struct CL2ComputeFunction_SetArg
	{
		const cl::cl_kernel		_kernel;
		const uint				_index;

		CL2ComputeFunction_SetArg (cl::cl_kernel kernel, uint index) :
			_kernel(kernel), _index(index)
		{}

		template <typename T>
		void operator () (const T& value) const
		{
			using namespace cl;
			CL_CALL( clSetKernelArg( _kernel, _index, sizeof(value), &value ) );
		}

		void operator () (const ComputeBufferPtr &value) const
		{
			using namespace cl;
			const cl_mem	mem = value->Id();
			CL_CALL( clSetKernelArg( _kernel, _index, sizeof(mem), &mem ) );
		}

		void operator () (const ComputeImagePtr &value) const
		{
			using namespace cl;
			const cl_mem	mem = value->Id();
			CL_CALL( clSetKernelArg( _kernel, _index, sizeof(mem), &mem ) );
		}
	};

	void CL2ComputeFunction::_SetArgs ()
	{
		FOR( i, _args )
		{
			Arg const&	arg = _args[i].second;

			CL2ComputeFunction_SetArg	func( _id, arg.index );

			arg.value.Apply( func );
		}
	}
	
/*
=================================================
	_CheckArgs
=================================================
*/
	void CL2ComputeFunction::_CheckArgs ()
	{
		FOR( i, _args )
		{
			if ( not _args[i].second.value )
			{
				LOG( (String("Uninitialized function argument: ") + StringCRef( _args[i].first )).cstr(), ELog::Warning );
			}
		}
	}
	
# if defined( GRAPHICS_API_OPENGL )
/*
=================================================
	CL2ComputeFunction_GetSharedObj
=================================================
*/
	struct CL2ComputeFunction_GetSharedObj
	{
		Array< cl::cl_mem > &	_objs;

		CL2ComputeFunction_GetSharedObj (Array< cl::cl_mem > &objs) : _objs(objs)
		{}

		template <typename T>
		void operator () (const T& value)
		{
			// do nothing
		}

		void operator () (const ComputeBufferPtr &value)
		{
			if ( value->IsShared() )
				_objs.PushBack( value->Id() );
		}

		void operator () (const ComputeImagePtr &value)
		{
			if ( value->IsShared() )
				_objs.PushBack( value->Id() );
		}
	};

/*
=================================================
	_AcquireObjects
=================================================
*/
	void CL2ComputeFunction::_AcquireObjects (cl::cl_command_queue cmdQueue)
	{
		using namespace cl;
		using namespace gl;

		CHECK( _objs.Empty() );
		
		CL2ComputeFunction_GetSharedObj		func( _objs );

		FOR( i, _args ) {
			_args[i].second.value.Apply( func );
		}

		if ( _objs.Empty() )
			return;

		//GL_CALL( glFlush() );
		CL_CALL( clEnqueueAcquireGLObjects( cmdQueue, (cl_uint) _objs.Count(), _objs.ptr(), 0, null, null ) );
	}
	
/*
=================================================
	_ReleaseObjects
=================================================
*/
	bool CL2ComputeFunction::_ReleaseObjects (cl::cl_command_queue cmdQueue)
	{
		using namespace cl;

		if ( _objs.Empty() )
			return false;
		
		CL_CALL( clEnqueueReleaseGLObjects( cmdQueue, (cl_uint) _objs.Count(), _objs.ptr(), 0, null, null ) );
		//CL_CALL( clFinish( cmdQueue ) );

		_objs.Clear();
		return true;
	}

# else
	
/*
=================================================
	_AcquireObjects
=================================================
*/
	void CL2ComputeFunction::_AcquireObjects (cl::cl_command_queue cmdQueue)
	{
	}
	
/*
=================================================
	_ReleaseObjects
=================================================
*/
	bool CL2ComputeFunction::_ReleaseObjects (cl::cl_command_queue cmdQueue)
	{
		return false;
	}

# endif	// GRAPHICS_API_OPENGL


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENCL
