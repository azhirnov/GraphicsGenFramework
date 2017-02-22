// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "GL4ComputeImage.h"

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
	GL4ComputeImage::GL4ComputeImage (const SubSystemsRef ss) :
		BaseObject( ss ),
		_flags( EMemoryAccess::Unknown )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	GL4ComputeImage::~GL4ComputeImage ()
	{
		_Destroy();
	}
	
/*
=================================================
	_Destroy
=================================================
*/
	void GL4ComputeImage::_Destroy ()
	{
		_shared	= null;
		_flags	= EMemoryAccess::Unknown;
		_level	= MipmapLevel();
		_layer	= TexArrLayer();
	}
	
/*
=================================================
	Bind
=================================================
*/
	void GL4ComputeImage::Bind (uint unit, EMemoryAccess::type access) const
	{
		CHECK( IsCreated() );
		CHECK( EnumEqMask( access, _flags & access, EMemoryAccess::Write ) );

		_shared->BindImage( unit, access, _level, _layer );
	}

/*
=================================================
	Create
=================================================
*/
	bool GL4ComputeImage::Create (const uint4 &dim, ETexture::type imageType, EPixelFormat::type format,
								  EMemoryAccess::type flags, BinaryCBuffer data,
								  BytesU xAlign, BytesU xyAlign)
	{
		//_Destroy();

		CHECK_ERR( _IsSupportedForImage( format ) );

		if ( not _shared or
			 _shared->TextureType() != imageType )
		{
			_shared = GXTypes::New<Texture>( SubSystems(), imageType );
		}

		CHECK_ERR( _shared->Create( dim, format, MipmapLevelsRange(), MultiSamples() ) );

		if ( not data.Empty() ) {
			CHECK_ERR( _shared->AddImage( uint4(), MipmapLevel(0), Texture::Utils::ConvertSize( imageType, dim ),
											format, data, xAlign, xyAlign ) );
		}

		_flags	= flags;
		_level	= MipmapLevel(0);
		_layer	= TexArrLayer();
		return true;
	}
	
/*
=================================================
	Create
=================================================
*/
	bool GL4ComputeImage::Create (const TexturePtr &texture, EMemoryAccess::type flags, MipmapLevel level, TexArrLayer layer)
	{
		_Destroy();

		CHECK_ERR( texture );
		CHECK_ERR( texture->HasLevel( level ) );
		CHECK_ERR( _IsSupportedForImage( texture->PixelFormat() ) );

		_shared = texture;
		_flags	= flags;
		_level	= level;
		_layer	= layer;

		return true;
	}
	
/*
=================================================
	SetImage
=================================================
*/
	bool GL4ComputeImage::SetImage (BinaryCBuffer data, const uint3 &size, const uint4 &offset,
									BytesU xAlign, BytesU xyAlign)
	{
		CHECK_ERR( _shared );

		CHECK_ERR( _shared->AddImage( offset, _level, size, PixelFormat(), data, xAlign, xyAlign ) );
		return true;
	}
	
/*
=================================================
	GetImage
=================================================
*/
	bool GL4ComputeImage::GetImage (OUT BinaryBuffer data, const uint3 &size, const uint4 &offset, BytesU xAlign, BytesU xyAlign)
	{
		CHECK_ERR( _shared );
		CHECK_ERR( All( size == Texture::Utils::ConvertSize( ImageType(), Dimension() ) ) );
		CHECK_ERR( IsZero( offset ) );

		CHECK_ERR( _shared->GetImage( data, _level, PixelFormat(), xAlign, xyAlign ) );
		return true;
	}
	
/*
=================================================
	Copy
=================================================
*/
	bool GL4ComputeImage::Copy (const ComputeBufferPtr &src, BytesU srcOffset, const uint4 &dstOffset, const uint4 &size)
	{
		TODO( "" );
		return false;
	}
	
/*
=================================================
	Copy
=================================================
*/
	bool GL4ComputeImage::Copy (const ComputeImagePtr &src, const uint4 &srcOffset, const uint4 &dstOffset, const uint4 &size)
	{
		TODO( "" );
		return false;
	}
	
/*
=================================================
	CopyTo
=================================================
*/
	bool GL4ComputeImage::CopyTo (const ComputeBufferPtr &dst, const uint4 &srcOffset, BytesU dstOffset, const uint4 &size)
	{
		TODO( "" );
		return false;
	}
	
/*
=================================================
	New
=================================================
*/
	ComputeImagePtr GL4ComputeImage::New (const TexturePtr &texture, EMemoryAccess::type flags, MipmapLevel level)
	{
		CHECK_ERR( texture );

		ComputeImagePtr img = GXTypes::New<GL4ComputeImage>( texture->SubSystems() );

		CHECK_ERR( img->Create( texture, flags, level ) );

		return img;
	}
	
/*
=================================================
	_IsSupportedForImage
=================================================
*/
	bool GL4ComputeImage::_IsSupportedForImage (EPixelFormat::type fmt)
	{
		// TODO: see glGetTexParameter with GL_IMAGE_FORMAT_COMPATIBILITY_TYPE
		return EPixelFormat::IsSupportedForImage( fmt );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
