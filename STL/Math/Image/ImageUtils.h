// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "../Color/ColorFormats.h"

namespace GX_STL
{
namespace GXMath
{
	

/*
=================================================
	AlignImage
=================================================
*/
	template <typename T, ubyte I, typename B>
	inline Vec<T,I> AlignImage (const Vec<T,I> &size, const B& align)
	{
		CompileTime::MustBeInteger<T>();
		ASSERT( IsPowerOfTwo( align ) and "align must be a power of 2" );

		Vec<T,I>	res;
		uint		align_po2 = IntLog2( align );

		FOR( i, res ) {
			res[i] = ( size[i] + (align-1) ) >> align_po2;
		}
		return res;
	}
	
/*
=================================================
	GetNumberOfMipmaps
----
	returns the number of mipmaps for current dimension,
	to calculate full number of images add 1 (top level)
=================================================
*/
	template <typename T>
	inline uint GetNumberOfMipmaps (const T len)
	{
		return IntLog2( len );
	}

/*
=================================================
	GetTextureFrame
=================================================
*/
	template <typename T, typename B>
	inline void GetTextureFrame (OUT Rectangle<T> &sTexcoord, uint uFrameIdx, const Vec<B,2> &vDim,
								 const Vec<B,2> &vNumFrames, const Vec<B,2> &border = Vec<B,2>())
	{
		CompileTime::MustBeInteger<B>();
		ASSERT( uFrameIdx < vNumFrames.Area() );

		const Vec<B,2>		v_frame_size	= vDim / vNumFrames;
		const Vec<B,2>		v_frame_pos		= Vec<B,2>( uFrameIdx % vNumFrames.x, uFrameIdx / vNumFrames.x );
		Rectangle<B>			s_frame_coord	= Rectangle<B>( Vec<B,2>(0), v_frame_size ) + (v_frame_pos * v_frame_size);

		s_frame_coord.LeftBottom() += border;
		s_frame_coord.RightTop()   -= border;

		sTexcoord = s_frame_coord.template To< Rectangle<T> >() / vDim.template To< Vec<T,2> >();
	}

/*
=================================================
	AlignedImageDataSize
=================================================
*/
	template <typename T>
	inline Bytes<usize> AlignedImageDataSize (const Vec<T,3> &dimension, Bytes<usize> bytePerPixel,
												Bytes<usize> rowAlign = Bytes<usize>(1),
												Bytes<usize> sliceAlign = Bytes<usize>(1))
	{
		CompileTime::MustBeInteger<T>();
		const usize3	dim			= Max( dimension, Vec<T,3>(1) ).template To<usize3>();
		const usize		row_size	= (usize) AlignedImageRowSize( dim.x, bytePerPixel, rowAlign );
		const usize		slice_size	= (usize) AlignedImageRowSize( dim.y * row_size, Bytes<usize>(1), sliceAlign );
		const usize		size		= slice_size * dim.z;
		return Bytes<usize>( size );
	}

	template <typename ColorType, typename T>
	inline Bytes<usize> AlignedImageDataSize (const Vec<T,3> &dimension, Bytes<usize> rowAlign, Bytes<usize> sliceAlign)
	{
		return AlignedImageDataSize( dimension, Bytes<usize>::SizeOf<ColorType>(), rowAlign, sliceAlign );
	}
	
/*
=================================================
	AlignedImageRowSize
=================================================
*/
	template <typename T>
	inline Bytes<usize> AlignedImageRowSize (const T rowSize, Bytes<usize> bytePerPixel, Bytes<usize> rowAlign)
	{
		CompileTime::MustBeInteger<T>();
		return (Bytes<usize>) Align( T( Max( rowSize, T(1) ) * bytePerPixel ), rowAlign );
	}
	
/*
=================================================
	AlignedImageSliceSize
=================================================
*/
	template <typename T>
	inline Bytes<usize> AlignedImageSliceSize (const Vec<T,2> dim, Bytes<usize> bytePerPixel,
												Bytes<usize> rowAlign, Bytes<usize> sliceAlign)
	{
		const usize	row_size	= (usize) AlignedImageRowSize( dim.x, bytePerPixel, rowAlign );
		const usize slice_size	= (usize) AlignedImageRowSize( dim.y * row_size, Bytes<usize>(1), sliceAlign );
		return Bytes<usize>( slice_size );
	}

/*
=================================================
	GetPixelOffset
=================================================
*/
	template <typename T>
	inline usize GetPixelOffset (const Vec<T,3> &coord, const Vec<T,3> &dimension, Bytes<usize> bytePerPixel,
								 Bytes<usize> rowAlign = Bytes<usize>(1),
								 Bytes<usize> sliceAlign = Bytes<usize>(1))
	{
		CompileTime::MustBeInteger<T>();
		const usize3	dim			= Max( dimension, Vec<T,3>(1) ).template To<usize3>();
		const usize		row_size	= (usize) AlignedImageRowSize( dim.x, bytePerPixel, rowAlign );
		const usize		slice_size	= (usize) AlignedImageRowSize( dim.y * row_size, Bytes<usize>(1), sliceAlign );
		const usize		z_off		= slice_size * coord.z;
		const usize		y_off		= z_off + row_size * coord.y;
		const usize		i			= y_off + bytePerPixel * coord.x;
		return i;
	}



