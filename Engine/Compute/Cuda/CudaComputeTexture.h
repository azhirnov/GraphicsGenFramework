// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CudaEnums.h"

#if defined( COMPUTE_API_CUDA )

namespace Engine
{
namespace Compute
{

	class	CudaComputeTexture;
	typedef CudaComputeTexture	ComputeTexture;

	SHARED_POINTER( ComputeTexture );



	//
	// Compute Texture
	//

	class CudaComputeTexture : public BaseObject
	{
	// types
	private:
		typedef Bitfield< CompileTime::IntLog2< uint, GlobalConst::Compute_MaxTextureSize >::value + 1 >	LevelsBits_t;


	// variables
	private:
		BaseObjectPtr			_shared;
		cuda::CUmipmappedArray	_id;
		SamplerState			_sampler;
		uint4					_dim;
		EPixelFormat::type		_format;
		cuda::CUarray_format	_cudaFormat;
		ETexture::type			_target;
		uint					_maxMipmaps;
		LevelsBits_t			_levels;		// allocated mipmap levels


	// methods
	public:
		explicit
		CudaComputeTexture (const SubSystemsRef ss);
		~CudaComputeTexture ();

		bool Create (const uint4 &dim, ETexture::type textureType, EPixelFormat::type format, uint levels);

		bool SetImage (const uint3 &size, EPixelFormat::type format, BinaryBuffer data,
						const uint4 &offset = uint4(), uint level = 0);

		bool GetImage (OUT Buffer<ubyte> &data, const uint3 &size, const uint4 &offset, uint level = 0);

		bool AllocLevel (uint level);

		void SetSampler (const SamplerState &sampler);

		void Bind (cuda::CUtexref texref);
		void BindImage (cuda::CUtexref texref, uint level);

		bool				HasLevel (uint level)		const	{ return _levels.Get( level ); }
		uint				MaxLevels ()				const	{ return _maxMipmaps; }
		bool				HasAllMipmaps ()			const	{ return _levels.HasInterval( 0, MaxLevels() ); }

		uint4 const &		Dimension ()				const	{ return _dim; }
		uint4				LevelDimension (uint level)	const	{ return _LevelDimension( _target, _dim, level ); }
		uint				Width ()					const	{ return _dim.x; }
		uint				Height ()					const	{ return _dim.y; }
		uint				Depth ()					const	{ return _dim.z; }
		uint				NumLayers ()				const	{ return _dim.w; }

		EPixelFormat::type	PixelFormat ()				const	{ return _format; }
		ETexture::type		TextureType ()				const	{ return _target; }
		SamplerState const&	GetSampler ()				const	{ return _sampler; }

		cuda::CUmipmappedArray	Id ()					const	{ return _id; }

		static ComputeTexturePtr  New (const SubSystemsRef ss);


	private:
		bool _SetImage2D (const uint2 &offset, uint level, const uint2 &size, Bytes<usize> bpp, BinaryBuffer data);
		bool _SetImage3D (const uint3 &offset, uint level, const uint3 &size, Bytes<usize> bpp, BinaryBuffer data);
		
		bool _GetImage2D (const uint2 &offset, uint level, const uint2 &size, Bytes<usize> bpp, OUT Buffer<ubyte> &data);
		bool _GetImage3D (const uint3 &offset, uint level, const uint3 &size, Bytes<usize> bpp, OUT Buffer<ubyte> &data);

		void _BindSampler (cuda::CUtexref texref, bool normalizedCoords);

		void _Destroy ();

		static uint  _NumberOfMipmaps (ETexture::type texType, const uint4 &dim);
		static uint4 _LevelDimension (ETexture::type target, const uint4 &dim, uint level);
	};


}	// Compute
}	// Engine

#endif	// COMPUTE_API_CUDA
