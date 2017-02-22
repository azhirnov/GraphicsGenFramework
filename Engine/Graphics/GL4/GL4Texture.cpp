// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "gl4.h"

#if defined( GRAPHICS_API_OPENGL )

#include "Engine/Graphics/Texture/Texture.h"
#include "Engine/Graphics/Engine/GraphicsEngine.h"

namespace Engine
{
namespace Graphics
{

/*
=================================================
	_Create
=================================================
*/
	bool Texture::_Create (const uint4 &dimension, EPixelFormat::type format, uint2 levels, MultiSamples samples)
	{
		using namespace gl;

		CHECK_ERR( not EPixelFormat::IsCompressed( format ) );

		CHECK_ERR( SubSystems()->Get< GraphicsEngine >()->GetContext()->
				  CreateTexture( TextureType(), OUT _tex ) );

		SubSystems()->Get< GraphicsEngine >()->GetStateManager()->ActiveTexture( _tex );

		_dim			= Utils::ValidateDimension( TextureType(), dimension );
		_format			= format;
		_maxMipmaps		= Utils::NumberOfMipmaps( TextureType(), _dim );
		levels			= Min( levels, uint2( _maxMipmaps ) );
		_multiSamples	= samples;

		switch ( TextureType() )
		{
			case ETexture::Tex2D :
			{
				GL4InternalPixelFormat	ifmt;
				GL4PixelFormat			fmt;
				GL4PixelType			type;

				CHECK_ERR( GL4Enum( format, OUT ifmt, OUT fmt, OUT type ) );
				CHECK_ERR( samples <= MultiSamples(1) );
				CHECK_ERR( IsNotZero( _dim.xy() ) );
				CHECK_ERR( IsZero( _dim.zw() ) );

				for (uint level = levels.x; level <= levels.y; ++level)
				{
					const uint2	size = Max( _dim.xy() >> level, uint2(1) );
					GL_CHECK( glTexImage2D( _tex.Target(), level, ifmt, size.x, size.y, 0, fmt, type, null ) );
					_levels.Set( level );
				}
				break;
			}

			case ETexture::Tex2DMS :
			{
				GL4InternalPixelFormat	ifmt;

				CHECK_ERR( GL4Enum( format, OUT ifmt ) );
				CHECK_ERR( IsZero( levels ) );
				CHECK_ERR( IsZero( _dim.zw() ) );
				CHECK_ERR( IsNotZero( _dim.xy() ) );
				
				GL_CHECK( glTexImage2DMultisample( _tex.Target(), samples.Get(), ifmt, _dim.x, _dim.y, true ) );
				_levels.Set( 0 );
				break;
			}
			
			case ETexture::Tex2DArray :
			{
				GL4InternalPixelFormat	ifmt;
				GL4PixelFormat			fmt;
				GL4PixelType			type;
				
				CHECK_ERR( GL4Enum( format, OUT ifmt, OUT fmt, OUT type ) );
				CHECK_ERR( samples <= MultiSamples(1) );
				CHECK_ERR( IsNotZero( _dim.xyw() ) );
				CHECK_ERR( IsZero( _dim.z ) );

				for (uint level = levels.x; level <= levels.y; ++level)
				{
					const uint2	size = Max( _dim.xy() >> level, uint2(1) );
					GL_CHECK( glTexImage3D( _tex.Target(), level, ifmt, size.x, size.y, _dim.w, 0, fmt, type, null ) );
					_levels.Set( level );
				}
				break;
			}

			case ETexture::Tex2DMSArray :
			{
				GL4InternalPixelFormat	ifmt;

				CHECK_ERR( GL4Enum( format, OUT ifmt ) );
				CHECK_ERR( IsZero( levels ) );
				CHECK_ERR( IsZero( _dim.z ) );
				CHECK_ERR( IsNotZero( _dim.xyw() ) );

				GL_CHECK( glTexImage3DMultisample( _tex.Target(), samples.Get(), ifmt, _dim.x, _dim.y, _dim.w, true ) );
				_levels.Set( 0 );
				break;
			}
			
			case ETexture::Tex3D :
			{
				GL4InternalPixelFormat	ifmt;
				GL4PixelFormat			fmt;
				GL4PixelType			type;
				
				CHECK_ERR( GL4Enum( format, OUT ifmt, OUT fmt, OUT type ) );
				CHECK_ERR( samples <= MultiSamples(1) );
				CHECK_ERR( IsNotZero( _dim.xyz() ) );
				CHECK_ERR( IsZero( _dim.w ) );

				for (uint level = levels.x; level <= levels.y; ++level)
				{
					const uint3	size = Max( _dim.xyz() >> level, uint3(1) );
					GL_CHECK( glTexImage3D( _tex.Target(), level, ifmt, size.x, size.y, size.z, 0, fmt, type, null ) );
					_levels.Set( level );
				}
				break;
			}
			
			case ETexture::TexCube :
			{
				GL4InternalPixelFormat	ifmt;
				GL4PixelFormat			fmt;
				GL4PixelType			type;
				
				CHECK_ERR( GL4Enum( format, OUT ifmt, OUT fmt, OUT type ) );
				CHECK_ERR( samples <= MultiSamples(1) );
				CHECK_ERR( _dim.z == 6 and IsZero( _dim.w ) );
				CHECK_ERR( IsNotZero( _dim.xy() ) );

				for (uint level = levels.x; level <= levels.y; ++level)
				{
					const uint2	size = Max( _dim.xy() >> level, uint2(1) );

					for (uint i = 0; i < _dim.z; ++i) {
						GL_CHECK( glTexImage2D( gl::GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, ifmt,
												size.x, size.y, 0, fmt, type, null ) );
					}
					_levels.Set( level );
				}
				break;
			}
			
			case ETexture::TexCubeArray :
			{
				GL4InternalPixelFormat	ifmt;
				GL4PixelFormat			fmt;
				GL4PixelType			type;
				
				CHECK_ERR( GL4Enum( format, OUT ifmt, OUT fmt, OUT type ) );
				CHECK_ERR( samples <= MultiSamples(1) );
				CHECK_ERR( _dim.z == 6 );
				CHECK_ERR( IsNotZero( _dim.xyw() ) );

				for (uint level = levels.x; level <= levels.y; ++level)
				{
					const uint2	size = Max( _dim.xy() >> level, uint2(1) );
					GL_CHECK( glTexImage3D( _tex.Target(), level, ifmt, size.x, size.y, _dim.z * _dim.w, 0, fmt, type, null ) );
					_levels.Set( level );
				}
				break;
			}
			
			default :
			{
				_Destroy();
				RETURN_ERR( "invalid texture target" );
			}
		}

		_UpdateLevels();
		return true;
	}
	
/*
=================================================
	_AddImage
=================================================
*/
	bool Texture::_AddImage (const uint4 &offset, uint level, const uint3 &size, EPixelFormat::type format,
							 BinaryCBuffer data, BytesU xAlign, BytesU xyAlign)
	{
		using namespace gl;
		
		SubSystems()->Get< GraphicsEngine >()->GetStateManager()->ActiveTexture( _tex );

		GL_CALL( glPixelStorei( GL_UNPACK_ALIGNMENT, (GLint) isize(xAlign) ) );	// xyAlign not supported!
		
		const uint4	level_size	= Utils::LevelDimension( TextureType(), Dimension(), level );
		const uint4	img_size	= Utils::ConvertSize( TextureType(), size );
		
		CHECK_ERR( All( offset + img_size <= level_size ) );
		
		if ( not data.Empty() )
		{
			const BytesU	req_img_size = _GetImageSize( format, TextureType(), size, xAlign, xAlign );
			CHECK_ERR( req_img_size == data.Size() );
		}

		GL4InternalPixelFormat	ifmt;
		GL4PixelFormat			fmt;
		GL4PixelType			type;
		CHECK_ERR( GL4Enum( format, OUT ifmt, OUT fmt, OUT type ) );

		switch ( TextureType() )
		{
			case ETexture::Tex2D :
			{
				const bool	fullsize = All( level_size.xy() == img_size.xy() );

				CHECK_ERR( All( level_size.xy() <= (offset.xy() + img_size.xy()) ) );
				CHECK_ERR( IsNotZero( img_size.xy() ) );
				CHECK_ERR( IsZero( img_size.zw() ) );

				if ( IsZero( offset.xy() ) and fullsize )
				{
					GL_CHECK( glTexImage2D( _tex.Target(), level, ifmt, img_size.x, img_size.y, 0, fmt, type, data.ptr() ) );
					_levels.Set( level );
					break;
				}

				if ( not HasLevel( MipmapLevel( level ) ) )
				{
					GL_CHECK( glTexImage2D( _tex.Target(), level, ifmt, level_size.x, level_size.y, 0, fmt, type, null ) );
					_levels.Set( level );
				}
				
				GL_CHECK( glTexSubImage2D( _tex.Target(), level, offset.x, offset.y, img_size.x, img_size.y, fmt, type, data.ptr() ) );
				break;
			}

			case ETexture::Tex2DArray :
			{
				CHECK_ERR( All( level_size.xy() <= (offset.xy() + img_size.xy()) ) );
				CHECK_ERR( IsNotZero( img_size.xy() ) );
				CHECK_ERR( IsZero( img_size.zw() ) );
				
				if ( not HasLevel( MipmapLevel( level ) ) )
				{
					GL_CHECK( glTexImage3D( _tex.Target(), level, ifmt, level_size.x, level_size.y, _dim.w, 0, fmt, type, null ) );
					_levels.Set( level );
				}

				GL_CHECK( glTexSubImage3D( _tex.Target(), level, offset.x, offset.y, offset.w,
											img_size.x, img_size.y, 1, fmt, type, data.ptr() ) );
				break;
			}

			case ETexture::Tex3D :
			{
				const bool	fullsize = All( level_size.xyz() == img_size.xyz() );

				CHECK_ERR( All( level_size.xyz() <= (offset.xyz() + img_size.xyz()) ) );
				CHECK_ERR( IsNotZero( img_size.xyw() ) );
				CHECK_ERR( IsZero( offset.w ) and IsZero( img_size.w ) );
				
				if ( IsZero( offset.xy() ) and fullsize )
				{
					GL_CHECK( glTexImage3D( _tex.Target(), level, ifmt, img_size.x, img_size.y, img_size.z, 0, fmt, type, data.ptr() ) );
					_levels.Set( level );
					break;
				}
				
				if ( not HasLevel( MipmapLevel( level ) ) )
				{
					GL_CHECK( glTexImage3D( _tex.Target(), level, ifmt, level_size.x, level_size.y, level_size.z, 0, fmt, type, null ) );
					_levels.Set( level );
				}

				GL_CHECK( glTexSubImage3D( _tex.Target(), level, offset.x, offset.y, offset.z,
											img_size.x, img_size.y, img_size.z, fmt, type, data.ptr() ) );
				break;
			}

			case ETexture::TexCube :
			{
				CHECK_ERR( All( level_size.xy() <= (offset.xy() + img_size.xy()) ) );
				CHECK_ERR( IsNotZero( img_size.xy() ) );
				CHECK_ERR( IsZero( img_size.zw() ) );
				
				if ( not HasLevel( MipmapLevel( level ) ) )
				{
					for (uint i = 0; i < _dim.z; ++i) {
						GL_CHECK( glTexImage2D( gl::GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, ifmt,
												level_size.x, level_size.y, 0, fmt, type, null ) );
					}
					_levels.Set( level );
				}

				GL_CHECK( glTexSubImage2D( gl::GL_TEXTURE_CUBE_MAP_POSITIVE_X + offset.z, level,
											offset.x, offset.y, img_size.x, img_size.y, fmt, type, data.ptr() ) );
				break;
			}
			
			case ETexture::TexCubeArray :
			{
				CHECK_ERR( All( level_size.xy() <= (offset.xy() + img_size.xy()) ) );
				CHECK_ERR( IsNotZero( img_size.xy() ) );
				CHECK_ERR( IsZero( img_size.zw() ) );
				
				if ( not HasLevel( MipmapLevel( level ) ) )
				{
					GL_CHECK( glTexImage3D( _tex.Target(), level, ifmt, level_size.x, level_size.y, _dim.z * _dim.w, 0, fmt, type, null ) );
					_levels.Set( level );
				}

				GL_CHECK( glTexSubImage3D( _tex.Target(), level, offset.x, offset.y, offset.w * _dim.z + offset.z,
											img_size.x, img_size.y, 1, fmt, type, data.ptr() ) );
				break;
			}

			default :
				RETURN_ERR( "invalid texture target" );
		}

		_UpdateLevels();
		return true;
	}
	
/*
=================================================
	_AddCompressedImage
=================================================
*/
	bool Texture::_AddCompressedImage (const uint4 &offset, uint level, const uint3 &size, EPixelFormat::type format,
										BinaryCBuffer data, BytesU xAlign, BytesU xyAlign)
	{
		using namespace gl;
		
		SubSystems()->Get< GraphicsEngine >()->GetStateManager()->ActiveTexture( _tex );
		
		GL_CALL( glPixelStorei( GL_UNPACK_ALIGNMENT, (GLint) isize(xAlign) ) );	// xyAlign not supported!
		
		const uint4	level_size	= Utils::LevelDimension( TextureType(), Dimension(), level );
		const uint4	img_size	= Utils::ConvertSize( TextureType(), size );
		
		CHECK_ERR( All( offset + img_size <= level_size ) );
		
		if ( not data.Empty() )
		{
			const BytesU	req_img_size = _GetImageSize( format, TextureType(), size, xAlign, xAlign );
			CHECK_ERR( req_img_size == data.Size() );
		}

		GL4InternalPixelFormat	ifmt;
		CHECK_ERR( GL4Enum( format, OUT ifmt ) );

		switch ( TextureType() )
		{
			case ETexture::Tex2D :
			{
				const bool	fullsize = All( level_size.xy() == img_size.xy() );

				CHECK_ERR( All( level_size.xy() <= (offset.xy() + img_size.xy()) ) );
				CHECK_ERR( IsNotZero( img_size.xy() ) );
				CHECK_ERR( IsZero( img_size.zw() ) );

				if ( IsZero( offset.xy() ) and fullsize )
				{
					GL_CHECK( glCompressedTexImage2D( _tex.Target(), level, ifmt, img_size.x, img_size.y,
													  0, (GLsizei) isize(data.Size()), data.ptr() ) );
					_levels.Set( level );
					break;
				}

				// TODO: check - is it normal to put null
				if ( not HasLevel( MipmapLevel( level ) ) )
				{
					GL_CHECK( glCompressedTexImage2D( _tex.Target(), level, ifmt, level_size.x, level_size.y, 0, 0, null ) );
					_levels.Set( level );
				}
				
				GL_CHECK( glCompressedTexSubImage2D( _tex.Target(), level, offset.x, offset.y, img_size.x, img_size.y,
													 ifmt, (GLsizei) isize(data.Size()), data.ptr() ) );
				break;
			}

			case ETexture::Tex2DArray :
			{
				CHECK_ERR( All( level_size.xy() <= (offset.xy() + img_size.xy()) ) );
				CHECK_ERR( IsNotZero( img_size.xy() ) );
				CHECK_ERR( IsZero( img_size.zw() ) );

				// TODO: check - is it normal to put null
				if ( not HasLevel( MipmapLevel( level ) ) )
				{
					GL_CHECK( glCompressedTexImage3D( _tex.Target(), level, ifmt, level_size.x, level_size.y, _dim.w, 0, 0, null ) );
					_levels.Set( level );
				}

				GL_CHECK( glCompressedTexSubImage3D( _tex.Target(), level, offset.x, offset.y, offset.w, img_size.x, img_size.y, 1,
													 ifmt, (GLsizei) isize(data.Size()), data.ptr() ) );
				break;
			}

			case ETexture::Tex3D :
			{
				const bool	fullsize = All( level_size.xyz() == img_size.xyz() );
				
				CHECK_ERR( All( level_size.xyz() <= (offset.xyz() + img_size.xyz()) ) );
				CHECK_ERR( IsNotZero( img_size.xyw() ) );
				CHECK_ERR( IsZero( offset.w ) and IsZero( img_size.w ) );
				
				if ( IsZero( offset.xy() ) and fullsize )
				{
					GL_CHECK( glCompressedTexImage3D( _tex.Target(), level, ifmt, img_size.x, img_size.y, img_size.z,
													  0, (GLsizei) isize(data.Size()), data.ptr() ) );
					_levels.Set( level );
					break;
				}
				
				// TODO: check - is it normal to put null
				if ( not HasLevel( MipmapLevel( level ) ) )
				{
					GL_CHECK( glCompressedTexImage3D( _tex.Target(), level, ifmt, level_size.x, level_size.y,
													  level_size.z, 0, 0, null ) );
					_levels.Set( level );
				}

				GL_CHECK( glCompressedTexSubImage3D( _tex.Target(), level, offset.x, offset.y, offset.z, img_size.x, img_size.y,
													 img_size.z, ifmt, (GLsizei) isize(data.Size()), data.ptr() ) );
				break;
			}

			case ETexture::TexCube :
			{
				CHECK_ERR( All( level_size.xy() <= (offset.xy() + img_size.xy()) ) );
				CHECK_ERR( IsNotZero( img_size.xy() ) );
				CHECK_ERR( IsZero( img_size.zw() ) );
				
				if ( not HasLevel( MipmapLevel( level ) ) )
				{
					for (uint i = 0; i < _dim.z; ++i) {
						GL_CHECK( glCompressedTexImage2D( gl::GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level,
														  ifmt, level_size.x, level_size.y, 0, 0, null ) );
					}
					_levels.Set( level );
				}

				GL_CHECK( glCompressedTexSubImage2D( gl::GL_TEXTURE_CUBE_MAP_POSITIVE_X + offset.z, level,
													 offset.x, offset.y, img_size.x, img_size.y, ifmt,
													 (GLsizei) isize(data.Size()), data.ptr() ) );
				break;
			}
			
			case ETexture::TexCubeArray :
			{
				CHECK_ERR( All( level_size.xy() <= (offset.xy() + img_size.xy()) ) );
				CHECK_ERR( IsNotZero( img_size.xy() ) );
				CHECK_ERR( IsZero( img_size.zw() ) );
				
				if ( not HasLevel( MipmapLevel( level ) ) )
				{
					GL_CHECK( glCompressedTexImage3D( _tex.Target(), level, ifmt, level_size.x, level_size.y,
													  _dim.z * _dim.w, 0, 0, null ) );
					_levels.Set( level );
				}

				GL_CHECK( glCompressedTexSubImage3D( _tex.Target(), level, offset.x, offset.y, offset.w * _dim.z + offset.z,
													 img_size.x, img_size.y, 1, ifmt, (GLsizei) isize(data.Size()), data.ptr() ) );
				break;
			}

			default :
				RETURN_ERR( "invalid texture target" );
		}

		_UpdateLevels();
		return true;
	}

/*
=================================================
	_AllocLevel
=================================================
*/
	bool Texture::_AllocLevel (uint level)
	{
		using namespace gl;

		CHECK_ERR( not EPixelFormat::IsCompressed( _format ) );

		SubSystems()->Get< GraphicsEngine >()->GetStateManager()->ActiveTexture( _tex );
		
		const uint4	level_size	= Utils::LevelDimension( TextureType(), Dimension(), level );

		GL4InternalPixelFormat	ifmt;
		GL4PixelFormat			fmt;
		GL4PixelType			type;
		
		CHECK_ERR( GL4Enum( _format, OUT ifmt, OUT fmt, OUT type ) );

		switch ( TextureType() )
		{
			case ETexture::Tex2D :
			{
				GL_CHECK( glTexImage2D( _tex.Target(), level, ifmt, level_size.x, level_size.y, 0, fmt, type, null ) );
				_levels.Set( level );
				break;
			}

			case ETexture::Tex2DArray :
			{
				GL_CHECK( glTexImage3D( _tex.Target(), level, ifmt, level_size.x, level_size.y, _dim.w, 0, fmt, type, null ) );
				_levels.Set( level );
				break;
			}

			case ETexture::Tex3D :
			{
				GL_CHECK( glTexImage3D( _tex.Target(), level, ifmt, level_size.x, level_size.y, level_size.z, 0, fmt, type, null ) );
				_levels.Set( level );
				break;
			}

			case ETexture::TexCube :
			{
				for (uint i = 0; i < _dim.z; ++i) {
					GL_CHECK( glTexImage2D( gl::GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, ifmt,
											level_size.x, level_size.y, 0, fmt, type, null ) );
				}
				_levels.Set( level );
				break;
			}
			
			case ETexture::TexCubeArray :
			{
				GL_CHECK( glTexImage3D( _tex.Target(), level, ifmt, level_size.x, level_size.y,
										_dim.z * _dim.w, 0, fmt, type, null ) );
				_levels.Set( level );
				break;
			}

			default :
				RETURN_ERR( "invalid texture target" );
		}

		_UpdateLevels();
		return true;
	}
	
/*
=================================================
	_GetImage
=================================================
*/
	bool Texture::_GetImage (INOUT BinaryBuffer data, uint level, EPixelFormat::type format, BytesU xAlign, BytesU xyAlign) const
	{
		using namespace gl;
		
		if ( not data.Empty() )
		{
			const BytesU	req_img_size = _GetImageSize( format, TextureType(), LevelDimension( MipmapLevel( level ) ), xAlign, xAlign );
			CHECK_ERR( req_img_size == data.Size() );
		}

		GL4InternalPixelFormat	ifmt;
		GL4PixelFormat			fmt;
		GL4PixelType			type;
		
		CHECK_ERR( GL4Enum( format, OUT ifmt, OUT fmt, OUT type ) );
		
		GL_CALL( glPixelStorei( GL_PACK_ALIGNMENT, (GLint) isize(xAlign) ) );	// xyAlign not supported

		GL_CHECK( glGetImageFromTexture( _tex.Id(), _tex.Target(), level, fmt, type, (GLsizei) isize(data.Size()), data.ptr() ) );

		return true;
	}
	
/*
=================================================
	_GetSubImage
=================================================
*/
	bool Texture::_GetSubImage (INOUT BinaryBuffer data, const uint4 &offset, uint level, const uint3 &size,
								EPixelFormat::type format, BytesU xAlign, BytesU xyAlign) const
	{
		using namespace gl;
		
		const uint4	level_size	= Utils::LevelDimension( TextureType(), Dimension(), level );
		const uint4	img_size	= Utils::ConvertSize( TextureType(), size );
		
		CHECK_ERR( All( offset + img_size <= level_size ) );

		if ( not data.Empty() )
		{
			const BytesU	req_img_size = _GetImageSize( format, TextureType(), size, xAlign, xAlign );
			CHECK_ERR( req_img_size == data.Size() );
		}

		GL4InternalPixelFormat	ifmt;
		GL4PixelFormat			fmt;
		GL4PixelType			type;
		
		CHECK_ERR( GL4Enum( format, OUT ifmt, OUT fmt, OUT type ) );
		
		GL_CALL( glPixelStorei( GL_PACK_ALIGNMENT, (GLint) isize(xAlign) ) );	// xyAlign not supported
		
		switch ( TextureType() )
		{
			case ETexture::Buffer :
			case ETexture::Tex1D :
				CHECK_ERR( IsZero( offset.yzw() ) and IsZero( img_size.yzw() ) );
				CHECK_ERR( IsNotZero( img_size.x ) );

				GL_CHECK( glGetSubImageFromTexture( _tex.Id(), _tex.Target(), level,
													offset.x, 0, 0,
													img_size.x, 1, 1, fmt, type,
													(GLsizei) isize(data.Size()), data.ptr() ) );
				break;

			case ETexture::Tex2D :
			case ETexture::Tex2DMS :
				CHECK_ERR( IsZero( offset.zw() ) and IsZero( img_size.zw() ) );
				CHECK_ERR( IsNotZero( img_size.xy() ) );

				GL_CHECK( glGetSubImageFromTexture( _tex.Id(), _tex.Target(), level,
													offset.x, offset.y, 0,
													img_size.x, img_size.y, 1, fmt, type,
													(GLsizei) isize(data.Size()), data.ptr() ) );
				break;

			case ETexture::Tex3D :
				CHECK_ERR( IsZero( offset.w ) and IsZero( img_size.w ) );
				CHECK_ERR( IsNotZero( img_size.xyz() ) );

				GL_CHECK( glGetSubImageFromTexture( _tex.Id(), _tex.Target(), level,
													offset.x, offset.y, offset.z,
													img_size.x, img_size.y, img_size.z, fmt, type,
													(GLsizei) isize(data.Size()), data.ptr() ) );
				break;

			case ETexture::Tex2DArray :
			case ETexture::Tex2DMSArray :
				CHECK_ERR( IsZero( offset.z ) and IsZero( img_size.z ) );
				CHECK_ERR( IsNotZero( img_size.xyw() ) );

				GL_CHECK( glGetSubImageFromTexture( _tex.Id(), _tex.Target(), level,
													offset.x, offset.y, offset.w,
													img_size.x, img_size.y, img_size.w, fmt, type,
													(GLsizei) isize(data.Size()), data.ptr() ) );
				break;

			case ETexture::TexCube :
				CHECK_ERR( offset.z < 6 and img_size.z < 6 );
				CHECK_ERR( IsZero( offset.w ) and IsZero( img_size.w ) );
				CHECK_ERR( IsNotZero( img_size.xyz() ) );

				GL_CHECK( glGetSubImageFromTexture( _tex.Id(), _tex.Target(), level,
													offset.x, offset.y, offset.z,
													img_size.x, img_size.y, img_size.z, fmt, type,
													(GLsizei) isize(data.Size()), data.ptr() ) );
				break;

			/*case ETexture::TexCubeArray :
				CHECK_ERR( IsZero( offset.z ) and IsZero( img_size.z ) );
				CHECK_ERR( IsNotZero( img_size.xyw() ) );

				GL_CHECK( glGetSubImageFromTexture( _tex.Id(), _tex.Target(), level,
													offset.x, offset.y, offset.w,
													img_size.x, img_size.y, img_size.w, fmt, type,
													GLsizei(data.Size()), data.ptr() ) );
				break;*/

			default :
				RETURN_ERR( "invalid texture target" );
		}


		return true;
	}

/*
=================================================
	_GenerateMipmaps
=================================================
*/
	bool Texture::_GenerateMipmaps ()
	{
		using namespace gl;
		
		_levels.SetInterval( 0, _maxMipmaps );
		_UpdateLevels();

		GL_CHECK( glGenerateTextureMipmaps( _tex.Target(), _tex.Id() ) );
		return true;
	}
	
/*
=================================================
	_UpdateLevels
=================================================
*/
	void Texture::_UpdateLevels ()
	{
		using namespace gl;

		const usize	min = _levels.MinIndex();
		const usize	max = _levels.MaxIndex();

		ASSERT( _levels.HasInterval( min, max ) );

		GL_CALL( glTextureParameterInt( _tex.Id(), _tex.Target(), gl::GL_TEXTURE_BASE_LEVEL, GLint(min) ) );
		GL_CALL( glTextureParameterInt( _tex.Id(), _tex.Target(), gl::GL_TEXTURE_MAX_LEVEL,  GLint(max) ) );
	}

		
}	// Graphics
}	// Engine

#endif	// GRAPHICS_API_OPENGL
