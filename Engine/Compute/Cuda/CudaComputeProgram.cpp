// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "CudaComputeProgram.h"

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
	CudaComputeProgram::CudaComputeProgram (const SubSystemsRef ss) :
		BaseObject( ss ), _module( null )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	CudaComputeProgram::~CudaComputeProgram ()
	{
		_Delete();
	}
	
/*
=================================================
	Create
=================================================
*/
	bool CudaComputeProgram::Create (StringCRef filename)
	{
		using namespace cuda;
		
		String		error_log;
		String		info_log;
		float		wall_time	= 0.0f;

		String		asm_data;
		CUlinkState	link_state	= null;
		bool		linked		= false;

		CHECK_ERR( _CreateLinker( link_state, wall_time, error_log, info_log ) );
		CHECK_ERR( _LoadFile( asm_data, filename ) );

		if ( _Link( asm_data, link_state, FileAddressUtils::GetNameAndExt( filename ) ) )
		{
			CHECK_ERR( _Compile( link_state ) );
			
			_ParseKernels( asm_data );
			_ParseGlobalVars( asm_data );

			linked = true;
		}
		
		_DeleteLinker( link_state, wall_time, error_log, info_log );

		CHECK_ERR( linked );
		return true;
	}
	
/*
=================================================
	New
=================================================
*/
	ComputeProgramPtr  CudaComputeProgram::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new CudaComputeProgram( ss ) );
	}
	
/*
=================================================
	_LoadFile
=================================================
*/
	bool CudaComputeProgram::_LoadFile (OUT String &asmData, StringCRef filename)
	{
		RFilePtr	file;
		CHECK_ERR( SubSystems()->Get< FileManager >()->OpenForRead( filename, file ) );

		usize const	len	= file->RemainingSize();

		asmData.Reserve( len + 10 );

		CHECK_ERR( file->Read( Buffer<char>::From( asmData.ptr(), len ) ) );

		asmData.SetLength( len );
		return true;
	}
	
/*
=================================================
	_CreateLinker
=================================================
*/
	bool CudaComputeProgram::_CreateLinker (OUT cuda::CUlinkState &linkState, OUT float &wallTime,
											OUT String &errorLog, OUT String &infoLog)
	{
		using namespace cuda;
		
		CUjit_option	options[ 32 ]	= {};
		void *			opt_vals[ 32 ]	= {};
		usize			i				= 0;
		usize const		log_size		= 2048;

		errorLog.Reserve( log_size + 1 );
		infoLog.Reserve( log_size + 1 );

		options[i]	= CU_JIT_WALL_TIME;
		opt_vals[i]	= (void *) &wallTime;
		++i;

		options[i]	= CU_JIT_INFO_LOG_BUFFER;
		opt_vals[i]	= (void *) infoLog.ptr();
		++i;

		options[i]	= CU_JIT_INFO_LOG_BUFFER_SIZE_BYTES;
		opt_vals[i]	= ReferenceCast<void *>( log_size );
		++i;

		options[i]	= CU_JIT_ERROR_LOG_BUFFER;
		opt_vals[i]	= (void *) errorLog.ptr();
		++i;

		options[i]	= CU_JIT_ERROR_LOG_BUFFER_SIZE_BYTES;
		opt_vals[i]	= ReferenceCast<void *>( log_size );
		++i;

		options[i]	= CU_JIT_LOG_VERBOSE;
		opt_vals[i]	= (void *) 1;
		++i;

		options[i]	= CU_JIT_TARGET_FROM_CUCONTEXT;
		opt_vals[i]	= null;
		++i;
		
		options[i]	= CU_JIT_GENERATE_LINE_INFO;
		opt_vals[i]	= (void *) 1;
		++i;
		
		DEBUG_ONLY(
			options[i]	= CU_JIT_GENERATE_DEBUG_INFO;
			opt_vals[i]	= (void *) 1;
			++i;
		)

		ASSERT( i < CountOf( options ) );

		CU_CHECK( cuLinkCreate( i, options, opt_vals, &linkState ) );
		return true;
	}
	