	//
	// Image Utils
	//
	
	struct ImageUtils : public Noninstancable
	{
		template <typename T, typename SrcType, typename DstType>
		static bool Copy (const Vec<T,3> &dimension, const SrcType *srcImage, DstType *dstImage,
						  Bytes<usize> srcRowAlignInBytes = Bytes<usize>(1),
						  Bytes<usize> dstRowAlignInBytes = Bytes<usize>(1))
		{
			return CopyPart( dimension, Vec<T,3>(0), srcImage, dimension, Vec<T,3>(0), dstImage, srcRowAlignInBytes, dstRowAlignInBytes );
		}


		template <typename T, typename SrcType, typename DstType>
		static bool CopyPart (const Vec<T,3> &srcDim, const Vec<T,3> &srcOff, const SrcType *srcImage,
							  const Vec<T,3> &dstDim, const Vec<T,3> &dstOff, DstType *dstImage,
							  Bytes<usize> srcRowAlignInBytes = Bytes<usize>(1),
							  Bytes<usize> dstRowAlignInBytes = Bytes<usize>(1))
		{
			CHECK_ERR( srcImage != null and dstImage != null );

			const uint3		src_dim	= Max( srcDim, Vec<T,3>(1) ).template To<uint3>();
			const uint3		src_off = srcOff.template To<uint3>();
			const uint3		dst_dim	= Max( dstDim, Vec<T,3>(1) ).template To<uint3>();
			const uint3		dst_off	= dstOff.template To<uint3>();
			const usize		src_row	= (usize) AlignedImageRowSize( src_dim.x, Bytes<usize>::SizeOf<SrcType>(), srcRowAlignInBytes );
			const usize		dst_row	= (usize) AlignedImageRowSize( dst_dim.x, Bytes<usize>::SizeOf<DstType>(), dstRowAlignInBytes );
			const void *	src		= (const void *) srcImage;
			void *			dst		= (void *) dstImage;

			const uint3		src_size = src_dim - src_off;
			const uint3		dst_size = dst_dim - dst_off;
			
			//CHECK_ERR( All( src_size == dst_size ) );
			CHECK_ERR( All( src_off <= src_dim ) );
			CHECK_ERR( All( dst_off <= dst_dim ) );

			for (uint z = 0; z < dst_size.z and z < src_size.z; ++z)
			{
				const usize	src_z_off = src_row * src_dim.y * (z + src_off.z);
				const usize	dst_z_off = dst_row * dst_dim.y * (z + dst_off.z);

				for (uint y = 0; y < dst_size.y and y < src_size.y; ++y)
				{
					const usize	src_y_off = src_z_off + src_row * (y + src_off.y);
					const usize	dst_y_off = dst_z_off + dst_row * (y + dst_off.y);

					for (uint x = 0; x < dst_size.x and x < src_size.x; ++x)
					{
						const usize	i = src_y_off + sizeof(SrcType) * (x + src_off.x);
						const usize	j = dst_y_off + sizeof(DstType) * (x + dst_off.x);

						const SrcType *	s = MovePointer< SrcType >( src, i );
						DstType *		d = MovePointer< DstType >( dst, j );
						
						ColorFormat::ColorFormatConverter::Convert( *d, *s );
					}
				}
			}
			return true;
		}


		template <typename T, typename SrcType, typename DstType>
		static bool Fill (const Vec<T,3> &regionDim, const SrcType &color,
						  const Vec<T,3> &dstOff, const Vec<T,3> &dstDim, DstType *dstImage,
						  Bytes<usize> dstRowAlignInBytes = Bytes<usize>(1))
		{
			CHECK_ERR( dstImage != null );

			const uint3		src_dim	= Max( regionDim, Vec<T,3>(1) ).template To<uint3>();
			const uint3		dst_dim	= Max( dstDim, Vec<T,3>(1) ).template To<uint3>();
			const uint3		dst_off	= dstOff.template To<uint3>();
			const usize		dst_row	= AlignedImageRowSize( dst_dim.x, Bytes<usize>::SizeOf<DstType>(), dstRowAlignInBytes );
			void *			dst		= (void *) dstImage;
			
			CHECK_ERR( All( src_dim <= dst_dim - dst_off ) );

			for (uint z = 0; z < src_dim.z; ++z)
			{
				const usize	dst_z_off = dst_row * dst_dim.y * (z + dst_off.z);

				for (uint y = 0; y < src_dim.y; ++y)
				{
					const usize	dst_y_off = dst_z_off + dst_row * (y + dst_off.y);

					for (uint x = 0; x < src_dim.x; ++x)
					{
						const usize	j = dst_y_off + sizeof(DstType) * (x + dst_off.x);
						DstType *	d = MovePointer< DstType >( dst, j );
						
						ColorFormat::ColorFormatConverter::Convert( *d, color );
					}
				}
			}
			return true;
		}


		template <typename Iterator, typename T, typename ColorType>
		static bool ForEach (Iterator &iter, const Vec<T,3> &regionDim, ColorType *image,
							 const Vec<T,3> &offset, const Vec<T,3> &dim,
							 Bytes<usize> dstRowAlignInBytes = Bytes<usize>(1))
		{
			CHECK_ERR( image != null );

			const uint3		src_dim	= Max( regionDim, Vec<T,3>(1) ).template To<uint3>();
			const uint3		dst_dim	= Max( dim, Vec<T,3>(1) ).template To<uint3>();
			const uint3		dst_off	= offset.template To<uint3>();
			const usize		dst_row	= AlignedImageRowSize( dst_dim.x, Bytes<usize>::SizeOf<ColorType>(), dstRowAlignInBytes );
			void *			dst		= (void *) image;
			
			CHECK_ERR( All( src_dim <= dst_dim - dst_off ) );

			for (uint z = 0; z < src_dim.z; ++z)
			{
				const usize	dst_z_off = dst_row * dst_dim.y * (z + dst_off.z);

				for (uint y = 0; y < src_dim.y; ++y)
				{
					const usize	dst_y_off = dst_z_off + dst_row * (y + dst_off.y);

					for (uint x = 0; x < src_dim.x; ++x)
					{
						const usize	j = dst_y_off + sizeof(ColorType) * (x + dst_off.x);
						ColorType *	d = MovePointer< ColorType >( dst, j );
						
						iter( *d, uint3(x,y,z) );
					}
				}
			}
			return true;
		}


		template <typename T, typename ColorType>
		static ColorType & GetPixel (const Vec<T,3> &imageDim, ColorType *image, const Vec<T,3> &pixelCoord,
									 uint rowAlignInBytes = 1)
		{
			ASSUME( image != null );
			
			const uint3		dim		= Max( imageDim, Vec<T,3>(1) ).template To<uint3>();
			const uint3		coord	= pixelCoord.template To<uint3>();
			const usize		row		= AlignedImageRowSize( dim.x, Bytes<usize>::SizeOf<ColorType>(), rowAlignInBytes );
			const usize		i		= row * (dim.y * coord.z + coord.y) + coord.x * sizeof(ColorType);
			
			ASSERT( All( coord < dim ) );

			return *MovePointer< ColorType *>( image, i );
		}
	};


}	// GXMath
}	// GX_STL
