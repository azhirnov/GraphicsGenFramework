// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Texture.h"

namespace Engine
{
namespace Graphics
{

	class TextureLoader;
	SHARED_POINTER( TextureLoader );



	//
	// Texture Loader
	//

	class TextureLoader : public IResourceLoader
	{
		//friend class ::EngineUtils::Converter::TextureManager;

	// types
	private:
		
		struct ImageHeader : public CompileTime::PODStruct
		{
		// variables
			// size
			uint				width;
			uint				height;
			uint				depth;

			// offset
			uint				layer;
			uint				level;

			EPixelFormat::type	format;

		// methods
			ImageHeader () :
				width( 0 ), height( 0 ), depth( 0 ),
				layer( 0 ), level( 0 ),
				format( EPixelFormat::Unknown )
			{}
		};

		struct TextureHeader : public ResourceFileHeader
		{
		// types
			enum { VERSION = 1 };

		// variables
			ETexture::type		target;
			EPixelFormat::type	format;
			ETextureData::type	dataType;
			uint				numImages;
			uint				width,
								height,
								depth,
								layers;
			bool				generateMipmaps;

		// methods
			TextureHeader () :
				ResourceFileHeader( Signature_t::Pack< 'G', 'T', 'E', 'X' >::value, VERSION ),
				target( ETexture::Unknown ), format( EPixelFormat::Unknown ),
				dataType( ETextureData::Unknown ), numImages( 0 ),
				width( 0 ), height( 0 ), depth( 0 ),
				layers( 0 ), generateMipmaps( false )
			{}

			bool Check () const
			{
				return _Check( TextureHeader() );
			}
		};


	// variables
	private:
		bool	_coloredMipmaps;


	// methods
	public:
		explicit
		TextureLoader (const SubSystemsRef ss);

		void EnableColoredMipmaps (bool enabled)	{ _coloredMipmaps = enabled; }

		
	// IResourceLoader //
	public:
		bool Load (OUT ResourcePtr &resource, PackFileID fileID, const RFilePtr &file) override;

		EResource::type GetType () const override	{ return EResource::Texture; }


	private:
		bool _GenerateColoredMipmaps (const TexturePtr &tex);

	};


}	// Graphics
}	// Engine