/*
=================================================
	_DeleteLinker
=================================================
*/
	void CudaComputeProgram::_DeleteLinker (cuda::CUlinkState &linkState, float wallTime, String &errorLog, String &infoLog)
	{
		using namespace cuda;

		errorLog.CalculateLength();
		infoLog.CalculateLength();

		if ( errorLog.Length() > 0 )
		{
			"Compute program compilation errors:\n" >> errorLog;
			LOG( errorLog.cstr(), ELog::WARN );
		}
		
		if ( infoLog.Length() > 0 )
		{
			"Compute program compilation messages:\n" >> infoLog;
			LOG( infoLog.cstr(), ELog::DEBUG );
		}

		// TODO: write compilation time

		CU_CALL( cuLinkDestroy( linkState ) );
		linkState = null;
	}

/*
=================================================
	_Link
=================================================
*/
	bool CudaComputeProgram::_Link (const String &asmData, cuda::CUlinkState linkState, StringCRef name)
	{
		using namespace cuda;

		CU_CHECK( cuLinkAddData( linkState, CU_JIT_INPUT_PTX, (void *)asmData.ptr(), asmData.Count(), name.cstr(), 0, null, null ) );
		return true;
	}
	
/*
=================================================
	_Compile
=================================================
*/
	bool CudaComputeProgram::_Compile (cuda::CUlinkState linkState)
	{
		using namespace cuda;

		void *	linked = null;

		CU_CHECK( cuLinkComplete( linkState, &linked, null ) );

		CU_CHECK( cuModuleLoadData( &_module, linked ) );
		return true;
	}
	
/*
=================================================
	_ParseKernels
=================================================
*/
	void CudaComputeProgram::_ParseKernels (const String &asmData)
	{
		_kernels.Reserve( 128 );

		StringCRef	kernel( ".entry" );
		StringCRef	kernel_param( ".param" );
		usize			pos = 0;

		Array< StringCRef >	tokens;

		for (; asmData.Find( kernel, pos, pos );)
		{
			const char	first_char = pos > 0 ? asmData[ pos-1 ] : ' ';
				
			pos += kernel.Length();

			if ( first_char != ' ' and
				 first_char != '\t' and
				 first_char != '\n' and
				 first_char != '\r' )
			{
				continue;
			}

			// find kernel name
			for (; pos < asmData.Length(); ++pos)
			{
				const char	c = asmData[ pos ];

				if ( c != ' ' and c != '\t' and c != '\n' and c != '\r' )
					break;
			}
			const usize	name_start = pos;

			// find name end
			for (; pos < asmData.Length(); ++pos)
			{
				const char	c = asmData[ pos ];

				if ( c == ' ' or c == '\t' or c != '\n' or c != '\r'or c == '(' )
					break;
			}
			const usize	name_end = pos;

			// find args start
			for (; pos < asmData.Length(); ++pos)
			{
				const char	c = asmData[ pos ];

				if ( c == '(' )
					break;
			}
			const usize	arg_start = pos+1;

			// find args end
			for (; pos < asmData.Length(); ++pos)
			{
				const char	c = asmData[ pos ];

				if ( c == ')' )
					break;
			}
			const usize	arg_end = pos;

			StringCRef	kernel_name = asmData.SubString( name_start, name_end - name_start );
			StringCRef	kernel_args	= asmData.SubString( arg_start,  arg_end - arg_start );

			usize const		kernel_idx	= _kernels.AddOrReplace( KernelInfo( kernel_name ) );
			usize			arg_counter	= 0;

			// parse args names
			for (usize arg_pos = 0; kernel_args.Find( kernel_param, arg_pos, arg_pos ); )
			{
				arg_pos += kernel_param.Length();

				usize const		arg_str_start = arg_pos;

				for (; arg_pos < kernel_args.Length(); ++arg_pos)
				{
					if ( kernel_args[ arg_pos ] == ',' )
						break;
				}

				CHECK( StringParser::DivideString_CPP( kernel_args.SubString( arg_str_start, arg_pos - arg_str_start ), tokens ) );


				// type (align N) (space) name ([N]) (,)
				StringCRef	arg_name;
				int				bracket_cnt = 0;

				for (usize i = tokens.LastIndex(); i < tokens.Count(); --i)
				{
					if ( tokens[i] == "," )
						continue;

					if ( tokens[i] == "]" ) {
						++bracket_cnt;
						continue;
					}

					if ( bracket_cnt > 0 and tokens[i].Front() >= '0' and tokens[i].Front() <= '9' )
						continue;

					if ( tokens[i] == "[" ) {
						--bracket_cnt;
						continue;
					}
					
					if ( bracket_cnt > 0 ) {
						WARNING( "invalid token into a brackets" );
						continue;
					}

					arg_name = tokens[i];
					break;
				}


				// __cudaparm_(kernel_name)_(arg_name)
				// (kernel_name)_(arg_name)

				usize	p = 0;

				if ( arg_name.Find( kernel_name, p, 0 ) )
				{
					_kernels[ kernel_idx ].args.AddOrReplace( KernelArgInfo( arg_name.SubString( p+1 ), arg_counter ) );
				}
				else
					_kernels[ kernel_idx ].args.AddOrReplace( KernelArgInfo( arg_name, arg_counter ) );

				++arg_counter;
			}
		}
	}
	
