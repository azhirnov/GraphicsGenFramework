// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Enums.h"
#include "Engine/Graphics/VertexAttribs/VertexDesc.h"

namespace Engine
{
namespace Graphics
{
	
//-----------------------------------------------------------------------------//
// ETextureData

	inline uint ETextureData::NumChannels (type value)
	{
		uint	channels = 0;

		if ( EnumEq( value, DiffuseColor ) )	channels += 3;
		if ( EnumEq( value, Normal ) )			channels += 2;
		if ( EnumEq( value, Height ) )			channels += 1;
		if ( EnumEq( value, Opacity ) )			channels += 1;
		if ( EnumEq( value, Specular ) )		channels += 1;
		// ignore DepthStencil
		if ( EnumEq( value, Emissive ) )		channels += 1;
		if ( EnumEq( value, Ambient ) )			channels += 1;
		if ( EnumEq( value, Shininess ) )		channels += 1;
		if ( EnumEq( value, Displacement ) )	channels += 1;

		return channels;
	}

	inline bool ETextureData::IsColor (type value)
	{
		return not IsDepthStencil( value ) and value != Unknown;
	}

	inline bool ETextureData::IsDepthStencil (type value)
	{
		return EnumEq( value, DepthStencil );
	}

	inline ETextureData::type operator | (ETextureData::type left, ETextureData::type right)
	{
		return ETextureData::type( uint(left) | uint(right) );
	}
	


//-----------------------------------------------------------------------------//
// EUniform

	inline EUniform::type EUniform::From (ETexture::type tex, EPixelFormat::type fmt)
	{
		bool	is_shadow	= false;
		uint	result		= 0;

		if ( EPixelFormat::Format::HasDepth( fmt ) )
			is_shadow = true;
		else
		if ( EPixelFormat::ValueType::IsSignedInteger( fmt ) )
			result |= EValueTypeInfo::_INT;
		else
		if ( EPixelFormat::ValueType::IsUnsignedInteger( fmt ) )
			result |= EValueTypeInfo::_UINT;
		else
			result |= EValueTypeInfo::_FLOAT;

		// sampler type
		switch ( tex )
		{
			case ETexture::Tex1D :			result |= EValueTypeInfo::_SAMP_1D;		break;
			case ETexture::Tex2D :			result |= (is_shadow ? EValueTypeInfo::_SAMP_2D : EValueTypeInfo::_SAMP_2DS);	break;
			case ETexture::Tex2DArray :		result |= (is_shadow ? EValueTypeInfo::_SAMP_2DA : EValueTypeInfo::_SAMP_2DAS);	break;
			case ETexture::Tex2DMS :		result |= EValueTypeInfo::_SAMP_2DMS;	break;
			case ETexture::Tex2DMSArray :	result |= EValueTypeInfo::_SAMP_2DAMS;	break;
			case ETexture::TexCube :		result |= (is_shadow ? EValueTypeInfo::_SAMP_CUBE : EValueTypeInfo::_SAMP_CUBES);	break;
			case ETexture::TexCubeArray :	result |= EValueTypeInfo::_SAMP_CUBEA;	break;
			case ETexture::Tex3D :			result |= EValueTypeInfo::_SAMP_3D;		break;
			case ETexture::Buffer :			result |= EValueTypeInfo::_SAMP_BUF;	break;
			default :						RETURN_ERR( "invalid texture type" );
		}

		return type( result );
	}

	

//-----------------------------------------------------------------------------//
// EFragOutput

	inline EFragOutput::type  EFragOutput::From (EPixelFormat::type fmt)
	{
		CHECK_ERR( EPixelFormat::Format::IsColor( fmt ), EFragOutput::Unknown );

		if ( EPixelFormat::ValueType::IsSignedInteger( fmt ) )
			return EFragOutput::Int4;

		if ( EPixelFormat::ValueType::IsUnsignedInteger( fmt ) )
			return EFragOutput::UInt4;

		if ( EPixelFormat::ValueType::IsFloat( fmt ) )
			return EFragOutput::Float4;

		RETURN_ERR( "invalid pixel format" );
	}



//-----------------------------------------------------------------------------//
// EAttribute
	
	inline EAttribute::type EAttribute::Build (type scalarValue, uint count)
	{
		return type( (scalarValue & EValueTypeInfo::_TYPE_MASK) | (count << EValueTypeInfo::_COL_OFF) );
	}
	
	inline EAttribute::type EAttribute::SetNormalized (type value, bool norm)
	{
		return EAttribute::type( (value & ~EAttribute::NormalizedFlag) | (norm ? EAttribute::NormalizedFlag : 0) );
	}

	inline EAttribute::type  EAttribute::ValueType::ToVSInputType (type value)
	{
		const type	count	= type( value & EValueTypeInfo::_COL_MASK );
		const bool	norm	= EnumEq( value, EValueTypeInfo::_NORM );
		const type	ftype	= type( EValueTypeInfo::_FLOAT | count );

		ASSERT( count >= EValueTypeInfo::_COL1 and count <= EValueTypeInfo::_COL4 );

		switch ( value & (EValueTypeInfo::_TYPE_MASK | EValueTypeInfo::_UNSIGNED) )
		{
			case EValueTypeInfo::_BYTE	:
			case EValueTypeInfo::_SHORT	:
			case EValueTypeInfo::_INT	:
				return norm ? ftype : type( EValueTypeInfo::_INT | count );

			case EValueTypeInfo::_UBYTE	:
			case EValueTypeInfo::_USHORT	:
			case EValueTypeInfo::_UINT	:
				return norm ? ftype : type( EValueTypeInfo::_UINT | count );

			case EValueTypeInfo::_HALF	:
			case EValueTypeInfo::_FLOAT	:
				return type( EValueTypeInfo::_FLOAT | count );

			case EValueTypeInfo::_DOUBLE	:
				return type( EValueTypeInfo::_DOUBLE | count );
		}
			
		RETURN_ERR( "invalid attrib type", ftype );
	}

	inline bool EAttribute::ValueType::IsInteger (type value)
	{
		return	EnumEqMask( ToVSInputType( value ), EValueTypeInfo::_INT,  EValueTypeInfo::_TYPE_MASK ) or
				EnumEqMask( ToVSInputType( value ), EValueTypeInfo::_UINT, EValueTypeInfo::_TYPE_MASK );
	}

	inline bool EAttribute::ValueType::IsFloat (type value)
	{
		return EnumEqMask( ToVSInputType( value ), EValueTypeInfo::_FLOAT, EValueTypeInfo::_TYPE_MASK );
	}

	inline bool EAttribute::ValueType::IsDouble (type value)
	{
		return EnumEqMask( ToVSInputType( value ), EValueTypeInfo::_DOUBLE, EValueTypeInfo::_TYPE_MASK );
	}
	
	

//-----------------------------------------------------------------------------//
// EIndex

	inline BytesU EIndex::SizeOf (type value)
	{
		switch ( value )
		{
			case UByte	:	return BytesU( CompileTime::SizeOf<ubyte>::bytes );
			case UShort	:	return BytesU( CompileTime::SizeOf<ushort>::bytes );
			case UInt	:	return BytesU( CompileTime::SizeOf<uint>::bytes );
		}

		RETURN_ERR( "invalid index type" );
	}



//-----------------------------------------------------------------------------//
// EPixelFormat

	inline bool EPixelFormat::IsCompressed (type value)
	{
		return EnumEq( value, EValueTypeInfo::_COMPRESSED );
	}

	inline uint EPixelFormat::NumColorChannels (type value)
	{
		const uint	num = ( value & EValueTypeInfo::_COL_MASK );
		return num >= EValueTypeInfo::_R and num <= EValueTypeInfo::_RGBA ? ( num >> EValueTypeInfo::_COL_OFF ): 0;
	}
	
	inline BitsU EPixelFormat::BitPerPixel (type value)
	{
		if ( IsCompressed( value ) ) {
			TODO( "" );
			return BitsU( 1 );
		}
		
		uint	val_size = 0;

		switch ( value & EValueTypeInfo::_TYPE_MASK )
		{
			case EValueTypeInfo::_BYTE :						val_size = 8;		break;
			case EValueTypeInfo::_SHORT :						val_size = 16;		break;
			case EValueTypeInfo::_INT :							val_size = 32;		break;
			case EValueTypeInfo::_HALF :						val_size = 16;		break;
			case EValueTypeInfo::_FLOAT :						val_size = 32;		break;
			case EValueTypeInfo::_DOUBLE :						val_size = 64;		break;
			case EValueTypeInfo::_INT_10_10_10_2 :				val_size = 32;		break;
			case EValueTypeInfo::_FLOAT_11_11_10 :				val_size = 32;		break;
			case EValueTypeInfo::_USHORT_4_4_4_4 :				val_size = 16;		break;
			case EValueTypeInfo::_USHORT_5_5_5_1 :				val_size = 16;		break;
			case EValueTypeInfo::_USHORT_5_6_5 :				val_size = 16;		break;
			case EValueTypeInfo::_INT_10_10_10 :				val_size = 32;		break;
			case EValueTypeInfo::_USHORT_5_5_5 :				val_size = 16;		break;
			case EValueTypeInfo::_INT24 :						val_size = 24;		break;
			case EValueTypeInfo::_DEPTH_24_STENCIL_8 :			val_size = 24+8;	break;
			case EValueTypeInfo::_FLOAT_DEPTH_32_STENCIL_8 :	val_size = 32+8;	break;
			default :											RETURN_ERR( "unknown type!" );
		}

		const uint	comp_count	= (value & EValueTypeInfo::_COL_MASK) >> EValueTypeInfo::_COL_OFF;

		return BitsU( val_size * comp_count );
	}

	template <typename T>
	inline EPixelFormat::type EPixelFormat::FromType (const T&, bool normalized)
	{
		return EPixelFormat::type( VertexDesc<T>::attrib | (normalized ? EValueTypeInfo::_NORM : 0) );
	}
	
	template <typename T>
	inline EPixelFormat::type EPixelFormat::FromNormType (const T& t)
	{
		return FromType( t, true );
	}

	inline bool EPixelFormat::ColorSpace::IsNonLinear (type value)
	{
		return EnumEq( value, EValueTypeInfo::_SRGB );
	}

	inline bool EPixelFormat::ColorSpace::IsLinear (type value)
	{
		return not IsNonLinear( value );
	}
	
	inline bool EPixelFormat::Format::IsColor (type value)
	{
		return NumColorChannels( value ) > 0;
	}

	inline bool EPixelFormat::Format::IsDepth (type value)
	{
		return ( value & EValueTypeInfo::_COL_MASK ) == EValueTypeInfo::_DEPTH;
	}

	inline bool EPixelFormat::Format::IsStencil (type value)
	{
		return ( value & EValueTypeInfo::_COL_MASK ) == EValueTypeInfo::_STENCIL;
	}

	inline bool EPixelFormat::Format::IsDepthStencil (type value)
	{
		return ( value & EValueTypeInfo::_COL_MASK ) == EValueTypeInfo::_DEPTH_STENCIL;
	}
	
	inline bool EPixelFormat::Format::HasDepth (type value)
	{
		return IsDepth( value ) or IsDepthStencil( value );
	}

	inline bool EPixelFormat::Format::HasStencil (type value)
	{
		return IsStencil( value ) or IsDepthStencil( value );
	}

	inline bool EPixelFormat::DataType::IsFloat (type value)
	{
		const uint val = (value & EValueTypeInfo::_TYPE_MASK);
		return	val == EValueTypeInfo::_HALF						or
				val == EValueTypeInfo::_FLOAT						or
				val == EValueTypeInfo::_DOUBLE						or
				val == EValueTypeInfo::_FLOAT_11_11_10				or
				val == (BPTC_RGB16F & EValueTypeInfo::_TYPE_MASK)	or
				val == (BPTC_RGB16F_Unsigned & EValueTypeInfo::_TYPE_MASK);
	}

	inline bool EPixelFormat::DataType::IsInteger (type value)
	{
		return not IsFloat( value );
	}

	inline bool EPixelFormat::ValueType::IsUnsignedInteger (type value)
	{
		return not EnumEq( value, EValueTypeInfo::_NORM ) and EnumEq( value, EValueTypeInfo::_UNSIGNED ) and DataType::IsInteger( value );
	}

	inline bool EPixelFormat::ValueType::IsSignedInteger (type value)
	{
		return not EnumEq( value, EValueTypeInfo::_NORM ) and not EnumEq( value, EValueTypeInfo::_UNSIGNED ) and DataType::IsInteger( value );
	}

	inline bool EPixelFormat::ValueType::IsUnsignedNormalized (type value)
	{
		return EnumEq( value, EValueTypeInfo::_NORM ) and EnumEq( value, EValueTypeInfo::_UNSIGNED ) and DataType::IsInteger( value );
	}

	inline bool EPixelFormat::ValueType::IsSignedNormalized (type value)
	{
		return EnumEq( value, EValueTypeInfo::_NORM ) and not EnumEq( value, EValueTypeInfo::_UNSIGNED ) and DataType::IsInteger( value );
	}

	inline bool EPixelFormat::ValueType::IsSignedFloat (type value)
	{
		return not EnumEq( value, EValueTypeInfo::_UNSIGNED ) and DataType::IsFloat( value );
	}

	inline bool EPixelFormat::ValueType::IsUnsignedFloat (type value)
	{
		return EnumEq( value, EValueTypeInfo::_UNSIGNED ) and DataType::IsFloat( value );
	}

	inline bool EPixelFormat::ValueType::IsFloat (type value)
	{
		return EnumEq( value, EValueTypeInfo::_NORM ) or DataType::IsFloat( value );
	}

	inline bool EPixelFormat::ValueType::IsInteger (type value)
	{
		return not IsFloat( value );
	}

	

//-----------------------------------------------------------------------------//
// EBlendFunc
	
	inline StringCRef EBlendFunc::ToString (type value)
	{
		switch ( value )
		{
			case EBlendFunc::Zero					:	return "Zero";
			case EBlendFunc::One					:	return "One";
			case EBlendFunc::SrcColor				:	return "SrcColor";
			case EBlendFunc::OneMinusSrcColor		:	return "OneMinusSrcColor";
			case EBlendFunc::DstColor				:	return "DstColor";
			case EBlendFunc::OneMinusDstColor		:	return "OneMinusDstColor";
			case EBlendFunc::SrcAlpha				:	return "SrcAlpha";
			case EBlendFunc::OneMinusSrcAlpha		:	return "OneMinusSrcAlpha";
			case EBlendFunc::DstAlpha				:	return "DstAlpha";
			case EBlendFunc::OneMinusDstAlpha		:	return "OneMinusDstAlpha";
			case EBlendFunc::ConstColor				:	return "ConstColor";
			case EBlendFunc::OneMinusConstColor		:	return "OneMinusConstColor";
			case EBlendFunc::ConstAlpha				:	return "ConstAlpha";
			case EBlendFunc::OneMinusConstAlpha		:	return "OneMinusConstAlpha";
			case EBlendFunc::SrcAlphaSaturate		:	return "SrcAlphaSaturate";
		}
		RETURN_ERR( "invalid blend function type", "unknown" );
	}



//-----------------------------------------------------------------------------//
// EBlendEq
	
	inline StringCRef EBlendEq::ToString (type value)
	{
		switch( value )
		{
			case EBlendEq::Add		:	return "Add";
			case EBlendEq::Sub		:	return "Sub";
			case EBlendEq::RevSub	:	return "ReverseSub";
			case EBlendEq::Min		:	return "Min";
			case EBlendEq::Max		:	return "Max";
		}
		RETURN_ERR( "invalid blend equation type", "unknown" );
	}


//-----------------------------------------------------------------------------//
// ECompareFunc
	
	inline StringCRef ECompareFunc::ToString (type value)
	{
		switch ( value )
		{
			case ECompareFunc::None		:	return "None";
			case ECompareFunc::Never	:	return "Never";
			case ECompareFunc::Less		:	return "Less";
			case ECompareFunc::Equal	:	return "Equal";
			case ECompareFunc::LEqual	:	return "LessEqual";
			case ECompareFunc::Greater	:	return "Greater";
			case ECompareFunc::NotEqual	:	return "NotEqual";
			case ECompareFunc::GEqual	:	return "GreaterEqual";
			case ECompareFunc::Always	:	return "Always";
		}
		RETURN_ERR( "invalid compare function type", "unknown" );
	}


	
//-----------------------------------------------------------------------------//
// EStencilOp
	
	inline StringCRef EStencilOp::ToString (type value)
	{
		switch ( value )
		{
			case EStencilOp::Keep		:	return "Keep";
			case EStencilOp::Zero		:	return "Zero";
			case EStencilOp::Replace	:	return "Replace";
			case EStencilOp::Incr		:	return "Increment";
			case EStencilOp::IncrWrap	:	return "IncrementWrao";
			case EStencilOp::Decr		:	return "Decrement";
			case EStencilOp::DecrWrap	:	return "DecrementWrap";
			case EStencilOp::Invert		:	return "Invert";
		}
		RETURN_ERR( "invalid stencil operation type", "unknown" );
	}



//-----------------------------------------------------------------------------//
// EPolygonMode
	
	inline StringCRef EPolygonMode::ToString (type value)
	{
		switch ( value )
		{
			case EPolygonMode::Point	:	return "Point";
			case EPolygonMode::Line		:	return "Line";
			case EPolygonMode::Fill		:	return "Fill";
		}
		RETURN_ERR( "invalid polygon mode", "unknown" );
	}



//-----------------------------------------------------------------------------//
// EWrapMode
	
	inline StringCRef EWrapMode::ToString (type value)
	{
		switch ( value )
		{
			case EWrapMode::Clamp			:	return "Clamp";
			case EWrapMode::Repeat			:	return "Repeat";
			case EWrapMode::MirroredRepeat	:	return "MirroredRepeat";
			case EWrapMode::MirroredClamp	:	return "MirroredClamp";
		}
		RETURN_ERR( "invalid wrap mode", "unknown" );
	}



//-----------------------------------------------------------------------------//
// EFilter
	
	inline StringCRef EFilter::ToString (type value)
	{
		switch ( value )
		{
			case EFilter::MinMagMipNearest					:	return "MinMagMipNearest";
			case EFilter::MinMagNearest_MipLinear			:	return "MinMagNearest_MipLinear";
			case EFilter::MinNearest_MagLinear_MipNearest	:	return "MinNearest_MagLinear_MipNearest";
			case EFilter::MinNearest_MagMipLinear			:	return "MinNearest_MagMipLinear";
			case EFilter::MinLinear_MagMipNearest			:	return "MinLinear_MagMipNearest";
			case EFilter::MinLinear_MagNearest_MipLinear	:	return "MinLinear_MagNearest_MipLinear";
			case EFilter::MinMagLinear_MipNearest			:	return "MinMagLinear_MipNearest";
			case EFilter::MinMagMipLinear					:	return "MinMagMipLinear";
			case EFilter::Anisotropic_2						:	return "Anisotropic_2";
			case EFilter::Anisotropic_4						:	return "Anisotropic_4";
			case EFilter::Anisotropic_8						:	return "Anisotropic_8";
			case EFilter::Anisotropic_12					:	return "Anisotropic_12";
			case EFilter::Anisotropic_16					:	return "Anisotropic_16";
		}
		RETURN_ERR( "invalid filter type", "unknown" );
	}



//-----------------------------------------------------------------------------//
// EShader
	
	inline StringCRef EShader::ToString (type value)
	{
		switch ( value )
		{
			case EShader::Vertex			:	return "Vertex";
			case EShader::TessControl		:	return "TessellationControl";
			case EShader::TessEvaluation	:	return "TessellationEvaluation";
			case EShader::Geometry			:	return "Geometry";
			case EShader::Fragment			:	return "Fragment";
			case EShader::Compute			:	return "Compute";
		}
		RETURN_ERR( "invalid shader type", "unknown" );
	}
	


//-----------------------------------------------------------------------------//
// ETexture
	
	inline bool ETexture::IsMultisampled (type value)
	{
		switch ( value )
		{
			case ETexture::Tex2DMS :
			case ETexture::Tex2DMSArray :
				return true;
		}
		return false;
	}



//-----------------------------------------------------------------------------//
// EPointSpriteCoordOrigin
	
	inline StringCRef EPointSpriteCoordOrigin::ToString (type value)
	{
		switch ( value )
		{
			case EPointSpriteCoordOrigin::LowerLeft :	return "LowerLeft";
			case EPointSpriteCoordOrigin::UpperLeft :	return "UpperLeft";
		}
		RETURN_ERR( "invalid point sprite coord origin type", "unknown" );
	}


	
//-----------------------------------------------------------------------------//
// EShaderCompilationFlags
	
	inline bool EShaderCompilationFlags::IsCompatible (type left, type right)
	{
		return (left & _FpMask) == (right & _FpMask);
	}
	
	inline bool EShaderCompilationFlags::IsValid (type value)
	{
		return EnumEq( value, FP_32 ) != EnumEq( value, FP_64 );	// may be 32 or 64, but not both
	}


}	// Graphics
}	// Engine
