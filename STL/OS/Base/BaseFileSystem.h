// Copyright © 2014-2016  Zhirnov Andrey. All rights reserved.

#pragma once

#include "Common.h"
#include "../../Types/StringUtils.h"
#include "../../Defines/Errors.h"

namespace GX_STL
{
namespace _platform_
{
	
	//
	// Base File System
	//

	template <typename T>
	struct BaseFileSystem : T
	{
	// types
	public:
		typedef T	base_t;


	// methods
	public:
		static bool SearchFile (StringCRef file, GXTypes::uint depth, OUT String &result);
		static bool SearchFileForward (StringCRef file, GXTypes::uint depth, OUT String &result);
		static bool SearchFileBackward (StringCRef file, GXTypes::uint depth, OUT String &result);

		static bool SearchDir (StringCRef dir, GXTypes::uint depth, OUT String &result);
		static bool SearchDirForward (StringCRef dir, GXTypes::uint depth, OUT String &result);
		static bool SearchDirBackward (StringCRef dir, GXTypes::uint depth, OUT String &result);
		
		static bool CreateDirectories (StringCRef path);	// create directories for path
		static bool FindAndSetCurrentDir (StringCRef dirname, uint searchDepth);

	private:
		template <typename B>
		static bool _Search (StringCRef name, GXTypes::uint depth, OUT String &result);
		
		template <typename B>
		static bool _SearchForward (StringCRef dir, StringCRef name, GXTypes::uint depth, OUT String &result);
		
		template <typename B>
		static bool _SearchBackward (StringCRef name, GXTypes::uint depth, OUT String &result);


	// types
	private:
		struct TFileSearch {
			static bool IsExist (StringCRef name) {
				return T::IsFileExist( name );
			}
		};

		struct TDirSearch {
			static bool IsExist (StringCRef name) {
				return T::IsDirectoryExist( name );
			}
		};
	};
	


/*
=================================================
	
=================================================
*/
	template <typename T>
	inline bool BaseFileSystem<T>::SearchFile (StringCRef file, GXTypes::uint depth, OUT String &result)
	{
		return _Search< TFileSearch >( file, depth, result );
	}

/*
=================================================
	
=================================================
*/
	template <typename T>
	inline bool BaseFileSystem<T>::SearchFileForward (StringCRef file, GXTypes::uint depth, OUT String &result)
	{
		return _SearchForward< TFileSearch >( "", file, depth, result );
	}
	
/*
=================================================
	
=================================================
*/
	template <typename T>
	inline bool BaseFileSystem<T>::SearchFileBackward (StringCRef file, GXTypes::uint depth, OUT String &result)
	{
		return _SearchBackward< TFileSearch >( file, depth, result );
	}
	
/*
=================================================
	
=================================================
*/
	template <typename T>
	inline bool BaseFileSystem<T>::SearchDir (StringCRef dir, GXTypes::uint depth, OUT String &result)
	{
		return _Search< TDirSearch >( dir, depth, result );
	}

/*
=================================================
	
=================================================
*/
	template <typename T>
	inline bool BaseFileSystem<T>::SearchDirForward (StringCRef dir, GXTypes::uint depth, OUT String &result)
	{
		return _SearchForward< TDirSearch >( dir.cstr(), dir, depth, result );
	}
	
/*
=================================================
	
=================================================
*/
	template <typename T>
	inline bool BaseFileSystem<T>::SearchDirBackward (StringCRef dir, GXTypes::uint depth, OUT String &result)
	{
		return _SearchBackward< TDirSearch >( dir, depth, result );
	}
	
/*
=================================================
	
=================================================
*/
	template <typename T>
	template <typename B>
	inline bool BaseFileSystem<T>::_Search (StringCRef name, GXTypes::uint depth, OUT String &result)
	{
		String	tmp;
		String	fname;	  

		if ( _SearchForward<B>( "", name, depth, result ) )
			return true;

		for (GXTypes::uint i = 0; i < depth; ++i)
		{
			FileAddressUtils::BuildPath( fname, tmp, name );

			if ( B::IsExist( fname ) ) {
				result = RVREF( fname );
				return true;
			}
			
			"../" >> tmp;

			if ( _SearchForward<B>( tmp, name, depth-i, result ) )
				return true;
		}
		return false;
	}
		
/*
=================================================
	
=================================================
*/
	template <typename T>
	template <typename B>
	inline bool BaseFileSystem<T>::_SearchBackward (StringCRef name, GXTypes::uint depth, OUT String &result)
	{
		result = name;
		
		for (GXTypes::uint i = 0; i < depth; ++i)
		{
			if ( B::IsExist( result ) ) {
				return true;
			}
			"../" >> result;
		}
		result.Clear();
		return false;
	}
	
/*
=================================================
	
=================================================
*/
	template <typename T>
	bool BaseFileSystem<T>::CreateDirectories (StringCRef path)
	{
		if ( path.Empty() )
			return true;

		if ( base_t::IsDirectoryExist( path ) )
			return true;

		String	tmp		= path;
		int		depth	= 0;

		while ( FileAddressUtils::PathMoveBack( tmp ) )
		{
			if ( base_t::IsDirectoryExist( tmp ) )
				break;

			++depth;
		}

		while ( depth >= 0 )
		{
			tmp = path;
			CHECK( FileAddressUtils::RemoveDirectoriesFromRight( tmp, depth-- ) );
			CHECK_ERR( base_t::NewDirectory( tmp ) );
		}
		
		return base_t::IsDirectoryExist( path );
	}
	
/*
=================================================
	
=================================================
*/
	template <typename T>
	bool BaseFileSystem<T>::FindAndSetCurrentDir (StringCRef dirname, uint searchDepth)
	{
		String	dir;
		CHECK_ERR( SearchDirBackward( dirname, searchDepth, dir ) or SearchDir( dirname, searchDepth, dir ) );
		
		CHECK_ERR( base_t::SetCurrentDirectory( dir.cstr() ) );
		return true;
	}

//-------------------------------------------------------------------

}	// _platform_
}	// GX_STL
