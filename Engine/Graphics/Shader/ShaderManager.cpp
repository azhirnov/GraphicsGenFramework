// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "ShaderManager.h"
#include "Engine/Graphics/Engine/GraphicsEngine.h"

namespace Engine
{
namespace Graphics
{

/*
=================================================
	constructor
=================================================
*/
	inline ShaderManager::LoadedShader::LoadedShader () :
		flags( EShaderCompilationFlags::None ), shaderType( EShader::Unknown )
	{}
	
/*
=================================================
	constructor
=================================================
*/
	inline ShaderManager::LoadedShader::LoadedShader (StringCRef address, EShader::type type, EShaderCompilationFlags::type flags) :
		address( address ), flags( flags ), shaderType( type )
	{
		FileAddress::FormatPath( INOUT this->address );
	}

/*
=================================================
	operator >
=================================================
*/
	inline bool ShaderManager::LoadedShader::operator > (const LoadedShader &right) const
	{
		return	flags		!= right.flags		?	flags		> right.flags		:
				shaderType	!= right.shaderType	?	shaderType	> right.shaderType	:
													address		> right.address;
	}
	
/*
=================================================
	operator <
=================================================
*/
	inline bool ShaderManager::LoadedShader::operator < (const LoadedShader &right) const
	{
		return	flags		!= right.flags		?	flags		< right.flags		:
				shaderType	!= right.shaderType	?	shaderType	< right.shaderType	:
													address		< right.address;
	}
	
/*
=================================================
	operator ==
=================================================
*/
	inline bool ShaderManager::LoadedShader::operator == (const LoadedShader &right) const
	{
		return	flags		== right.flags		and
				shaderType	== right.shaderType	and
				address		== right.address;
	}

/*
=================================================
	constructor
=================================================
*/
	ShaderManager::ShaderManager (const SubSystemsRef ss) :
		BaseObject( ss ),
		_debugOutputNumber( 0 ),
		_copilationInProgress( false )
	{
		_loadedShaders.Reserve( 64 );
	}
	
/*
=================================================
	destructor
=================================================
*/
	ShaderManager::~ShaderManager ()
	{
		_copilationInProgress = false;

		Clear();
	}
	
/*
=================================================
	Clear
=================================================
*/
	void ShaderManager::Clear ()
	{
		ClearIncludeDirectories();
		ClearImportDirectories();
		ClearShaderCache();
	}
	
/*
=================================================
	SetDebugOutputFolder
=================================================
*/
	void ShaderManager::SetDebugOutputFolder (StringCRef dir)
	{
		_debugOutputFolder = dir;

		if ( not _debugOutputFolder.Empty() )
		{
			CHECK( SubSystems()->Get< FileManager >()->IsDirectoryExist( dir ) );

			FileAddress::FormatPath( INOUT _debugOutputFolder );
		}
	}

/*
=================================================
	AddIncludeDirectory
=================================================
*/
	bool ShaderManager::AddIncludeDirectory (StringCRef dir)
	{
		CHECK_ERR( SubSystems()->Get< FileManager >()->IsDirectoryExist( dir ) );

		String str = dir;
		FileAddress::FormatPath( str );

		ASSERT( not _includeDirs.IsExist( str ) );

		_includeDirs.PushBack( RVREF( str ) );
		return true;
	}
	
/*
=================================================
	AddIncludeDirectories
=================================================
*/
	bool ShaderManager::AddIncludeDirectories (Buffer<StringCRef> dirs)
	{
		Ptr< FileManager >	fm = SubSystems()->Get< FileManager >();
		String				str;

		FOR( i, dirs )
		{
			CHECK_ERR( fm->IsDirectoryExist( dirs[i] ) );
			
			str = dirs[i];
			FileAddress::FormatPath( str );
			
			ASSERT( not _includeDirs.IsExist( str ) );

			_includeDirs.PushBack( str );
		}
		return true;
	}
	
/*
=================================================
	ClearIncludeDirectories
=================================================
*/
	bool ShaderManager::ClearIncludeDirectories ()
	{
		CHECK_ERR( not _copilationInProgress );

		_includeDirs.Clear();
		return true;
	}
	
/*
=================================================
	AddImportDirectory
=================================================
*/
	bool ShaderManager::AddImportDirectory (StringCRef dir)
	{
		CHECK_ERR( SubSystems()->Get< FileManager >()->IsDirectoryExist( dir ) );
		
		String str = dir;
		FileAddress::FormatPath( str );
		
		ASSERT( not _importDirs.IsExist( str ) );

		_importDirs.PushBack( RVREF( str ) );
		return true;
	}
	
/*
=================================================
	AddImportDirectories
=================================================
*/
	bool ShaderManager::AddImportDirectories (Buffer<StringCRef> dirs)
	{
		Ptr< FileManager >	fm = SubSystems()->Get< FileManager >();
		String				str;

		FOR( i, dirs )
		{
			CHECK_ERR( fm->IsDirectoryExist( dirs[i] ) );
			
			str = dirs[i];
			FileAddress::FormatPath( str );
			
			ASSERT( not _importDirs.IsExist( str ) );

			_importDirs.PushBack( str );
		}
		return true;
	}
	
/*
=================================================
	ClearImportDirectories
=================================================
*/
	bool ShaderManager::ClearImportDirectories ()
	{
		CHECK_ERR( not _copilationInProgress );

		_importDirs.Clear();
		return true;
	}
	
/*
=================================================
	CompileShader
=================================================
*
	bool ShaderManager::CompileShader (OUT ShaderID &shader, EShader::type shaderType, StringCRef source,
										EShaderCompilationFlags::type compilationFlags)
	{
		ScopeSetter<bool>	compilation_in_progress( _copilationInProgress = true, false );

		CHECK_ERR( EShaderCompilationFlags::IsValid( compilationFlags ) );

		LoadedActiveShaders_t	result;

		CHECK_ERR( _ParseAndCompileShader( OUT result, source, "", ShaderBits_t().Set( shaderType ), compilationFlags ) );

		shader = result[shaderType].Get().shader;
		return true;
	}
	
/*
=================================================
	LoadShader
=================================================
*
	bool ShaderManager::LoadShader (OUT ShaderID &shader, EShader::type shaderType, StringCRef filename,
									EShaderCompilationFlags::type compilationFlags)
	{
		ScopeSetter<bool>	compilation_in_progress( _copilationInProgress = true, false );

		CHECK_ERR( EShaderCompilationFlags::IsValid( compilationFlags ) );
		
		StringCRef			header;
		Array< ShaderID >	shaders;

		CHECK_ERR( _ImportShaders( OUT header,  ) );

		CHECK_ERR( data.shader.IsValid() );

		shader = data.shader;
		return true;
	}

/*
=================================================
	LinkProgram
=================================================
*
	bool ShaderManager::LinkProgram (OUT ProgramID &program,
									 Buffer<ShaderID> shaders,
									 const VertexAttribsState &input,
									 const FragmentOutputState &output)
	{
		ScopeSetter<bool>	compilation_in_progress( _copilationInProgress = true, false );

		return false;
	}
	
/*
=================================================
	LoadProgram
=================================================
*/
	bool ShaderManager::LoadProgram (OUT ProgramID &program,
									 StringCRef filename,
									 ShaderBits_t activeShaders,
									 EShaderCompilationFlags::type compilationFlags,
									 const VertexAttribsState &input,
									 const FragmentOutputState &output)
	{
		ScopeSetter<bool>	compilation_in_progress( _copilationInProgress = true, false );

		CHECK_ERR( not activeShaders.Empty() );
		CHECK_ERR( EShaderCompilationFlags::IsValid( compilationFlags ) );
		
		Ptr< FileManager >		fm	= SubSystems()->Get< FileManager >();
		LoadedActiveShaders_t	loaded;
		ShaderParsingData		parsing_data;
		RFilePtr				file;

		parsing_data.sources.PushBack( ShaderSource( StringCRef(), filename, false ) );
		parsing_data.activeShaders		= activeShaders;
		parsing_data.compilationFlags	= compilationFlags;
		parsing_data.vertexAttribs		= input;
		parsing_data.fragmentOutput		= output;

		// read file
		CHECK_ERR( fm->OpenForRead( filename, OUT file ) );
		{
			const usize		len = (usize) file->RemainingSize();
			String &		src = parsing_data.SrcBack();

			src.Reserve( len+1 );

			CHECK_ERR( file->Read( src.ptr(), BytesU(len) ) );
			src.SetLength( len );
		}

		// compile shaders
		_ValidateIncludeAndImportDirs();
		CHECK_ERR( _ParseAndCompileShader( OUT loaded, INOUT parsing_data, "" ) );

		Array< ShaderID >	shaders;

		FOR( i, loaded )
		{
			if ( not loaded[i] )
				continue;

			shaders << loaded[i].Get().shader;
			shaders << loaded[i].Get().dependencies;
		}

		// link program
		CHECK_ERR( _LinkProgram( OUT program, shaders, activeShaders, input, output ) );

		DEBUG_ONLY( _DumpProgramResources( program, filename ); )
		return true;
	}
	
/*
=================================================
	CompileProgram
=================================================
*/
	bool ShaderManager::CompileProgram (OUT ProgramID &program,
										StringCRef source,
										Buffer<ShaderID> inputShaders,
										ShaderBits_t activeShaders,
										EShaderCompilationFlags::type compilationFlags,
										const VertexAttribsState &input,
										const FragmentOutputState &output)
	{
		ScopeSetter<bool>		compilation_in_progress( _copilationInProgress = true, false );

		CHECK_ERR( not activeShaders.Empty() );
		CHECK_ERR( EShaderCompilationFlags::IsValid( compilationFlags ) );
		
		LoadedActiveShaders_t	loaded;
		ShaderParsingData		parsing_data;

		parsing_data.sources.PushBack( ShaderSource( source, StringCRef(), false ) );
		parsing_data.activeShaders		= activeShaders;
		parsing_data.compilationFlags	= compilationFlags;
		parsing_data.vertexAttribs		= input;
		parsing_data.fragmentOutput		= output;

		// compile shaders
		_ValidateIncludeAndImportDirs();
		CHECK_ERR( _ParseAndCompileShader( OUT loaded, INOUT parsing_data, "" ) );

		Array< ShaderID >	shaders = inputShaders;

		FOR( i, loaded )
		{
			if ( not loaded[i] )
				continue;

			shaders << loaded[i].Get().shader;
			shaders << loaded[i].Get().dependencies;
		}

		_RemoveDuplicateShaders( shaders );

		// link program
		CHECK_ERR( _LinkProgram( OUT program, shaders, activeShaders, input, output ) );

		DEBUG_ONLY( _DumpProgramResources( program, "" ); )
		return true;
	}
	
/*
=================================================
	LoadShader
=================================================
*/
	bool ShaderManager::LoadShader (OUT Array<ShaderID> &shaders,
									OUT String &header,
									StringCRef filename,
									EShader::type shaderType,
									EShaderCompilationFlags::type compilationFlags)
	{
		ScopeSetter<bool>		compilation_in_progress( _copilationInProgress = true, false );
		
		shaders.Clear();
		header.Clear();

		CHECK_ERR( EShaderCompilationFlags::IsValid( compilationFlags ) );
		
		Ptr< FileManager >		fm	= SubSystems()->Get< FileManager >();
		LoadedActiveShaders_t	loaded;
		ShaderParsingData		parsing_data;
		RFilePtr				file;
		
		parsing_data.sources.PushBack( ShaderSource( StringCRef(), filename, false ) );
		parsing_data.activeShaders		= ShaderBits_t().Set( shaderType );;
		parsing_data.compilationFlags	= compilationFlags;
		
		// read file
		CHECK_ERR( fm->OpenForRead( filename, OUT file ) );
		{
			const usize		len = (usize)file->RemainingSize();
			String &		src = parsing_data.SrcBack();

			src.Reserve( len+1 );

			CHECK_ERR( file->Read( src.ptr(), BytesU(len) ) );
			src.SetLength( len );
		}
		
		// compile shaders
		_ValidateIncludeAndImportDirs();
		CHECK_ERR( _ParseAndCompileShader( OUT loaded, INOUT parsing_data, "" ) );

		FOR( i, loaded )
		{
			if ( not loaded[i] )
				continue;

			if ( header.Empty() )
				header = loaded[i].Get().header;

			shaders << loaded[i].Get().shader;
			shaders << loaded[i].Get().dependencies;
		}

		_RemoveDuplicateShaders( shaders );
		return true;
	}
		
/*
=================================================
	CompileShader
=================================================
*/
	bool ShaderManager::CompileShader (OUT Array<ShaderID> &shaders,
									   OUT String &header,
									   StringCRef source,
									   EShader::type shaderType,
									   EShaderCompilationFlags::type compilationFlags)
	{
		ScopeSetter<bool>		compilation_in_progress( _copilationInProgress = true, false );
		
		shaders.Clear();
		header.Clear();

		CHECK_ERR( EShaderCompilationFlags::IsValid( compilationFlags ) );
		
		LoadedActiveShaders_t	loaded;
		ShaderParsingData		parsing_data;
		
		parsing_data.sources.PushBack( ShaderSource( source, StringCRef(), false ) );
		parsing_data.activeShaders		= ShaderBits_t().Set( shaderType );
		parsing_data.compilationFlags	= compilationFlags;

		// compile shaders
		_ValidateIncludeAndImportDirs();
		CHECK_ERR( _ParseAndCompileShader( OUT loaded, INOUT parsing_data, "" ) );

		FOR( i, loaded )
		{
			if ( not loaded[i] )
				continue;

			if ( header.Empty() )
				header = loaded[i].Get().header;

			shaders << loaded[i].Get().shader;
			shaders << loaded[i].Get().dependencies;
		}

		_RemoveDuplicateShaders( shaders );
		return true;
	}

/*
=================================================
	ClearShaderCache
=================================================
*/
	bool ShaderManager::ClearShaderCache ()
	{
		CHECK_ERR( not _copilationInProgress );

		if ( _loadedShaders.Empty() )
			return true;

		Ptr< GraphicsContext >	context = SubSystems()->Get< GraphicsEngine >()->GetContext();

		FOR( i, _loadedShaders )
		{
			context->DeleteShader( _loadedShaders[i].shader );
		}

		_loadedShaders.Clear();
		return true;
	}
	
/*
=================================================
	_ParseShaderSource
----
	search blocks: [HEADER], [SOURCE], #include, #import
=================================================
*/
	bool ShaderManager::_ParseShaderSource (StringCRef source, usize sourceIdx, usize insertTo, INOUT Array<Tag> &tags)
	{
		static const StringCRef		headerKey	= "[HEADER]";
		static const StringCRef		sourceKey	= "[SOURCE]";
		static const StringCRef		importKey	= "import";
		static const StringCRef		includeKey	= "include";

		Array< StringCRef >		tokens;
		Array< Tag >			tmp_tags;
		const usize				count		= tags.Count();
		usize					num_headers	= 0;
		usize					num_sources	= 0;

		for (usize pos = 0; source.Find( headerKey, OUT pos, pos ); ++pos, ++num_headers)
		{
			usize	lines = StringParser::CalculateNumberOfLines( source.SubString( 0, pos ) );		// TODO: optimize

			tmp_tags << Tag( sourceIdx, pos, lines, EParsingTag::Header );
		}

		for (usize pos = 0; source.Find( sourceKey, OUT pos, pos ); ++pos, ++num_sources)
		{
			usize	lines = StringParser::CalculateNumberOfLines( source.SubString( 0, pos ) );

			tmp_tags << Tag( sourceIdx, pos, lines, EParsingTag::Source );
		}
		
		for (usize pos = 0; source.Find( importKey, OUT pos, pos ); ++pos)
		{
			tokens.Clear();

			usize		tmp = pos;
			StringCRef	res;

			StringParser::ReadCurrLine( source, INOUT tmp, OUT res );
			StringParser::DivideString_CPP( res, OUT tokens );

			if ( tokens.Count() > 3		and
				 tokens[0] == "#"		and
				 tokens[1] == importKey	and
				 tokens[2] == "\"" )
			{
				usize	lines = StringParser::CalculateNumberOfLines( source.SubString( 0, pos ) );

				tmp_tags << Tag( sourceIdx, pos, lines, EParsingTag::Import );
			}
		}
		
		for (usize pos = 0; source.Find( includeKey, OUT pos, pos ); ++pos)
		{
			tokens.Clear();
			
			usize		tmp = pos;
			StringCRef	res;
			
			StringParser::ReadCurrLine( source, INOUT tmp, OUT res );
			StringParser::DivideString_CPP( res, OUT tokens );

			if ( tokens.Count() > 3		 and
				 tokens[0] == "#"		 and
				 tokens[1] == includeKey and
				 tokens[2] == "\"" )
			{
				usize	lines = StringParser::CalculateNumberOfLines( source.SubString( 0, pos ) );

				tmp_tags << Tag( sourceIdx, pos, lines, EParsingTag::Include );
			}
		}

		// it is not a good idea to use many [HEADER] and [SOURCE] tags
		CHECK_ERR( num_headers <= 1 );
		CHECK_ERR( num_sources <= 1 );

		if ( tmp_tags.Empty() )
		{
			tags.Insert( Tag( sourceIdx, 0, 0, EParsingTag::SourceWithoutTag ), insertTo );
			return true;
		}

		// if no [HEADER] or [SOURCE] tag
		if ( num_headers == 0 and num_sources == 0 )
			tmp_tags << Tag( sourceIdx, 0, 0, EParsingTag::SourceWithoutTag );

		// sort tags buy position in text
		Sort( tmp_tags );

		// check first tag
		EParsingTag::type	first_tag = tmp_tags.Front().tag;

		CHECK_ERR(	first_tag == EParsingTag::Header			or
					first_tag == EParsingTag::Source			or
					first_tag == EParsingTag::HeaderWithoutTag	or
					first_tag == EParsingTag::SourceWithoutTag );

		tags.Insert( Buffer<Tag>( tmp_tags ), insertTo );
		return true;
	}
	
/*
=================================================
	_RecursiveParseShaders
=================================================
*/
	bool ShaderManager::_RecursiveParseShaders (INOUT ShaderParsingData &data)
	{
		CHECK_ERR( not data.sources.Empty() );

		Array< Tag >		tags;
		Ptr< FileManager >	fm		= SubSystems()->Get< FileManager >();
		String				tmp_str;
		bool				global_in_header = true;
		
		CHECK_ERR( _ParseShaderSource( data.Src(0), 0, 0, INOUT tags ) );

		FOR( i, tags )
		{
			const Tag		tag				= tags[i];
			usize			pos				= tag.offset;
			const usize		src_idx			= tag.sourceIdx;
			StringRef		str				= data.Src( src_idx );
			const bool		in_header		= data.sources[ src_idx ].isInHeader and global_in_header;
			const bool		has_tag_string	= (tag.tag != EParsingTag::SourceWithoutTag and tag.tag != EParsingTag::HeaderWithoutTag);

			// skip line with tag
			if ( has_tag_string )
				StringParser::ToNextLine( str, INOUT pos );
			
			switch ( tag.tag )
			{
				case EParsingTag::Header :
				case EParsingTag::HeaderWithoutTag :
				{
					global_in_header = true;
					data.headerRef << usize3( src_idx, pos, tag.line+1 );
					break;
				}

				case EParsingTag::Source :
				case EParsingTag::SourceWithoutTag :
				{
					global_in_header = false;
					data.sourceRef << usize3( src_idx, pos, tag.line+1 );
					break;
				}

				case EParsingTag::Include :
				{
					tags[i].tag		= in_header ? EParsingTag::HeaderWithoutTag : EParsingTag::SourceWithoutTag;
					tags[i].offset	= pos;

					String	temp_src;
					usize	str_pos = 0;	// begin of 'include'
					CHECK_ERR( StringParser::ReadStringCStyle( str.SubString( tag.offset, pos - tag.offset ),
																INOUT str_pos, OUT tmp_str ) );

					CHECK_ERR( _SearchShaderInclude( OUT temp_src, INOUT data.includedFiles, tmp_str ) );
					
					if ( not temp_src.Empty() )
					{
						data.sources.PushBack( ShaderSource( RVREF( temp_src ), RVREF( tmp_str ), in_header ) );

						CHECK_ERR( _ParseShaderSource( data.SrcBack(), data.sources.LastIndex(), i, INOUT tags ) );
					}

					--i;	// to first inserted tag
					break;
				}

				case EParsingTag::Import :
				{
					// TODO: memory usage warning: using import with many shader types is not a good idea!

					tags[i].tag		= in_header ? EParsingTag::HeaderWithoutTag : EParsingTag::SourceWithoutTag;
					tags[i].offset	= pos;

					usize	str_pos = 0;	// begin of 'import'
					CHECK_ERR( StringParser::ReadStringCStyle( str.SubString( tag.offset, pos - tag.offset ),
																INOUT str_pos, OUT tmp_str ) );

					StringCRef	header;
					CHECK_ERR( _ImportShaders( OUT header, INOUT data.importedFiles, INOUT data.dependentShaders,
												tmp_str, data.activeShaders, data.compilationFlags ) );

					if ( not header.Empty() )
					{
						data.sources.PushBack( ShaderSource( header, tmp_str, in_header ) );
					
						CHECK_ERR( _ParseShaderSource( data.SrcBack(), data.sources.LastIndex(), i, INOUT tags ) );
					}

					--i;	// to first inserted tag
					break;
				}

				default :
					RETURN_ERR( "Unsupported tag!" );
			}

			// clear line with tag
			if ( has_tag_string )
			{
				usize	begin	= tag.offset;
				usize	end		= pos;

				str = data.Src( src_idx );
				StringParser::ToBeginOfLine( str, INOUT begin );

				for (usize c = begin; c < end; ++c) {
					str[c] = '\0';
				}
			}
		}
		
		return true;
	}
	
/*
=================================================
	_ParseAndCompileShader
=================================================
*/
	bool ShaderManager::_ParseAndCompileShader (OUT LoadedActiveShaders_t &result, INOUT ShaderParsingData &parsingData, StringCRef filename)
	{
		Array< SourceInfo >		src;
		String					header;
		String					attribs_and_fragout;
		
		result.Clear();

		CHECK_ERR( not parsingData.sources.Empty() );
		CHECK_ERR( not parsingData.activeShaders.Empty() );
		CHECK_ERR( EShaderCompilationFlags::IsValid( parsingData.compilationFlags ) );
		// parsingData.vertexAttribs is optional
		// parsingData.fragmentOutput is optional

		// must be empty
		parsingData.headerRef.Clear();
		parsingData.sourceRef.Clear();
		parsingData.includedFiles.Clear();
		parsingData.importedFiles.Clear();
		parsingData.dependentShaders.Clear();

		CHECK_ERR( _RecursiveParseShaders( INOUT parsingData ) );

		// add attribs
		{
			_AddAttribs( OUT attribs_and_fragout, parsingData.vertexAttribs );
			_AddOutputs( OUT attribs_and_fragout, parsingData.fragmentOutput );

			if ( not attribs_and_fragout.Empty() )
			{
				src << SourceInfo( attribs_and_fragout,
								   "generated attribs",
								   usize2( 0, StringParser::CalculateNumberOfLines( attribs_and_fragout ) ).To<uint2>()
				);
			}
		}

		// build header and source
		FOR( i, parsingData.headerRef )
		{
			usize3 const& c = parsingData.headerRef[i];

			StringCRef	str = parsingData.Src( c.x ).SubString( c.y ).cstr();

			header << str << '\n';
			src << SourceInfo( str,
							   parsingData.Filename( c.x ),
							   usize2( c.z, StringParser::CalculateNumberOfLines( str ) ).To<uint2>()
			);
		}
		
		FOR( i, parsingData.sourceRef )
		{
			usize3 const& c = parsingData.sourceRef[i];
			
			StringCRef	str = parsingData.Src( c.x ).SubString( c.y ).cstr();
			
			src << SourceInfo( str,
							   parsingData.Filename( c.x ),
							   usize2( c.z, StringParser::CalculateNumberOfLines( str ) ).To<uint2>()
			);
		}

		CHECK( _WriteDebugOutput( src ) );

		_RemoveDuplicateShaders( parsingData.dependentShaders );

		// compile shaders
		FOR( i, parsingData.activeShaders )
		{
			if ( not parsingData.activeShaders[i] )
				continue;

			LoadedShader &	shader_data	= result[i].CreateDefault();

			shader_data.address		= filename;
			shader_data.header		= header;
			shader_data.flags		= parsingData.compilationFlags;
			shader_data.shaderType	= EShader::type( i );

			FOR( j, parsingData.dependentShaders )
			{
				if ( parsingData.dependentShaders[j].GetType() == shader_data.shaderType )
					shader_data.dependencies << parsingData.dependentShaders[j];
			}

			CHECK_ERR( _CompileShader( INOUT shader_data, src ) );
		}
		return true;
	}
	
/*
=================================================
	_SearchShaderInclude
=================================================
*/
	bool ShaderManager::_SearchShaderInclude (OUT String &source, INOUT HashSet<String> &includedFiles, StringCRef filename) const
	{
		String				fname;
		Ptr< FileManager >	fm	= SubSystems()->Get< FileManager >();

		FOR( i, _includeDirs )
		{
			fname = filename;
			FileAddress::AddBaseDirectoryToPath( INOUT fname, _includeDirs[i] );

			if ( fm->IsFileExist( fname ) )
			{
				FileAddress::FormatPath( INOUT fname );

				if ( includedFiles.IsExist( fname ) )
				{
					LOG( (String("file \"") << fname << "\" already included").cstr(), ELog::Debug );
					return true;
				}
				
				includedFiles.Add( fname );

				RFilePtr	file;

				CHECK_ERR( fm->OpenForRead( fname, OUT file ) );
				
				const usize		len = (usize)file->RemainingSize();

				source.Reserve( len+1 );

				CHECK_ERR( file->Read( source.ptr(), BytesU(len) ) );
				source.SetLength( len );

				return true;
			}
		}
		RETURN_ERR( "Can't open shader source file: \"" << filename << "\"" );
	}
	
/*
=================================================
	_ImportShader
=================================================
*/
	bool ShaderManager::_ImportShaders (OUT StringCRef &header, INOUT HashSet<String> &importedFiles,
										INOUT Array<ShaderID> &shaders, StringCRef filename,
										ShaderBits_t activeShaders, EShaderCompilationFlags::type compilationFlags)
	{
		header = StringCRef();

		String				fname;
		Ptr< FileManager >	fm			= SubSystems()->Get< FileManager >();
		LoadedShader		shader_data;

		FOR( i, _importDirs )
		{
			fname = filename;
			FileAddress::AddBaseDirectoryToPath( INOUT fname, _importDirs[i] );

			// is file exist?
			if ( fm->IsFileExist( fname ) )
			{
				FileAddress::FormatPath( INOUT fname );

				if ( importedFiles.IsExist( fname ) )
				{
					LOG( (String("file \"") << fname << "\" already imported").cstr(), ELog::Debug );
					return true;
				}

				importedFiles.Add( fname );

				// search in cache
				FOR( j, activeShaders )
				{
					if ( not activeShaders[j] )
						continue;

					shader_data.address		= fname;
					shader_data.shaderType	= EShader::type( j );
					shader_data.flags		= compilationFlags;

					LoadedShaders_t::const_iterator	iter;

					if ( _loadedShaders.Find( shader_data, OUT iter ) )
					{
						shaders << iter->shader;
						shaders << iter->dependencies;

						if ( header.Empty() )
							header = iter->header;

						activeShaders.Reset( j );
					}
				}

				if ( activeShaders.Empty() )
					return true;

				// load from file
				LoadedActiveShaders_t	loaded;
				ShaderParsingData		parsing_data;
				RFilePtr				file;

				CHECK_ERR( fm->OpenForRead( fname, OUT file ) );

				parsing_data.sources.PushBack( ShaderSource( StringCRef(), filename, true ) );
				parsing_data.activeShaders		= activeShaders;
				parsing_data.compilationFlags	= compilationFlags;

				// read file
				{
					const usize	len = (usize)file->RemainingSize();
					String &	src = parsing_data.Src( 0 );

					src.Reserve( len+1 );

					CHECK_ERR( file->Read( src.ptr(), BytesU(len) ) );
					src.SetLength( len );
				}

				CHECK_ERR( _ParseAndCompileShader( OUT loaded, INOUT parsing_data, fname ) );
					
				FOR( j, loaded )
				{
					if ( not loaded[j] )
						continue;

					usize	idx = _loadedShaders.Add( loaded[j].Get() );
					
					shaders << _loadedShaders[idx].shader;
					shaders << _loadedShaders[idx].dependencies;

					if ( header.Empty() )
						header = _loadedShaders[idx].header;
				}
				return true;
			}
		}
		RETURN_ERR( "Can't open shader source file: \"" << filename << "\"" );
	}
	
/*
=================================================
	_WriteDebugOutput
=================================================
*/
	bool ShaderManager::_WriteDebugOutput (Buffer<SourceInfo> sources)
	{
		if ( _debugOutputFolder.Empty() )
			return true;	// debug output disabled

		Ptr< FileManager >	fm = SubSystems()->Get< FileManager >();
		WFilePtr			file;
		String				fname;
		StaticString<64>	unique_name;	unique_name << "shader_" << _debugOutputNumber << ".glsl";

		FileAddress::BuildPath( OUT fname, _debugOutputFolder, unique_name );

		CHECK_ERR( fm->CreateFile( fname, OUT file ) );

		String &	separator = fname;

		FOR( i, sources )
		{
			separator.Clear();
			separator << "/* part " << i << ", file \"" << sources[i].filename
					  << "\", line " << sources[i].FirstLine() << " */\n";	// it breakes line numeration 

			file->Write( StringCRef( separator ) );
			file->Write( StringCRef( sources[i].source ) );
		}

		++_debugOutputNumber;
		return true;
	}
	
/*
=================================================
	_ValidateIncludeAndImportDirs
=================================================
*/
	void ShaderManager::_ValidateIncludeAndImportDirs ()
	{
		if ( _includeDirs.Empty() )
			_includeDirs << "";

		if ( _importDirs.Empty() )
			_importDirs << "";
	}
	
/*
=================================================
	_RemoveDuplicateShaders
=================================================
*/
	void ShaderManager::_RemoveDuplicateShaders (INOUT Array<ShaderID> &shaders)
	{
		// sort and remove duplicates
		Sort( shaders,
			  LAMBDA() (const ShaderID &left, const ShaderID &right) -> bool { return left.Id() > right.Id(); } );

		for (usize i = 1; i < shaders.Count(); ++i)
		{
			if ( shaders[i-1].Id() == shaders[i].Id() )
			{
				shaders.Erase( i );
				--i;
			}
		}
	}


}	// Graphics
}	// Engine
