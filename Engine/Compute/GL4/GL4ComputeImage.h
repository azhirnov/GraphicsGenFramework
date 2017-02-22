// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "GL4Enums.h"

#if defined( COMPUTE_API_OPENGL )

namespace Engine
{
namespace Compute
{
	
	class	GL4ComputeImage;
	typedef GL4ComputeImage		ComputeImage;
	
	SHARED_POINTER( ComputeImage );
	

	class	GL4ComputeBuffer;
	typedef	GL4ComputeBuffer	ComputeBuffer;

	SHARED_POINTER( ComputeBuffer );



	//
	// Compute Image
	//
	
	class GL4ComputeImage : public BaseObject
	{
	// variables
	private:
		TexturePtr			_shared;
		EMemoryAccess::type	_flags;
		MipmapLevel			_level;
		TexArrLayer			_layer;


	// methods
	public:
		explicit
		GL4ComputeImage (const SubSystemsRef ss);
		~GL4ComputeImage ();

		bool Create (const uint4 &dim, ETexture::type imageType, EPixelFormat::type format,
					 EMemoryAccess::type flags = EMemoryAccess::ReadWrite,
					 BinaryCBuffer data = Uninitialized,
					 BytesU xAlign = 4_b,
					 BytesU xyAlign = 4_b);
		
		bool Create (const TexturePtr &texture, EMemoryAccess::type flags, MipmapLevel level = MipmapLevel(), TexArrLayer layer = Uninitialized);

		bool SetImage (BinaryCBuffer data, const uint3 &size, const uint4 &offset = Uninitialized,
						BytesU xAlign = 4_b, BytesU xyAlign = 4_b);

		bool GetImage (OUT BinaryBuffer data, const uint3 &size, const uint4 &offset,
						BytesU xAlign = 4_b, BytesU xyAlign = 4_b);
		
		bool Copy (const ComputeBufferPtr &src, BytesU srcOffset, const uint4 &dstOffset, const uint4 &size);
		bool Copy (const ComputeImagePtr &src, const uint4 &srcOffset, const uint4 &dstOffset, const uint4 &size);
		
		bool CopyTo (const ComputeBufferPtr &dst, const uint4 &srcOffset, BytesU dstOffset, const uint4 &size);

		void Bind (uint unit, EMemoryAccess::type access) const;
		
		bool					IsShared ()			const	{ return _shared.IsNotNull(); }
		TexturePtr const&		GetSharedObject ()	const	{ return _shared; }
		TextureID				Id ()				const	{ return _shared ? _shared->GetTextureID() : TextureID(); }

		uint4 const &			Dimension ()		const;
		uint					Width ()			const	{ return Dimension().x; }
		uint					Height ()			const	{ return Dimension().y; }
		uint					Depth ()			const	{ return Dimension().z; }
		uint					NumLayers ()		const	{ return Dimension().w; }

		EPixelFormat::type		PixelFormat ()		const;
		ETexture::type			ImageType ()		const;
		EMemoryAccess::type		MemoryAccess ()		const	{ return _flags; }
		
		bool					IsCreated ()		const	{ return Id().IsValid(); }

		// TODO: Clear

		static ComputeImagePtr  New (const TexturePtr &texture, EMemoryAccess::type flags = EMemoryAccess::ReadWrite, MipmapLevel level = MipmapLevel());


	private:
		void _Destroy ();

		static bool _IsSupportedForImage (EPixelFormat::type fmt);
	};

	
/*
=================================================
	Dimension
=================================================
*/
	inline uint4 const &  GL4ComputeImage::Dimension () const
	{
		static const uint4	tmp = uint4(0);
		return _shared ? _shared->Dimension() : tmp;
	}
	
/*
=================================================
	PixelFormat
=================================================
*/
	inline EPixelFormat::type  GL4ComputeImage::PixelFormat () const
	{
		return _shared ? _shared->PixelFormat() : EPixelFormat::Unknown;
	}
	
/*
=================================================
	ImageType
=================================================
*/
	inline ETexture::type  GL4ComputeImage::ImageType () const
	{
		return _shared ? _shared->TextureType() : ETexture::Unknown;
	}

}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENGL
