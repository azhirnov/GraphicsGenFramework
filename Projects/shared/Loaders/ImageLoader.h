// Copyright © 2017  Zhirnov Andrey. All rights reserved.

#pragma once

#ifdef OP_USE_LODEPNG
# pragma warning (push)
# pragma warning (disable: 4005)
#  include "External/lodepng/lodepng.h"
# pragma warning (pop)
#endif	// OP_USE_LODEPNG


namespace Shared
{
	using namespace Engine;
	using namespace Engine::Base;
	using namespace Engine::Graphics;


	//
	// Image Loader
	//

	class ImageLoader
	{
	// methods
	public:
		static bool Load (OUT TexturePtr &texture, const SubSystemsRef ss, StringCRef filename)
		{
		#ifdef OP_USE_LODEPNG

			texture = Texture::New( ss, PackFileID(), ETexture::Tex2D );

			std::vector<ubyte> data;
			uint width;
			uint height;
			CHECK_ERR( lodepng::decode( data, width, height, filename.cstr(), LCT_RGBA, 8 ) == 0 );
			
			CHECK_ERR( texture->Create( uint4( width, height, 0, 0 ), EPixelFormat::RGBA8_UNorm, MipmapLevelsRange() ), null );
			
			CHECK_ERR( texture->AddImage( uint4(), MipmapLevel(0), uint3(width, height, 0), EPixelFormat::RGBA8_UNorm,
											BinaryBuffer::FromStd( data ) ), null );

			texture->SetDebugName( filename );
			return true;
		#else

			return false;
		#endif	// OP_USE_LODEPNG
		}
	};


}	// Shared
