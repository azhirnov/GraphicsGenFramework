// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

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
		_flags( EMemoryAccess::type(0) )
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
		_flags	= EMemoryAccess::type(0);
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
								  EMemoryAccess::type flags, BinaryBuffer data,
								  Bytes<usize> xAlign, Bytes<usize> xyAlign)
	{
		//_Destroy();

		CHECK_ERR( _IsSupportedForImage( format ) );

		if ( _shared.IsNull() or
			 _shared->TextureType() != imageType )
		{
			_shared = Texture::New( SubSystems(), PackFileID(), imageType );
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

		CHECK_ERR( texture.IsNotNull() );
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
	bool GL4ComputeImage::SetImage (BinaryBuffer data, const uint3 &size, const uint4 &offset,
									Bytes<usize> xAlign, Bytes<usize> xyAlign)
	{
		CHECK_ERR( _shared.IsNotNull() );

		CHECK_ERR( _shared->AddImage( offset, _level, size, PixelFormat(), data, xAlign, xyAlign ) );
		return true;
	}
	
/*
=================================================
	GetImage
=================================================
*/
	bool GL4ComputeImage::GetImage (OUT Buffer<ubyte> data, const uint3 &size, const uint4 &offset,
									Bytes<usize> xAlign, Bytes<usize> xyAlign)
	{
		CHECK_ERR( _shared.IsNotNull() );
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
	bool GL4ComputeImage::Copy (const ComputeBufferPtr &src, Bytes<usize> srcOffset, const uint4 &dstOffset, const uint4 &size)
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
	bool GL4ComputeImage::CopyTo (const ComputeBufferPtr &dst, const uint4 &srcOffset, Bytes<usize> dstOffset, const uint4 &size)
	{
		TODO( "" );
		return false;
	}
	
/*
=================================================
	New
=================================================
*/
	ComputeImagePtr GL4ComputeImage::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new GL4ComputeImage( ss ) );
	}
	
/*
=================================================
	New
=================================================
*/
	ComputeImagePtr GL4ComputeImage::New (const TexturePtr &texture, EMemoryAccess::type flags, MipmapLevel level)
	{
		CHECK_ERR( texture.IsNotNull(), null );

		ComputeImagePtr img = ComputeImage::New( texture->SubSystems() );

		CHECK_ERR( img->Create( texture, flags, level ), null );

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
