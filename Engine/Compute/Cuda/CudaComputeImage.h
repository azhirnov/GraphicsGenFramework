// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "CudaComputeTexture.h"

#if defined( COMPUTE_API_CUDA )

namespace Engine
{
namespace Compute
{
	
	class	CudaComputeImage;
	typedef CudaComputeImage	ComputeImage;
	
	SHARED_POINTER( ComputeImage );
	

	class	CudaComputeBuffer;
	typedef	CudaComputeBuffer	ComputeBuffer;

	SHARED_POINTER( ComputeBuffer );



	//
	// Compute Image
	//

	class CudaComputeImage : public BaseObject
	{
	// variables
	private:
		ComputeTexturePtr	_texture;
		uint4				_dim;
		uint				_level;


	// methods
	protected:
		explicit CudaComputeImage (const SubSystemsRef ss);
		explicit CudaComputeImage (const ComputeTexturePtr &tex, uint level = 0);
		
	public:
		bool Create (const uint4 &dim, ETexture::type imageType, EPixelFormat::type format);
		
		bool SetImage (const uint3 &size, BinaryBuffer data, const uint4 &offset = uint4());

		bool GetImage (OUT Buffer<ubyte> &data, const uint3 &size, const uint4 &offset);
		
		bool Copy (const ComputeBufferPtr &fromBuffer);
		bool Copy (const CudaComputeImage &fromImage);

		void Bind (cuda::CUtexref texref);

		uint						GetLevel ()			const	{ return _level; }
		ComputeTexturePtr const&	GetTexture ()		const	{ return _texture; }

		uint4 const &				Dimension ()		const	{ return _dim; }
		uint						Width ()			const	{ return _dim.x; }
		uint						Height ()			const	{ return _dim.y; }
		uint						Depth ()			const	{ return _dim.z; }
		uint						NumLayers ()		const	{ return _dim.w; }

		EPixelFormat::type			PixelFormat ()		const	{ return _texture->PixelFormat(); }
		ETexture::type				ImageType ()		const	{ return _texture->TextureType(); }
		SamplerState const&			GetSampler ()		const	{ return _texture->GetSampler(); }
		
		bool						IsCreated ()		const	{ return _texture.IsNotNull(); }
		cuda::CUarray				Id ()				const;
		Bytes<ulong>						ImageSize ()		const;

		static ComputeImagePtr  New (const SubSystemsRef ss);
		static ComputeImagePtr  New (const ComputeTexturePtr &tex, uint level = 0);


	private:
		void _Destroy ();
	};


}	// Compute
}	// Engine

#endif	// COMPUTE_API_CUDA
