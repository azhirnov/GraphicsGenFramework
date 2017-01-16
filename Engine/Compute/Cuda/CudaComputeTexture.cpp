// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CudaComputeTexture.h"

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
	CudaComputeTexture::CudaComputeTexture (const SubSystemsRef ss) :
		BaseObject( ss ),
		_id( null ), _format( EPixelFormat::type(-1) ), _target( ETexture::type(-1) ),
		_maxMipmaps(0), _cudaFormat( cuda::CUarray_format(0) )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	CudaComputeTexture::~CudaComputeTexture ()
	{
		_Destroy();
	}
	
/*
=================================================
	Create
=================================================
*/
	bool CudaComputeTexture::Create (const uint4 &dim, ETexture::type textureType, EPixelFormat::type format, uint levels)
	{
		using namespace cuda;

		_Destroy();

		CUDA_ARRAY3D_DESCRIPTOR		desc = {};
		
		uint	channels	= 0;
		bool	integer		= false;
		bool	normalized	= false;

		CHECK( CudaEnum( format, _cudaFormat, channels, integer, normalized ) );

		desc.Format			= _cudaFormat;
		desc.NumChannels	= channels;
		
		_dim = Max( dim, uint4(1) );

		switch ( textureType )
		{
			case ETexture::TEX_1D :
				desc.Width	= _dim.x;
				desc.Height	= 0;
				desc.Depth	= 0;
				desc.Flags	= 0;
				break;

			case ETexture::TEX_1D_ARRAY :
				desc.Width	= _dim.x;
				desc.Height	= 0;
				desc.Depth	= _dim.w;
				desc.Flags	= CUDA_ARRAY3D_LAYERED;
				break;

			case ETexture::TEX_2D :
				desc.Width	= _dim.x;
				desc.Height	= _dim.y;
				desc.Depth	= 0;
				desc.Flags	= 0;
				break;

			case ETexture::TEX_2D_ARRAY :
				desc.Width	= _dim.x;
				desc.Height	= _dim.y;
				desc.Depth	= _dim.w;
				desc.Flags	= CUDA_ARRAY3D_LAYERED;
				break;

			case ETexture::TEX_CUBE :
				CHECK_ERR( dim.z == 6 );
				desc.Width	= _dim.x;
				desc.Height	= _dim.y;
				desc.Depth	= _dim.z;
				desc.Flags	= CUDA_ARRAY3D_CUBEMAP;
				break;

			case ETexture::TEX_CUBE_ARRAY :
				CHECK_ERR( _dim.z == 6 );
				desc.Width	= _dim.x;
				desc.Height	= _dim.y;
				desc.Depth	= _dim.z * _dim.w;
				desc.Flags	= CUDA_ARRAY3D_LAYERED | CUDA_ARRAY3D_CUBEMAP;
				break;

			case ETexture::TEX_3D :
				desc.Width	= _dim.x;
				desc.Height	= _dim.y;
				desc.Depth	= _dim.z;
				desc.Flags	= 0;
				break;

			default :
				RETURN_ERR( "unsupported texture type!" );
		}
		
		uint const	max_levels	= _NumberOfMipmaps( textureType, _dim );
		_maxMipmaps				= Clamp( levels, 1u, max_levels );

		CU_CHECK( cuMipmappedArrayCreate( &_id, &desc, _maxMipmaps ) );

		_format	= format;
		_target	= textureType;
		_levels.SetInterval( 0, _maxMipmaps-1 );

		return true;
	}
	
/*
=================================================
	_NumberOfMipmaps
=================================================
*/
	uint CudaComputeTexture::_NumberOfMipmaps (ETexture::type texType, const uint4 &dim)
	{
		switch ( texType )
		{
			case ETexture::TEX_1D :
			case ETexture::TEX_2D :
			case ETexture::TEX_3D :			return GetNumberOfMipmaps( dim.Max() ) + 1;

			case ETexture::TEX_CUBE :
			case ETexture::TEX_CUBE_ARRAY :
			case ETexture::TEX_2D_ARRAY :	return GetNumberOfMipmaps( dim.xy().Max() ) + 1;
		}

		RETURN_ERR( "invalid texture type", uint(1) );
	}

/*
=================================================
	_LevelDimension
=================================================
*/
	uint4 CudaComputeTexture::_LevelDimension (ETexture::type target, const uint4 &dim, uint level)
	{
		ASSERT( level == 0 or (target == ETexture::TEX_2D_MS or target == ETexture::TEX_2D_MS_ARRAY) );

		switch ( target )
		{
			case ETexture::TEX_1D :			return uint4( Max( dim.x >> level, 1u ), 1, 1, 1 );

			case ETexture::TEX_2D :			return uint4( Max( dim.xy() >> level, uint2(1) ), 1, 1 );

			case ETexture::TEX_CUBE :		return uint4( Max( dim.xy() >> level, uint2(1) ), 6, 1 );

			case ETexture::TEX_2D_ARRAY :	return uint4( Max( dim.xy() >> level, uint2(1) ), 1, dim.w );

			case ETexture::TEX_3D :			return uint4( Max( dim.xyz() >> level, uint3(1) ), 1 );

			case ETexture::TEX_CUBE_ARRAY :	return uint4( Max( dim.xy() >> level, uint2(1) ), 6, dim.w );
		}

		RETURN_ERR( "invalid texture type", uint4(1) );
	}

