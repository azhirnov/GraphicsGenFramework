// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.
/*
	Texture sizes for any types:
		- Tex1D:			( width,	0,			0,		0		)
		- Tex1DArray :		( width,	0,			0,		layers	)
		- Tex2D :			( width,	height,		0,		0		)
		- Tex2DArray :		( width,	height,		0,		layers	)
		- Tex3D :			( width,	height,		depth,	0		)
		- TexCube :			( width,	height,		6,		0		)
		- TexCubeArray :	( width,	height,		6,		layers	)
*/

#pragma once

#include "Engine/Graphics/Types/MultiSamples.h"
#include "Engine/Graphics/Types/MipmapLevel.h"
#include "Engine/Graphics/Types/TexArrLayer.h"
#include "Engine/Graphics/Types/Common.h"
#include "Sampler.h"

namespace Engine
{
namespace Graphics
{

	//
	// Texture
	//

	class Texture : public Resource
	{
		friend class TextureLoader;

	// types
	public:
		struct Utils
		{
			static uint4	ValidateDimension (ETexture::type target, const uint4 &dim);
			static uint4	LevelDimension (ETexture::type target, const uint4 &dim, uint level);
			static uint		NumberOfMipmaps (ETexture::type target, const uint4 &dim);
			
			static uint3	ConvertOffset (ETexture::type imgType, const uint4 &offset);
			static uint4	ConvertSize (ETexture::type imgType, const uint3 &size);
			static uint3	ConvertSize (ETexture::type imgType, const uint4 &size);
		};

	private:
		typedef Bitfield< CompileTime::IntLog2< uint, GlobalConst::Graphics_MaxTextureSize > + 1 >	LevelsBits_t;


	// variables
	private:
		SamplerPtr				_sampler;
		TextureID				_tex;
		uint4					_dim;			// width, height, depth, layers
		uint					_maxMipmaps;
		LevelsBits_t			_levels;		// allocated mipmap levels
		MultiSamples			_multiSamples;
		EPixelFormat::type		_format;
		ETextureData::type		_dataType;
		const ETexture::type	_target;
		MemoryBarrierObj		_pendingBarrier;
		bool					_imgSupported;


	// methods
	private:
		Texture (PackFileID fileID, ETexture::type target, ETextureData::type dataType, const SubSystemsRef ss);
		~Texture ();
		
	public:
		void Bind (uint unit);
		bool BindImage (uint unit, EMemoryAccess::type access, MipmapLevel level, TexArrLayer layer = Uninitialized);

		bool Create (const uint4 &dim, EPixelFormat::type format, const MipmapLevelsRange &levels = Uninitialized, MultiSamples samples = Uninitialized);
		bool AllocLevel (MipmapLevel level);

		bool AddImage (const uint4 &offset, MipmapLevel level, const uint3 &size, EPixelFormat::type format, BinaryBuffer data,
						BytesU xAlign = 4_b, BytesU xyAlign = 4_b);
		
		bool GetImage (OUT Buffer<ubyte> &data, MipmapLevel level, EPixelFormat::type format,
						BytesU xAlign = 4_b, BytesU xyAlign = 4_b);

		bool GetImage (OUT Buffer<ubyte> &data, const uint4 &offset, MipmapLevel level,
						const uint3 &size, EPixelFormat::type format,
						BytesU xAlign = 4_b, BytesU xyAlign = 4_b);

		bool SetSampler (const SamplerPtr &sampler);

		bool GenerateMipmaps ();
		//bool GenerateMipmaps (const uint2 &levels);	// TODO
		
		void Barrier (EMemoryBarrier::type flags = EMemoryBarrier::Unknown);

		bool				HasLevel (MipmapLevel level)		const	{ return _levels.Get( level.Get() ); }
		uint				MaxLevels ()						const	{ return _maxMipmaps; }
		bool				HasAllMipmaps ()					const	{ return _levels.HasInterval( 0, MaxLevels() ); }

		uint4 const &		Dimension ()						const	{ return _dim; }
		uint4				LevelDimension (MipmapLevel level)	const;

		uint				Width ()							const	{ return _dim.x; }
		uint				Height ()							const	{ return _dim.y; }
		uint				Depth ()							const	{ return _dim.z; }
		uint				NumLayers ()						const	{ return _dim.w; }

		EPixelFormat::type	PixelFormat ()						const	{ return _format; }
		ETexture::type		TextureType ()						const	{ return _target; }
		ETextureData::type	DataType ()							const	{ return _dataType; }

		SamplerPtr const &	GetSampler ()						const	{ return _sampler; }
		TextureID const &	GetTextureID ()						const	{ return _tex; }
		
		MultiSamples		GetMultiSamples ()					const	{ return _multiSamples; }
		bool				IsMultiSampled ()					const	{ return ETexture::IsMultisampled( TextureType() ); }

		
	// Resource //
	public:
		bool SwapData (const ResourcePtr &other) override;
		bool IsValid () const override;


	// constructors
	public:
		static TexturePtr NewTex2D			 (const SubSystemsRef ss, const uint2 &size, EPixelFormat::type format, const MipmapLevelsRange &levels = Uninitialized, MultiSamples samples = Uninitialized, ETextureData::type dataType = ETextureData::Unknown);
		static TexturePtr NewTex2DArray		 (const SubSystemsRef ss, const uint2 &size, TexArrLayer layers, EPixelFormat::type format, const MipmapLevelsRange &levels = Uninitialized, MultiSamples samples = Uninitialized, ETextureData::type dataType = ETextureData::Unknown);
		static TexturePtr NewTex3D			 (const SubSystemsRef ss, const uint3 &size, EPixelFormat::type format, const MipmapLevelsRange &levels = Uninitialized, ETextureData::type dataType = ETextureData::Unknown);
		static TexturePtr NewTexCubeMap		 (const SubSystemsRef ss, const uint2 &size, EPixelFormat::type format, const MipmapLevelsRange &levels = Uninitialized, ETextureData::type dataType = ETextureData::Unknown);
		static TexturePtr NewTexCubeMapArray (const SubSystemsRef ss, const uint2 &size, TexArrLayer layers, EPixelFormat::type format, const MipmapLevelsRange &levels = Uninitialized, ETextureData::type dataType = ETextureData::Unknown);
		static TexturePtr New				 (const SubSystemsRef ss, PackFileID fileID, ETexture::type target, ETextureData::type dataType = ETextureData::Unknown);


	// api specific methods
	private:
		bool _Create (const uint4 &dim, EPixelFormat::type format, uint2 levels, MultiSamples samples);

		bool _AddImage (const uint4 &offset, uint level, const uint3 &size, EPixelFormat::type format,
						BinaryBuffer data, BytesU xAlign, BytesU xyAlign);

		bool _AddCompressedImage (const uint4 &offset, uint level, const uint3 &size, EPixelFormat::type format,
									BinaryBuffer data, BytesU xAlign, BytesU xyAlign);

		bool _GetImage (INOUT Buffer<ubyte> &data, uint level, EPixelFormat::type format,
						BytesU xAlign, BytesU xyAlign) const;
		
		bool _GetSubImage (INOUT Buffer<ubyte> &data, const uint4 &offset, uint level, const uint3 &size,
							EPixelFormat::type format, BytesU xAlign, BytesU xyAlign) const;

		bool _AllocLevel (uint level);
		bool _GenerateMipmaps ();
		void _UpdateLevels ();


	// hidden methods
	private:
		void _Destroy ();
		void _BarrierNow (EMemoryBarrier::type flags = EMemoryBarrier::Unknown);

		static BytesU _GetImageSize (EPixelFormat::type format, ETexture::type type, const uint3 &dim, BytesU xAlign, BytesU xyAlign);

		static BytesU _GetImageSize (EPixelFormat::type format, ETexture::type type, const uint4 &dim, BytesU xAlign, BytesU xyAlign);

		static void _CheckDataTypeAndFormat (EPixelFormat::type format, ETextureData::type dataType);
	};


}	// Graphics
}	// Engine
