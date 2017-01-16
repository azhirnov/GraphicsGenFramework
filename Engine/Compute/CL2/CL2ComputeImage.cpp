// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#include "CL2ComputeImage.h"

#if defined( COMPUTE_API_OPENCL )

#include "Engine/Compute/Engine/ComputeEngine.h"
#include "CL2ComputeUtils.h"
#include "CL2ComputeBuffer.h"

namespace Engine
{
namespace Compute
{
	
/*
=================================================
	constructor
=================================================
*/
	CL2ComputeImage::CL2ComputeImage (const SubSystemsRef ss) :
		BaseObject( ss ),
		_shared( null ), _id( null ), _format( EPixelFormat::type(-1) ),
		_target( ETexture::type(-1) ), _flags( EMemoryAccess::type(0) )
	{
	}
	
/*
=================================================
	destructor
=================================================
*/
	CL2ComputeImage::~CL2ComputeImage ()
	{
		_Destroy();
	}

/*
=================================================
	Create
=================================================
*/
	bool CL2ComputeImage::Create (const uint4 &dim, ETexture::type imageType, EPixelFormat::type format,
								  EMemoryAccess::type flags, BinaryBuffer data,
								  Bytes<usize> xAlign, Bytes<usize> xyAlign)
	{
		using namespace cl;

		_Destroy();

		CL2ChannelOrder		order;
		CL2ChannelDataType	data_type;
		Bytes<usize>		bpp;

		uint4 const			size = Texture::Utils::ValidateDimension( imageType, dim );

		CHECK_ERR( CL2Enum( format, order, data_type, bpp ) );

		cl_image_format		img_format	= {};
		cl_image_desc		img_desc	= {};

		img_format.image_channel_order		= (cl_channel_order) order;
		img_format.image_channel_data_type	= (cl_channel_type) data_type;

		img_desc.image_type			= CL2Enum( imageType );
		img_desc.image_width		= size.x;
		img_desc.image_height		= size.y;
		img_desc.image_depth		= size.z;
		img_desc.image_array_size	= size.w;

		const bool	has_data		= not data.Empty();

		if ( has_data )
		{
			img_desc.image_row_pitch	= AlignedImageRowSize( size.x, bpp, xAlign );
			img_desc.image_slice_pitch	= AlignedImageSliceSize( size.xy(), bpp, xAlign, xyAlign );
		}

		cl_int	cl_err = 0;

		CL_CHECK( ((_id = clCreateImage(
							SubSystems()->Get< ComputeEngine >()->GetContext(),
							CL2Enum( flags ),
							&img_format,
							&img_desc,
							has_data ? (void *)data.ptr() : null,
							&cl_err )), cl_err) );

		_target	= imageType;
		_flags	= flags;
		_dim	= size;
		_format	= format;
		return true;
	}
		
# if defined( GRAPHICS_API_OPENGL )
/*
=================================================
	Create
=================================================
*/
	bool CL2ComputeImage::Create (const TexturePtr &texture, EMemoryAccess::type flags, MipmapLevel level)
	{
		CHECK_ERR( texture.IsNotNull() );
		CHECK_ERR( texture->HasLevel( level ) );
		CHECK_ERR( _CreateFromGLTex( texture->GetTextureID().Target(), texture->GetTextureID().Id(), flags, level.Get() ) );

		_target	= texture->TextureType();
		_shared = texture;
		_flags	= flags;
		_dim	= texture->Dimension();
		_format	= texture->PixelFormat();

		return true;
	}
	
/*
=================================================
	Create
=================================================
*/
	bool CL2ComputeImage::Create (const TexturePtr &texture, EMemoryAccess::type flags, ECubeMapFace::type face, MipmapLevel level)
	{
		CHECK_ERR( texture.IsNotNull() );
		CHECK_ERR( _CreateFromGLTex( GL4Enum( face ), texture->GetTextureID().Id(), flags, level.Get() ) );
		
		_target	= texture->TextureType();
		_shared = texture;
		_flags	= flags;
		_dim	= texture->Dimension();
		_format	= texture->PixelFormat();

		return true;
	}
	
/*
=================================================
	_CreateFromGLTex
=================================================
*/
	bool CL2ComputeImage::_CreateFromGLTex (gl::GLenum target, gl::GLuint textureId, EMemoryAccess::type flags, uint level)
	{
		using namespace cl;

		_Destroy();

		cl_int	cl_err = 0;

		CL_CHECK( ((_id = clCreateFromGLTexture(
							SubSystems()->Get< ComputeEngine >()->GetContext(),
							CL2Enum( flags ),
							target,
							level,
							textureId,
							&cl_err )), cl_err) );
		return true;
	}

# endif	// GRAPHICS_API_OPENGL

/*
=================================================
	_Destroy
=================================================
*/
	void CL2ComputeImage::_Destroy ()
	{
		using namespace cl;

		_shared = null;
		_target	= ETexture::type(-1);
		_format	= EPixelFormat::type(-1);
		_flags	= EMemoryAccess::type(0);
		_dim	= uint4();

		if ( _id == null )
			return;

		CL_CALL( clReleaseMemObject( _id ) );
		_id = null;
	}
		
/*
=================================================
	SetImage
=================================================
*/
	bool CL2ComputeImage::SetImage (BinaryBuffer data, const uint3 &size, const uint4 &offset,
									Bytes<usize> xAlign, Bytes<usize> xyAlign)
	{
		using namespace cl;
		
		CHECK_ERR( IsCreated() );
		CHECK_ERR( All( offset + Texture::Utils::ConvertSize( ImageType(), size ) <= _dim ) );

		uint3			img_offset	= Texture::Utils::ConvertOffset( ImageType(), offset );
		uint3			img_size	= Max( size, uint3(1) );
		
		Bytes<usize>	bpp			= EPixelFormat::BitPerPixel( _format ).ToBytes();
		usize			row_pitch	= AlignedImageRowSize( img_size.x, bpp, xAlign );
		usize			slice_pitch	= 0;
		
		// for 3D
		if ( Depth() > 1 or NumLayers() > 1 )
			slice_pitch = AlignedImageSliceSize( img_size.xy(), bpp, xAlign, xyAlign );

		cl_command_queue	cmd_queue = SubSystems()->Get< ComputeEngine >()->GetCommandQueue();

		CL2ComputeUtils::AcquireObjects( cmd_queue, this );

		CL_CHECK( clEnqueueWriteImage(
					cmd_queue,
					_id,
					true,
					img_offset.To< usize3 >().ptr(),
					img_size.To< usize3 >().ptr(),
					row_pitch, slice_pitch,
					data.ptr(),
					0, null,
					null ) );

		CL2ComputeUtils::ReleaseObjects( cmd_queue, this );
		return true;
	}
	
/*
=================================================
	GetImage
=================================================
*/
	bool CL2ComputeImage::GetImage (OUT Buffer<ubyte> data, const uint3 &size, const uint4 &offset,
									Bytes<usize> xAlign, Bytes<usize> xyAlign)
	{
		using namespace cl;
		
		CHECK_ERR( IsCreated() );
		CHECK_ERR( All( offset + Texture::Utils::ConvertSize( ImageType(), size ) <= _dim ) );

		uint3			img_offset	= Texture::Utils::ConvertOffset( ImageType(), offset );
		uint3			img_size	= Max( size, uint3(1) );

		Bytes<usize>	bpp			= Bytes<usize>( EPixelFormat::BitPerPixel( _format ) );
		usize			row_pitch	= AlignedImageRowSize( img_size.x, bpp, xAlign );
		usize			slice_pitch	= AlignedImageSliceSize( img_size.xy(), bpp, xAlign, xyAlign );

		cl_command_queue	cmd_queue = SubSystems()->Get< ComputeEngine >()->GetCommandQueue();

		CL2ComputeUtils::AcquireObjects( cmd_queue, this );

		CL_CHECK( clEnqueueReadImage(
					cmd_queue,
					_id,
					true,
					img_offset.To< usize3 >().ptr(),
					img_size.To< usize3 >().ptr(),
					row_pitch, slice_pitch,
					data.ptr(),
					0, null,
					null ) );

		CL2ComputeUtils::ReleaseObjects( cmd_queue, this );
		return true;
	}
		
/*
=================================================
	Copy
=================================================
*/
	bool CL2ComputeImage::Copy (const ComputeBufferPtr &src, Bytes<usize> srcOffset, const uint4 &dstOffset, const uint4 &size)
	{
		using namespace cl;
		
		CHECK_ERR( IsCreated() );
		CHECK_ERR( src.IsNotNull() and src->IsCreated() );
		CHECK_ERR( All( dstOffset + size <= this->Dimension() ) );
		
		uint3			dst_offset	= Texture::Utils::ConvertOffset( this->ImageType(), dstOffset );
		uint3			dst_size	= Texture::Utils::ConvertSize( this->ImageType(), size );
		Bytes<usize>	bpp			= Bytes<usize>( EPixelFormat::BitPerPixel( PixelFormat() ) );
		usize			data_size	= dst_size.Volume() * bpp;

		CHECK_ERR( data_size <= src->Size() );
		
		cl_command_queue	cmd_queue = SubSystems()->Get< ComputeEngine >()->GetCommandQueue();

		CL2ComputeUtils::AcquireObjects( cmd_queue, this, src );

		CL_CHECK( clEnqueueCopyBufferToImage(
					cmd_queue,
					src->Id(),
					this->Id(),
					srcOffset,
					dst_offset.To< usize3 >().ptr(),
					dst_size.To< usize3 >().ptr(),
					0, null,
					null ) );
		
		CL2ComputeUtils::ReleaseObjects( cmd_queue, this, src );
		return true;
	}
	
/*
=================================================
	Copy
=================================================
*/
	bool CL2ComputeImage::Copy (const ComputeImagePtr &src, const uint4 &srcOffset, const uint4 &dstOffset, const uint4 &size)
	{
		using namespace cl;
		
		CHECK_ERR( IsCreated() );
		CHECK_ERR( src.IsNotNull() and src->IsCreated() );
		CHECK_ERR( All( srcOffset + size <= src->Dimension() ) );
		CHECK_ERR( All( dstOffset + size <= this->Dimension() ) );
		CHECK_ERR( EPixelFormat::BitPerPixel( PixelFormat() ) == EPixelFormat::BitPerPixel( src->PixelFormat() ) );

		uint3	src_offset	= Texture::Utils::ConvertOffset( src->ImageType(), srcOffset );
		uint3	dst_offset	= Texture::Utils::ConvertOffset( this->ImageType(), dstOffset );
		uint3	src_size	= Texture::Utils::ConvertSize( src->ImageType(), size );
		uint3	dst_size	= Texture::Utils::ConvertSize( this->ImageType(), size );

		CHECK_ERR( All( src_size == dst_size ) );
		
		cl_command_queue	cmd_queue = SubSystems()->Get< ComputeEngine >()->GetCommandQueue();

		CL2ComputeUtils::AcquireObjects( cmd_queue, this, src );

		CL_CHECK( clEnqueueCopyImage(
					cmd_queue,
					src->_id,
					this->Id(),
					src_offset.To< usize3 >().ptr(),
					dst_offset.To< usize3 >().ptr(),
					dst_size.To< usize3 >().ptr(),
					0, null,
					null ) );

		CL2ComputeUtils::ReleaseObjects( cmd_queue, this, src );
		return true;
	}
		
/*
=================================================
	CopyTo
=================================================
*/
	bool CL2ComputeImage::CopyTo (const ComputeBufferPtr &dst, const uint4 &srcOffset, Bytes<usize> dstOffset, const uint4 &size)
	{
		using namespace cl;
		
		CHECK_ERR( IsCreated() );
		CHECK_ERR( dst.IsNotNull() and dst->IsCreated() );
		CHECK_ERR( All( dstOffset + size <= this->Dimension() ) );
		
		uint3			src_offset	= Texture::Utils::ConvertOffset( this->ImageType(), srcOffset );
		uint3			src_size	= Texture::Utils::ConvertSize( this->ImageType(), size );
		Bytes<usize>	bpp			= Bytes<usize>( EPixelFormat::BitPerPixel( PixelFormat() ) );
		usize			data_size	= src_size.Volume() * bpp;

		CHECK_ERR( data_size <= dst->Size() );
		
		cl_command_queue	cmd_queue = SubSystems()->Get< ComputeEngine >()->GetCommandQueue();

		CL2ComputeUtils::AcquireObjects( cmd_queue, this, dst );

		CL_CHECK( clEnqueueCopyImageToBuffer(
					cmd_queue,
					this->_id,
					dst->Id(),
					src_offset.To< usize3 >().ptr(),
					src_size.To< usize3 >().ptr(),
					dstOffset,
					0, null,
					null ) );
		
		CL2ComputeUtils::ReleaseObjects( cmd_queue, this, dst );
		return true;
	}
	
/*
=================================================
	_FillImage
=================================================
*/
	void CL2ComputeImage::_FillImage (BinaryBuffer pattern)
	{
		using namespace cl;
		
		CHECK_ERR( IsCreated(), void() );
		
		uint3	dst_size	= Texture::Utils::ConvertSize( ImageType(), Dimension() );

		cl_command_queue	cmd_queue = SubSystems()->Get< ComputeEngine >()->GetCommandQueue();

		CL2ComputeUtils::AcquireObjects( cmd_queue, this );

		CL_CALL( clEnqueueFillImage(
					cmd_queue,
					_id,
					pattern.ptr(),
					usize3(0).ptr(),
					dst_size.To<usize3>().ptr(),
					0, null, 
					null ) );
		
		CL2ComputeUtils::ReleaseObjects( cmd_queue, this );
	}

/*
=================================================
	New
=================================================
*/
	ComputeImagePtr  CL2ComputeImage::New (const SubSystemsRef ss)
	{
		return BaseObject::_New( new CL2ComputeImage( ss ) );
	}
	
/*
=================================================
	New
=================================================
*/
	ComputeImagePtr  CL2ComputeImage::New (const TexturePtr &texture, EMemoryAccess::type flags, MipmapLevel level)
	{
		ComputeImagePtr	p = CL2ComputeImage::New( texture->SubSystems() );

		CHECK_ERR( p->Create( texture, flags, level ), null );
		return p;
	}
	
/*
=================================================
	_ConvertSize
=================================================
*
	uint3 CL2ComputeImage::_ConvertSize (ETexture::type imgType, const uint4 &size)
	{
		return Max( _ConvertOffset( imgType, size ), uint3(1) );
	}*/


}	// Compute
}	// Engine

#endif	// COMPUTE_API_OPENCL
