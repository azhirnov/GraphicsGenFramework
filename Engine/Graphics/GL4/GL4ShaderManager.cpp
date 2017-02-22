// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Graphics/Engine/GraphicsEngine.h"
#include "Engine/Graphics/Texture/Sampler.h"
#include "GL4ObjectIDs.h"

namespace Engine
{
namespace Graphics
{
	
/*
=================================================
	GetDefaultShaderOptions
=================================================
*/
	StringCRef ShaderManager::GetDefaultShaderOptions () const
	{
		switch ( SubSystems()->Get< GraphicsEngine >()->VideoCardVendor() )
		{
			case EVideoCardVendor::AMD :
				return	"// AMD options:\n"
						"\n";

			case EVideoCardVendor::NVidia :
				return	"// NVidia options:\n"
						//"#pragma optionNV(fastmath on)\n"
						//"#pragma optionNV(fastprecision on)\n"
						//"#pragma optionNV(ifcvt none)\n"
						"#pragma optionNV(inline all)\n"
						//"#pragma optionNV(strict on)\n"
						"#pragma optionNV(unroll all)\n"
						;

			case EVideoCardVendor::Intel :
				return	"// Intel options:\n"
						"\n";
		}

		RETURN_ERR( "unknown vendor type!", "" );
	}
	
/*
=================================================
	_AddAttribs
=================================================
*/
	void ShaderManager::_AddAttribs (INOUT String &str, const VertexAttribsState &state)
	{
		uint	num_attribs = 0;
		
		FOR( i, state )
		{
			if ( not state[i].enabled )
				continue;

			++num_attribs;
		}

		if ( num_attribs == 0 )
			return;

		// shared
		str << "#define TAttribsData \\\n"
			<< "TAttribsData_t { \\\n";

		FOR( i, state )
		{
			if ( not state[i].enabled )
				continue;

			str << "\t" << EAttribute::ToString( state[i].type ) << " attrib" << state[i].index << ";\\\n";
		}

		str << "}\n\n";

		str << "#if SHADER == SH_VERTEX\n";

		FOR( i, state )
		{
			if ( not state[i].enabled )
				continue;

			str << "layout(location=" << state[i].index << ") in "
				<< EAttribute::ToString( state[i].type ) << " inAttrib" << state[i].index << ";\n";
		}
		str << "\n";
		
		FOR( i, state )
		{
			if ( not state[i].enabled )
				continue;

			str << "#define ATTRIB_" << state[i].index << " inAttrib" << state[i].index << "\n";
		}

		str << "#endif\t// SH_VERTEX\n\n";
	}
	
/*
=================================================
	_AddOutputs
=================================================
*/
	void ShaderManager::_AddOutputs (INOUT String &str, const FragmentOutputState &frag)
	{
		bool	initialized = false;

		FOR( i, frag )
		{
			if ( frag[i] == EFragOutput::Unknown )
				continue;

			if ( not initialized )
			{
				str << "#if SHADER == SH_FRAGMENT\n";
				initialized = true;
			}

			str << "layout(location=" << i << ") out "
				<< EFragOutput::ToString( frag[i] ) << " outColor" << i << ";\n";
		}
		
		if ( initialized )
			str << "#endif\n";
	}

/*
=================================================
	_CompileShader
=================================================
*/
	bool ShaderManager::_CompileShader (INOUT LoadedShader &shaderData, Buffer<SourceInfo> shaderSource, StringCRef filename) const
	{
		using namespace gl;

		static const char	shaderExtensions[] =
			"#extension GL_ARB_gpu_shader_int64 : enable\n";		// compilation not failed if not supported

		static const char *	perShaderExtensions[] = {
			// Vertex
			"",
			// TessControl
			"",
			// TessEvaluation
			"",
			// Geometry
			"",
			// Fragment
			//"#extension GL_ARB_shader_stencil_export : enable\n"
			"",
			// Compute
			""
		};
		STATIC_ASSERT( CountOf(perShaderExtensions) == EShader::_Count );

		static const char	shaderTypesStr[] = 
			"#define SH_VERTEX           0\n"
			"#define SH_TESS_CONTROL     1\n"
			"#define SH_TESS_EVALUATION  2\n"
			"#define SH_GEOMETRY         3\n"
			"#define SH_FRAGMENT         4\n"
			"#define SH_COMPUTE          5\n";
		
		static const char *	perShaderSource[] = {
			"#define SHADER  SH_VERTEX\n",			// Vertex
			"#define SHADER  SH_TESS_CONTROL\n",	// TessControl
			"#define SHADER  SH_TESS_EVALUATION\n",	// TessEvaluation
			"#define SHADER  SH_GEOMETRY\n",		// Geometry
			"#define SHADER  SH_FRAGMENT\n",		// Fragment
			"#define SHADER  SH_COMPUTE\n"			// Compute
		};
		STATIC_ASSERT( CountOf(perShaderSource) == EShader::_Count );

		static const char	baseTypesRedefinitionStr[] =
			"#define and		&&\n"
			"#define or			||\n"
			"#define not		!\n\n"

			"#define bool2		bvec2\n"
			"#define bool3		bvec3\n"
			"#define bool4		bvec4\n"

			"#define int2		ivec2\n"
			"#define int3		ivec3\n"
			"#define int4		ivec4\n"

			"#define uint2		uvec2\n"
			"#define uint3		uvec3\n"
			"#define uint4		uvec4\n"

			"#ifdef GL_ARB_gpu_shader_int64\n"

			"# define ilong		int64_t\n"
			"# define ilong2	i64vec2\n"
			"# define ilong3	i64vec3\n"
			"# define ilong4	i64vec4\n"

			"# define ulong		uint64_t\n"
			"# define ulong2	u64vec2\n"
			"# define ulong3	u64vec3\n"
			"# define ulong4	u64vec4\n"

			"#endif // GL_ARB_gpu_shader_int64\n"

			"#define float2		vec2\n"
			"#define float3		vec3\n"
			"#define float4		vec4\n"
			
			"#define float2x2	mat2\n"
			"#define float3x3	mat3\n"
			"#define float4x4	mat4\n"

			"#define double2	dvec2\n"
			"#define double3	dvec3\n"
			"#define double4	dvec4\n"
			
			"#define double2x2	dmat2\n"
			"#define double3x3	dmat3\n"
			"#define double4x4	dmat4\n";

		static const char	baseTypesFp32Str[] =
			"#define GX_FP_32	1\n"
			"#define real		float\n"
			"#define real2		float2\n"
			"#define real3		float3\n"
			"#define real4		float4\n"
			"#define real2x2	float2x2\n"
			"#define real3x3	float3x3\n"
			"#define real4x4	float4x4\n";

		static const char	baseTypesFp64Str[] =
			"#define GX_FP_64	1\n"
			"#define real		double\n"
			"#define real2		double2\n"
			"#define real3		double3\n"
			"#define real4		double4\n"
			"#define real2x2	double2x2\n"
			"#define real3x3	double3x3\n"
			"#define real4x4	double4x4\n";

		static const char	debugShaderStr[] =
			"#pragma optimize(off)\n"
			"#pragma debug(on)\n";
		
		static const char	computeVariableGroupSizeStr[] =
			"#extension GL_ARB_compute_variable_group_size : require\n"	// compilation failed if not supported
			"layout (local_size_variable) in;\n";
		
		// build source
		Array<const char *>		sources;
		StaticString< 64 >		shader_version;		shader_version << "#version " << gl::GL4_GetShaderVersion() << " core\n\n";

		sources << shader_version.cstr();
		sources << shaderExtensions;
		sources << perShaderExtensions[ shaderData.shaderType ];

		// Debug
		if ( EnumEq( shaderData.flags, EShaderCompilationFlags::Debug ) )
			sources << debugShaderStr;

		// VendorOptimizations
		if ( EnumEq( shaderData.flags, EShaderCompilationFlags::VendorOptimizations ) )
			sources << GetDefaultShaderOptions().cstr();

		// Variable work group size
		if ( shaderData.shaderType == EShader::Compute and
			 EnumEq( shaderData.flags, EShaderCompilationFlags::ComputeVariableGroupSize ) )
			sources << computeVariableGroupSizeStr;

		sources << shaderTypesStr;
		sources << perShaderSource[ shaderData.shaderType ];
		sources << baseTypesRedefinitionStr;

		// Float point size (single or double)
		if ( EnumEq( shaderData.flags, EShaderCompilationFlags::FP_32 ) )	sources << baseTypesFp32Str;	else
		if ( EnumEq( shaderData.flags, EShaderCompilationFlags::FP_64 ) )	sources << baseTypesFp64Str;

		sources << "#line 0\n";
		
		FOR( i, shaderSource ) {
			sources << shaderSource[i].source.cstr();
		}
		

		// compile shader
		Ptr< GraphicsContext >	gc = SubSystems()->Get< GraphicsEngine >()->GetContext();

		CHECK_ERR( gc->CreateShader( OUT shaderData.shader, shaderData.shaderType ) );

		GL_CALL( glShaderSource( shaderData.shader.Id(), GLsizei(sources.Count()), sources.ptr(), null ) );
		GL_CALL( glCompileShader( shaderData.shader.Id() ) );

		GLint	compile_status = 0;
		GL_CALL( glGetShaderiv( shaderData.shader.Id(), GL_COMPILE_STATUS,  &compile_status ) );

		const bool	compiled = ( compile_status == GL_TRUE );

		GLint	log_size = 0;
		GL_CALL( glGetShaderiv( shaderData.shader.Id(), GL_INFO_LOG_LENGTH, &log_size ) );

		if ( log_size > 5 )
		{
			String	info;
			info.Reserve( log_size+1 );
			
			GL_CALL( glGetShaderInfoLog( shaderData.shader.Id(), log_size, null, info.ptr() ) );
			info.CalculateLength();
			
			_ParseCompilationErrors( shaderSource, shaderData.shaderType, info, compiled, filename );
		}

		if ( not compiled )
		{
			gc->DeleteShader( shaderData.shader );
			RETURN_ERR( "compilation error!" );
		}

		return true;
	}
	
/*
=================================================
	_LinkProgram
=================================================
*/
	bool ShaderManager::_LinkProgram (OUT ProgramID &prog, Buffer<ShaderID> shaders, ShaderBits_t activeShaders,
									  const VertexAttribsState &input, const FragmentOutputState &output,
									  StringCRef filename) const
	{
		using namespace gl;
		
		Ptr< GraphicsContext >	gc = SubSystems()->Get< GraphicsEngine >()->GetContext();

		CHECK_ERR( gc->CreateProgram( OUT prog ) );
		
		FOR( i, shaders )
		{
			GL_CALL( glAttachShader( prog.Id(), shaders[i].Id() ) );
		}

		GL_CALL( glLinkProgram( prog.Id() ) );
		
		GLint	link_status = 0;
		GL_CALL( glGetProgramiv( prog.Id(), GL_LINK_STATUS, &link_status ) );

		const bool	linked = ( link_status == GL_TRUE );

		GLint	log_size = 0;
		GL_CALL( glGetProgramiv( prog.Id(), GL_INFO_LOG_LENGTH, &log_size ) );

		if ( log_size > 5 )
		{
			String	info;
			info.Reserve( log_size + 1 );

			GL_CALL( glGetProgramInfoLog( prog.Id(), log_size, null, info.ptr() ) );
			info.CalculateLength();
			
			_ParseLinkingErrors( info, linked, filename );
		}

		if ( not linked )
		{
			gc->DeleteProgram( prog );
			RETURN_ERR( "program linking error!" );
		}

		GLint	tess_patch = 0;

		if ( activeShaders.Get( EShader::TessControl ) and activeShaders.Get( EShader::TessEvaluation ) )
		{
			GL_CALL( glGetProgramiv( prog.Id(), GL_TESS_CONTROL_OUTPUT_VERTICES, &tess_patch ) );
		}

		prog = ProgramID( prog.Id(), tess_patch, input, output );
		return true;
	}
	
/*
=================================================
	_ParseCompilationErrors
=================================================
*/
	void ShaderManager::_ParseCompilationErrors (Buffer<SourceInfo> sources, EShader::type shaderType, StringCRef log,
												 bool compiled, StringCRef filename) const
	{
		// pattern:	<number> ( <line> ) : <error/warning> <code>: <description>

		Array< StringCRef >		lines;
		Array< StringCRef >		tokens;
		String					str;		str.Reserve( log.Length() * 2 );
		usize					prev_line = -1;

		str << EShader::ToString( shaderType ) << " shader \"" << filename << "\" compilation "
			<< (compiled ? "message" : "error") << "\n---------------\n";

		StringParser::DivideLines( log, OUT lines );

		FOR( i, lines )
		{
			StringParser::DivideString_CPP( lines[i], OUT tokens );

			bool	added = false;

			if ( tokens.Count() > 8 and
				 tokens[1] == "(" and
				 tokens[3] == ")" and
				 tokens[4] == ":" )
			{
				const usize	line		= (usize) StringUtils::ToInt32( tokens[2] );
				const bool	is_err		= tokens[5].EqualsIC( "error" );
				usize		cur_line	= 0;
				usize		pos			= 0;
				StringCRef	line_str;

				// unite error in same source lines
				if ( prev_line == line )
				{
					str << lines[i] << "\n";
					continue;
				}

				prev_line = line;
				
				FOR( j, sources )
				{
					cur_line += sources[j].NumLines();

					if ( cur_line <= line )
						continue;

					usize	local_line	= line - (cur_line - sources[j].NumLines());
					usize	file_line	= local_line + sources[j].FirstLine();

					CHECK( local_line < sources[j].NumLines() );

					CHECK( StringParser::MoveToLine( sources[j].source, INOUT pos, local_line ) );

					StringParser::ReadLineToEnd( sources[j].source, INOUT pos, OUT line_str );

					str << "in source \"" << sources[j].filename << "\" (" << file_line << "):\n\""
						<< line_str << "\"\n" << lines[i] << "\n";
					
					added = true;
					break;
				}
			}

			if ( not added )
			{
				str << "<unknown message> " << lines[i] << "\n";
			}
		}

		LOG( str.cstr(), compiled ? (ELog::Debug | ELog::SpoilerFlag) : (ELog::Error | ELog::OpenSpoilerFlag) );
	}
	
/*
=================================================
	_ParseLinkingErrors
=================================================
*/
	void ShaderManager::_ParseLinkingErrors (StringCRef log, bool linked, StringCRef filename) const
	{
		String	str;

		str << "Program \"" << filename << "\" linking "
			<< (linked ? "message" : "error") << "\n---------------\n" << log;

		// TODO: parse errors

		LOG( str.cstr(), linked ? (ELog::Debug | ELog::SpoilerFlag) : (ELog::Error | ELog::OpenSpoilerFlag) );
	}
	
/*
=================================================
	GLVariableTypeToStr
=================================================
*/
	static const char * GLVariableTypeToStr (gl::GLenum type)
	{
		using namespace gl;

		switch ( type )
		{
			case GL_BOOL :										return "bool";
			case GL_BOOL_VEC2 :									return "bool2";
			case GL_BOOL_VEC3 :									return "bool3";
			case GL_BOOL_VEC4 :									return "bool4";
			case GL_FLOAT :										return "float";
			case GL_FLOAT_VEC2 :								return "float2";
			case GL_FLOAT_VEC3 :								return "float3";
			case GL_FLOAT_VEC4 :								return "float4";
			case GL_FLOAT_MAT2 :								return "float2x2";
			case GL_FLOAT_MAT3 :								return "float3x3";
			case GL_FLOAT_MAT4 :								return "float4x4";
			case GL_FLOAT_MAT2x3 :								return "float2x3";
			case GL_FLOAT_MAT2x4 :								return "float2x4";
			case GL_FLOAT_MAT3x2 :								return "float3x2";
			case GL_FLOAT_MAT3x4 :								return "float3x4";
			case GL_FLOAT_MAT4x2 :								return "float4x2";
			case GL_FLOAT_MAT4x3 :								return "float4x3";
			case GL_DOUBLE :									return "double";
			case GL_DOUBLE_VEC2 :								return "double2";
			case GL_DOUBLE_VEC3 :								return "double3";
			case GL_DOUBLE_VEC4 :								return "double4";
			case GL_DOUBLE_MAT2 :								return "double2x2";
			case GL_DOUBLE_MAT3 :								return "double3x3";
			case GL_DOUBLE_MAT4 :								return "double4x4";
			case GL_DOUBLE_MAT2x3 :								return "double2x3";
			case GL_DOUBLE_MAT2x4 :								return "double2x4";
			case GL_DOUBLE_MAT3x2 :								return "double3x2";
			case GL_DOUBLE_MAT3x4 :								return "double3x4";
			case GL_DOUBLE_MAT4x2 :								return "double4x2";
			case GL_DOUBLE_MAT4x3 :								return "double4x3";
			case GL_INT :										return "int";
			case GL_INT_VEC2 :									return "int2";
			case GL_INT_VEC3 :									return "int3";
			case GL_INT_VEC4 :									return "int4";
			case GL_UNSIGNED_INT :								return "uint";
			case GL_UNSIGNED_INT_VEC2 :							return "uint2";
			case GL_UNSIGNED_INT_VEC3 :							return "uint3";
			case GL_UNSIGNED_INT_VEC4 :							return "uint4";
			case GL_SAMPLER_1D :								return "sampler1D";
			case GL_SAMPLER_2D :								return "sampler2D";
			case GL_SAMPLER_3D :								return "sampler3D";
			case GL_SAMPLER_CUBE :								return "samplerCube";
			case GL_SAMPLER_1D_SHADOW :							return "sampler1DShadow";
			case GL_SAMPLER_2D_SHADOW :							return "sampler2DShadow";
			case GL_SAMPLER_2D_MULTISAMPLE :					return "sampler2DMS";
			case GL_SAMPLER_2D_MULTISAMPLE_ARRAY :				return "sampler2DMSArray";
			//case GL_SAMPLER_CUBE_MAP_ARRAY_ARB :				return "";
			//case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW_ARB :		return "";
			case GL_SAMPLER_1D_ARRAY :							return "sampler1DArray";
			case GL_SAMPLER_2D_ARRAY :							return "sampler2DArray";
			case GL_SAMPLER_1D_ARRAY_SHADOW :					return "sampler1DArrayShadow";
			case GL_SAMPLER_2D_ARRAY_SHADOW :					return "sampler2DArrayShadow";
			case GL_SAMPLER_CUBE_SHADOW :						return "samplerCubeShadow";
			case GL_SAMPLER_BUFFER :							return "samplerBuffer";
			case GL_SAMPLER_2D_RECT :							return "sampler2DRect";
			case GL_SAMPLER_2D_RECT_SHADOW :					return "sampler2DRectShadow";
			case GL_INT_SAMPLER_1D :							return "isampler1D";
			case GL_INT_SAMPLER_2D :							return "isampler2D";
			case GL_INT_SAMPLER_3D :							return "isampler3D";
			case GL_INT_SAMPLER_CUBE :							return "isamplerCube";
			case GL_INT_SAMPLER_1D_ARRAY :						return "isampler1DArray";
			case GL_INT_SAMPLER_2D_ARRAY :						return "isampler2DArray";
			case GL_INT_SAMPLER_BUFFER :						return "isamplerBuffer";
			case GL_INT_SAMPLER_2D_RECT :						return "isampler2DRect";
			//case GL_INT_SAMPLER_CUBE_MAP_ARRAY_ARB :			return "";
			case GL_INT_SAMPLER_2D_MULTISAMPLE :				return "isampler2DMS";
			case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY :			return "isampler2DMSArray";
			case GL_UNSIGNED_INT_SAMPLER_1D :					return "usampler1D";
			case GL_UNSIGNED_INT_SAMPLER_2D :					return "usampler2D";
			case GL_UNSIGNED_INT_SAMPLER_3D :					return "usampler3D";
			case GL_UNSIGNED_INT_SAMPLER_CUBE :					return "usamplerCube";
			case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY :				return "usampler1DArray";
			case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY :				return "usampler2DArray";
			case GL_UNSIGNED_INT_SAMPLER_BUFFER :				return "usamplerBuffer";
			case GL_UNSIGNED_INT_SAMPLER_2D_RECT :				return "usampler2DRect";
			//case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY_ARB :	return "";
			case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE :		return "usampler2DMS";
			case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY :	return "usampler2DMSArray";
			case GL_IMAGE_1D :									return "image1D";
			case GL_IMAGE_2D :									return "image2D";
			case GL_IMAGE_3D :									return "image3D";
			case GL_IMAGE_2D_RECT :								return "image2DRect";
			case GL_IMAGE_CUBE :								return "imageCube";
			case GL_IMAGE_BUFFER :								return "imageBuffer";
			case GL_IMAGE_1D_ARRAY :							return "image1DArray";
			case GL_IMAGE_2D_ARRAY :							return "image2DArray";
			case GL_IMAGE_CUBE_MAP_ARRAY :						return "imageCubeArray";
			case GL_IMAGE_2D_MULTISAMPLE :						return "image2DMS";
			case GL_IMAGE_2D_MULTISAMPLE_ARRAY :				return "image2DMSArray";
			case GL_INT_IMAGE_1D :								return "iimage1D";
			case GL_INT_IMAGE_2D :								return "iimage2D";
			case GL_INT_IMAGE_3D :								return "iimage3D";
			case GL_INT_IMAGE_2D_RECT :							return "iimage2DRect";
			case GL_INT_IMAGE_CUBE :							return "iimageCube";
			case GL_INT_IMAGE_BUFFER :							return "iimageBuffer";
			case GL_INT_IMAGE_1D_ARRAY :						return "iimage1DArray";
			case GL_INT_IMAGE_2D_ARRAY :						return "iimage2DArray";
			case GL_INT_IMAGE_CUBE_MAP_ARRAY :					return "iimageCubeArray";
			case GL_INT_IMAGE_2D_MULTISAMPLE :					return "iimage2DMS";
			case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY :			return "iimage2DMSArray";
			case GL_UNSIGNED_INT_IMAGE_1D :						return "uimage1D";
			case GL_UNSIGNED_INT_IMAGE_2D :						return "uimage2D";
			case GL_UNSIGNED_INT_IMAGE_3D :						return "uimage3D";
			case GL_UNSIGNED_INT_IMAGE_2D_RECT :				return "uimage2DRect";
			case GL_UNSIGNED_INT_IMAGE_CUBE :					return "uimageCube";
			case GL_UNSIGNED_INT_IMAGE_BUFFER :					return "uimageBuffer";
			case GL_UNSIGNED_INT_IMAGE_1D_ARRAY :				return "uimage1DArray";
			case GL_UNSIGNED_INT_IMAGE_2D_ARRAY :				return "uimage2DArray";
			case GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY :			return "uimageCubeArray";
			case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE :			return "uimage2DMS";
			case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY :	return "uimage2DMSArray";
		}

		WARNING( "Unknown OpenGL type!" );
		return "<unknown>";
	}
	
/*
=================================================
	_DumpProgramResources
=================================================
*/
	void ShaderManager::_DumpProgramResources (ProgramID prog, StringCRef filename)
	{
		CHECK( prog.IsValid() );

		String	str;	str.Reserve( 256 );

		str << "Program \"" << filename << "\" resources\n---------------\n";

		_DumpUniformBlocksInfo( prog, INOUT str );
		_DumpBufferBlockInfo( prog, INOUT str );
		_DumpUniformsInfo( prog, INOUT str );

		LOG( str.cstr(), ELog::Info | ELog::SpoilerFlag );
	}

/*
=================================================
	_DumpUniformBlocksInfo
=================================================
*/
	void ShaderManager::_DumpUniformBlocksInfo (ProgramID prog, INOUT String &str)
	{
		using namespace gl;
		
		static const GLenum ubProperties[] = {
			GL_NUM_ACTIVE_VARIABLES,
			GL_BUFFER_BINDING,
			GL_BUFFER_DATA_SIZE,
		};
		static const GLenum varArrayProps[] = {
			GL_ACTIVE_VARIABLES
		};
		static const GLenum varProps[] = {
			GL_TYPE,
			GL_OFFSET,
			GL_ARRAY_SIZE,
			GL_ARRAY_STRIDE,
			GL_LOCATION
		};

		CHECK( prog.IsValid() );

		GLint			ub_count		= 0;
		GLint			max_ub_length	= 0;
		GLint			max_var_length	= 0;
		String			buf;
		GLsizei			length			= 0;
		GLint			index			= 0;
		Array<GLint>	var_indices;
		GLint			ub_params[ CountOf(ubProperties) ] = {};
		GLint			var_params[ CountOf(varProps) ]	   = {};

		GL_CALL( glGetProgramInterfaceiv( prog.Id(), GL_UNIFORM_BLOCK,  GL_ACTIVE_RESOURCES, &ub_count ) );
		GL_CALL( glGetProgramInterfaceiv( prog.Id(), GL_UNIFORM_BLOCK,  GL_MAX_NAME_LENGTH,  &max_ub_length ) );
		GL_CALL( glGetProgramInterfaceiv( prog.Id(), GL_UNIFORM,		GL_MAX_NAME_LENGTH,  &max_var_length ) );

		if ( ub_count <= 0 )
			return;

		buf.Reserve( Max( max_ub_length, max_var_length ) );
		
		for (int i = 0; i < ub_count; ++i)
		{
			GLuint	idx = (GLuint) i;

			GL_CALL( glGetProgramResourceName( prog.Id(), GL_UNIFORM_BLOCK, idx, max_ub_length, &length, buf.ptr() ) );
			buf.SetLength( length );

			GL_CALL( index = glGetProgramResourceIndex( prog.Id(), GL_UNIFORM_BLOCK, buf.cstr() ) );

			GL_CALL( glGetProgramResourceiv( prog.Id(), GL_UNIFORM_BLOCK, index, GLsizei(CountOf( ubProperties )),
											 ubProperties, GLsizei(CountOf( ub_params )), null, ub_params ) );

			GLint	ub_binding	= ub_params[1];
			GLint	ub_size		= ub_params[2];

			var_indices.Resize( ub_params[0] );
			GL_CALL( glGetProgramResourceiv( prog.Id(), GL_UNIFORM_BLOCK, index, GLsizei(CountOf( varArrayProps )), varArrayProps,
											 GLsizei(var_indices.Count()), null, var_indices.ptr() ) );
			
			str << "UBO     " << buf << ", index: " << index << ", binding: " << ub_binding
				<< ", size: " << ToString( BytesU( ub_size ) ) << '\n';

			FOR( j, var_indices )
			{
				GL_CALL( glGetProgramResourceiv( prog.Id(), GL_UNIFORM, var_indices[j], GLsizei(CountOf( varProps )), varProps,
												 GLsizei(CountOf( var_params )), null, var_params ) );

				GL_CALL( glGetProgramResourceName( prog.Id(), GL_UNIFORM, var_indices[j], max_var_length, &length, buf.ptr() ) );
				buf.SetLength( length );

				GLenum	var_type		= var_params[0];
				GLint	var_offset		= var_params[1];
				GLint	array_size		= var_params[2];
				//GLint	array_stride	= var_params[3];
				GLint	location		= var_params[4];

				str << "    " << GLVariableTypeToStr( var_type ) << "  " << buf;

				if ( array_size > 1 )
					str << '[' << array_size << ']';

				str << ", offset: " << var_offset << ", location: " << location << "\n";
			}
		}
	}
	
/*
=================================================
	_DumpBufferBlockInfo
=================================================
*/
	void ShaderManager::_DumpBufferBlockInfo (ProgramID prog, INOUT String &str)
	{
		using namespace gl;

		static const GLenum ssbProperties[] = {
			GL_NUM_ACTIVE_VARIABLES,
			GL_BUFFER_BINDING,
			GL_BUFFER_DATA_SIZE,
		};
		static const GLenum varArrayProps[] = {
			GL_ACTIVE_VARIABLES
		};
		static const GLenum varProps[] = {
			GL_TYPE,
			GL_OFFSET,
			GL_ARRAY_SIZE,
			GL_ARRAY_STRIDE,
			GL_TOP_LEVEL_ARRAY_SIZE,
			GL_TOP_LEVEL_ARRAY_STRIDE
		};

		CHECK( prog.IsValid() );

		GLint			ssb_count		= 0,
						max_ssb_length	= 0,
						max_var_length	= 0;
		String			buf;
		GLsizei			length			= 0;
		GLint			index			= 0;
		Array<GLint>	var_indices;
		GLint			ssb_params[ CountOf(ssbProperties) ] = {};
		GLint			var_params[ CountOf(varProps) ]		 = {};

		GL_CALL( glGetProgramInterfaceiv( prog.Id(), GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &ssb_count ) );
		GL_CALL( glGetProgramInterfaceiv( prog.Id(), GL_SHADER_STORAGE_BLOCK, GL_MAX_NAME_LENGTH,  &max_ssb_length ) );
		GL_CALL( glGetProgramInterfaceiv( prog.Id(), GL_BUFFER_VARIABLE,      GL_MAX_NAME_LENGTH,  &max_var_length ) );

		if ( ssb_count <= 0 )
			return;

		buf.Resize( Max( max_ssb_length, max_var_length ) );

		for (int i = 0; i < ssb_count; ++i)
		{
			GLuint	idx = (GLuint) i;

			GL_CALL( glGetProgramResourceName( prog.Id(), GL_SHADER_STORAGE_BLOCK, idx, max_ssb_length, &length, buf.ptr() ) );
			buf.SetLength( length );

			GL_CALL( index = glGetProgramResourceIndex( prog.Id(), GL_SHADER_STORAGE_BLOCK, buf.cstr() ) );

			GL_CALL( glGetProgramResourceiv( prog.Id(), GL_SHADER_STORAGE_BLOCK, index, GLsizei(CountOf( ssbProperties )),
											 ssbProperties, GLsizei(CountOf( ssb_params )), null, ssb_params ) );
			
			GLint	ssb_binding	= ssb_params[1];
			GLint	ssb_size	= ssb_params[2];

			var_indices.Resize( ssb_params[0] );
			GL_CALL( glGetProgramResourceiv( prog.Id(), GL_SHADER_STORAGE_BLOCK, index, GLsizei(CountOf( varArrayProps )), varArrayProps,
											 GLsizei(var_indices.Count()), null, var_indices.ptr() ) );

			str << "SSBO    " << buf << ", index: " << index << ", binding: " << ssb_binding
				<< ", size: " << ToString( BytesU( ssb_size ) ) << '\n';

			FOR( j, var_indices )
			{
				GL_CALL( glGetProgramResourceiv( prog.Id(), GL_BUFFER_VARIABLE, var_indices[j], GLsizei(CountOf( varProps )), varProps,
												 GLsizei(CountOf( var_params )), null, var_params ) );

				GL_CALL( glGetProgramResourceName( prog.Id(), GL_BUFFER_VARIABLE, var_indices[j], max_var_length, &length, buf.ptr() ) );
				buf.SetLength( length );

				GLenum	var_type		= var_params[0];
				GLint	var_offset		= var_params[1];
				GLint	array_size		= var_params[2];
				//GLint	array_stride	= var_params[3];
				//GLint	tl_arr_size		= var_params[4];
				GLint	tl_arr_stride	= var_params[5];

				str << "    " << GLVariableTypeToStr( var_type ) << "  " << buf;

				if ( array_size > 1 )
					str << '[' << array_size << ']';

				str << ", offset: " << var_offset << ", stride: " << tl_arr_stride << "\n";
			}
		}
	}

/*
=================================================
	_DumpUniformsInfo
=================================================
*/
	void ShaderManager::_DumpUniformsInfo (ProgramID prog, INOUT String &str)
	{
		using namespace gl;

		static const GLenum varProps[] = {
			GL_TYPE,
			GL_LOCATION,
			GL_ARRAY_SIZE,
			GL_OFFSET,
			GL_BLOCK_INDEX
		};

		CHECK( prog.IsValid() );
		
		GLint	count			= 0,
				max_name_length = 0,
				length			= 0;
		GLuint	id				= prog.Id();
		String	name;
		GLint	var_params[ CountOf(varProps) ] = {0};

		GL_CALL( glGetProgramInterfaceiv( id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &count ) );
		GL_CALL( glGetProgramInterfaceiv( id, GL_UNIFORM, GL_MAX_NAME_LENGTH, &max_name_length ) );
		
		if ( count <= 0 )
			return;

		name.Resize( max_name_length );
		
		for (int i = 0; i < count; ++i)
		{
			GL_CALL( glGetProgramResourceiv( id, GL_UNIFORM, GLuint(i), GLsizei(CountOf( varProps )), varProps,
											 GLsizei(CountOf( var_params )), &length, var_params ) );

			GL_CALL( glGetProgramResourceName( id, GL_UNIFORM, i, max_name_length, &length, name.ptr() ) );
			name.SetLength( length );

			const GLenum	type		= var_params[0];
			const GLint		location	= var_params[1];
			const GLuint	array_size	= var_params[2];
			//const GLint		offset		= var_params[3];
			const GLint		blockidx	= var_params[4];
			
			if ( blockidx != -1 )
				continue;

			str << "uniform " << GLVariableTypeToStr( type ) << "  " << name;

			if ( array_size > 1 )
				str << '[' << array_size << ']';

			str << ", location: " << location << '\n';
		}
	}

}	// Graphics
}	// Engine
