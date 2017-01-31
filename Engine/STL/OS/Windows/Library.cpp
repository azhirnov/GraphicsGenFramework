// Copyright © 2014-2017  Zhirnov Andrey. All rights reserved.

#include "Library.h"

#ifdef PLATFORM_WINDOWS_SHELL

#include "Engine/STL/OS/Base/BaseFileSystem.h"
#include "WinHeader.h"

namespace GX_STL
{
namespace OS
{
	
/*
=================================================
	constructor
=================================================
*/
	Library::Library () :
		_library( UninitializedT<HMODULE>() ),
		_freeWhenDelete(true)
	{}
	
/*
=================================================
	constructor
=================================================
*/
	Library::Library (StringCRef name, bool canFree):
		_library( UninitializedT<HMODULE>() ),
		_freeWhenDelete(true)
	{
		Load( name, canFree );
	}
	
/*
=================================================
	destructor
=================================================
*/
	Library::~Library ()
	{
		if ( _freeWhenDelete )
			Unload();
	}
		
/*
=================================================
	IsValid
=================================================
*/
	bool Library::IsValid () const
	{
		return _library.IsNotNull<HMODULE>();
	}
	
/*
=================================================
	FindAndLoad
=================================================
*/
	bool Library::FindAndLoad (StringCRef name, uint searchDepth, bool canFree)
	{
		String	fname;

		if ( not FileSystem::SearchFile( name, searchDepth, fname ) )
			return false;

		return Load( fname, canFree );
	}
	
/*
=================================================
	Load
=================================================
*/
	bool Library::Load (StringCRef name, bool canFree)
	{
		if ( name == null )
			return false;

		Unload();

		_library		= ::LoadLibraryA( name.cstr() );
		_name			= name;
		_freeWhenDelete	= canFree;
			
		return IsValid();
	}
	
/*
=================================================
	Load
=================================================
*/
	bool Library::LoadFromHandle (const Handle_t &lib, bool canFree)
	{
		Unload();

		char	a_buf[MAX_PATH] = {0};
			
		_freeWhenDelete	 = canFree;
		_library		 = lib;
		uint	size	 = GetModuleFileName( _library.Get<HMODULE>(), a_buf, (DWORD) CountOf(a_buf) );

		if ( size == 0 ) {
			_library = null;
			return false;
		}

		_name.Copy( StringCRef( a_buf, size ) );
		return IsValid();
	}
	
/*
=================================================
	LoadSelf
=================================================
*/
	bool Library::LoadSelf ()
	{
		return LoadFromHandle( Handle_t( ::GetModuleHandle( (LPCSTR)null ) ) );
	}
	
/*
=================================================
	Unload
=================================================
*/
	bool Library::Unload ()
	{
		bool	ret = true;
			
		_name.Clear();

		if ( IsValid() ) {
			ret		 = ( FreeLibrary( _library.Get<HMODULE>() ) == TRUE );
			_library = null;
		}
		return ret;
	}
	
/*
=================================================
	GetProc
=================================================
*/
	void * Library::GetProc (StringCRef procName, void *defProc) const
	{
		ASSERT( IsValid() );
		ASSERT( not procName.Empty() );

		void * tmp = GetProcAddress( _library.Get<HMODULE>(), procName.cstr() );
		return tmp != null ? tmp : defProc;
	}

	
}	// OS
}	// GX_STL

#endif	// PLATFORM_WINDOWS_SHELL
