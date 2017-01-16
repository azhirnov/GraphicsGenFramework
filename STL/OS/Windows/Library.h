// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "OSWindows.h"
#include "FileSystem.h"

#ifdef PLATFORM_WINDOWS_SHELL

namespace GX_STL
{
namespace _platform_
{
	using namespace winapi;
	

	//
	// Library
	//

	struct Library
	{
	private:
		HMODULE		_library;
		String		_name;
		bool		_freeWhenDelete;

	public:
		Library (): _library(null), _freeWhenDelete(true)
		{}

		explicit
		Library (StringCRef name, bool canFree = true):
			_library(null), _freeWhenDelete(true)
		{
			Load( name, canFree );
		}

		~Library ()
		{
			if ( _freeWhenDelete )
				Unload();
		}
		
		void * Handle () const
		{
			return _library;
		}
		
		const String & GetName () const
		{
			return _name;
		}
		
		bool IsValid () const
		{
			return _library != null;
		}

		bool FindAndLoad (StringCRef name, uint searchDepth, bool canFree = true)
		{
			String	fname;

			if ( not FileSystem::SearchFile( name, searchDepth, fname ) )
				return false;

			return Load( fname, canFree );
		}

		bool Load (StringCRef name, bool canFree = true)
		{
			if ( name == null )
				return false;

			Unload();

			_library		= LoadLibrary( name.cstr() );
			_name			= name;
			_freeWhenDelete	= canFree;
			
			return IsValid();
		}

		bool Load (HMODULE lib, bool canFree = false)
		{
			Unload();

			char	a_buf[MAX_PATH] = {0};
			
			_freeWhenDelete	 = canFree;
			_library		 = lib;
			uint	size	 = GetModuleFileName( _library, a_buf, (DWORD) CountOf(a_buf) );

			if ( size == 0 ) {
				_library = null;
				return false;
			}

			_name.Copy( StringCRef( a_buf, size ) );
			return IsValid();
		}

		bool LoadSelf ()
		{
			return Load( GetModuleHandle( (LPCSTR)null ) );
		}

		bool Unload ()
		{
			bool	ret = true;
			
			_name.Clear();

			if ( IsValid() ) {
				ret		 = ( FreeLibrary( _library ) == TRUE );
				_library = null;
			}
			return ret;
		}

		template <typename T>
		bool GetProc (T *&proc, StringCRef procName) const
		{
			ASSERT( IsValid() );
			ASSERT( not procName.Empty() );

			T * tmp = PointerCast< T >( GetProcAddress( _library, procName.cstr() ) );

			if ( tmp != null )
			{
				proc = tmp;
				return true;
			}
			return false;
		}

		void * GetProc (StringCRef procName, void *defProc = null) const
		{
			ASSERT( IsValid() );
			ASSERT( not procName.Empty() );

			void * tmp = GetProcAddress( _library, procName.cstr() );
			return tmp != null ? tmp : defProc;
		}
	};

	
}	// _platform_
}	// GX_STL

#endif	// PLATFORM_WINDOWS_SHELL