/*
=================================================
	_ParseGlobalVars
=================================================
*/
	void CudaComputeProgram::_ParseGlobalVars (const String &asmData)
	{
		// ld.global, st.global, atom.global
		_globalVars.Reserve( 128 );
		
		StringCRef	global( ".global" );
		usize			pos = 0;

		Array< StringCRef >	tokens;

		for (; asmData.Find( global, pos, pos ); pos += global.Length())
		{
			const char	first_char = pos > 0 ? asmData[ pos-1 ] : ' ';
			
			pos += global.Length();

			if ( first_char != ' ' and
				 first_char != '\t' and
				 first_char != '\n' and
				 first_char != '\r' )
			{
				continue;
			}

			StringCRef	line;
			StringParser::ReadLine( asmData, pos, line );

			CHECK( StringParser::DivideString_CPP( line, tokens ) );

			// .global (attribute()) (align N) (vector)  (type)

			if ( tokens.Back() != ";" )
				continue;

			StringCRef	global_name;
			int				bracket_cnt = 0;

			for (usize i = tokens.LastIndex(); i < tokens.Count(); --i)
			{
				if ( tokens[i] == ";" )
					continue;

				if ( tokens[i] == "]" ) {
					++bracket_cnt;
					continue;
				}

				if ( bracket_cnt > 0 and tokens[i].Front() >= '0' and tokens[i].Front() <= '9' )
					continue;

				if ( tokens[i] == "[" ) {
					--bracket_cnt;
					continue;
				}

				if ( bracket_cnt > 0 ) {
					WARNING( "invalid token into a brackets" );
					continue;
				}

				global_name = tokens[i];
				break;
			}

			_globalVars.AddOrReplace( global_name );
		}
	}
	
/*
=================================================
	_Delete
=================================================
*/
	void CudaComputeProgram::_Delete ()
	{
		using namespace cuda;

		if ( _module != null )
		{
			CU_CALL( cuModuleUnload( _module ) );
			_module = null;
		}
		
		_kernels.Clear();
		_globalVars.Clear();
	}
	
/*
=================================================
	_GetKernel
=================================================
*/
	bool CudaComputeProgram::_GetKernel (StringCRef name, OUT Ptr<const KernelInfo> &kernel, OUT uint &numArgs) const
	{
		bool	res = _kernels.Find( KernelInfo( name ), kernel );

		numArgs = res and kernel.IsNotNull() ? kernel->args.Count() : 0;

		return res;
	}
	
/*
=================================================
	_HasKernelArg
=================================================
*/
	bool CudaComputeProgram::_HasKernelArg (Ptr<const KernelInfo> kernel, const StString_t &name, OUT uint &index) const
	{
		if ( kernel.IsNotNull() )
		{
			KernelArgs_t::const_iterator	iter;

			if ( kernel->args.Find( KernelArgInfo( name ), iter ) )
			{
				index = iter->index;
				return true;
			}
		}

		index = -1;
		return false;
	}
	
/*
=================================================
	_HasGlobal
=================================================
*/
	bool CudaComputeProgram::_HasGlobal (const StString_t &name) const
	{
		return _globalVars.IsExist( name );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_CUDA
