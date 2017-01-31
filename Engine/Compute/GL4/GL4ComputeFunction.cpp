// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "GL4ComputeFunction.h"
#include "Engine/Compute/Engine/ComputeEngine.h"

#if defined( COMPUTE_API_OPENGL )

namespace Engine
{
namespace Compute
{
	
/*
=================================================
	constructor
=================================================
*/
	GL4ComputeFunction::GL4ComputeFunction ()
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	GL4ComputeFunction::~GL4ComputeFunction ()
	{
		Destroy();
	}
	
/*
=================================================
	Create
=================================================
*/
	bool GL4ComputeFunction::Create (const ComputeProgramPtr &program, StringCRef name)
	{
		Destroy();

		CHECK_ERR( program and program->IsCreated() );
		
		_program = program;

		String	src;
		
		src << "#define kernel_" << name << " 1\n"
			<< program->_header;

		CHECK_ERR( program->SubSystems()->Get< GraphicsEngine >()->GetShaderManager()->
			CompileProgram( OUT _progID, src, program->_shaders,
							ShaderManager::ShaderBits_t().Set( EShader::Compute ),
							program->_flags ) );

		_InitArgs();
		return true;
	}
	
/*
=================================================
	Load
=================================================
*/
	bool GL4ComputeFunction::Load (const SubSystemsRef ss, StringCRef filename, EShaderCompilationFlags::type flags)
	{
		Destroy();

		_program = ComputeProgram::New( ss );

		_program->_flags = flags;

		CHECK_ERR( ss->Get< GraphicsEngine >()->GetShaderManager()->
			LoadProgram( OUT _progID, filename, ShaderManager::ShaderBits_t().Set( EShader::Compute ), _program->_flags ) );

		if ( not EnumEq( _program->_flags, EShaderCompilationFlags::ComputeVariableGroupSize ) )
		{
			using namespace gl;
			GL_CALL( glGetProgramiv( _progID.Id(), GL_COMPUTE_WORK_GROUP_SIZE, (int *)_fixedGroupSize.ptr() ) );
		}

		_InitArgs();
		return true;
	}
	
/*
=================================================
	Destroy
=================================================
*/
	void GL4ComputeFunction::Destroy ()
	{
		if ( _program and _progID.IsValid() )
		{
			_program->SubSystems()->Get< GraphicsEngine >()->GetContext()->DeleteProgram( _progID );
		}
		CHECK( not _progID.IsValid() );

		_program = null;
		_args.Clear();
	}
	
/*
=================================================
	CalcLocalGroupSize
=================================================
*/
	static void CalcLocalGroupSize (INOUT uint3 &localSize, const uint3 &globalSize, const uint3 &maxLocalGroupSize, uint maxInvocations)
	{
		//localSize = (size > uint3(0)).To<uint3>();
		localSize = globalSize;

		for (;;)
		{
			localSize = Max( localSize >> 1, uint3(1) );

			if ( localSize.Volume() < maxInvocations	and
				 All( localSize < maxLocalGroupSize )	and
				 All( globalSize % localSize == uint3(0) ) )
				break;
		}
	}
	
/*
=================================================
	_RunFixedGroupSize
=================================================
*/
	void GL4ComputeFunction::_RunFixedGroupSize (const uint3 &size)
	{
		using namespace gl;

		Ptr< GL4StateManager >	state_mngr	= _program->SubSystems()->Get< GraphicsEngine >()->GetStateManager();
		const uint3				global_size	= Max( size, uint3(1) );
		const uint3				local_size	= Max( _fixedGroupSize, uint3(1) );
		const uint3				group_size	= Max( global_size / local_size, uint3(1) );
		
		CHECK( All( global_size % local_size == uint3(0) ) );
		CHECK( All( group_size  * local_size == global_size ) );
		
		DEBUG_ONLY( _CheckArgs() );

		state_mngr->BindProgram( _progID );

		state_mngr->DispatchCompute( group_size );
	}
	
/*
=================================================
	_RunVariableGroupSize
=================================================
*/
	void GL4ComputeFunction::_RunVariableGroupSize (const uint3 &size, const uint3 &localSize)
	{
		using namespace gl;

		Ptr< ComputeEngine >	comp_eng	= _program->SubSystems()->Get< ComputeEngine >();
		Ptr< GL4StateManager >	state_mngr	= _program->SubSystems()->Get< GraphicsEngine >()->GetStateManager();
		uint3					local_size	= localSize;
		const uint3				global_size	= Max( size, uint3(1) );

		if ( IsZero( local_size ) )
			CalcLocalGroupSize( INOUT local_size, global_size, comp_eng->GetMaxWorkGroupSize(), comp_eng->GetMaxWorkGroupInvocations() );

		local_size = Max( local_size, uint3(1) );

		const uint3	group_size	= Max( global_size / local_size, uint3(1) );

		CHECK( All( global_size % local_size == uint3(0) ) );
		CHECK( All( group_size  * local_size == global_size ) );

		CHECK( All( local_size     < comp_eng->GetMaxWorkGroupSize() ) );
		CHECK( local_size.Volume() < comp_eng->GetMaxWorkGroupInvocations() );

		DEBUG_ONLY( _CheckArgs() );

		_SetArgs();
		
		state_mngr->BindProgram( _progID );

		state_mngr->DispatchCompute( group_size, local_size );
	}

/*
=================================================
	Run
=================================================
*/
	void GL4ComputeFunction::Run (const uint3 &size, const uint3 &localSize)
	{
		CHECK( IsCreated() );
		
		if ( EnumEq( _program->_flags, EShaderCompilationFlags::ComputeVariableGroupSize ) )
		{
			_RunVariableGroupSize( size, localSize );
		}
		else
		{
			_RunFixedGroupSize( size );
		}

		// if you copy-paste function call and forget rename,
		// assert will triggered on _CheckArgs()
		DEBUG_ONLY( ResetArgs(); )
	}
	
/*
=================================================
	GetTextureUniformTypes
=================================================
*/
	static Set<gl::GLenum> __GetTextureUniformTypes ()
	{
		using namespace gl;

		static const GLenum textureUniformTypes[] = {
			GL_SAMPLER_1D, GL_SAMPLER_2D, GL_SAMPLER_3D, GL_SAMPLER_CUBE, GL_SAMPLER_1D_SHADOW,
			GL_SAMPLER_2D_SHADOW, GL_SAMPLER_2D_MULTISAMPLE, GL_SAMPLER_2D_MULTISAMPLE_ARRAY, GL_SAMPLER_1D_ARRAY,
			GL_SAMPLER_2D_ARRAY, GL_SAMPLER_1D_ARRAY_SHADOW, GL_SAMPLER_2D_ARRAY_SHADOW, GL_SAMPLER_CUBE_SHADOW,
			GL_SAMPLER_BUFFER, GL_SAMPLER_2D_RECT, GL_SAMPLER_2D_RECT_SHADOW, GL_INT_SAMPLER_1D, GL_INT_SAMPLER_2D,
			GL_INT_SAMPLER_3D, GL_INT_SAMPLER_CUBE, GL_INT_SAMPLER_1D_ARRAY, GL_INT_SAMPLER_2D_ARRAY, GL_INT_SAMPLER_BUFFER,
			GL_INT_SAMPLER_2D_RECT, GL_INT_SAMPLER_2D_MULTISAMPLE, GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
			GL_UNSIGNED_INT_SAMPLER_1D, GL_UNSIGNED_INT_SAMPLER_2D, GL_UNSIGNED_INT_SAMPLER_3D, GL_UNSIGNED_INT_SAMPLER_CUBE,
			GL_UNSIGNED_INT_SAMPLER_1D_ARRAY, GL_UNSIGNED_INT_SAMPLER_2D_ARRAY, GL_UNSIGNED_INT_SAMPLER_BUFFER,
			GL_UNSIGNED_INT_SAMPLER_2D_RECT, GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE, GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
		};

		Set<GLenum> texture_types;
		texture_types.AddArray( textureUniformTypes );

		return texture_types;
	}

