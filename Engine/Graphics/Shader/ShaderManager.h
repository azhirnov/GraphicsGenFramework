// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.
/*
	Shaders support #include and #import directives.
	#include - attach source code
	#import  - compile shader and attach header source only.
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
		typedef Bitfield< EShader::_COUNT >		ShaderBits_t;

	private:
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
			EParsingTag::type	tag;

		// methods
			Tag () : sourceIdx(-1), offset(0), tag(EParsingTag::None) {}
			Tag (usize sourceIdx, usize offset, EParsingTag::type tag) : sourceIdx(sourceIdx), offset(offset), tag(tag) {}

			// for sorting
			bool operator > (const Tag &right) const	{ return offset > right.offset; }
		};


		struct ShaderSource : CompileTime::FastCopyable
		{
		// variables
			String		sourceStr;
			bool		isInHeader;		// if attachment in header then header attached to header, otherwise attachet to source
			
		// methods
			ShaderSource () : isInHeader(false) {}
			ShaderSource (StringCRef src, bool inHeader) : sourceStr(src), isInHeader(inHeader) {}
			ShaderSource (String &&src, bool inHeader) : sourceStr( RVREF(src) ), isInHeader(inHeader) {}
		};


		struct ShaderParsingData : CompileTime::FastCopyable
		{
		// variables
			Array< ShaderSource >			sources;
			Array< usize2 >					headerRef;		// {source index, offset}
			Array< usize2 >					sourceRef;		// {source index, offset}
			HashSet< String >				includedFiles;
			HashSet< String >				importedFiles;
			Array< ShaderID >				dependentShaders;
			ShaderBits_t					activeShaders;
			EShaderCompilationFlags::type	compilationFlags;
			VertexAttribsState				vertexAttribs;
			FragmentOutputState				fragmentOutput;

		// methods
			ShaderParsingData () : compilationFlags( EShaderCompilationFlags::None )
			{}

			String &	Src (usize i)	{ return sources[i].sourceStr; }
			String &	SrcBack ()		{ return sources.Back().sourceStr; }
		};


		struct LoadedShader : CompileTime::FastCopyable
		{
		// types
			typedef FixedSizeArray< ShaderID, 16 >	ShaderIDs_t;

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

		typedef Set< LoadedShader >											LoadedShaders_t;
		typedef StaticArray< Optional< LoadedShader >, EShader::_COUNT >	LoadedActiveShaders_t;


	// variables
	private:
		Array< String >		_includeDirs;
		Array< String >		_importDirs;
		LoadedShaders_t		_loadedShaders;			// cache for searching import shaders
		String				_debugOutputFolder;
		uint				_debugOutputNumber;
		bool				_copilationInProgress;


	// methods
	public:
		explicit
		ShaderManager (const SubSystemsRef ss);
		~ShaderManager ();

		void Clear ();

		bool AddIncludeDirectory (StringCRef dir);
		bool AddIncludeDirectories (Buffer<StringCRef> dirs);
		void ClearIncludeDirectories ();

		bool AddImportDirectory (StringCRef dir);
		bool AddImportDirectories (Buffer<StringCRef> dirs);
		void ClearImportDirectories ();
		
		void ClearShaderCache ();

		bool LoadProgram (OUT ProgramID &program,
						  StringCRef filename,
						  ShaderBits_t activeShaders,
						  EShaderCompilationFlags::type compilationFlags,
						  const VertexAttribsState &input = Uninitialized(),
						  const FragmentOutputState &output = Uninitialized());
		
		bool CompileProgram (OUT ProgramID &program,
							 StringCRef source,
							 Buffer<ShaderID> shaders,
							 ShaderBits_t activeShaders,
							 EShaderCompilationFlags::type compilationFlags,
							 const VertexAttribsState &input = Uninitialized(),
							 const FragmentOutputState &output = Uninitialized());

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

		void SetDebugOutputFolder (StringCRef dir);


	private:
		bool _ParseAndCompileShader (OUT LoadedActiveShaders_t &result, INOUT ShaderParsingData &parsingData, StringCRef filename);

		bool _SearchShaderInclude (OUT String &source, INOUT HashSet<String> &includedFiles, StringCRef include) const;

		bool _ImportShaders (OUT StringCRef &header, INOUT HashSet<String> &importedFiles, INOUT Array<ShaderID> &shaders,
							 StringCRef filename, ShaderBits_t activeShaders, EShaderCompilationFlags::type compilationFlags);

		bool _RecursiveParseShaders (INOUT ShaderParsingData& data);

		bool _WriteDebugOutput (Buffer<const char*> source);

		void _ValidateIncludeAndImportDirs ();

		static void _RemoveDuplicateShaders (INOUT Array<ShaderID> &shaders);

		static bool _ParseShaderSource (StringCRef source, usize sourceIdx, usize insertTo, INOUT Array<Tag> &tags);


	// api specific methods
	private:
		bool _CompileShader (INOUT LoadedShader &data, Buffer<const char*> source) const;
	
		bool _LinkProgram (OUT ProgramID &prog, Buffer<ShaderID> shaders, ShaderBits_t activeShaders,
							const VertexAttribsState &input, const FragmentOutputState &output) const;

		void _ParseCompilationErrors (Buffer<const char*> source, EShader::type shaderType, StringCRef log, bool compiled) const;
		void _ParseLinkingErrors (StringCRef log, bool linked) const;
		
		static void _AddAttribs (OUT String &str, const VertexAttribsState &state);
		static void _AddOutputs (OUT String &str, const FragmentOutputState &frag);

		static void _DumpProgramResources (ProgramID prog);
		static void _DumpUniformBlocksInfo (ProgramID prog, INOUT String &str);
		static void _DumpBufferBlockInfo (ProgramID prog, INOUT String &str);
		static void _DumpUniformsInfo (ProgramID prog, INOUT String &str);
	};


}	// Graphics
}	// Engine
