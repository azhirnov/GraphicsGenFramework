// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CL2Emulator.h"
#include "Engine/Compute/Types/Enums.h"

#if defined( COMPUTE_API_OPENCL )

namespace Engine
{
namespace Compute
{
	using namespace Engine::Graphics;

	enum CL2MemoryAccess {};
	enum CL2ImageType {};
	enum CL2ChannelOrder {};
	enum CL2ChannelDataType {};
	enum CL2BytePerPixel {};

	
/*
=================================================
	MemoryAccess
=================================================
*/
	inline CL2MemoryAccess CL2Enum (EMemoryAccess::type value)
	{
		switch ( value )
		{
			case EMemoryAccess::Read		:	return (CL2MemoryAccess) cl::CL_MEM_READ_ONLY;
			case EMemoryAccess::Write		:	return (CL2MemoryAccess) cl::CL_MEM_WRITE_ONLY;
			case EMemoryAccess::ReadWrite	:	return (CL2MemoryAccess) cl::CL_MEM_READ_WRITE;
		}
		RETURN_ERR( "invalid memory flag", CL2MemoryAccess() );
	}
	
/*
=================================================
	ImageType
=================================================
*/
	inline CL2ImageType CL2Enum (ETexture::type value)
	{
		switch ( value )
		{
			case ETexture::Tex2D		:	return (CL2ImageType) cl::CL_MEM_OBJECT_IMAGE2D;
			case ETexture::Tex3D		:	return (CL2ImageType) cl::CL_MEM_OBJECT_IMAGE3D;

#		 if COMPUTE_API_OPENCL >= 120
			case ETexture::Buffer	:	return (CL2ImageType) cl::CL_MEM_OBJECT_IMAGE1D_BUFFER;
			case ETexture::Tex1D		:	return (CL2ImageType) cl::CL_MEM_OBJECT_IMAGE1D;
			case ETexture::Tex1DArray	:	return (CL2ImageType) cl::CL_MEM_OBJECT_IMAGE1D_ARRAY;
			case ETexture::Tex2DArray	:	return (CL2ImageType) cl::CL_MEM_OBJECT_IMAGE2D_ARRAY;
#		 endif
		}
		RETURN_ERR( "invalid image type!", CL2ImageType() );
	}
	
/*
=================================================
	PixelFormat
=================================================
*/
	inline bool CL2Enum (EPixelFormat::type value, OUT CL2ChannelOrder &order, OUT CL2ChannelDataType &type, OUT Bytes<usize> &bpp)
	{
		const bool	norm = EnumEq( value, EValueTypeInfo::_NORM );
		const bool	srgb = EnumEq( value, EValueTypeInfo::_SRGB );

		bpp = Bytes<usize>( EPixelFormat::BitPerPixel( value ) );

		switch ( value & (EValueTypeInfo::_TYPE_MASK | EValueTypeInfo::_UNSIGNED) )
		{
			case EValueTypeInfo::_BYTE						:	type = (CL2ChannelDataType) (norm ? cl::CL_SNORM_INT8  : cl::CL_SIGNED_INT8);		break;
			case EValueTypeInfo::_SHORT						:	type = (CL2ChannelDataType) (norm ? cl::CL_SNORM_INT16 : cl::CL_SIGNED_INT16);		break;
			case EValueTypeInfo::_INT						:	type = (CL2ChannelDataType) cl::CL_SIGNED_INT32;									break;
			case EValueTypeInfo::_UBYTE						:	type = (CL2ChannelDataType) (norm ? cl::CL_UNORM_INT8  : cl::CL_UNSIGNED_INT8);		break;
			case EValueTypeInfo::_USHORT					:	type = (CL2ChannelDataType) (norm ? cl::CL_UNORM_INT16 : cl::CL_UNSIGNED_INT16);	break;
			case EValueTypeInfo::_UINT						:	type = (CL2ChannelDataType) cl::CL_UNSIGNED_INT32;									break;
			case EValueTypeInfo::_HALF						:	type = (CL2ChannelDataType) cl::CL_HALF_FLOAT;			break;
			case EValueTypeInfo::_FLOAT						:	type = (CL2ChannelDataType) cl::CL_FLOAT;				break;
			case EValueTypeInfo::_INT_10_10_10				:	type = (CL2ChannelDataType) cl::CL_UNORM_INT_101010;	break;
			case EValueTypeInfo::_USHORT_5_5_5				:	type = (CL2ChannelDataType) cl::CL_UNORM_SHORT_555;		break;
			case EValueTypeInfo::_USHORT_5_6_5				:	type = (CL2ChannelDataType) cl::CL_UNORM_SHORT_565;		break;
			//case EValueTypeInfo::_FLOAT_DEPTH_32_STENCIL_8	:	type = (CL2ChannelDataType) cl::CL_FLOAT;				break;
				
#		 if COMPUTE_API_OPENCL >= 120
			case EValueTypeInfo::_INT24						:	type = (CL2ChannelDataType) cl::CL_UNORM_INT24;			break;
			//case EValueTypeInfo::_DEPTH_24_STENCIL_8		:	type = (CL2ChannelDataType) cl::CL_UNORM_INT24;			break;
#		 endif

			default											:	RETURN_ERR( "invalid pixel type" );
		}

		switch ( value & EValueTypeInfo::_COL_MASK )
		{
			case EValueTypeInfo::_R				:	order = (CL2ChannelOrder) cl::CL_R;								break;
			case EValueTypeInfo::_RG			:	order = (CL2ChannelOrder) cl::CL_RG;							break;
				
#		 if COMPUTE_API_OPENCL >= 120
			case EValueTypeInfo::_DEPTH			:	order = (CL2ChannelOrder) cl::CL_DEPTH;							break;
			case EValueTypeInfo::_DEPTH_STENCIL	:	order = (CL2ChannelOrder) cl::CL_DEPTH_STENCIL;					break;
#		 endif

#		 if COMPUTE_API_OPENCL >= 200
			case EValueTypeInfo::_RGB			:	order = (CL2ChannelOrder) (srgb ? cl::CL_sRGB : cl::CL_RGB);	break;
			case EValueTypeInfo::_RGBA			:	order = (CL2ChannelOrder) (srgb ? cl::CL_sRGBA : cl::CL_RGBA);	break;
#		 else
			case EValueTypeInfo::_RGB			:	order = (CL2ChannelOrder) cl::CL_RGB;							break;	// Warning: this format has some problems!
			case EValueTypeInfo::_RGBA			:	order = (CL2ChannelOrder) cl::CL_RGBA;							break;
#		 endif

			default								:	RETURN_ERR( "invalid pixel format" );
		}
		return true;
	}


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENCL
