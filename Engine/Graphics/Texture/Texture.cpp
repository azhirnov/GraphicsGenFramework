// Copyright � 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Texture.h"
#include "Engine/Graphics/Engine/GraphicsEngine.h"

namespace Engine
{
namespace Graphics
{
	
/*
=================================================
	NewTex*
=================================================
*/
	TexturePtr Texture::NewTex2D (const SubSystemsRef ss, const uint2 &size, EPixelFormat::type format, const MipmapLevelsRange &levels, MultiSamples samples, ETextureData::type dataType)
	{
		ETexture::type	target	= samples > MultiSamples(1) ? ETexture::Tex2DMS : ETexture::Tex2D;
		TexturePtr		p		= New<Texture>( ss, target, dataType );

		CHECK_ERR( p->Create( uint4( size, 1u, 1u ), format, levels, samples ) );
		return p;
	}

	TexturePtr Texture::NewTex2DArray (const SubSystemsRef ss, const uint2 &size, TexArrLayer layers, EPixelFormat::type format, const MipmapLevelsRange &levels, MultiSamples samples, ETextureData::type dataType)
	{
		ETexture::type	target	= samples > MultiSamples(1) ? ETexture::Tex2DMSArray : ETexture::Tex2DArray;
		TexturePtr		p		= New<Texture>( ss, target, dataType );

		CHECK_ERR( p->Create( uint4( size, 1u, layers.Get() ), format, levels, samples ) );
		return p;
	}

	TexturePtr Texture::NewTex3D (const SubSystemsRef ss, const uint3 &size, EPixelFormat::type format, const MipmapLevelsRange &levels, ETextureData::type dataType)
	{
		TexturePtr	p = New<Texture>( ss, ETexture::Tex3D, dataType );

		CHECK_ERR( p->Create( uint4( size, 1u ), format, levels, Uninitialized ) );
		return p;
	}

	TexturePtr Texture::NewTexCubeMap (const SubSystemsRef ss, const uint2 &size, EPixelFormat::type format, const MipmapLevelsRange &levels, ETextureData::type dataType)
	{
		TexturePtr	p = New<Texture>( ss, ETexture::TexCube, dataType );

		CHECK_ERR( p->Create( uint4( size, 6u, 1u ), format, levels, Uninitialized ) );
		return p;
	}

	TexturePtr Texture::NewTexCubeMapArray (const SubSystemsRef ss, const uint2 &size, TexArrLayer layers, EPixelFormat::type format, const MipmapLevelsRange &levels, ETextureData::type dataType)
	{
		TexturePtr	p = New<Texture>( ss, ETexture::TexCubeArray, dataType );

		CHECK_ERR( p->Create( uint4( size, 6u, layers.Get() ), format, levels, Uninitialized ) );
		return p;
	}
	
/*
=================================================
	constructor
=================================================
*/
	Texture::Texture (PackFileID fileID, ETexture::type target, ETextureData::type dataType, const SubSystemsRef ss) :
		Resource( fileID, EResource::Texture, ss ),
		_maxMipmaps(0),
		_format( EPixelFormat::Unknown ),
		_target( target ),
		_dataType( dataType ),
		_imgSupported( false )
	{
	}
		
	Texture::Texture (const SubSystemsRef ss, ETexture::type target) :
		Texture( PackFileID(), target, Uninitialized, ss )
	{}

	Texture::Texture (const SubSystemsRef ss, ETexture::type target, ETextureData::type dataType) :
		Texture( PackFileID(), target, dataType, ss )
	{}
	
/*
=================================================
	destructor
=================================================
*/
	Texture::~Texture ()
	{
		_Destroy();
	}

/*
=================================================
	_Destroy
=================================================
*/
	void Texture::_Destroy ()
	{
		Resource::_Destroy();

		if ( SubSystems()->Get< GraphicsEngine >() )
		{
			// unbind and delete
			SubSystems()->Get< GraphicsEngine >()->GetContext()->DeleteTexture( _tex );
		}

		_pendingBarrier.Reset();

		_sampler		= null;
		_tex			= TextureID();
		_dim			= uint4();
		_maxMipmaps		= 0;
		_levels			= 0;
		_multiSamples	= MultiSamples();
		_format			= EPixelFormat::Unknown;
		_imgSupported	= false;
	}
	
/*
=================================================
	SwapData
=================================================
*/
	bool Texture::SwapData (const ResourcePtr &other)
	{
		CHECK_ERR( Resource::SwapData( other ) );

		Texture *	tex = other.ToPtr< Texture >();

		CHECK_ERR( this->TextureType() == tex->TextureType() );

		SwapMembers( this, tex, &Texture::_sampler );
		SwapMembers( this, tex, &Texture::_tex );
		SwapMembers( this, tex, &Texture::_dim );
		SwapMembers( this, tex, &Texture::_maxMipmaps );
		SwapMembers( this, tex, &Texture::_levels );
		SwapMembers( this, tex, &Texture::_multiSamples );
		SwapMembers( this, tex, &Texture::_format );
		SwapMembers( this, tex, &Texture::_imgSupported );

		// TODO: ???
		this->_pendingBarrier.Reset();
		tex->_pendingBarrier.Reset();
		return true;
	}
	
/*
=================================================
	IsValid
=================================================
*/
	bool Texture::IsValid () const
	{
		return _IsResourceLoaded() and _tex.IsValid();
	}
	
/*
=================================================
	Bind
=================================================
*/
	void Texture::Bind (uint unit)
	{
		CHECK( _tex.IsValid() );

		Barrier( EMemoryBarrier::TextureFetch );
		
		Ptr< StateManager >	sm = SubSystems()->Get< GraphicsEngine >()->GetStateManager();

		sm->BindTexture( _tex, unit );

		if ( GetSampler() )
		{
			sm->BindSampler( GetSampler()->GetSamplerID(), unit );
		}
	}
	
/*
=================================================
	BindImage
=================================================
*/
	bool Texture::BindImage (uint unit, EMemoryAccess::type access, MipmapLevel level, TexArrLayer layer)
	{
		CHECK( _tex.IsValid() );
		CHECK_ERR( _imgSupported );
		CHECK_ERR( not (EnumEq( access, EMemoryAccess::Write ) and _IsResourceLocked()) );

		Ptr< StateManager >		sm = SubSystems()->Get< GraphicsEngine >()->GetStateManager();

		Barrier( EMemoryBarrier::ShaderImageAccess );

		sm->BindImage( _tex, unit, level, layer, access, PixelFormat() );
		
		// add barrier if image may be changed
		if ( EnumEq( access, EMemoryAccess::Write ) )
		{
			sm->CreateBarrier( OUT _pendingBarrier );
		}
		return true;
	}

/*
=================================================
	Create
=================================================
*/
	bool Texture::Create (const uint4 &dim, EPixelFormat::type format, const MipmapLevelsRange &levels, MultiSamples samples)
	{
		CHECK_ERR( not _IsResourceLocked() );
		
		_Destroy();

		_CheckDataTypeAndFormat( format, _dataType );

		CHECK_ERR( _Create( dim, format, levels.Get(), samples ) );

		_pendingBarrier.Reset();

		_imgSupported = EPixelFormat::IsSupportedForImage( format );
		
		_EditMemStat().Set( BytesU(), _GetImageSize( PixelFormat(), TextureType(), Dimension(), 1_b, 1_b ) );	// TODO: add mipmaps size

		_SetResourceStatus( EResourceStatus::Loaded );
		return true;
	}

/*
=================================================
	AddImage
=================================================
*/
	bool Texture::AddImage (const uint4 &offset, MipmapLevel level, const uint3 &size, EPixelFormat::type format,
							BinaryCBuffer data, BytesU xAlign, BytesU xyAlign)
	{
		CHECK_ERR( IsValid() );
		CHECK_ERR( not _IsResourceLocked() );
		CHECK_ERR( _format == format );	// TODO: is it needed ?

		_BarrierNow( EMemoryBarrier::TextureUpdate );
		
		_EditMemStat().Add( BytesU(), data.Size() );

		if ( EPixelFormat::IsCompressed( format ) )
			return _AddCompressedImage( offset, level.Get(), size, format, data, xAlign, xyAlign );
		else
			return _AddImage( offset, level.Get(), size, format, data, xAlign, xyAlign );
	}

/*
=================================================
	AllocLevel
=================================================
*/
	bool Texture::AllocLevel (MipmapLevel level)
	{
		CHECK_ERR( IsValid() );
		CHECK_ERR( not _IsResourceLocked() );
		CHECK_ERR( not HasLevel( level ) );
		
		_BarrierNow( EMemoryBarrier::TextureUpdate );
		
		_EditMemStat().Add( BytesU(), _GetImageSize( PixelFormat(), TextureType(), LevelDimension(level), 1_b, 1_b ) );

		return _AllocLevel( level.Get() );
	}
	
/*
=================================================
	GetImage
=================================================
*/
	bool Texture::GetImage (OUT BinaryBuffer data, MipmapLevel level, EPixelFormat::type format, BytesU xAlign, BytesU xyAlign)
	{
		CHECK_ERR( IsValid() );
		CHECK_ERR( not HasLevel( level ) );
		
		_BarrierNow( EMemoryBarrier::TextureUpdate );

		return _GetImage( data, level.Get(), format, xAlign, xyAlign );
	}
	
/*
=================================================
	GetImage
=================================================
*/
	bool Texture::GetImage (OUT BinaryBuffer data, const uint4 &offset, MipmapLevel level,
							const uint3 &size, EPixelFormat::type format,
							BytesU xAlign, BytesU xyAlign)
	{
		CHECK_ERR( IsValid() );
		CHECK_ERR( not HasLevel( level ) );
		
		_BarrierNow( EMemoryBarrier::TextureUpdate );

		return _GetSubImage( data, offset, level.Get(), size, format, xAlign, xyAlign );
	}

/*
=================================================
	GenerateMipmaps
=================================================
*/
	bool Texture::GenerateMipmaps ()
	{
		CHECK_ERR( IsValid() );
		CHECK_ERR( not _IsResourceLocked() );
		
		_BarrierNow( EMemoryBarrier::TextureUpdate );
		
		_EditMemStat().Set( BytesU(), BytesU( _GetImageSize( PixelFormat(), TextureType(), Dimension(), 1_b, 1_b ) * 4 / 3 ) );

		return _GenerateMipmaps();
	}
	
/*
=================================================
	SetSampler
=================================================
*/
	bool Texture::SetSampler (const SamplerPtr &sampler)
	{
		CHECK_ERR( _IsResourceCreatedOrLoaded() );	// if texture not created on destroy sampler will be reset to null
		CHECK_ERR( not _IsResourceLocked() );

		_sampler = sampler;
		return true;
	}
	
/*
=================================================
	Barrier
=================================================
*/
	void Texture::Barrier (EMemoryBarrier::type flags)
	{
		SubSystems()->Get<GraphicsEngine>()->GetStateManager()->
			MemoryBarrierBeforeDraw( INOUT _pendingBarrier, flags );
	}
	
/*
=================================================
	_BarrierNow
=================================================
*/
	void Texture::_BarrierNow (EMemoryBarrier::type flags)
	{
		SubSystems()->Get<GraphicsEngine>()->GetStateManager()->
			MemoryBarrier( INOUT _pendingBarrier, flags );
	}

/*
=================================================
	LevelDimension
=================================================
*/
	uint4 Texture::Utils::LevelDimension (ETexture::type target, const uint4 &dim, uint level)
	{
		ASSERT( level == 0 or (target == ETexture::Tex2DMS or target == ETexture::Tex2DMSArray) );

		switch ( target )
		{
			case ETexture::Buffer :
			case ETexture::Tex1D :			return uint4( Max( dim.x >> level, 1u ), 0, 0, 0 );

			case ETexture::Tex2D :			return uint4( Max( dim.xy() >> level, uint2(1) ), 0, 0 );

			case ETexture::TexCube :		return uint4( Max( dim.xy() >> level, uint2(1) ), 6, 0 );

			case ETexture::Tex2DArray :		return uint4( Max( dim.xy() >> level, uint2(1) ), 0, dim.w );

			case ETexture::Tex3D :			return uint4( Max( dim.xyz() >> level, uint3(1) ), 0 );

			case ETexture::TexCubeArray :	return uint4( Max( dim.xy() >> level, uint2(1) ), 6, dim.w );

			//case ETexture::Tex2DMSArray :
			//case ETexture::Tex2DMS :	// not supported for multisampled texture
		}

		RETURN_ERR( "invalid texture type" );
	}
		
/*
=================================================
	NumberOfMipmaps
=================================================
*/	
	uint Texture::Utils::NumberOfMipmaps (ETexture::type target, const uint4 &dim)
	{
		switch ( target )
		{
			case ETexture::Buffer :
			case ETexture::Tex2DMS :
			case ETexture::Tex2DMSArray :	return 1;

			case ETexture::Tex1D :
			case ETexture::Tex2D :
			case ETexture::Tex3D :			return ImageUtils::GetNumberOfMipmaps( dim.Max() ) + 1;

			case ETexture::TexCube :
			case ETexture::TexCubeArray :
			case ETexture::Tex2DArray :		return ImageUtils::GetNumberOfMipmaps( dim.xy().Max() ) + 1;
		}

		RETURN_ERR( "invalid texture type", uint(1) );
	}
	
/*
=================================================
	_CheckDataTypeAndFormat
=================================================
*/
	void Texture::_CheckDataTypeAndFormat (EPixelFormat::type format, ETextureData::type dataType)
	{
		if ( dataType == ETextureData::Unknown )
			return;

		const bool	is_depth	= EPixelFormat::Format::HasDepth( format );
		const bool	is_stencil	= EPixelFormat::Format::HasStencil( format );
		const bool	is_color	= EPixelFormat::Format::IsColor( format );

		CHECK( (is_depth or is_stencil) == ETextureData::IsDepthStencil( dataType ) );
		CHECK( is_color == ETextureData::IsColor( dataType ) );
	}
	
/*
=================================================
	LevelDimension
=================================================
*/
	uint4 Texture::LevelDimension (MipmapLevel level) const
	{
		return Utils::LevelDimension( TextureType(), Dimension(), level.Get() );
	}
	
/*
=================================================
	_GetImageSize
=================================================
*/
	BytesU Texture::_GetImageSize (EPixelFormat::type format, ETexture::type type, const uint3 &size,
										 BytesU xAlign, BytesU xyAlign)
	{
		CHECK_ERR( not EPixelFormat::IsCompressed( format ), BytesU() );

		BytesU			bpp = BytesU( EPixelFormat::BitPerPixel( format ) );
		uint3 const		dim	= Max( size, uint3(1) );

		return ImageUtils::AlignedDataSize( dim, bpp, xAlign, xyAlign );
	}

	BytesU Texture::_GetImageSize (EPixelFormat::type format, ETexture::type type, const uint4 &dim, BytesU xAlign, BytesU xyAlign)
	{
		return _GetImageSize( format, type, Utils::ConvertSize( type, dim ), xAlign, xyAlign );
	}

/*
=================================================
	ConvertOffset
=================================================
*/
	uint3 Texture::Utils::ConvertOffset (ETexture::type imgType, const uint4 &offset)
	{
		switch ( imgType )
		{
			case ETexture::Tex2D		:	return offset.xyo();
			case ETexture::Tex3D		:	return offset.xyz();
			case ETexture::Buffer		:	return offset.xoo();
			case ETexture::Tex1D		:	return offset.xoo();
			case ETexture::Tex1DArray	:	return offset.xwo();
			case ETexture::Tex2DArray	:	return offset.xyw();
		}
		WARNING( "unsupported image type!" );
		return uint3();
	}
	
/*
=================================================
	ConvertSize
=================================================
*/
	uint4 Texture::Utils::ConvertSize (ETexture::type imgType, const uint3 &size)
	{
		uint4	res;

		switch ( imgType )
		{
			case ETexture::Tex2D		:	res = size.xyoo();	break;
			case ETexture::Tex3D		:	res = size.xyzo();	break;
			case ETexture::Buffer		:	res = size.xooo();	break;
			case ETexture::Tex1D		:	res = size.xooo();	break;
			case ETexture::Tex1DArray	:	res = size.xooy();	break;
			case ETexture::Tex2DArray	:	res = size.xyoz();	break;
			default						:	res = size.xyzo();	WARNING( "unsupported image type!" );
		}
		
		return res;
	}
	
/*
=================================================
	ConvertSize
=================================================
*/
	uint3 Texture::Utils::ConvertSize (ETexture::type imgType, const uint4 &size)
	{
		uint3	res;

		switch( imgType )
		{
			case ETexture::Tex2D		:	res = size.xyo();	break;
			case ETexture::Tex3D		:	res = size.xyz();	break;
			case ETexture::Buffer		:	res = size.xoo();	break;
			case ETexture::Tex1D		:	res = size.xoo();	break;
			case ETexture::Tex1DArray	:	res = size.xyo();	break;
			case ETexture::Tex2DArray	:	res = size.xyz();	break;
			default						:	res = size.xyz();	WARNING( "unsupported image type!" );
		}

		return res;
	}
	
/*
=================================================
	ValidateDimension
=================================================
*/
	uint4 Texture::Utils::ValidateDimension (ETexture::type target, const uint4 &dim)
	{
		switch ( target )
		{
			case ETexture::Buffer :
			case ETexture::Tex1D :			return uint4( dim.x, 0, 0, 0 );
				
			case ETexture::Tex2DMSArray :
			case ETexture::Tex2DMS :
			case ETexture::Tex2D :			return uint4( dim.xy(), 0, 0 );

			case ETexture::TexCube :		return uint4( dim.xy(), 6, 0 );

			case ETexture::Tex2DArray :		return uint4( dim.xy(), 0, dim.w );

			case ETexture::Tex3D :			return uint4( dim.xyz(), 0 );

			case ETexture::TexCubeArray :	return uint4( dim.xy(), 6, dim.w );
		}

		RETURN_ERR( "invalid texture type" );
	}


}	// Graphics
}	// Engine
