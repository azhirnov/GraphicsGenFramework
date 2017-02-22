// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "RenderTargetManager.h"

namespace Engine
{
namespace Graphics
{
	
/*
=================================================
	constructor
=================================================
*/
	RenderTargetManager::RenderTargetManager (const SubSystemsRef ss) :
		BaseObject( ss )
	{
		_defaultRT = RenderTarget::NewSystemRT( ss );

		_rtStack.PushBack( _defaultRT );
	}
	
/*
=================================================
	Initialize
=================================================
*/
	bool RenderTargetManager::Initialize ()
	{
		CHECK_ERR( _defaultRT->Create() );

		Platform::WindowDesc	wnd;
		SubSystems()->Get< Platform >()->GetWindowDesc( wnd );

		_defaultRT->SetViewport( wnd.size );
		return true;
	}
	
/*
=================================================
	Deinitialize
=================================================
*/
	void RenderTargetManager::Deinitialize ()
	{
		_rtStack.Clear();
		_defaultRT->Destroy();
	}
	
/*
=================================================
	Push
=================================================
*/
	bool RenderTargetManager::Push (const RenderTargetPtr &rt)
	{
		rt->Bind();
		_rtStack.PushBack( rt );
		return true;
	}
	
/*
=================================================
	Pop
=================================================
*/
	bool RenderTargetManager::Pop (const RenderTargetPtr &rt)
	{
		if ( _rtStack.Count() > 1 and IsCurrent( rt ) )
		{
			rt->Unbind();
			_rtStack.PopBack();

			_rtStack.Back()->Bind();
			return true;
		}
		return false;
	}
	
/*
=================================================
	Remove
=================================================
*/
	bool RenderTargetManager::Remove (const RenderTargetPtr &rt)
	{
		CHECK_ERR( rt != _defaultRT );

		const bool	current = IsCurrent( rt );

		CHECK_ERR( _rtStack.FindAndErase( rt ) );

		if ( current )
		{
			rt->Unbind();
			_rtStack.Back()->Bind();
		}

		return true;
	}
	
/*
=================================================
	IsCurrent
=================================================
*/
	bool RenderTargetManager::IsCurrent (const RenderTargetPtr &rt) const
	{
		return rt == GetCurrent();
	}
	
/*
=================================================
	_OnResize
=================================================
*/
	void RenderTargetManager::_OnResize (const uint2 &newSize)
	{
		_defaultRT->SetViewport( newSize );
	}
	
/*
=================================================
	MakeScreenShot
=================================================
*/
	bool RenderTargetManager::MakeScreenShot (StringCRef filename, ESaveImageFormat::type format, bool makeUnique)
	{
		return SaveRenderTargetImage( GetSystemRenderTarget(), filename, format, makeUnique );
	}
	
/*
=================================================
	SaveRenderTargetImage
=================================================
*/
	bool RenderTargetManager::SaveRenderTargetImage (const RenderTargetPtr &rt, StringCRef filename, ESaveImageFormat::type format, bool makeUnique)
	{
		Ptr< FileManager >	fm = SubSystems()->Get< FileManager >();
		String				name;
		String const		path = FileAddress::GetPath( filename );

		if ( not fm->IsDirectoryExist( path ) )
		{
			fm->CreateDirectories( path );
		}

		if ( makeUnique and fm->IsFileExist( filename ) )
		{
			CHECK_ERR( fm->MakeUniqueName( filename, name ) );

			filename = name;
		}

		switch ( format )
		{
			case ESaveImageFormat::PNG :
			case ESaveImageFormat::PNG_Flipped :
				return _SaveRenderTargetImage_PNG( rt, filename, format == ESaveImageFormat::PNG_Flipped );

			case ESaveImageFormat::RAW :
				return _SaveRenderTargetImage_RAW( rt, filename );
		}

		RETURN_ERR( "unsupported image format!" );
	}


}	// Graphics
}	// Engine

#ifdef GX_USE_LODEPNG
# pragma warning (push)
# pragma warning (disable: 4005)	// 'identifier' : macro redefinition
#  include "External/lodepng/lodepng.h"
# pragma warning (pop)
#endif	// GX_USE_LODEPNG

namespace Engine
{
namespace Graphics
{
	
/*
=================================================
	_SaveRenderTargetImage_PNG
----
	TODO: save asyncroniously
=================================================
*/
	bool RenderTargetManager::_SaveRenderTargetImage_PNG (const RenderTargetPtr &rt, StringCRef filename, bool flipY)
	{
	#ifdef GX_USE_LODEPNG
		const RectU		vp = rt->GetViewport();
		const BytesU	align( 4 );

		BinaryArray		arr;
		arr.Resize( (usize)ImageUtils::AlignedDataSize( uint3( vp.Size(), 1 ), 4_b, align, 1_b ), false );

		BinaryBuffer	data( arr );
		CHECK_ERR( rt->GetImage( OUT data, EPixelFormat::RGBA8_UNorm, vp, ERenderTarget::Color0, align ) );

		if ( flipY )
		{
			uint const		half_height = (vp.Height() - 1) / 2;
			usize const		line_size	= (usize)ImageUtils::AlignedDataSize( int3( vp.Width(), 1, 1 ), 4_b, align, 1_b );

			BinaryArray	line;		line.Resize( line_size, false );

			for (uint y = 0; y < half_height; ++y)
			{
				uint y1 = vp.Height() - 1 - y;

				UnsafeMem::MemCopy( line.ptr(),						arr.ptr() + line_size * y,	 BytesU(line_size) );
				UnsafeMem::MemCopy( arr.ptr() + line_size * y,		arr.ptr() + line_size * y1, BytesU(line_size) );
				UnsafeMem::MemCopy( arr.ptr() + line_size * y1,		line.ptr(),					 BytesU(line_size) );
			}
		}

		uint error = lodepng::encode( filename.cstr(), arr.ptr(), vp.Width(), vp.Height(), LCT_RGBA, 8 );
		CHECK_ERR( error == 0 );

		//const char * errorText = lodepng_error_text(error);
		return true;
	#else

		return false;
	#endif	// GX_USE_LODEPNG
	}
	
/*
=================================================
	_SaveRenderTargetImage_RAW
----
	TODO: save asyncroniously
=================================================
*/
	bool RenderTargetManager::_SaveRenderTargetImage_RAW (const RenderTargetPtr &rt, StringCRef filename)
	{
		const RectU		vp		= rt->GetViewport();
		const BytesU	align	= 4_b;

		BinaryArray		arr;
		arr.Resize( (usize)ImageUtils::AlignedDataSize( uint3( vp.Size(), 1 ), 4_b, align, 1_b ), false );

		BinaryBuffer	data( arr );
		CHECK_ERR( rt->GetImage( OUT data, rt->GetFormat( ERenderTarget::Color0 ), vp, ERenderTarget::Color0, align ) );

		WFilePtr		file;
		CHECK_ERR( SubSystems()->Get< FileManager >()->OpenForWrite( filename, file ) );
		CHECK_ERR( file->Write( data ) );

		return true;
	}


}	// Graphics
}	// Engine