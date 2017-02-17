// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.
/*
	Shaders support #include and #import directives.
	#include - attach source code
	#import  - compile shader and attach header source only.

	[HEADER] - source with this tag will be attached in #include or #import directives.
	[SOURCE] - source with this tag will be attached in #include and will be hidden in #import directive.

	Warning:
	 -	#include, #import, [HEADER], [SOURCE] tags and directives always works inside preprocessor,
		for example:
		#if 0
			#include "..."	// always works
		#endif

	 - relative path not supported (yet).
*/

#pragma once

#include "ShaderProgram.h"

namespace Engine
{
namespace Graphics
{

	//
	// Shader Manager
	//

	class ShaderManager : public BaseObject
	{
	// types
	public:
		typedef Bitfield< EShader::_Count >		ShaderBits_t;

	private:
		static const uint	MAX_DEPENDENCIES = 32;

		struct EParsingTag
		{
			enum type : ubyte
			{
				None,
				Header,				// [HEADER]
				Source,				// [SOURCE]
				HeaderWithoutTag,	// ...
				SourceWithoutTag,	// ...
				Include,			// #include
				Import				// #import
			};
		};


		struct Tag : CompileTime::PODStruct
		{
		// variables
			usize				sourceIdx;
			usize				offset;
			usize				line;
			EParsingTag::type	tag;

		// methods
			Tag () :
				sourceIdx(-1), offset(0), line(0), tag(EParsingTag::None)
			{}

			Tag (usize sourceIdx, usize offset, usize line, EParsingTag::type tag) :
				sourceIdx(sourceIdx), offset(offset), line(line), tag(tag)
			{}

			// for sorting
			bool operator > (const Tag &right) const
			{
				return offset != right.offset	?	offset	> right.offset :
													tag		> right.tag;
			}
		};


		struct ShaderSource : CompileTime::FastCopyable
		{
		// variables
			String		sourceStr;
			String		filename;		// TODO: use StringCRef if it is safe
			bool		isInHeader;		// if attachment in header then header attached to header, otherwise attachet to source
			
		// methods
			ShaderSource () :
				isInHeader(false)
			{}

			ShaderSource (StringCRef src, StringCRef fname, bool inHeader) :
				sourceStr(src), filename(fname), isInHeader(inHeader)
			{}

			ShaderSource (String &&src, String && fname, bool inHeader) :
				sourceStr(RVREF(src)), filename(RVREF(fname)), isInHeader(inHeader)
			{}
		};


		struct ShaderParsingData : CompileTime::FastCopyable
		{
		// variables
			Array< ShaderSource >			sources;
			Array< usize3 >					headerRef;		// {source index, offset, line}
			Array< usize3 >					sourceRef;		// {source index, offset, line}
			HashSet< String >				includedFiles;
			HashSet< String >				importedFiles;
			Array< ShaderID >				dependentShaders;
			ShaderBits_t					activeShaders;
			EShaderCompilationFlags::type	compilationFlags;
			VertexAttribsState				vertexAttribs;
			FragmentOutputState				fragmentOutput;

		// methods
			ShaderParsingData () : compilationFlags( EShaderCompilationFlags::Unknown )
			{}

			String &	Src (usize i)		{ return sources[i].sourceStr; }
			StringCRef	Filename (usize i)	{ return sources[i].filename; }
			String &	SrcBack ()			{ return sources.Back().sourceStr; }
		};


		struct LoadedShader : CompileTime::FastCopyable
		{
		// types
			typedef FixedSizeArray< ShaderID, MAX_DEPENDENCIES >	ShaderIDs_t;

		// variables
			String							address;		// used for search
			mutable ShaderID				shader;
			String							header;			// TODO: use separated array to store headers without duplicates
			ShaderIDs_t						dependencies;
			EShaderCompilationFlags::type	flags;			// used for search
			EShader::type					shaderType;		// used for search

		// methods
			LoadedShader ();
			LoadedShader (StringCRef address, EShader::type type, EShaderCompilationFlags::type flags);

			bool operator >  (const LoadedShader &right) const;
			bool operator <  (const LoadedShader &right) const;
			bool operator == (const LoadedShader &right) const;
		};


		struct LoadedShaderHash
		{
			typedef LoadedShader				key_t;
			typedef Hash< String >::result_t	result_t;

			result_t operator () (const key_t &value) const
			{
				return	Hash< String >()( value.address ) +
						Hash< EShader::type >()( value.shaderType ) +
						Hash< EShaderCompilationFlags::type >()( value.flags );
			}
		};


		/*struct HeaderInfo
		{
		// variables
			String		source;
			ushort2		linesRange;		// offset, count

		// methods
			HeaderInfo () : linesRange(0) {}
			HeaderInfo (String &&source, uint firstLine, uint count) : source(source), linesRange(uint2(firstLine, count)) {}

			uint FirstLine ()	const	{ return linesRange.x; }
			uint NumLines ()	const	{ return linesRange.y; }
		};*/


		struct SourceInfo
		{
		// variables
			StringCRef	source;
			StringCRef	filename;
			ushort2		linesRange;		// offset, count
			
