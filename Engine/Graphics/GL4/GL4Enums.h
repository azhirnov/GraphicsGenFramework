// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "GL4Emulator.h"

#if defined( GRAPHICS_API_OPENGL )

#include "Engine/Graphics/Types/Enums.h"

namespace Engine
{
namespace Graphics
{

	enum GL4BlendFunc {};
	enum GL4BlendEq {};
	enum GL4CompareFunc {};
	enum GL4StencilOp {};
	enum GL4PolygonMode {};
	enum GL4WrapMode {};
	enum GL4MinFilter {};
	enum GL4MagFilter {};
	enum GL4AnisotropyFilter {};
	enum GL4TextureTarget {};
	enum GL4CubeMapFace {};
	enum GL4InternalPixelFormat {};
	enum GL4PixelFormat {};
	enum GL4PixelType {};
	enum GL4VertexAttribType {};
	enum GL4RenderTarget {};
	enum GL4Primitive {};
	enum GL4Shader {};
	enum GL4BufferUsage {};
	enum GL4PointSpriteCoordOrigin {};
	enum GL4MemoryAccess {};
	enum GL4Index {};
	enum GL4QueryTarget {};
	enum GL4MemoryBarrier {};
	enum GL4BufferTarget {};
	enum GL4BufferBinding {};

	
/*
=================================================
	BlendFunc
=================================================
*/
	inline GL4BlendFunc GL4Enum (EBlendFunc::type value)
	{
		switch ( value )
		{
			case EBlendFunc::Zero					:	return (GL4BlendFunc) gl::GL_ZERO;
			case EBlendFunc::One					:	return (GL4BlendFunc) gl::GL_ONE;
			case EBlendFunc::SrcColor				:	return (GL4BlendFunc) gl::GL_SRC_COLOR;
			case EBlendFunc::OneMinusSrcColor		:	return (GL4BlendFunc) gl::GL_ONE_MINUS_SRC_COLOR;
			case EBlendFunc::DstColor				:	return (GL4BlendFunc) gl::GL_DST_COLOR;
			case EBlendFunc::OneMinusDstColor		:	return (GL4BlendFunc) gl::GL_ONE_MINUS_DST_COLOR;
			case EBlendFunc::SrcAlpha				:	return (GL4BlendFunc) gl::GL_SRC_ALPHA;
			case EBlendFunc::OneMinusSrcAlpha		:	return (GL4BlendFunc) gl::GL_ONE_MINUS_SRC_ALPHA;
			case EBlendFunc::DstAlpha				:	return (GL4BlendFunc) gl::GL_DST_ALPHA;
			case EBlendFunc::OneMinusDstAlpha		:	return (GL4BlendFunc) gl::GL_ONE_MINUS_DST_ALPHA;
			case EBlendFunc::ConstColor				:	return (GL4BlendFunc) gl::GL_CONSTANT_COLOR;
			case EBlendFunc::OneMinusConstColor		:	return (GL4BlendFunc) gl::GL_ONE_MINUS_CONSTANT_COLOR;
			case EBlendFunc::ConstAlpha				:	return (GL4BlendFunc) gl::GL_CONSTANT_ALPHA;
			case EBlendFunc::OneMinusConstAlpha		:	return (GL4BlendFunc) gl::GL_ONE_MINUS_CONSTANT_ALPHA;
			case EBlendFunc::SrcAlphaSaturate		:	return (GL4BlendFunc) gl::GL_SRC_ALPHA_SATURATE;
		}

		RETURN_ERR( "invalid blend func", GL4BlendFunc() );
	}
	
/*
=================================================
	BlendEq
=================================================
*/
	inline GL4BlendEq GL4Enum (EBlendEq::type value)
	{
		switch ( value )
		{
			case EBlendEq::Add		:	return (GL4BlendEq) gl::GL_FUNC_ADD;
			case EBlendEq::Sub		:	return (GL4BlendEq) gl::GL_FUNC_SUBTRACT;
			case EBlendEq::RevSub	:	return (GL4BlendEq) gl::GL_FUNC_REVERSE_SUBTRACT;
			case EBlendEq::Min		:	return (GL4BlendEq) gl::GL_MIN;
			case EBlendEq::Max		:	return (GL4BlendEq) gl::GL_MAX;
		}

		RETURN_ERR( "invalid blend equation", GL4BlendEq() );
	}
	
/*
=================================================
	CompareFunc
=================================================
*/
	inline GL4CompareFunc GL4Enum (ECompareFunc::type value)
	{
		switch ( value )
		{
			case ECompareFunc::None		:	return (GL4CompareFunc) gl::GL_NONE;
			case ECompareFunc::Never	:	return (GL4CompareFunc) gl::GL_NEVER;
			case ECompareFunc::Less		:	return (GL4CompareFunc) gl::GL_LESS;
			case ECompareFunc::Equal	:	return (GL4CompareFunc) gl::GL_EQUAL;
			case ECompareFunc::LEqual	:	return (GL4CompareFunc) gl::GL_LEQUAL;
			case ECompareFunc::Greater	:	return (GL4CompareFunc) gl::GL_GREATER;
			case ECompareFunc::NotEqual	:	return (GL4CompareFunc) gl::GL_NOTEQUAL;
			case ECompareFunc::GEqual	:	return (GL4CompareFunc) gl::GL_GEQUAL;
			case ECompareFunc::Always	:	return (GL4CompareFunc) gl::GL_ALWAYS;
		}

		RETURN_ERR( "invalid compare function", GL4CompareFunc() );
	}
	
/*
=================================================
	StencilOp
=================================================
*/
	inline GL4StencilOp GL4Enum (EStencilOp::type value)
	{
		switch ( value )
		{
			case EStencilOp::Keep		:	return (GL4StencilOp) gl::GL_KEEP;
			case EStencilOp::Zero		:	return (GL4StencilOp) gl::GL_ZERO;
			case EStencilOp::Replace	:	return (GL4StencilOp) gl::GL_REPLACE;
			case EStencilOp::Incr		:	return (GL4StencilOp) gl::GL_INCR;
			case EStencilOp::IncrWrap	:	return (GL4StencilOp) gl::GL_INCR_WRAP;
			case EStencilOp::Decr		:	return (GL4StencilOp) gl::GL_DECR;
			case EStencilOp::DecrWrap	:	return (GL4StencilOp) gl::GL_DECR_WRAP;
			case EStencilOp::Invert		:	return (GL4StencilOp) gl::GL_INVERT;
		}

		RETURN_ERR( "invalid stencil op", GL4StencilOp() );
	}
	
/*
=================================================
	PolygonMode
=================================================
*/
	inline GL4PolygonMode GL4Enum (EPolygonMode::type value)
	{
		switch ( value )
		{
			case EPolygonMode::Point	:	return (GL4PolygonMode) gl::GL_POINT;
			case EPolygonMode::Line		:	return (GL4PolygonMode) gl::GL_LINE;
			case EPolygonMode::Fill		:	return (GL4PolygonMode) gl::GL_FILL;
		}

		RETURN_ERR( "invalid polygon mode", GL4PolygonMode() );
	}
	
/*
=================================================
	Shader
=================================================
*/
	inline GL4Shader GL4Enum (EShader::type value)
	{
		switch ( value )
		{
			case EShader::Vertex			:	return (GL4Shader) gl::GL_VERTEX_SHADER;
			case EShader::TessControl		:	return (GL4Shader) gl::GL_TESS_CONTROL_SHADER;
			case EShader::TessEvaluation	:	return (GL4Shader) gl::GL_TESS_EVALUATION_SHADER;
			case EShader::Geometry			:	return (GL4Shader) gl::GL_GEOMETRY_SHADER;
			case EShader::Fragment			:	return (GL4Shader) gl::GL_FRAGMENT_SHADER;
			case EShader::Compute			:	return (GL4Shader) gl::GL_COMPUTE_SHADER;
		}

		RETURN_ERR( "invalid shader type", GL4Shader() );
	}
	
/*
=================================================
	WrapMode
=================================================
*/
	inline GL4WrapMode GL4Enum (EWrapMode::type value)
	{
		switch ( value )
		{
			case EWrapMode::Clamp			:	return (GL4WrapMode) gl::GL_CLAMP_TO_EDGE;
			case EWrapMode::MirroredRepeat	:	return (GL4WrapMode) gl::GL_MIRRORED_REPEAT;
			case EWrapMode::Repeat			:	return (GL4WrapMode) gl::GL_REPEAT;
			case EWrapMode::MirroredClamp	:	return (GL4WrapMode) gl::GL_MIRROR_CLAMP_TO_EDGE;
		}

		RETURN_ERR( "invalid wrap mode", GL4WrapMode() );
	}
	
/*
=================================================
	Filter
=================================================
*/
	inline bool GL4Enum (EFilter::type value, OUT GL4MinFilter &minFilter,
						 OUT GL4MagFilter &magFilter, OUT GL4AnisotropyFilter &aniso)
	{
		static const uint	min_mask = (EFilter::_MIN_NEAREST | EFilter::_MIN_LINEAR | EFilter::_MIP_NEAREST | EFilter::_MIP_LINEAR);
		static const uint	mag_mask = (EFilter::_MAG_NEAREST | EFilter::_MAG_LINEAR);

		minFilter	= GL4MinFilter();
		magFilter	= GL4MagFilter();
		aniso		= GL4AnisotropyFilter(0);

		switch ( value & min_mask )
		{
			case EFilter::_MIN_NEAREST							:	minFilter = (GL4MinFilter) gl::GL_NEAREST;				break;
			case EFilter::_MIN_LINEAR							:	minFilter = (GL4MinFilter) gl::GL_LINEAR;					break;
			case EFilter::_MIN_NEAREST | EFilter::_MIP_NEAREST	:	minFilter = (GL4MinFilter) gl::GL_NEAREST_MIPMAP_NEAREST;	break;
			case EFilter::_MIN_LINEAR  | EFilter::_MIP_NEAREST	:	minFilter = (GL4MinFilter) gl::GL_LINEAR_MIPMAP_NEAREST;	break;
			case EFilter::_MIN_NEAREST | EFilter::_MIP_LINEAR	:	minFilter = (GL4MinFilter) gl::GL_NEAREST_MIPMAP_LINEAR;	break;
			case EFilter::_MIN_LINEAR  | EFilter::_MIP_LINEAR	:	minFilter = (GL4MinFilter) gl::GL_LINEAR_MIPMAP_LINEAR;	break;
			default												:	RETURN_ERR( "invalid min and mip filtering flags" );
		}

		switch ( value & mag_mask )
		{
			case EFilter::_MAG_NEAREST	:	magFilter = (GL4MagFilter) gl::GL_NEAREST;	break;
			case EFilter::_MAG_LINEAR	:	magFilter = (GL4MagFilter) gl::GL_LINEAR;		break;
			default						:	RETURN_ERR( "invalid mag filtering flag" );
		}

		if ( EnumEq( value, EFilter::_ANISOTROPIC ) )
			aniso = GL4AnisotropyFilter( value & EFilter::_A_FACTOR_MASK );
		
		return true;
	}
	
/*
=================================================
	TextureTarget
=================================================
*/
	inline GL4TextureTarget GL4Enum (ETexture::type value)
	{
		switch ( value )
		{
			case ETexture::Tex1D			:	return (GL4TextureTarget) gl::GL_TEXTURE_1D;
			case ETexture::Tex2D			:	return (GL4TextureTarget) gl::GL_TEXTURE_2D;
			case ETexture::Tex2DArray		:	return (GL4TextureTarget) gl::GL_TEXTURE_2D_ARRAY;
			case ETexture::Tex2DMS			:	return (GL4TextureTarget) gl::GL_TEXTURE_2D_MULTISAMPLE;
			case ETexture::Tex2DMSArray		:	return (GL4TextureTarget) gl::GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
			case ETexture::TexCube			:	return (GL4TextureTarget) gl::GL_TEXTURE_CUBE_MAP;
			case ETexture::TexCubeArray		:	return (GL4TextureTarget) gl::GL_TEXTURE_CUBE_MAP_ARRAY;
			case ETexture::Tex3D			:	return (GL4TextureTarget) gl::GL_TEXTURE_3D;
			case ETexture::Buffer			:	return (GL4TextureTarget) gl::GL_TEXTURE_BUFFER;
		}

		RETURN_ERR( "invalid texture target", GL4TextureTarget() );
	}
	
/*
=================================================
	CubeMapFace
=================================================
*/
	inline GL4CubeMapFace GL4Enum (ECubeMapFace::type value)
	{
		switch ( value )
		{
			case ECubeMapFace::PositiveX	:	return (GL4CubeMapFace) gl::GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			case ECubeMapFace::NegativeX	:	return (GL4CubeMapFace) gl::GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
			case ECubeMapFace::PositiveY	:	return (GL4CubeMapFace) gl::GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
			case ECubeMapFace::NegativeY	:	return (GL4CubeMapFace) gl::GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
			case ECubeMapFace::PositiveZ	:	return (GL4CubeMapFace) gl::GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			case ECubeMapFace::NegativeZ	:	return (GL4CubeMapFace) gl::GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		}

		RETURN_ERR( "invalid cubemap face type", GL4CubeMapFace() );
	}
	
/*
=================================================
	Rendertarget
=================================================
*/
	inline GL4RenderTarget GL4Enum (ERenderTarget::type value)
	{
		if ( value == ERenderTarget::Depth )
			return (GL4RenderTarget) gl::GL_DEPTH_ATTACHMENT;

		if ( value == ERenderTarget::Stencil )
			return (GL4RenderTarget) gl::GL_STENCIL_ATTACHMENT;

		if ( value == ERenderTarget::DepthStencil )
			return (GL4RenderTarget) gl::GL_DEPTH_STENCIL_ATTACHMENT;

		if ( value >= ERenderTarget::Color0 )
			return (GL4RenderTarget) ( gl::GL_COLOR_ATTACHMENT0 + (value - ERenderTarget::Color0) );
		
		RETURN_ERR( "invalid attachment type", GL4RenderTarget(0) );
	}
	
/*
=================================================
	RenderTarget
=================================================
*/
	inline GL4RenderTarget GL4Enum (ERenderTarget::type value, EPixelFormat::type format)
	{
		const EValueTypeInfo::type fmt = EValueTypeInfo::type(format & EValueTypeInfo::_COL_MASK);

		// depth
		if ( value == ERenderTarget::Depth and (fmt == EValueTypeInfo::_DEPTH or fmt == EValueTypeInfo::_DEPTH_STENCIL) )
			return (GL4RenderTarget) gl::GL_DEPTH_ATTACHMENT;

		// stencil
		if ( value == ERenderTarget::Stencil and (fmt == EValueTypeInfo::_STENCIL or fmt == EValueTypeInfo::_DEPTH_STENCIL) )
			return (GL4RenderTarget) gl::GL_STENCIL_ATTACHMENT;
		
		// depth stencil
		if ( value == ERenderTarget::DepthStencil and fmt == EValueTypeInfo::_DEPTH_STENCIL )
			return (GL4RenderTarget) gl::GL_DEPTH_STENCIL_ATTACHMENT;

		// color
		if ( value >= ERenderTarget::Color0 and fmt >= EValueTypeInfo::_R and fmt <= EValueTypeInfo::_RGBA )
			return (GL4RenderTarget) ( gl::GL_COLOR_ATTACHMENT0 + (value - ERenderTarget::Color0) );

		RETURN_ERR( "invalid attachment type", GL4RenderTarget(0) );
	}
	
/*
=================================================
	Primitive
=================================================
*/
	inline GL4Primitive GL4Enum (EPrimitive::type value)
	{
		switch ( value )
		{
			case EPrimitive::Point			:	return (GL4Primitive) gl::GL_POINTS;
			case EPrimitive::Line			:	return (GL4Primitive) gl::GL_LINES;
			case EPrimitive::LineStrip		:	return (GL4Primitive) gl::GL_LINE_STRIP;
			case EPrimitive::Triangle		:	return (GL4Primitive) gl::GL_TRIANGLES;
			case EPrimitive::TriangleStrip	:	return (GL4Primitive) gl::GL_TRIANGLE_STRIP;
		}

		RETURN_ERR( "invalid primitive type", GL4Primitive(0) );
	}
	
/*
=================================================
	InternalPixelFormat
=================================================
*/
	inline GL4InternalPixelFormat GL4Enum (EPixelFormat::type value)
	{
		switch ( value )
		{
			// signed normalized
			case EPixelFormat::RGBA16_SNorm :			return (GL4InternalPixelFormat)	gl::GL_RGBA16_SNORM;
			case EPixelFormat::RGBA8_SNorm :			return (GL4InternalPixelFormat)	gl::GL_RGBA8_SNORM;
			case EPixelFormat::RGB16_SNorm :			return (GL4InternalPixelFormat)	gl::GL_RGB16_SNORM;
			case EPixelFormat::RGB8_SNorm :				return (GL4InternalPixelFormat)	gl::GL_RGB8_SNORM;
			case EPixelFormat::RG16_SNorm :				return (GL4InternalPixelFormat)	gl::GL_RG16_SNORM;
			case EPixelFormat::RG8_SNorm :				return (GL4InternalPixelFormat)	gl::GL_RG8_SNORM;
			case EPixelFormat::R16_SNorm :				return (GL4InternalPixelFormat)	gl::GL_R16_SNORM;
			case EPixelFormat::R8_SNorm :				return (GL4InternalPixelFormat)	gl::GL_R8_SNORM;

			// unsigned normalized
			case EPixelFormat::RGBA16_UNorm :			return (GL4InternalPixelFormat)	gl::GL_RGBA16;
			case EPixelFormat::RGBA8_UNorm :			return (GL4InternalPixelFormat)	gl::GL_RGBA8;
			case EPixelFormat::RGB16_UNorm :			return (GL4InternalPixelFormat)	gl::GL_RGB16;
			case EPixelFormat::RGB8_UNorm :				return (GL4InternalPixelFormat)	gl::GL_RGB8;
			case EPixelFormat::RG16_UNorm :				return (GL4InternalPixelFormat)	gl::GL_RG16;
			case EPixelFormat::RG8_UNorm :				return (GL4InternalPixelFormat)	gl::GL_RG8;
			case EPixelFormat::R16_UNorm :				return (GL4InternalPixelFormat)	gl::GL_R16;
			case EPixelFormat::R8_UNorm :				return (GL4InternalPixelFormat)	gl::GL_R8;
			case EPixelFormat::RGB10_A2_UNorm :			return (GL4InternalPixelFormat)	gl::GL_RGB10_A2;
			case EPixelFormat::RGBA4_UNorm :			return (GL4InternalPixelFormat)	gl::GL_RGBA4;
			case EPixelFormat::RGB5_A1_UNorm :			return (GL4InternalPixelFormat)	gl::GL_RGB5_A1;
			case EPixelFormat::RGB_5_6_5_UNorm :		return (GL4InternalPixelFormat)	gl::GL_RGB565;
			
			// sRGB
			case EPixelFormat::SRGB8 :					return (GL4InternalPixelFormat)	gl::GL_SRGB8;
			case EPixelFormat::SRGB8_A8 :				return (GL4InternalPixelFormat)	gl::GL_SRGB8_ALPHA8;

			// signed integer
			case EPixelFormat::R8I :					return (GL4InternalPixelFormat)	gl::GL_R8I;
			case EPixelFormat::RG8I :					return (GL4InternalPixelFormat)	gl::GL_RG8I;
			case EPixelFormat::RGB8I :					return (GL4InternalPixelFormat)	gl::GL_RGB8I;
			case EPixelFormat::RGBA8I :					return (GL4InternalPixelFormat)	gl::GL_RGBA8I;
			case EPixelFormat::R16I :					return (GL4InternalPixelFormat)	gl::GL_R16I;
			case EPixelFormat::RG16I :					return (GL4InternalPixelFormat)	gl::GL_RG16I;
			case EPixelFormat::RGB16I :					return (GL4InternalPixelFormat)	gl::GL_RGB16I;
			case EPixelFormat::RGBA16I :				return (GL4InternalPixelFormat)	gl::GL_RGBA16I;
			case EPixelFormat::R32I :					return (GL4InternalPixelFormat)	gl::GL_R32I;
			case EPixelFormat::RG32I :					return (GL4InternalPixelFormat)	gl::GL_RG32I;
			case EPixelFormat::RGB32I :					return (GL4InternalPixelFormat)	gl::GL_RGB32I;
			case EPixelFormat::RGBA32I :				return (GL4InternalPixelFormat)	gl::GL_RGBA32I;
			
			// unsigned integer
			case EPixelFormat::R8U :					return (GL4InternalPixelFormat)	gl::GL_R8UI;
			case EPixelFormat::RG8U :					return (GL4InternalPixelFormat)	gl::GL_RG8UI;
			case EPixelFormat::RGB8U :					return (GL4InternalPixelFormat)	gl::GL_RGB8UI;
			case EPixelFormat::RGBA8U :					return (GL4InternalPixelFormat)	gl::GL_RGBA8UI;
			case EPixelFormat::R16U :					return (GL4InternalPixelFormat)	gl::GL_R16UI;
			case EPixelFormat::RG16U :					return (GL4InternalPixelFormat)	gl::GL_RG16UI;
			case EPixelFormat::RGB16U :					return (GL4InternalPixelFormat)	gl::GL_RGB16UI;
			case EPixelFormat::RGBA16U :				return (GL4InternalPixelFormat)	gl::GL_RGBA16UI;
			case EPixelFormat::R32U :					return (GL4InternalPixelFormat)	gl::GL_R32UI;
			case EPixelFormat::RG32U :					return (GL4InternalPixelFormat)	gl::GL_RG32UI;
			case EPixelFormat::RGB32U :					return (GL4InternalPixelFormat)	gl::GL_RGB32UI;
			case EPixelFormat::RGBA32U :				return (GL4InternalPixelFormat)	gl::GL_RGBA32UI;
			case EPixelFormat::RGB10_A2U :				return (GL4InternalPixelFormat)	gl::GL_RGB10_A2UI;
			
			// float
			case EPixelFormat::R16F :					return (GL4InternalPixelFormat)	gl::GL_R16F;
			case EPixelFormat::RG16F :					return (GL4InternalPixelFormat)	gl::GL_RG16F;
			case EPixelFormat::RGB16F :					return (GL4InternalPixelFormat)	gl::GL_RGB16F;
			case EPixelFormat::RGBA16F :				return (GL4InternalPixelFormat)	gl::GL_RGBA16F;
			case EPixelFormat::R32F :					return (GL4InternalPixelFormat)	gl::GL_R32F;
			case EPixelFormat::RG32F :					return (GL4InternalPixelFormat)	gl::GL_RG32F;
			case EPixelFormat::RGB32F :					return (GL4InternalPixelFormat)	gl::GL_RGB32F;
			case EPixelFormat::RGBA32F :				return (GL4InternalPixelFormat)	gl::GL_RGBA32F;
			case EPixelFormat::RGB_11_11_10F :			return (GL4InternalPixelFormat)	gl::GL_R11F_G11F_B10F;

			// depth stencil
			case EPixelFormat::Depth16 :				return (GL4InternalPixelFormat)	gl::GL_DEPTH_COMPONENT16;
			case EPixelFormat::Depth24 :				return (GL4InternalPixelFormat)	gl::GL_DEPTH_COMPONENT24;
			case EPixelFormat::Depth32 :				return (GL4InternalPixelFormat)	gl::GL_DEPTH_COMPONENT32;
			case EPixelFormat::Depth32F :				return (GL4InternalPixelFormat)	gl::GL_DEPTH_COMPONENT32F;
			case EPixelFormat::Depth24_Stencil8 :		return (GL4InternalPixelFormat)	gl::GL_DEPTH24_STENCIL8;
			case EPixelFormat::Depth32F_Stencil8 :		return (GL4InternalPixelFormat)	gl::GL_DEPTH32F_STENCIL8;

			// compressed
			case EPixelFormat::DXT1_RGB8_UNorm :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			case EPixelFormat::DXT1_RGB8_A1_UNorm :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			case EPixelFormat::DXT3_RGBA8_UNorm :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			case EPixelFormat::DXT5_RGBA8_UNorm :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			case EPixelFormat::RGTC1_RED8_SNorm :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SIGNED_RED_RGTC1;
			case EPixelFormat::RGTC1_RED8_UNorm :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RED_RGTC1;
			case EPixelFormat::RGTC2_RG8_SNorm :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SIGNED_RG_RGTC2;
			case EPixelFormat::RGTC2_RG8_UNorm :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RG_RGTC2;
			case EPixelFormat::BPTC_RGBA8_UNorm :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_BPTC_UNORM;
			case EPixelFormat::BPTC_SRGB8_A8_UNorm :	return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
			case EPixelFormat::BPTC_RGB16F :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
			case EPixelFormat::BPTC_RGB16F_Unsigned :	return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
			case EPixelFormat::ETC2_RGB8_UNorm :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGB8_ETC2;
			case EPixelFormat::ECT2_SRGB8_UNorm :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ETC2;
			case EPixelFormat::ETC2_RGB8_A1_UNorm :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
			case EPixelFormat::ETC2_SRGB8_A1_UNorm :	return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
			case EPixelFormat::ETC2_RGBA8_UNorm :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA8_ETC2_EAC;
			case EPixelFormat::ETC2_SRGB8_A8_UNorm :	return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
			case EPixelFormat::EAC_R11_SNorm :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_R11_EAC;
			case EPixelFormat::EAC_R11_UNorm :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SIGNED_R11_EAC;
			case EPixelFormat::EAC_RG11_SNorm :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RG11_EAC;
			case EPixelFormat::EAC_RG11_UNorm :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SIGNED_RG11_EAC;
			case EPixelFormat::ASTC_RGBA_4x4 :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
			case EPixelFormat::ASTC_RGBA_5x4 :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_ASTC_5x4_KHR;
			case EPixelFormat::ASTC_RGBA_5x5 :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_ASTC_5x5_KHR;
			case EPixelFormat::ASTC_RGBA_6x5 :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_ASTC_6x5_KHR;
			case EPixelFormat::ASTC_RGBA_6x6 :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_ASTC_6x6_KHR;
			case EPixelFormat::ASTC_RGBA_8x5 :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_ASTC_8x5_KHR;
			case EPixelFormat::ASTC_RGBA_8x6 :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_ASTC_8x6_KHR;
			case EPixelFormat::ASTC_RGBA_8x8 :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_ASTC_8x8_KHR;
			case EPixelFormat::ASTC_RGBA_10x5 :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_ASTC_10x5_KHR;
			case EPixelFormat::ASTC_RGBA_10x6 :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_ASTC_10x6_KHR;
			case EPixelFormat::ASTC_RGBA_10x8 :			return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_ASTC_10x8_KHR;
			case EPixelFormat::ASTC_RGBA_10x10 :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_ASTC_10x10_KHR;
			case EPixelFormat::ASTC_RGBA_12x10 :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_ASTC_12x10_KHR;
			case EPixelFormat::ASTC_RGBA_12x12 :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_RGBA_ASTC_12x12_KHR;
			case EPixelFormat::ASTC_SRGB8_A8_4x4 :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
			case EPixelFormat::ASTC_SRGB8_A8_5x4 :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR;
			case EPixelFormat::ASTC_SRGB8_A8_5x5 :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR;
			case EPixelFormat::ASTC_SRGB8_A8_6x5 :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR;
			case EPixelFormat::ASTC_SRGB8_A8_6x6 :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR;
			case EPixelFormat::ASTC_SRGB8_A8_8x5 :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR;
			case EPixelFormat::ASTC_SRGB8_A8_8x6 :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR;
			case EPixelFormat::ASTC_SRGB8_A8_8x8 :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR;
			case EPixelFormat::ASTC_SRGB8_A8_10x5 :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR;
			case EPixelFormat::ASTC_SRGB8_A8_10x6 :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR;
			case EPixelFormat::ASTC_SRGB8_A8_10x8 :		return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR;
			case EPixelFormat::ASTC_SRGB8_A8_10x10 :	return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR;
			case EPixelFormat::ASTC_SRGB8_A8_12x10 :	return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR;
			case EPixelFormat::ASTC_SRGB8_A8_12x12 :	return (GL4InternalPixelFormat)	gl::GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR;
		}

		RETURN_ERR( "invalid internal pixel format", GL4InternalPixelFormat() );
	}
	
/*
=================================================
	InternalPixelFormat
=================================================
*/
	inline bool GL4Enum (EPixelFormat::type value, OUT GL4InternalPixelFormat &ifmt)
	{
		ifmt = GL4Enum( value );
		return ifmt != GL4InternalPixelFormat();
	}
	
/*
=================================================
	PixelFormat
=================================================
*/
	inline bool GL4Enum (EPixelFormat::type value, OUT GL4InternalPixelFormat &ifmt, OUT GL4PixelFormat &fmt, OUT GL4PixelType &type)
	{
		const uint	val_type		= ( value & EValueTypeInfo::_TYPE_MASK );
		const bool	is_compressed	= EnumEq( value, EValueTypeInfo::_COMPRESSED );
		const bool	is_integer		= ( val_type >= EValueTypeInfo::_BYTE and val_type <= EValueTypeInfo::_INT ) and
										not EnumEq( value, EValueTypeInfo::_NORM );
		const bool	is_unsigned		= EnumEq( value, EValueTypeInfo::_UNSIGNED );
		
		// internal format
		CHECK_ERR( GL4Enum( value, ifmt ) );

		if ( is_compressed )
		{
			fmt		= (GL4PixelFormat) 0;
			type	= (GL4PixelType) 0;
			return true;
		}

		// pixel type
		switch ( value & EValueTypeInfo::_TYPE_MASK )
		{
			case EValueTypeInfo::_BYTE						:	type = (GL4PixelType) (is_unsigned ? gl::GL_UNSIGNED_BYTE : gl::GL_BYTE); break;
			case EValueTypeInfo::_SHORT						:	type = (GL4PixelType) (is_unsigned ? gl::GL_UNSIGNED_SHORT : gl::GL_SHORT); break;
			case EValueTypeInfo::_INT						:	type = (GL4PixelType) (is_unsigned ? gl::GL_UNSIGNED_INT : gl::GL_INT); break;
			case EValueTypeInfo::_HALF						:	type = (GL4PixelType) gl::GL_HALF_FLOAT; break;
			case EValueTypeInfo::_FLOAT						:	type = (GL4PixelType) gl::GL_FLOAT; break;
			case EValueTypeInfo::_INT_10_10_10_2			:	type = (GL4PixelType) (is_unsigned ? gl::GL_UNSIGNED_INT_2_10_10_10_REV : gl::GL_INT_2_10_10_10_REV); break;
			case EValueTypeInfo::_FLOAT_11_11_10			:	type = (GL4PixelType) gl::GL_UNSIGNED_INT_10F_11F_11F_REV; break;
			case EValueTypeInfo::_USHORT_4_4_4_4			:	type = (GL4PixelType) gl::GL_UNSIGNED_SHORT_4_4_4_4_REV; break;
			case EValueTypeInfo::_USHORT_5_5_5_1			:	type = (GL4PixelType) gl::GL_UNSIGNED_SHORT_1_5_5_5_REV; break;
			case EValueTypeInfo::_USHORT_5_6_5				:	type = (GL4PixelType) gl::GL_UNSIGNED_SHORT_5_6_5_REV; break;
			case EValueTypeInfo::_INT24						:	type = (GL4PixelType) gl::GL_UNSIGNED_BYTE; break;
			case EValueTypeInfo::_DEPTH_24_STENCIL_8		:	type = (GL4PixelType) gl::GL_UNSIGNED_INT_24_8; break;
			case EValueTypeInfo::_FLOAT_DEPTH_32_STENCIL_8	:	type = (GL4PixelType) gl::GL_FLOAT_32_UNSIGNED_INT_24_8_REV; break;
			default											:	RETURN_ERR( "invalid pixel type" );
		}

		// pixel format
		switch ( value & EValueTypeInfo::_COL_MASK )
		{
			case EValueTypeInfo::_R				:	fmt = (GL4PixelFormat) (is_integer ? gl::GL_RED_INTEGER : gl::GL_RED);		break;
			case EValueTypeInfo::_RG			:	fmt = (GL4PixelFormat) (is_integer ? gl::GL_RG_INTEGER : gl::GL_RG);		break;
			case EValueTypeInfo::_RGB			:	fmt = (GL4PixelFormat) (is_integer ? gl::GL_RGB_INTEGER : gl::GL_RGB);		break;
			case EValueTypeInfo::_RGBA			:	fmt = (GL4PixelFormat) (is_integer ? gl::GL_RGBA_INTEGER : gl::GL_RGBA);	break;
			case EValueTypeInfo::_DEPTH			:	fmt = (GL4PixelFormat) gl::GL_DEPTH_COMPONENT;								break;
			case EValueTypeInfo::_DEPTH_STENCIL	:	fmt = (GL4PixelFormat) gl::GL_DEPTH_STENCIL;								break;
			default								:	RETURN_ERR( "invalid pixel format" );
		}
		return true;
	}
	
/*
=================================================
	VertexAttribute
=================================================
*/
	inline bool GL4Enum (EAttribute::type value, OUT GL4VertexAttribType &type, OUT uint &count, OUT bool &norm)
	{
		type	= GL4VertexAttribType();
		count	= 0;

		const bool	is_unsigned = EnumEq( value, EValueTypeInfo::_UNSIGNED );

		count = ( (value & EValueTypeInfo::_COL_MASK) >> EValueTypeInfo::_COL_OFF );
		CHECK_ERR( count >= 1 and count <= 4 );

		norm = EnumEq( value, EValueTypeInfo::_NORM );

		switch ( value & EValueTypeInfo::_TYPE_MASK )
		{
			case EValueTypeInfo::_BYTE		:	type = (GL4VertexAttribType) (is_unsigned ? gl::GL_UNSIGNED_BYTE  : gl::GL_BYTE);	break;
			case EValueTypeInfo::_SHORT		:	type = (GL4VertexAttribType) (is_unsigned ? gl::GL_UNSIGNED_SHORT : gl::GL_SHORT);	break;
			case EValueTypeInfo::_INT		:	type = (GL4VertexAttribType) (is_unsigned ? gl::GL_UNSIGNED_INT   : gl::GL_INT);	break;
			case EValueTypeInfo::_HALF		:	type = (GL4VertexAttribType) gl::GL_HALF_FLOAT;										break;
			case EValueTypeInfo::_FLOAT		:	type = (GL4VertexAttribType) gl::GL_FLOAT;											break;
			case EValueTypeInfo::_DOUBLE	:	type = (GL4VertexAttribType) gl::GL_DOUBLE;											break;
			default							:	RETURN_ERR( "invalid attrib type" );
		}
		return true;
	}
	
/*
=================================================
	AttributeToString
=================================================
*/
	inline StringCRef EAttribute::ToString (type value)
	{
		switch ( value )
		{
			case Int		:	return "int";
			case Int2		:	return "int2";
			case Int3		:	return "int3";
			case Int4		:	return "int4";
			case UInt		:	return "uint";
			case UInt2		:	return "uint2";
			case UInt3		:	return "uint3";
			case UInt4		:	return "uint4";
			case Float		:	return "float";
			case Float2		:	return "float2";
			case Float3		:	return "float3";
			case Float4		:	return "float4";
			case Double		:	return "double";
			case Double2	:	return "double2";
			case Double3	:	return "double3";
			case Double4	:	return "double4";
		}

		RETURN_ERR( "invalid attrib type", "unknown" );
	}
	
/*
=================================================
	IndexType
=================================================
*/
	inline GL4Index GL4Enum (EIndex::type value)
	{
		switch ( value )
		{
			case EIndex::UByte	:	return (GL4Index) gl::GL_UNSIGNED_BYTE;
			case EIndex::UShort	:	return (GL4Index) gl::GL_UNSIGNED_SHORT;
			case EIndex::UInt	:	return (GL4Index) gl::GL_UNSIGNED_INT;
		}

		RETURN_ERR( "invalid index type", GL4Index() );
	}

/*
=================================================
	IsSupportedForImage
=================================================
*/
	inline bool EPixelFormat::IsSupportedForImage (type value)
	{
		switch ( value )
		{
			case RGBA32F :
			case RGBA16F :
			case RG32F :
			case RG16F :
			case RGB_11_11_10F :
			case R32F :
			case R16F :
			case RGBA32U :
			case RGBA16U :
			case RGB10_A2U :
			case RGBA8U :
			case RG32U :
			case RG16U :
			case RG8U :
			case R32U :
			case R16U :
			case R8U :
			case RGBA32I :
			case RGBA16I :
			case RGBA8I :
			case RG32I :
			case RG16I:
			case RG8I :
			case R32I :
			case R16I :
			case R8I :
			case RGBA16_UNorm :
			case RGB10_A2_UNorm :
			case RGBA8_UNorm :
			case RG16_UNorm :
			case R16_UNorm :
			case R8_UNorm :
			case RGBA16_SNorm :
			case RGBA8_SNorm :
			case RG16_SNorm :
			case RG8_SNorm :
			case R16_SNorm :
			case R8_SNorm :
				return true;
		}
		return false;
	}
	
/*
=================================================
	IsSupportedForBuffer
=================================================
*/
	inline bool EPixelFormat::IsSupportedForBuffer (type value)
	{
		switch ( value )
		{
			case R8_UNorm :
			case R16_UNorm :
			case RG8_UNorm :
			case RG16_UNorm :
			case RGBA8_UNorm :
			case RGBA16_UNorm :
			case R16F :
			case R32F :
			case RG16F :
			case RG32F :
			case RGB32F :
			case RGBA16F :
			case RGBA32F :
			case R8I :
			case R16I :
			case R32I :
			case RG8I :
			case RG16I :
			case RG32I :
			case RGB32I :
			case RGBA8I :
			case RGBA16I :
			case RGBA32I :
			case R8U :
			case R16U :
			case R32U :
			case RG8U :
			case RG16U :
			case RG32U :
			case RGBA8U :
			case RGB32U :
			case RGBA16U :
			case RGBA32U :
				return true;
		}
		return false;
	}

/*
=================================================
	UniformSizeOf
=================================================
*/
	inline uint EUniform::SizeOf (type value, Bytes<usize> rowAlign)
	{
		switch ( value )
		{
			case Float :
			case UInt :
			case Int :			return 4;

			case Float2 :
			case UInt2 :
			case Int2 :			return 4*2;

			case Float3 :
			case UInt3 :
			case Int3 :			return 4*3;

			case Float4 :
			case UInt4 :
			case Int4 :			return 4*4;

			case Double :		return 8;
			case Double2 :		return 8*2;
			case Double3 :		return 8*3;
			case Double4 :		return 8*4;

			case Float2x2 :		return Align( 4*2, rowAlign ) * 2;
			case Float3x3 :		return Align( 4*3, rowAlign ) * 3;
			case Float4x4 :		return Align( 4*4, rowAlign ) * 4;
			case Double2x2 :	return Align( 8*2, rowAlign ) * 2;
			case Double3x3 :	return Align( 8*3, rowAlign ) * 3;
			case Double4x4 :	return Align( 8*4, rowAlign ) * 4;

			default :			return 4;
		}

		RETURN_ERR( "invalid uniform type", 0 );
	}
	
/*
=================================================
	UniformNumColumns
=================================================
*/
	inline uint EUniform::NumColumns (type value)
	{
		switch ( value )
		{
			case Int :
			case UInt :
			case Float :
			case Double :

			case Int2 :
			case UInt2 :
			case Float2 :
			case Double2 :

			case Int3 :
			case UInt3 :
			case Float3 :
			case Double3 :

			case Int4 :
			case UInt4 :
			case Float4 :
			case Double4 :		return 1;

			case Float2x2 :
			case Double2x2 :	return 2;

			case Float3x3 :
			case Double3x3 :	return 3;

			case Float4x4 :
			case Double4x4 :	return 4;
		}

		RETURN_ERR( "invalid uniform type", 0 );
	}
	
/*
=================================================
	UniformToString
=================================================
*/
	inline StringCRef EUniform::ToString (type value)
	{
		switch ( value )
		{
			case Int :			return "int";
			case Int2 :			return "int2";
			case Int3 :			return "int3";
			case Int4 :			return "int4";
			case UInt :			return "uint";
			case UInt2 :		return "uint2";
			case UInt3 :		return "uint3";
			case UInt4 :		return "uint4";
			case Float :		return "float";
			case Float2 :		return "float2";
			case Float3 :		return "float3";
			case Float4 :		return "float4";
			case Float2x2 :		return "float2x2";
			case Float3x3 :		return "float3x3";
			case Float4x4 :		return "float4x4";
			case Double :		return "double";
			case Double2 :		return "double2";
			case Double3 :		return "double3";
			case Double4 :		return "double4";
			case Double2x2 :	return "double2x2";
			case Double3x3 :	return "double3x3";
			case Double4x4 :	return "double4x4";

			case FloatSampler1D :				return "sampler1D";
			case FloatSampler1DShadow :			return "sampler1DShadow";
			case FloatSampler1DArray :			return "sampler1DArray";
			case FloatSampler1DArrayShadow :	return "sampler1DArrayShadow";
			case FloatSampler2D :				return "sampler2D";
			case FloatSampler2DShadow :			return "sampler2DShadow";
			case FloatSampler2DMS :				return "sampler2DMS";
			case FloatSampler2DArray :			return "sampler2DArray";
			case FloatSampler2DArrayShadow :	return "sampler2DArrayShadow";
			case FloatSampler2DMSArray :		return "sampler2DMSArray";
			case FloatSamplerCube :				return "samplerCube";
			case FloatSamplerCubeShadow :		return "samplerCubeShadow";
			case FloatSamplerCubeArray :		return "samplerCubeArray";
			case FloatSampler3D :				return "sampler3D";
			case FloatSamplerBuffer :			return "samplerBuffer";
			case IntSampler1D :					return "isampler1D";
			case IntSampler1DArray :			return "isampler1DArray";
			case IntSampler2D :					return "isampler2D";
			case IntSampler2DMS :				return "isampler2DMS";
			case IntSampler2DArray :			return "isampler2DArray";
			case IntSampler2DMSArray :			return "isampler2DMSArray";
			case IntSamplerCube :				return "isamplerCube";
			case IntSamplerCubeArray :			return "isamplerCubeArray";
			case IntSampler3D :					return "isampler3D";
			case IntSamplerBuffer :				return "isamplerBuffer";
			case UIntSampler1D :				return "uisampler1D";
			case UIntSampler1DArray :			return "uisampler1DArray";
			case UIntSampler2D :				return "uisampler2D";
			case UIntSampler2DMS :				return "uisampler2DMS";
			case UIntSampler2DArray :			return "uisampler2DArray";
			case UIntSampler2DMSArray :			return "uisampler2DMSArray";
			case UIntSamplerCube :				return "uisamplerCube";
			case UIntSamplerCubeArray :			return "uisamplerCubeArray";
			case UIntSampler3D :				return "uisampler3D";
			case UIntSamplerBuffer :			return "uisamplerBuffer";
		}

		RETURN_ERR( "invalid uniform type", "uncnown" );
	}
	
/*
=================================================
	FragOutputToString
=================================================
*/
	inline StringCRef  EFragOutput::ToString (type value)
	{
		switch ( value )
		{
			case Int4	:	return "int4";
			case UInt4	:	return "uint4";
			case Float4	:	return "float4";
		}

		RETURN_ERR( "invalid fragment output type", "unknown" );
	}
	
/*
=================================================
	BufferUsage
=================================================
*/
	inline GL4BufferUsage GL4Enum (EBufferUsage::type value)
	{
		switch ( value )
		{
			case EBufferUsage::Static  :	return (GL4BufferUsage) gl::GL_STATIC_DRAW;
			case EBufferUsage::Dynamic :	return (GL4BufferUsage) gl::GL_DYNAMIC_DRAW;
			case EBufferUsage::Stream  :	return (GL4BufferUsage) gl::GL_STREAM_DRAW;
		}
		RETURN_ERR( "invalid buffer usage type", GL4BufferUsage() );
	}
	
/*
=================================================
	PointSpriteCoordOrigin
=================================================
*/
	inline GL4PointSpriteCoordOrigin GL4Enum (EPointSpriteCoordOrigin::type value)
	{
		switch ( value )
		{
			case EPointSpriteCoordOrigin::UpperLeft :	return (GL4PointSpriteCoordOrigin) gl::GL_UPPER_LEFT;
			case EPointSpriteCoordOrigin::LowerLeft :	return (GL4PointSpriteCoordOrigin) gl::GL_LOWER_LEFT;
		}
		RETURN_ERR( "invalid point sprite coord origin type", GL4PointSpriteCoordOrigin() );
	}
	
/*
=================================================
	MemoryAccess
=================================================
*/
	inline GL4MemoryAccess GL4Enum (EMemoryAccess::type value)
	{
		switch ( value )
		{
			case EMemoryAccess::Read  :		return (GL4MemoryAccess) gl::GL_READ_ONLY;
			case EMemoryAccess::Write :		return (GL4MemoryAccess) gl::GL_WRITE_ONLY;
			case EMemoryAccess::ReadWrite :	return (GL4MemoryAccess) gl::GL_READ_WRITE;
		}
		RETURN_ERR( "invalid image access type", GL4MemoryAccess() );
	}
	
/*
=================================================
	Query
=================================================
*/
	inline GL4QueryTarget GL4Enum (EQuery::type value)
	{
		switch ( value )
		{
			case EQuery::SamplesPassed						: return (GL4QueryTarget) gl::GL_SAMPLES_PASSED;
			case EQuery::AnySamplesPassed					: return (GL4QueryTarget) gl::GL_ANY_SAMPLES_PASSED;
			case EQuery::PrimitiveGenerated					: return (GL4QueryTarget) gl::GL_PRIMITIVES_GENERATED;
			case EQuery::TransformFeedbackPrimitivesWritten	: return (GL4QueryTarget) gl::GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN;
			case EQuery::TimeElapsed						: return (GL4QueryTarget) gl::GL_TIME_ELAPSED;
				
			// GL_ARB_pipeline_statistics_query
			case EQuery::VerticesSubmitted					: return (GL4QueryTarget) gl::GL_VERTICES_SUBMITTED_ARB;
			case EQuery::PrimitivesSubmitted				: return (GL4QueryTarget) gl::GL_PRIMITIVES_SUBMITTED_ARB;
			case EQuery::VertexShaderInvocations			: return (GL4QueryTarget) gl::GL_VERTEX_SHADER_INVOCATIONS_ARB;
			case EQuery::TessControlShaderPatches			: return (GL4QueryTarget) gl::GL_TESS_CONTROL_SHADER_PATCHES_ARB;
			case EQuery::TessEvalShaderInvocations			: return (GL4QueryTarget) gl::GL_TESS_EVALUATION_SHADER_INVOCATIONS_ARB;
			case EQuery::GeometryShaderInvocations			: return (GL4QueryTarget) gl::GL_GEOMETRY_SHADER_INVOCATIONS;
			case EQuery::GeometryShaderPrimitivesEmitted	: return (GL4QueryTarget) gl::GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED_ARB;
			//case EQuery::FragmentShaderInvocations			: return (GL4QueryTarget) gl::GL_FRAGMENT_SHADER_INVOCATIONS;
			case EQuery::ComputeShaderInvocations			: return (GL4QueryTarget) gl::GL_COMPUTE_SHADER_INVOCATIONS_ARB;
			case EQuery::ClippingInputPrimitives			: return (GL4QueryTarget) gl::GL_CLIPPING_INPUT_PRIMITIVES_ARB;
			case EQuery::ClippingOutputPrimitives			: return (GL4QueryTarget) gl::GL_CLIPPING_OUTPUT_PRIMITIVES_ARB;
		}
		RETURN_ERR( "Invalid query target", GL4QueryTarget() );
	}
	
/*
=================================================
	MemoryBarrier
=================================================
*/
	inline GL4MemoryBarrier GL4Enum (EMemoryBarrier::type value)
	{
		uint	result = 0;

		result |= EnumEq( value, EMemoryBarrier::VertexAttribArray )	? gl::GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT	: 0;
		result |= EnumEq( value, EMemoryBarrier::ElementArray )			? gl::GL_ELEMENT_ARRAY_BARRIER_BIT			: 0;
		result |= EnumEq( value, EMemoryBarrier::Uniform )				? gl::GL_UNIFORM_BARRIER_BIT				: 0;
		result |= EnumEq( value, EMemoryBarrier::TextureFetch )			? gl::GL_TEXTURE_FETCH_BARRIER_BIT			: 0;
		result |= EnumEq( value, EMemoryBarrier::ShaderImageAccess )	? gl::GL_SHADER_IMAGE_ACCESS_BARRIER_BIT	: 0;
		result |= EnumEq( value, EMemoryBarrier::Command )				? gl::GL_COMMAND_BARRIER_BIT				: 0;
		result |= EnumEq( value, EMemoryBarrier::PixelBuffer )			? gl::GL_PIXEL_BUFFER_BARRIER_BIT			: 0;
		result |= EnumEq( value, EMemoryBarrier::TextureUpdate )		? gl::GL_TEXTURE_UPDATE_BARRIER_BIT			: 0;
		result |= EnumEq( value, EMemoryBarrier::BufferUpdate )			? gl::GL_BUFFER_UPDATE_BARRIER_BIT			: 0;
		result |= EnumEq( value, EMemoryBarrier::Framebuffer )			? gl::GL_FRAMEBUFFER_BARRIER_BIT			: 0;
		result |= EnumEq( value, EMemoryBarrier::TransformFeedback )	? gl::GL_TRANSFORM_FEEDBACK_BARRIER_BIT		: 0;
		result |= EnumEq( value, EMemoryBarrier::QueryBuffer )			? gl::GL_QUERY_BUFFER_BARRIER_BIT			: 0;
		result |= EnumEq( value, EMemoryBarrier::AtomicCounter )		? gl::GL_ATOMIC_COUNTER_BARRIER_BIT			: 0;
		result |= EnumEq( value, EMemoryBarrier::ClientMappedBuffer )	? gl::GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT	: 0;
		result |= EnumEq( value, EMemoryBarrier::ShaderStorage )		? gl::GL_SHADER_STORAGE_BARRIER_BIT			: 0;

		return (GL4MemoryBarrier) result;
	}
	
/*
=================================================
	BufferTarget
=================================================
*/
	inline GL4BufferTarget GL4Enum (EBufferTarget::type value)
	{
		switch ( value )
		{
			case EBufferTarget::Vertex				:	return (GL4BufferTarget) gl::GL_ARRAY_BUFFER;
			case EBufferTarget::AtomicCounter		:	return (GL4BufferTarget) gl::GL_ATOMIC_COUNTER_BUFFER;
			case EBufferTarget::CopyRead			:	return (GL4BufferTarget) gl::GL_COPY_READ_BUFFER;
			case EBufferTarget::CopyWrite			:	return (GL4BufferTarget) gl::GL_COPY_WRITE_BUFFER;
			case EBufferTarget::DispatchIndirect	:	return (GL4BufferTarget) gl::GL_DISPATCH_INDIRECT_BUFFER;
			case EBufferTarget::DrawIndirect		:	return (GL4BufferTarget) gl::GL_DRAW_INDIRECT_BUFFER;
			case EBufferTarget::Index				:	return (GL4BufferTarget) gl::GL_ELEMENT_ARRAY_BUFFER;
			case EBufferTarget::PixelPack			:	return (GL4BufferTarget) gl::GL_PIXEL_PACK_BUFFER;
			case EBufferTarget::PixelUnpack			:	return (GL4BufferTarget) gl::GL_PIXEL_UNPACK_BUFFER;
			case EBufferTarget::Query				:	return (GL4BufferTarget) gl::GL_QUERY_BUFFER;
			case EBufferTarget::ShaderStorage		:	return (GL4BufferTarget) gl::GL_SHADER_STORAGE_BUFFER;
			case EBufferTarget::Texture				:	return (GL4BufferTarget) gl::GL_TEXTURE_BUFFER;
			case EBufferTarget::TransformFeedback	:	return (GL4BufferTarget) gl::GL_TRANSFORM_FEEDBACK_BUFFER;
			case EBufferTarget::Uniform				:	return (GL4BufferTarget) gl::GL_UNIFORM_BUFFER;
		}

		RETURN_ERR( "Invalid buffer target", GL4BufferTarget() );
	}
	
/*
=================================================
	BufferBinding
=================================================
*/
	inline GL4BufferBinding GL4EnumBinding (EBufferTarget::type value)
	{
		switch ( value )
		{
			case EBufferTarget::Vertex				:	return (GL4BufferBinding) gl::GL_ARRAY_BUFFER_BINDING;
			case EBufferTarget::AtomicCounter		:	return (GL4BufferBinding) gl::GL_ATOMIC_COUNTER_BUFFER_BINDING;
			case EBufferTarget::CopyRead			:	return (GL4BufferBinding) gl::GL_COPY_READ_BUFFER_BINDING;
			case EBufferTarget::CopyWrite			:	return (GL4BufferBinding) gl::GL_COPY_WRITE_BUFFER_BINDING;
			case EBufferTarget::DispatchIndirect	:	return (GL4BufferBinding) gl::GL_DISPATCH_INDIRECT_BUFFER_BINDING;
			case EBufferTarget::DrawIndirect		:	return (GL4BufferBinding) gl::GL_DRAW_INDIRECT_BUFFER_BINDING;
			case EBufferTarget::Index				:	return (GL4BufferBinding) gl::GL_ELEMENT_ARRAY_BUFFER_BINDING;
			case EBufferTarget::PixelPack			:	return (GL4BufferBinding) gl::GL_PIXEL_PACK_BUFFER_BINDING;
			case EBufferTarget::PixelUnpack			:	return (GL4BufferBinding) gl::GL_PIXEL_UNPACK_BUFFER_BINDING;
			case EBufferTarget::Query				:	return (GL4BufferBinding) gl::GL_QUERY_BUFFER_BINDING;
			case EBufferTarget::ShaderStorage		:	return (GL4BufferBinding) gl::GL_SHADER_STORAGE_BUFFER_BINDING;
			case EBufferTarget::Texture				:	return (GL4BufferBinding) gl::GL_TEXTURE_BUFFER_BINDING;
			case EBufferTarget::TransformFeedback	:	return (GL4BufferBinding) gl::GL_TRANSFORM_FEEDBACK_BUFFER_BINDING;
			case EBufferTarget::Uniform				:	return (GL4BufferBinding) gl::GL_UNIFORM_BUFFER_BINDING;
		}

		RETURN_ERR( "Invalid buffer target", GL4BufferBinding() );
	}


}	// Graphics
}	// Engine

#endif	// GRAPHICS_API_OPENGL
