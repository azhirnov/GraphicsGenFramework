// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "CL2ComputeProgram.h"

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
	CL2ComputeProgram::CL2ComputeProgram (const SubSystemsRef ss) :
		BaseObject( ss ),
		_id( null ),	_flags( EProgramUnitFlags::Unknown )
	{
	}

/*
=================================================
	destructor
=================================================
*/
	CL2ComputeProgram::~CL2ComputeProgram ()
	{
		_Delete();
	}
	
/*
=================================================
	Create
=================================================
*/
	bool CL2ComputeProgram::Create (StringCRef filename, EProgramUnitFlags::type flags)
	{
		_Delete();

		String			source;
		Array<String>	temp_src;
		String			fname		= filename;

		source.Reserve( 1<<20 );

		if ( not SubSystems()->Get< FileManager >()->IsFileExist( fname ) )
			fname << '.' << _DefaultExtension();

		if ( EnumEq( flags, EProgramUnitFlags::FP_32 ) )
			source << "#define COMPUTE_FP 32\n";
		else
		if ( EnumEq( flags, EProgramUnitFlags::FP_64 ) )
			source << "#define COMPUTE_FP 64\n";

		temp_src.Reserve( 256 );

		CHECK_ERR( _Load( fname, temp_src, source ) );
		CHECK_ERR( _SetSource( source ) );
		CHECK_ERR( _Compile() );

		_flags = flags;
		return true;
	}
	
/*
=================================================
	_Delete
=================================================
*/
	void CL2ComputeProgram::_Delete ()
	{
		using namespace cl;

		_flags = EProgramUnitFlags::Unknown;

		if ( _id == null )
			return;

		CL_CALL( clReleaseProgram( _id ) );
		_id = null;
	}
	
/*
=================================================
	_SetSource
=================================================
*/
	bool CL2ComputeProgram::_SetSource (StringCRef source)
	{
		using namespace cl;
		
		cl_int		cl_err	= 0;
		const char*	src		= source.cstr();

		CL_CALL( (_id = clCreateProgramWithSource(
			SubSystems()->Get< ComputeEngine >()->GetContext(),
			1,
			&src,
			null,
			&cl_err )), cl_err );

		CHECK_ERR( _id != null );
		return true;
	}

/*
=================================================
	_Compile
=================================================
*/
	bool CL2ComputeProgram::_Compile () const
	{
		using namespace cl;

		const cl_device_id	devices[]	= { SubSystems()->Get< ComputeEngine >()->GetDevice() };
		usize				log_size	= 0;
		cl_int				cl_err		= 0;
		StringCRef			options		= SubSystems()->Get< ComputeEngine >()->GetBuildOptions();

		( (cl_err = clBuildProgram( _id, (cl_uint)CountOf(devices), devices, options.cstr(), null, null )) );
		const bool compiled = (cl_err == CL_SUCCESS);

		CL_CALL( clGetProgramBuildInfo( _id, devices[0], CL_PROGRAM_BUILD_LOG, 0, null, &log_size ) );

		if ( log_size > 5 )
		{
			String	info;
			info.Reserve( log_size + 128 );
			info << "compute program build " << (compiled ? "message" : "error") << ":\n";

			CL_CALL( clGetProgramBuildInfo( _id, devices[0], CL_PROGRAM_BUILD_LOG, log_size, info.End(), null ) );
			info.CalculateLength();
		
			LOG( info.cstr(), compiled ? ELog::Debug : ELog::Warning );
		}

		CHECK_ERR( compiled );
		return true;
	}
	
/*
=================================================
	_Load
=================================================
*/
	bool CL2ComputeProgram::_Load (StringCRef filename, OUT Array<String> &tmp, OUT String &source) const
	{
		tmp.PushBack( String() );
		CHECK_ERR( _ReadFile( filename, tmp.Back() ) );

		Array<usize2>		src_idx;
		HashSet<String>		included_files;

		CHECK_ERR( _RecursiveParse( tmp, src_idx, included_files, filename ) );

		FOR( i, src_idx )
		{
			StringCRef	str	= tmp[ src_idx[i].x ].cstr() + src_idx[i].y;

			if ( str.Empty() )
				continue;

			source << str;
		}

		return true;
	}
	