/*
=================================================
	_Destroy
=================================================
*/
	void CudaComputeTexture::_Destroy ()
	{
		using namespace cuda;

		if ( _id != null )
		{
			CU_CALL( cuMipmappedArrayDestroy( _id ) );
			_id = null;
		}

		_shared		= null;
		_sampler	= SamplerState();
		_dim		= uint4();
		_format		= EPixelFormat::type(-1);
		_target		= ETexture::type(-1);
		_maxMipmaps	= 0;
		_levels		= 0;
		_cudaFormat	= cuda::CUarray_format(0);
	}

/*
=================================================
	SetImage
=================================================
*/
	bool CudaComputeTexture::SetImage (const uint3 &size, EPixelFormat::type format, BinaryBuffer data,
										const uint4 &offset, uint level)
	{
		CHECK_ERR( _format == format );

		const Bits<usize>	bpp = EPixelFormat::BitPerPixel( _format );

		switch ( _target )
		{
			case ETexture::TEX_1D :
				return _SetImage2D( offset.xo(), level, size.xo(), bpp.ToBytes(), data );

			case ETexture::TEX_2D :
				return _SetImage2D( offset.xy(), level, size.xy(), bpp.ToBytes(), data );

			case ETexture::TEX_CUBE :
				return _SetImage3D( offset.xyz(), level, size.xyo(), bpp.ToBytes(), data );

			case ETexture::TEX_2D_ARRAY :
				return _SetImage3D( offset.xyw(), level, size.xyo(), bpp.ToBytes(), data );

			case ETexture::TEX_3D :
				return _SetImage3D( offset.xyz(), level, size, bpp.ToBytes(), data );

			case ETexture::TEX_CUBE_ARRAY :
				return _SetImage3D( uint3( offset.xy(), offset.w * 6 + offset.z ), level, size.xyo(), bpp.ToBytes(), data );
		}

		RETURN_ERR( "unsupported texture type!" );
	}
	
/*
=================================================
	_SetImage2D
=================================================
*/
	bool CudaComputeTexture::_SetImage2D (const uint2 &offset, uint level, const uint2 &size,
										  Bytes<usize> bpp, BinaryBuffer data)
	{
		using namespace cuda;
		
		CUarray					level_array		= null;
		CUDA_MEMCPY2D			copy_params2d	= {};
		CUDA_ARRAY_DESCRIPTOR	desc			= {};

		CU_CHECK( cuMipmappedArrayGetLevel( &level_array, _id, level ) );
		
		CU_CALL( cuArrayGetDescriptor( &desc, level_array ) );

		CHECK_ERR( size.x + offset.x <= desc.Width and size.y + offset.y <= desc.Height );

		usize const		data_size	= size.x * size.y * bpp;

		CHECK_ERR( data.Size() == data_size );

		// destination
		copy_params2d.dstMemoryType	= CU_MEMORYTYPE_ARRAY;
		copy_params2d.dstArray		= level_array;
		copy_params2d.dstXInBytes	= offset.x * bpp;
		copy_params2d.dstY			= offset.y;

		// source
		copy_params2d.srcMemoryType	= CU_MEMORYTYPE_HOST;
		copy_params2d.srcHost		= data.ptr();
		copy_params2d.srcPitch		= Max( size.x, 1u ) * bpp;
		copy_params2d.WidthInBytes	= copy_params2d.srcPitch;
		copy_params2d.Height		= Max( size.y, 1u );

		CU_CHECK( cuMemcpy2D( &copy_params2d ) );
		return true;
	}
	
