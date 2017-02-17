// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.
/*
	Texture File Format:

		TextureHeader header

		SamplerState sampler

		uint[] offsets

		byte[] data
*/

#pragma once

#include "TextureLoader.h"
#include "Engine/Graphics/Engine/GraphicsEngine.h"

namespace Engine
{
namespace Graphics
{
	
/*
=================================================
	constructor
=================================================
*/
	TextureLoader::TextureLoader (const SubSystemsRef ss) :
		IResourceLoader( ss ),
		_coloredMipmaps( false )
	{}

/*
=================================================
	Load
=================================================
*/
	bool TextureLoader::Load (OUT ResourcePtr &resource, PackFileID fileID, const RFilePtr &file)
	{
		CHECK_ERR( file );

		// load header
		TextureHeader	header;
		CHECK_ERR( file->Read( header ) );
		CHECK_ERR( header.Check() );

		// load sampler
		SamplerState	sampler_state;
		CHECK_ERR( file->Read( sampler_state ) );

		TexturePtr		tex = New<Texture>( Texture( fileID, header.target, header.dataType, SubSystems() ) );

		Array< uint >	offsets;
		BinaryArray		data;

		// load offsets
		offsets.Resize( header.numImages, false );
		CHECK_ERR( file->Read( (Buffer<uint>) offsets ) );

		// create texture
		CHECK_ERR( tex->Create( Max( uint4( header.width, header.height, header.depth, header.layers ), uint4(1) ),
								header.format, MipmapLevelsRange() ) );

		if ( _coloredMipmaps and tex->DataType() == ETextureData::DiffuseColor )
		{
			CHECK_ERR( _GenerateColoredMipmaps( tex ) );
			resource = tex;
			return true;
		}

		// load and add images
		FOR( i, offsets )
		{
			const isize	data_size = offsets[i] - (i == 0 ? 0 : offsets[i-1]);
			CHECK_ERR( data_size > 0 );

			data.Resize( data_size, false );
			CHECK_ERR( file->Read( (Buffer<ubyte>) data ) );

			ImageHeader const &	img = *(ImageHeader const *) data.ptr();	// TODO: use safe cast

			uint4	img_offset( 0, 0, 0, img.layer );

			if ( tex->TextureType() == ETexture::TexCube or
				 tex->TextureType() == ETexture::TexCubeArray )
			{
				img_offset.z = img.layer % 6;
				img_offset.w = img.layer / 6;
			}

			CHECK_ERR( tex->AddImage(	img_offset,
										MipmapLevel( img.level ),
										Max( uint3( img.width, img.height, img.depth ), uint3(1) ),
										img.format,
										data.SubArray( sizeof(img) ) ) );
		}

		// create sampler
		SamplerPtr	sampler;
		CHECK_ERR( SubSystems()->Get< GraphicsEngine >()->GetContext()->CreateSampler( sampler_state, OUT sampler ) );
		CHECK_ERR( tex->SetSampler( sampler ) );

		// generate mip levels
		if ( header.generateMipmaps )
		{
			ASSERT( not tex->HasAllMipmaps() );
			CHECK_ERR( tex->GenerateMipmaps() );
		}

		tex->_SetResourceStatus( EResourceStatus::LoadedAndLocked );

		resource = tex;
		return true;
	}
	
/*
=================================================
	_GenerateColoredMipmaps
=================================================
*/
	bool TextureLoader::_GenerateColoredMipmaps (const TexturePtr &tex)
	{
		RenderTargetPtr	rt = New<RenderTarget>( SubSystems() );

		CHECK_ERR( rt->Create() );

		const uint levels = tex->MaxLevels();

		if ( tex->NumLayers() > 1 )
		{
			for (uint i = 0; i < levels; ++i)
			{
				const float4	color( ColorUtils::RainbowRGB( float(i) / levels ), 1.0f );

				for (uint j = 0; j < tex->NumLayers(); ++j)
				{
					rt->SetViewport( tex->LevelDimension( MipmapLevel( i ) ).xy() );
					CHECK_ERR( rt->AttachLayer( tex, ERenderTarget::Color0, j, i ) );
					rt->SetClearColor( ERenderTarget::Color0, color );

					rt->Bind();
					rt->Clear();
					rt->Unbind();
				}
			}
		}
		else
		{
			for (uint i = 0; i < levels; ++i)
			{
				const float4	color( ColorUtils::RainbowRGB( float(i) / levels ), 1.0f );

				rt->SetViewport( tex->LevelDimension( MipmapLevel( i ) ).xy() );
				CHECK_ERR( rt->Attach( tex, ERenderTarget::Color0, i ) );
				rt->SetClearColor( ERenderTarget::Color0, color );

				rt->Bind();
				rt->Clear();
				rt->Unbind();
			}
		}

		tex->_SetResourceStatus( EResourceStatus::LoadedAndLocked );
		return true;
	}


}	// Graphics
}	// Engine
