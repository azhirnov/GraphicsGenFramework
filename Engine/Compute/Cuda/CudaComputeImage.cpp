// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "CudaComputeImage.h"

#if defined( COMPUTE_API_CUDA )

namespace Engine
{
namespace Compute
{
	
/*
=================================================
	constructor
=================================================
*/
	CudaComputeImage::CudaComputeImage (const SubSystemsRef ss) :
		BaseObject( ss ), _level(0)
	{}
	
/*
=================================================
	constructor
=================================================
*/
	CudaComputeImage::CudaComputeImage (const ComputeTexturePtr &tex, uint level) :
		BaseObject( tex->SubSystems() ),
		_texture(tex), _level(level), _dim( tex->LevelDimension( level ) )
	{
		CHECK( _texture->HasLevel( _level ) );
	}

/*
=================================================
	Create
=================================================
*/
	bool CudaComputeImage::Create (const uint4 &dim, ETexture::type imageType, EPixelFormat::type format)
	{
		_Destroy();

		_dim	 = dim;
		_texture = CudaComputeTexture::New( SubSystems() );

		CHECK_ERR( _texture->Create( dim, imageType, format, 1 ) );
		return true;
	}
		
/*
=================================================
	_Destroy
=================================================
*/
	void CudaComputeImage::_Destroy ()
	{
		_texture	= null;
		_dim		= uint4();
		_level		= 0;
	}
	
/*
=================================================
	SetImage
=================================================
*/
	bool CudaComputeImage::SetImage (const uint3 &size, BinaryBuffer data, const uint4 &offset)
	{
		CHECK_ERR( _texture->SetImage( size, PixelFormat(), data, offset, 0 ) );
		return true;
	}

/*
=================================================
	GetImage
=================================================
*/
	bool CudaComputeImage::GetImage (OUT Buffer<ubyte> &data, const uint3 &size, const uint4 &offset)
	{
		CHECK_ERR( _texture->GetImage( data, size, offset, 0 ) );
		return true;
	}
	
/*
=================================================
	Copy
=================================================
*/
	bool CudaComputeImage::Copy (const ComputeBufferPtr &fromBuffer)
	{
		TODO( "" );
		return true;
	}
	
/*
=================================================
	Copy
=================================================
*/
	bool CudaComputeImage::Copy (const CudaComputeImage &fromImage)
	{
		TODO( "" );
		return true;
	}

/*
=================================================
	Id
=================================================
*/
	cuda::CUarray  CudaComputeImage::Id () const
	{
		using namespace cuda;
		
		CUarray		level_array = null;
		CU_CALL( cuMipmappedArrayGetLevel( &level_array, _texture->Id(), _level ) );

		return level_array;
	}
	
/*
=================================================
	ImageSize
=================================================
*/
	Bytes<ulong>  CudaComputeImage::ImageSize () const
	{
		Bytes<ulong>	bpp = Bytes<ulong>( EPixelFormat::BitPerPixel( PixelFormat() ) );
		Bytes<ulong>	size;
		
		switch ( ImageType() )
		{
			case ETexture::TEX_1D :				size = Bytes<ulong>( _dim.x );						break;
			case ETexture::TEX_2D :				size = Bytes<ulong>( _dim.x * _dim.y );				break;
			case ETexture::TEX_CUBE :			size = Bytes<ulong>( _dim.x * _dim.y * 6 );			break;
			case ETexture::TEX_2D_ARRAY :		size = Bytes<ulong>( _dim.x * _dim.y * _dim.w );		break;
			case ETexture::TEX_3D :				size = Bytes<ulong>( _dim.x * _dim.y * _dim.z );		break;
			case ETexture::TEX_CUBE_ARRAY :		size = Bytes<ulong>( _dim.x * _dim.y * _dim.w * 6 );	break;
			default :							RETURN_ERR( "unsupported image type!", Bytes<ulong>() );
		}

		return bpp * size;
	}

/*
=================================================
	Bind
=================================================
*/
	void CudaComputeImage::Bind (cuda::CUtexref texref)
	{
		_texture->BindImage( texref, _level );
	}
	
/*
=================================================
	New
=================================================
*/
	ComputeImagePtr  CudaComputeImage::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new CudaComputeImage( ss ) );
	}
	
/*
=================================================
	New
=================================================
*/
	ComputeImagePtr  CudaComputeImage::New (const ComputeTexturePtr &tex, uint level)
	{
		return BaseObject::_New( new CudaComputeImage( tex, level ) );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_CUDA