	static Set<gl::GLenum> const& GetTextureUniformTypes ()
	{
		static const Set<gl::GLenum>	texture_types = RVREF( __GetTextureUniformTypes() );
		return texture_types;
	}
	
/*
=================================================
	GetImageUniformTypes
=================================================
*/
	static Set<gl::GLenum> __GetImageUniformTypes ()
	{
		using namespace gl;

		static const GLenum imageUniformTypes[] = {
			GL_IMAGE_1D, GL_IMAGE_2D, GL_IMAGE_3D, GL_IMAGE_2D_RECT, GL_IMAGE_CUBE, GL_IMAGE_BUFFER,
			GL_IMAGE_1D_ARRAY, GL_IMAGE_2D_ARRAY, GL_IMAGE_CUBE_MAP_ARRAY, GL_IMAGE_2D_MULTISAMPLE,
			GL_IMAGE_2D_MULTISAMPLE_ARRAY, GL_INT_IMAGE_1D, GL_INT_IMAGE_2D, GL_INT_IMAGE_3D, GL_INT_IMAGE_2D_RECT,
			GL_INT_IMAGE_CUBE, GL_INT_IMAGE_BUFFER, GL_INT_IMAGE_1D_ARRAY, GL_INT_IMAGE_2D_ARRAY,
			GL_INT_IMAGE_CUBE_MAP_ARRAY, GL_INT_IMAGE_2D_MULTISAMPLE, GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY,
			GL_UNSIGNED_INT_IMAGE_1D, GL_UNSIGNED_INT_IMAGE_2D, GL_UNSIGNED_INT_IMAGE_3D, GL_UNSIGNED_INT_IMAGE_2D_RECT,
			GL_UNSIGNED_INT_IMAGE_CUBE, GL_UNSIGNED_INT_IMAGE_BUFFER, GL_UNSIGNED_INT_IMAGE_1D_ARRAY,
			GL_UNSIGNED_INT_IMAGE_2D_ARRAY, GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY,
			GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE, GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY
		};
		
		Set<GLenum> image_types;
		image_types.AddArray( imageUniformTypes );

		return image_types;
	}

	static Set<gl::GLenum> const& GetImageUniformTypes ()
	{
		static const Set<gl::GLenum>	image_types = RVREF( __GetImageUniformTypes() );
		return image_types;
	}

/*
=================================================
	_InitArgs
=================================================
*/
	void GL4ComputeFunction::_InitArgs ()
	{
		using namespace gl;

		GLint	count			= 0,
				max_name_length = 0,
				length			= 0,
				index			= 0;

		String	name;
		GLuint	id				= _progID.Id();
		
		// init shader storage blocks
		{
			GL_CALL( glGetProgramInterfaceiv( id, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &count ) );
			GL_CALL( glGetProgramInterfaceiv( id, GL_SHADER_STORAGE_BLOCK, GL_MAX_NAME_LENGTH, &max_name_length ) );

			name.Resize( max_name_length );

			for (int i = 0; i < count; ++i)
			{
				GL_CALL( glGetProgramResourceName( id, GL_SHADER_STORAGE_BLOCK, i, max_name_length, &length, name.ptr() ) );
				name.SetLength( length );

				GL_CALL( index = glGetProgramResourceIndex( id, GL_SHADER_STORAGE_BLOCK, name.cstr() ) );

				GL_CALL( glShaderStorageBlockBinding( id, index, i ) );

				StringCRef	name_ref		= name;
				bool		write_access	= true;		// by default it is true

				if ( name_ref.StartsWithIC( "SSBO_" ) )
					name_ref = name_ref.SubString( 5 );

				if ( name_ref.StartsWith( "in" ) )
					write_access = false;
				else
				if ( name_ref.StartsWith( "out" ) )
					write_access = true;

				_args.Add( StString_t( name_ref ), Arg( i, Arg::StorageBuffer, write_access ) );
			}
		}

		// init images and other uniforms
		{
			static const GLenum	properties[] = {
				GL_TYPE,
				GL_LOCATION,
				//GL_ARRAY_SIZE,
				//GL_OFFSET,
				//GL_BLOCK_INDEX
			};

			Set<GLenum> const&	texture_types	= GetTextureUniformTypes();
			Set<GLenum> const&	image_types		= GetImageUniformTypes();

			count = max_name_length = 0;

			GL_CALL( glGetProgramInterfaceiv( id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &count ) );
			GL_CALL( glGetProgramInterfaceiv( id, GL_UNIFORM, GL_MAX_NAME_LENGTH, &max_name_length ) );

			name.Resize( max_name_length );

			GLint	prop_results[ CountOf(properties) ] = {};
			uint	texture_unit	= 0;
			uint	image_unit		= 0;

			for (int i = 0; i < count; ++i)
			{
				length = 0;

				GL_CALL( glGetProgramResourceiv( id, GL_UNIFORM, GLuint(i), GLsizei(CountOf( properties )), properties,
												 GLsizei(CountOf( prop_results )), &length, prop_results ) );

				GL_CALL( glGetProgramResourceName( id, GL_UNIFORM, i, max_name_length, &length, name.ptr() ) );
				name.SetLength( length );

				const GLenum	type		= prop_results[0];
				const GLuint	location	= prop_results[1];

				CHECK( location != -1 );

				if ( texture_types.IsExist( type ) )
				{
					GL_CALL( glProgramUniform1i( id, location, texture_unit ) );

					_args.Add( StString_t(name), Arg( texture_unit, Arg::Texture ) );

					++texture_unit;
				}
				else
				if ( image_types.IsExist( type ) )
				{
					bool	write_access = true;		// by default it is true

					if ( name.StartsWith( "in" ) )
						write_access = false;
					else
					if ( name.StartsWith( "out" ) )
						write_access = true;

					GL_CALL( glProgramUniform1i( id, location, image_unit ) );

					_args.Add( StString_t(name), Arg( image_unit, Arg::Image, write_access ) );

					++image_unit;
				}
				else
				{
					_args.Add( StString_t(name), Arg( location, Arg::Uniform ) );
				}
			}
		}
	}
	
/*
=================================================
	GL4ComputeFunction_SetArg
=================================================
*/
	struct GL4ComputeFunction::_SetArgFunc
	{
		const Arg &			_arg;
		const ProgramID		_prog;

		_SetArgFunc (ProgramID prog, const Arg &arg) : _prog( prog ), _arg( arg )
		{}

		template <typename T>
		void operator () (const T& value) const;

		void operator () (const ComputeBufferPtr &value) const
		{
			ASSERT( _arg.type == Arg::StorageBuffer );

			value->Bind( _arg.index, _arg.writeAccess ? EMemoryAccess::ReadWrite : EMemoryAccess::Read );
		}

		void operator () (const ComputeImagePtr &value) const
		{
			ASSERT( _arg.type == Arg::Image );

			value->Bind( _arg.index, _arg.writeAccess ? EMemoryAccess::ReadWrite : EMemoryAccess::Read );
		}

		void operator () (const MemoryBufferPtr &value) const
		{
			ASSERT( _arg.type == Arg::StorageBuffer );

			value->BindBase( EBufferTarget::ShaderStorage, _arg.index, _arg.writeAccess ? EMemoryAccess::ReadWrite : EMemoryAccess::Read );
		}

		void operator () (const VertexBufferPtr &value) const
		{
			operator() ((MemoryBufferPtr) value);
		}

		void operator () (const IndexBufferPtr &value) const
		{
			operator() ((MemoryBufferPtr) value);
		}

		void operator () (const TexturePtr &value) const
		{
			ASSERT( _arg.type == Arg::Texture );

			value->Bind( _arg.index );
		}
	};
	
#	define SET_UNIFORM_VALUE_VEC( _type_, _base_, _postfix_ ) \
		template <> \
		inline void GL4ComputeFunction::_SetArgFunc::operator () (const _type_ &value) const \
		{ \
			using namespace gl; \
			GL_CALL( glProgramUniform##_postfix_( _prog.Id(), _arg.index, 1, (const _base_ *) &value ) ); \
		}

#	define SET_UNIFORM_VALUE_VECN( _type_, _postfix_ ) \
		SET_UNIFORM_VALUE_VEC( _type_,		_type_,	1##_postfix_ ) \
		SET_UNIFORM_VALUE_VEC( _type_##2,	_type_,	2##_postfix_ ) \
		SET_UNIFORM_VALUE_VEC( _type_##3,	_type_,	3##_postfix_ ) \
		SET_UNIFORM_VALUE_VEC( _type_##4,	_type_,	4##_postfix_ )
	
#	define SET_UNIFORM_VALUE_MAT( _type_, _base_, _postfix_ ) \
		template <> \
		inline void GL4ComputeFunction::_SetArgFunc::operator () (const _type_ &value) const \
		{ \
			using namespace gl; \
			GL_CALL( glProgramUniformMatrix##_postfix_( _prog.Id(), _arg.index, 1, GL_FALSE, (const _base_ *) &value ) ); \
		}

#	define SET_UNIFORM_VALUE_MATN( _type_, _postfix_ ) \
		SET_UNIFORM_VALUE_MAT( _type_##2x2,	_type_,		2##_postfix_ ) \
		SET_UNIFORM_VALUE_MAT( _type_##2x3,	_type_,		2x3##_postfix_ ) \
		SET_UNIFORM_VALUE_MAT( _type_##2x4,	_type_,		2x4##_postfix_ ) \
		SET_UNIFORM_VALUE_MAT( _type_##3x2,	_type_,		3x2##_postfix_ ) \
		SET_UNIFORM_VALUE_MAT( _type_##3x3,	_type_,		3##_postfix_ ) \
		SET_UNIFORM_VALUE_MAT( _type_##3x4,	_type_,		3x4##_postfix_ ) \
		SET_UNIFORM_VALUE_MAT( _type_##4x2,	_type_,		4x2##_postfix_ ) \
		SET_UNIFORM_VALUE_MAT( _type_##4x3,	_type_,		4x3##_postfix_ ) \
		SET_UNIFORM_VALUE_MAT( _type_##4x4,	_type_,		4##_postfix_ )

	SET_UNIFORM_VALUE_VECN( int,		iv  )
	SET_UNIFORM_VALUE_VECN( uint,		uiv )
	SET_UNIFORM_VALUE_VECN( float,		fv  )
	SET_UNIFORM_VALUE_VECN( double,		dv  )
	SET_UNIFORM_VALUE_VECN( ilong,		i64vARB )
	SET_UNIFORM_VALUE_VECN( ulong,		ui64vARB )

	SET_UNIFORM_VALUE_MATN( float,		fv  )
	SET_UNIFORM_VALUE_MATN( double,		dv  )

/*
=================================================
	_SetArgs
=================================================
*/
	void GL4ComputeFunction::_SetArgs ()
	{
		FOR( i, _args )
		{
			Arg const&	arg = _args[i].second;

			GL4ComputeFunction::_SetArgFunc		func( _progID, arg );

			arg.value.Apply( func );
		}
	}
	
/*
=================================================
	_CheckArgs
=================================================
*/
	void GL4ComputeFunction::_CheckArgs ()
	{
		FOR( i, _args )
		{
			if ( not _args[i].second.value )
			{
				LOG( (String("Uninitialized function argument: ") + StringCRef( _args[i].first )).cstr(), ELog::Warning );
			}
		}
	}
	
/*
=================================================
	ResetArgs
=================================================
*/
	void GL4ComputeFunction::ResetArgs ()
	{
		FOR( i, _args )
		{
			_args[i].second.value.Destroy();
		}
	}
		

}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