/*
=================================================
	_SetImage3D
=================================================
*/
	bool CudaComputeTexture::_SetImage3D (const uint3 &offset, uint level, const uint3 &size,
										  Bytes<usize> bpp, BinaryBuffer data)
	{
		using namespace cuda;
		
		CUarray					level_array		= null;
		CUDA_MEMCPY3D			copy_params3d	= {};
		CUDA_ARRAY3D_DESCRIPTOR	desc			= {};

		CU_CHECK( cuMipmappedArrayGetLevel( &level_array, _id, level ) );
		
		CU_CALL( cuArray3DGetDescriptor ( &desc, level_array ) );
		
		CHECK_ERR( size.x + offset.x <= desc.Width and size.y + offset.y <= desc.Height and size.z + offset.z <= desc.Depth );
		
		usize const		data_size	= size.x * size.y * bpp;

		CHECK_ERR( data.Size() == data_size );

		// destination
		copy_params3d.dstMemoryType	= CU_MEMORYTYPE_ARRAY;
		copy_params3d.dstArray		= level_array;
		copy_params3d.dstXInBytes	= offset.x * bpp;
		copy_params3d.dstY			= offset.y;
		copy_params3d.dstZ			= offset.z;

		// source
		copy_params3d.srcMemoryType	= CU_MEMORYTYPE_HOST;
		copy_params3d.srcHost		= data.ptr();
		copy_params3d.srcPitch		= Max( size.x, 1u ) * bpp;
		copy_params3d.WidthInBytes	= copy_params3d.srcPitch;
		copy_params3d.Height		= Max( size.y, 1u );
		copy_params3d.Depth			= Max( size.z, 1u );

		CU_CHECK( cuMemcpy3D( &copy_params3d ) );
		return true;
	}
	
/*
=================================================
	GetImage
=================================================
*/
	bool CudaComputeTexture::GetImage (OUT Buffer<ubyte> &data, const uint3 &size, const uint4 &offset, uint level)
	{
		const Bits<usize>	bpp = EPixelFormat::BitPerPixel( _format );

		switch ( _target )
		{
			case ETexture::TEX_1D :
				return _GetImage2D( offset.xo(), level, size.xo(), bpp.ToBytes(), data );

			case ETexture::TEX_2D :
				return _GetImage2D( offset.xy(), level, size.xy(), bpp.ToBytes(), data );

			case ETexture::TEX_CUBE :
				return _GetImage3D( offset.xyz(), level, size.xyo(), bpp.ToBytes(), data );

			case ETexture::TEX_2D_ARRAY :
				return _GetImage3D( offset.xyw(), level, size.xyo(), bpp.ToBytes(), data );

			case ETexture::TEX_3D :
				return _GetImage3D( offset.xyz(), level, size, bpp.ToBytes(), data );

			case ETexture::TEX_CUBE_ARRAY :
				return _GetImage3D( uint3( offset.xy(), offset.w * 6 + offset.z ), level, size.xyo(), bpp.ToBytes(), data );
		}

		RETURN_ERR( "unsupported texture type!" );
	}
	
/*
=================================================
	_GetImage2D
=================================================
*/
	bool CudaComputeTexture::_GetImage2D (const uint2 &offset, uint level, const uint2 &size,
										  Bytes<usize> bpp, OUT Buffer<ubyte> &data)
	{
		using namespace cuda;
		
		CUarray					level_array		= null;
		CUDA_MEMCPY2D			copy_params2d	= {};
		CUDA_ARRAY_DESCRIPTOR	desc			= {};

		CU_CHECK( cuMipmappedArrayGetLevel( &level_array, _id, level ) );
		
		CU_CALL( cuArrayGetDescriptor( &desc, level_array ) );

		CHECK_ERR( size.x + offset.x <= desc.Width and size.y + offset.y <= desc.Height );
		
		usize const		data_size	= size.x * size.y * bpp;

		CHECK_ERR( data.Size() >= data_size );

		// source
		copy_params2d.srcMemoryType	= CU_MEMORYTYPE_ARRAY;
		copy_params2d.srcArray		= level_array;
		copy_params2d.srcXInBytes	= offset.x * bpp;
		copy_params2d.srcY			= offset.y;
		
		// destination
		copy_params2d.dstMemoryType	= CU_MEMORYTYPE_HOST;
		copy_params2d.dstHost		= data.ptr();
		copy_params2d.dstPitch		= Max( size.x, 1u ) * bpp;
		copy_params2d.WidthInBytes	= copy_params2d.dstPitch;
		copy_params2d.Height		= Max( size.y, 1u );

		CU_CHECK( cuMemcpy2D( &copy_params2d ) );

		data = data.SubArray( 0, data_size );
		return true;
	}
	
/*
=================================================
	_GetImage3D
=================================================
*/
	bool CudaComputeTexture::_GetImage3D (const uint3 &offset, uint level, const uint3 &size,
										  Bytes<usize> bpp, OUT Buffer<ubyte> &data)
	{
		using namespace cuda;
		
		CUarray					level_array		= null;
		CUDA_MEMCPY3D			copy_params3d	= {};
		CUDA_ARRAY3D_DESCRIPTOR	desc			= {};

		CU_CHECK( cuMipmappedArrayGetLevel( &level_array, _id, level ) );
		
		CU_CALL( cuArray3DGetDescriptor ( &desc, level_array ) );
		
		CHECK_ERR( size.x + offset.x <= desc.Width and size.y + offset.y <= desc.Height and size.z + offset.z <= desc.Depth );
		
		usize const		data_size	= size.x * size.y * bpp;

		CHECK_ERR( data.Size() >= data_size );

		// source
		copy_params3d.srcMemoryType	= CU_MEMORYTYPE_ARRAY;
		copy_params3d.srcArray		= level_array;
		copy_params3d.srcXInBytes	= offset.x * bpp;
		copy_params3d.srcY			= offset.y;
		copy_params3d.srcZ			= offset.z;

		// destination
		copy_params3d.dstMemoryType	= CU_MEMORYTYPE_HOST;
		copy_params3d.dstHost		= data.ptr();
		copy_params3d.dstPitch		= Max( size.x, 1u ) * bpp;
		copy_params3d.WidthInBytes	= copy_params3d.dstPitch;
		copy_params3d.Height		= Max( size.y, 1u );
		copy_params3d.Depth			= Max( size.z, 1u );

		CU_CHECK( cuMemcpy3D( &copy_params3d ) );
		
		data = data.SubArray( 0, data_size );
		return true;
	}

/*
=================================================
	SetSampler
=================================================
*/
	void CudaComputeTexture::SetSampler (const SamplerState &sampler)
	{
		_sampler = sampler;
	}
	
/*
=================================================
	Bind
=================================================
*/
	void CudaComputeTexture::Bind (cuda::CUtexref texref)
	{
		using namespace cuda;

		CU_CALL( cuTexRefSetMipmappedArray( texref, _id, CU_TRSA_OVERRIDE_FORMAT ) );

		_BindSampler( texref, true );
	}
	
/*
=================================================
	BindImage
=================================================
*/
	void CudaComputeTexture::BindImage (cuda::CUtexref texref, uint level)
	{
		using namespace cuda;

		CUarray		level_array = null;

		CU_CALL( cuMipmappedArrayGetLevel( &level_array, _id, level ) );
		CU_CALL( cuTexRefSetArray( texref, level_array, CU_TRSA_OVERRIDE_FORMAT ) );

		_BindSampler( texref, false );
	}
	
/*
=================================================
	_BindSampler
=================================================
*/
	void CudaComputeTexture::_BindSampler (cuda::CUtexref texref, bool normalizedCoords)
	{
		using namespace cuda;
		
		typedef Vec< CUaddress_mode, 3 >	AddressMode;

		AddressMode	address_mode( CudaEnum( _sampler.wrapMode.x ),
								  CudaEnum( _sampler.wrapMode.y ),
								  CudaEnum( _sampler.wrapMode.z ) );

		if ( normalizedCoords and Any( address_mode != AddressMode( CU_TR_ADDRESS_MODE_CLAMP ) ) )
		{
			address_mode = AddressMode( CU_TR_ADDRESS_MODE_CLAMP );
		}

		CU_CALL( cuTexRefSetAddressMode( texref, 0, address_mode.x ) );
		CU_CALL( cuTexRefSetAddressMode( texref, 1, address_mode.y ) );
		CU_CALL( cuTexRefSetAddressMode( texref, 2, address_mode.z ) );

		CUfilter_mode	filter			= CU_TR_FILTER_MODE_LINEAR;
		CUfilter_mode	mipmap_filter	= CU_TR_FILTER_MODE_LINEAR;
		uint			anisotropy		= 0;

		CHECK( CudaEnum( _sampler.filter, filter, mipmap_filter, anisotropy ) );

		CU_CALL( cuTexRefSetFilterMode( texref, filter ) );
		CU_CALL( cuTexRefSetMipmapFilterMode( texref, mipmap_filter ) );
		CU_CALL( cuTexRefSetMaxAnisotropy( texref, anisotropy ) );

		CUarray_format	format		= CU_AD_FORMAT_FLOAT;
		uint			channels	= 0;
		bool			integer		= false;
		bool			normalized	= false;

		CHECK( CudaEnum( _format, format, channels, integer, normalized ) );

		uint const		flags		= (integer ? CU_TRSF_READ_AS_INTEGER : 0) |
									  (normalizedCoords ? CU_TRSF_NORMALIZED_COORDINATES : 0);

		CU_CALL( cuTexRefSetFlags( texref, flags ) );
		CU_CALL( cuTexRefSetFormat( texref, format, channels ) );
		 
		//CU_CALL( cuTexRefSetMipmapLevelBias( texref, 0.0f ) );
		
		const uint		min_level	= _levels.MinIndex();
		const uint		max_level	= _levels.MaxIndex();

		ASSERT( _levels.HasInterval( min_level, max_level ) );

		CU_CALL( cuTexRefSetMipmapLevelClamp( texref, float(min_level), float(max_level) ) );
	}

/*
=================================================
	New
=================================================
*/
	ComputeTexturePtr  CudaComputeTexture::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new CudaComputeTexture( ss ) );
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_CUDA
