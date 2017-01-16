// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "Engine/Graphics/Shader/ShaderProgram.h"
#include "Engine/Graphics/Engine/GraphicsEngine.h"
#include "Engine/Graphics/Shader/TextureUniform.h"
#include "Engine/Graphics/Shader/ImageUniform.h"
#include "Engine/Graphics/Shader/UniformBlock.h"
#include "Engine/Graphics/Texture/Sampler.h"

#if defined( GRAPHICS_API_OPENGL )

namespace Engine
{
namespace Graphics
{
	
/*
=================================================
	FormatUniformBlockSource
=================================================
*/
	String ShaderProgram::FormatUniformBlockSource (StringCRef source)
	{
		Array< StringCRef >	parts;
		String					str;

		StringParser::DivideString_CPP( source, parts );

		int	tab_offset = 0;

		FOR( i, parts )
		{
			if ( parts[i] == "struct" or parts[i] == "class" )
			{
				str << "uniform";
				continue;
			}

			bool is_end_of_line = ( parts[i] == ";" );

			if ( not is_end_of_line )
				str << " ";

			str << parts[i];

			if ( parts[i] == "{" )
			{
				++tab_offset;
				is_end_of_line = true;
			}
			else
			if ( parts[i] == "}" )
			{
				--tab_offset;
				is_end_of_line = true;
			}

			if ( is_end_of_line )
			{
				for (int j = 0; j < tab_offset; ++j)
				{
					str << "\t";
				}
			}
		}

		return str;
	}
	
/*
=================================================
	_CreateUniformBuffer
=================================================
*/
	void ShaderProgram::_CreateUniformBuffer (OUT uint &location, OUT uint &size, StringCRef name)
	{
		using namespace gl;

		GL_CALL( location = glGetUniformBlockIndex( _program.Id(), name.cstr() ) );
		CHECK( location != -1 );
		
		GLint	ub_size = 0;
		GL_CALL( glGetActiveUniformBlockiv( _program.Id(), location, GL_UNIFORM_BLOCK_DATA_SIZE, &ub_size ) );

		size = ub_size;
	}

/*
=================================================
	Update
=================================================
*/
	void TextureUniform::Update (const TexturePtr &tex, uint unit)
	{
		using namespace gl;

		CHECK( _isInteger == EPixelFormat::ValueType::IsInteger( tex->PixelFormat() ) );

		tex->Bind( unit );

		if ( _texUnit != unit )
		{
			_texUnit = unit;
			GL_CALL( glProgramUniform1i( _location.Program(), _location.Location(), _texUnit ) );
		}
	}

/*
=================================================
	Update
=================================================
*/
	void ImageUniform::Update (const TexturePtr &tex, uint unit, MipmapLevel level, TexArrLayer layer)
	{
		using namespace gl;

		tex->BindImage( unit, _access, level, layer );

		if ( _imgUnit != unit )
		{
			_imgUnit = unit;
			GL_CALL( glProgramUniform1i( _location.Program(), _location.Location(), _imgUnit ) );
		}
	}


}	// Graphics
}	// Engine

#endif	// GRAPHICS_API_OPENGL