/*
=================================================
	_RecursiveParse
=================================================
*/
	bool CL2ComputeProgram::_RecursiveParse (INOUT Array<String> &tmp, INOUT Array<usize2> &source,
											 INOUT HashSet<String> &includedFiles, StringCRef srcFilename) const
	{
		static const StringCRef	importKey( "#include" );
		StringCRef				path		= FileAddress::GetPath( srcFilename );
		const usize				i			= tmp.LastIndex();
		usize					lines		= 1;
		usize					prev_pos	= 0;

		source.PushBack( usize2( i, 0 ) );

		for (usize pos = 0; tmp[i].Find( importKey, pos, pos ); )
		{
			// read filename
			String		filename;
			StringCRef	fname;
			{
				String &	src = tmp[i];

				lines += StringParser::CalculateNumberOfLines( tmp[i].SubString( prev_pos, pos - prev_pos ) );
				prev_pos = pos;

				src[ pos ] = '\0';

				pos += importKey.Length();

				CHECK_ERR( _ParseString( src, pos, fname ) );
			
				StringParser::ToNextLine( src, pos );

				FileAddress::BuildPath( filename, path, fname );
				FileAddress::FormatPath( filename );
			}

			if ( not includedFiles.IsExist( filename ) )
			{
				includedFiles.Add( filename );

				// parse included file
				tmp.PushBack( String() );

				CHECK_ERR( _ReadFile( filename, tmp.Back() ) );
				CHECK_ERR( _RecursiveParse( tmp, source, includedFiles, filename ) );

				tmp.PushBack( String() );
				tmp.Back() << "#line " << lines << " \"" << FileAddress::GetNameAndExt( srcFilename ) << "\"\n";
				source.PushBack( usize2( tmp.LastIndex(), 0 ) );
			}
			else
			{
				LOG( (String("file \"") << fname << "\" already included").cstr(), ELog::Debug );
			}
			
			// continue parsing
			source.PushBack( usize2( i, pos ) );
		}

		return true;
	}
	
/*
=================================================
	_ParseString
=================================================
*/
	bool CL2ComputeProgram::_ParseString (StringCRef src, INOUT usize &pos, OUT StringCRef &filename) const
	{
		// find open bracket
		for (; pos < src.Length(); ++pos)
		{
			const char	c = src[ pos ];

			if ( c == ' ' or c == '\t' )
				continue;

			if ( c == '"' )
				break;

			RETURN_ERR( "incorrect char '" << c << "'" );
		}

		const usize	begin = ++pos;

		// find close bracket
		for (; pos < src.Length(); ++pos)
		{
			const char	c = src[ pos ];

			if ( c == '"' )
				break;

			if ( c == '\n' or c == '\r' ) {
				RETURN_ERR( "no pair for braket \" " );
			}
		}

		filename = StringCRef( src.cstr() + begin, pos - begin );
		return true;
	}

/*
=================================================
	_ReadFile
=================================================
*/
	bool CL2ComputeProgram::_ReadFile (StringCRef filename, OUT String &src) const
	{
		RFilePtr	file;
		CHECK_ERR( SubSystems()->Get< FileManager >()->OpenForRead( filename, file ) );

		const usize	size = (usize)file->RemainingSize();

		src << "#line 1 \"" << FileAddress::GetNameAndExt( filename ) << "\"\n";
		src.Reserve( size + 2 + src.Length() );

		CHECK_ERR( file->Read( src.End(), BytesU(size) ) );
		
		src.SetLength( src.Length() + size );
		src << '\n';

		return true;
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENCL
