// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "cuda.h"
#include "../Types/Enums.h"

#if defined( COMPUTE_API_CUDA )

namespace Engine
{
namespace Compute
{
	
	enum CudaEventFlags {};

	
/*
=================================================
	EventFlags
=================================================
*/
	inline CudaEventFlags CudaEnum (EEventFlags::type value)
	{
		uint	flags = cuda::CU_EVENT_DEFAULT;

		if ( EnumEq( value, EEventFlags::BLOCKING_SYNC ) )
			flags |= cuda::CU_EVENT_BLOCKING_SYNC;

		if ( EnumEq( value, EEventFlags::DISABLE_TIMING ) )
			flags |= cuda::CU_EVENT_DISABLE_TIMING;

		if ( EnumEq( value, EEventFlags::INTERPROCESS ) )
			flags |= (cuda::CU_EVENT_INTERPROCESS | cuda::CU_EVENT_DISABLE_TIMING);

		return CudaEventFlags( flags );
	}

/*
=================================================
	Filter
=================================================
*/
	inline bool CudaEnum (EFilter::type value, OUT cuda::CUfilter_mode &filter,
						  OUT cuda::CUfilter_mode &mipmapFilter, OUT uint &anisotropy)
	{
		static const uint	mip_mask = (EFilter::_MIP_NEAREST | EFilter::_MIP_LINEAR);
		static const uint	min_mask = (EFilter::_MIN_NEAREST | EFilter::_MIN_LINEAR);
		
		switch ( value & min_mask )
		{
			case EFilter::_MIN_NEAREST	:	filter = cuda::CU_TR_FILTER_MODE_POINT;		break;
			case EFilter::_MIN_LINEAR	:	filter = cuda::CU_TR_FILTER_MODE_LINEAR;	break;
			default						:	RETURN_ERR( "invalid filtering flag" );
		}

		switch ( value & mip_mask )
		{
			case EFilter::_MIP_NEAREST	:	mipmapFilter = cuda::CU_TR_FILTER_MODE_POINT;	break;
			case EFilter::_MIP_LINEAR	:	mipmapFilter = cuda::CU_TR_FILTER_MODE_LINEAR;	break;
			default						:	RETURN_ERR( "invalid mipmap filtering flag" );
		}

		if ( EnumEq( value, EFilter::_ANISOTROPIC ) )
			anisotropy = uint( value & EFilter::_A_FACTOR_MASK );
		else
			anisotropy = 0;

		return true;
	}
	
/*
=================================================
	AddressMode
=================================================
*/
	inline cuda::CUaddress_mode CudaEnum (EWrapMode::type value)
	{
		switch ( value )
		{
			case EWrapMode::CLAMP			:	return cuda::CU_TR_ADDRESS_MODE_CLAMP;
			case EWrapMode::REPEAT			:	return cuda::CU_TR_ADDRESS_MODE_WRAP;
			case EWrapMode::MIRRORED_REPEAT	:	return cuda::CU_TR_ADDRESS_MODE_MIRROR;
			case EWrapMode::MIRRORED_CLAMP	:	return cuda::CU_TR_ADDRESS_MODE_MIRROR;
		}
		RETURN_ERR( "unsupported address mode", cuda::CUaddress_mode(0) );
	}
	
/*
=================================================
	ArrayFormat
=================================================
*/
	inline bool CudaEnum (EPixelFormat::type value, OUT cuda::CUarray_format &format, OUT uint &numChannels,
						  OUT bool &integer, OUT bool &normalized)
	{
		CHECK_ERR( not EPixelFormat::IsCompressed( value ) );
		CHECK_ERR( EPixelFormat::Format::IsColor( value ) );

		numChannels = EPixelFormat::NumColorChannels( value );
		integer		= not EPixelFormat::ValueType::IsFloat( value );
		normalized	= EPixelFormat::ValueType::IsUnsignedNormalized( value ) or
					  EPixelFormat::ValueType::IsSignedNormalized( value );

		switch ( value )
		{
			case EPixelFormat::R8_SNORM :
			case EPixelFormat::RG8_SNORM :
			case EPixelFormat::RGB8_SNORM :
			case EPixelFormat::RGBA8_SNORM :
			case EPixelFormat::R8I :
			case EPixelFormat::RG8I :
			case EPixelFormat::RGB8I :
			case EPixelFormat::RGBA8I :			format = cuda::CU_AD_FORMAT_SIGNED_INT8;	break;
			case EPixelFormat::R16_SNORM :
			case EPixelFormat::RG16_SNORM :
			case EPixelFormat::RGB16_SNORM :
			case EPixelFormat::RGBA16_SNORM :
			case EPixelFormat::R16I :
			case EPixelFormat::RG16I :
			case EPixelFormat::RGB16I :
			case EPixelFormat::RGBA16I :		format = cuda::CU_AD_FORMAT_SIGNED_INT16;	break;
			case EPixelFormat::R32I :
			case EPixelFormat::RG32I :
			case EPixelFormat::RGB32I :
			case EPixelFormat::RGBA32I :		format = cuda::CU_AD_FORMAT_SIGNED_INT32;	break;
			case EPixelFormat::R8_UNORM :
			case EPixelFormat::RG8_UNORM :
			case EPixelFormat::RGB8_UNORM :
			case EPixelFormat::RGBA8_UNORM :
			case EPixelFormat::R8U :
			case EPixelFormat::RG8U :
			case EPixelFormat::RGB8U :
			case EPixelFormat::RGBA8U :			format = cuda::CU_AD_FORMAT_UNSIGNED_INT8;	break;
			case EPixelFormat::R16_UNORM :
			case EPixelFormat::RG16_UNORM :
			case EPixelFormat::RGB16_UNORM :
			case EPixelFormat::RGBA16_UNORM :
			case EPixelFormat::R16U :
			case EPixelFormat::RG16U :
			case EPixelFormat::RGB16U :
			case EPixelFormat::RGBA16U :		format = cuda::CU_AD_FORMAT_UNSIGNED_INT16;	break;
			case EPixelFormat::R32U :
			case EPixelFormat::RG32U :
			case EPixelFormat::RGB32U :
			case EPixelFormat::RGBA32U :		format = cuda::CU_AD_FORMAT_UNSIGNED_INT32;	break;
			case EPixelFormat::R16F :
			case EPixelFormat::RG16F :
			case EPixelFormat::RGB16F :
			case EPixelFormat::RGBA16F :		format = cuda::CU_AD_FORMAT_HALF;			break;
			case EPixelFormat::R32F :
			case EPixelFormat::RG32F :
			case EPixelFormat::RGB32F :
			case EPixelFormat::RGBA32F :		format = cuda::CU_AD_FORMAT_FLOAT;			break;
			default :							RETURN_ERR( "invalid pixel format" );
		}

		return true;
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_CUDA