		// methods
			SourceInfo () : linesRange(0) {}
			SourceInfo (StringCRef source, StringCRef filename, uint2 linesRange) : source(source), filename(filename), linesRange(linesRange) {}
			
			uint FirstLine ()	const	{ return linesRange.x; }
			uint NumLines ()	const	{ return linesRange.y; }
		};


		typedef HashSet< LoadedShader, LoadedShaderHash >					LoadedShaders_t;
		//typedef HashMap< String, HeaderInfo >								UniqueHeaders_t;
		typedef StaticArray< Optional< LoadedShader >, EShader::_Count >	LoadedActiveShaders_t;


	// variables
	private:
		Array< String >		_includeDirs;
		Array< String >		_importDirs;
		LoadedShaders_t		_loadedShaders;			// cache for searching import shaders
		//UniqueHeaders_t		_headers;
		String				_debugOutputFolder;
		uint				_debugOutputNumber;
		bool				_copilationInProgress;
		bool				_dumpProgramInfo;


	// methods
	public:
		explicit
		ShaderManager (const SubSystemsRef ss);
		~ShaderManager ();

		void Clear ();

		bool AddIncludeDirectory (StringCRef dir);
		bool AddIncludeDirectories (Buffer<StringCRef> dirs);
		bool ClearIncludeDirectories ();

		bool AddImportDirectory (StringCRef dir);
		bool AddImportDirectories (Buffer<StringCRef> dirs);
		bool ClearImportDirectories ();
		
		bool ClearShaderCache ();

		bool LoadProgram (OUT ProgramID &program,
						  StringCRef filename,
						  ShaderBits_t activeShaders,
						  EShaderCompilationFlags::type compilationFlags,
						  const VertexAttribsState &input = Uninitialized,
						  const FragmentOutputState &output = Uninitialized);
		
		bool CompileProgram (OUT ProgramID &program,
							 StringCRef source,
							 Buffer<ShaderID> shaders,
							 ShaderBits_t activeShaders,
							 EShaderCompilationFlags::type compilationFlags,
							 const VertexAttribsState &input = Uninitialized,
							 const FragmentOutputState &output = Uninitialized);

		bool LoadShader (OUT Array<ShaderID> &shaders,
						 OUT String &header,
						 StringCRef filename,
						 EShader::type shaderType,
						 EShaderCompilationFlags::type compilationFlags);
		
		bool CompileShader (OUT Array<ShaderID> &shaders,
							OUT String &header,
							StringCRef source,
							EShader::type shaderType,
							EShaderCompilationFlags::type compilationFlags);
		
		StringCRef GetDefaultShaderOptions () const;

		bool SetDebugOutputFolder (StringCRef dir);

		void EnableDumpProgramInfo (bool value)		{ _dumpProgramInfo = value; }
		bool ProgramInfoDumpEnabled () const		{ return _dumpProgramInfo; }


	private:
		bool _ParseAndCompileShader (OUT LoadedActiveShaders_t &result, INOUT ShaderParsingData &parsingData, StringCRef filename);

		bool _SearchShaderInclude (OUT String &source, INOUT HashSet<String> &includedFiles, StringCRef include) const;

		bool _ImportShaders (OUT StringCRef &header, INOUT HashSet<String> &importedFiles, INOUT Array<ShaderID> &shaders,
							 StringCRef filename, ShaderBits_t activeShaders, EShaderCompilationFlags::type compilationFlags);

		bool _RecursiveParseShaders (INOUT ShaderParsingData& data);

		bool _WriteDebugOutput (Buffer<SourceInfo> source);

		void _ValidateIncludeAndImportDirs ();

		RFilePtr _SearchAndOpenFile (StringCRef filename) const;

		static void _RemoveDuplicateShaders (INOUT Array<ShaderID> &shaders);

		static bool _ParseShaderSource (StringCRef source, usize sourceIdx, usize insertTo, EParsingTag::type defaultTag,
										INOUT Array<Tag> &tags);
		
		static const char *	_GraphicsShaderExtension ()		{ return "glsl"; }
		static const char *	_ComputeShaderExtension ()		{ return "glcs"; }


	// api specific methods
	private:
		bool _CompileShader (INOUT LoadedShader &data, Buffer<SourceInfo> source, StringCRef filename) const;
	
		bool _LinkProgram (OUT ProgramID &prog, Buffer<ShaderID> shaders, ShaderBits_t activeShaders,
						   const VertexAttribsState &input, const FragmentOutputState &output,
						   StringCRef filename) const;

		void _ParseCompilationErrors (Buffer<SourceInfo> source, EShader::type shaderType, StringCRef log,
									  bool compiled, StringCRef filename) const;

		void _ParseLinkingErrors (StringCRef log, bool linked, StringCRef filename) const;
		
		static void _AddAttribs (OUT String &str, const VertexAttribsState &state);
		static void _AddOutputs (OUT String &str, const FragmentOutputState &frag);

		static void _DumpProgramResources (ProgramID prog, StringCRef filename);
		static void _DumpUniformBlocksInfo (ProgramID prog, INOUT String &str);
		static void _DumpBufferBlockInfo (ProgramID prog, INOUT String &str);
		static void _DumpUniformsInfo (ProgramID prog, INOUT String &str);
	};


}	// Graphics
}	// Engine
